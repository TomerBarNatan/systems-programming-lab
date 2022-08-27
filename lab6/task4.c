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

typedef struct pair{
    char* name;
    char* value;
    struct pair *next;
} pair;

pair** list = NULL;


void freePair(pair *templist){
    if (templist!= NULL){
        free (templist->name);
        free (templist->value);
        templist->next = NULL;
        free(templist);
    }
}

void override(pair *templist, char* newValue){
    free(templist->value);
    templist->value = malloc(sizeof(strlen(newValue)));
    strcpy(templist->value, newValue);
}

void setPair(cmdLine *pCmdLine){
    pair* newPair = malloc(sizeof(pair));
    char* temp = pCmdLine->arguments[1];
    newPair->name = malloc(sizeof(strlen(temp)));
    strcpy(newPair->name, temp);
    temp = pCmdLine->arguments[2];
    newPair->value = malloc(sizeof(strlen(temp)));
    strcpy(newPair->value, temp);
    newPair-> next = NULL;
    
    if (*list == NULL) {
        *list = newPair;
    }	
    else {
        pair* templist = *list;
        while (templist->next != NULL){
            if (!strcmp(templist-> name, pCmdLine->arguments[1])){
                override(templist, pCmdLine -> arguments[2]);
                freePair(newPair);
                return;
            }
        templist = templist->next;
        }
        if (!strcmp(templist->name, pCmdLine->arguments[1])){
            override(templist, pCmdLine -> arguments[2]);
            freePair(newPair);
            return;
            }
        templist->next = newPair;
    }
}

void vars(pair* templist){
    if (templist != NULL){
        printf("Pair name: %s, Pair value: %s\n", templist->name, templist->value);
        vars(templist->next);
    }
}

void replace(cmdLine *pCmdLine, int num, const char *newString){
    pair* templist = *list;
    while(templist != NULL){
        if (!strcmp(templist->name , newString)){
            replaceCmdArg(pCmdLine, num, templist-> value);
            return;
        }
        templist = templist->next;
    }
    perror("variable not found");
}

void delete(char* tempName){    
    pair* templist = *list;
	if (templist == NULL) {
        return;
    }  
	
    pair* prevlist = NULL;
    pair* currlist = NULL;
	
    while (templist!= NULL){
        if (!strcmp(templist->name, tempName)){
            currlist = templist;
            templist = templist->next;
            freePair(currlist);
            if(prevlist != NULL) 
                prevlist->next = templist;
            else *list = templist;
            return;
        }
        else {
            prevlist = templist;
            templist = templist->next;
        }
    }
	perror("variable not found");
}

void freeList(pair* templist){
    if (templist->next != NULL)
        freeList(templist->next);
    freePair(templist);
}

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
    
    if(tildaOrCd(pCmdLine))
        return;

    if (!strcmp(pCmdLine->arguments[0],"quit")){
        freeCmdLines(pCmdLine);
        freeList(*list);
        free(list);
        fprintf (stderr,"Program quiting");
        exit(0);
    }
    
    if (!strcmp(pCmdLine->arguments[0],"cd")){
        if (pCmdLine->argCount > 1) {
            if (!strcmp(pCmdLine->arguments[1], "~")) {
                if(chdir(getenv("HOME")) < 0)
                    fprintf (stderr,"cd failed.");
                return;
            }
            if(chdir(pCmdLine->arguments[1]) < 0) {
                perror("cd failed.");
            }
            return;
        }
    }       
    else if (!strcmp(pCmdLine->arguments[0], "set")) {
        setPair(pCmdLine);
        return;
    }

    else if (!strcmp(pCmdLine -> arguments[0], "vars")) {
        vars(*list);
        return;
    }

    if(!strcmp(pCmdLine->arguments[0], "delete")) {
        delete(pCmdLine->arguments[1]);
        return;
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

int tildaOrCd(cmdLine* pCmdLine){
    int found = 0;
    
    for (int i=0; i< pCmdLine->argCount; i++){
        char* arg = pCmdLine->arguments[i];
        if(!(strncmp(arg, "$",1))){
            replace(pCmdLine, i, arg +1);
            found = 1;
        }
    }
    
    if (found){
        execute(pCmdLine);
        return 1;
    }

    return 0; 
}

int main(int argc, char** argv) {
   char buf[PATH_MAX];
   char input[2048];
   cmdLine* line = NULL;
   list = malloc(sizeof(pair*));
   
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
