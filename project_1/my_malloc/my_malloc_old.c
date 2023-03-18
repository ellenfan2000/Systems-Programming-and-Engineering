#include "my_malloc.h"

#include<assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
// #include "my_malloc.h"
//#define ALIGN (sizeof(size_t))
#define METASIZE (sizeof(meta_d))
#define FPTRSIZE (sizeof(free_ptr))

void * start = NULL;
meta_t * free_first = NULL;

/* size_t size_align(size_t size) { */
/*   if (size % ALIGN == 0) { */
/*     return size; */
/*   } */
/*   else { */
/*     return (size / ALIGN + 1) * ALIGN; */
/*   } */
/* } */

meta_t * get_prev_header(meta_t * header){
  return (meta_t *)((char *)header-(header- 1)->size- 2*METASIZE);
}

meta_t * get_next_header(meta_t * header){
  return (meta_t *)((char *)header+header->size+ 2*METASIZE);
}

free_ptr * get_ptr(meta_t * header){
  return (free_ptr *)((char *)header + METASIZE);
}

meta_t * get_tail(meta_t * header){
  return (meta_t *)((char *)header + header->size + METASIZE);;
}

void heap_start() {
  if (start == NULL) {
    // start = sbrk(0);

    free_first = sbrk(FPTRSIZE + 2*METASIZE);
    free_first->alloc = '0';
    free_first->size = 16;

    free_ptr * ptr = get_ptr(free_first);
    ptr->next = NULL;
    ptr->prev = NULL;

    meta_t * tail = get_tail(free_first);
    tail->alloc = '0';
    tail->size = 16;
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
  
  size_t free_space = 0;
  free_ptr * ptr = get_ptr(free_first);
  meta_t * header = free_first;
  while(ptr != NULL){
    header = (meta_t *)((char *)ptr - METASIZE);
    free_space += header->size;
    ptr = ptr->next;
  }
  
  return free_space;
}

void remove_free_block(free_ptr * ptr){
  // free_ptr * prev = ptr->prev;
  // free_ptr * next = ptr->next;
  ptr->prev->next = ptr->next;
  if (ptr->next != NULL){
    ptr->next->prev = ptr->prev;
  }
  ptr = NULL;
}

//this function is to split big free block when 
//allocating space and update free blocks meta data
void use_free_block(meta_t * header, size_t size) {
  // origin free block tail
  assert(header->alloc == '0');
  meta_t * tail = get_tail(header);
  free_ptr * ptr = get_ptr(header);

  //if need to split the block
  if (header->size >= size + 2 * METASIZE+FPTRSIZE) {

    meta_t * alloc_tail = (meta_t *)((char *)header + size + METASIZE);
    alloc_tail->size = size;
    alloc_tail->alloc = '1';

    meta_t * new_header = (meta_t *)((char *)alloc_tail + METASIZE);
    new_header->size = header->size - size - 2 * METASIZE;
    new_header->alloc = '0';

    tail->size = new_header->size;
    tail->alloc = '0';

    header->size = size;
    header->alloc = '1';

    free_ptr * new_ptr = get_ptr(new_header);

    new_ptr->next = ptr->next;
    new_ptr->prev = ptr;

    if(ptr->next != NULL){
      ptr->next->prev = new_ptr;
    }
    ptr->next = new_ptr;
  }
  else {
    header->alloc = '1';
    tail->alloc = '1';
  }
  remove_free_block(ptr);
}

meta_t * find_free_block(free_ptr * ptr, size_t size){
  // free_ptr * ptr= get_ptr(header);
  // if(ptr != NULL){
  //   printf("start %p \n", (void *)ptr);
  // }
  // printf("start %p \n", (void *)ptr);
  meta_t * blk_header;
  while (ptr!= NULL){
    blk_header = (meta_t *)((char *)ptr - METASIZE);
    assert(blk_header->alloc=='0');
    if (blk_header->size >= size){
      // assert(blk_header->alloc=='0');
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
  // assert(get_ptr(free_first)->next);
  meta_t * blk_header = find_free_block(get_ptr(free_first)->next,size);
  if(blk_header != NULL){
    use_free_block(blk_header, size);
    return (char *)blk_header + METASIZE;
  }
  else{ //if no free block is available, increase the heap size. 
    blk_header = sbrk(blk_size);
    if(blk_header == (meta_t *)-1){
      return NULL;
    }
    blk_header->size = size;
    blk_header->alloc = '1';
    meta_t * blk_tail = get_tail(blk_header);
    blk_tail->size = size;
    blk_tail->alloc = '1';
    return (char *)blk_header + METASIZE;
  }
}

void merge_backward(meta_t * header) {
  assert(header->alloc == '0');
  free_ptr * ptr = get_ptr(header);
  meta_t * next_header = get_next_header(header);
  free_ptr * next_ptr = get_ptr(next_header);

  // while((void *)next_header < sbrk(0) && next_header->alloc == '0'){
  if((void *)next_header < sbrk(0) && next_header->alloc == '0'){
    header->size += next_header->size+2*METASIZE;
    get_tail(next_header)->size = header->size;
    // ptr->next = next_ptr->next;
    
    // if(next_ptr->next!=NULL){
    // next_ptr->next->prev = ptr;
    // }
    // next_ptr = next_ptr->next;
    remove_free_block(next_ptr);

    // next_header = get_next_header(next_header);
    // next_ptr = get_ptr(next_header);
  }
}

void merge_forward(meta_t * header) {
  free_ptr * ptr = get_ptr(header);
  meta_t * prev_header = get_prev_header(header);

  free_ptr * prev_ptr = get_ptr(prev_header);
  // while ((void *)prev_header>=start && prev_header->alloc == '0') {
  if((void *)prev_header>=start && prev_header->alloc == '0') {
    prev_header->size += header->size +2*METASIZE;
    get_tail(header)->size = prev_header->size;

    assert(get_ptr(prev_header)->next == ptr);

  
    prev_ptr->next = ptr->next;
    if(ptr->next != NULL){
      ptr->next->prev = prev_ptr;
    }

    ptr = NULL;
    header = prev_header;
    // prev_header = get_prev_header(header);
    // ptr = prev_ptr;
    // prev_ptr = get_ptr(prev_header);
  }

}

meta_t * find_prev_free(meta_t * header){
  meta_t * saver = header;
  assert((void *)header >=start);
  assert(header->alloc == '0');
  header = get_prev_header(header);
  // meta_d * prev_free_header = get_prev_header(header);
  while (header > free_first && header->alloc == '1') {
    // printf("location: %p \n",prev_free_header);
    header = get_prev_header(header);
  }
  assert(header->alloc =='0');
  return header;
}

void ff_free(void * ptr) {
  // printf("%p \n ", ptr); 
  if(ptr == NULL){
    return;
  }
  //update meta_d information to free block
  meta_t * blk_header = (meta_t *)((char *)ptr - METASIZE);
  meta_t * blk_tail = get_tail(blk_header);

  blk_header->alloc = '0';
  blk_tail->alloc = '0';

  //new free_block's pointer
  free_ptr * blk_free_ptr = get_ptr(blk_header);

  //the nearest free block before current block
  meta_t * prev_free_header = find_prev_free(blk_header);
  assert(prev_free_header >= free_first);
  free_ptr * prev_free_ptr = get_ptr(prev_free_header);

  blk_free_ptr->next = prev_free_ptr->next;
  blk_free_ptr->prev = prev_free_ptr;
  if(prev_free_ptr->next!= NULL){
    prev_free_ptr->next->prev = blk_free_ptr;
  }
  prev_free_ptr->next = blk_free_ptr;
  // blk_free_ptr->prev = prev_free_ptr;

  merge_backward(blk_header);
  merge_forward(blk_header);
  // if(get_next_header(prev_free_header)==blk_header){
  //   merge_forward(blk_header);
  // }
}


void * bf_malloc(size_t size){
  return ff_malloc(size);

}
void bf_free(void * ptr){
  ff_free(ptr);
}

// int main(void) {
//   heap_start();
//   free_ptr * free = get_ptr(free_first);
//   meta_d * a = (char*)ff_malloc(16)-METASIZE;
//   meta_d* b = (char*)ff_malloc(32)-METASIZE;
//   meta_d * c = (char*)ff_malloc(48)-METASIZE;
//   meta_d * d = (char*)ff_malloc(64)-METASIZE;
//   meta_d * e = (char*)ff_malloc(80)-METASIZE;
//   meta_d* f = (char*)ff_malloc(16)-METASIZE;
//   meta_d * g = (char*)ff_malloc(32)-METASIZE;
//   meta_d * h = (char*)ff_malloc(48)-METASIZE;
//   meta_d * i = (char*)ff_malloc(64)-METASIZE;
//   meta_d * j = (char*)ff_malloc(80)-METASIZE;

//    ff_free(h+1); 
//    ff_free(i+1);
//    ff_free(j+1);
//     ff_free(f+1);
//   ff_free(e+1);
//   ff_free(d+1);
//   ff_free(g+1);
//   // ff_free(d+1);
//   ff_free(b+1);
//   ff_free(c+1);
  
//   // ff_malloc(80);
  
//   // ff_malloc(80);
  
//   // ff_malloc(64);
  
//   // ff_free(b+1);
//   // ff_free(a+1);//first block cannot be changed
 
//   // ff_free(c+1);
  
  


//   void * k = ff_malloc(10);
//   void * l = ff_malloc(20);
//   void * m = ff_malloc(80);


//   size_t heap_usage = get_data_segment_size();
//   printf("heap start: %p \n", start);
//   printf("a location: %p \n", a);
//   printf("b location: %p \n", b);
//   printf("c location: %p \n", c);
//   printf("d location: %p \n", d);
//   printf("e location: %p \n", e);
//   printf("f location: %p \n", f);
//   printf("heap usage: %lu \n", heap_usage);
//   printf("fptr size: %lu \n", METASIZE);
//   printf("fptr size: %lu \n", FPTRSIZE);
//   // void * a = ff_malloc(16);
//   // size_t size = 16;

//   // size_t heap_usage = get_data_segment_size();
//   // size_t free_heap  =get_data_segment_free_space_size();
  
//   // void * b = ff_malloc(32);
//   // heap_usage = get_data_segment_size();
//   // free_heap  =get_data_segment_free_space_size();

//   // void * c = ff_malloc(64);
//   // heap_usage = get_data_segment_size();
//   // free_heap  =get_data_segment_free_space_size();

//   // ff_free(b);
//   // ff_free(c);
//   // heap_usage = get_data_segment_size();
//   // free_heap  =get_data_segment_free_space_size();

 
//   // void * d = ff_malloc(16);
//   // void * e = ff_malloc(32);
//   // heap_usage = get_data_segment_size();
//   // free_heap  =get_data_segment_free_space_size();
// }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// #define NUM_ITERS    2
// #define NUM_ITEMS    10000

// // #ifdef FF
// #define MALLOC(sz) ff_malloc(sz)
// #define FREE(p)    ff_free(p)
// // #endif
// // #ifdef BF
// // #define MALLOC(sz) bf_malloc(sz)
// // #define FREE(p)    bf_free(p)
// // #endif


// double calc_time(struct timespec start, struct timespec end) {
//   double start_sec = (double)start.tv_sec*1000000000.0 + (double)start.tv_nsec;
//   double end_sec = (double)end.tv_sec*1000000000.0 + (double)end.tv_nsec;

//   if (end_sec < start_sec) {
//     return 0;
//   } else {
//     return end_sec - start_sec;
//   }
// };


// struct malloc_list {
//   size_t bytes;
//   int *address;
// };
// typedef struct malloc_list malloc_list_t;

// malloc_list_t malloc_items[2][NUM_ITEMS];

// unsigned free_list[NUM_ITEMS];


// int main(int argc, char *argv[])
// {
//   int i, j, k;
//   unsigned tmp;
//   unsigned long data_segment_size;
//   unsigned long data_segment_free_space;
//   // struct timespec start_time, end_time;

//   srand(0);

//   const unsigned chunk_size = 32;
//   const unsigned min_chunks = 4;
//   const unsigned max_chunks = 16;
//   for (i=0; i < NUM_ITEMS; i++) {
//     malloc_items[0][i].bytes = ((rand() % (max_chunks - min_chunks + 1)) + min_chunks) * chunk_size;
//     malloc_items[1][i].bytes = ((rand() % (max_chunks - min_chunks + 1)) + min_chunks) * chunk_size;
//     free_list[i] = i;
//   } //for i

//   i = NUM_ITEMS;
//   while (i > 1) {
//     i--;
//     j = rand() % i;
//     tmp = free_list[i];
//     free_list[i] = free_list[j];
//     free_list[j] = tmp;
//   } //while


//   for (i=0; i < NUM_ITEMS; i++) {
//     malloc_items[0][i].address = (int *)MALLOC(malloc_items[0][i].bytes);
//   } //for i


//   //Start Time
//   // clock_gettime(CLOCK_MONOTONIC, &start_time);

//   for (i=0; i < NUM_ITERS; i++) {
//     unsigned malloc_set = i % 2;
//     for (j=0; j < NUM_ITEMS; j+=50) {
//       for (k=0; k < 50; k++) {
//         unsigned item_to_free = free_list[j+k];
//         FREE(malloc_items[malloc_set][item_to_free].address);
//       } //for k
//       for (k=0; k < 50; k++) {
// 	      malloc_items[1-malloc_set][j+k].address = (int *)MALLOC(malloc_items[1-malloc_set][j+k].bytes);
//       } //for k
//     } //for j
//   } //for i

//   //Stop Time
//   // clock_gettime(CLOCK_MONOTONIC, &end_time);

//   data_segment_size = get_data_segment_size();
//   data_segment_free_space = get_data_segment_free_space_size();
//   printf("data_segment_size = %lu, data_segment_free_space = %lu\n", data_segment_size, data_segment_free_space);

//   // double elapsed_ns = calc_time(start_time, end_time);
//   // printf("Execution Time = %f seconds\n", elapsed_ns / 1e9);
//   printf("Fragmentation  = %f\n", (float)data_segment_free_space/(float)data_segment_size);

//   for (i=0; i < NUM_ITEMS; i++) {
//     FREE(malloc_items[0][i].address);
//   } //for i

//   return 0;
// }