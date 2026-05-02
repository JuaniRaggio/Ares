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
static heap_stats_t heap_status;
static size_t header_size;
static int buddy_initialized;

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
void mem_init(heap_region_t *regions, size_t region_count) {
        if (buddy_initialized) {
                return;
        }

        header_size = align_up((size_t)sizeof(block_header_t));
        pool_count = 0;

        size_t total_free = 0;

}
void *mem_alloc(size_t size) {
}
void mem_free(void *ptr) {
}
void mem_get_stats(heap_stats_t *stats) {
        if (stats == (void *)0) {
                return;
        }

        size_t largest = 0;
        size_t smallest = ~((size_t)0);
        size_t count = 0;

        for (size_t p = 0; p < pool_count; p++) {
                for (size_t i = 0; i < NUM_ORDERS; i++) {
                }
        }

        if (count == 0) {
                smallest = 0;
        }

        *stats = heap_status;
        stats->size_largest_free_block_bytes = largest;
        stats->size_smallest_free_block_in_bytes = smallest;
        stats->number_of_free_blocks = count;
}
