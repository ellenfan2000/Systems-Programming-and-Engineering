#include "my_malloc.h"

#include <stdio.h>
#include <unistd.h>

#define NUM_ITERS 100
#define NUM_ITEMS 10000

struct malloc_list {
  size_t bytes;
  int * address;
};
typedef struct malloc_list malloc_list_t;

malloc_list_t malloc_items[2][NUM_ITEMS];

unsigned free_list[NUM_ITEMS];

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

meta_d * get_prev_header(meta_d * header){
  return (meta_d *)((char *)header-(header- 1)->size- 2*METASIZE);
}

meta_d * get_next_header(meta_d * header){
  return (meta_d *)((char *)header+header->size+ 2*METASIZE);
}

free_ptr * get_ptr(meta_d * header){
  return (free_ptr *)((char *)header + METASIZE);
}

meta_d * get_tail(meta_d * header){
  return (meta_d *)((char *)header + header->size + METASIZE);;
}

void heap_start() {
  if (start == NULL) {
    start = sbrk(0);

    free_first = sbrk(FPTRSIZE + 2*METASIZE);
    free_first->alloc = '0';
    free_first->size = 16;

    free_ptr * ptr = get_ptr(free_first);
    ptr->next = NULL;
    ptr->prev = NULL;

    meta_d * tail = get_tail(free_first);
    tail->alloc = '0';
    tail->size = 16;
  }
}

unsigned long get_data_segment_size(){
  // heap_start();
  void * end = sbrk(0);
  unsigned long ans = (char *) end -  (char *) start;
  return ans;
}

unsigned long get_data_segment_free_space_size(){
  // heap_start();
  // meta_d * heap_end = sbrk(0);

  // meta_d * blk_header = start;
  size_t free_space = 0;
  free_ptr * ptr = get_ptr(free_first);
  meta_d * header = free_first;
  while(ptr != NULL){
    header = (meta_d *)((char *)ptr - METASIZE);
    free_space += header->size;
    ptr = ptr->next;
  }
  // while (blk_header < heap_end) {
  //   if (blk_header->alloc == '0') {
  //     free_space += blk_header->size+2*METASIZE;
  //   }
  //   blk_header = get_next_header(blk_header);
  // }
  return free_space;
}

void remove_free_block(free_ptr * ptr){
  free_ptr * prev = ptr->prev;
  free_ptr * next = ptr->next;
  prev->next = next;
  if (next != NULL){
    next->prev = prev;
  }
}

//this function is to split big free block when allocating space and update free blocks meta data
void use_free_block(meta_d * header, size_t size) {
  // origin free block tail
  meta_d * tail = get_tail(header);
  free_ptr * ptr = get_ptr(header);

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
    // free_ptr * ptr = (free_ptr *)((char *)header + METASIZE);
    free_ptr * new_ptr = get_ptr(new_header);

    new_ptr->next = ptr->next;
    new_ptr->prev = ptr;

    ptr->next = new_ptr;
  }
  else {
    header->alloc = '1';
    tail->alloc = '1';
  }
  remove_free_block(ptr);
}

meta_d * find_free_block(free_ptr * ptr, size_t size){
  // free_ptr * ptr= get_ptr(header);
  meta_d * blk_header;
  while (ptr!= NULL){
    blk_header = (meta_d *)((char *)ptr - METASIZE);
    if (blk_header->size >= size){
      return blk_header;
    }
    ptr = ptr->next;
  }
  return NULL;
}

