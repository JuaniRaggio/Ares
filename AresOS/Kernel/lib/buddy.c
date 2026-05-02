/**
 * @file buddy.c
 * @brief Buddy system memory allocator.
 *
 * Implements the same interface as multi_region_heap.c (mem_init, mem_alloc,
 * mem_free, mem_get_stats) using a binary buddy algorithm with per-order
 * free lists.  Each region becomes an independent buddy pool whose size is
 * rounded down to the nearest power of two.
 */

#include <multi_region_heap.h>

#define MIN_ORDER 5   /* 32 bytes - smallest allocatable block */
#define MAX_ORDER 25  /* 32 MB */
#define NUM_ORDERS (MAX_ORDER - MIN_ORDER + 1)
#define MAX_POOLS HEAP_REGION_COUNT

/* Block header stored at the start of every block (free or allocated). */
typedef struct block_header {
        size_t order;
        size_t is_free;
} block_header_t;

/* One buddy pool per memory region. */
typedef struct buddy_pool {
        uint8_t *base;
        size_t total_size;
        size_t max_order;
        block_header_t *free_lists[NUM_ORDERS];
} buddy_pool_t;

static buddy_pool_t pools[MAX_POOLS];
static size_t pool_count      = 0;
static heap_stats_t heap_status;
static size_t header_size     = 0;
static int buddy_initialized = 0;

static inline size_t align_up(size_t val) {
        return (val + (HEAP_ALIGNMENT - 1)) & ~((size_t)(HEAP_ALIGNMENT - 1));
}

static inline size_t log2_of(size_t n) {
        return 63 - __builtin_clzll(n);
}

static inline size_t next_pow2(size_t n) {
        if (n <= 1)
                return 1;
        return (size_t)1 << (64 - __builtin_clzll(n - 1));
}

static inline size_t block_size(block_header_t *blk) {
        return (size_t)1 << blk->order;
}

static inline block_header_t *get_next_free(block_header_t *blk) {
        return *(block_header_t **)((uint8_t *)blk + header_size);
}

static inline void set_next_free(block_header_t *blk, block_header_t *next) {
        *(block_header_t **)((uint8_t *)blk + header_size) = next;
}

static inline block_header_t *get_buddy(buddy_pool_t *pool,
                                       block_header_t *blk) {
        size_t blk_offset   = (size_t)((uint8_t *)blk - pool->base);
        size_t buddy_offset = blk_offset ^ ((size_t)1 << blk->order);
        return (block_header_t *)(pool->base + buddy_offset);
}

static void add_to_free_list(buddy_pool_t *pool, block_header_t *blk) {
        size_t idx = blk->order - MIN_ORDER;
        set_next_free(blk, pool->free_lists[idx]);
        pool->free_lists[idx] = blk;
        blk->is_free          = 1;
}

static void remove_from_free_list(buddy_pool_t *pool, block_header_t *blk) {
        size_t idx           = blk->order - MIN_ORDER;
        block_header_t *cur  = pool->free_lists[idx];
        block_header_t *prev = (void *)0;

        while (cur != (void *)0) {
                if (cur == blk) {
                        if (prev == (void *)0) {
                                pool->free_lists[idx] = get_next_free(cur);
                        } else {
                                set_next_free(prev, get_next_free(cur));
                        }
                        return;
                }
                prev = cur;
                cur  = get_next_free(cur);
        }
}

static void split_block(buddy_pool_t *pool, block_header_t *blk,
                       size_t target_order) {
        while (blk->order > target_order) {
                blk->order--;
                size_t half_size      = (size_t)1 << blk->order;
                block_header_t *buddy = (block_header_t *)((uint8_t *)blk + half_size);
                buddy->order          = blk->order;
                buddy->is_free        = 1;
                add_to_free_list(pool, buddy);
        }
}

static block_header_t *coalesce_block(buddy_pool_t *pool, block_header_t *blk) {
        while (blk->order < pool->max_order) {
                block_header_t *buddy = get_buddy(pool, blk);

                if (!buddy->is_free || buddy->order != blk->order) {
                        break;
                }

                remove_from_free_list(pool, buddy);

                if (buddy < blk) {
                        blk = buddy;
                }
                blk->order++;
        }
        return blk;
}

