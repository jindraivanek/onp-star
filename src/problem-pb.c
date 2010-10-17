// Problem batohu

#include "common.h"
#include "problem-pb.h"

int PredmetCmp(void* v1, void* v2) {
  //porovnavaci funkce podle pomeru vaha/cena
  Predmet* p1 = (Predmet*) v1;
  Predmet* p2 = (Predmet*) v2;
  double x1 = p1->vaha/p1->cena; double x2 = p2->vaha/p2->cena;
  if(x1 < x2) return 1; //chceme sestupne razeni
  if(x1 == x2) return 0;
  return -1;
}

Predmet** PredmetyRandom(int n, double vahaMin, double vahaMax, double cenaMin, double cenaMax) {
  Predmet** pr = malloc(sizeof(Predmet*)*n);
  int i; for(i=0; i<n; i++) {
    Predmet* p = malloc(sizeof(Predmet));
    p->vaha = ((double) rand() / RAND_MAX) * (vahaMax-vahaMin) + vahaMin;
    p->cena = ((double) rand() / RAND_MAX) * (cenaMax-cenaMin) + cenaMin;
    pr[i]=p;
  }
  return pr;
}

double BatohVaha(Problem* prb, Stav* s) {
  double sum=0;
  int i; for(i=0; i<prb->n; i++) if(SetIsIn(s->batoh,i)) sum+=prb->predmety[i]->vaha;
  return sum;
}

double BatohCena(Problem* prb, Stav* s) {
  double sum=0;
  int i; for(i=0; i<prb->n; i++) if(SetIsIn(s->batoh,i)) sum+=prb->predmety[i]->cena;
  return sum;
}

void ProblemInit() {
  problemHashMax = (1 << 16) - 1;
}

Problem* ProblemCreateRandom(int n) {
  ProblemInit();
  Problem* prb = malloc(sizeof(Problem));
  prb->n=n;
  prb->predmety=PredmetyRandom(n, 1,10, 1,10);
  double sum=0; int i; for(i=0; i<prb->n; i++) sum+=prb->predmety[i]->vaha;
  prb->mez = ((double) rand() / RAND_MAX) * (0.5) + 0.25;
  return prb;
}

Problem* ProblemCreateFromFile(char* filename) {
  ProblemInit();
  FILE* f = fopen(filename,"r");
  Problem* prb = malloc(sizeof(Problem));
  if(fscanf(f,"%lf\n",&prb->mez)<1) goto error; //mez
  if(prb->mez<0) goto error; //zaporne ne
  if(fscanf(f,"%i\n",&prb->n)<1) goto error; //pocet predmetu
  if(prb->n<1) goto error; //musi byt kladny
  Predmet** p = malloc(sizeof(Predmet*)*prb->n);
  int i;
  for(i=0; i<prb->n; i++) {
    p[i] = malloc(sizeof(Predmet));
    if(fscanf(f,"%lf %lf\n", &(p[i]->vaha), &(p[i]->cena))<2) goto error;
    if(p[i]->vaha<0 || p[i]->cena<0) goto error; //zaporne ne
  }
  fclose(f);
  prb->predmety=p;
  return prb;
  error:
  fclose(f);
  return NULL;
}

void ProblemDestroy(Problem* prb) {
  int i; for(i=0; i<prb->n; i++) {
    free(prb->predmety[i]);
  }
  free(prb->predmety);
  free(prb);
}

void ProblemPrint(Problem* prb) {
  printf("Objem batohu: %.3f\n   objem   cena\n", prb->mez);
  int i; for(i=0; i<prb->n; i++)
    printf("%i: %.3f %.3f\n", i, prb->predmety[i]->vaha, prb->predmety[i]->cena);
}

int ProblemSave(Problem* prb, char* filename) {
  FILE* f = fopen(filename,"w");
  if(fprintf(f,"%lf\n",prb->mez)<0) goto error; //mez
  if(fprintf(f,"%i\n",prb->n)<0) goto error; //pocet predmetu
  int i;
  for(i=0; i<prb->n; i++) {
    if(fprintf(f,"%lf %lf\n", prb->predmety[i]->vaha, prb->predmety[i]->cena)<0) goto error;
  }
  fclose(f);
  return 1;
  error:
  fclose(f);
  return -1;
}

