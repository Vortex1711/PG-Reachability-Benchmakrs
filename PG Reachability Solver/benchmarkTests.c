#include "gameGenerator.h"
#include "pgSolver1.h"
#include "pgSolver2.h"
#include "strategyImprovement.h"

void graphInfo(long **G, long n, int tCount, lxw_worksheet *worksheet) {
    worksheet_write_number(worksheet, tCount+2, 2, n, NULL);

    long eCount = 0; //number of edges
    long maxP = 0; //maximum priority

    for(int i=0; i<n; i++) {
        if(G[i][1] > maxP) {
            maxP = G[i][1];
        }

        for(int j=2; G[i][j] >= 0; j++) {
            eCount++;
        }
    }

    worksheet_write_number(worksheet, tCount+2, 3, maxP, NULL);
    worksheet_write_number(worksheet, tCount+2, 4, eCount, NULL);
}

/**
 * @brief performs the benchmarking test for a given test file
 * 
 * @param fileName the path and name of the test file
 * @param nMax the maximum number of vertices in a test graph
 * @param pMax the maximum priority of any vertex in a test graph
 * @return int 1 if the test was successful
 */
int oneFileBenchmark(char *fileName, long nMax, long pMax, int tCount, lxw_workbook *workbook, lxw_worksheet *worksheet) {
    FILE *fp;
    fp = fopen(fileName, "r");


    if(fp == NULL) {
        printf("Error while opening file\n");
        return 0;
    }

    long n;
    long **G = createPG(fp, &n, nMax, pMax);
    long **GC; //graph copy

    if(n <= 0) {
        fclose(fp);
        return 0;
    }

    printf("%s\n", fileName);

    graphInfo(G, n, tCount, worksheet);

    time_t t1, t2, t3, start, end;

    GC = duplicateGraph(G, n);
    time(&start);
    int *W1 = pgSolver1(GC, n);
    time(&end);
    t1 = end - start;
    freeGraph(n, GC);

    worksheet_write_number(worksheet, tCount+2, 5, t1, NULL);
    

    GC = duplicateGraph(G, n);
    time(&start);
    int *W2 = pgSolver2(GC, n);
    time(&end);
    t2 = end - start;
    freeGraph(n, GC);
    
    worksheet_write_number(worksheet, tCount+2, 6, t2, NULL);

    GC = duplicateGraph(G, n);
    time(&start);
    int *W3 = pgSolver3(GC, n);
    time(&end);
    t3 = end - start;
    freeGraph(n, GC);

    worksheet_write_number(worksheet, tCount+2, 7, t3, NULL);

    

    printf("PGSolver1: %lds; PGSolver2: %lds; PGSolver3: %lds\n", t1, t2, t3);

    free(W1);
    free(W2);
    free(W3);

    freeGraph(n, G);
    fclose(fp);

    return 1;
}

/**
 * @brief tests at most tMax files from a given directory 
 * 
 * @param directory the directory where the test files are found, must be in the same directory as the PG Reachability Solver directory
 * @param nMax the maximum number of vertices in a test graph
 * @param pMax the maximum priority of any given vertex in a test graph
 * @param tMax the maximum number of tests to be performed
 */
int benchmarkTestSet(char *directory, long nMax, long pMax, long tMax, int tCount, lxw_workbook *workbook, lxw_worksheet *worksheet) {
    char *location = (char *) malloc(300 * sizeof(char)); //stores the full path to the directory with the tests

    //get current location
    if (getcwd(location, 300) == NULL) {
        printf("Error in accessing current working directory\n");
        free(location);
        return tCount;
    }

    int i;
    for(i=strlen(location) - 1; i > 0; i--) {
        if(location[i] == '/') {
            break;
        }
    }
    location[i] ='\0';
    //free(location + i + 1);
    printf("%s\n", location);

    location = (char *) realloc(location, (strlen(location) + strlen(directory) + 4) * sizeof(char));
    strcat(location, "/");
    strcat(location, directory);

    tMax = tMax + tCount;
    

    DIR *d; 
    struct dirent *de; //directory entry, a file
    
    d = opendir(location);

    char *fileName;

    if(d) {
        printf("Testing from files in %s\n", directory);
        while(tCount < tMax && (de = readdir(d)) != NULL) {
            if(de->d_type != DT_REG) {
                //we make sure we only look at actual files
                continue;
            }
            fileName = (char *) malloc((strlen(location) + strlen(de->d_name) + 2) * sizeof(char));
            strcpy(fileName, location);
            strcat(fileName, "/");
            strcat(fileName, de->d_name);
            if(oneFileBenchmark(fileName, nMax, pMax, tCount, workbook, worksheet)) {
                //write information about test to file
                worksheet_write_string(worksheet, tCount+2, 0, directory, NULL);
                worksheet_write_string(worksheet, tCount+2, 1, de->d_name, NULL);

                tCount++;
                printf("Test %d completed.\n", tCount);
            }
            free(fileName);
        }

        closedir(d);
    }
    else {
        printf("Directory could not be opened\n");
    }

    free(location);

    return tCount;
}