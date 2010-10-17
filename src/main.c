//#undef NDEBUG //pri debugovani odkomentovat
#include <unistd.h>
#include "common.h"
#include "problem.h"
#include "options.h"
#include "astar.h"

void printUsage(char* s, Options* o) {
  printf("USAGE: %s [-f filename] [-s size] [-h heuristika] [-a aproximace] [-v verbose] [-u heapLimitMax] [-d heapLimitMin] [-m btreeMemMax]\n",s);
  printf("filename .. soubor se zadanim\n");
  printf("size .. velikost nahodne generovaneho zadani, pokud neni zadane filename, musi byt kladne\n");
  printf("heuristika .. pouzita heuristika, pripustne volby: 0 az %i\n", o->heurs);
  printf("aproximace .. pouzita aproximace, pripustne volby: 0 az %i\n", o->aproxs);
  printf("verbose .. upovidanost, pripustne volby: 0 az 4\n");
  printf("heapLimitMin .. minimalni velikost haldy, kladne\n");
  printf("heapLimitMax .. maximalni velikost haldy, kladne, musi platit heapLimitMin <= heapLimitMax\n");
  printf("btreeMemMax .. maximalni velikost cache pro B-strom v MB\n");
  exit(1);
}

//nastavi default hodnoty
Options* OptionsCreate() {
  Options* o = malloc(sizeof(Options));
  o->size=20;
  o->verbose=1;
  o->filename=NULL;
  o->heapLimitMax=100000;
  o->heapLimitMin=50000;
  o->btreeMemMax=100; //100MB
  //protoze je heuristics staticke pole, toto vrati jeho velikost
  o->heurs=sizeof(heuristics)/sizeof(heuristics[0]);
  o->heur=o->heurs;
  o->aproxs=sizeof(aproximations)/sizeof(aproximations[0]);
  o->aprox=o->aproxs;
  return o;
}

int main(int argc, char** argv) {
  srand(time(0));
  Options* opt = OptionsCreate();

  int c;
  while ((c = getopt (argc, argv, "f:s:v:u:d:m:h:a:")) != -1) {
    switch(c) {
      case 'f':
        opt->filename = malloc(strlen(optarg)+1);
        if(sscanf(optarg, "%s", opt->filename)==0) printUsage(argv[0],opt);
        break;
      case 's':
        if(sscanf(optarg, "%i", &opt->size)==0) printUsage(argv[0],opt);
        break;
      case 'v':
        if(sscanf(optarg, "%i", &opt->verbose)==0) printUsage(argv[0],opt);
        break;
      case 'u':
        if(sscanf(optarg, "%i", &opt->heapLimitMax)==0) printUsage(argv[0],opt);
        break;
      case 'd':
        if(sscanf(optarg, "%i", &opt->heapLimitMin)==0) printUsage(argv[0],opt);
        break;
      case 'm':
        if(sscanf(optarg, "%i", &opt->btreeMemMax)==0) printUsage(argv[0],opt);
        break;
      case 'h':
        if(sscanf(optarg, "%i", &opt->heur)==0) printUsage(argv[0],opt);
        break;
      case 'a':
        if(sscanf(optarg, "%i", &opt->aprox)==0) printUsage(argv[0],opt);
        break;
      case '?':
        printUsage(argv[0],opt);
        abort();
        break;
      default:
        abort();
    }
    if(opt->size<1 || opt->verbose<0 || opt->verbose>4 || opt->heapLimitMax<0 || opt->heapLimitMin<0 || opt->heapLimitMax < opt->heapLimitMin || opt->btreeMemMax < 1 || opt->heur < 0 || opt->heur > opt->heurs || opt->aprox < 0 || opt->aprox > opt->aproxs) {
      printUsage(argv[0],opt);
      abort();
    } 
  }

  Heur h;
  if(opt->heur==0) h=NULL;
  else h=heuristics[opt->heur-1];
  Aproximace a;
  if(opt->aprox==0) a=NULL;
  else a=aproximations[opt->aprox-1];

  Problem* p;
  if(opt->filename) {
    p = ProblemCreateFromFile(opt->filename);
    exitstr(p!=NULL, "Chyba pri cteni ze souboru!");
  }
  else p = ProblemCreateRandom(opt->size);
  ProblemPrint(p);
  exitstr(ProblemSave(p, "../examples/last"), "Chyba pri zapisu do souboru!");
  AStar(p,h,a,opt);
  ProblemDestroy(p);
  return 0;
}
