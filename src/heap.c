#include "common.h"
#include "heap.h"

void DummyStoreHeapIndexFunction(void* item, int index) {return;}

Heap* HeapCreate(HeapCompareFunction hcf, StoreHeapIndexFunction shif, int levels) { // levels .. pocet alokovanych vrstev
  Heap* ph = malloc(sizeof(Heap));
  ph->all = (1 << levels)-1; // (1 << levels) = 2^(levels)
  ph->array=malloc(ph->all*sizeof(void*));
  ph->size=0;
  ph->hcf=hcf;
  if(shif) ph->shif=shif;
  else ph->shif=DummyStoreHeapIndexFunction;
  return ph;
}

int HeapDestroy(Heap* ph) {
  free(ph->array);
  free(ph);
  return 1;
}

int HeapDecreaseKey(Heap* ph, int index) {
  while(1) {
    if(index>=ph->size || index<0) return 0;
    if(index==0) return 1;
    int otecIndex = (index-1) >> 1;
    if(ph->hcf(ph->array[index],ph->array[otecIndex])==-1) {
      ph->shif(ph->array[index],otecIndex);
      ph->shif(ph->array[otecIndex],index);
      swap(ph->array[index],ph->array[otecIndex]);
      index=otecIndex;
    } else return 1;
  }
}

int HeapIncreaseKey(Heap* ph, int index) {
  while(1) {
    if(index>=ph->size) return 0;
    int syn1Index = (index << 1) + 1;
    int syn2Index = syn1Index + 1;
    int synIndex=-1;
    if(syn1Index>=ph->size) return 1;
    if(syn1Index==ph->size-1) {
      if(ph->hcf(ph->array[index],ph->array[syn1Index])==1) synIndex=syn1Index;
    } else {
      if(ph->hcf(ph->array[index],ph->array[syn1Index])!=-1 && ph->hcf(ph->array[syn2Index],ph->array[syn1Index])!=-1) synIndex=syn1Index;
      if(ph->hcf(ph->array[index],ph->array[syn2Index])!=-1 && ph->hcf(ph->array[syn1Index],ph->array[syn2Index])==1) synIndex=syn2Index;
    }
    if(synIndex>0) {
      ph->shif(ph->array[index],synIndex);
      ph->shif(ph->array[synIndex],index);
      swap(ph->array[index],ph->array[synIndex]);
      index=synIndex;
    } else return 1;
  }
}

int HeapPush(Heap* ph, void* item) {
  if(ph->size==ph->all) { //nafouknuti
    int newall = ((ph->all+1) << 1) - 1;
    ph->array = realloc(ph->array,newall*sizeof(void*));
    ph->all=newall;
  }
  ph->shif(item,ph->size);
  ph->array[ph->size]=item;
  ph->size++;
  HeapDecreaseKey(ph,ph->size-1);
  return 1;
}

void HeapOdfouk(Heap* ph) {
  if(ph->size > ph->all/2 || ph->all <= 1) return;
  int newall = ph->all/2;
  ph->array = realloc(ph->array,newall*sizeof(void*));
  if(ph->array==NULL) { printf("ERROR realloc, %i\n", newall); exit(1); }
  ph->all=newall;
}

void* HeapPop(Heap* ph) {
  if(ph->size==0) return NULL;
  ph->shif(ph->array[0],-1);
  void* item = ph->array[0];
  ph->size--;
  if(ph->size>0) {
    ph->array[0]=ph->array[ph->size];
    ph->shif(ph->array[ph->size], 0);
    HeapIncreaseKey(ph,0);
  }
  HeapOdfouk(ph);
  return item;
}

int HeapDeleteIndex(Heap* ph, int index) {
  if(index >= ph->size || index < 0) return -1;
  ph->shif(ph->array[index],-1);
  ph->array[index]=ph->array[ph->size-1];
  ph->shif(ph->array[ph->size-1], index);
  ph->size--;
  HeapDecreaseKey(ph,index);
  HeapIncreaseKey(ph,index);
  HeapOdfouk(ph);
  return 1;
}

//zavola se hff na vsechny prvky haldy, pokud vrati false, prvek se odstrani.
void HeapFilter(Heap* ph, HeapFilterFunction hff) {
  int i=0;
  for(i=0; i<ph->size; i++) {
    if(hff(ph->array[i])==0) HeapDeleteIndex(ph,i);
  }
}

void HeapCheck(Heap* ph) {
  int i;
  for(i=0; i<ph->size; i++) {
    int syn1Index = (i << 1) + 1;
    int syn2Index = syn1Index + 1;
    if(syn1Index<ph->size)
      if(ph->hcf(ph->array[i],ph->array[syn1Index])==1)
        printf("%i ", i);
    if(syn2Index<ph->size)
/*
      assertcmd(ph->hcf(ph->array[i],ph->array[syn2Index])!=1, printf("%i ", i));
*/
      if(ph->hcf(ph->array[i],ph->array[syn2Index])==1) {
        printf("%i ", i);
        exit(1);
      }
  }
}
