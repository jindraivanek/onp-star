#include "common.h"
#include "hasharray.h"


SArray* SArrayCreate(SArrayCompareFunction cmpf) {
  SArray* s = malloc(sizeof(SArray));
  s->size=0;
  s->alloc = SARRAY_INIT_SIZE;
  s->data = malloc(sizeof(void*)*s->alloc);
  s->cmpf = cmpf;
  return s;
}

int SArrayDestroy(SArray* s) {
  free(s->data);
  free(s);
  return 1;
}

void SArrayRealloc(SArray* s) {
  while(s->size >= s->alloc) {
    s->alloc = s->alloc*SARRAY_FACTOR + 1;
    s->data = realloc(s->data, sizeof(void*)*s->alloc);
  }
  while(s->size+1 < (float)s->alloc/SARRAY_FACTOR) {
    s->alloc = (float)s->alloc/SARRAY_FACTOR;
    s->data = realloc(s->data, sizeof(void*)*s->alloc);
  }
}

int SArrayAdd(SArray* s, void* x) {
  int p;
  for(p=0; p<s->size && s->cmpf(s->data[p], x)!=1; p++);
  s->size++;
  if(s->size==s->alloc) SArrayRealloc(s);
  if(p<s->size-1)
    memmove(&s->data[p+1], &s->data[p], (s->size-p-1)*sizeof(void*));
  s->data[p] = x;
  return 1;
}

void* SArrayFind(SArray* s, void* x) {
  int i; for(i=0; i<s->size; i++) {
    if (s->cmpf(s->data[i], x) == 0) return s->data[i];
  }
  return NULL;
}

int SArrayIsHere(SArray* s, void* x) {
  if( SArrayFind(s,x) == NULL) return 0;
  else return 1;
}

//smaze prvek na indexu @index a vrati ho
//pamatuj: po smazani prvku se indexy vsech prvku za nim snizi o 1
void* SArrayDeleteIndex(SArray* s, int index) {
  if(index > s->size-1 || index < 0) return NULL;
  void* r = s->data[index];
  if(index < s->size-1) memmove(&s->data[index], &s->data[index+1], (s->size-1-index)*sizeof(void*));
  s->size--;
  if(s->size+1 < (float)s->alloc/SARRAY_FACTOR) SArrayRealloc(s);
  return r;
}  


// vystup z hash fce musi byt mensi nez size !!!
HashArray* HashArrayCreate(int size, hashfunction hf, SArrayCompareFunction cmpf) {
  HashArray* ph = malloc(sizeof (HashArray));
  ph->array = malloc(size * sizeof (SArray*));
  int i;
  for (i = 0; i < size; i++) ph->array[i] = NULL;
  ph->all = size;
  ph->size = 0;
  ph->hashfce = hf;
  ph->cmpf = cmpf;
  return ph;
}

int HashArrayDestroy(HashArray* ph) {
  int i;
  for (i = 0; i < ph->all; i++) {
    if (ph->array[i] != NULL) {
      SArrayDestroy(ph->array[i]);
    }
  }
  free(ph->array);
  free(ph);
  ph=NULL;
  return 1;
}

int HashArrayAdd(HashArray* ph, void* x) {
  int index = ph->hashfce(x);
  if (ph->array[index] == NULL) {
    ph->array[index] = SArrayCreate(ph->cmpf);
  }
  SArrayAdd(ph->array[index], x);
  ph->size++;
  return 1;
}

void* HashArrayFind(HashArray* ph, void* x) {
  int index = ph->hashfce(x);
  if (ph->array[index] == NULL) return NULL;
  else return SArrayFind(ph->array[index], x);
}

int HashArrayIsHere(HashArray* ph, void* x) {
  if (HashArrayFind(ph, x) != NULL) return 1;
  else return 0;
}

void* HashArrayDeleteIndex(HashArray* ph, int hash, int index) {
  if (hash < ph->all && hash >= 0 && ph->array[hash] != NULL) {
    void* r = SArrayDeleteIndex(ph->array[hash], index);
    if(r!=NULL) {
      ph->size--;
      if(ph->array[hash]->size == 0) {
        SArrayDestroy(ph->array[hash]);
        ph->array[hash] = NULL;
      }
      return r;
    }
  }
  return NULL;
} 

void* HashArrayFindDelete(HashArray* ph, void* x) {
  int index = ph->hashfce(x);
  SArray* s = ph->array[index];
  if (s == NULL) return NULL;
  int i; for(i=0; i<s->size; i++) {
    if (s->cmpf(s->data[i], x) == 0) return HashArrayDeleteIndex(ph, index, i);
    if (s->cmpf(s->data[i], x) == 1) return NULL;
  }
  return NULL;
}

