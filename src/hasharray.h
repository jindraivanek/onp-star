#ifndef _HASHARRAY_H
#define _HASHARRAY_H

#include <stdlib.h>

#define SARRAY_FACTOR 1.5
#define SARRAY_INIT_SIZE 4

typedef int (*SArrayCompareFunction)(void*,void*);

typedef struct SArray {
  void** data;
  int size;
  int alloc;
  SArrayCompareFunction cmpf; 
} SArray;

SArray* SArrayCreate(SArrayCompareFunction cmpf);
int SArrayDestroy(SArray* s);
int SArrayAdd(SArray* s, void* x);
void* SArrayFind(SArray* s, void* x);
int SArrayIsHere(SArray* s, void* x);
void* SArrayDeleteIndex(SArray* s, int index);

typedef int (*hashfunction)(void*);

typedef struct HashArray {
  SArray** array; //pole
  int all; //alokovane pameti
  int size; //skutecne obsazenych polozek
  hashfunction hashfce; //hashovaci funkce
  SArrayCompareFunction cmpf; //porovnavaci funkce
} HashArray;

HashArray* HashArrayCreate(int size, hashfunction hf, SArrayCompareFunction cmpf);
int HashArrayDestroy(HashArray* ph);
int HashArrayAdd(HashArray* ph, void* x);
void* HashArrayFind(HashArray* ph, void* x);
int HashArrayIsHere(HashArray* ph, void* x);
void* HashArrayDeleteIndex(HashArray* ph, int hash, int index);
void* HashArrayFindDelete(HashArray* ph, void* x);

#endif
