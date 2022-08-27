all: main

main: task1.o task4.o
	gcc -g -Wall -m32 -o task1 task1.o
	gcc -g -Wall -m32 -o task4 task4.o
	
task1.o: task1.c
	gcc -g -Wall -m32 -c task1.c -o task1.o
	
task4.o: task4.c
	gcc -g -Wall -m32 -fno-pic -c task4.c -o task4.o

.PHONY: clean

clean: 
	rm -v ./*.o task1 task4
