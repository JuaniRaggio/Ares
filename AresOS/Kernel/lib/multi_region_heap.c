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

/* Insert a block into the free list maintaining ascending address order.
 * Coalesces with adjacent blocks when possible. */
static void insert_block_into_free_list(block_list_t *block_to_insert) {
        block_list_t *iterator;
        uint8_t *block_addr = (uint8_t *)block_to_insert;

        /* Find insertion point: iterate until we find a block at a higher
         * address */
        for (iterator = &free_list_start;
             iterator->next_free_block != &free_list_end &&
             (uint8_t *)iterator->next_free_block < block_addr;
             iterator = iterator->next_free_block) {
        }

        /* Try to coalesce with the next block */
        uint8_t *next_addr = (uint8_t *)iterator->next_free_block;
        if (block_addr + block_to_insert->block_size == next_addr &&
            iterator->next_free_block != &free_list_end) {
                block_to_insert->block_size +=
                    iterator->next_free_block->block_size;
                block_to_insert->next_free_block =
                    iterator->next_free_block->next_free_block;
        } else {
                block_to_insert->next_free_block = iterator->next_free_block;
        }

        /* Try to coalesce with the previous block (iterator) */
        if (iterator != &free_list_start &&
            (uint8_t *)iterator + iterator->block_size == block_addr) {
                iterator->block_size += block_to_insert->block_size;
                iterator->next_free_block = block_to_insert->next_free_block;
        } else {
                iterator->next_free_block = block_to_insert;
        }
}

void mem_init(heap_region_t *regions, size_t region_count) {
        if (heap_initialized) {
                return;
        }

        header_size = align_up((size_t)sizeof(block_list_t));

        block_list_t *first_free = (void *)0;
        block_list_t *prev_free  = (void *)0;
        size_t total_free        = 0;

        for (size_t i = 0; i < region_count; i++) {
                /* Align starting address upwards */
                uint8_t *addr = regions[i].initial_address;
                size_t offset = (size_t)addr & (HEAP_ALIGNMENT - 1);
                if (offset != 0) {
                        size_t adjustment = HEAP_ALIGNMENT - offset;
                        addr += adjustment;
                        if (regions[i].region_size_in_bytes <= adjustment) {
                                continue;
                        }
                        regions[i].region_size_in_bytes -= adjustment;
                }

                /* Ensure usable size is aligned */
                size_t usable = regions[i].region_size_in_bytes &
                                ~((size_t)(HEAP_ALIGNMENT - 1));
                if (usable < header_size + HEAP_ALIGNMENT) {
                        continue;
                }

                /* Create initial free block for this region */
                block_list_t *block    = (block_list_t *)addr;
                block->block_size      = usable;
                block->next_free_block = (void *)0;

                /* Link regions in address order */
                if (first_free == (void *)0) {
                        first_free = block;
                } else {
                        prev_free->next_free_block = block;
                }
                prev_free = block;
                total_free += usable;
        }

        /* Set up sentinels */
        free_list_start.block_size      = 0;
        free_list_start.next_free_block = first_free;
        free_list_end.block_size        = 0;
        free_list_end.next_free_block   = (void *)0;

        if (prev_free != (void *)0) {
                prev_free->next_free_block = &free_list_end;
        } else {
                free_list_start.next_free_block = &free_list_end;
        }

        /* Initialize heap status */
        total_heap_size                               = total_free;
        heap_status.available_heap_space_bytes        = total_free;
        heap_status.minimum_ever_free_bytes           = total_free;
        heap_status.successful_allocations            = 0;
        heap_status.successful_frees                  = 0;
        heap_status.size_largest_free_block_bytes     = 0;
        heap_status.size_smallest_free_block_in_bytes = 0;
        heap_status.number_of_free_blocks             = 0;
        heap_initialized                              = 1;
}

void *mem_alloc(size_t size) {
        if (size == 0) {
                return (void *)0;
        }

        size_t needed = align_up(size + header_size);

        if (needed > heap_status.available_heap_space_bytes) {
                return (void *)0;
        }

        block_list_t *prev  = &free_list_start;
        block_list_t *block = free_list_start.next_free_block;

        while (block != &free_list_end) {
                if (block->block_size >= needed) {
                        if (block->block_size - needed >=
                            header_size + HEAP_ALIGNMENT) {
                                block_list_t *new_block =
                                    (block_list_t *)((uint8_t *)block + needed);
                                new_block->block_size =
                                    block->block_size - needed;
                                new_block->next_free_block =
                                    block->next_free_block;

                                block->block_size     = needed;
                                prev->next_free_block = new_block;
                        } else {
                                prev->next_free_block = block->next_free_block;
                        }

                        block->next_free_block = (void *)0;
                        heap_status.available_heap_space_bytes -=
                            block->block_size;

                        if (heap_status.available_heap_space_bytes <
                            heap_status.minimum_ever_free_bytes) {
                                heap_status.minimum_ever_free_bytes =
                                    heap_status.available_heap_space_bytes;
                        }

                        heap_status.successful_allocations++;

                        return (void *)((uint8_t *)block + header_size);
                }

                prev  = block;
                block = block->next_free_block;
        }

        return (void *)0;
}

void mem_free(void *ptr) {
        if (ptr == (void *)0) {
                return;
        }

        block_list_t *block = (block_list_t *)((uint8_t *)ptr - header_size);

        heap_status.available_heap_space_bytes += block->block_size;
        heap_status.successful_frees++;

        insert_block_into_free_list(block);
}

void mem_get_stats(heap_stats_t *stats) {
        if (stats == (void *)0) {
                return;
        }

        size_t largest  = 0;
        size_t smallest = ~((size_t)0);
        size_t count    = 0;

        block_list_t *block = free_list_start.next_free_block;
        while (block != &free_list_end) {
                count++;
                if (block->block_size > largest) {
                        largest = block->block_size;
                }
                if (block->block_size < smallest) {
                        smallest = block->block_size;
                }
                block = block->next_free_block;
        }

        if (count == 0) {
                smallest = 0;
        }

        *stats = heap_status;

        stats->size_largest_free_block_bytes     = largest;
        stats->size_smallest_free_block_in_bytes = smallest;
        stats->number_of_free_blocks             = count;
}
