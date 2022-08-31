#include "utilities.h"
#include "strategyImprovement.h"

/**
 * @brief Caluclate Val[v]
 * 
 * @param Val the valuations of each vertex
 * @param finished finished[v] = 0 if Val[v] has not yet been calculated for the current strategies
 * @param v the vertex for which we are calculating Val[v]
 * @param sigma the strategy of P2
 * @param tau the strategy of P1
 * @param C the subgraph of G with the nodes from X; C[x][0] - winner at x; C[x][1] - corresponding node in G
 * @param xn the number of nodes in X
 * @param G the graph with all the parity game information; G[x][0] - owner of node x; G[x][1] - priority of x; G[x][k] > -1, k > 1, (x, G[x][k]) is an edge
 * @param M the maximum priority in G
 */
void evaluate(valuation *Val, int *finished, long v, long *sigma, long *tau, long **C, long xn, long **G, int M) {
    if(finished[v] == 1) {
        return; //Val[v] has already been computed
    }

    int owner = G[C[v][1]][0]; //player that owns the vertex C[v][1]
    int omega = G[C[v][1]][1]; //priority of vertex C[v][1]
    int w; //the next vertex according to the 2 strategies

    long i;

    //initialize Val[v]
    Val[v].infinite = 0;
    for(i=0; i<=M; i++) {
        Val[v].L[i] = 0;
    }
    finished[v] = 0; //this means we have begun looking into the play that starts at v
    //we do not yet know if the play is finite or not
                    
    if(owner == 2 && sigma[v] == -1) {
        //This is a finite play
        Val[v].L[omega]++;
        finished[v] = 1; //we have finished calculating Val[v]
        return;
    }

    //Determine the next vertex in the play
    if(owner == 1) {
        w = tau[v];
    }
    else w = sigma[v];
    
    if(finished[w] == 0) {
        //There is a cycle
        //We know that because the value of Val[w] depends on the value of Val[v]
        //However, w is also the next vertex in the play after v
        //So there is a cycle that contains both v and w
        int max = omega;
        int wOwner;
        int wOmega;

        //Determine the maximum priority in the cycle
        while(w != v) {
            wOwner = G[C[w][1]][0];
            wOmega = G[C[w][1]][1];
            
            if(wOmega > max) {
                max = wOmega;
            }

            if(wOwner == 1) {
                w= tau[w];
            } else w = sigma[w];
        }

        if(max % 2 == 1) {
            Val[v].infinite = 1;
        }
        else Val[v].infinite = 2;

        finished[v] = 1; //we have finished calculating Val[v]

        return;
    }

    if(finished[w] == -1) {
        //we need to know Val[w] to determine Val[v]
        //however, Val[w] has not yet been determined
        evaluate(Val, finished, w, sigma, tau, C, xn, G, M);
    }

    if(Val[w].infinite == 0) {
        //The play is finite
        for(i=0; i<=M; i++) {
            Val[v].L[i] = Val[w].L[i];
        }
        Val[v].L[omega]++; //Add this occurance of the value omega
    }
    else {
        //We are in a cycle
        //The maximum priority in the cycle has already been calculated
        Val[v].infinite = Val[w].infinite;
    }

    finished[v] = 1; //we have finished calculating Val[v]

}

/**
 * @brief Returns 1 if (v, y) is preferrable to (v, x) for P2, 0 if it is the same, -1 if prefferable for P1
 * 
 * @param Val the current valuation
 * @param x a node
 * @param y a node
 * @param M maximum value in the graph
 * @return int 1 if (v, y) is preferrable to (v, x) for P2, 0 if it is the same, -1 if prefferable for P1
 */
