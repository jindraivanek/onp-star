#ifndef _PROBLEM_RU_H
#define _PROBLEM_RU_H

#include "set.h"
#include "hasharray.h"

#define PROBLEM_BACKTRACK 40

int problemHashMax;

typedef struct Uloha {
  double cas;
  double dokdy; //do kdy to ma byt hotovo
  double penale; //naklad za to ze se to nestihlo
  double koef; //koeficient rustu penale
} Uloha;

typedef struct Problem {
  int n;
  Uloha** ulohy;
} Problem;

typedef struct Stav {
  Set* m;
} Stav;

void UlohyPrint(Problem* prb);
#include "problem-func.h"


double ProblemHeuristika1(Problem* prb, Stav* s);
double ProblemHeuristika2(Problem* prb, Stav* s);
double ProblemHeuristika3(Problem* prb, Stav* s);
double ProblemAproximace1(Problem* prb, Stav* s, char** str);
double ProblemAproximace2(Problem* prb, Stav* s, char** str);

extern Heur heuristics[3];
extern Aproximace aproximations[2];

#endif
