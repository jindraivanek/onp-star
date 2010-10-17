#include "zahradnik.h"

double min(double a, double b) {
  if(b<a) return b;
  else return a;
}

void LogExpand(Problem* prb, AStav* s) {
  printf("EXPAND - ");
  StavPrint(prb, s->stav);
  printf(" - odhad %.1f, cena %.1f", s->odhad, s->cena);
  printf("\n");
}

void LogFileExpand(FILE* f, Problem* prb, AStav* s) {
  char* str = StavString(prb, s->stav);
  fprintf(f,"%s\n",str);
  free(str);
}

void LogNaslednik(Problem* prb, AStav* n, char* s) {
  printf("%s: ", s);
  StavPrint(prb, n->stav);
  printf(" - odhad %.1f, cena %.1f", n->odhad, n->cena);
  printf("\n");
}

void LogHeap(Heap *h) {
  printf("HEAP: (");
  int i; for(i=0; i<h->size; i++) printf("%.0f/%.0f ", ((AStav*)h->array[i])->odhad, ((AStav*)h->array[i])->cena);
  printf(")\n");
}


AStav* AStavCreate(Stav* s) {
  AStav* a = malloc(sizeof(AStav));
  a->stav = s;
  a->cena = 0;
  a->odhad = 0;
  a->heapIndex = -2; //-2 .. jeste nebyl v halde, -1 .. byl z haldy odstranen (== uzavreny stav)
  return a;
}

char* AStavSave(AStav* a, int size) {
  char* buf = malloc(size);
  char* x = buf;
  bufwriteone(a->cena,x);
  bufwriteone(a->odhad,x);
  ProblemStavSave(a->stav, &x);
  return buf;
}

void AStavSaveBTree(AStav* a, BTree* bt) {
  char* buf = AStavSave(a, bt->dataSize);
  BTreeInsert(bt, &a->odhad, buf);
  free(buf);
}

int AStavSize(AStav* a) {
  //TODO: pouze docasne, blby zpusob zjistovani velikosti
  char* buf = malloc(1024);
  char* x=buf;
  bufwriteone(a->cena,x);
  bufwriteone(a->odhad,x);
  ProblemStavSave(a->stav, &x);
  free(buf);
  return x-buf;
}

AStav* AStavLoad(char* buf, int size) {
  AStav* a = malloc(sizeof(AStav)); assert(a);
  bufreadone(a->cena,buf);
  bufreadone(a->odhad,buf);
  a->heapIndex = -2;
  a->stav = ProblemStavLoad(&buf); assert(a->stav);
  return a;
}

void AStavDestroy(AStav* a) {
  if(a->stav) ProblemStavDestroy(a->stav);
  free(a);
}

int AStavCompare(void* v1, void* v2) {
  //nejprve radit podle odhadu vzestupne, pak podle ceny sestupne
  //to ze ve vypisu cena neni serazena je tim ze se do haldy v prubehu pridavaji nove prvky
  AStav* s1 = (AStav*) v1;
  AStav* s2 = (AStav*) v2;
/// TODO: dovymyslet porovnavani doubleu
  if(doubleeq(s1->odhad, s2->odhad)) {
    double x1 = s1->odhad - s1->cena;
    double x2 = s2->odhad - s2->cena;
    if(x1 > x2) return 1;
    if(x1 < x2) return -1;
    //return 0;
  }
  if(s1->odhad > s2->odhad) return 1;
  if(s1->odhad < s2->odhad) return -1;
  return 0;
}

int StavCompare(void* v1, void* v2) {
  AStav* s1 = (AStav*) v1;
  AStav* s2 = (AStav*) v2;
  int r = ProblemStavCompare((void*) (s1->stav), (void*) (s2->stav));
  return r;
}

void AStavStoreHeapIndex(void* s, int i) {
  ((AStav*)s)->heapIndex=i;
}

int AStavHash(void* s) {
  return StavHash(((AStav*)s)->stav);
}

int OdhadCompare(void* x1, void* x2) {
  double o1 = *(double*) x1;
  double o2 = *(double*) x2;
  if(fabs(o1 - o2) < 0.0001) return 0;
  if(o1 > o2) return 1;
  if(o1 < o2) return -1;
  return 0;
}

