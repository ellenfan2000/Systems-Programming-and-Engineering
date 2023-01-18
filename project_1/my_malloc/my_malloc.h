#include <stdlib.h>

struct meta_data {
  size_t size;
  char alloc;
};

//First Fit malloc/free
void * ff_malloc(size_t size);
void ff_free(void * ptr);

//Best Fit malloc/free
void * bf_malloc(size_t size);
void bf_free(void * ptr);
