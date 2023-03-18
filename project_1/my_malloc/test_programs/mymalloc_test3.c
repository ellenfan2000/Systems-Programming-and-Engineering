#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "my_malloc.h"

#ifdef FF
#define MALLOC(sz) ff_malloc(sz)
#define FREE(p)    ff_free(p)
#endif
#ifdef BF
#define MALLOC(sz) bf_malloc(sz)
#define FREE(p)    bf_free(p)
#endif
#ifdef WF
#define MALLOC(sz) wf_malloc(sz)
#define FREE(p)    wf_free(p)
#endif

typedef struct {
  int number;
  char *name;
  char *address;
  char gender;
} student_t;

//credit: https://stackoverflow.com/questions/10162152/how-to-work-with-string-fields-in-a-c-struct
student_t *createStudent(int number, char *name,
		       char *addr, char sex) {
  student_t *p = MALLOC(sizeof(student_t));

  p->number = number;

  p->name = MALLOC(strlen(name)+1);
  strcpy(p->name, name);

  p->address = MALLOC(strlen(addr)+1);
  strcpy(p->address, addr);

  p->gender = sex;
  return p;
}

int main(int argc, char *argv[])
{
  const unsigned NUM_STUDENTS = 6;
  student_t *array[NUM_STUDENTS];

  array[0] = createStudent(0,
			   "Little Sheep",
			   "102 Castro St, Mountain View, CA 94041",
			   'F');
  array[1] = createStudent(1,
			   "Duke Chapel",
			   "401 Chapel Dr, Durham, NC 27708",
			   'M');
  array[2] = createStudent(2,
			   "Nasher Musuem",
			   "2001 Campus Dr, Durham, NC 27705",
			   'F');
  array[3] = createStudent(3,
			   "Paul Smith",
			   "8221 Melrose Ave, Los Angeles, CA 90046",
			   'M');
  array[4] = createStudent(4,
			   "Norton Simon",
			   "411 W Colorado Blvd, Pasadena, CA 91105",
			   'M');
  array[5] = createStudent(5,
			   "Santa Monica",
			   "200 Santa Monica Pier A, Santa Monica, CA 90401",
			   'F');
  if ( strcmp(array[0]->address, "102 Castro St, Mountain View, CA 94041") != 0 ||
       strcmp(array[3]->address, "8221 Melrose Ave, Los Angeles, CA 90046") != 0 ||
       strcmp(array[4]->name, "Norton Simon") != 0 ||
       array[NUM_STUDENTS - 1]->number != NUM_STUDENTS - 1 ) {
    printf("Test failed: contents not correct after initialization.\n");
    return 0;
  }

  FREE(array[0]);
  FREE(array[1]);
  FREE(array[2]);

  array[0] = createStudent(6,
			   "Sherlock Holmes",
			   "221B Baker St",
			   'M');
  array[1] = createStudent(7,
			   "Molly Hooper",
			   "this is a fake address",
			   'F');

  if ( array[0]->number != 6 ||
       array[1]->number != 7 ) {
    printf("Test failed: incorrect number.\n");
    return 0;
  }
  if ( strcmp(array[0]->name, "Sherlock Holmes") != 0 ||
       strcmp(array[1]->name, "Molly Hooper") != 0 ) {
    printf("Test failed: incorrect people's name.\n");
    return 0;
  }
  if ( strcmp(array[0]->address, "221B Baker St") != 0 ||
       strcmp(array[1]->address, "this is a fake address") != 0 ) {
    printf("Test failed: incorrect address.\n");
    return 0;
  }
  if ( array[0]->gender != 'M' ||
       array[1]->gender != 'F' ) {
    printf("Test failed: wrong gender.\n");
    return 0;
  }
  printf("Test passed!\n");
  return 0;
}
