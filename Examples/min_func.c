// #ifndef _____ - #define_____ - #endif
// To avoid recursive inclusions
#ifndef INCL_MYPROGMIN
#define INCL_MYPROGMIN

int min_func(int a, int b) {
    if (a < b)
        return a;
    else
        return b;
}
#endif