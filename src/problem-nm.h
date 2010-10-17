#ifndef _PROBLEM_NM_H
#define _PROBLEM_NM_H

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

//Problem ProblemCreateRandom(int n);
//Problem ProblemCreateFromFile(char* filename);
Problem* ProblemCreateFromFileEdges(char* filename);
//char* StavString(Problem prb, Stav* s);
//void StavPrint(Problem prb, Stav* s);
//Stav* ProblemStavCreate(Problem prb, Set* m);
//void ProblemStavDestroy(Stav* s);
//int ProblemStavCompare(void* v1, void* v2);
//int StavHash(Stav* s);
//Set* ZavisleVrcholy(Problem prb, Set* x);
//Stav* ProblemStart(Problem prb);
//int ProblemCil(Problem prb, Stav* s);
//SArray* ProblemNaslednici(Problem prb, Stav* s);
//double ProblemOceneni(Problem prb, Stav* s1,Stav* s2);
double ProblemHeuristika0(Problem* prb, Stav* s);
double ProblemHeuristika1(Problem* prb, Stav* s);
////double ProblemHeuristika2(Problem prb, Stav* s);
//double ProblemHeuristika(Problem prb, Stav* s);
double ProblemAproximace1(Problem* prb, Stav* s, char** str);
double ProblemAproximace2(Problem* prb, Stav* s, char** str);

extern Heur heuristics[1];
extern Aproximace aproximations[2];

#endif
