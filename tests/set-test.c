#include <stdlib.h>
#include "../src/set.h"

void SetPrint(Set* s) {
  int i;
  for(i=0; i<s->size; i++) printf("%i",SetIsIn(s,i));
  printf("\n");
}

int main() {
  Set* s = SetCreate(70);
  printf("baf\n");
  SetPrint(s);
  SetAdd(s,1);
  SetAdd(s,11);
  SetAdd(s,21);
  SetAdd(s,32);
  SetAdd(s,-33);
  SetAdd(s,34);
  SetAdd(s,52);
  SetAdd(s,65);
  SetPrint(s);
  Set* c = SetCopy(s);
  if(SetCmp(s,c)==0) printf("A");
  SetPrint(c);
  SetAdd(c,5);
  SetAdd(c,69);
  SetPrint(c);
}