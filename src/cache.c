
#include "../inc/main.h"

void print_fast_cache(t_tiny_chunk** fast_cache) {
    t_tiny_chunk *temp;
    int count;
    int min_align;

    count = (min_chunk_size == 8) ? 8 : 7;
    min_align = (count == 8) ? 1 : 2;

    for (int i = 0; i < count; i++) {
        temp = fast_cache[i];
        if (temp) {
            while (temp) {
                log_info("tiny chunk");
                printf("size = %d\n", (i + min_align) * 8);
                print_tiny_chunk(temp);
                temp = temp->next;
            }
        } else {
            printf("fast_cache[%i] = NULL\n", count);
        }

    }
}

t_cache* create_frontend_cache(t_pagemap* pagemap) {
    log_info("creating frontend cache");
    t_cache* cache = (t_cache*)PAGEMAP_SHIFT(pagemap);
    printf("cache pointer: %p\n", cache);
    if (min_chunk_size == 8) cache->fcache_size = 8; 
    else cache->fcache_size = 7; 
    cache->fast_cache = create_fast_cache(cache);
    cache->sorted_cache = NULL;     // add create_hash_table here or after create pages in base_page?
    cache->unsorted_cache = NULL;
    void *last_byte = (void*)MEMORY_SHIFT(cache, sizeof(t_cache));
    printf("sizeof(t_cache): %zu\n", sizeof(t_cache));
    printf("cache end: %p\n", last_byte);

    return cache;
}

t_tiny_chunk** create_fast_cache(t_cache *cache) {
    t_tiny_chunk** fast_cache = (t_tiny_chunk**)CACHE_SHIFT(cache);

    printf("fast_cache pointer: %p\n", fast_cache);
    for (size_t i = 0; i < cache->fcache_size; ++i) {
        fast_cache[i] = NULL; 
    }

    void *last_byte = (void*)MEMORY_SHIFT(cache, (cache->fcache_size * sizeof(t_tiny_chunk)));
    printf("fast_cache end: %p\n", last_byte);
    return fast_cache;
}

