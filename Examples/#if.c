// #if - #elif - #endif
// Run: gcc -D OPTION=2 #if.c -o if
#include <stdio.h>

int main(int argc, char *argv[]) {

#if OPTION == 1
    printf("Code option 1\n");
#elif OPTION == 2
    printf("Code option 2\n");
#else
    printf("Default code\n");
#endif

return 0;
}