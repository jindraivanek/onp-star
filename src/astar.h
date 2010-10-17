#include "common.h"
#include "problem.h"
#include "zahradnik.h"

void AStar(Problem* prb, Heur heur, Aproximace aproximace, Options* opt) {
  int count = 0;
  Zahradnik* z = ZahradnikCreate(prb, opt, heur, aproximace);
  AStav* start = AStavCreate(ProblemStart(prb));
  start->odhad = z->heur(prb, start->stav);
  ZahradnikAdd(z, start);
  AStav* s = NULL;
  AStav* n;
  SArray* nasl;
  int aproxRes = -1; //priznak jestli je reseni aproximace nebo ne
  int i;

  while (z->open->size > 0) {
    if (opt->verbose >= 2) ZahradnikPrintStatus(z);
    assert(z->open->size && ((AStav*) z->open->array[0])->heapIndex == 0);
    s = ZahradnikPop(z); // s->heapIndex je nyni -1
    count++;
    if (opt->verbose == 1 && count % 1000 == 0) {
      ZahradnikPrintStatus(z);
      LogExpand(prb, s);
    }
    if (opt->verbose >= 2) LogExpand(prb, s);
    if (ProblemCil(prb, s->stav) || doubleeq(s->odhad, z->aprox)) break;
    if(ZahradnikAproximace(z, s)==-1) { aproxRes=1; break; }
    nasl = ProblemNaslednici(prb, s->stav);
    for (i = 0; i < nasl->size; i++) {
      n = AStavCreate((Stav*) nasl->data[i]);
      n->cena = s->cena + ProblemOceneni(z->prb, s->stav, n->stav);
      ZahradnikAdd(z, n);

    }
    SArrayDestroy(nasl);
    fflush(stdout);
  }
  printf("HURA!!!\n");
  printf("Pocet expanzi: %i\n", count);
  if(aproxRes==1 || z->open->size==0) {
    printf("Reseni (APROXIMACE): %s, cena: %.1f\n", z->aproxStr, z->aprox);
    z->odhad=z->aprox;
  } else {
    char* res = PrintReseniRekonstrukce(z, prb, s);
    printf("Reseni: %s, cena: %.1f\n", res, s->cena);
    free(res);
  }
  ZahradnikPrintStatus(z);
  ZahradnikDestroy(z);
}