char* PrintReseniRekonstrukce(Zahradnik* z, Problem* prb, AStav* final) {
   //DONE: predelat, cele ukladani stavu mozna netreba? Stejne je musim mit v pameti.. Ovsem muzu mazat stavy s mensi cenou nez ma vrchol haldy.. Takze ukladani ponechat
   //   printf("REKONSTRUKCE RESENI\n");
  #ifndef PROBLEM_BACKTRACK
    return StavReseni(prb, final->stav, NULL);
  #else
   AStav* n = NULL;
   AStav* p = NULL;
   AStav* f = final;
   SArray* pred;
   int i, j=1, x;
   char** s = malloc(sizeof(char*)*prb->n);
   //s[0] = StavReseni(prb, final->stav, NULL);
   for(j=0; j<prb->n; j++) {
     pred = ProblemPredchudci(prb, f->stav);
     if(pred == NULL) break;
     for(i = 0, x = -1; i < pred->size; i++) {
       n = AStavCreate(pred->data[i]);
       p = (AStav*) HashArrayFind(z->all, (void*) n);
       AStavDestroy(n);
       if(p != NULL &&
               doubleeq(f->cena, p->cena + ProblemOceneni(prb, p->stav, f->stav))) {
         x = i;
         s[j] = StavReseni(prb, f->stav, p->stav);
         f = p;
         for (i = i + 1; i < pred->size; i++) ProblemStavDestroy(pred->data[i]); //smazani zbylych predchudcu
         break;
       }
     }
     SArrayDestroy(pred);

     assert(x != -1); //nejaky uzavreny stav chybi
   }
   char* str = calloc(prb->n*PROBLEM_BACKTRACK, sizeof(char)); strcpy(str, "");
   for(i=j-1; i>=0; i--) { strcat(str,s[i]); free(s[i]); }
   free(s);
   return str;
   #endif
 }

//hladove hleda nejake reseni (tj. horni odhad)
double AStavAprox(Zahradnik* z, Heur heur, AStav* a, char** str) {
  SArray* nasl; int i;
  double x = 0.0, min, h;
  Stav* n; Stav* v=NULL;
  Stav* s = a->stav;
  #ifdef PROBLEM_BACKTRACK
    *str = PrintReseniRekonstrukce(z, z->prb, a);
    strcat(*str, "|"); //DEBUG
  #endif
  while(!ProblemCil(z->prb, s)) {
    min = INT_MAX;
    nasl = ProblemNaslednici(z->prb, s);
    //hledame stav s nejmensi heuristikou
    for(i=0; i<nasl->size; i++) {
      n = (Stav*)nasl->data[i];
      h = ProblemOceneni(z->prb, s, n) + heur(z->prb, n);
      if(h<min) {
        min=h;
        if(v) ProblemStavDestroy(v);
        v=n;
      } else ProblemStavDestroy(n);
    }
    SArrayDestroy(nasl);
    assert(v);
    x+=ProblemOceneni(z->prb, s, v);
    #ifdef PROBLEM_BACKTRACK
      char* part = StavReseni(z->prb, v, s);
      strcat(*str, part);
      if(part) free(part);
    #endif
    if(s!=a->stav) ProblemStavDestroy(s);
    s = v; v = NULL;
  }
  #ifndef PROBLEM_BACKTRACK
    *str = StavReseni(z->prb, s, NULL);
  #endif
  ProblemStavDestroy(s);
  return x;
}

Zahradnik* ZahradnikCreate(Problem* prb, Options* opt, Heur heur, Aproximace aproximace) {
  Zahradnik* z = malloc(sizeof(Zahradnik)); assert(z);
  z->prb = prb;
  z->verbose = opt->verbose;
  if(heur==NULL) z->heur = ProblemHeuristika0;
  else z->heur = heur;
  z->aproximace = aproximace;
  z->all = HashArrayCreate(problemHashMax, AStavHash, StavCompare); assert(z->all);
  z->open = HeapCreate(AStavCompare, AStavStoreHeapIndex, 1); assert(z->open);
  z->odhad = 0.0; // dolni odhad
  z->aprox = (unsigned int)~0; // horni odhad
  z->aproxStr = NULL;
  AStav* start = AStavCreate(ProblemStart(prb));
  z->odklad = BTreeCreate(ZAHRADNIK_FILENAME, OdhadCompare, 1024, sizeof(double), AStavSize(start));
  AStavDestroy(start);
  z->limitMax = opt->heapLimitMax; z->limitMin = opt->heapLimitMin;
  z->limitOdhad = z->aprox;
  return z;
}

