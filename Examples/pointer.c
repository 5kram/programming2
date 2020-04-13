// Pointer Example
#include <stdio.h>

int main (int argc, char *argv[]) {
    char *ptr, str[] = "hello!";
    int *ptrint;

    printf("str -> %s\n", str);
    ptr = str;
    do {
        printf("ptr -> %c\n", *ptr);
        ptr++;
    }
    while (*ptr != '\0');
    
    // Type cast to int ptr
    ptrint = (int *)str;
    printf("int -> %d hex -> %x\n", *ptrint, *ptrint);

    return 0;
}