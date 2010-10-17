#include "graf.h"
#include "common.h"
#include "problem-tsp.h"

void ProblemInit() {
  problemHashMax = (1 << 16) - 1;
}

Problem* ProblemCreateRandom(int n) {
  ProblemInit();
  Problem* prb = malloc(sizeof(Problem));
  prb->n = n;
  prb->g = MGrafCreateRandom(n,1,10,0);
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
  char *str = malloc(sizeof(char)*(s->m->size*3+5));
  sprintf(str,"(");
  int i; for(i=0; i<s->m->size; i++) {
    if(SetIsIn(s->m,i)) {
      sprintf(str,"%s%i ",str,i);
    }
  }
  sprintf(str,"%s) %i",str,s->v);
  return str;
}

void StavPrint(Problem* prb, Stav* s) {
  char* str = StavString(prb, s);
  printf("%s",str);
  free(str);
}

char* StavReseni(Problem* prb, Stav* s, Stav* pred) {
  char* str = malloc(PROBLEM_BACKTRACK);
  sprintf(str, "%i ", s->v);
  return str;
}

Stav* ProblemStavCreate(Problem* prb, int v, Set* m) {
  Stav* s = malloc(sizeof(Stav));
  if(m) s->m = SetCopy(m);
  else s->m = SetCreate(prb->g->n);
  s->v = v;
  return s;
}

int ProblemStavSave(Stav* s, char** pBuf) {
  bufwriteone(s->v, *pBuf);
  SetSave(s->m, pBuf);
  return 1;
}

Stav* ProblemStavLoad(char** pBuf) {
  Stav* s = malloc(sizeof(Stav)); assert(s);
  bufreadone(s->v, *pBuf);
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
  int r = SetCmp(s1->m, s2->m);
  if(r!=0) return r;
  else {
    if(s1->v == s2->v) return 0;
    if(s1->v > s2->v) return 1;
    return -1;
  }
}

int StavHash(Stav* s) {
  return (s->m->data[0]) % problemHashMax;
}

Stav* ProblemStart(Problem* prb) {
  return ProblemStavCreate(prb,0,NULL);
}

int ProblemCil(Problem* prb, Stav* s) {
  if(s->m->count == s->m->size && s->v == 0) return 1;
  else return 0;
}

SArray* ProblemNaslednici(Problem* prb, Stav* s) {
  SArray* p = SArrayCreate(ProblemStavCompare);
  Set* mv = SetCopy(s->m);
  SetAdd(mv,s->v);
  Stav* n;
  int i; for(i=0; i<prb->g->n; i++) {
    if(prb->g->mat[s->v][i]>=0 && (!SetIsIn(mv,i) || (i==0 && mv->count==prb->g->n))) {
      n = ProblemStavCreate(prb,i,mv);
      SArrayAdd(p, (void*) n);
    }
  }
  SetDestroy(mv);
  return p;
}

// vrati vsechny mozne predchudce (pro pouziti pri rekonstrukci reseni z closed stavu)
SArray* ProblemPredchudci(Problem* prb, Stav* s) {
  if(s->m->count==0) return NULL;
  SArray* p = SArrayCreate(ProblemStavCompare);
  int* filter = SetArray(s->m);
  Stav* n;
  int i; for(i=0; i<s->m->count; i++) {
    n = ProblemStavCreate(prb,filter[i],s->m);
    SetRemove(n->m, filter[i]);
    SArrayAdd(p, (void*) n);
  }
  free(filter);
  return p;
}

double ProblemOceneni(Problem* prb, Stav* s1,Stav* s2) {
  return prb->g->mat[s1->v][s2->v];
}

double ProblemHeuristika2(Problem* prb, Stav* s) {
  //soucet minimalnich ohodnoceni hran vedoucich z vrcholu odkud obch. cest. jeste nevysel 
  double sum=0;
  double min;
  Set* m1 = SetInvertCopy(s->m); //odkud muze jit
  Set* m2 = SetCopy(m1); 
  SetRemove(m2, s->v); SetAdd(m2, 0); //kam muze jit
  int* filter1 = SetArray(m1);
  int* filter2 = SetArray(m2);
  int i,j,x,y;
  for(i=0; i<m1->count; i++) {
    x=filter1[i];
    min = UINT_MAX;
    for(j=0; j<m2->count; j++) {
      y=filter2[j];
      if(prb->g->mat[x][y]>=0 && prb->g->mat[x][y]<min)
          min=prb->g->mat[x][y];
      }
    if(min!=UINT_MAX) sum+=min;
  }
  SetDestroy(m1); SetDestroy(m2); free(filter1); free(filter2);
  return sum;
}

double ProblemHeuristika3(Problem* prb, Stav* s) {
  //soucet minimalnich ohodnoceni hran vedoucich do vrcholu do kterych obch. cest. jeste nevesel 
  double sum=0;
  double min;
  Set* m2 = SetInvertCopy(s->m); //kam muze jit
  Set* m1 = SetCopy(m2); //odkud muze jit
  SetRemove(m1, s->v); SetAdd(m1, 0);
  int* filter1 = SetArray(m1);
  int* filter2 = SetArray(m2);
  int i,j,x,y;
  for(i=0; i<m1->count; i++) {
    x=filter1[i];
    min = UINT_MAX;
    for(j=0; j<m2->count; j++) {
      y=filter2[j];
      if(prb->g->mat[y][x]>=0 && prb->g->mat[y][x]<min)
          min=prb->g->mat[y][x];
      }
    if(min!=UINT_MAX) sum+=min;
  }
  SetDestroy(m1); SetDestroy(m2); free(filter1); free(filter2);
  return sum;
}

double ProblemHeuristika4(Problem* prb, Stav* s) {
  double h2=ProblemHeuristika2(prb, s);
  double h3=ProblemHeuristika3(prb, s);
  if(h2>h3) return h2;
  else return h3;
}

double ProblemHeuristika(Problem* prb, Stav* s) {

  return ProblemHeuristika4(prb, s);
}

Heur heuristics[3] = {ProblemHeuristika2,ProblemHeuristika3,ProblemHeuristika4};

double ProblemAproximace1(Problem* prb, Stav* s, char** str) {
  double sum=0.0;
  double min=UINT_MAX;
  Set* m = SetInvertCopy(s->m);
  SetRemove(m, s->v); SetAdd(m, 0); //kam muze jit
  *str = calloc(m->count*PROBLEM_BACKTRACK, sizeof(char)); strcpy(*str, "");
  int* filter = SetArray(m);
  int i,x,y,next=-1;
  x=s->v;
  while(m->count>0) {
    for(i=0; i<m->count; i++) {
      y=filter[i];
      if(y==0 && m->count>1) continue; //do startovniho vrcholu az nakonec
      min = UINT_MAX; next=-1;
      if(prb->g->mat[x][y]>=0 && prb->g->mat[x][y]<min) {
        min=prb->g->mat[x][y];
        next = y;
      }
    }
    sum+=min;
    if(next<0) break;
    SetRemove(m, next);
    x = next;
    free(filter); filter = SetArray(m);
    sprintf(*str, "%s%i ", *str, next);
  }
  SetDestroy(m); free(filter);
  return sum;
}

Aproximace aproximations[1] = {ProblemAproximace1};

