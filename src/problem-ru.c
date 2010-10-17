// rozvrhovani uloh

#include "common.h"
#include "problem-ru.h"

Uloha** UlohyCreateRandom(int n, double casMin, double casMax,
                                 double dokdyMin, double dokdyMax,
                                 double penaleMin, double penaleMax,
                                 double koefMin, double koefMax) {
  Uloha** ulohy = malloc(sizeof(Uloha*)*n);
  int i; for(i=0; i<n; i++) {
    Uloha* u = malloc(sizeof(Uloha));
    u->cas = randRange(casMin, casMax);
    u->dokdy = randRange(dokdyMin, dokdyMax);
    u->penale = randRange(penaleMin, penaleMax);
    u->koef = randRange(koefMin, koefMax);
    ulohy[i]=u;
  }
  return ulohy;
}


double UlohySumCas(Problem* prb, Set* m) {
  double sum=0;
  int i; for(i=0; i<prb->n; i++) { if(SetIsIn(m, i)) sum+=prb->ulohy[i]->cas; }
  return sum;
}

double UlohaNaklad(Uloha* u, double time) {
  if(time <= u->dokdy) return 0;
  else return u->penale + u->koef*(time-u->dokdy);
}

double UlohaIndexNaklad(Problem* prb, int uloha, double time) {
  return UlohaNaklad(prb->ulohy[uloha], time);
}

double StavNaklad(Problem* prb, Stav* s, int uloha) {
  double t = UlohySumCas(prb,s->m);
  return UlohaIndexNaklad(prb,uloha,t);
}

void ProblemInit() {
  problemHashMax = (1 << 16) - 1;
}

Problem* ProblemCreateRandom(int n) {
  ProblemInit();
  Problem* prb = malloc(sizeof(Problem));
  prb->n = n;
  prb->ulohy = UlohyCreateRandom(n, 1,10, 1,10, 1,10, 0,10);
  return prb;
}

Problem* ProblemCreateFromFile(char* filename) {
  ProblemInit();
  FILE* f = fopen(filename,"r");
  Problem* prb = malloc(sizeof(Problem));
  if(fscanf(f,"%i\n",&prb->n)<1) goto error; //pocet uloh
  if(prb->n<1) goto error; //musi byt kladny
  Uloha** p = malloc(sizeof(Uloha*)*prb->n);
  int i;
  for(i=0; i<prb->n; i++) {
    p[i] = malloc(sizeof(Uloha));
    if(fscanf(f,"%lf %lf %lf %lf\n", &p[i]->cas, &p[i]->dokdy, &p[i]->penale, &p[i]->koef)<4) goto error;
    if(p[i]->cas<0 || p[i]->dokdy<0 || p[i]->penale<0 || p[i]->koef<0) goto error; //zaporne ne
  }
  fclose(f);
  prb->ulohy=p;
  return prb;
  error:
  fclose(f);
  return NULL;
}

void ProblemDestroy(Problem* prb) {
  int i; for(i=0; i<prb->n; i++) {
    free(prb->ulohy[i]);
  }
  free(prb->ulohy);
  free(prb);
}

void ProblemPrint(Problem* prb) {
  printf("   cas   doKdy   penale   koef\n");
  int i; for(i=0; i<prb->n; i++) {
    printf("%i: %.3f %.3f %.3f %.3f\n", i, prb->ulohy[i]->cas, prb->ulohy[i]->dokdy, prb->ulohy[i]->penale, prb->ulohy[i]->koef);
  }
}

int ProblemSave(Problem* prb, char* filename) {
  FILE* f = fopen(filename,"w");
  if(fprintf(f,"%i\n",prb->n)<0) goto error; //pocet uloh
  int i;
  for(i=0; i<prb->n; i++) {
    if(fprintf(f,"%lf %lf %lf %lf\n", prb->ulohy[i]->cas, prb->ulohy[i]->dokdy, prb->ulohy[i]->penale, prb->ulohy[i]->koef)<4) goto error;
  }
  fclose(f);
  return 1;
  error:
  fclose(f);
  return -1;
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
  int x=-1,i; for(i=0; i<prb->n; i++) if(SetIsIn(s->m,i) && !SetIsIn(pred->m,i)) x=i;
  char* r = malloc(sizeof(char)*PROBLEM_BACKTRACK);
  sprintf(r,"%i ", x);
  return r;
}

