
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "LineParser.h"
#include "linux/limits.h"
#include <errno.h>

int pid;	

void execute(cmdLine *pCmdLine) {
    int status;
    if (pid == 0){
        execvp(pCmdLine->arguments[0],pCmdLine->arguments);
        perror("Error");
        _exit(EXIT_FAILURE);
    }
    else if (pid == -1) {
        _exit(EXIT_FAILURE);
    }
    else {
        if (pCmdLine->blocking)
            waitpid(pid,&status,0);
    }
}

void debug(cmdLine *pCmdLine){
    fprintf(stderr, "PID : %d \n",pid);
    fprintf(stderr, "Executing command : %s \n",pCmdLine->arguments[0]);
}


int main(int argc, char** argv) {
   char buf[PATH_MAX];
   char input[2048];
   cmdLine* line = NULL;
   int d=0;

   if (argc>1 && strcmp(argv[1],"-d")==0)
      d=1;
   
    while (1){
        getcwd(buf, PATH_MAX);
        printf("%s> ",buf);
        fgets(input,2048,stdin);
        if (strcmp(input,"quit\n")==0){
            exit (0);
        }
        line = parseCmdLines(input);
        
        if (strcmp(line->arguments[0],"cd")==0){
            if(chdir (line->arguments[1])== -1)
                    fprintf (stderr,"%s %d\n","cd failed. The error number is: ",errno);
                return 0;
	}
        pid=fork();
        if(d)
            debug(line);
        execute(line);
  
   }
}
