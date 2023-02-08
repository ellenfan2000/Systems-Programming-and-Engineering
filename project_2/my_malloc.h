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
meta_t * find_free_block(meta_t * blk,size_t size);

//thread safe malloc with lock
void * ts_malloc_lock(size_t size);
void ts_free_lock(void *ptr);


meta_t * find_free_block_nolock(meta_t * blk,size_t size);
void * merge_free_blk_nolock(meta_t * blk);


//Thread Safe malloc/free: non-locking version
void *ts_malloc_nolock(size_t size);
void ts_free_nolock(void *ptr);
