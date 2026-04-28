#include <multi_region_heap.h>

heap_region_t heap_regions[] = {};

typedef struct block_list {
        struct block_list *next_free_block;
        size_t size_in_bytes;
} block_list_t;

size_t available_bytes        = 0;
size_t successfull_alocations = 0;
size_t successfull_frees      = 0;
