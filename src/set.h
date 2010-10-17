#ifndef _SET_H
#define _SET_H

#include "common.h"

typedef struct Set {
  uint* data; // pole
  uint size; // velikost mnoziny
  uint count;
} Set;


Set* SetCreate(int size);
Set* SetCopy(Set* s);
int SetSave(Set* s, char** pBuf);
Set* SetLoad(char** pBuf);
void SetDestroy(Set* s);
int SetIsSubset(Set* set, Set* subset);
int SetCmp(Set* s1, Set* s2);
int SetIsIn(Set* s, int index);
void SetAdd(Set* s, int index);
void SetRemove(Set* s, int index);
void SetInvert(Set* s);
Set* SetInvertCopy(Set* s);
int* SetArray(Set* s);

#endif