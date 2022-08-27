#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "LineParser.h"
#include "linux/limits.h"
#include <errno.h>
#include <signal.h>
#include <fcntl.h>

#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0

int pid;
int newPipe[2];

void cat(cmdLine* pCmdLine){
    if (pCmdLine->inputRedirect!=NULL){
        close (0);
        open(pCmdLine->inputRedirect,O_RDONLY);
    }
    if (pCmdLine->outputRedirect!=NULL){
        close (1);
        open(pCmdLine->outputRedirect,O_WRONLY|O_CREAT,777);
    }
}	

void execute(cmdLine *pCmdLine) {
    int pid;
    int secpid;
    int newPipe[2];
    int dupl =0;

    if (!strcmp(pCmdLine->arguments[0],"quit")){
        freeCmdLines(pCmdLine);
        fprintf (stderr,"Program quiting");
        exit(0);
    }
    
    if (pipe(newPipe) == -1) { 
        perror("pipe"); 
        exit(EXIT_FAILURE); 
    }
    
    pid=fork();
    
    if (pid == 0){
        if (pCmdLine->next != NULL){
            close(1);
            dupl = dup(newPipe[1]);
            close (newPipe[1]);
        }
        else cat(pCmdLine);
        execvp(pCmdLine -> arguments[0], pCmdLine->arguments);
        perror("Error");
        _exit(EXIT_FAILURE);
    }
    else if (pid == -1) {
        _exit(EXIT_FAILURE);
    }
    else {
        close (newPipe[1]);
        if (pCmdLine->next != NULL){
            pCmdLine = pCmdLine->next;
            secpid=fork();
            if (secpid ==0){
                close(0);
                dupl = dup(newPipe[0]);
                close (newPipe[0]);
                execvp(pCmdLine -> arguments[0], pCmdLine->arguments);
                perror("Error");
                _exit(EXIT_FAILURE);
            }
            else if (pid == -1) {
                _exit(EXIT_FAILURE);
            }
            else close (newPipe[0]);
        }
        else close(newPipe[0]);
    }
    waitpid (pid,NULL,0);
    waitpid (secpid,NULL,0);
}

int main(int argc, char** argv) {
   char buf[PATH_MAX];
   char input[2048];
   cmdLine* line = NULL;
   
    while (1){
        getcwd(buf, PATH_MAX);
        printf("%s> ",buf);
        if (fgets(input,2048,stdin) != NULL){
            line = parseCmdLines(input);
            execute(line);
        }
    freeCmdLines(line);	
   }
}