//vrati prvek z vrcholu haldy bez jeho vyrazeni z haldy
AStav* ZahradnikTop(Zahradnik* z) {
  if(z->open->size>0) return (AStav*) z->open->array[0];
  else return NULL;
}

void ZahradnikPrintStatus(Zahradnik* z) {
  printf("STATUS - expanzi: %i, open: %i, odklad(%.3f): %i, pamet: %ikB, odhad: %.3f, aprox(%.4f): %.3f\n", z->all->size-z->open->size, z->open->size, z->limitOdhad, BTreeGetCount(z->odklad), mallinfo().uordblks/1024, z->odhad, z->aprox/z->odhad, z->aprox);
}

void ZahradnikFilter(Zahradnik* z, double aprox, int typ) {
  //typ=0 ... filtruje odhad >= aprox
  //typ=1 ... filtruje odhad < aprox, pouze uzavrene stavy
  //ZahradnikCheck(z);
  int i,j;
  AStav* s;
  for(i=0; i<z->all->all; i++) {
    for(j=0; z->all->array[i]!=NULL && j<z->all->array[i]->size; j++) { //Pri smazani posedniho prvku v SArray se all->array[i] muze nastavit na NULL
      s = (AStav*) z->all->array[i]->data[j];
      if(
        (typ==0 && s->odhad >= aprox) ||
        (typ==1 && s->heapIndex<0 && s->odhad < aprox) ) {
        //printf("Filter:\n");
//          LogExpand(prb, open->array[s->heapIndex]);
//        printf("%i,%i: %i/%i,%X=%X\n", i,j,s->heapIndex, z->open->size, z->open->array[s->heapIndex], s);
        if(z->verbose>=4) LogNaslednik(z->prb, s, "DELETE (FILTER):");
        if(z->open && s->heapIndex>=0) assert(HeapDeleteIndex(z->open, s->heapIndex)!=-1);
        if(z->all) assert(HashArrayDeleteIndex(z->all, i, j)!=NULL);
        AStavDestroy(s);
        j--;
      }
    }
  }
}

void ZahradnikOdloz(Zahradnik* z) {
  if(z->verbose>=2) printf("ZAHRADNIK OREZANI\n");
  //printf("ZAHRADNIK-odkladaci limit: %f\n", z->limitOdhad);
  //ZahradnikFilter(z, ZahradnikTop(z)->odhad, 1, NULL); //blbost, smazane stavy se generuji znova s horsim odhadem/cenou
  Heap* h = HeapCreate(z->open->hcf, z->open->shif, 1);
  int i;
  //for(i=0; i<z->open->size; i++) assert(((AStav*)z->open->array[i])->stav->m);
  for(i=0; i<z->limitMin-1; i++) HeapPush(h, HeapPop(z->open));
  AStav* x = (AStav*) HeapPop(z->open); z->limitOdhad = x->odhad; HeapPush(h,(void*) x);

  //ZahradnikCheck(z);
  for(i=0; i<z->open->size; i++) {
    //ZahradnikCheck(z);
    x = (AStav*) z->open->array[i];
    AStavSaveBTree(x, z->odklad);
    HashArrayFindDelete(z->all, (void*) x);
    if(z->verbose>=3) LogNaslednik(z->prb, x, "ODLOZENO:");
    AStavDestroy(x);
  }

  HeapDestroy(z->open);
  z->open = h;
  //ZahradnikCheck(z);
}

