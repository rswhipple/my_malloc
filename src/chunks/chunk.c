#include "../../inc/main.h" 


void chunk_write_boundary_tag(t_chunk* chunk) {
    size_t* boundary_tag = (size_t*)((char*)chunk + CHUNK_SIZE(chunk) - sizeof(size_t));
    *boundary_tag = chunk->size;
}

t_chunk* chunk_top_create(t_page* page) {
    t_chunk* chunk = (t_chunk*)page->memory;
    chunk->size = page->memory_size;
    SET_FREE(chunk);
    chunk_write_boundary_tag(chunk);
    chunk->fd = NULL;
    chunk->bk = NULL;

    return chunk;
}

// input parameters are t_chunk *chunk and size_t chunk_size
t_chunk* chunk_split(t_chunk* chunk, size_t size) {
    if (CHUNK_SIZE(chunk) <= (size + 72)) {
        fprintf(stderr, "Invalid split size: %zu (chunk size: %zu)\n", size, chunk->size);
        custom_exit("Error in chunk_split()\n");
    }

    // Placeholder variables
    t_chunk* temp = chunk->fd;
    size_t initial_chunk_size = chunk->size;

    int flag = 0;

    if (g_pagemap->top_chunk == chunk) flag = 1;
    else if (g_pagemap->last_chunk == chunk) flag = 2;

    // Update the original chunk's size, free status, next pointer & boundary_tag
    t_chunk* first_chunk = chunk;
    first_chunk->size = size;
    chunk_write_boundary_tag(chunk);
    SET_IN_USE(first_chunk);    // set in_use after writing boundary tag 
    first_chunk->fd = NULL;
    first_chunk->bk = NULL;

    // Create the second chunk immediately after the first chunk
    t_chunk* second_chunk = (t_chunk*)MEMORY_SHIFT(CHUNK_SHIFT(chunk), size);
    second_chunk->size = initial_chunk_size - size;
    SET_FREE(second_chunk);
    second_chunk->bk = chunk;
    second_chunk->fd = temp;
    chunk_write_boundary_tag(second_chunk);

    if (flag == 1) g_pagemap->top_chunk = second_chunk;
    else if (flag == 2) g_pagemap->last_chunk = second_chunk;
    else {
        second_chunk->fd = g_pagemap->frontend_cache->unsorted_cache;
        g_pagemap->frontend_cache->unsorted_cache = second_chunk;
    }
        
    return first_chunk;
}


t_chunk* huge_chunk_allocate(size_t size) {
    if (size < (size_t)PAGE_SIZE) {
        log_error("Error: allocation size too small.\n");
        return NULL;
    }

    // Allocate a huge chunk
    t_chunk* huge_chunk = (t_chunk*)mmap(NULL, size, PROT_READ | 
                PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (huge_chunk == MAP_FAILED) custom_exit("mmap");
    huge_chunk->size = size;
    return huge_chunk;
}

t_chunk* chunk_merge(t_chunk* value_1, t_chunk* value_2) {
    // update cache_table bin head
    if (cache_table_is_bin_head(value_1)) cache_table_remove_head(value_1);
    if (cache_table_is_bin_head(value_2)) cache_table_remove_head(value_2);

    // remove t_chunks from cache_table
    if (value_1->bk) value_1->bk->fd = value_1->fd;
    if (value_1->fd) value_1->fd->bk = value_1->bk;
    if (value_2->bk) value_2->bk->fd = value_2->fd;
    if (value_2->fd) value_2->fd->bk = value_2->bk;

    value_1->size += value_2->size;
    chunk_write_boundary_tag(value_1);

    return value_1;
}

t_chunk* try_merge(t_chunk* value) {
    // coalescing algo
    // find out if chunk is first chunk in page or rearrange pageheaders
    int flag = 0;
    t_chunk* prev = PREV_CHUNK(value, PREV_SIZE(value));
    if (prev) {
        printf("prev->size: %zu\n", prev->size);
        printf("prev pointer: %p\n", prev);
        if (!IS_IN_USE(prev)) flag += 1;
    }
    t_chunk* next = NEXT_CHUNK(value);
    if (next) {
        if (!IS_IN_USE(next)) flag += 2;
        printf("next->size: %zu\n", next->size);
        printf("next pointer: %p\n", next);
    }


    switch (flag) {
    case 1: return chunk_merge(prev, value);
    case 2: return chunk_merge(value, next);
    case 3: return chunk_merge(chunk_merge(prev, value), next);
    default: break;
    }

    return value;
}

void chunk_free(void* ptr, size_t size) {
    t_chunk* value = (t_chunk*)((char*)ptr - sizeof(size_t));
    value->bk = NULL;
    value->fd = NULL;
    value->size = size;
    SET_FREE(value);

    value = try_merge(value);
    if (g_pagemap->frontend_cache->unsorted_cache == NULL) {
        g_pagemap->frontend_cache->unsorted_cache = value;
        return;
    }
    // add to unsorted_cache
    t_chunk* unsorted_chunk = g_pagemap->frontend_cache->unsorted_cache;
    value->fd = unsorted_chunk;
    g_pagemap->frontend_cache->unsorted_cache = value;
}

void huge_chunk_free(void* ptr, size_t size) {
    if (munmap(ptr, size) == -1) custom_exit("munmap error");
}