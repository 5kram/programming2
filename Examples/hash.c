// Hash Table
#include <stdio.h>

unsigned long hash (char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        //printf("c = %c hash = %ld\n", c, hash);
        //printf("hash << 5 -> %lu\nhash << 5 + hash -> %lu\n", hash << 5, (hash << 5) + hash);
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

int main (int argc, char *argv[]) {
    unsigned long index;
    char name[15];
    
    scanf(" %s", name);
    index = hash (name);
    printf("index -> %lu\n", index%4);


    return 0;
}