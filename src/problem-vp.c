// Vrcholove Pokryti

#include "common.h"
#include "problem-vp.h"

int IntArraySum(int* x, int n) {
  int i,s = 0;
  for(i=0; i<n; i++) s+=x[i];
  return s;
}

int IntArrayMax(int* x, int n) {
  int i,m = 0,r=0;
  for(i=0; i<n; i++) if(x[i]>m) {m=x[i]; r=i;}
  return r;
}

void ProblemInit() {
  problemHashMax = (1 << 16) - 1;
}

Problem* ProblemCreateRandom(int n) {
  ProblemInit();
  Problem* prb = malloc(sizeof(Problem));
  prb->g = MGrafCreateRandom(n,-1,1,1);
  prb->n=n;
  return prb;
}

Problem* ProblemCreateFromFile(char* filename) {
  ProblemInit();
  MGraf* g = MGrafCreateFromFile(filename);
  if(g==NULL) return NULL;
  int i,j; 
  //zajisteni symetrie
  for(i=0; i<g->n; i++) {
    for(j=i+1; j<g->n; j++) g->mat[j][i]=g->mat[i][j];
  }
  Problem* prb = malloc(sizeof(Problem));
  prb->g = g;
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

//vrati pole se stupnem kazdeho vrcholu v podgrafu G_N(X),
//tj. zapocitavaji se pouze ty hrany, ktere nemaji zadny vrchol v x
int* NepokryteHrany(Problem* prb, Set* x) {
  int* s = malloc(sizeof(int)*prb->n);
  memset(s, 0, sizeof(int)*prb->n);
  Set* z = SetInvertCopy(x);
  int i,j;
  for(i=0; i<prb->g->n; i++) {
    if(SetIsIn(z,i)) {
      for(j=0; j<prb->g->n; j++) {
        if(prb->g->mat[i][j]>=0 && SetIsIn(z,j)) s[i]++;
      }
    }
  }
  SetDestroy(z);
  return s;
}

Stav* ProblemStart(Problem* prb) {
  Set* m = SetCreate(prb->g->n);
  Stav* s = ProblemStavCreate(prb, m);
  return s;
}

int ProblemCil(Problem* prb, Stav* s) {
  int* n = NepokryteHrany(prb, s->m);
  int zc = IntArraySum(n, prb->n);
  free(n);
  if(zc==0) return 1;
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
  int* n = NepokryteHrany(prb, s->m);
  int zc = IntArraySum(n, prb->n)/2; //pocet nepokrytych hran
  while(zc>0) {
    int maxI = IntArrayMax(n, prb->n);
    h++;
    zc-=n[maxI];
    n[maxI]=0;
  }
  free(n);
  return h;
}

Heur heuristics[1] = {ProblemHeuristika1};

//ve str vraci pouze aproximacni cast reseni
double ProblemAproximace1(Problem* prb, Stav* s, char** str) {
  //hladova aproximace
  double h=0;
  Set* m = SetCopy(s->m);
  int* z = NepokryteHrany(prb, m);
  while(IntArraySum(z, prb->n)>0) {
    int maxI = IntArrayMax(z, prb->n);
    h++;
    SetAdd(m,maxI);
    free(z);
    z = NepokryteHrany(prb, m);
  }
  free(z);
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
  int* z = NepokryteHrany(prb, m);
  while(IntArraySum(z, prb->n)>0) {
    int i; for(i=0; i<prb->g->n; i++) if(z[i]>0) break;
    { SetAdd(m,i); h++; }
    int x=-1;
    int j; for(j=i+1; j<prb->g->n; j++) if(z[j]>0 && prb->g->mat[i][j]>=0) {x=j; break;}
    assert(x>=0);
    { SetAdd(m,x); h++; }
    free(z);
    z = NepokryteHrany(prb, m);
  }
  free(z);
  Stav* n = ProblemStavCreate(prb, m);
  *str = StavReseni(prb, n, NULL);
  ProblemStavDestroy(n);
  return h;
}

Aproximace aproximations[2] = {ProblemAproximace1,ProblemAproximace2};

