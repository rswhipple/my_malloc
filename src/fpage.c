#include "../inc/main.h"

void create_fpages(t_span* span) {
  int page_count = FAST_PAGE_ALLOCATION_SIZE;
  span->fastpages = create_base_fpage(span);
  page_count -= 1;

  t_fpage* current;
  t_fpage* temp;
  UNUSED(current);
  current = span->fastpages;
  int chunk_size = min_chunk_size + 8;
  while (page_count > 0 && chunk_size <= 64) {
    log_info("creating fpage");
    printf("page_count: %d\n", page_count);
    printf("chunk_size: %d\n", chunk_size);
    temp = create_fpage(current);
    current->next = temp;
    current = temp;
    chunk_size += 8;
    page_count -= 1;
  }
}

t_fpage* create_base_fpage(t_span* span) {
  log_info("creating base fpage");
  t_fpage* fpage = (t_fpage*)SPAN_SHIFT(span);
  fpage->chunk_count = 1;
  // fpage->prev = NULL;
  fpage->next = NULL;
  fpage->memory = PAGE_SIZE - sizeof(t_span) - sizeof(t_fpage);
  fpage->chunk_size = min_chunk_size;
  fpage->max_chunks = fpage->memory / fpage->chunk_size;
  printf("fpage pointer: %p\n", fpage);
  printf("available memory: %zu\n", fpage->memory);
  printf("chunk size: %zu\n", fpage->chunk_size);
  printf("maximum number of chunks: %zu\n", fpage->max_chunks);
  create_top_tiny_chunk(fpage);
  return fpage;
}

t_fpage* create_fpage(t_fpage* prev_page) {
  log_info("in create fpage");
  printf("prev_page: %p\n", prev_page);
  t_fpage* fpage = (t_fpage*)MEMORY_SHIFT(FASTPAGE_SHIFT(prev_page), prev_page->memory);
  fpage->chunk_count = 1;
  fpage->next = NULL;
  fpage->memory = PAGE_SIZE - sizeof(t_fpage);
  fpage->chunk_size = prev_page->chunk_size + 8;
  fpage->max_chunks = fpage->memory / fpage->chunk_size;
  printf("fpage pointer: %p\n", fpage);
  printf("chunk size: %zu\n", fpage->chunk_size);
  printf("maximum number of chunks: %zu\n", fpage->max_chunks);
  printf("sizeof(t_fpage): %zu\n", sizeof(t_fpage));
  create_top_tiny_chunk(fpage);
  return fpage;
}