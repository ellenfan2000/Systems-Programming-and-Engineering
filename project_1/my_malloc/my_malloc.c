#include "my_malloc.h"

#include <stdio.h>
#include <unistd.h>

#define ALIGN (sizeof(size_t))

void * head = NULL;

size_t size_align(size_t size) {
  if (size % ALIGN == 0) {
    return size;
  }
  else {
    return (size / ALIGN + 1) * ALIGN;
  }
}

void heap_head() {
  if (head == NULL) {
    head = sbrk(0);
  }
}
/* int find_block(size_t size, void ** blk) { */
/*   void * pos = head; */
/*   void * heap_end = sbrk(0); */
/*   while ((char *)pos + size < heap_end) { */
/*   } */
/* } */

/* void * ff_malloc(size_t size) { */

/* } */

int main(void) {
  heap_head();
  /* if (head != NULL) { */
  /*   printf("1. "); */
  /*   printf("head of heap is %p \n", (void *)head); */
  /* } */
  size_t size = 2;
  void * loc = (char *)head + size;
  void * end = sbrk(0);
  printf("head of heap is %p \n", (void *)head);
  printf("end of heap is %p \n", (void *)end);
  printf("loc is %p \n", (void *)loc);
  printf("size of struct %lu \n", sizeof(size_t));
  printf("%lu \n", size_align(10));
  printf("%lu \n", size_align(20));
  printf("%lu \n", size_align(30));
  printf("%lu \n", ALIGN);

  if (loc >= head) {
    printf("out of heap \n");
  }
}
