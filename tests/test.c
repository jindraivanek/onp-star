#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include "memtest.h"
#include "cmockery/cmockery.h"
#include "../src/hasharray.h"

int intcmp(void* a, void* b) {
  if(*(int*)a==*(int*)b) return 0;
  if(*(int*)a<*(int*)b) return -1;
  else return 1;
}

void SArrayPrint(SArray* s) {
  int i; for(i=0; i<s->size; i++) printf("%i ", *(int*)s->data[i]);
  printf("\n");
}

void SArray_test(void **state) {
  SArray* s = SArrayCreate(intcmp);
  int i; for(i=2; i<20; i+=2) {
    int* x = (int*) malloc(sizeof(int));
    *x = i;
    SArrayAdd(s, (void*) x);
    SArrayPrint(s);
  }
  for(i=1; i<20; i+=2) {
    int* x = (int*) malloc(sizeof(int));
    *x = i;
    SArrayAdd(s, (void*) x);
    SArrayPrint(s);
  }
  for(i=18; i>=0; i-=1) {
    free(SArrayDeleteIndex(s, i));
    SArrayPrint(s);
  }
  SArrayDestroy(s);
}

int main() {
  const UnitTest tests[] = {
    unit_test(SArray_test)
  };
  return run_tests(tests);
}
