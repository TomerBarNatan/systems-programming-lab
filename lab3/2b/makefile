 
#format is target-name: target dependencies
#{-tab-}actions

# All Targets
runall: run

# Tool invocations
# Executable "hello" depends on the files hello.o and run.o.
run: task2b.o
	gcc -m32 -g -Wall -o run task2b.o

# Depends on the source and header files
task2b.o: task2b.c 
	gcc -g -Wall -m32 -c -o task2b.o task2b.c 
	

#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o run