void ZahradnikAdd(Zahradnik* z, AStav* a) {
//  char* buf  = AStavSave(a, z->odklad->dataSize);
//  AStav* x = AStavLoad(buf, z->odklad->dataSize);
//  BTree* test = BTreeCreate("btree-test.db", OdhadCompare, 1024, z->odklad->keySize, z->odklad->dataSize);
//  AStavSaveBTree(a, test);
//  AStav* x = AStavLoad(BTreeSearch(test, &a->odhad), test->dataSize);
//  AStav* x = AStavLoad(BTreePopN(test, 1)[0], test->dataSize);
//  assert(StavCompare(a,x)==0);

  if(z->open->size > z->limitMax) {
    ZahradnikOdloz(z);
    ZahradnikPrintStatus(z);
  }
  AStav* p = HashArrayFind(z->all, (void*) a);
  if(p==NULL) { //tento stav je poprve
    if(a->odhad >= z->aprox) {
      if(z->verbose>=4) LogNaslednik(z->prb, a, "NOVY NASLEDNIK NEZAJEM");
      AStavDestroy(a);
      return;
    }
    if(a->odhad > z->limitOdhad) {
      AStavSaveBTree(a, z->odklad);
      if(z->verbose>=3) LogNaslednik(z->prb, a, "NOVY NASLEDNIK ODLOZENO");
      AStavDestroy(a);
      return;
    } else {
      HashArrayAdd(z->all, (void*) a);
      HeapPush(z->open, (void*) a);
      if(z->verbose>=3) LogNaslednik(z->prb, a, "NOVY NASLEDNIK");
      return;
    }
  } else {

    if(p->heapIndex>-1 && a->cena < p->cena) { // neni uzavrene a cena se zlepsi
      p->cena = a->cena;
      p->odhad = a->odhad;
      HeapDecreaseKey(z->open, p->heapIndex);
      if(z->verbose>=3) LogNaslednik(z->prb, p, "ZNAMY NASLEDNIK DECREASE");
      AStavDestroy(a);
      return;
    } else {
      if(z->verbose>=3) LogNaslednik(z->prb, p, "ZNAMY NASLEDNIK");
      AStavDestroy(a);
      return;
    }
  }
  //ZahradnikCheck(z);
}

/*
void ZahradnikCheck(Zahradnik *z) {
  int i,j,x;
  AStav* s=NULL;
  AStav* n;

  FILE* tmp = fopen(ZAHRADNIK_FILENAME_TMP, "w+");
  for(i=0,x=0; i<z->all->all; i++) {
    for(j=0; z->all->array[i]!=NULL && j<z->all->array[i]->size; j++,x++) {
      s = (AStav*) z->all->array[i]->data[j];
      fseek(tmp, SEEK_SET, 0);
      //AStavSave(s, tmp);
      fflush(tmp);
      fseek(tmp, SEEK_SET, 0);
      //n=AStavLoad(tmp);
      if(ProblemStavCompare(s->stav,n->stav)!=0)
        printf("CHECKERROR\n");
    }
  }
  fclose(tmp);

  printf("HEAPCHECK:\n");
  for(i=0; i<z->open->size; i++) {
    printf("%i=%i, %X\n", i, ((AStav*)z->open->array[i])->heapIndex, (uint)z->open->array[i]);
  }
//  printf("HRABECHECK:\n");
//  for(i=0,x=0; i<z->all->all; i++) {
//    for(j=0; z->all->array[i]!=NULL && j<z->all->array[i]->size; j++,x++) {
//      s = (AStav*) z->all->array[i]->data[j];
//      if(s->heapIndex>-1)
//        printf("%i,%i: %i/%i,%x=%x\n", i,j,s->heapIndex, z->open->size, z->open->array[s->heapIndex], s);
//    }
//  }
//  printf("COUNT: %i=%i\n", x, z->all->size);
}
*/
//Nacte stavy z odkladaciho souboru tak, ze v halde bude (skoro) limitMin prvku (nejmensich v ramci odkladaciho souboru). Nastavi novy limitOdhad.
void ZahradnikLoad(Zahradnik* z) {
  if(z->open->size >= z->limitMin) return;
  int count=BTreeGetCount(z->odklad);
  if(count==0) return;
  //ZahradnikCheck(z);

  printf("ZAHRADNIK LOAD\n");
  ZahradnikPrintStatus(z);

  AStav* n;
  double nLimit;
  AStav* x;
  int i,c;
  double ubytek=1.0;
  while(z->open->size < z->limitMin && count>0) {
    c=(z->limitMin-z->open->size)*ubytek; if(c>count) c=count;
    void** r = BTreePopN(z->odklad, c);
    for(i=0; i<c; i++) {
      if(r[i]==NULL) break;
      n = AStavLoad(r[i], z->odklad->dataSize);
      free(r[i]);
      if(n && n->odhad <= z->aprox) {
        x = (AStav*) HashArrayFind(z->all, n);
        if(x) {
          if(n->odhad < x->odhad) {
            x->cena = n->cena;
            x->odhad = n->odhad;
            HeapDecreaseKey(z->open, x->heapIndex);
            if(z->verbose>=3) LogNaslednik(z->prb, n, "LOAD STAV DECREASE:");
          }
          AStavDestroy(n);
        }
        else {
          HashArrayAdd(z->all, (void*) n);
          HeapPush(z->open, (void*) n);
          if(z->verbose>=3) LogNaslednik(z->prb, n, "LOAD STAV:");
          nLimit = n->odhad; n=NULL;
        }
      }
    }
    free(r);
    count-=i;
  }
  z->limitOdhad=nLimit;
  ZahradnikPrintStatus(z);
}


