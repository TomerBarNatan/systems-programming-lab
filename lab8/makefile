all: main

main: task1.o task2.o task3.o
	gcc -g -Wall -m32 -o task1 task1.o
	gcc -g -Wall -m32 -o task2 task2.o
	gcc -g -Wall -m32 -o task3 task3.o
	
task1.o: task1.c
	gcc -g -Wall -m32 -c task1.c -o task1.o
	
task2.o: task2.c
	gcc -g -Wall -m32 -c task2.c -o task2.o
	
task3.o: task3.c
	gcc -g -Wall -m32 -c task3.c -o task3.o

.PHONY: clean

clean: 
	rm -v ./*.o task1 task2 task3
