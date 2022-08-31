#ifndef STRATEGYIMPROVEMENT_H_INCLUDED
#define STRATEGYIMPROVEMENT_H_INCLUDED

typedef struct valuation {
    int infinite;
    long *L;
} valuation;

void pgStrategyImprovement(long **C, long xn, int M, long **G);
int *pgSolver3(long **G, long n);

#endif