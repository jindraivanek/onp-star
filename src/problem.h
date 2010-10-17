//pouze nacteme prislusny hlavickovy soubor podle makra
#ifndef _PROBLEM_H
#define	_PROBLEM_H

#ifdef PROBLEM_TSP
#include "problem-tsp.h"
#endif
#ifdef PROBLEM_RU
#include "problem-ru.h"
#endif
#ifdef PROBLEM_VP
#include "problem-vp.h"
#endif
#ifdef PROBLEM_PB
#include "problem-pb.h"
#endif

#endif	/* _PROBLEM_H */

