#include <stdlib.h>
#include <math.h>

const int cil = 1024;

typedef struct {
  int x;
} Stav;

void ProblemStavPrint(Stav* s) {
  printf("%i",s->x);
}

Stav* ProblemStavCreate(int x) {
  Stav* s = malloc(sizeof(Stav));
  s->x = x;
  return s;
}

int ProblemStavCompare(void* v1, void* v2) {
  if(((Stav*)v1)->x == ((Stav*)v2)->x) return 0;
  else return -1;
}

int ProblemStavHash(Stav* s) {
  return (s->x) % 256;
}

Stav* ProblemStart() {
  return ProblemStavCreate(0); 
}

ProblemCil(Stav* s) {
  if(s->x == cil) return 1;
  else return 0;
}

Spojak* ProblemNaslednici(Stav* s) {
  Spojak* p = SpojakCreate(ProblemStavCompare);
  if(s->x < 100000) {
    SpojakAdd(p, (void*) ProblemStavCreate(s->x+1));
    SpojakAdd(p, (void*) ProblemStavCreate(s->x*2));
  } if(s->x > 0) {
    SpojakAdd(p, (void*) ProblemStavCreate(s->x-1));
    SpojakAdd(p, (void*) ProblemStavCreate(s->x/2));
  }
  return p;
}

int ProblemOceneni(Stav* s1,Stav* s2) {
  return 1;
}

int ProblemHeuristika0(Stav* s) {
  return 0;
}

int ProblemHeuristika(Stav* s) {
  float x = log(((float)cil)/((float)s->x));
  if(x<0) x=0;
  return x;
}
