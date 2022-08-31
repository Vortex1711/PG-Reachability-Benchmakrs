#include "utilities.h"
#include "graphFunctions.h"
#include "pgSolver2.h"
#include "strategyImprovement.h"

/**
 * @brief If the value n is advantageous for a given player, returns a positive value, otherwise, negative. Used in pgObsolete
 * 
 * @param n an integer value
 * @param t a vertex
 * @param inf a stand-in for infinity
 * @param G the graph
 * @return int n or -n
 */
int phi(int n, int t, int inf, long **G) {
    if(n == inf || n == -inf) {
        return epsilon(G, t) * n;
    }

    if(n % 2 == 1) {
        return -n;
    } 

    return n;
}

/**
 * @brief creates an array of the k-obsolete edges of the graph
 * 
 * @param k the maximum length of the cycle
 * @param G the graph with all the parity game information; G[x][0] - owner of node x; G[x][1] - priority of x; G[x][k] > -1, k > 1, (x, G[x][k]) is an edge 
 * @param n number of nodes in the graph
 * @param mu the array of edge weights
 * @param M the maximum omega value of the graph
 * @return edge* the array of k-obsolete edges
 */
edge* pgObsolete(long k, long **G, long n) {
    edge *F; //array of k-obsolete edges
    long sizeF = n; //size of F
    F = (edge *) malloc (sizeF * sizeof(edge));
    verify_alloc(F);
    long idxF = 0; //smallest empty position in the F array
    int e; //epsilon(v)
    int inf = INT_MAX - 1; //infinity
    int max; 
    long u;
    int hasOutgoingEdges;

    for(long v=0; v < n; v++) {
        e = epsilon(G, v);
        long *eta = (long *) malloc (n * sizeof(long));
        verify_alloc(eta);

        //initialize eta_0
        for(long w = 0; w < n; w++) {
            if(w == v) {
                eta[w] = 0;
            }
            else eta[w] = inf;
        }
        

        for(long i = 1; i < k; i++) {
            long *nextEta = (long *) malloc (n * sizeof(long));
            verify_alloc(nextEta);

            for(long w = 0; w < n; w++) {
                if(w == v) {
                    nextEta[w] = 0; //eta_x[v] = 0 for all x
                    continue;
                }

                hasOutgoingEdges = 0;

                //finding the best available outgoing edge at w
                for (long j= 2; G[w][j] > -2; j++) {
                    if(G[w][j] > -1) {
                        u = G[w][j];
                        if(hasOutgoingEdges == 0) {
                            hasOutgoingEdges = 1;
                            max = epsilon(G, w) * phi(eta[u], v, inf, G);

                        } else if(epsilon(G, w) * phi(eta[u], v, inf, G) > max) {
                            max = epsilon(G, w) * phi(eta[u], v, inf, G);
                        }
                    }
                }

                if(!hasOutgoingEdges) {
                    //if it has no outgoing edges, it is a sink
                    nextEta[w] = inf;
                    continue;
                }

                
                
               if(G[w][1] > epsilon(G, w) * phi(max, v, inf, G)) {
                    nextEta[w] = G[w][1];
               }
               else nextEta[w] = epsilon(G, w) * phi(max, v, inf, G);

                
            }

            free(eta);
            eta = nextEta; //replace eta_i-1 with eta_i
        }
        for(long i= 2; G[v][i] > -2; i++) {
            if(G[v][i] > -1) {
                u = G[v][i];
                if(eta[u] > G[v][1]) {
                    max = eta[u];
                }
                else max = G[v][1];
                if(eta[u] != inf && max % 2 != G[v][0] % 2) {
                    //it is an obsolete edge, so we add it to the array
                    if(idxF == sizeF) {
                        sizeF = sizeF * 2;
                        F = (edge *) realloc(F, sizeF * sizeof(edge));
                        verify_alloc(F);
                    }
                    F[idxF].v1 = v;
                    F[idxF].v2 = u;
                    idxF++;
                }
            }
        }
        free(eta);
    }

    //we add one final element to the array, to signal the end of it
    if(idxF == sizeF) {
        sizeF = sizeF + 1;
        F = (edge *) realloc(F, sizeF * sizeof(edge));
    }
    F[idxF].v1 = -1;
    F[idxF].v2 = -1;

    return F;
}


/**
 * @brief determines the player with a winning positional strategy in the parity game for the nodes in X
 * 
 * @param X subset of nodes of G
 * @param G the graph with all the parity game information; G[x][0] - owner of node x; G[x][1] - priority of x; G[x][k] > -1, k > 1, (x, G[x][k]) is an edge 
 * @param n number of nodes of the graph
 * @return long** the subgraph with the nodes of X, also stores the winners of each node
 */
