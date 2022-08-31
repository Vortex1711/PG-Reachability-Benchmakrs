#include "utilities.h"
#include "benchmarkTests.h"

void initSheet(lxw_worksheet *worksheet) {
    worksheet_set_column(worksheet, 0, 9, 15, NULL);

    worksheet_merge_range(worksheet, 0, 0, 0, 4, "Tests", NULL);
    worksheet_merge_range(worksheet, 0, 5, 0, 7, "Execution Time(s)", NULL);


    worksheet_write_string(worksheet, 1, 0, "Set", NULL);
    worksheet_write_string(worksheet, 1, 1, "Name", NULL);
    worksheet_write_string(worksheet, 1, 2, "Vertices", NULL);
    worksheet_write_string(worksheet, 1, 3, "Max Priority", NULL);
    worksheet_write_string(worksheet, 1, 4, "Edges", NULL);
    worksheet_write_string(worksheet, 1, 5, "PGSolver1", NULL);
    worksheet_write_string(worksheet, 1, 6, "PGSolver2", NULL);
    worksheet_write_string(worksheet, 1, 7, "PGSolver3", NULL);
}


int main(int argc, char **argv) {
    lxw_workbook *workbook = workbook_new("../Results/Results.xlsx");
    lxw_worksheet *worksheet = workbook_add_worksheet(workbook, NULL);

    initSheet(worksheet);


    int tCount = 0;

    tCount = benchmarkTestSet("Keiren Tests", 1000, 10, 15, tCount, workbook, worksheet);

    tCount = benchmarkTestSet("Random Tests", 1000, 10, 15, tCount, workbook, worksheet);

    tCount = benchmarkTestSet("Bipartite Symmetric Tests", 1000, 10, 15, tCount, workbook, worksheet);


    printf("%d tests completed.\n", tCount);


    workbook_close(workbook);

    return 0;



}