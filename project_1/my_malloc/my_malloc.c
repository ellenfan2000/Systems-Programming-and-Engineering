#include "my_malloc.h"

#include<assert.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
// #include "my_malloc.h"

#define METASIZE (sizeof(meta_t))
meta_t * start = NULL;

void heap_start() {
  if (start == NULL) {
    start = sbrk(METASIZE);
    start->size = 0;
    start->alloc = '0';
    start->next = NULL;
    start->prev = NULL;
  }
}

int newly_sbrk = -1;

meta_t * find_free_block(meta_t * blk,size_t size){
    meta_t * prev = blk->prev;
    while(blk!= NULL){
        if (blk->size >= size){
            assert(blk->alloc == '0');
            newly_sbrk = -1;
            return blk;
        }
        prev = blk;
        blk = blk->next;
    }
    size_t blk_size = size + METASIZE;
    blk = sbrk(blk_size);
    newly_sbrk = 1;
    if(blk == (meta_t *)-1){
        return NULL;
    }
    blk->next = NULL;
    blk->prev = prev;
    blk->size = size;
    blk->alloc = '0';
    // update_alloced_ptr(blk);
    return blk;
}

meta_t * get_next_blk(meta_t * blk){
    return (meta_t *)((char *)blk + METASIZE + blk->size);
}

void update_alloced_ptr(meta_t * blk){
    meta_t * curr;
    if (blk->prev == NULL){
        curr = start;
    }else{
        curr = blk->prev;
    }
    // meta_t * curr = blk->prev;
    assert(curr->alloc = '0'); 
    while(curr<blk){
        curr->next = blk;
        curr = get_next_blk(curr);
    }
    assert(curr == blk);
    curr = get_next_blk(curr);

    if(blk->next ==NULL){  
        while((void *)curr<sbrk(0)){
            curr->prev = blk;
            curr= get_next_blk(curr);
        }
    }else{
        while(curr<blk->next){
            curr->prev = blk;
            curr= get_next_blk(curr);
        }
    }
}

void remove_free_block(meta_t * blk){
    blk->prev->next = blk->next;
    if(blk->next != NULL){
        blk->next->prev = blk->prev;
        update_alloced_ptr(blk->next);
    }
    update_alloced_ptr(blk->prev);
    // update_alloced_ptr(blk->prev);
}

int split_free_block(meta_t * blk,size_t size){
    // return (char *)free_blk + METASIZE;
    if(blk->size >= size + METASIZE){
        //change free blocks' pointers
        meta_t * new_free_blk = (meta_t *)((char *)blk + size + METASIZE);
        new_free_blk->alloc = '0';
        new_free_blk->size = blk->size - size - METASIZE;
        new_free_blk->next = blk->next;
        new_free_blk->prev = blk->prev;

        blk->prev->next = new_free_blk;
        if(blk->next != NULL){
            blk->next->prev = new_free_blk;
        }
        blk->size = size;
        //change allocated blocks' pointer 
        update_alloced_ptr(new_free_blk);
        return 1;
    }
    remove_free_block(blk);
    // update_alloced_ptr(blk->prev);
    return -1;
}

void * ff_malloc(size_t size){
    heap_start();
    meta_t * free_blk = find_free_block(start, size);
    free_blk->alloc = '1';
    if(newly_sbrk = -1){
        int split = split_free_block(free_blk,size);
    }// if(newly_sbrk = -1 && split == -1){
    //     remove_free_block(free_blk);
    // }
    return (char *)free_blk + METASIZE;

}

void * merge_free_blk(meta_t * blk){
    //start with the left one
    assert(blk->alloc == '0');
    //merge with the one after
    if(get_next_blk(blk) == blk->next && blk->next != NULL){
        blk->size += blk->next->size + METASIZE;
        blk->next = blk->next->next;
        if(blk->next!=NULL){
            blk->next->prev = blk;
        }
    }
    //merge with the one before;
    if(get_next_blk(blk->prev) == blk && blk->prev != start){
        blk->prev->size += blk->size + METASIZE;
        blk->prev->next = blk->next;
        if(blk->next!=NULL){
            blk->next->prev = blk->prev;
        }
        blk = blk->prev;
    }
    update_alloced_ptr(blk);
}

void ff_free(void * ptr){
    meta_t * blk = (meta_t *)((char *)ptr-METASIZE);
    if(blk->alloc!='1' || ptr == NULL){
        perror("invalid free \n");
    }
    blk->alloc = '0';
    blk->prev->next = blk;
    if(blk->next!=NULL){
        blk->next->prev = blk;
    }
    update_alloced_ptr(blk);

    merge_free_blk(blk);

    // if(blk->next != NULL){
    //     merge_free_blk(blk->next);
    // }
    // merge_free_blk(blk);
    // else{
    //     merge_free_blk(blk);
    // }
    
}

