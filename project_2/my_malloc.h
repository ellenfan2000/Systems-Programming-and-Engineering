#include <stdlib.h>

typedef struct _meta_data {
  size_t size;
  struct _meta_data * next;
  struct _meta_data * prev;
  char alloc;
} meta_t;

meta_t * find_free_block(meta_t * blk,size_t size);
meta_t * get_next_blk(meta_t * blk);
void remove_free_block(meta_t * blk);
int split_free_block(meta_t * blk,size_t size);
void * merge_free_blk(meta_t * blk);
meta_t * bf_find_free_block(meta_t * blk,size_t size);

//thread safe malloc with lock
void * ts_malloc_lock(size_t size);
void ts_free_lock(void *ptr);

//Thread Safe malloc/free: non-locking version
void *ts_malloc_nolock(size_t size);
void ts_free_nolock(void *ptr);


// //First Fit malloc/free
// void * ff_malloc(size_t size);
// void ff_free(void * ptr);

// //Best Fit malloc/free
// void * bf_malloc(size_t size);
// void bf_free(void * ptr);
