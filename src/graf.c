#include "common.h"
#include "graf.h"
#include "set.h"
#include "hasharray.h"


MGraf* MGrafCreate(int order, double** matice) {
  MGraf* g = malloc(sizeof(MGraf));
  g->n = order;
  g->mat = matice;
  return g;
}

//syntaxe: na prvnim radku pocet vrcholu, nasleduje matice
//sousede vrcholu na jednom radku, oddelene mezerou, -1..hrana neexistuje
MGraf* MGrafCreateFromFile(char* filename) {
  FILE* f = fopen(filename,"r"); assertstr(f,"Cannot open file!");
  MGraf* g = malloc(sizeof(MGraf));
  if(fscanf(f,"%i\n",&g->n)<1) goto error;
  if(g->n<1) goto error; //musi byt kladny
  int i,j;
  g->mat = malloc(sizeof(double*)*g->n);
  for(i=0; i<g->n; i++) g->mat[i] = malloc(sizeof(double)*g->n);
  for(i=0; i<g->n; i++) {
    for(j=0; j<g->n; j++) {
      if(fscanf(f,"%lf ",&g->mat[i][j])<1) goto error;
      if(i==j) g->mat[i][i]=-1.0;
    }
  }
  fclose(f);
  return g;
  error:
  fclose(f);
  return NULL;
}

//nahodny uplny orientovany (sym=0) nebo neorientovanys(sym=1) graf 
MGraf* MGrafCreateRandom(int order, double min, double max, int sym) {
  double** m = malloc(sizeof(double*)*order);
  int i,j,k;
  for(i=0; i<order; i++) m[i] = malloc(sizeof(double)*order);
  MGraf* g = MGrafCreate(order,m);
  for(i=0; i<order; i++) {
    if(sym) k=i; else k=0;
    for(j=0; j<order; j++) {
      if(i==j) g->mat[i][j]=-1;
      else {
        double x = ((double)rand()/RAND_MAX) * (max-min) + min;
        g->mat[i][j] = x; 
        if(sym) g->mat[j][i] = x;
      }
    }
  }
  return g;
}

void MGrafDestroy(MGraf* g) {
  int i; for(i=0; i<g->n; i++) free(g->mat[i]);
  free(g->mat);
  free(g);
}

int MGrafSave(MGraf* g, char* filename) {
  int i,j;
  FILE* f = fopen(filename, "w");
  if(fprintf(f,"%i\n",g->n)<0) goto error;
  for(i=0; i<g->n; i++) {
    for(j=0; j<g->n; j++) if(fprintf(f,"%lf ",g->mat[i][j])<0) goto error;
    if(fprintf(f,"\n")<0) goto error;
  }
  fclose(f);
  return 1;
  error:
  fclose(f);
  return -1;
}

void MGrafPrint(MGraf* g) {
  int i,j;
  printf("   | "); for(i=0; i<g->n; i++) printf("%2i   | ",i); printf("\n");
  for(i=0; i<g->n; i++) {
    printf("%2i | ",i);
    for(j=0; j<g->n; j++) if(g->mat[i][j]<0.0) printf("  x    "); else printf("%2.2f   ",g->mat[i][j]);
    printf("\n");
  }
}

