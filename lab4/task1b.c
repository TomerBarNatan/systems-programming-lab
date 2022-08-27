#include "util.h"

#define STDIN 0
#define STDOUT 1
#define ERROR 2
#define SYS_READ 3
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_WRITE 4
#define SYS_LSEEK 19

#define O_WRITEONLY 1
#define O_CREATE 64
#define O_READONLY 0

extern int system_call();

void printDebug(int debugMode,int id,int ret){
    if(debugMode){
        system_call(SYS_WRITE,ERROR,"\nID = ",6);
        system_call(SYS_WRITE,ERROR,itoa(id),strlen(itoa(id)));
        system_call(SYS_WRITE,ERROR,"\nReturn Code = ",15);
        system_call(SYS_WRITE,ERROR,itoa(ret),strlen(itoa(ret)));
        system_call(SYS_WRITE,ERROR,"\n",1);
    }
}
void printLowerCase(int debugMode,int fileIn,int fileOut){
    char c;
    int readReturn=1;
    int writeReturn=0;
    while(readReturn){
        readReturn=system_call(SYS_READ,fileIn,&c,1);
        printDebug(debugMode,SYS_READ,readReturn);
        if('A' <= c && c <= 'Z'){
            c += 'a'-'A';
        }
        writeReturn = system_call(SYS_WRITE,fileOut,&c,1);
        printDebug(debugMode,SYS_WRITE,writeReturn);
    }
}

void printIOpaths(int debugMode,char* fileInMode,char* fileOutMode){
    if(debugMode){
        system_call(SYS_WRITE,ERROR,"Input: ",7);
        system_call(SYS_WRITE,ERROR,fileInMode,strlen(fileInMode));
        system_call(SYS_WRITE,ERROR,"\nOutput: ",9);
        system_call(SYS_WRITE,ERROR,fileOutMode,strlen(fileOutMode));
        system_call(SYS_WRITE,ERROR,"\n",1);
    }
}

int main(int argc , char* argv[], char* envp[]){
    int debugMode = 0;
    int i=0;
    char* fileInMode = "stdin";
    char* fileOutMode = "stdout";
    int fileIn = STDIN;
    int fileOut = STDOUT;
    for(;i<argc;i++){
        if(!strncmp(argv[i],"-D",2)){
            debugMode = 1;
        }
        if(!strncmp(argv[i],"-i",2)){
            fileIn = system_call(SYS_OPEN,argv[i]+2,O_READONLY,0644);
            fileInMode = argv[i]+2;
        }
        if(!strncmp(argv[i],"-o",2)){
            fileOut = system_call(SYS_OPEN,argv[i]+2,O_WRITEONLY | O_CREATE,0644);
            fileOutMode = argv[i]+2;
        }
    }
    printIOpaths(debugMode,fileInMode,fileOutMode);
    printLowerCase(debugMode,fileIn,fileOut);
    system_call(6,fileIn);
    system_call(6,fileOut);
    return 0;
}
