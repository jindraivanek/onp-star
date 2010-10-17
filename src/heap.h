#ifndef _HEAP_H
#define _HEAP_H

#define swap(a,b) {void* tmp=a;	a=b; b=tmp;}

typedef int (*HeapCompareFunction)(void*,void*); //0=stejne, 1=prvni vetsi, -1=druhe vetsi
typedef void (*StoreHeapIndexFunction)(void*, int); //funkce ktera se zavola pri zmene indexu polozky v poli haldy, umoznuje zjistit pozici z haldy jen pomoci objektu, pouziti je nepovinne
typedef int (*HeapFilterFunction)(void*); //polozka v halde -> true/false

void DummyStoreHeapIndexFunction(void* item, int index);

typedef struct Heap {
  void** array; // pole
  int all; // alokovane pameti
  int size; // velikost haldy
  HeapCompareFunction hcf;
  StoreHeapIndexFunction shif;
} Heap;

Heap* HeapCreate(HeapCompareFunction hcf, StoreHeapIndexFunction shif, int levels);
int HeapDestroy(Heap* ph);
int HeapDecreaseKey(Heap* ph, int index);
int HeapIncreaseKey(Heap* ph, int index);
int HeapPush(Heap* ph, void* item);
void* HeapPop(Heap* ph);
int HeapDeleteIndex(Heap* ph, int index);
void HeapCheck(Heap* ph);

#endif
