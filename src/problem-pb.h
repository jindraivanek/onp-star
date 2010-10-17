#ifndef _PROBLEM_PB_H
#define _PROBLEM_PB_H

#include "set.h"
#include "hasharray.h"

int problemHashMax;

typedef struct Predmet {
  double vaha;
  double cena;
} Predmet;

typedef struct Problem {
  double mez;
  Predmet** predmety;
  int n; //pocet predmetu
} Problem;

typedef struct Stav {
  Set* batoh;
} Stav;

#include "problem-func.h"

double ProblemHeuristika1(Problem* prb, Stav* s);
double ProblemAproximace1(Problem* prb, Stav* s, char** str);

extern Heur heuristics[1];
extern Aproximace aproximations[1];
#endif
