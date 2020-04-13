// Dynamic Memory and Backwards Dup
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *BackDup (char *str) {
    char *back_ptr;
    int i;
    
    printf("strlen -> %ld\n", strlen(str));
    back_ptr = (char *)malloc(sizeof(char) * strlen(str));
    
    for (i = strlen(str); i >= 0; i--) {
            back_ptr[i] = str[i]; 
    }

    return back_ptr;
}



int main (int argc, char *argv[]) {
    char *back_ptr;

    if (argc != 2) {
        return 0;
    }

    back_ptr = BackDup(argv[1]);
    printf("BackDup -> %s\n", back_ptr);

    return 0;
}