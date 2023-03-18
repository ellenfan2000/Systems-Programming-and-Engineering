#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "my_malloc.h"

#define NUM_ITERS    10000
#define NUM_ITEMS    10000
#define ALLOC_SIZE   128

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
            printf("ERROR: Content check failed while freeing %p!\n", p);
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


int main(int argc, char *argv[])
{
  int i, j;
  int *array[NUM_ITEMS];
  int *spacing_array[NUM_ITEMS];
  unsigned long data_segment_size;
  unsigned long data_segment_free_space;
  struct timespec start_time, end_time;

  int num_iters = NUM_ITERS;
  if (argc>=2) {
      num_iters = atoi(argv[1]);
      if (num_iters==0) {
          printf("Syntax: %s [num_iters_override]\n",argv[0]);
          return 1;
      }
      num_iters &= -2; // make it a multiple of 2
  }

  if (NUM_ITEMS < 10000) {
    printf("Error: NUM_ITEMS must be >= 1000\n");
    return -1;
  } //if

  for (i=0; i < NUM_ITEMS; i++) {
    array[i] = (int *)MALLOC_WITH_CHECK(ALLOC_SIZE);
    spacing_array[i] = (int *)MALLOC_WITH_CHECK(ALLOC_SIZE);
  } //for i

  for (i=0; i < NUM_ITEMS; i++) {
    FREE_WITH_CHECK(array[i],ALLOC_SIZE);
  } //for i

  //Start Time
  clock_gettime(CLOCK_MONOTONIC, &start_time);

  for (i=0; i < num_iters; i++) {
    printf("\rIteration %5d / %5d (%.1f%%)", i, num_iters, (float)i/num_iters*100); fflush(stdout);
    for (j=0; j < 1000; j++) {
      array[j] = (int *)MALLOC_WITH_CHECK(ALLOC_SIZE);
    } //for j

    for (j=1000; j < NUM_ITEMS; j++) {
      array[j] = (int *)MALLOC(ALLOC_SIZE);
      FREE_WITH_CHECK(array[j-1000],ALLOC_SIZE);

      if ((i==num_iters/2) && (j==NUM_ITEMS/2)) {
       //Record fragmentation halfway through (try to represent steady state)
       data_segment_size = get_data_segment_size();
       data_segment_free_space = get_data_segment_free_space_size();
      } //if
    } //for j

    for (j=NUM_ITEMS-1000; j < NUM_ITEMS; j++) {
      FREE_WITH_CHECK(array[j],ALLOC_SIZE);
    } //for j
  } //for i
  printf("\n\n");

  //Stop Time
  clock_gettime(CLOCK_MONOTONIC, &end_time);

  double elapsed_ns = calc_time(start_time, end_time);
  printf("Execution Time = %f seconds\n", elapsed_ns / 1e9);
  printf("Fragmentation  = %f\n", (float)data_segment_free_space/(float)data_segment_size);

  for (i=0; i < NUM_ITEMS; i++) {
    FREE_WITH_CHECK(spacing_array[i],ALLOC_SIZE);
  }
  
  return 0;
}
