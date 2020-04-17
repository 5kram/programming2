#include "objdb.h"
#include <stdio.h>
#include <stdlib.h>
#define DB_ERROR -1;
#define MN "B453L1N3"
#define MN_SIZE 8


int open (char dbname[]) {
    FILE *fp;

    fp = fopen(dbname, "r+");
    if (fp == NULL) {
        fp = fopen(dbname, "w+");
        if (ferror(fp)) {
            fprintf(stderr, "\nFunction: %s\nLine: %d\n", __func__, __LINE__);
            return DB_ERROR;
        }
        fwrite(MN, 1, MN_SIZE, fp);
        return 1;
    }

    
    return 1;
}