// Max_func.c
#include "min_func.c"

int max_func(int a, int b) {
    if (min_func(a, b) == a)
        return(b);
    else
        return(a);
}