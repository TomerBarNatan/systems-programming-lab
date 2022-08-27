#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char censor(char c) {
  if(c == '!')
    return '.';
  else
    return c;
}

char* map(char *array, int array_length, char (*f) (char)){
	char* mapped_array = (char*)(malloc(array_length*sizeof(char))); 
        for (int i=0; i<array_length;i++)
            mapped_array[i] = f(array[i]);
        return mapped_array;
}

char encrypt(char c){
        if (c>=0x20 && c<=0x7E)
            return c+3;
        else return c;
}

char decrypt(char c){
        if (c>=0x20 && c<=0x7E)
            return c-3;
        else return c;
}

char xprt(char c){
    printf("0x%X\n", c);
    return c;
}

char cprt(char c){
    if (c>=0x20 && c<=0x7E)
        printf("%c\n", c);
    else printf(".\n");
    return c;
}

char my_get(char c){
    char a = fgetc(stdin);
    return a;
}

char quit(char c){
    exit(0);
    return c;
}

struct fun_desc {
  char *name;
  char (*fun)(char);
};

int main(int argc, char **argv){
	char newarray[5]="";
	char* carray = &newarray[0];
	struct fun_desc menu[] = {{"Censor", &censor}, {"Encrypt", &encrypt}, {"Decrypt", &decrypt} , {"Print hex", &xprt} , {"Print string", &cprt}, {"Get string", &my_get}, {"Quit" , &quit}, {0,0}};
	int chosen;
	char* temparr;
        int size = sizeof(menu)/sizeof(menu[0]) -1;
	while(1){
		printf("%s\n", "Please choose a function:");
		for(int i =0; i<size; i++)
			printf("%d) %s\n", i, menu[i].name);
		printf("Option: ");
                char input;
		input = fgetc(stdin);
		sscanf(&input, "%d", &chosen);
		if (chosen>=0 && chosen<=size-1){
			printf("Within bounds\n");
                        fgetc(stdin);
			temparr = map(carray , 5 , menu[chosen].fun);
			carray = temparr;
			printf("%s\n\n" , "Done.");
		}
		else {
			printf("Not within bounds\n");
			quit(0);
		}
	}
}
