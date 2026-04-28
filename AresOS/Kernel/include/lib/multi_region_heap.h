#pragma once

/**
 * @file multi_region_heap.h
 * @brief Multi-region heap memory manager inspired by FreeRTOS heap_5.
 *
 * Provides a first-fit allocator with coalescing over multiple
 * non-contiguous memory regions.
 */

#include <stdint.h>

typedef uint64_t size_t;
typedef int64_t ssize_t;

/** @brief Alignment requirement for all allocations (x86_64). */
#define HEAP_ALIGNMENT 16

/** @brief Number of heap regions to initialize. */
#define HEAP_REGION_COUNT 2

/**
 * @brief Describes a contiguous memory region available for the heap.
 */
typedef struct heap_region {
        uint8_t *initial_address;      /**< Start address of the region. */
        uint64_t region_size_in_bytes; /**< Size of the region in bytes. */
} heap_region_t;

/**
 * @brief Heap statistics for tracking memory usage and detecting leaks.
 */
typedef struct heap_stats {
        size_t available_heap_space_bytes;        //< Sum of all free blk
        size_t size_largest_free_block_bytes;     //< Size of largest free blk
        size_t size_smallest_free_block_in_bytes; //< Size of smallest free blk
        size_t number_of_free_blocks;   //< Number of free blocks in the list
        size_t minimum_ever_free_bytes; //< Minimum free memory since boot
        size_t successful_allocations;  //< Total successful mem_alloc calls
        size_t successful_frees;        //< Total successful mem_free calls
} heap_stats_t;

/**
 * @brief Initialize the heap with the given memory regions.
 * @param regions Array of heap regions to manage.
 * @param region_count Number of regions in the array.
 */
void mem_init(heap_region_t *regions, size_t region_count);

/**
 * @brief Allocate a block of memory using first-fit strategy.
 * @param size Requested size in bytes.
 * @return Pointer to the allocated block, or NULL if no suitable block found.
 */
void *mem_alloc(size_t size);

/**
 * @brief Free a previously allocated block.
 * @param ptr Pointer returned by mem_alloc. NULL is a no-op.
 */
void mem_free(void *ptr);

/**
 * @brief Fill a heap_stats_t structure with current heap state information.
 * @param stats Pointer to the structure to fill.
 */
void mem_get_stats(heap_stats_t *stats);
