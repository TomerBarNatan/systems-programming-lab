int digit_cnt(char *input) {
    int counter = 0;
    int i = 0;
    
    while (input[i] != '\0') {
        if ('0' <= input[i] && input[i] <= '9') {
            counter++;
        }
        i++;
    }
    return counter;
}

int main(int argc, char** argv){
    return 0;
}
