#include "my_malloc.h"

#include <stdio.h>
#include <unistd.h>

//#define ALIGN (sizeof(size_t))
#define METASIZE (sizeof(meta_d))
#define FPTRSIZE (sizeof(free_ptr))

void * start = NULL;
meta_d * free_first = NULL;

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

    free_first = sbrk(FPTRSIZE + 2*METASIZE);
    free_first->alloc = '0';
    free_first->size = 0;

    free_ptr * ptr = (free_ptr *)((char *)free_first + METASIZE);
    ptr->next = NULL;
    ptr->prev = NULL;

    meta_d * tail = (meta_d *)((char *)free_first + free_first->size + METASIZE);
    tail->alloc = '0';
    tail->size = 0;
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

  meta_d * blk_header = start;
  size_t free_space = 0;
  while (blk_header < heap_end) {
    if (blk_header->alloc == '0') {
      free_space += blk_header->size+2*METASIZE;
    }
    blk_header = (meta_d *)((char *)blk_header + blk_header->size + 2 * METASIZE);
  }
  return free_space;
}

void remove_free_block(meta_d * header){
  free_ptr * ptr = (free_ptr *)((char *)header + METASIZE);
  free_ptr * prev = ptr->prev;
  free_ptr * next = ptr->next;
  prev->next = next;
  next->prev = prev;
}

//this function is to split big free block when allocating space and update free blocks meta data
void use_free_block(meta_d * header, size_t size) {
  // origin free block tail
  meta_d * tail = (meta_d *)((char *)header + header->size + METASIZE);

  //if need to split the block
  if (header->size > size + 2 * METASIZE) {

    meta_d * new_tail = (meta_d *)((char *)header + size + METASIZE);
    new_tail->size = size;
    new_tail->alloc = '1';

    meta_d * new_header = (meta_d *)((char *)new_tail + METASIZE);
    new_header->size = header->size - size - 2 * METASIZE;
    new_header->alloc = '0';

    tail->size = header->size - size - 2 * METASIZE;

    header->size = size;
    header->alloc = '1';

    //update free block pointers;
    free_ptr * ptr = (free_ptr *)((char *)header + METASIZE);
    free_ptr * new_ptr = (free_ptr *)((char *)new_header + METASIZE);

    new_ptr->next = ptr->next;
    new_ptr->prev = ptr;

    ptr->next = new_ptr;
  }
  else {
    header->alloc = '1';
    tail->alloc = '1';
  }
  remove_free_block(header);
}

meta_d * find_free_block(meta_d * header){
  if(header->alloc == '0'){
    free_ptr * ptr= (free_ptr *)((char *)header + METASIZE);
    while(ptr->next != NULL){
            


  }
  }

}

void * ff_malloc(size_t size) {
  heap_start();
  meta_d * heap_end = sbrk(0);
  // meta_d * blk_header = start;
  meta_d * blk_header = free_first;
  free_ptr * ptr= (free_ptr *)((char *)blk_header + METASIZE);
  
  //ensure enough space when freeing (space for free_pointer)
  if (size < FPTRSIZE){
    size = FPTRSIZE;
  }
  size_t blk_size = size + 2 * METASIZE;


  // find free block 
  // while (blk_header < heap_end) {
  //   if (blk_header->alloc == '0' && blk_header->size >= size) {
  //     use_free_block(blk_header, size);
  //     return (char *)blk_header + METASIZE;
  //   }
  //   blk_header = (meta_d *)((char *)blk_header + blk_header->size + 2 * METASIZE);
  // }
  

  //if no free block is available, increase the heap size. 
  blk_header = sbrk(blk_size);
  blk_header->size = size;
  blk_header->alloc = '1';
  meta_d * blk_tail = (meta_d *)((char *)blk_header + blk_header->size + METASIZE);
  blk_tail->size = size;
  blk_tail->alloc = '1';

  return (char *)blk_header + METASIZE;
}

void merge_free_region(meta_d * header) {
  meta_d * tail = (meta_d *)((char *)header + header->size + METASIZE);

  meta_d * prev_tail = header - 1;
  size_t size = header->size;
  while ((void *)prev_tail> start && prev_tail->alloc == '0') {
    header = (meta_d *)((char *)prev_tail - prev_tail->size - METASIZE);
    size = size + 2 * METASIZE + prev_tail->size;
    header->size = size;
    tail->size = size;
    prev_tail = header - 1;
  }
  meta_d * next_header = tail + 1;
  while ((void *)next_header < sbrk(0) && next_header->alloc == '0') {
    tail = (meta_d *)((char *)next_header + next_header->size + METASIZE);
    size = size + 2 * METASIZE + next_header->size;
    header->size = size;
    tail->size = size;
    next_header = tail + 1;
  }
}

void ff_free(void * ptr) {
  if(ptr == NULL){
    return;
  }
  meta_d * blk_header = (meta_d *)((char *)ptr - METASIZE);
  meta_d * blk_tail = (meta_d *)((char *)ptr + blk_header->size);
  blk_header->alloc = '0';
  blk_tail->alloc = '0';
  merge_free_region(blk_header);
}

void * bf_malloc(size_t size){


}
void bf_free(void * ptr){
  ff_free(ptr);
}

int main(void) {
  heap_start();
  printf("fptr size: %lu \n", METASIZE);
  printf("fptr size: %lu \n", FPTRSIZE);
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
