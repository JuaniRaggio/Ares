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
