
#include "../../inc/main.h"

t_cache* create_frontend_cache(t_pagemap* pagemap) {
    // log_info("creating frontend cache");
    t_cache* cache = (t_cache*)PAGEMAP_SHIFT(pagemap);
    // printf("cache pointer: %p\n", cache);
    // printf("sizeof(t_cache): %zu\n", sizeof(t_cache));
    if (min_chunk_size == 8) cache->fcache_size = 8;
    else cache->fcache_size = 7;
    cache->fast_cache = create_fast_cache(cache);
    cache->cache_table = cache_table_create(cache);
    cache->unsorted_cache = NULL;
    // void* last_byte = (void*)MEMORY_SHIFT(cache, (NUM_BINS * sizeof(t_chunk*) + sizeof(t_cache)));
    // printf("cache end: %p\n", last_byte);

    return cache;
}

t_tiny_chunk** create_fast_cache(t_cache* cache) {
    t_tiny_chunk** fast_cache = (t_tiny_chunk**)CACHE_SHIFT(cache);

    // printf("fast_cache pointer: %p\n", fast_cache);
    for (size_t i = 0; i < cache->fcache_size; ++i) {
        fast_cache[i] = NULL;
    }

    // void* last_byte = (void*)MEMORY_SHIFT(cache, (cache->fcache_size * sizeof(t_tiny_chunk*)));
    // printf("fast_cache end: %p\n", last_byte);
    return fast_cache;
}

void* search_unsorted_cache(size_t size) {
    t_chunk* unsorted_cache = g_pagemap->frontend_cache->unsorted_cache;
    while (unsorted_cache) {
        printf("unsorted_cache->size: %zu\n", unsorted_cache->size);
        if (unsorted_cache->size >= size) {
            return unsorted_cache;
        }
        unsorted_cache = unsorted_cache->fd;
    }

    return NULL;
}

void* search_cache(size_t size, int page_type) {
    void* ptr = NULL;

    if ((ptr = search_unsorted_cache(size)) != NULL) {
        return ptr;
    }
    if (page_type == 2) {
        return search_sorted_cache(size, page_type);
    }
    else if (page_type == 3) {
        return search_unsorted_cache(size);
    }
    printf("Returning NULL\n");
    return NULL;
}