int isSwitchable(valuation *Val, long x, long y, int M) {
    int p;

    if(x == y) {
        return 0;
    }

    if(x == -1) {
        if(Val[y].infinite == 1) {
            return -1;
        }

        if(Val[y].infinite == 2) {
            return 1;
        }

        p = -1;
        for(int i=M; i >= 0; i--) {
            if(Val[y].L[i] > 0) {
                p = i;
                break;
            }
        }

        if(p==-1) {
            return 0;
        }

        if(p % 2 == 0) {
            return 1;
        }

        return -1;
    }

    if(y == -1) {
        if(Val[x].infinite == 1) {
            return 1;
        }

        if(Val[x].infinite == 2) {
            return -1;
        }

        p = -1;
        for(int i=M; i >= 0; i--) {
            if(Val[x].L[i] > 0) {
                p = i;
                break;
            }
        }

        if(p == -1) {
            return 0;
        }

        if(p % 2 == 0) {
            return -1;
        }
        return 1;
    }

    if(Val[x].infinite != 0 && Val[y].infinite != 0) {
        if(Val[x].infinite == Val[y].infinite) {
            return 0;
        }
        if(Val[x].infinite == 2) {
            return -1;
        }

        return 1; //Val[y].infinite == 2
    }

    if(Val[x].infinite != Val[y].infinite) {
        //one of them is a finite play, the other is not
        if(Val[x].infinite == 0) {
            if(Val[y].infinite == 1) {
                return -1;
            }
            return 1;
        }

        if(Val[y].infinite == 0) {
            if(Val[x].infinite == 1) {
                return 1;
            }
            return -1;
        }
    }

    //neither x nor y are -1, neither are infinite plays 

    p = -1;
    for(int i=M; i>=0; i--) {
        if(Val[x].L[i] != Val[y].L[i]) {
            p=i;
            break;
        }
    }

    if(p < 0) {
        return 0;
    }

    if(p % 2 == 0) {
        if(Val[x].L[p] < Val[y].L[p]) {
            return 1;
        }
        return -1; //Val[x].L[p] > Val[y].L[p]
    }

    //p % 2 == 1
    if(Val[x].L[p] > Val[y].L[p]) {
        return 1;
    }
    return -1;
}


/**
 * @brief improve the strategy of the given player
 * 
 * @param C the subgraph of G with the nodes from X; C[x][0] - winner at x; C[x][1] - corresponding node in G
 * @param xn the number of nodes in X
 * @param player the player number
 * @param Val the valuation at the start of the function
 * @param sigma the strategy of P2
 * @param tau the strategy of P1
 * @param G the graph with all the parity game information; G[x][0] - owner of node x; G[x][1] - priority of x; G[x][k] > -1, k > 1, (x, G[x][k]) is an edge
 * @param M the maximum priority in G
 * @return int 1 if there was a swtich made
 */
int switchEdges(long **C, long xn, int player, valuation *Val, long *sigma, long *tau, long **G, int M) {
    long v, u, w;
    long i;
    int switchMade = 0; //1 if a switch has been made
    int switchable;
    int owner; //player that owns the vertex C[v][1]
    int omega; //priority of vertex C[v][1]
    for(v=0; v<xn; v++) {
        owner = G[C[v][1]][0];
        omega = G[C[v][1]][1];

        if(owner != player) {
            continue;
        }

        if(owner == 2) {
            u = sigma[v];
        }
        else u = tau[v];

        for(i=2; C[v][i] >= 0; i++) {
            //look through adjacent vertices for switchable edges 
            w = C[v][i];
            switchable = isSwitchable(Val, u, w, M);
            if(owner == 1 && switchable == -1) {
                //printf("Switched (%d, %d) to (%d, %d)\n", v, u, v, w);
                //Val[u] =| Val[w] (choosing W is worse for P2)
                switchMade = 1;
                tau[v] = w;
                u = w;
            }
            if(owner == 2 && switchable == 1) {
                //Val[u] |= Val[w] (choosing W is better for P2)
                switchMade = 1;
                sigma[v] = w;
                u = w;
            }
        }

        //P2 can also choose to simply end the game
        if(owner == 2 && isSwitchable(Val, u, -1, M) == 1) {
            switchMade = 1;
            sigma[v] = -1;
        }

    }

    
    return switchMade;
}

