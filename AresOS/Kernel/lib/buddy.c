/**
 * @file buddy.c
 * @brief Buddy system memory allocator.
 *
 * Implements the same interface as multi_region_heap.c (mem_init, mem_alloc,
 * mem_free, mem_get_stats) using a binary buddy algorithm with per-order
 * free lists.  Each region becomes an independent buddy pool whose size is
 * rounded down to the nearest power of two.
 */

#define MIN_ORDER 5   /* 32 bytes - smallest allocatable block */
#define MAX_ORDER 25  /* 32 MB */
#define NUM_ORDERS (MAX_ORDER - MIN_ORDER + 1)
#define MAX_POOLS HEAP_REGION_COUNT

typedef struct block_header {
        size_t order;
        size_t is_free;
} block_header_t;

typedef struct buddy_pool {
        uint8_t *base;
        size_t total_size;
        size_t max_order;
        block_header_t *free_lists[NUM_ORDERS];
} buddy_pool_t;

static buddy_pool_t pools[MAX_POOLS];
static size_t pool_count;
static heap_stats_t heap_status;
static size_t header_size;
static int buddy_initialized;

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
