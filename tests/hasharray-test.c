#include <stddef.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
// #include "memtest.h"
#include "cmockery/cmockery.h"
#include "../src/hasharray.h"

int hf(void* x) {
  return *((int*)x)%100;
}

int intcmp(void* a, void* b) {
  if(*(int*)a==*(int*)b) return 0;
  if(*(int*)a<*(int*)b) return -1;
  else return 1;
}

void SArrayPrint(SArray* s) {
  printf("%i: ", s->size);
  int i; for(i=0; i<s->size; i++) printf("%x->%i ", s->data[i], *(int*)s->data[i]);
  printf("\n");
}

void HashArrayPrint(HashArray* ph) {
  int i; for(i=0; i<ph->all; i++) {
    if(ph->array[i] != NULL) SArrayPrint(ph->array[i]);
    else printf("NULL\n");
  }
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
  SArrayDestroy(s,1);
}

int HashArray_test() {

  HashArray* h;
  h=HashArrayCreate(100,hf,intcmp);
  int i; for(i=2; i<2000; i+=2) {
    int* x = (int*) malloc(sizeof(int));
    *x = i;
    HashArrayAdd(h,(void*) x);
//     printf("pridano %i\n",i);
  }
  HashArrayPrint(h);
  int* a = (int*) malloc(sizeof(int)); *a = 148;
  int* b = (int*) malloc(sizeof(int)); *b = 2763;
//   printf("%i,%i,%i\n",HashArrayIsHere(h,(void*) a),HashArrayIsHere(h,(void*) b),intcmp((void*)a, (void*)b));
//   HashArrayDeleteIndex(h, 86, 13);
//   HashArrayDeleteIndex(h, 156, 13);
//   HashArrayDeleteIndex(h, 13, 13);
//   HashArrayDeleteIndex(h, 14, 1);
  HashArrayPrint(h);

  /*
  for(i=0;i<1000;i++) {
    if(h->array[i]) printf("%i\n",*(int*)(h->array[i]));
    else printf("NULL\n");
  }
  */
  HashArrayDestroy(h,1);

  /*
  Spojak* s = (Spojak*) malloc(sizeof(Spojak));
  SpojakCreate(s,intcmp);
  int i; for(i=0; i<100; i++) {
    int* x = (int*) malloc(sizeof(int)); *x=i;
    SpojakAdd(s,(void*) x);
  }
  int* a = (int*) malloc(sizeof(int)); *a = 47;
  int* b = (int*) malloc(sizeof(int)); *b = 2568;
  printf("%i,%i\n",SpojakIsHere(s,(void*) a),SpojakIsHere(s,(void*) b));
  */
  return 0;
}

int main() {
  const UnitTest tests[] = {
    unit_test(SArray_test)
//     ,unit_test(HashArray_test)
  };
  return run_tests(tests);
}
