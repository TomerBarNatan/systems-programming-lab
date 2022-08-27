#include <stdio.h>
#include <string.h>

void addfun(char* addstr, int dm, FILE* inputF){
    int output = 0;
    int c;
    c = fgetc(inputF);
    int index = 2;
    while (c != EOF){
            output = (c + addstr[index])%128;
            if (index >= strlen(addstr) -1)
                index = 2;
            else index ++;
            fputc(output, stdout);
            if (dm){
                fprintf (stderr, "%#04x\t", c);
                fprintf (stderr, "%#04x\n", output);
            }
            if(c == '\n'){
                fputc('\n', stdout);
                index = 2;
            }
            c = fgetc(inputF);
		}
}

void redfun(char* redstr, int dm, FILE* inputF){
    int output = 0;
    int c;
    c = fgetc(inputF);
    int index = 2;
     while (c != EOF){
                if (c!= '\n'){
			output = (c - redstr[index])%128;
                        if (index >= strlen(redstr) -1)
                            index = 2;
                        else index ++;
                        if (output < 0)
                            output = output + 128;
			fputc(output, stdout);
			if (dm){
                            fprintf (stderr, "%#04x\t", c);
                            fprintf (stderr, "%#04x\n", output);
			}
                }
                else{
                        fputc('\n', stdout);
                        index = 2;
                    }
			c = fgetc(inputF);
		}
}
void elsefun(int dm, FILE* inputF){
    int c;
    c = fgetc(inputF);
    while (c != EOF){
        if(dm){
            fprintf(stderr, "%#04x\t", c);
            if (c >= 'A' && c <= 'Z')
                fprintf(stderr, "%#04x\n", c+32);
            else fprintf(stderr, "%#04x\n", c);
        }
        if (c >= 'A' && c <= 'Z')
                fputc(c+32, stdout);
        else fputc(c, stdout);
        if(c == '\n') {
                fputc('\n', stdout);
            }
        c = fgetc(inputF);
            }
}


int main(int argc, char **argv){
	int dm = 0;
        int add = 0;
        int red = 0;
        char* addstr;
        char* redstr;
        int infile = 0;
        FILE* inputF = stdin;
        for (int i = 0; i<argc; i++){
            if(strcmp(argv[i], "-D") == 0)
                dm =1;
            if (strncmp(argv[i], "+e", 2) == 0){
                add = 1;
                addstr = argv[i];
            }
            if (strncmp(argv[i], "-e", 2) == 0){
                red = 1;
                redstr = argv[i];
            }
            if (strncmp(argv[i], "-i" , 2)==0){
                infile = 1;
                inputF = fopen(argv[i]+2, "r");
            }
        }
                
        if (add){
            addfun(addstr, dm, inputF);
        }
        else if (red){
           redfun(redstr, dm, inputF);
        }
        else {
            elsefun(dm, inputF);
        }
        
        if(infile)
            fclose(inputF);
	return 0;
}
