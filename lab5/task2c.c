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
#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0

int pid;	

typedef struct process{
    cmdLine* cmd;	/* the parsed command line*/
    pid_t pid;	/* the process id that is running the command*/
    int status;	/* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	/* next process in chain */
} process;

process** list = NULL;

void addProcess(process** process_list, cmdLine* cmd, int pid){
    process* newProc = malloc(sizeof(process));
    newProc->cmd = cmd;
    newProc->pid = pid;
    newProc->status = 1;
    newProc-> next = NULL;
    
    
    if (*process_list == NULL) {
        *process_list = newProc;
    }	
    else {
        process* templist = *process_list;
        while (templist->next !=NULL){
            templist = templist->next;
        }
        templist->next = newProc;
    }
}


void updateProcessStatus(process* process_list, int pid, int status) {
    if(process_list != NULL){
        if(process_list->pid == pid) {
            process_list->status = status;
            return;
        }
        updateProcessStatus(process_list->next, pid, status);
    }
}

void updateProcessList(process **process_list){
    process* templist = *process_list;
    int status;
    while (templist != NULL) {
        int temppid = waitpid(templist->pid,&status,WNOHANG);
        if (temppid > 0) {
            if (WIFSIGNALED(status) || WIFEXITED(status)) {
                updateProcessStatus(*process_list,templist->pid,-1);
            }
            else if (WIFSTOPPED(status)) {
                updateProcessStatus(*process_list,templist->pid,0);
            }
            else {
                updateProcessStatus(*process_list,templist->pid,1);
            }
        }
        else if (temppid == -1){
            printf("atha");
            updateProcessStatus(*process_list,templist->pid,-1);
        }
        templist = templist->next;
    }
}

void freeProcessLine(process* process_list){
    if (process_list != NULL){
        freeCmdLines(process_list->cmd);
        free(process_list);
    }
}

void freeProcessList(process* process_list){
    if (process_list->next != NULL)
        freeProcessList(process_list->next);
    freeProcessLine(process_list);
}

void printProcessList(process** process_list){
    if (*process_list == NULL) {
        return;
    }
    updateProcessList(process_list);
    
    process* templist = *process_list;
    process *prevlist = NULL;
    process* currlist = NULL;
    while (templist!= NULL){
        char* charstatus = "Teminated";
        if ((*process_list)->status == 1)
            charstatus = "Running";
        else if((*process_list)->status ==0)
            charstatus = "Suspended";
        
        printf ("PID	Command 	STATUS \n");
        printf ("%d	    %s     %s\n", (int)((*process_list)->pid), ((*process_list)->cmd)->arguments[0], charstatus);
        if (templist->status == -1) {
            currlist = templist;
            templist = templist->next;
            if(prevlist != NULL) 
                prevlist->next = templist;
            else *process_list = templist;
            freeProcessLine(currlist);
        }
        else {
            prevlist = templist;
            templist = templist->next;
        }
    }
}

void execute(cmdLine *pCmdLine) {
    if (pid == 0){
        execvp(pCmdLine->arguments[0],pCmdLine->arguments);
        perror("Error");
        _exit(EXIT_FAILURE);
    }
    else if (pid == -1) {
        _exit(EXIT_FAILURE);
    }
    else {
        addProcess(list, pCmdLine, pid);
        if (pCmdLine->blocking)
            waitpid(pid,NULL,0);
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
   list = malloc(sizeof(process*));

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
	if (strcmp(line->arguments[0],"procs")==0){
            printProcessList(list);
            continue;
        }
        if (strcmp(line->arguments[0],"suspend") == 0) {
            kill(atoi(line->arguments[1]), SIGTSTP);
            updateProcessStatus(*list, atoi(line->arguments[1]), 0);
            continue;
        }
        if (strcmp(line->arguments[0],"kill") == 0) {
            kill(atoi(line->arguments[1]), SIGINT);
            updateProcessStatus(*list, atoi(line->arguments[1]), -1);
            continue;
        }
        if (strcmp(line->arguments[0],"wake") == 0) {
            kill(atoi(line->arguments[1]), SIGCONT);
            updateProcessStatus(*list, atoi(line->arguments[1]), 1);
            continue;
        }
        pid=fork();
        if(d)
            debug(line);
        execute(line);
        
   }
}
