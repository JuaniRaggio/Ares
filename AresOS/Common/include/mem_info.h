#pragma once

/**
 * @file mem_info.h
 * @brief Memory manager statistics shared between kernel and userland.
 *
 * Filled by the active memory manager (multi-region heap or buddy system)
 * and exposed to userland through the SYS_MEM_STATS syscall.
 */

#include <stdint.h>

typedef struct heap_stats {
        uint64_t total_heap_size_bytes;            //< Total managed heap size
        uint64_t available_heap_space_bytes;       //< Sum of all free blk
        uint64_t occupied_heap_space_bytes;        //< Total - available
        uint64_t size_largest_free_block_bytes;    //< Size of largest free blk
        uint64_t size_smallest_free_block_in_bytes; //< Size of smallest free blk
        uint64_t number_of_free_blocks;   //< Number of free blocks in the list
        uint64_t minimum_ever_free_bytes; //< Minimum free memory since boot
        uint64_t successful_allocations;  //< Total successful mem_alloc calls
        uint64_t successful_frees;        //< Total successful mem_free calls
} heap_stats_t;