void * ff_malloc(size_t size) {
  heap_start();
  // meta_d * heap_end = sbrk(0);
  //ensure enough space when freeing (space for free_pointer)
  if (size < FPTRSIZE){
    size = FPTRSIZE;
  }
  size_t blk_size = size + 2 * METASIZE;

  // find free block 
  meta_d * blk_header = find_free_block(get_ptr(free_first)->next,size);
  if(blk_header != NULL){
    use_free_block(blk_header, size);
    return (char *)blk_header + METASIZE;
  }
  else{ //if no free block is available, increase the heap size. 
    blk_header = sbrk(blk_size);
    if(blk_header == (meta_d *)-1){
      return NULL;
    }
    blk_header->size = size;
    blk_header->alloc = '1';
    meta_d * blk_tail = get_tail(blk_header);
    blk_tail->size = size;
    blk_tail->alloc = '1';
    return (char *)blk_header + METASIZE;
  }
  // while (blk_header < heap_end) {
  //   if (blk_header->alloc == '0' && blk_header->size >= size) {
  //     use_free_block(blk_header, size);
  //     return (char *)blk_header + METASIZE;
  //   }
  //   blk_header = (meta_d *)((char *)blk_header + blk_header->size + 2 * METASIZE);
  // }
}

void merge_free_region(meta_d * header) {
  // meta_d * tail = get_tail(header);

  // meta_d * prev_tail = header - 1;
  // size_t size = header->size;

  free_ptr * ptr = get_ptr(header);
  meta_d * next_header = get_next_header(header);
  free_ptr * next_ptr = get_ptr(next_header);

  while((void *)next_header < sbrk(0) && ptr->next == next_ptr){
    header->size += next_header->size+2*METASIZE;
    get_tail(header)->size = header->size;
    ptr->next = next_ptr->next;

    next_ptr->next = NULL;
    next_ptr->prev = NULL;
    next_header = get_next_header(next_header);
    next_ptr = get_ptr(next_header);
  }
}

void ff_free(void * ptr) {
  // printf("%p \n ", ptr); 
  if(ptr == NULL){
    return;
  }
  meta_d * blk_header = (meta_d *)((char *)ptr - METASIZE);
  meta_d * blk_tail = get_tail(blk_header);
  blk_header->alloc = '0';
  blk_tail->alloc = '0';

  free_ptr * blk_free_ptr = get_ptr(blk_header);

  meta_d * prev_header = get_prev_header(blk_header);
  while (prev_header> free_first && prev_header->alloc != '0') {
    prev_header = get_prev_header(prev_header);
  }

  free_ptr * prev_free_ptr = get_ptr(prev_header);
  blk_free_ptr->next = prev_free_ptr->next;
  prev_free_ptr->next = blk_free_ptr;
  blk_free_ptr->prev = prev_free_ptr;
  merge_free_region(prev_header);
}


// void * bf_malloc(size_t size){


// }
// void bf_free(void * ptr){
//   ff_free(ptr);
// }

int main(void) {
  heap_start();
  
  // free_ptr * free = get_ptr(free_first);
  // void * a = ff_malloc(16);
  // void * b = ff_malloc(32);
  // void * c = ff_malloc(48);
  // void * d = ff_malloc(64);
  // void * e = ff_malloc(80);

  // ff_free(b);
  // ff_free(c);
  // ff_free(d);
  // void * f = ff_malloc(50);

  // size_t heap_usage = get_data_segment_size();
  // printf("heap start: %p \n", start);
  // printf("a location: %p \n", a);
  // printf("b location: %p \n", b);
  // printf("c location: %p \n", c);
  // printf("d location: %p \n", d);
  // printf("e location: %p \n", e);
  // printf("f location: %p \n", f);
  // printf("heap usage: %lu \n", heap_usage);
  // printf("fptr size: %lu \n", METASIZE);
  // printf("fptr size: %lu \n", FPTRSIZE);
  // void * a = ff_malloc(16);
  // size_t size = 16;

  // size_t heap_usage = get_data_segment_size();
  // size_t free_heap  =get_data_segment_free_space_size();
  
  // void * b = ff_malloc(32);
  // heap_usage = get_data_segment_size();
  // free_heap  =get_data_segment_free_space_size();

  // void * c = ff_malloc(64);
  // heap_usage = get_data_segment_size();
  // free_heap  =get_data_segment_free_space_size();

  // ff_free(b);
  // ff_free(c);
  // heap_usage = get_data_segment_size();
  // free_heap  =get_data_segment_free_space_size();

 
  // void * d = ff_malloc(16);
  // void * e = ff_malloc(32);
  // heap_usage = get_data_segment_size();
  // free_heap  =get_data_segment_free_space_size();
}
