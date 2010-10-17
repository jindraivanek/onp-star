#ifndef _OPTIONS_H
#define	_OPTIONS_H

#include "common.h"

typedef struct Options {
  int size;
  char* filename;
  uint verbose;
  uint heapLimitMax, heapLimitMin;
  uint btreeMemMax;
  uint heur; //cislo heuristiky
  uint heurs; //pocet heuristik
  uint aprox; //cislo aproximace
  uint aproxs; //pocet aproximaci
} Options;


#endif	/* _OPTIONS_H */