//Best Fit malloc/free
void * bf_malloc(size_t size){
    ff_malloc(size);

}
void bf_free(void * ptr){
    ff_free(ptr);
}

unsigned long get_data_segment_size(){
    // heap_start();
  void * end = sbrk(0);
  unsigned long ans = (char *) end -  (char *) start;
  return ans;
}
unsigned long get_data_segment_free_space_size(){
    size_t free_space = 0;
    meta_t * ptr = start;
    while(ptr != NULL){
        free_space += ptr->size;
        ptr = ptr->next;
    }
  return free_space;
}

#define NUM_ITERS    100
#define NUM_ITEMS    10000

// #ifdef FF
#define MALLOC(sz) ff_malloc(sz)
#define FREE(p)    ff_free(p)
// #endif
// #ifdef BF
// #define MALLOC(sz) bf_malloc(sz)
// #define FREE(p)    bf_free(p)
// #endif

char dumb_hash(void* p) {
    unsigned char h = (unsigned char) (17*(uintptr_t)p*(uintptr_t)p + 7*(uintptr_t)p + 3*((uintptr_t)p>>8)); // dumb garbage fake hash
    return h;
}

void* MALLOC_WITH_CHECK(size_t sz) {
    void* p = MALLOC(sz);
    unsigned char h = dumb_hash(p);
    memset(p,h,sz);
    return p;
}

bool FREE_WITH_CHECK(void* p, size_t sz) {
    bool r = true;
    unsigned char h = dumb_hash(p);
    int i;
    unsigned char* cp = p;
    for (i=0; i<sz; i++, cp++) {
        if (*cp != h) {
            r = false;
            printf("p=%p cp=%p *cp=%x h=%x\n",p,cp,*cp,h);
            break;
        }
    }
    FREE(p);
    return r;
}

double calc_time(struct timespec start, struct timespec end) {
  double start_sec = (double)start.tv_sec*1000000000.0 + (double)start.tv_nsec;
  double end_sec = (double)end.tv_sec*1000000000.0 + (double)end.tv_nsec;

  if (end_sec < start_sec) {
    return 0;
  } else {
    return end_sec - start_sec;
  }
};


struct malloc_list {
  size_t bytes;
  int *address;
};
typedef struct malloc_list malloc_list_t;

malloc_list_t malloc_items[2][NUM_ITEMS];

unsigned free_list[NUM_ITEMS];


int main(int argc, char *argv[])
{
  int i, j, k;
  unsigned tmp;
  unsigned long data_segment_size;
  unsigned long data_segment_free_space;
  struct timespec start_time, end_time;
  
  int num_iters = NUM_ITERS;
  if (argc>=2) {
      num_iters = atoi(argv[1]);
      if (num_iters==0) {
        //   printf("Syntax: %s [num_iters_override]\n",argv[0]);
          return 1;
      }
      num_iters &= -2; // make it a multiple of 2
  }

  srand(0);

  const unsigned chunk_size = 32;
  const unsigned min_chunks = 4;
  const unsigned max_chunks = 16;
  for (i=0; i < NUM_ITEMS; i++) {
    malloc_items[0][i].bytes = ((rand() % (max_chunks - min_chunks + 1)) + min_chunks) * chunk_size;
    malloc_items[1][i].bytes = ((rand() % (max_chunks - min_chunks + 1)) + min_chunks) * chunk_size;
    free_list[i] = i;
  } //for i

  i = NUM_ITEMS;
  while (i > 1) {
    i--;
    j = rand() % i;
    tmp = free_list[i];
    free_list[i] = free_list[j];
    free_list[j] = tmp;
  } //while


  for (i=0; i < NUM_ITEMS; i++) {
    malloc_items[0][i].address = (int *)MALLOC_WITH_CHECK(malloc_items[0][i].bytes);
  } //for i

  bool r = false;

  //Start Time
//   clock_gettime(CLOCK_MONOTONIC, &start_time);
  
  for (i=0; i < num_iters; i++) {
    unsigned malloc_set = i % 2;
    // printf("\rIteration %5d / %5d (%.1f%%)", i, num_iters, (float)i/num_iters*100); fflush(stdout);
    for (j=0; j < NUM_ITEMS; j+=50) {
      for (k=0; k < 50; k++) {
        unsigned item_to_free = free_list[j+k];
        r = FREE_WITH_CHECK(malloc_items[malloc_set][item_to_free].address, malloc_items[malloc_set][item_to_free].bytes);
        if (!r) {
        //   printf("ERROR: Content check failed while freeing %p\n", malloc_items[malloc_set][item_to_free].address);
        }
      } //for k
      for (k=0; k < 50; k++) {
        malloc_items[1-malloc_set][j+k].address = (int *)MALLOC_WITH_CHECK(malloc_items[1-malloc_set][j+k].bytes);
      } //for k
    } //for j
  } //for i
  printf("\n\n");

  //Stop Time
//   clock_gettime(CLOCK_MONOTONIC, &end_time);

  data_segment_size = get_data_segment_size();
  data_segment_free_space = get_data_segment_free_space_size();
  printf("data_segment_size = %lu, data_segment_free_space = %lu\n", data_segment_size, data_segment_free_space);

//   double elapsed_ns = calc_time(start_time, end_time);
//   printf("Execution Time = %f seconds\n", elapsed_ns / 1e9);
  printf("Fragmentation  = %f\n", (float)data_segment_free_space/(float)data_segment_size);

  for (i=0; i < NUM_ITEMS; i++) {
    r = FREE_WITH_CHECK(malloc_items[0][i].address, malloc_items[0][i].bytes);
    if (!r) {
      printf("ERROR: Content check failed while freeing %p\n", malloc_items[0][i].address);
    }
  } //for i

  return 0;
}