long **pgSubgraphSolver(long *X, long **G, long n) {
    long xn = 0; //number of nodes in X
    long x, i, j, l;
    long v, w;
    int M = -1; //maximum priority of the nodes in X

    while(X[xn] >= 0) {
        if(G[X[xn]][1] > M) {
            M = G[X[xn]][1];
        }
        xn++;
    }

    long **C = (long **) malloc(xn * sizeof(long *)); //the subgraph with the nodes of X, also stores the winners of each node
    if(xn == 1) {
        //only one node, player that owns it loses at this position
        C[0] = (long *) malloc (3 * sizeof(long));
        i=2;
        while(G[X[0]][i] != -2 && G[X[0]][i] != X[0]) {
            i++;
        }
        if(G[X[0]][i] != -2) {
            C[0][0] = 2 - (G[X[0]][1] % 2); //this subgraph consists of a node with an edge to itself
        } else if(G[X[0]][0] == 2) {
            C[0][0] = 1; //this node is a sink, player that owns it loses at this position
        } else C[0][0] = 2;
        C[0][1] = X[0];
        C[0][2] = -2;
        return C;
    }


    for(x = 0; x < xn; x++) {
        v = X[x];
        C[x] = (long *) malloc((xn+3) * sizeof(long));
        C[x][0] = 0; //no winner determined yet
        C[x][1] = v; //the vertex of G which x corresponds to
        //add all outgoing edges of v which lead to another node in X
        l=2;
        for(i = 2; G[v][i] > -2; i++) {
            if(G[v][i] > -1) {
                for(j = 0; j < xn; j++) {
                    if(X[j] == G[v][i]) {
                        C[x][l] = j;
                        l++;
                    }
                }
            }
        }
        C[x][l] = -2;
    }

    pgStrategyImprovement(C, xn, M, G);

    return C;
}


/**
 * @brief Solves the parity game through 
 * 
 * @param G the graph with all the parity game information; G[x][0] - owner of node x; G[x][1] - priority of x; G[x][k] > -1, k > 1, (x, G[x][k]) is an edge 
 * @param n number of nodes in the graph
 * @return int* W[x] - the player that has a winning positional strategy at node x
 */
int *pgSolver2(long **G, long n) {
    long r; //number of subsets left to solve
    long *newXr; //a subset that is obtained from removing all vertices with known winners from X[r]
    long xn; //number of vertices in a subset
    int *W = (int *) malloc (n * sizeof(int)); //W[x] - the player that has a winning positional strategy at node x
    long **X = (long **) malloc ((n+1) * sizeof(long *)); //Array of subsets
    edge *F; //array of n-obsolete edges
    long **C; //The graph formed from the subset X[r], also contains the winners of each node

    F = pgObsolete(n, G, n); //find all n-obsolete edges
    while(F[0].v1 != -1) { 
        //After removing all n-obsolete edges in the graph, more n-obsolete edges may be discovered
        //We keep removing n-obsolete edges until no more are found
        long k = 0;
        long i;
        long j;
        while (F[k].v1 != -1) {
            i = F[k].v1;
            j = 2;
            while(G[i][j] != F[k].v2) {
                j++;
            }
            G[i][j] = -1; //Remove the n-obsolete edge
            k++;
        }
        free(F);
        F = pgObsolete(n, G, n);
    }
    free(F);

    //initialize the W array
    for(long i=0; i<n; i++) {
        W[i] = 0; 
    }

    if(n == 0) {
        r = 0;
    }
    else{
        //we create X[1], which is the subset of nodes in G that contains all nodes
        r = 1;
        X[1] = (long *) malloc((n+1) * sizeof(long));
        for(long i=0; i<n; i++) {
            X[1][i] = i;
        }
        X[1][n] = -2; //-2 marks the end of the subset
    } 

    while(r > 0) {
        xn = 0; 
        while(X[r][xn] >= 0) {
            xn++;
        } 
        newXr = (long *) malloc ((xn+1) * sizeof(long));
        xn = 0;
        //we remove all nodes in X[r] with known winners, formig newXr
        for(long i=0; X[r][i] > -2; i++) {
            if(W[X[r][i]] == 0) {
                newXr[xn] = X[r][i];
                xn++;
            }
        }
        if(xn == 0) {
            //if X[r] contained only positions with known winners, we skip it.
            free(newXr);
            free(X[r]);
            r--;
            continue;
        }
        //replace X[r] with newXr
        free(X[r]);
        newXr[xn] = -2;
        X[r] = newXr;

        r = addSubsets(X, r, G, n); //add all the strongly connected components of X[r] to X

        C = pgSubgraphSolver(X[r], G, n); //solve the mpg of subgraph X[r]
        for(long i=0; X[r][i] > -2; i++) {
            W[C[i][1]] = C[i][0]; //store the winners of the nodes in X[r]
            free(C[i]);
        }
        free(C);
        free(X[r]);
        r--;

        reach(W, 1, G, n); //play the reachability game for P1
        reach(W, 2, G, n); //play the reachability game for P2
    }
    
    free(X);

    return W;
}