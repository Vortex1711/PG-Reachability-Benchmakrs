pgReachabilitySolver: main.o utilities.o gameGenerator.o graphFunctions.o pgSolver1.o pgSolver2.o strategyImprovement.o benchmarkTests.o
		gcc -g -Og main.o utilities.o gameGenerator.o graphFunctions.o pgSolver1.o pgSolver2.o strategyImprovement.o benchmarkTests.o -o pgReachabilitySolver -I/usr/local/include -L/usr/local/lib -lxlsxwriter -lz

main.o: main.c utilities.h benchmarkTests.h
		gcc -g -Og -c main.c -I/usr/local/include -L/usr/local/lib -lxlsxwriter -lz

utilities.o: utilities.c utilities.h
		gcc -g -Og -c utilities.c

gameGenerator.o: gameGenerator.c gameGenerator.h
		gcc -g -Og -c gameGenerator.c

graphFunctions.o: graphFunctions.c graphFunctions.h
		gcc -g -Og -c graphFunctions.c

pgSolver1.o: pgSolver1.c pgSolver1.h graphFunctions.h utilities.h
		gcc -g -Og -c pgSolver1.c

pgSolver2.o: pgSolver2.c pgSolver2.h strategyImprovement.h graphFunctions.h utilities.h
		gcc -g -Og -c pgSolver2.c

strategyImprovement.o: strategyImprovement.c strategyImprovement.h utilities.h
		gcc -g -Og -c strategyImprovement.c 

benchmarkTests.o: benchmarkTests.c utilities.h pgSolver1.h pgSolver2.h strategyImprovement.h gameGenerator.h
		gcc -g -Og -c benchmarkTests.c -I/usr/local/include -L/usr/local/lib -lxlsxwriter -lz



clean:
		-rm *.o pgReachabilitySolver