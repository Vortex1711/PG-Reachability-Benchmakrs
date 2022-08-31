#include "utilities.h"




/**
 * @brief Verifies that a pointer is not NULL, used after allocation operations
 * 
 * @param p - pointer
 */
void verify_alloc(void *p) {
    if(p==NULL) {
        printf("Error: Allocation failure\n");
        exit(EXIT_FAILURE);
    }
} 

/**
 * @brief frees a 2D array
 * 
 * @param n - number of rows in G
 * @param G - the 2D array
 */
void freeGraph(long n, long **G) {
    for(long i = 0; i < n; i++) {
        free(G[i]);
    }
    free(G);
}

/**
 * @brief returns -1 for a vertex owned by player 1, returns 1 otherwise
 * 
 * @param G - the graph; G[v][0] represents the owner of v
 * @param i - the vertex in question
 * @return int - 1 or -1
 */
int epsilon(long **G, long i) {
    if(G[i][0] == 2) {
        return 1;
    }
    else if(G[i][0] == 1) {
        return -1;
        }
        else return 0;
}

/**
 * @brief determines the maximum Omega value in the graph
 * 
 * @param G - the graph; G[v][1] = omega(v), 
 * @param n - number of vertices
 * @return int - the maximum Omega value
 */
int maxOmega(long **G, long n) {
    unsigned int max = G[0][1];
    int maxIndex = 0;

    for(long i=1; i < n; i++) {
        if(G[i][1] > max) {
            max = G[i][1];
            maxIndex = i; 
        }
    }
    return max;
}



/**
 * @brief returns a^b
 * 
 * @param a 
 * @param b 
 * @return long long - a^b
 */
long long power(long long a, long b) {
    long long x = 1;
    while(b) {
        x = x * a;
        b--;
    }
    return x;
}

/**
 * @brief applies the modulus operator to an integer
 * 
 * @param x - the integer in question
 * @return long long - x or -x
 */
long long modulus(long long x) {
    if(x < 0) {
        return -x;
    }

    return x;
}

/**
 * @brief returns an exact copy of the graph G
 * 
 * @param G the graph
 * @param n number of nodes
 * @return long** an exact copy of the graph G
 */
long **duplicateGraph(long **G, long n) {
    long i, j;
    long size;
    long **C = (long **) malloc (n * sizeof(long *)); //the subgraph C will contain the whole graph G

    for(i = 0; i<n; i++) {
        size = 20;
        C[i] = (long *) malloc(size * sizeof(long));
        verify_alloc(C);

        j=0;
        while(G[i][j] != -2) {
            if(j == size) {
                //increase size of C[i]
                size = size * 2;
                C[i] = (long *) realloc(C[i], size * sizeof(long));
                verify_alloc(C);
            }
            C[i][j] = G[i][j];
            j++;
        }
        if(j == size) {
            //increase size of C[i]
            size = size + 1;
            C[i] = (long *) realloc(C[i], size * sizeof(long));
            verify_alloc(C);
        }
        C[i][j] = -2;

    }

    return C;
}
