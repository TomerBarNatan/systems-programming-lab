all: main

main: task1b.o
	gcc -g -Wall -m32 -o task1b task1b.o

task1b.o: task1b.c
	gcc -g -Wall -m32 -c task1b.c -o task1b.o

.PHONY: clean

clean: 
	rm -v ./*.o task1b
