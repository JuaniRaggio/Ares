/**
 * @file multi_region_heap.c
 * @brief Multi-region first-fit heap allocator with coalescing.
 *
 * Inspired by FreeRTOS heap_5. Manages multiple non-contiguous memory
 * regions through a single address-ordered free list with sentinel nodes.
 */

#include <multi_region_heap.h>

/**
 * @brief Free list node.
 *
 * Each free block has a header containing a pointer to the next free
 * block and the total block size (including this header).
 */
typedef struct block_list {
        struct block_list *next_free_block; /**< Next block in the free list. */
        size_t block_size; /**< Total block size including header. */
} block_list_t;

/** @brief Sentinel head of the free list (size=0). */
static block_list_t free_list_start;

/** @brief Sentinel tail of the free list (size=0, next=NULL). */
static block_list_t free_list_end;

/** @brief Heap state: incrementally maintained statistics. */
static heap_stats_t heap_status;

/** @brief Total heap size across all regions (set at init, never changes). */
static size_t total_heap_size;

/** @brief sizeof(block_list_t) aligned up to HEAP_ALIGNMENT. */
static size_t header_size;

static int heap_initialized;
static inline size_t align_up(size_t val) {
        return (val + (HEAP_ALIGNMENT - 1)) & ~((size_t)(HEAP_ALIGNMENT - 1));
}

static void insert_block_into_free_list(block_list_t *block_to_insert) {
void mem_init(heap_region_t *regions, size_t region_count) {
}
void *mem_alloc(size_t size) {
}

void mem_get_stats(heap_stats_t *stats) {
}
