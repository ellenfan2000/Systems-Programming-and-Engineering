#include "my_malloc.h"

#include<assert.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

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

int newly_sbrk = 0;

meta_t * find_free_block(meta_t * blk,size_t size){
    // meta_t * prev = blk->prev;
    while(blk!= NULL){
        if (blk->size >= size){
            assert(blk->alloc == '0');
            newly_sbrk = -1;
            return blk;
        }
        // prev = blk;
        blk = blk->next;
    }
    size_t blk_size = size + METASIZE;
    blk = sbrk(blk_size);
    newly_sbrk = 1;
    if(blk == (meta_t *)-1){
        return NULL;
    }
    blk->next = NULL;
    blk->prev = NULL;
    blk->size = size;
    blk->alloc = '0';
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
        // update_alloced_ptr(blk->next);
    }
    blk->prev = NULL;
    blk->next = NULL;
    // update_alloced_ptr(blk->prev);
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
        blk->next =NULL;
        blk->prev = NULL;
        //change allocated blocks' pointer 
        // update_alloced_ptr(new_free_blk);
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
    if(newly_sbrk == -1){
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
        // blk = blk->prev;
    }
    // update_alloced_ptr(blk);

}

void ff_free(void * ptr){
    meta_t * blk = (meta_t *)((char *)ptr-METASIZE);
    if(blk->alloc!='1' || ptr == NULL){
        perror("invalid free \n");
    }
    blk->alloc = '0';
    meta_t * curr = start;
    int flag = 0;
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
    merge_free_blk(blk);
    
}

meta_t * bf_find_free_block(meta_t * blk,size_t size){
  // meta_t * prev = blk->prev;
    meta_t * ans;
    size_t small = ULONG_MAX;
    // size = ULONG_MAX;
    int flag = 0;
    while(blk!= NULL){
        if (blk->size >= size){
            flag = 1;
            newly_sbrk = -1;
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
      blk = sbrk(blk_size);
      newly_sbrk = 1;
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


//Best Fit malloc/free
void * bf_malloc(size_t size){
    heap_start();
    meta_t * free_blk = bf_find_free_block(start, size);
    free_blk->alloc = '1';
    if(newly_sbrk == -1){
        int split = split_free_block(free_blk,size);
    }
    return (char *)free_blk + METASIZE;
    // return ff_malloc(size);
  
}
void bf_free(void * ptr){
    ff_free(ptr);
}

unsigned long get_data_segment_size(){
  heap_start();
  void * end = sbrk(0);
  unsigned long ans = (char *) end -  (char *) start;
  return ans;
}
unsigned long get_data_segment_free_space_size(){
    size_t free_space = 0;
    meta_t * ptr = start;
    while(ptr != NULL){
        free_space += ptr->size + METASIZE;
        ptr = ptr->next;
    }
  return free_space;
}

// int main(void) {
//   heap_start();
//   meta_t * a = (char*)bf_malloc(16)-METASIZE;
//     meta_t* b = (char*)bf_malloc(32)-METASIZE;
//     meta_t * c = (char*)bf_malloc(48)-METASIZE;
//   meta_t * d = (char*)bf_malloc(64)-METASIZE;
//   meta_t * e = (char*)bf_malloc(80)-METASIZE;
//   meta_t* f = (char*)bf_malloc(16)-METASIZE;
//   meta_t * g = (char*)bf_malloc(32)-METASIZE;
//   meta_t * h = (char*)bf_malloc(48)-METASIZE;
//   meta_t * i = (char*)bf_malloc(64)-METASIZE;
//   meta_t * j = (char*)bf_malloc(80)-METASIZE;

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
  
  


//   void * k = bf_malloc(10);
//   void * l = bf_malloc(20);
//   void * m = bf_malloc(80);

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