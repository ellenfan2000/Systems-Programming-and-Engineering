#include <stdlib.h>

typedef struct _meta_data_new {
  size_t size;
  struct _meta_data_new * next;
  struct _meta_data_new * prev;
  char alloc;
} meta_t;


//First Fit malloc/free
void * ff_malloc(size_t size);
void ff_free(void * ptr);

//Best Fit malloc/free
void * bf_malloc(size_t size);
void bf_free(void * ptr);

unsigned long get_data_segment_size();//in byte
unsigned long get_data_segment_free_space_size(); //in byte