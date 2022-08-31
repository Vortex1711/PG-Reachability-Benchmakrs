#ifndef GAMEGENERATOR_H_INCLUDED
#define GAMEGENERATOR_H_INCLUDED

#include "utilities.h"

long **createPG(FILE *file, long *n, long nMax, long pMax);
void printGraph(long *n, long **G);

#endif