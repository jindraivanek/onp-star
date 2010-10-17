#ifndef _PROBLEM_TSP_H
#define _PROBLEM_TSP_H

#include <stdio.h>

#include "graf.h"
#include "set.h"
#include "hasharray.h"

//pokud je definovan PROBLEM_BACKTRACK, znamena to ze reseni se musi rekonstruovat, tj. projit celou cestu od konecneho stavu do startu. Cislo udava velikost casti retezce pouzivane pro vypis reseni. Kvuli rekonstrukci je dale potrebovat implementovat funkci ProblemPredchudci a StavReseniPart
#define PROBLEM_BACKTRACK 4

int tspOrder;
int problemHashMax;

typedef struct Problem {
  MGraf* g; //graf
  int n; //velikost problemu
} Problem;

typedef struct Stav {
  Set* m;
  int v; //koncovy vrchol
} Stav;

#include "problem-func.h"

double ProblemHeuristika0(Problem* prb, Stav* s);
double ProblemHeuristika2(Problem* prb, Stav* s);
double ProblemHeuristika3(Problem* prb, Stav* s);
double ProblemHeuristika4(Problem* prb, Stav* s);
double ProblemAproximace1(Problem* prb, Stav* s, char** str);

extern Heur heuristics[3];
extern Aproximace aproximations[1];

#endif
