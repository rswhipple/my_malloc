#include "../inc/main.h"

void create_pages(t_pagemap* pagemap, t_span* span) {
  // TODO: create and assign pages to types (fast, small, large)
  int pages_left = span->num_pages;
  t_page* current = NULL;
  span->page_head = create_base_page(pagemap, span);
  pages_left -= 1;
  current = span->page_head;
  while (pages_left > 0) {
    current = create_page(current, span, fast);
    pages_left -= 1;
  }
}

t_page* create_base_page(t_pagemap* pagemap, t_span* span) {
  UNUSED(pagemap);
  t_page* page = (t_page*)SPAN_SHIFT(span);
  page->chunk_count = 1;
  page->prev = NULL;
  page->next = NULL;
  page->memory = PAGE_SIZE - sizeof(t_span) - sizeof(t_page);
  page->pagetype = small;
  create_top_chunk(page);
  return page;
}

t_page* create_page(t_page* prev_page, t_span* span, int pagetype) {
  UNUSED(span);
  log_info("in create page");
  printf("prev_page: %p\n", prev_page);
  printf("prev_page->memory: %zu\n", prev_page->memory);
  printf("sizeof(t_page): %zu\n", sizeof(t_fpage));
  t_page* page = (t_page*)MEMORY_SHIFT(PAGE_SHIFT(prev_page), prev_page->memory);
  page->chunk_count = 1;
  page->prev = prev_page;
  page->next = NULL;
  page->memory = PAGE_SIZE - sizeof(t_page);
  page->pagetype = pagetype;
  create_top_chunk(page); // TODO figure out logic
  return page;
}