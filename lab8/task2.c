#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>

typedef struct fun_desc {
  char *name;
  void (*fun)();
}fun_desc;

int debugMode;
void* addr = NULL;
int Currentfd = -1;
struct stat sb;
Elf32_Ehdr* header;

void toggleDebugMode();
void examineELFFile();
void printSectionNames();
void printSymbols();
void quit();
Elf32_Shdr* calculate(int i);
Elf32_Sym* calculate2(Elf32_Shdr* mySection, int j);

int main(int argc, char **argv) {
    int number;
    fun_desc menu[] = {
        {"Toggle Debug Mode", toggleDebugMode},
        {"Examine ELF File", examineELFFile},
        {"Print Section Names", printSectionNames},
        {"Print Symbols", printSymbols},
        {"Quit", quit},
        {NULL, NULL}
    };
    
    while(1){
        printf("Choose action: \n");
        for (int i=0 ; i < 5 ; i++) {
            printf("%d - %s\n", i, menu[i].name);
        }
        char c = fgetc(stdin);
        sscanf(&c, "%d", &number);
        fgetc(stdin);
        if (0 <= number && number < 5) {
            menu[number].fun();        
        }
    }
    return 0;
}

void toggleDebugMode() {
    if (debugMode) {
        printf("Debug flag now off\n");
        debugMode = 0;
        return;
    }
    printf("Debug flag now on\n");
    debugMode = 1;
}

void examineELFFile() {
    char filename[100];
    
    printf("Please enter file name: \n");
    fgets(filename, 100, stdin);
    sscanf(filename, "%s", filename);
    if (debugMode) {
        printf("File name set to: %s\n", filename);
    }
    
    if (Currentfd > 0) {
        close(Currentfd);
    }

    Currentfd = open(filename, O_RDONLY);
    if (Currentfd < 0) {
        perror("Error - couldn't open file\n");
        return;
    }
    
    if (fstat(Currentfd, &sb) == -1) {
        perror("Error to obtain file size\n");
        return;
    }
    
    addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, Currentfd, 0);
    if (addr == MAP_FAILED) {
        perror("Error in mmap - couldn't read file\n");
        munmap(addr, sb.st_size);
        close(Currentfd);
        Currentfd = -1;
        return;
    }
    
    header = (Elf32_Ehdr*)addr;
    printf("Magic number 1 : %d , Magic number 2 : %d , Magic number 3 : %d\n", //1
           header -> e_ident[1], header -> e_ident[2], header -> e_ident[3]);
    printf("Data encoding : %d\n", header -> e_type); //2
    printf("Entry point : %x\n", header -> e_entry); // 3
    printf("The file offset in which the section header table resides : %d\n", header -> e_shoff); // 4
    printf("The number of section header entries : %d\n", header -> e_shnum); // 5
    printf("The size of each section header entry : %d\n", header -> e_shentsize); // 6
    printf("The file offset in which the program header table resides : %d\n", header -> e_phoff); // 7
    printf("The number of program header entries : %d\n", header -> e_phnum); // 8
    printf("The size of each program header entry : %d\n", header -> e_phentsize); // 9
}

void printSectionNames() { //task1
    if (Currentfd < 0) {
        perror("Error - file currently unmapped\n");
        return;
    }
    char *s = (char*) (addr + (calculate(header -> e_shstrndx)) -> sh_offset);
    printf("Index Section\t\t\tAddress  Offset Size   Type\n");
    for (int i = 0 ; i < header -> e_shnum ; i ++) {
        Elf32_Shdr* mySection = calculate(i);
        printf("[%02d]  %-25s %08x %06x %06x %d\n", i, &s[mySection -> sh_name],
               mySection -> sh_addr, mySection -> sh_offset, mySection -> sh_size, mySection -> sh_type);
    }
}

void printSymbols() { //task2
    Elf32_Shdr* mySection;
    
    for (int i = 0 ; i < header -> e_shnum ; i++) {
        mySection = calculate(i);
        if (mySection -> sh_type == SHT_SYMTAB) {
            break;
        }
    }
    
    if ((mySection -> sh_size) == 0) {
        perror("No symbols to print");
        return;
    }
    
    char *s = (char*) (addr + (calculate(mySection -> sh_link)) -> sh_offset);
    printf("Index Value\tIndex  Section\t\tSymbol\n");
    for (int i = 0 ; i < ((mySection -> sh_size)/(sizeof (Elf32_Sym))) ; i++) {
        Elf32_Sym* mySymbol = calculate2(mySection, i);
        printf("[%02d]  %08x  %5d  ", i, mySymbol -> st_value, mySymbol -> st_shndx);
        if (mySymbol -> st_shndx == SHN_ABS) {
            printf("ABS\t\t");
        }
        else {
            printf("UND\t\t");
        } 
        printf("%-30s\n", &s[mySymbol->st_name]);
    }
}

void quit() {
    if (Currentfd > 0){
        close(Currentfd);
        munmap(addr, sb.st_size);
    }
    printf("Quit due to quit command\n");
    exit(0);
}

Elf32_Shdr* calculate(int i) {
    return (i * (header -> e_shentsize) + addr + header -> e_shoff);
}

Elf32_Sym* calculate2(Elf32_Shdr* mySection, int j){
    return ((j * sizeof(Elf32_Sym)) + addr + mySection -> sh_offset);
}
