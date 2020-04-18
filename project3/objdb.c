#include "objdb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DB_ERROR -1;
#define MN_SIZE 4
#define DEBUG

// Validation of DB
// Checks the Existance of Magic Number
// Returns 0 -> Not Valid
// Returns 1 -> Valid
int DbValid(FILE *fp) {
    int i, val[] = {218, 122, 186, 83}, buf[MN_SIZE] = {0};
    
    fseek(fp, 0, SEEK_SET);
    for (i = 0; i < MN_SIZE; i++) {
        if (fread(&buf[i], 1, 1, fp) != 1) {
            #ifdef DEBUG
                fprintf(stderr, "\nFunction: %s\nLine: %d\n", __func__, __LINE__);
            #endif
            return 0;
        }
    }
    for (i = 0; i < MN_SIZE; i++) {
        if (val[i] != buf[i]) {
            return 0;
        }
    }
    return 1;
}
// Metadata Insertion
// Return 0 -> ERROR
// Return 1 -> OK
int metadata (FILE **fp) {
    int i, val[] = {218, 122, 186, 83};
    
    for (i = 0; i < 4; i++) {
        if (fwrite(&val[i], 1, 1, *fp) != 1) {
            #ifdef DEBUG
                fprintf(stderr, "\nFunction: %s\nLine: %d\n", __func__, __LINE__); 
            #endif
            return 0;
        }
    }   
    return 1;
}
// Return DB_ERROR(-1) -> Error Opening db
// Return 0 -> Invalid db
// Return 1 -> OK
int open (char dbname[]) {
    FILE *fp;
    int check;

    fp = fopen(dbname, "r+");
    // A file, with same name, already exists
    if (fp != NULL) {
        check = DbValid(fp);
        if (!check) {
            #ifdef DEBUG
                fprintf(stderr, "\nExists. Its not a DB\n");
            #endif
            return 0;
        }
        #ifdef DEBUG
            fprintf(stderr, "\nExists. Its a DB\n");
        #endif
    }
    // Open new DB
    else {
        fp = fopen(dbname, "w+");
        if (ferror(fp)) {
            #ifdef DEBUG
                fprintf(stderr, "\nNew DB. Error in opening:\nFunction: %s\nLine: %d\n", __func__, __LINE__); 
            #endif
            return DB_ERROR;
        }
        check = metadata(&fp);
        if (!check) {
            #ifdef DEBUG
                fprintf(stderr, "\nNew DB. Error in Metadata:\nFunction: %s\nLine: %d\n", __func__, __LINE__); 
            #endif
            return DB_ERROR;
        }
    }
    return 1;
}
