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

int main(int argc, char **argv){
    int base_len = 5;
    char arr1[base_len];
    char* arr2 = map(arr1, base_len, my_get);
    char* arr3 = map(arr2, base_len, encrypt);
    char* arr4 = map(arr3, base_len, xprt);
    char* arr5 = map(arr4, base_len, decrypt);
    map(arr5, base_len, cprt);
}
