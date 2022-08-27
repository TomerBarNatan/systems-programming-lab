#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct {
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[10000];
  size_t mem_count;
  /*
   .
   .
   Any additional fields you deem necessary
  */
} state;

typedef struct fun_desc {
  char *name;
  void (*fun)(state*);
}fun_desc;

void toggleDebugMode(state *s);
void setFileName(state *s);
void setUnitSize(state *s);
void loadIntoMemory(state *s);
void memoryDisplay(state* s);
void saveIntoFile(state* s);
void fileModify(state* s);
void quit(state* s);

int main(int argc, char **argv) {
    state *myState = calloc(1, sizeof(state));
    myState -> unit_size = 1;
    fun_desc menu[] = {
        {"Toggle Debug Mode", toggleDebugMode},
        {"Set File Name", setFileName},
        {"Set Unit Size", setUnitSize},
        {"Load Into Memory", loadIntoMemory},
        {"Memory Display", memoryDisplay},
        {"Save Into File", saveIntoFile},
        {"File Modify", fileModify},
        {"Quit", quit},
        {NULL, NULL}
    };
    int number;

    while(1){
        if (myState -> debug_mode) {
            printf("Unit Size : %d\nFile Name : %s\nMem Count : %d\n",
                myState -> unit_size, myState -> file_name, myState -> mem_count);
        }
        
        printf("Choose action: \n");
        for (int i=0 ; i < 8 ; i++) {
            printf("%d - %s\n", i, menu[i].name);
        }
        
        char c = fgetc(stdin);
        sscanf(&c, "%d", &number);
        fgetc(stdin);
        if (0 <= number && number < 8) {
            menu[number].fun(myState);        
        }
    }
    return 0;
}

void toggleDebugMode(state *s) {
    if (s -> debug_mode == 1) {
        printf("Debug flag now off\n");
        s -> debug_mode = 0;
        return;
    }
    printf("Debug flag now on\n");
    s -> debug_mode = 1;
}

void setFileName(state *s) {
    printf("Please enter a file name : ");
    char buf[101];
    fgets(buf, 100, stdin);
    buf[strlen(buf) -1] = 0;
    strcpy(s -> file_name, buf);
    if (s -> debug_mode) {
        printf("Debug: file name set to '%s'\n", buf);
    }
}

void setUnitSize(state *s) {
    char buf[10];
    printf("Please enter unit size: ");
    fgets(buf, 10, stdin);
    int input = atoi(buf);
    if (input == 1 || input == 2 || input == 4){
        s -> unit_size = input;
        if (s -> debug_mode) { 
            printf("Debug: set size to %d\n", input);
        }
    }
    else {
        printf("Invalid unit size\n");
    }
}

void loadIntoMemory(state *s) { // task1a
    char locationAndLength[256];
    int length;
    int location;
    FILE* file;
    
    if (s -> file_name == NULL) { // 1
        printf("Can't find any file");
        return;
    }
    
    file = fopen(s -> file_name, "r"); //2
    if (file == NULL){
        printf("Error while opening the file\n"); 
        return;
    }
    
    printf("Please enter <location> <length>\n"); //3
    fgets(locationAndLength, 256, stdin);
    sscanf(locationAndLength,"%x %d", &location, &length);
    
    if (s -> debug_mode){ //4
        printf("File Name : %s\nLocation : %x\nLength : %d\n",
            s -> file_name, location, length);
    }

    fseek(file, location, SEEK_SET); //5
    fread(s -> mem_buf, length, s->unit_size, file);
    s -> mem_count += length;
    
    printf("Loaded %d units into memory\n", length);
    
    fclose(file); //6
}

void memoryDisplay(state* s) { // task1b
    char buf[256];
    int u;
    int addr;
    printf("Please enter <u> <addr>\n");
    fgets(buf, 256, stdin);
    sscanf(buf, "%d %x", &u, &addr);
    printf("Decimal\tHexadecimal\n");
    printf("=============\n");
    char* buffer = (char*)&addr ;
    if(addr == 0){
        buffer = (char*)s->mem_buf;
    }
    char* end = buffer + (s -> unit_size) * u;
    while (buffer < end) {
        int var = *((int*)(buffer));
        if (s -> unit_size == 1 || s -> unit_size == 4) {
            printf("%#hhu\t%02hhX\n", var, var);
        }
        else if (s -> unit_size == 2) {
            printf("%#hu\t%02hX\n", var, var);
        }
        else {
            printf("Invalid unit");
        }  
        buffer += s -> unit_size;
    }
}

void saveIntoFile(state* s) { // task1c
    char buf[256];
    int sourceAddress;
    int targetLocation;
    int length;
    FILE* file;
    
    printf("Please enter <source-address> <target-location> <length>\n");
    fgets(buf, 256, stdin);
    sscanf(buf, "%x %x %d", &sourceAddress, &targetLocation, &length);
    file = fopen(s -> file_name, "r+");
    if (file == NULL){
        printf("Error while opening the file\n"); 
        return;
    }
    
    fseek(file, 0, SEEK_END); // the note
    if (ftell(file) < targetLocation) {
        printf("Out of bound");
        rewind(file);
        return;
    }
    rewind(file);
    
    fseek(file, targetLocation, SEEK_SET);
    if (sourceAddress == 0) {
        fwrite(s -> mem_buf, length, s -> unit_size, file);
    }
    else {
        fwrite(&sourceAddress, length, s -> unit_size, file);
    }
    fclose(file);
}

void fileModify(state* s) { // task1d
    char buf[256];
    int location;
    FILE* file;
    int val;
    
    printf("Please enter <location> <val>\n");
    fgets(buf, 256, stdin);
    sscanf(buf, "%x %x", &location, &val);
    file = fopen(s -> file_name, "r+");
    if (file == NULL){
        printf("Error while opening the file\n"); 
        return;
    }
    
    fseek(file, 0, SEEK_END); // the note
    if (ftell(file) < location) {
        printf("Out of bound");
        rewind(file);
        return;
    }
    rewind(file);
    
    fseek(file, location, SEEK_SET);
    fwrite(&val, 1, s -> unit_size, file);
    fclose(file);
}

void quit(state *s) {
    printf("quitting\n");
    free(s);
    exit(0);
}
