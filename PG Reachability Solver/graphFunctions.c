#include "graphFunctions.h"
#include "utilities.h"


/**
 * @brief adds all strongly connected components of X[r] to X
 * 
 * @param X array of subsets of G
 * @param r largest occupied position in X
 * @param G the graph with all the parity game information; G[x][0] - owner of node x; G[x][1] - priority of x; G[x][k] > -1, k > 1, (x, G[x][k]) is an edge 
 * @param n number of nodes of the graph
 * @return long the new largest occupied position in X
 */
long addSubsets(long **X, long r, long **G, long n) {
    long s; //number of strongly connected components of X[r]
    long **F; //array of strongly connected components of X[r]
    long xn = 0; //number of vertices in X[r]
    long *Xsize; //X[i] - number of occupied positions in the array X[r+i]
    long i, j;
    F = strConnComp(X[r], G, n, &s);
   
    while(X[r][xn] >= 0) {
        xn++;
    }
    free(X[r]);

    //initialize the subsets X[r]...X[r+s-1]
    Xsize = (long *) malloc (s* sizeof(long));
    for(i=0; i<s; i++) {
        X[r+i] = (long *) malloc((xn+1) * sizeof(long));
        Xsize[i] = 0;
        X[r+i][0] = -2;
    } 

    //Add the nodes to their respective subset
    for(j=0; j<xn; j++) {
        i = s - F[j][0];
        X[r+i][Xsize[i]] = F[j][1];
        Xsize[i]++;
        X[r+i][Xsize[i]] = -2;
        free(F[j]);
    }

    free(Xsize);
    free(F);

    return r+s-1;
}



/**
 * @brief find the strongly connected components of the subgraph of G with the nodes from X
 * 
 * @param X the subset of nodes which we need to split
 * @param G the graph with all the parity game information; G[x][0] - owner of node x; G[x][1] - priority of x; G[x][k] > -1, k > 1, (x, G[x][k]) is an edge 
 * @param n number of nodes in the graph
 * @param s number of strongly connected components of the subgraph G with the nodes from X
 * @return long** the subgraph of G with the nodes from X, with associated numbers for which scc they belong to
 */
long **strConnComp(long *X, long **G, long n, long *s) {
    long i, j, k, l, a;
    long solved = 0; //the number of vertices of X for which we identified what strongly connected component it is part of
    *s = 0;
    long xn = 0; //number of nodes in X
    while(X[xn] >= 0) {
        xn++;
    }

    long **F = (long **) malloc(xn * sizeof(long *)); //the subgraph of G with the nodes from X
    for(long x = 0; x < xn; x++) {
        long v = X[x];
        F[x] = (long *) malloc((xn+3) * sizeof(long));
        F[x][0] = 0; //vertex x does not yet belong to any strongly connected components
        F[x][1] = v; //the vertex in G wich x corresponds to 
        //Adding all outgoing edges of v that connect to other nodes from X
        l=2;
        for(i = 2; G[v][i] > -2; i++) {
            if(G[v][i] > -1) {
                for(j = 0; j < xn; j++) {
                    if(X[j] == G[v][i]) {
                        F[x][l] = j;
                        l++;
                    }
                }
            }
        }
        F[x][l] = -2;
    }

    i=0;
    long *v = (long *) malloc((xn+1) * sizeof(long)); //vertex v[x+1] is reachable from v[x]
    long *m = (long *) malloc((xn + 1) * sizeof(long)); //for m[i] <= j <=i, v[j] is reachable from v[i]
    long flag, w;

    while(solved < xn) {

        if(i == 0) {
            //initialize new list of vertices
            i = 1;
            for(k= 0; k<n; k++) {
                if(F[k][0] == 0) {
                    v[1] = k;
                    break;
                }
            }
            m[1] = 1;
            j = 1;
            a = 2;
        }

        while(j <= i) {
            w = -1;
            //find a node w such that (v, w) is in F
            for(a=2; F[v[j]][a] > -2; a++) {
                w = F[v[j]][a];
                if(w >= 0 && F[w][0] == 0) {
                    break;
                }
            }
            if(F[v[j]][a] >= 0) {
                F[v[j]][a] = -1; //remove the edge 
                flag = 0;
                for(k = 1; k < m[i]; k++) {
                    if(w == v[k]) { //v[k] reachable from v[j]
                        //v[m[k]] is reachable from all v[m[l]] wiht l=k,...,i
                        for(l = k + 1; l <= i; l++) {
                            m[l] = m[k];
                        }
                        j = m[k];
                        flag = 1;
                        break;
                    }
                }
                if(!flag) {
                    for(k = m[i]; k <= i; k++) {
                        if(w == v[k]) {
                            break;
                        }
                    }
                    
                    //w is not yet in the list; new entry
                    if(k == i+1) {
                        i++;
                        v[i] = w;
                        m[i] = i;
                        j = i;
                    }
                }
            }
            else {
                //no outgoing edges left from v[j]
                j++;
            }
        }

        //new scc found: {v[m[i]],...v[i]}
        (*s)++;
        for(k = m[i]; k<=i; k++) {
            F[v[k]][0] = *s; //mark the vertex as belonging to the scc with number s
        }
        solved = solved + i - m[i] + 1; 
        i = m[i] -1;
        j = m[i];
    }

    free(v);
    free(m);

    return F;
}



/**
 * @brief determines additional winning positions of player e through the equivalent reachability game
 * 
 * @param W W[x] - the player that has a winning positional strategy at vertex x
 * @param e the player number 
 * @param G the graph with all the parity game information; G[x][0] - owner of node x; G[x][1] - priority of x; G[x][k] > -1, k > 1, (x, G[x][k]) is an edge 
 * @param n number of nodes of the graph
 */
void reach(int *W, unsigned int e, long **G, long n) {
    long v, w;
    long **E = (long **) malloc(n * sizeof(long *)); //E[v][w] = 1 means that (v, w) is in the graph
    verify_alloc(E);

    //initialize E
    for(long i=0; i<n; i++) {
        E[i] = (long *) malloc(n * sizeof(long));
        verify_alloc(E[i]);
        for(long j=0; j<n; j++) {
            E[i][j] = 0;
        }
    }
    for(v=0; v<n; v++) {
        for(long i=2; G[v][i] > -2; i++) {
            w = G[v][i];
            if(w != -1) {
                E[v][w] = 1;
            }
        }
    }

    long *Wqueue = (long *) malloc(n * sizeof(long)); //queue of winning positions for Pe
    long qi=0, qmax = 0;

    //initialize Wqueue
    for(long v=0; v<n; v++) {
        if(W[v] == e) {
            Wqueue[qmax] = v;
            qmax++;
        }
    }
    
    int flag;
    for(qi = 0; qi < qmax; qi++) {
        w = Wqueue[qi];
        for(v = 0; v < n; v++) {
            if(E[v][w] == 0) {
                continue;
            }
            if(W[v] == e) {
                continue;
            }
            flag = 1;
            if(G[v][0] == e) {
                //v is owned by Pe and has an edge to the winning position w
                W[v] = e;
                Wqueue[qmax] = v;
                qmax++;
                continue;
            }
            for(long i=0; i<n; i++) {
                if(E[v][i] == 1 && W[i] != e) {
                    flag =0;
                    break;
                }
            }
            if(flag == 1) {
                //w not owned by Pe, but all edges lead to winning positions for Pe
                W[v] = e;
                Wqueue[qmax] = v;
                qmax++;
            }  
            
        }
    }
    for(long i = 0; i<n; i++) {
        free(E[i]);
    }
    free(E);
    free(Wqueue);
}