/* Deklarace funkci, ktere musi implementovat vsechny problemy
 */
#ifndef _PROBLEM_FUNC_H
#define	_PROBLEM_FUNC_H

Problem* ProblemCreateRandom(int n); //vytvoreni nahodneho zadani problemu

Problem* ProblemCreateFromFile(char* filename); //nacteni zadani ze souboru

void ProblemDestroy(Problem* prb); //smazani zadani

void ProblemPrint(Problem* prb); //citelny vypis zadani

int ProblemSave(Problem* prb, char* filename); //ulozeni zadani do souboru

char* StavString(Problem* prb, Stav* s); //retezec s citelnym zapisem stavu

void StavPrint(Problem* prb, Stav* s); //vypise stav

// vypise cele reseni (u problemu kde je cilovy stav take celym resenim) 
// nebo patricnou cast reseni (u problemu kde se musi projit cela cesta ze startovniho do ciloveho stavu)
// u takovych musi byt nastaveno PROBLEM_BACKTRACK na max. velikost teto casti reseni
char* StavReseni(Problem* prb, Stav* s, Stav* pred); 

int ProblemStavSave(Stav* s, char** pBuf); //ulozi stav do bufferu - pro ukladani do B-stromu

Stav* ProblemStavLoad(char** pBuf); //nacte stav z bufferu - pro nacitani z B-stromu

void ProblemStavDestroy(Stav* s); //vymaze stav

int ProblemStavCompare(void* v1, void* v2); //porovna dva stavy, podle jejich obsahu

int StavHash(Stav* s); //vrati hash stavu - pro pouziti v hrabich, take musi byt nastaven int problemHashMax .. maximalni hash

Stav* ProblemStart(Problem* prb); //vrati startovni stav

int ProblemCil(Problem* prb, Stav* s); //testuje zda je stav cilovy

SArray* ProblemNaslednici(Problem* prb, Stav* s); //vrati seznam nasledniku stavu

SArray* ProblemPredchudci(Problem* prb, Stav* s); //vrati seznam predchudcu stavu - potreba pri rekonstrukci reseni

double ProblemOceneni(Problem* prb, Stav* s1,Stav* s2); //vrati ohodnoceni hrany mezi stavy

typedef double (*Heur)(Problem*,Stav*); //zadefinovani funkce heuristiky

typedef double (*Aproximace)(Problem*,Stav*,char** str); //zadefinovani funkce aproximace - do str zapise vypis sveho reseni

#endif	/* _PROBLEM_FUNC_H */

