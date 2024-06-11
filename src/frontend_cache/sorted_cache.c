#include "../../inc/main.h"

t_page* get_page_head(int page_type) {
  t_page* page_head = g_pagemap->span_head->page_head;
  if (page_type == 3) {
    while ((int)page_head->pagetype != page_type) {
      page_head = page_head->next;
    }
    page_head = g_pagemap->span_head->page_head;
  }
  return page_head;
}

t_chunk* get_top_chunk(t_page* page) {
  t_chunk* top_chunk = page->top_chunk;
  while (IS_IN_USE(top_chunk)) {
    top_chunk = top_chunk->fd;
  }

  if (top_chunk == NULL) {
    t_chunk* new_chunk = create_top_chunk(page);
    top_chunk->fd = new_chunk;
    new_chunk->bk = top_chunk;
    top_chunk = new_chunk;
  }

  return top_chunk;
}

void* search_sorted_cache(size_t size, int page_type) {
  // TODO: determine how to get correct bin_size
  char key[32];
  snprintf(key, sizeof(key), "%zu", size);

  cache_table* cache_table = g_pagemap->frontend_cache->cache_table;

  void* ptr = NULL;

  if ((ptr = cache_table_get(cache_table, key)) != NULL) {
    return (void*)MEMORY_SHIFT(ptr, sizeof(t_chunk));
  }

  t_page* page_head = get_page_head(page_type);

  t_chunk* top_chunk = get_top_chunk(page_head);

  if ((cache_table_set(cache_table, key, top_chunk)) != NULL) {
    SET_IN_USE(top_chunk);
    return (void*)MEMORY_SHIFT(cache_table_get(cache_table, key), sizeof(t_chunk));
  }

  printf("Returning NULL\n");
  return NULL;
}