/**
 * @brief determines the player with a winning positional strategy in the parity game for the nodes in C
 * 
 * @param C the subgraph of G with the nodes from X; C[x][0] - winner at x; C[x][1] - corresponding node in G
 * @param xn the number of nodes in X
 * @param M the maximum priority in C
 * @param G the graph with all the parity game information; G[x][0] - owner of node x; G[x][1] - priority of x; G[x][k] > -1, k > 1, (x, G[x][k]) is an edge
 */
void pgStrategyImprovement(long **C, long xn, int M, long **G) {
    valuation *Val = (valuation *) malloc(xn * sizeof(valuation)); //current strategy valuation
    int *finished = (int *) malloc(xn * sizeof(int)); //finished[x] shows if val[x] has been calculated with the current strategies
    long i;
    int switchSigma; //1 if there was a change in the strategy of P2
    int switchTau; //1 if there was a change in the strategy of P1


    for(i=0; i<xn; i++) {
        Val[i].L = (long *) malloc((M+1) * sizeof(long));
    }

    long *sigma = (long *) malloc(xn * sizeof(long)); //strategy of P2
    //initialize sigma
    for(i=0; i<xn; i++) {
        sigma[i] = -1; 
    }

    //initialize tau
    long *tau = (long *) malloc(xn * sizeof(long)); //strategy of P1 
    for(i = 0; i < xn; i++) {
        if(G[C[i][1]][0] == 1) {
            tau[i] = C[i][2];
        }
        else tau[i] = -1;
    }


    do {
        do {
            
            //Calculate Val[i] for all vertices
            for(i = 0; i<xn; i++) {
                finished[i] = -1;
            }
            
            for(i = 0; i<xn; i++) {
                evaluate(Val, finished, i, sigma, tau, C, xn, G, M);
                
            }

            switchTau = switchEdges(C, xn, 1, Val, sigma, tau, G, M); //improve tau strategy

        }while(switchTau == 1); 
        //no odd-switchable edges left

        switchSigma = switchEdges(C, xn, 2, Val, sigma, tau, G, M); //improve sigma strategy


    }while(switchSigma == 1);
    //no switchable edges left

    //determine the winners of each vertex
    for(i = 0; i<xn; i++) {
        if(Val[i].infinite == 2) {
            C[i][0] = 2;
        }
        else{
            C[i][0] = 1;
        } 

        free(Val[i].L);
    }

    free(Val);
    free(finished);
    free(sigma);
    free(tau);

}

/**
 * @brief Solves the parity game using just the strategy improvement algorithm
 * 
 * @param G the graph with all the parity game information; G[x][0] - owner of node x; G[x][1] - priority of x; G[x][k] > -1, k > 1, (x, G[x][k]) is an edge 
 * @param n number of nodes in the graph
 * @return int* W[x] - the player that has a winning positional strategy at node x
 */
int *pgSolver3(long **G, long n) {
    long xn = n;
    long **C; //the subgraph of G with the nodes from X; C[x][0] - winner at x; C[x][1] - corresponding node in G
    long size; //allocated size of a row in G
    long i, j;
    int M=-1; //maximum priority in the graph
    int *W; //W[x] - the player that has a winning strategy at vertex x


    C = (long **) malloc (n * sizeof(long *)); //the subgraph C will contain the whole graph G

    for(i = 0; i<n; i++) {
        size = n/10 + 2;
        C[i] = (long *) malloc(size * sizeof(long));
        verify_alloc(C);

        if(G[i][1] > M) {
            M = G[i][1];
        }

        C[i][0] = 0;
        C[i][1] = i;

        //Add all edges to C
        j = 2;
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
            size = size * 2;
            C[i] = (long *) realloc(C[i], size * sizeof(long));
            verify_alloc(C);
        }
        C[i][j] = -2;

    }



    pgStrategyImprovement(C, xn, M, G);

    //store the winners 
    W = (int *) malloc(n * sizeof(int));
    for(i=0; i<n; i++) {
        W[i] = C[i][0];
        free(C[i]);
    }

    free(C);

    return W;
}