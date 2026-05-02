/**
 * @file buddy.c
 * @brief Buddy system memory allocator.
 *
 * Implements the same interface as multi_region_heap.c (mem_init, mem_alloc,
 * mem_free, mem_get_stats) using a binary buddy algorithm with per-order
 * free lists.  Each region becomes an independent buddy pool whose size is
 * rounded down to the nearest power of two.
 */

