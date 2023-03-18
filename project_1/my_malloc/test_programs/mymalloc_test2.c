#include <stdlib.h>
#include <stdio.h>
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


int main(int argc, char *argv[])
{
  const unsigned NUM_ITEMS = 10;
  int i;
  int size0, size1, size2, size3, size4, size5, size6, size7, size8, size9, size10, size11;
  int sum = 0;
  int expected_sum = 0;
  int *array[NUM_ITEMS];

  size0 = 4;
  expected_sum += size0 * size0;
  array[0] = (int *)MALLOC(size0 * sizeof(int));
  for (i=0; i < size0; i++) {
    array[0][i] = size0;
  } //for i

  size1 = 16;
  expected_sum += size1 * size1;
  array[1] = (int *)MALLOC(size1 * sizeof(int));
  for (i=0; i < size1; i++) {
    array[1][i] = size1;
  } //for i

  size2 = 8;
  expected_sum += size2 * size2;
  array[2] = (int *)MALLOC(size2 * sizeof(int));
  for (i=0; i < size2; i++) {
    array[2][i] = size2;
  } //for i

  size3 = 32;
  expected_sum += size3 * size3;
  array[3] = (int *)MALLOC(size3 * sizeof(int));
  for (i=0; i < size3; i++) {
    array[3][i] = size3;
  } //for i

  for (i=0; i < size0; i++) {
    sum += array[0][i];
  } //for i
  for (i=0; i < size2; i++) {
    sum += array[2][i];
  } //for i
  FREE(array[0]);
  FREE(array[2]);

  size4 = 7;
  expected_sum += size4 * size4;
  array[4] = (int *)MALLOC(size4 * sizeof(int));
  for (i=0; i < size4; i++) {
    array[4][i] = size4;
  } //for i

  size5 = 256;
  expected_sum += size5 * size5;
  array[5] = (int *)MALLOC(size5 * sizeof(int));
  for (i=0; i < size5; i++) {
    array[5][i] = size5;
  } //for i


  for (i=0; i < size5; i++) {
    sum += array[5][i];
  } //for i
  FREE(array[5]);

  for (i=0; i < size1; i++) {
    sum += array[1][i];
  } //for i
  FREE(array[1]);

  for (i=0; i < size3; i++) {
    sum += array[3][i];
  } //for i
  FREE(array[3]);

  size6 = 23;
  expected_sum += size6 * size6;
  array[6] = (int *)MALLOC(size6 * sizeof(int));
  for (i=0; i < size6; i++) {
    array[6][i] = size6;
  } //for i

  size7 = 4;
  expected_sum += size7 * size7;
  array[7] = (int *)MALLOC(size7 * sizeof(int));
  for (i=0; i < size7; i++) {
    array[7][i] = size7;
  } //for i

  for (i=0; i < size4; i++) {
    sum += array[4][i];
  } //for i
  FREE(array[4]);

  size8 = 10;
  expected_sum += size8 * size8;
  array[8] = (int *)MALLOC(size8 * sizeof(int));
  for (i=0; i < size8; i++) {
    array[8][i] = size8;
  } //for i

  size9 = 32;
  expected_sum += size9 * size9;
  array[9] = (int *)MALLOC(size9 * sizeof(int));
  for (i=0; i < size9; i++) {
    array[9][i] = size9;
  } //for i

  for (i=0; i < size6; i++) {
    sum += array[6][i];
  } //for i
  FREE(array[6]);

  for (i=0; i < size7; i++) {
    sum += array[7][i];
  } //for i
  FREE(array[7]);

  for (i=0; i < size8; i++) {
    sum += array[8][i];
  } //for i
  FREE(array[8]);

  for (i=0; i < size9; i++) {
    sum += array[9][i];
  } //for i
  FREE(array[9]);

  size10 = 1021;
  expected_sum += size10 * size10;
  array[10] = (int *)MALLOC(size10 * sizeof(int));
  for (i=0; i < size10; i++) {
    array[10][i] = size10;
  } //for i

  size11 = 4;
  expected_sum += size11 * size11;
  array[11] = (int *)MALLOC(size11 * sizeof(int));
  for (i=0; i < size11; i++) {
    array[11][i] = size11;
  } //for i

  for (i=0; i < size10; i++) {
    sum += array[10][i];
  } //for i
  FREE(array[10]);

  for (i=0; i < size11; i++) {
    sum += array[11][i];
  } //for i
  FREE(array[11]);

  if (sum == expected_sum) {
    printf("Calculated expected value of %d\n", sum);
    printf("Test passed\n");
  } else {
    printf("Expected sum=%d but calculated %d\n", expected_sum, sum);
    printf("Test failed\n");
  } //else

  return 0;
}
