// slab.c creado para manejar lista de procesos en wait por semáforos

#include <stddef.h>
#include <slab.h>
#include <memory_manager.h>

static void slab_init(slab_cache_t *cache, size_t size){
    cache->object_size = size;
    cache->free_list = NULL;
    cache->total = 0;
}

slab_cache_t* create_cache(size_t object_size){
    slab_cache_t *new_cache = mem_alloc(sizeof(slab_cache_t));
    if (new_cache != NULL){
        slab_init(new_cache, object_size);
    }
    return new_cache;
}


void* slab_alloc(slab_cache_t *cache){
    if (cache->free_list != NULL) {
        void* obj = cache->free_list;
        cache->free_list = *(void**) obj;
        return obj;
    }
    return mem_alloc(cache->object_size);
}

void slab_free(slab_cache_t *cache, void *obj){
    *(void**) obj = cache->free_list;
    cache->free_list = obj;
    return;
}

static void delete_slabs(void* obj){
    if(obj != NULL){
        delete_slabs(*(void**) obj);
        mem_free(obj);
    }
}

void delete_cache(slab_cache_t *cache){
    if(cache == NULL) return;
    delete_slabs(cache->free_list);
    mem_free(cache);
    return;
}
