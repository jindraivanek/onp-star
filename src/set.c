#include "common.h"
#include "set.h"

const int intbit = 8*sizeof(int);

inline int SetAll(Set* s) { return s->size/intbit+1; }

Set* SetCreate(int size) {
  Set* s = malloc(sizeof(Set));
  s->size = size;
  s->count = 0;
  s->data = malloc(sizeof(int)*SetAll(s));
  int i; for(i=0; i<SetAll(s); i++) s->data[i]=0;
  return s;
}

Set* SetCopy(Set* s) {
  Set* c = malloc(sizeof(Set));
  memcpy(c, s, sizeof(Set));
  c->data = malloc(sizeof(uint)*SetAll(c));
  memcpy(c->data, s->data, sizeof(uint)*SetAll(c));
  return c;
}

int SetSave(Set* s, char** pBuf) {
  bufwriteone(s->size, *pBuf);
  bufwriteone(s->count, *pBuf);
  bufwritearray(s->data,SetAll(s), *pBuf);
  return 1;
}

Set* SetLoad(char** pBuf) {
  Set* s = malloc(sizeof(Set)); assert(s);
  s->size = 0;
  bufreadone(s->size, *pBuf);
  bufreadone(s->count, *pBuf);
  s->data = malloc(sizeof(int)*SetAll(s)); assert(s->data);
  bufreadarray(s->data, SetAll(s), *pBuf);
  return s;
}

void SetDestroy(Set* s) {
  free(s->data);
  free(s);
}

int SetIsSubset(Set* set, Set* subset) {
  if(subset->size > set->size) return 0;
  int i; for(i=0; i<SetAll(subset); i++) {
    if( ~(~subset->data[i] | (subset->data[i] & set->data[i])) != 0 ) return 0;
  }
  return 1;
}

int SetCmp(Set* s1, Set* s2) {
  if(s1->size != s2->size || s1->count != s2->count) return -1;
  int i; for(i=0; i<SetAll(s1); i++) {
    if( (s1->data[i] > s2->data[i]) ) return 1;
    if( (s1->data[i] < s2->data[i]) ) return -1;
  }
  return 0;
}

int SetIsIn(Set* s, int index) { //zjisti jestli je prvek v mnozine
  if(index>=s->size) return 0;
  uint i = index/intbit;
  uint base = i*intbit;
  uint offset = index - base;
  uint x = s->data[i];
  return (x >> offset) & 1;
}

void SetAdd(Set* s, int index) {
  if(index>=s->size || index<0) return;
  uint i = index/intbit;
  uint base = i*intbit;
  uint offset = index - base;
  uint x = s->data[i];
  if((x >> offset) & 1) return; //uz tam je?
  s->data[i] = x + (1 << offset);
  s->count++;
}

void SetRemove(Set* s, int index) {
  if(index>=s->size || index<0 || s->count==0) return;
  uint i = index/intbit;
  uint base = i*intbit;
  uint offset = index - base;
  uint x = s->data[i];
  if(((x >> offset) & 1) == 0) return; //je tam vubec?
  s->data[i] = x - (1 << offset);
  s->count--;
}

void SetInvert(Set* s) {
  int i; for(i=0; i<SetAll(s); i++) s->data[i]=~s->data[i];
  s->count = s->size - s->count;
}

Set* SetInvertCopy(Set* s) {
  Set* r = SetCopy(s);
  SetInvert(r);
  return r;
}

//vrati pole intu s prvky ktere jsou v mnozine
//velikost pole odpovida s->count
int* SetArray(Set* s) {
  int i;
  int* r = malloc(sizeof(int)*s->count);
  int c=0;
  for(i=0; i<s->size; i++) {
    if(SetIsIn(s,i)) {
      r[c]=i;
      c++;
    }
  }
  return r;
}

