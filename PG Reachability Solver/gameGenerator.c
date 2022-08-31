#include "gameGenerator.h"

/**
 * @brief if word is a string that represents a positive integer, then return that integer, otherwise, return -1
 * 
 * @param word 
 * @return long 
 */
long toNumber(char *word) {
    long result = 0;
    int i;


    if(strlen(word) > 9) {
        //if word is indeed a number, but has more than 9 digits, it will not fit in long
        return -1;
    }

    for(i=0; word[i] != '\0'; i++) {
        if('0' <= word[i] && word[i] <= '9') {
            result = result * 10 + (word[i] - '0');
        }
        else return -1;
    }

    return result;
}

/**
 * @brief prints the graph. For debugging purposes
 * 
 * @param n - number of vertices
 * @param G - the graph; G[v][0] represents the owner of v, G[v][1] = omega(v), if G[v][i] > -1, the edge (v, G[v][i]) is in the graph (i > 1)
 */
void printGraph(long *n, long **G) {
    long j;
    printf("n = %ld\n", *n);
    for(long i=0; i < *n; i++) {
        j=0;
        while(G[i][j] != -2) {
            printf("%ld ", G[i][j]);
            j++;
        }
        printf("\n");
    }
}

long getN(char *input) {
    char *n;
    int i=0;
    //all test cases start with the line "parity x;" where x is the number of the last vertex (so there are x+1 vertices, starting from 0)
    n = strtok(input, " ;");
    n = strtok(NULL, " ;");

    return toNumber(n) + 1; 
} 

/**
 * @brief 
 * 
 * @param n - pointer to the number of vertices
 * @return long** - the graph; G[v][0] represents the owner of v, G[v][1] = omega(v), if G[v][i] > -1, the edge (v, G[v][i]) is in the graph (i > 1)
 */
long **createPG(FILE *file, long *n, long nMax, long pMax) {
    long **G;
    char c;
    int firstLine = 1; //indicates if we are reading the first line or a different one
    char *inputLine;
    long ilSize; //current size of the inputLine array
    long gvSize; //size of G[v]
    long i, j;
    long v, w;
    char *token;

    do{
        i=-1;
        ilSize = 20;
        inputLine = (char *) malloc(ilSize * sizeof(char));
        do {
            c = fgetc(file);
            i++;
            if(i >= ilSize) {
                ilSize = ilSize * 2;
                inputLine = (char *) realloc(inputLine, ilSize * sizeof(char));
            }
            inputLine[i] = c;
        }while(c != '\n' && c != EOF);
        inputLine[i] = '\0';


        if(firstLine) {
            firstLine = 0;
            *n = getN(inputLine);
            if(*n < 0 || *n > nMax) {
                //we only consider test cases for which n <= nMax
                *n = -1;
                free(inputLine);
                return NULL;
            }

            G = (long **) malloc((*n) * sizeof(long));
        } 
        else {
            token = strtok(inputLine, " ,;");
            if(token == NULL) {
                break; //account for some test files having an additonal new line at the end
            }
            v = toNumber(token); //the vertex which the line describes
            if(v < 0) {
                //some test cases start listing the graph specifications from the 3rd line
                free(inputLine);
                continue;
            }
            gvSize = 20;
            G[v] = (long *) malloc (gvSize * sizeof(long));
            token = strtok(NULL, " ,;");
            G[v][1] = toNumber(token); //the priority of the vertex v
            if(G[v][1] > pMax) {
                //We only consider test cases in which the maximum number of iterations for the mpgSubgraphSolver loop can be stored in long long
                free(inputLine);
                freeGraph(v+1, G);
                *n = -1;

                return NULL;
            }

            token = strtok(NULL, " ,;");
            j = 1;
            do {
                if(j == 1) {
                    G[v][0] = 2 - toNumber(token); //the owner of v
                    j++;
                } else {
                    if(j >= gvSize) {
                        gvSize = gvSize * 2;
                        G[v] = (long *) realloc(G[v], gvSize * sizeof(long));
                    }
                    G[v][j] = w;
                    j++;
                }
                token = strtok(NULL, " ,;");
                w = -1;
                if(token != NULL) {
                    w = toNumber(token);
                }
            }while(w >= 0);

            if(j == 2) {
                //Graph has a sink, pgSolver3 does not work for graphs with sinks
                *n = -1;
                free(inputLine);
                freeGraph(v+1, G);
                return NULL;
            }
            if(j >= gvSize) {
                gvSize++;
                G[v] = (long *) realloc(G[v], gvSize * sizeof(long));
            }
            G[v][j] = -2;
        }

        free(inputLine);

    }while(c != EOF);


    return G;


}

