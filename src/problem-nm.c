// (Nezavisla mnozina) nyni Vrcholove Pokryti, nazvy souboru zatim nechavam

#ifdef PROBLEM_NM

#include "common.h"
#include "problem-nm.h"

void ProblemInit() {
  problemHashMax = (1 << 16) - 1;
}

Problem* ProblemCreateRandom(int n) {
  ProblemInit();
  Problem* prb = malloc(sizeof(Problem));
  prb->g = MGrafCreateRandom(n,-9,1);
  prb->n=n;
  return prb;
}

Problem* ProblemCreateFromFile(char* filename) {
  ProblemInit();
  MGraf* g = MGrafCreateFromFile(filename);
  if(g==NULL) return NULL;
  Problem* prb = malloc(sizeof(Problem));
  prb->g = g;
  prb->n = prb->g->n;
  return prb;
}

Problem* ProblemCreateFromFileEdges(char* filename) {
  ProblemInit();
  Problem* prb = malloc(sizeof(Problem));
  prb->g = MGrafCreateFromFileEdges(filename);
  prb->n = prb->g->n;
  return prb;
}

void ProblemDestroy(Problem* prb) {
  MGrafDestroy(prb->g);
}

void ProblemPrint(Problem* prb) {
  MGrafPrint(prb->g);
}

int ProblemSave(Problem* prb, char* filename) {
  return MGrafSave(prb->g, filename);
}

char* StavString(Problem* prb, Stav* s) {
  char *str = malloc(sizeof(char)*(s->m->size*3+2));
  sprintf(str,"(");
  int i; for(i=0; i<s->m->size; i++) {
    if(SetIsIn(s->m,i)) {
      sprintf(str,"%s%i ",str,i);
    }
  }
  sprintf(str,"%s)",str);
  return str;
}

void StavPrint(Problem* prb, Stav* s) {
  char* str = StavString(prb, s);
  printf("%s",str);
  free(str);
}

char* StavReseni(Problem* prb, Stav* s, Stav* pred) {
  return StavString(prb, s);
}

Stav* ProblemStavCreate(Problem* prb, Set* m) {
  Stav* s = malloc(sizeof(Stav));
  //if(m) s->m = SetCopy(m);
  //else s->m = SetCreate(prb->g->n);
  s->m = m;
  return s;
}

int ProblemStavSave(Stav* s, char** pBuf) {
  SetSave(s->m, pBuf);
  return 1;
}

Stav* ProblemStavLoad(char** pBuf) {
  Stav* s = malloc(sizeof(Stav)); assert(s);
  s->m = SetLoad(pBuf); assert(s->m);
  return s;
}

void ProblemStavDestroy(Stav* s) {
  SetDestroy(s->m);
  free(s);
}

int ProblemStavCompare(void* v1, void* v2) {
  Stav* s1 = (Stav*) v1;
  Stav* s2 = (Stav*) v2;
  return SetCmp(s1->m, s2->m);
}

int StavHash(Stav* s) {
  return (s->m->data[0]) % problemHashMax;
}

Set* NepokryteVrcholy(Problem* prb, Set* x) {
  Set* z = SetInvertCopy(x);
  int i,j;
  for(i=0; i<prb->g->n; i++) {
    if(SetIsIn(x,i)) {
      for(j=0; j<prb->g->n; j++) {
        if(prb->g->mat[i][j]>=0 && SetIsIn(z,j)) SetRemove(z,j);
      }
    }
  }
  return z;
}

Stav* ProblemStart(Problem* prb) {
  //prb = ProblemCreate();
  //MGrafPrint(prb->g);
  Set* m = SetCreate(prb->g->n);
  //int i; for(i=0; i<prb->g->n; i++) SetAdd(m,i);
  Stav* s = ProblemStavCreate(prb, m);
  return s;
}

int ProblemCil(Problem* prb, Stav* s) {
  Set* z = NepokryteVrcholy(prb, s->m);
  int r = z->count;
  SetDestroy(z);
  if(r==0) return 1;
  else return 0;
}

SArray* ProblemNaslednici(Problem* prb, Stav* s) {
  SArray* p = SArrayCreate(ProblemStavCompare);
  Stav* n; Set* nm;
  int i; for(i=0; i<prb->g->n; i++) {
    if(!SetIsIn(s->m,i)) {
      nm = SetCopy(s->m);
      SetAdd(nm,i);
      n = ProblemStavCreate(prb, nm);
      SArrayAdd(p, (void*) n);
    }
  }
  return p;
}

double ProblemOceneni(Problem* prb, Stav* s1,Stav* s2) {
  return 1;
}

double ProblemHeuristika1(Problem* prb, Stav* s) {
  double h=0;
  Set* z = NepokryteVrcholy(prb, s->m);
  int zc = z->count;
  while(zc>0) {
    int maxI;
    int maxStupen=MGrafMaxStupen(prb->g, z, &maxI);
    h++;
    zc-=maxStupen+1;
    SetRemove(z,maxI);
  }
  SetDestroy(z);
  return h;
}

Heur heuristics[1] = {ProblemHeuristika1};

//ve str vraci pouze aproximacni cast reseni
double ProblemAproximace1(Problem* prb, Stav* s, char** str) {
  //hladova aproximace
  double h=0;
  Set* m = SetCopy(s->m);
  Set* z = NepokryteVrcholy(prb, m);
  while(z->count>0) {
    int maxI;
    MGrafMaxStupen(prb->g, z, &maxI);
    h++;
    SetAdd(m,maxI);
    SetDestroy(z);
    z = NepokryteVrcholy(prb, m);
  }
  SetDestroy(z);
  Stav* n = ProblemStavCreate(prb, m);
  *str = StavReseni(prb, n, NULL);
  ProblemStavDestroy(n);
  return h;
}

//ve str vraci pouze aproximacni cast reseni
double ProblemAproximace2(Problem* prb, Stav* s, char** str) {
  //2-aproximace po hranach
  double h=0;
  Set* m = SetCopy(s->m);
  Set* z = NepokryteVrcholy(prb, m);
  while(z->count>0) {
    int i; for(i=0; i<prb->g->n; i++) if(SetIsIn(z,i)) break;
    { SetAdd(m,i); h++; }
    int x=-1;
    int j; for(j=0; j<prb->g->n; j++) if(SetIsIn(z,j) && prb->g->mat[i][j]>0) {x=j; break;}
    if(x>=0) { SetAdd(m,x); h++; }
    SetDestroy(z);
    z = NepokryteVrcholy(prb, m);
  }
  SetDestroy(z);
  Stav* n = ProblemStavCreate(prb, m);
  *str = StavReseni(prb, n, NULL);
  ProblemStavDestroy(n);
  return h;
}

Aproximace aproximations[2] = {ProblemAproximace1,ProblemAproximace2};

#endif