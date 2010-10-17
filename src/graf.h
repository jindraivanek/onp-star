/* Graf reprezentovany pomoci matice
 */ 
#ifndef _GRAF_H
#define _GRAF_H

#include "set.h"

typedef struct MGraf {
  int n;
  double** mat;
} MGraf; //graf zadany pomoci matice

MGraf* MGrafCreate(int order, double** matice);
MGraf* MGrafCreateRandom(int order, double min, double max, int sym);
void MGrafDestroy(MGraf* g);
MGraf* MGrafCreateFromFile(char* filename);
int MGrafSave(MGraf* g, char* filename);
void MGrafPrint(MGraf* g);

#endif