Stav* ProblemStavCreate(Problem* prb, Set* m, int v) {
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

Stav* ProblemStart(Problem* prb) {
  Set* m = SetCreate(prb->n);
  return ProblemStavCreate(prb,m,-1);
}

int ProblemCil(Problem* prb, Stav* s) {
  if(s->m->count == s->m->size) return 1;
  else return 0;
}

SArray* ProblemNaslednici(Problem* prb, Stav* s) {
  SArray* p = SArrayCreate(ProblemStavCompare);
  Stav* n;
  int i; for(i=0; i<prb->n; i++) {
    if(!SetIsIn(s->m,i)) {
      Set* m = SetCopy(s->m);
      SetAdd(m,i);
      n = ProblemStavCreate(prb,m,i);
      SArrayAdd(p, (void*) n);
    }
  }
  return p;
}

SArray* ProblemPredchudci(Problem* prb, Stav* s) {
  if(s->m->count==0) return NULL;
  SArray* p = SArrayCreate(ProblemStavCompare);
  Stav* n;
  int i; for(i=0; i<prb->n; i++) {
    if(SetIsIn(s->m,i)) {
      Set* m = SetCopy(s->m);
      SetRemove(m,i);
      n = ProblemStavCreate(prb,m,i);
      SArrayAdd(p, (void*) n);
    }
  }
  return p;  
}

double ProblemOceneni(Problem* prb, Stav* s1,Stav* s2) {
  int x=-1,i; for(i=0; i<prb->n; i++) if(SetIsIn(s2->m,i) && !SetIsIn(s1->m,i)) x=i;
  return StavNaklad(prb,s2,x);
}

double ProblemHeuristika1(Problem* prb, Stav* s) {
  double cas = UlohySumCas(prb, s->m);
  double h=0;
  int i; for(i=0; i<prb->n; i++)
    if(!SetIsIn(s->m,i)) h+=UlohaIndexNaklad(prb,i,cas+prb->ulohy[i]->cas);
  return h;
}

int CmpUlohaCas(void* x1, void* x2) {
  if(((Uloha*)x1)->cas < ((Uloha*)x2)->cas) return -1;
  if(((Uloha*)x1)->cas > ((Uloha*)x2)->cas) return 1;
  return 0;
}

double ProblemHeuristika2(Problem* prb, Stav* s) {
  SArray* c = SArrayCreate(CmpUlohaCas);
  int i; for(i=0; i<prb->n; i++) if(!SetIsIn(s->m,i)) SArrayAdd(c, (void*)prb->ulohy[i]);
  //v c jsou zbyvajici ulohy serazene vzestupne podle casu
  double cas = UlohySumCas(prb,s->m);
  Set* mInv = SetCopy(s->m); SetInvert(mInv);
  if(mInv->count==0) return 0;
  double min,x,h=0;
  for(i=0; i<c->size; i++) {
    cas += ((Uloha*)c->data[i])->cas;
    min = (unsigned int)~0;
    int j; for(j=0; j<c->size; j++) {
      x = UlohaNaklad((Uloha*)c->data[j], cas);
      if(x<min) min=x;
    }
    h+=min;
  }
  SArrayDestroy(c);
  return h;
}

double ProblemHeuristika3(Problem* prb, Stav* s) {
  return max(ProblemHeuristika1(prb,s), ProblemHeuristika2(prb,s));
}

Heur heuristics[3] = {ProblemHeuristika1, ProblemHeuristika2, ProblemHeuristika3};

double ProblemAproximace1a2(Problem* prb, Stav* s, char** str, int type) {
  double cas;
  double a=0;
  double max,x=-1;
  int i, maxI=-1;
  Set* m = SetCopy(s->m);
  Set* mInv = SetInvertCopy(s->m);
  double casKonec = UlohySumCas(prb, m)+UlohySumCas(prb, mInv);
  *str = calloc(mInv->count*PROBLEM_BACKTRACK, sizeof(char)); strcpy(*str, "");
  while(mInv->count>0) {
    cas = UlohySumCas(prb, m);
    max = -1;
    for(i=0; i<prb->n; i++) {
      if(type==1) x = UlohaIndexNaklad(prb,i,cas+prb->ulohy[i]->cas);
      if(type==2) x = (UlohaIndexNaklad(prb,i,cas+prb->ulohy[i]->cas)+UlohaIndexNaklad(prb,i,casKonec))/2;
      if(SetIsIn(mInv,i) && x>max) {
        max = x;
        maxI = i;
      }
    }
    a += max;
    SetRemove(mInv,maxI);
    SetAdd(m,maxI);
    sprintf(*str,"%s%i ", *str, maxI);
  }
  SetDestroy(m); SetDestroy(mInv);
  return a;
}

double ProblemAproximace1(Problem* prb, Stav* s, char** str) {
  return ProblemAproximace1a2(prb, s, str, 1);
}

double ProblemAproximace2(Problem* prb, Stav* s, char** str) {
  return ProblemAproximace1a2(prb, s, str, 2);
}

Aproximace aproximations[2] = {ProblemAproximace1, ProblemAproximace2};
