#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "my_malloc.h"

int main(void) {
  void * head = sbrk(0);
  meta_d * header_p = head;
  sbrk(64);
  meta_d header;
  header.size = 30;
  header.alloc = '1';
  *header_p = header;
  printf("heap header location: %p \n", head);
  printf("meta data:location:%p, size: %lu, alloc:%c \n",
         (void *)header_p,
         header_p->size,
         header_p->alloc);
}
