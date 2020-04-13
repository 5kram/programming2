// #ifdef - #else - #endif 
// Run: gcc -Wall -g ifdef.c -o ifdef -D DEBUG
#include <stdio.h>

int main (int argc, char *argv[]) {

#ifdef DEBUG
    printf("Debug code\n");
#else
    printf("Release code\n");
#endif

    return 0;
}