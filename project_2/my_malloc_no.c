#include "my_malloc.h"

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>

#define METASIZE (sizeof(meta_t))

__thread meta_t * startNolock = NULL;
// unsigned long total_space = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
__thread int newly_sbrk_nolock = 0;//indicator of whether the free block found is generated by s

//Build a permanent free block as the start of the heap
void heap_start_nolock() {
    
  if (startNolock == NULL) {
    pthread_mutex_lock(&lock);
    startNolock = sbrk(METASIZE);
    pthread_mutex_unlock(&lock);
    startNolock->size = 0;
    startNolock->alloc = '0';
    startNolock->next = NULL;
    startNolock->prev = NULL;
    // total_space += METASIZE;
  }
}

/*
    find the next block's header's position 
    if two headers are connected
*/
meta_t * get_next_blk(meta_t * blk){
    return (meta_t *)((char *)blk + METASIZE + blk->size);
}

/*
    remove the free block from the doubly linked list
*/
void remove_free_block(meta_t * blk){
    blk->prev->next = blk->next;
    if(blk->next != NULL){
        blk->next->prev = blk->prev;
    }
    blk->prev = NULL;
    blk->next = NULL;
}

/*
    split the free block if it is big enough to fit
    the current malloced data and a meta_t
    otherwise, just remove the free block
    from the doubly linked list
*/
int split_free_block(meta_t * blk,size_t size){
    if(blk->size > size + METASIZE){
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
        blk->next =NULL;
        blk->prev = NULL;
        //change allocated blocks' pointer 
        // update_alloced_ptr(new_free_blk);
        return 1;
    }

    //if donot need to split the block
    remove_free_block(blk);
    // update_alloced_ptr(blk->prev);
    return -1;
}
/*
    After freeing a malloced block, if its previous
    block is a free block (or the next block), merge 
    the two free blocks to form a bigger one
*/
void * merge_free_blk(meta_t * blk){
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
    if(get_next_blk(blk->prev) == blk && blk->prev != startNolock){
        blk->prev->size += blk->size + METASIZE;
        blk->prev->next = blk->next;
        if(blk->next!=NULL){
            blk->next->prev = blk->prev;
        }
        // blk = blk->prev;
    }

}
/*
    find a free block with smallest enough size
    to fit the malloced size.
    if there is no one in the existed heap
    increase the heap's size
*/
meta_t * bf_find_free_block(meta_t * blk,size_t size){
  // meta_t * prev = blk->prev;
    meta_t * ans; //store the smallest free block 
    size_t small = ULONG_MAX; //store the smallest free block's size
    int flag = 0; //indicator of whether there is a free block in the existed heap
    while(blk!= NULL){
        if (blk->size >= size){
            flag = 1;
            newly_sbrk_nolock = -1;
            assert(blk->alloc == '0');
            if(blk->size == size){
                return blk;
            }
            if(blk->size < small){
              small = blk->size;
              ans = blk;
            }
        }
        blk = blk->next;
    }
    if(flag == 1){
      return ans;
    }
    else{
      size_t blk_size = size + METASIZE;
      pthread_mutex_lock(&lock);
      blk = sbrk(blk_size);
      pthread_mutex_unlock(&lock);
    //   total_space += blk_size;
      newly_sbrk_nolock = 1;
      if(blk == (meta_t *)-1){
          return NULL;
      }
      blk->next = NULL;
      blk->prev = NULL;
      blk->size = size;
      blk->alloc = '0';
      return blk;
    }
}


//Thread safe best fit malloc with lock
void * ts_malloc_nolock(size_t size){
    
    heap_start_nolock();
    // printf("%p \n", (void *)startNolock);
    meta_t * free_blk = bf_find_free_block(startNolock, size);
    free_blk->alloc = '1';
    if(newly_sbrk_nolock == -1){
        int split = split_free_block(free_blk,size);
    }
    return (char *)free_blk + METASIZE;
}

void ts_free_nolock(void *ptr){
    if(startNolock == NULL){
        heap_start_nolock();
    }
    // printf("%p \n", (void *)startNolock);
    meta_t * blk = (meta_t *)((char *)ptr-METASIZE);
    if(blk->alloc!='1' || ptr == NULL){
        perror("invalid free \n");
    }
    blk->alloc = '0';
    meta_t * curr = startNolock;
    int flag = 0; //indicator whether the block is in the middle of the doubly linked free list or the last
    while(curr->next!= NULL){
      //in the middle of the list
      if (curr->next > blk && curr < blk){
        blk->next = curr->next;
        blk->prev = curr;
        curr->next = blk;
        blk->next->prev = blk;
        flag = 1;
        break;
      }
      curr = curr->next;
    }
    //at the end of the list
    if(flag == 0){
      curr->next = blk;
      blk->prev = curr;
      blk->next = NULL;
    }
    // printf("%p \n", (void *)blk);
    merge_free_blk(blk);

}