// int main(void) {
//   heap_start();
//   meta_t * a = (char*)ff_malloc(16)-METASIZE;
//     meta_t* b = (char*)ff_malloc(32)-METASIZE;
//     meta_t * c = (char*)ff_malloc(48)-METASIZE;
//   meta_t * d = (char*)ff_malloc(64)-METASIZE;
//   meta_t * e = (char*)ff_malloc(80)-METASIZE;
//   meta_t* f = (char*)ff_malloc(16)-METASIZE;
//   meta_t * g = (char*)ff_malloc(32)-METASIZE;
//   meta_t * h = (char*)ff_malloc(48)-METASIZE;
//   meta_t * i = (char*)ff_malloc(64)-METASIZE;
//   meta_t * j = (char*)ff_malloc(80)-METASIZE;

// //   void * a = ff_malloc(16);
// //     void* b = ff_malloc(32);
// //     void * c = ff_malloc(48);
// //  void * d = ff_malloc(64);
// //   void * e = ff_malloc(80);
// //   void* f = ff_malloc(16);
// //   void * g = ff_malloc(32);
// //   void * h = ff_malloc(48);
// //   void * i = ff_malloc(64);
// //   void * j = ff_malloc(80);

//    ff_free((void *)((char*)h+METASIZE)); 
// //    ff_free((char*)i+METASIZE);
//    ff_free((char*)j+METASIZE);
//    ff_free((char*)i+METASIZE);

//     ff_free((char*)f+METASIZE);
//   ff_free((char*)e+METASIZE);
// //   ff_free((char*)d+METASIZE);
// //   ff_free((char*)g+METASIZE);
//   // ff_free(d+1);
//   ff_free((char*)b+METASIZE);
//   ff_free((char*)c+METASIZE);
  
//   // ff_malloc(80);
  
//   // ff_malloc(80);
  
//   // ff_malloc(64);
  
//   // ff_free(b+1);
//   // ff_free(a+1);//first block cannot be changed
 
//   // ff_free(c+1);
  
  


//   void * k = ff_malloc(10);
//   void * l = ff_malloc(20);
//   void * m = ff_malloc(80);

//     ff_free(k);
//     ff_free(l);
//     ff_free(m);
//     ff_free((char*)d+METASIZE);
//     ff_free((char*)g+METASIZE);
//     ff_free((char*)a+METASIZE);

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
// //   void * a = ff_malloc(16);
// //   size_t size = 16;

// //   size_t heap_usage = get_data_segment_size();
// //   size_t free_heap  =get_data_segment_free_space_size();
  
// //   void * b = ff_malloc(32);
// //   heap_usage = get_data_segment_size();
// //   free_heap  =get_data_segment_free_space_size();

// //   void * c = ff_malloc(64);
// //   heap_usage = get_data_segment_size();
// //   free_heap  =get_data_segment_free_space_size();

// //   ff_free(b);
// //   ff_free(c);
// //   heap_usage = get_data_segment_size();
// //   free_heap  =get_data_segment_free_space_size();

 
// //   void * d = ff_malloc(16);
// //   void * e = ff_malloc(32);
// //   heap_usage = get_data_segment_size();
// //   free_heap  =get_data_segment_free_space_size();

// //     printf("heap usage: %lu \n", heap_usage);
// }