AStav* ZahradnikPop(Zahradnik* z) {
  AStav* a = (AStav*) HeapPop(z->open);
  z->odhad = a->odhad;
  if(z->open->size <= 0) ZahradnikLoad(z);
  return a;
}

int ZahradnikAproximace(Zahradnik* z, AStav* s) {
  int ret=1;
  char* str;
  double aproxTmp;
  if(z->aproximace!=NULL) {
    #ifdef PROBLEM_BACKTRACK
    str = PrintReseniRekonstrukce(z, z->prb, s);
    char* tmp;
    aproxTmp = s->cena + z->aproximace(z->prb, s->stav, &tmp);
    strcat(str,tmp);
    free(tmp);
    #else
    aproxTmp = s->cena + z->aproximace(z->prb, s->stav, &str);
    #endif
  }
  else aproxTmp = s->cena + AStavAprox(z, z->heur, s, &str);
  if(aproxTmp < z->aprox*1.1 && z->aproximace!=NULL) {
    char* str2 = malloc(sizeof(char)*(strlen(str)+1)); strcpy(str2, str);
    double aproxTmp2 = s->cena + AStavAprox(z, z->heur, s, &str2);
    if(aproxTmp2 < aproxTmp) {
      aproxTmp = aproxTmp2;
      free(str); str=str2;
      if(aproxTmp < z->aprox) printf("VYLEPSENA APROXIMACE\n");
    }
  }
  if(aproxTmp < z->aprox) {
    z->aprox = aproxTmp;
    if(z->aproxStr) free(z->aproxStr);
    z->aproxStr = str;
    ZahradnikPrintStatus(z);
    if(z->verbose>=1) printf("Aproximace: %.3f (%s)\n", z->aprox, z->aproxStr);
    if(doubleeq(z->aprox, s->odhad)) ret=-1; //tzn. s se vyfiltruje
    ZahradnikFilter(z, z->aprox, 0);
    //TestCheckConsistency(open, all);
    ZahradnikPrintStatus(z);
  }
  else free(str);
  return ret;
}

void ZahradnikDestroy(Zahradnik* z) {
  int i,j;
  for(i=0; i<z->all->all; i++) {
    for(j=0; z->all->array[i]!=NULL && j<z->all->array[i]->size; j++) //Pri smazani posledniho prvku v SArray se all->array[i] muze nastavit na NULL
      AStavDestroy((AStav*) z->all->array[i]->data[j]);
  }
  if(z->open) HeapDestroy(z->open);
  if(z->all) HashArrayDestroy(z->all);
  if(z->odklad) BTreeDestroy(z->odklad);
  if(z->aproxStr) free(z->aproxStr);
  free(z);
}

