#include <stdio.h> 
#include <fcntl.h> 
#include <errno.h> 
#include <time.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief creates the string equivalent of the integer x and stores it in number[]
 * 
 * @param x
 * @param number 
 */
void numberToString(int x, char number[]) {

    int d; //digit
    int i=1;
    char c;

    while(x > 0 && i >= 0) {
        d = x % 10;
        x = x/10;

        c = d + '0';
        number[i] = c;

        i--;
    }


}

/**
 * @brief Outputs the game information, in the same format as Keiren's tests
 * 
 * @param fp the file where we print the information
 * @param G the graph, showing all the edges
 * @param omega the priorities of each vertex
 * @param owner the owners of each vertex
 * @param n the number of nodes
 */
void writeGraph(FILE *fp, int **G, int *omega, int *owner, int n) {
    fprintf(fp, "parity %d;\n", n-1);

    int firstEdge;

    for(int i=0; i<n; i++) {
        fprintf(fp, "%d %d %d ", i, omega[i], owner[i]);

        //printing the adjacent vertices
        firstEdge = 1;
        for(int j=0; j<n; j++) {
            if(G[i][j] == 1) {
                if(firstEdge) {
                    firstEdge = 0;
                    fprintf(fp, "%d", j);
                }
                else fprintf(fp, ", %d", j);
            }
        }

        if(i != n-1) {
            fprintf(fp, ";\n");
        }
    }
}


int main(int argc, char **argv) {
    int tTotal = 15; //total number of tests to be generated in each category
    int tCount; //current number of tests generated
    char tName[50]; //name of the test file we are creating
    char tNumber[6]; //test number, translated to string

    FILE *fp;

    int n, nMin, nMax = 300;
    int eMax; //max number of outgoing edges
    int e;
    int *owners; //array of vertex owners
    int *omega; //array of vertex priorities
    int oMax; //maximum omega for the graph
    int **G; //the graph
    int P1Count; //for bipartite and symmetric games, the number of vertices owned by P1

    int i, j, k;

    srand(time(NULL));

    //Create 15 random tests with 0-1000 nodes
    for(tCount = 0; tCount < tTotal; tCount++) {
        nMin = 300 * (tCount / 5); //every 5 tests we change nMin

        n = 1 + nMin + rand() % nMax; //generate a random number of vertices between nMin and nMin+nMax 

        owners = (int *) malloc(n * sizeof(int));
        omega = (int *) malloc(n * sizeof(int));
        G = (int **) malloc(n * sizeof(int *));

        if(n < 300) {
            oMax = rand() % 4 + 1; //this is to make sure we have some tests that are apropriate for pgSolver1
        }
        else oMax = rand() % 10 + 1;

        for(i=0; i<n; i++) {
            owners[i] = rand() % 2; //owners are either 0(P2) or 1(P1)
            omega[i] = rand() % oMax; //maximum priority is 9

            G[i] = (int *) malloc(n * sizeof(int));
            for(j = 0; j<n; j++) {
                G[i][j] = 0;
            }

            //setting the maximum outgoing degree of vertex i
            if(tCount % 10 < 5) {
                eMax = 1 + (rand() % 10); //the goal was to have a varied number of edges between the 10 different tests of a given nMin
            }
            else eMax = 1 + (rand() % 20);

            //Setting adjacent vertices
            for(k=0; k < eMax && k<n; k++) {
                j = rand() % n;
                G[i][j] = 1;
            }


        }
        
        strcpy(tName, "../Random Tests/RandTest_");
        strcpy(tNumber, "00");
        numberToString(tCount, tNumber);
        strcat(tName, tNumber);
        strcat(tName, ".gm");



        fp = fopen(tName, "w");

        writeGraph(fp, G, omega, owners, n);

        fclose(fp);

        free(owners);
        free(omega);
        for(i=0; i<n; i++) {
            free(G[i]);
        }
        free(G);

    }

    printf("Generated %d random tests\n", tCount);



    //Create 15 random bipartite symmetric tests with 0-1000 nodes
    for(tCount = 0; tCount < tTotal; tCount++) {
        nMin = 300 * (tCount / 5); //every 5 tests we change nMin

        n = 1 + nMin + rand() % nMax; //generate a random number of vertices between nMin and nMin+nMax 

        owners = (int *) malloc(n * sizeof(int));
        omega = (int *) malloc(n * sizeof(int));
        G = (int **) malloc(n * sizeof(int *));

        if(n < 300) {
            oMax = rand() % 4 + 1; //this is to make sure we have some tests that are apropriate for pgSolver1
        }
        else oMax = rand() % 10 + 1;

        P1Count = rand() % n;
        if(P1Count == 0) {
            P1Count = 1;
        }

        for(i=0; i<n; i++) {
            //initialize graph
            G[i] = (int *) malloc(n * sizeof(int));
            for(j = 0; j<n; j++) {
                G[i][j] = 0;
            }
        }

        for(i=0; i<n; i++) {
            omega[i] = rand() % oMax; 

            if(i < P1Count) {
                owners[i] = 1;
                //we make sure that from each vertex owned by P1, we have at least one outgoing edge to a vertex owned by P2
                j = P1Count + rand() % (n-P1Count);
                G[i][j] = 1;
                G[j][i] = 1; 
                //more edges will be added from P2's vertices
            }
            else {
                owners[i] = 0;

                //setting the maximum outgoing degree of vertex i
                if(tCount % 10 < 5) {
                    eMax = 1 + (rand() % 10); //the goal was to have a varied number of edges between the 10 different tests of a given nMin
                }
                else eMax = 1 + rand() % 30;

                //Setting adjacent vertices
                for(k=0; k < eMax && k < n; k++) {
                    j = rand() % P1Count;
                    G[i][j] = 1; //bipartite, the two nodes connected by an edge are owned by different players
                    G[j][i] = 1; //symmetric
                }

                if(k == 0) {
                    printf("ERROR: SINK DETECTED AT TEST %d", tCount);
                }
            }

        }
        
        strcpy(tName, "../Bipartite Symmetric Tests/BipSymTest_");
        strcpy(tNumber, "00");
        numberToString(tCount, tNumber);
        strcat(tName, tNumber);
        strcat(tName, ".gm");

        fp = fopen(tName, "w");

        writeGraph(fp, G, omega, owners, n);

        fclose(fp);

        free(owners);
        free(omega);
        for(i=0; i<n; i++) {
            free(G[i]);
        }
        free(G);

    }

    printf("Generated %d random bipartite symmetric tests\n", tCount);

}