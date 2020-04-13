// Dynamic Memory
#include <stdio.h>
#include <stdlib.h>

int *FuncSum(int a, int b){
    int *sum;

    sum = (int *)malloc(sizeof(int));
    *sum = a + b;

    return sum;
}

int main (int argc, char *argv[]) {
    int a, b, *sum;

    scanf(" %d %d", &a, &b);
    sum = FuncSum(a, b);
    printf("Sum -> %d\n", *sum);
    // Always!
    free(sum);

    return 0;
}