#ifndef UTILITIES_H_INCLUDED
#define UTILITIES_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>


#include "xlsxwriter.h"
//Copyright 2014-2022, John McNamara <jmcnamara@cpan.org>
//All rights reserved.


void verify_alloc(void *p);
void freeGraph(long n, long **G);
long **duplicateGraph(long **G, long n) ;

int epsilon(long **G, long i);
int maxOmega(long **G, long n); 
long long power(long long a, long b);
long long modulus (long long x);

#endif
