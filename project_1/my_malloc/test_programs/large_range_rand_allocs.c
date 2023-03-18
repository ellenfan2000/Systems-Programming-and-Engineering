#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "my_malloc.h"

#define NUM_ITERS    50
#define NUM_ITEMS    10000

#ifdef FF
#define MALLOC(sz) ff_malloc(sz)
#define FREE(p)    ff_free(p)
#endif
#ifdef BF
#define MALLOC(sz) bf_malloc(sz)
#define FREE(p)    bf_free(p)
#endif

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
            //printf("p=%p cp=%p *cp=%x h=%x\n",p,cp,*cp,h);
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
  bool r;

  int num_iters = NUM_ITERS;
  if (argc>=2) {
      num_iters = atoi(argv[1]);
      if (num_iters==0) {
          printf("Syntax: %s [num_iters_override]\n",argv[0]);
          return 1;
      }
      num_iters &= -2; // make it a multiple of 2
  }

  srand(0);

  const unsigned chunk_size = 32;
  const unsigned min_chunks = 1;
  const unsigned max_chunks = 2048;
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


  //Start Time
  clock_gettime(CLOCK_MONOTONIC, &start_time);

  for (i=0; i < num_iters; i++) {
    unsigned malloc_set = i % 2;
    //printf("\rIteration %d / %d (%.1f%%)", i, num_iters, (float)i/num_iters*100); fflush(stdout);
    for (j=0; j < NUM_ITEMS; j+=50) {
      printf("\rIteration %d / %d (%.1f%%), item %d / %d", i, num_iters, (float)i/num_iters*100, j, NUM_ITEMS); fflush(stdout);
      for (k=0; k < 50; k++) {
        unsigned item_to_free = free_list[j+k];
        r = FREE_WITH_CHECK(malloc_items[malloc_set][item_to_free].address, malloc_items[malloc_set][item_to_free].bytes);
        if (!r) {
          printf("ERROR: Content check failed while freeing %p\n", malloc_items[malloc_set][item_to_free].address);
        }
      } //for k
      for (k=0; k < 50; k++) {
        malloc_items[1-malloc_set][j+k].address = (int *)MALLOC_WITH_CHECK(malloc_items[1-malloc_set][j+k].bytes);
      } //for k
    } //for j
  } //for i
  printf("\n\n");

  //Stop Time
  clock_gettime(CLOCK_MONOTONIC, &end_time);

  data_segment_size = get_data_segment_size();
  data_segment_free_space = get_data_segment_free_space_size();

  double elapsed_ns = calc_time(start_time, end_time);
  printf("Execution Time = %f seconds\n", elapsed_ns / 1e9);
  printf("Fragmentation  = %f\n", (float)data_segment_free_space/(float)data_segment_size);

  for (i=0; i < NUM_ITEMS; i++) {
    r = FREE_WITH_CHECK(malloc_items[0][i].address, malloc_items[0][i].bytes);
    if (!r) {
      printf("ERROR: Content check failed while freeing %p\n", malloc_items[0][i].address);
    }
  } //for i

  return 0;
}
