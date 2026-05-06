#include <stddef.h>

typedef struct slab_cache{
    size_t object_size;

    void *free_list; //free objects list

    size_t total;
}slab_cache_t;

slab_cache_t* create_cache(size_t object_size);
void* slab_alloc(slab_cache_t *cache);
void slab_free(slab_cache_t *cache, void *obj);
void delete_cache(slab_cache_t *cache);