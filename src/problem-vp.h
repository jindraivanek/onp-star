#ifndef _PROBLEM_VP_H
#define _PROBLEM_VP_H

#include "graf.h"
#include "set.h"
#include "hasharray.h"

int nmOrder;
int problemHashMax;

typedef struct Problem {
  MGraf* g; //graf
  int n;
} Problem;


typedef struct {
  Set* m;
} Stav;

#include "problem-func.h"

double ProblemHeuristika0(Problem* prb, Stav* s);
double ProblemHeuristika1(Problem* prb, Stav* s);
double ProblemAproximace1(Problem* prb, Stav* s, char** str);
double ProblemAproximace2(Problem* prb, Stav* s, char** str);

extern Heur heuristics[1];
extern Aproximace aproximations[2];

#endif
