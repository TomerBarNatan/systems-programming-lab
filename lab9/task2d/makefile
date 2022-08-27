all: main

main: 
	gcc -m32 -c task2d.c -o task2d.o
	ld -o task2d task2d.o startup.o start.o -L/usr/lib32 -lc -T linking_script -dynamic-linker /lib32/ld-linux.so.2

.PHONY: clean

clean: 
	rm -v ./*.o task2d
