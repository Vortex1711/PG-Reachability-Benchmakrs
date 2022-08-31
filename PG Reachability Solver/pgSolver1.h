#ifndef PGSOLVER1_H_INCLUDED
#define PGSOLVER1_H_INCLUDED

#include "graphFunctions.h"
 

int *pgSolver1(long **G, long n);
int *mpgSolver(long ** G, long n, long long *mu, int M);
long long *getMu(long **G, long n, long M);
edge* mpgObsolete(long k, long **G, long n, long long *mu, int M);
long **mpgSubgraphSolver(long *X, long **G, long n, long long *mu);

#endif