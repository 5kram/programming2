// Include2
// Without Recursion
#include <stdio.h>
#include "min_func.c"
#include "max_func.c"

int main(int argc, char *argv[]) {
    int a,b;

    scanf(" %d %d",&a ,&b);
    printf("%d %d\n", min_func(a, b), max_func(a, b));
    
    return(0);
}