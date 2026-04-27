#pragma once

#include <stdint.h>

typedef uint64_t size_t;
typedef int64_t ssize_t;

typedef struct heap_region {
        uint8_t
            *initial_address; //>> TODO: Dividimos en bytes o en size of word?
        uint64_t region_size_in_bytes;
} heap_region_t;

// @brief Heap stats will allow us to keep track of memory leaks and any
// unexpected memory issues
typedef struct heap_stats {
        /* sum of all the free blocks */
        size_t available_heap_space_bytes;
        /* The maximum size, in bytes */
        size_t size_largest_free_block_bytes;
        /* The minimum size, in bytes */
        size_t size_smallest_free_block_in_bytes;
        size_t number_of_free_blocks;
        /* The minimum amount of total free memory (sum of all free blocks)
         * there has been in the heap since the system booted. */
        size_t minimum_ever_free_bytes;
        size_t successful_allocations;
        size_t successful_frees;
} heap_stats_t;
