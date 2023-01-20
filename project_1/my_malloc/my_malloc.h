#include <stdlib.h>

typedef struct _meta_data {
  size_t size;
  char alloc;
} meta_d;

typedef struct _free_block_ptr{
  meta_d * prev;
  meta_d * next;
  /* data */
} free_ptr;


//First Fit malloc/free
void * ff_malloc(size_t size);
void ff_free(void * ptr);

//Best Fit malloc/free
void * bf_malloc(size_t size);
void bf_free(void * ptr);

unsigned long get_data_segment_size();//in byte
unsigned long get_data_segment_free_space_size(); //in byte