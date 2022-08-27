#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    char sig[];
} virus;

void PrintHex(char* buffer, int length){
    int i=0;
	for(;i<length;i++)
		 printf("%02X ", (unsigned char)buffer[i]);
	printf("\n");
    }
    
typedef struct link link;

struct link {
    link *nextVirus;
    virus *vir;
};

link* l;
char input[10000];

/* Print the data of every link in list. Each item followed by a newline character. */
void list_print(link *virus_list) {
    if(virus_list!=NULL){
        list_print(virus_list->nextVirus);
        printf("Virus name: ");
        printf("%s\n", (virus_list->vir)->virusName);
        printf("Virus size: %d\n",(virus_list->vir)->SigSize);
        printf("signature: \n");
        PrintHex((virus_list->vir)->sig, (virus_list->vir)->SigSize);
        printf("\n");
    }
}

/* Add a new link with the given data to the list
  (either at the end or the beginning, depending on what your TA tells you),
  and return a pointer to the list (i.e., the first link in the list).
  If the list is null - create a new entry and return a pointer to the entry. */
link* list_append(link* virus_list, virus* data) {
    if(virus_list->vir == NULL){
        virus_list->vir = data;
        return virus_list;
    }
    link* newLink = (link*)malloc(sizeof(link));
    newLink->vir = data;
    newLink->nextVirus = virus_list;
    return newLink;
}


void list_free(link *virus_list){
    free(virus_list->vir);
    link* nextVirus = virus_list->nextVirus;
    free(virus_list);
    if(nextVirus!=NULL)
        list_free(nextVirus);
}

void load(char* text){
    FILE* sign = fopen(text, "r");
    unsigned short size;
    while(fread(&size, 2, 1, sign) != 0){
        virus* v = (virus*)malloc(size);
        size -= 18;
        v->SigSize = size;
        fread(v->virusName, 16, 1, sign);
        fread(v->sig, (v->SigSize), 1, sign);
        l = list_append(l, v);
    }
    fclose(sign);
}

void detect_virus(char *buffer, unsigned int size){
    link* list = l;
    while(list !=NULL){
        int i=0;
        while (i<size){
            if(memcmp(&buffer[i], list->vir->sig, list->vir->SigSize) == 0) {
                printf("The starting byte location in the suspected file: %d\nThe virus name: %s\nThe size of the virus signature: %d\n",i, list->vir->virusName, list->vir->SigSize);
            }
            i++;
        }
        list = list->nextVirus;
    }
}

int main(int argc, char **argv) {
   l=(link*)malloc(sizeof(link));
    int isLoad = 0;
    char* menu[] = {"Load signatures", "Print signatures", "Detect viruses", "Quit"};
    int chosen;
    while(1){
        printf("%s\n", "Please choose a function:");
        int i =0;
        for(; i<4; i++)
            printf("%d) %s\n", i+1, menu[i]);
        printf("Option: ");
        char c=fgetc(stdin);
        sscanf(&c, "%d", &chosen);
        fgetc(stdin);
        if (chosen == 1){
            printf("%s\n", "Please enter a file name");
            char fileName[256];
            fgets(fileName, 256, stdin);
            sscanf(fileName, "%s", fileName);
            load(fileName);
            isLoad = 1;
        }
        else if (chosen == 2){
            if(isLoad == 1)
                list_print(l);
        }
        else if(chosen==3){
            if (isLoad == 1){
                printf("%s\n", "Please enter a file name");
                char fileName[256];
                fgets(fileName, 256, stdin);
                sscanf(fileName, "%s", fileName);
                FILE* file = fopen(fileName, "r");
                unsigned int minsize = 10000;
                fseek(file, 0, SEEK_END);
                if (ftell(file)<10000)
                    minsize = ftell(file);
                rewind(file);
                fread(input, minsize, 1, file);
                detect_virus(input, minsize);
                fclose(file);
            }
        }
        else if(chosen == 4){
            list_free(l);
            exit(0);
        }
        else exit(0);
    }
    return 0;
}
