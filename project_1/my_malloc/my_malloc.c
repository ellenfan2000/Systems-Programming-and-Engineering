#include "my_malloc.h"

#include <stdio.h>
#include <unistd.h>

//#define ALIGN (sizeof(size_t))
#define OVERHEAD (sizeof(struct meta_data))

void * start = NULL;

/* size_t size_align(size_t size) { */
/*   if (size % ALIGN == 0) { */
/*     return size; */
/*   } */
/*   else { */
/*     return (size / ALIGN + 1) * ALIGN; */
/*   } */
/* } */

void heap_start() {
  if (start == NULL) {
    start = sbrk(0);
  }
}

unsigned long get_data_segment_size(){
  // heap_start();
  void * end = sbrk(0);
  unsigned long ans = (char *) end -  (char *) start;
  return ans;
}

unsigned long get_data_segment_free_space_size(){
  heap_start();
  meta_d * heap_end = sbrk(0);

  meta_d * rg_header = start;
  size_t free_space = 0;
  while (rg_header < heap_end) {
    if (rg_header->alloc == '0') {
      free_space += rg_header->size+2*OVERHEAD;
    }
    rg_header = (meta_d *)((char *)rg_header + rg_header->size + 2 * OVERHEAD);
  }
  return free_space;
}

void update_free_region(meta_d * header, size_t size) {
  meta_d * tail = (meta_d *)((char *)header + header->size + OVERHEAD);
  if (header->size > size + 2 * OVERHEAD) {
    meta_d * new_tail = (meta_d *)((char *)header + size + OVERHEAD);
    new_tail->size = size;
    new_tail->alloc = '1';

    meta_d * new_header = (meta_d *)((char *)new_tail + OVERHEAD);
    new_header->size = header->size - size - 2 * OVERHEAD;
    new_header->alloc = '0';

    tail->size = header->size - size - 2 * OVERHEAD;

    header->size = size;
    header->alloc = '1';
  }
  else {
    header->alloc = '1';
    tail->alloc = '1';
  }
}

void * ff_malloc(size_t size) {
  heap_start();
  meta_d * heap_end = sbrk(0);

  meta_d * rg_header = start;

  size_t rg_size = size + 2 * OVERHEAD;
  // void * ans;
  while (rg_header < heap_end) {
    if (rg_header->alloc == '0' && rg_header->size >= size) {
      //ans = (char *)rg_header + OVERHEAD;
      update_free_region(rg_header, size);
      return (char *)rg_header + OVERHEAD;
    }
    rg_header = (meta_d *)((char *)rg_header + rg_header->size + 2 * OVERHEAD);
  }
  rg_header = sbrk(rg_size);
  rg_header->size = size;
  rg_header->alloc = '1';
  meta_d * rg_tail = (meta_d *)((char *)rg_header + rg_header->size + OVERHEAD);
  rg_tail->size = size;
  rg_tail->alloc = '1';

  return (char *)rg_header + OVERHEAD;
}

void merge_free_region(meta_d * header) {
  meta_d * tail = (meta_d *)((char *)header + header->size + OVERHEAD);

  meta_d * prev_tail = header - 1;
  size_t size = header->size;
  while ((void *)prev_tail> start && prev_tail->alloc == '0') {
    header = (meta_d *)((char *)prev_tail - prev_tail->size - OVERHEAD);
    size = size + 2 * OVERHEAD + prev_tail->size;
    header->size = size;
    tail->size = size;
    prev_tail = header - 1;
  }
  meta_d * next_header = tail + 1;
  while ((void *)next_header < sbrk(0) && next_header->alloc == '0') {
    tail = (meta_d *)((char *)next_header + next_header->size + OVERHEAD);
    size = size + 2 * OVERHEAD + next_header->size;
    header->size = size;
    tail->size = size;
    next_header = tail + 1;
  }
}

void ff_free(void * ptr) {
  meta_d * rg_header = (meta_d *)((char *)ptr - OVERHEAD);
  meta_d * rg_tail = (meta_d *)((char *)ptr + rg_header->size);
  rg_header->alloc = '0';
  rg_tail->alloc = '0';
  merge_free_region(rg_header);
}

// void * bf_malloc(size_t size){


// }
void bf_free(void * ptr){
  ff_free(ptr);
}

int main(void) {
  heap_start();

  void * a = ff_malloc(16);
  size_t size = 16;

  size_t heap_usage = get_data_segment_size();
  size_t free_heap  =get_data_segment_free_space_size();
  
  void * b = ff_malloc(32);
  heap_usage = get_data_segment_size();
  free_heap  =get_data_segment_free_space_size();

  void * c = ff_malloc(64);
  heap_usage = get_data_segment_size();
  free_heap  =get_data_segment_free_space_size();

  ff_free(b);
  ff_free(c);
  heap_usage = get_data_segment_size();
  free_heap  =get_data_segment_free_space_size();

 
  void * d = ff_malloc(16);
  void * e = ff_malloc(32);
  heap_usage = get_data_segment_size();
  free_heap  =get_data_segment_free_space_size();
}