char* StavString(Problem* prb, Stav* s) {
  char *str = malloc(sizeof(char)*(s->batoh->size*3+2));
  sprintf(str,"(");
  int i; for(i=0; i<s->batoh->size; i++) {
    if(SetIsIn(s->batoh,i)) {
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
  char* st = StavString(prb, s);
  char* str = malloc(sizeof(char)*(strlen(st)+30));
  sprintf(str, "%s, cena batohu: %.3f",st , BatohCena(prb,s));
  return str;
}

Stav* ProblemStavCreate(Problem* prb, Set* m) {
  Stav* s = malloc(sizeof(Stav));
  s->batoh=m;
  return s;
}

int ProblemStavSave(Stav* s, char** pBuf) {
  SetSave(s->batoh, pBuf);
  return 1;
}

Stav* ProblemStavLoad(char** pBuf) {
  Stav* s = malloc(sizeof(Stav)); assert(s);
  s->batoh = SetLoad(pBuf); assert(s->batoh);
  return s;
}

void ProblemStavDestroy(Stav* s) {
  SetDestroy(s->batoh);
  free(s);
}

int ProblemStavCompare(void* v1, void* v2) {
  Stav* s1 = (Stav*) v1;
  Stav* s2 = (Stav*) v2;
  return SetCmp(s1->batoh, s2->batoh);
}

int StavHash(Stav* s) {
  return (s->batoh->data[0]) % problemHashMax;
}

Stav* ProblemStart(Problem* prb) {
  Set* m = SetCreate(prb->n);
  int i; for(i=0; i<prb->n; i++) SetAdd(m,i);
  Stav* s = ProblemStavCreate(prb, m);
  return s;
}

int ProblemCil(Problem* prb, Stav* s) {
  if(BatohVaha(prb,s)<=prb->mez) return 1;
  else return 0;
}

SArray* ProblemNaslednici(Problem* prb, Stav* s) {
  SArray* p = SArrayCreate(ProblemStavCompare);
  Stav* n;
  int i; for(i=0; i<prb->n; i++) {
    if(SetIsIn(s->batoh,i)) {
      Set* m = SetCopy(s->batoh);
      SetRemove(m,i);
      n = ProblemStavCreate(prb, m);
      SArrayAdd(p, (void*) n);
    }
  }
  return p;
}

double ProblemOceneni(Problem* prb, Stav* s1, Stav* s2) {
  int x=-1;
  int i; for(i=0; i<prb->n; i++) if(SetIsIn(s1->batoh,i) && !SetIsIn(s2->batoh,i)) x=i;
  return prb->predmety[x]->cena;
}

double ProblemHeuristika1(Problem* prb, Stav* s) {
  //s rozdelenim posledniho predmetu
  SArray* sp = SArrayCreate(PredmetCmp);
  int i; for(i=0; i<prb->n; i++) if(SetIsIn(s->batoh,i)) SArrayAdd(sp, (void*)prb->predmety[i]);
  double h=0; double v=BatohVaha(prb,s);
  if(v<=prb->mez) return 0;
  for(i=0; i<sp->size; i++) {
    if(v-((Predmet*)sp->data[i])->vaha < prb->mez) break;
    h+=((Predmet*)sp->data[i])->cena;
    v-=((Predmet*)sp->data[i])->vaha;
  }
  double x = (v-prb->mez)/((Predmet*)sp->data[i])->vaha;
  h+=((Predmet*)sp->data[i])->cena*x;
  SArrayDestroy(sp);
  return h;
}

Heur heuristics[1] = {ProblemHeuristika1};

double ProblemAproximace1(Problem* prb, Stav* s, char** str) {
  SArray* sp = SArrayCreate(PredmetCmp);
  int i; for(i=0; i<prb->n; i++) if(SetIsIn(s->batoh,i)) SArrayAdd(sp, (void*)prb->predmety[i]);
  Set* b = SetCopy(s->batoh);
  double h=0.0; double v=BatohVaha(prb,s);
  if(v<=prb->mez) return 0;
  for(i=0; i<sp->size; i++) {
    h+=((Predmet*)sp->data[i])->cena;
    v-=((Predmet*)sp->data[i])->vaha;
    int j; for(j=0; j<prb->n; j++) if(prb->predmety[j]==sp->data[i]) break; //zjisteni indexu predmetu
    SetRemove(b, j);
    if(v<=prb->mez) break;
  }
  SArrayDestroy(sp);
  Stav* r = ProblemStavCreate(prb, b);
  *str = StavReseni(prb, r, NULL);
  ProblemStavDestroy(r);
  return h;
}

Aproximace aproximations[1] = {ProblemAproximace1};
