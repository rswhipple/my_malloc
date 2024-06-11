#include "../../inc/main.h"

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

void* search_fast_cache(size_t size) {
  int index = get_fpage_index(size);
  t_tiny_chunk** f_cache = g_pagemap->frontend_cache->fast_cache;
  t_fpage* fpage = NULL;
  t_tiny_chunk* tiny;

  if (f_cache[index]) {
    // allocate top chunk in link list and replace
    tiny = f_cache[index];
    f_cache[index] = f_cache[index]->next;
  }
  else {
    // split off new chunk
    // TODO check logic
    fpage = g_pagemap->span_head->fastpages;
    while (index > 0) {
      fpage = fpage->next;
      index--;
    }
    tiny = create_tiny_chunk(fpage);
  }

  return (void*)MEMORY_SHIFT(tiny, TINY_CHUNK_OVERHEAD);
}

void print_fast_cache(t_tiny_chunk** fast_cache) {
  t_tiny_chunk* temp;
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
    }
    else {
      printf("fast_cache[%i] = NULL\n", i);
    }

  }
}