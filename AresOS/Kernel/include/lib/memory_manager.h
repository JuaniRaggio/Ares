#pragma once

/**
 * @file memory_manager.h
 * @brief Common physical memory manager interface.
 *
 * Both physical memory managers implement this interface. Exactly one is
 * linked at compile time (selected by the Makefile via MM_SRC):
 *   - multi_region_heap.c : first-fit allocator with coalescing (default)
 *   - buddy.c             : binary buddy system  (make buddy)
 *
 * Callers (kernel and processes) only see these four functions and never
 * depend on which implementation is active.
 */

#include <mem_info.h>
#include <stdint.h>

typedef uint64_t size_t;
typedef int64_t ssize_t;

/** @brief Alignment requirement for all allocations (x86_64). */
#define HEAP_ALIGNMENT 16

/** @brief Number of heap regions handed to mem_init. */
#define HEAP_REGION_COUNT 2

/**
 * @brief Describes a contiguous memory region available to the allocator.
 */
typedef struct heap_region {
        uint8_t *initial_address;      /**< Start address of the region. */
        uint64_t region_size_in_bytes; /**< Size of the region in bytes. */
} heap_region_t;

/* heap_stats_t lives in Common/include/mem_info.h so userland can decode
 * the SYS_MEM_STATS syscall output with the same layout. */

/**
 * @brief Initialize the memory manager with the given memory regions.
 * @param regions Array of memory regions to manage.
 * @param region_count Number of regions in the array.
 */
void mem_init(heap_region_t *regions, size_t region_count);

/**
 * @brief Allocate a block of memory.
 * @param size Requested size in bytes.
 * @return Pointer to the allocated block, or NULL if it could not be served.
 */
void *mem_alloc(size_t size);

/**
 * @brief Free a previously allocated block.
 * @param ptr Pointer returned by mem_alloc. NULL is a no-op.
 */
void mem_free(void *ptr);

/**
 * @brief Fill a heap_stats_t structure with current memory state.
 * @param stats Pointer to the structure to fill.
 */
void mem_get_stats(heap_stats_t *stats);
