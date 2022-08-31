#ifndef GRAPHFUNCTIONS_H_INCLUDED
#define GRAPHFUNCTIONS_H_INCLUDED

typedef struct edge {
    unsigned long v1;
    unsigned long v2;
} edge;   

void reach(int *W, unsigned int e, long **G, long n);
long addSubsets(long **X, long r, long **G, long n);
long **strConnComp(long *X, long **G, long n, long *s);


#endif