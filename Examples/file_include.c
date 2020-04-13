// File inclusion
#include <stdio.h>
#include "min.c"
#include "max.c"
#include "min_func.c"

int main(int argc, char *argv[]) {
    int a, b;
    
    printf("Enter 2 digits: \n");
    scanf(" %d %d", &a, &b);
    printf("Max is %d. Min is %d\n", max(a,b), min(a,b));
    printf("Minfunc is %d\n", min_func(a, b));

    return 0;
}