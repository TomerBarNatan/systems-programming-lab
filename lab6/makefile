runall: run

run: task1.o task2.o task3.o task4.o LineParser.o
	gcc -m32 -g -Wall -o run1 task1.o LineParser.o
	gcc -m32 -g -Wall -o run2 task2.o LineParser.o
	gcc -m32 -g -Wall -o run3 task3.o LineParser.o
	gcc -m32 -g -Wall -o run4 task4.o LineParser.o

LineParser.o: LineParser.c LineParser.h
	gcc -m32 -Wall -c  LineParser.c -o LineParser.o
	
task1.o: task1.c
	gcc -m32 -Wall -c task1.c -o task1.o
	
task2.o: task2.c
	gcc -m32 -Wall -c task2.c -o task2.o
	
task3.o: task3.c
	gcc -m32 -Wall -c task3.c -o task3.o
	
task4.o: task4.c
	gcc -m32 -Wall -c task4.c -o task4.o

.PHONY: clean

clean: 
	rm -f *.o run
