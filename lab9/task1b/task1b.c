#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <elf.h>

void *map_start;
struct stat fd_stat;
Elf32_Ehdr* header;

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *,int), int arg);
void printer(Elf32_Phdr* phdr, int arg);
void printl(Elf32_Phdr* phdr, int arg);

int main(int argc, char **argv) {
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("Can't open file\n");
        exit(1);
    }
    if (fstat(fd, &fd_stat) < 0) {
        perror("Failed to return information about a file\n");
        exit(1);
    }
    map_start = mmap(NULL, fd_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map_start == MAP_FAILED) {
        perror("Error in mmap - couldn't read file\n");
        munmap(map_start, fd_stat.st_size);
        close(fd);
        fd = -1;
        return 0;
    }
    header = (Elf32_Ehdr*) map_start;
    printf("Verify Iterator : \n"); // task0
    foreach_phdr(map_start, &printer , 0);
    printf("readelf -l procedure : \n"); // task1a
    printf("%-15s%-10s%-10s%-10s%-10s%-8s%-5s%-8s%-40s%-20s\n", "Type", "Offset", "VirtAddr", "PhysAddr", "Filesiz", "Memsiz", "Flg", "Align", "Protection Flags", "Mapping Flags");
    foreach_phdr(map_start, &printl, 0);
}

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *,int), int arg) {
    header = (Elf32_Ehdr*) map_start;
    for (int i = 0 ; i < header -> e_phnum ; i++) {
        func((Elf32_Phdr *)(map_start + header -> e_phoff + i * header -> e_phentsize) , i);
    }
    return 0;
}

void printer(Elf32_Phdr* phdr, int arg) {
    printf("Program header number %d at address %x\n", arg, phdr -> p_vaddr);
}

void printl(Elf32_Phdr* phdr, int arg) {
    char* type = "NULL";
    switch (phdr -> p_type) {
        case PT_LOAD : type = "LOAD"; break;
        case PT_DYNAMIC: type = "DYNAMIC"; break;
        case PT_INTERP: type = "INTERP"; break;
        case PT_NOTE: type = "NOTE"; break;
        case PT_SHLIB: type = "SHLIB"; break;
        case PT_PHDR: type = "PHDR"; break;
        case PT_TLS: type = "TLS"; break;
        case PT_NUM: type = "NUM"; break;
        case PT_LOOS: type = "LOOS"; break;
        case PT_GNU_EH_FRAME: type = "GNU_EH_FRAME"; break;
        case PT_GNU_STACK: type = "GNU_STACK"; break;
        case PT_GNU_RELRO: type = "GNU_RELRO"; break;
        case PT_SUNWBSS: type = "SUNWBSS"; break;
        case PT_SUNWSTACK: type = "SUNWSTACK"; break;
        case PT_HIOS: type = "HIOS"; break;
        case PT_LOPROC: type = "LOPROC"; break;
        case PT_HIPROC: type = "HIPROC"; break;
    }
    
    char *protectionFlags = calloc(1, 100);
    memset(protectionFlags, 0, 100);
    char *mappingFlags = "";
    
    char flag[3] = {' ',' ',' '};
    if (phdr -> p_flags & PF_R) {
        flag[0] = 'R';
        strcat(protectionFlags,"PROT_READ ");
    }
    if (phdr -> p_flags & PF_W) {
        flag[1] = 'W';
        strcat(protectionFlags,"PROT_WRITE ");
    }
    if (phdr -> p_flags & PF_X) {
        flag[2] = 'E';
        strcat(protectionFlags,"PROT_EXEC");
    }
    if (phdr -> p_type == PT_LOAD) {
        mappingFlags = "MAP_PRIVATE | MAP_FIXED";
    }   
    else {
        protectionFlags = "";
    }
    printf("%-15s%-10x%-10x%-10x%-10x%-8x%-5s%-8x%-40s%-20s\n",
           type, phdr -> p_offset,phdr -> p_vaddr,phdr -> p_paddr,phdr -> p_filesz,phdr -> p_memsz,flag, phdr -> p_align, protectionFlags, mappingFlags);
    //free(protectionFlags);
}
