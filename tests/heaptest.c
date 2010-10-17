#include <stdio.h>
#include "../src/heap.h"

typedef struct {
  int heapIndex;
  int cislo;
} Iint;

int intcmp(void* a, void* b) {
  if((*(int*)a)==(*(int*)b)) return 0;
  if(*(int*)a>*(int*)b) return 1;
  else return -1;
}

int Iintcmp(void* a, void* b) {
  if(((Iint*)a)->cislo == ((Iint*)b)->cislo) return 0;
  if(((Iint*)a)->cislo > ((Iint*)b)->cislo) return 1;
  else return -1;
}

void HeapPrintInt(Heap* hp) {
  int i; for(i=0; i<hp->size; i++) printf("%i ",*((int*)hp->array[i]));
  printf("\n");
}

void HeapPrintIint(Heap* hp) {
  int i; for(i=0; i<hp->size; i++) printf("%i ",((Iint*)hp->array[i])->cislo);
  printf("\n");
}

void IintStoreHeapIndex(void* x, int index) {
  //printf("shif%i ",index);
  ((Iint*)x)->heapIndex=index;
}

int main() {
  Heap* h;
  h = HeapCreate(Iintcmp,IintStoreHeapIndex,1);
  int i,j; for(i=0; i<1000; i++) {
    j = (i%100)*10 + i/100;
    Iint* x = (Iint*) malloc(sizeof(Iint));
    x->cislo = j;
    HeapPush(h,(void*) x);
    printf("%i:%i ", x->heapIndex, ((Iint*)h->array[x->heapIndex])->cislo);
  }
  //HeapPrintIint(h);
  HeapDeleteIndex(h,999);
  HeapDeleteIndex(h,1);
  void* x;
	while(x=HeapPop(h)) {
		printf("%i\n",((Iint*)x)->cislo);
	}
  //HeapPrintInt(h);
  //x=HeapPop(h);
  //HeapPrintInt(h);
  HeapDestroy(h,1);
  
  return 0;
}
