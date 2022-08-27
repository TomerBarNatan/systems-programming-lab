all: run

# Executable "run" depends on the files add.o , main.o and numbers.o.

run: menu_map.o
	gcc -g -m32 -Wall -o run menu_map.o

# Depends on the source and header files
menu_map.o: menu_map.c
	gcc -m32 -g -Wall -c -o menu_map.o menu_map.c


#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o run