static buddy_pool_t *find_pool_for_ptr(void *ptr) {
        for (size_t i = 0; i < pool_count; i++) {
                if ((uint8_t *)ptr >= pools[i].base &&
                    (uint8_t *)ptr < pools[i].base + pools[i].total_size) {
                        return &pools[i];
                }
        }
        return (void *)0;
}

static size_t compute_target_order(size_t alloc_size) {
        size_t needed = alloc_size + header_size;
        size_t order  = log2_of(next_pow2(needed));
        if (order < MIN_ORDER)
                return MIN_ORDER;
        return order;
}

static block_header_t *find_free_block(buddy_pool_t *pool, size_t target_order) {
        size_t order = target_order;
        while (order <= pool->max_order) {
                if (pool->free_lists[order - MIN_ORDER] != (void *)0)
                        return pool->free_lists[order - MIN_ORDER];
                order++;
        }
        return (void *)0;
}

static void init_pool(buddy_pool_t *pool, uint8_t *addr, size_t order) {
        pool->base       = addr;
        pool->total_size = (size_t)1 << order;
        pool->max_order  = order;

        for (size_t j = 0; j < NUM_ORDERS; j++)
                pool->free_lists[j] = (void *)0;

        block_header_t *root = (block_header_t *)addr;
        root->order          = order;
        root->is_free        = 1;
        add_to_free_list(pool, root);
}

void mem_init(heap_region_t *regions, size_t region_count) {
        if (buddy_initialized)
                return;

        header_size = align_up((size_t)sizeof(block_header_t));
        pool_count  = 0;

        size_t total_free = 0;

        for (size_t i = 0; i < region_count && pool_count < MAX_POOLS; i++) {
                uint8_t *addr    = (uint8_t *)align_up((size_t)regions[i].initial_address);
                size_t adj       = addr - regions[i].initial_address;
                size_t available = regions[i].region_size_in_bytes;

                if (available <= adj)
                        continue;
                available -= adj;

                /* Find largest power of 2 that fits */
                size_t order = log2_of(available);
                if (order < MIN_ORDER)
                        continue;
                if (order > MAX_ORDER)
                        order = MAX_ORDER;

                init_pool(&pools[pool_count++], addr, order);
                total_free += (size_t)1 << order;
        }

        heap_status.available_heap_space_bytes = total_free;
        heap_status.minimum_ever_free_bytes    = total_free;
        heap_status.successful_allocations     = 0;
        heap_status.successful_frees           = 0;

        buddy_initialized = 1;
}

void *mem_alloc(size_t size) {
        if (size == 0 || !buddy_initialized)
                return (void *)0;

        size_t target_order = compute_target_order(size);
        if (target_order > MAX_ORDER)
                return (void *)0;

        for (size_t p = 0; p < pool_count; p++) {
        }

        return (void *)0;
}

void mem_free(void *ptr) {
        if (ptr == (void *)0 || !buddy_initialized)
                return;

        block_header_t *blk = (block_header_t *)((uint8_t *)ptr - header_size);
        buddy_pool_t *pool  = find_pool_for_ptr(blk);
        if (pool == (void *)0)
                return;

        size_t freed_bytes = block_size(blk);
        blk->is_free       = 1;

        blk = coalesce_block(pool, blk);
        add_to_free_list(pool, blk);

        heap_status.available_heap_space_bytes += freed_bytes;
        heap_status.successful_frees++;
}

void mem_get_stats(heap_stats_t *stats) {
        if (stats == (void *)0)
                return;

        size_t largest  = 0;
        size_t smallest = ~((size_t)0);
        size_t count    = 0;

        for (size_t p = 0; p < pool_count; p++) {
                for (size_t i = 0; i < NUM_ORDERS; i++) {
                        block_header_t *blk = pools[p].free_lists[i];
                        while (blk != (void *)0) {
                                size_t bsize = block_size(blk);
                                count++;
                                if (bsize > largest)
                                        largest = bsize;
                                if (bsize < smallest)
                                        smallest = bsize;
                                blk = get_next_free(blk);
                        }
                }
        }

        *stats                                   = heap_status;
        stats->size_largest_free_block_bytes     = largest;
        stats->size_smallest_free_block_in_bytes = (count == 0) ? 0 : smallest;
        stats->number_of_free_blocks             = count;
}
