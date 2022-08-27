#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "linux/limits.h"
#include <errno.h>
#include <signal.h>
#include <fcntl.h>



int main(int argc, char** argv) {
	int pid;
	int secpid;
	int d=0;
	int newPipe[2];
	int dupl =0;
	
	if (pipe(newPipe) == -1) { 
    	perror("pipe"); 
    	exit(EXIT_FAILURE); 
        }
   
	if (argc>1 && strcmp(argv[1],"-d")==0)
		d=1;
	
	pid=fork();
	
	if (pid == 0){
		close(1);
		dupl = dup(newPipe[1]);
		if(d)
			fprintf(stderr,"(child1>redirecting stdout to the write end of the pipe…)\n" );
		close (newPipe[1]);
		char* const buf[3]={"ls", "-l",0};
		if(d)
			fprintf(stderr,"(child1>going to execute cmd: …)\n" );
		execvp(buf[0],buf);
		perror("Error");
		_exit(EXIT_FAILURE);
	}
	else if (pid == -1) {
		_exit(EXIT_FAILURE);
	}
	else {
		if(d) 
			fprintf(stderr,"(parent_process>closing the write end of the pipe…)\n");
		close (newPipe[1]);
		if(d)
			fprintf(stderr,"(parent_process>forking…)\n");
		secpid=fork();
		if(d)
			fprintf(stderr,"(parent_process>created process with id: %d)\n",secpid);
		if (secpid ==0){
			close(0);
			dupl = dup(newPipe[0]);
			if(d)
				fprintf(stderr,"(child2>redirecting stdin to the read end of the pipe…)\n");
			close (newPipe[0]);
			char* const buf[4]={"tail", "-n", "2",0};
			if(d)
				fprintf(stderr,"(child2>going to execute cmd: …)\n" );
			execvp(buf[0],buf);
			perror("Error");
			_exit(EXIT_FAILURE);
		}
		else if (pid == -1) {
			_exit(EXIT_FAILURE);
		}
		else {
			if(d)
				fprintf(stderr,"(parent_process>closing the read end of the pipe…)\n" );
			close (newPipe[0]);
			if(d)
				fprintf(stderr,"(parent_process>waiting for child processes to terminate…)\n" );
			waitpid (pid,NULL,0);
			waitpid (secpid,NULL,0);
			if(d)
				fprintf(stderr,"(parent_process>exiting…)\n");
		}
	}
}

