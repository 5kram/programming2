#include "objdb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DB_ERROR -1;
#define MN_SIZE 4
#define BLOCK 512
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
    
    for (i = 0; i < MN_SIZE + 1; i++) {
        if (fwrite(&val[i], 1, 1, *fp) != 1) {
            #ifdef DEBUG
                fprintf(stderr, "\nFunction: %s\nLine: %d\n", __func__, __LINE__); 
            #endif
            return 0;
        }
        fflush(*fp);
    }   
    return 1;
}
// Return 1 -> name exists in db
// Rerurn 0 -> name doesnt exist in db
int find_name (FILE **fp, char objname[]) {

    return 0;
}
// fp already in correct position
int move_block (FILE **fp, FILE **op, char objname[]) {
    char buffer[BLOCK] = {0};
    int namelen = 0, objsize = 0, repeats = 0, remain = 0, i;
    // Inserts objs info into data
    // Size of name + name + size of obj
    namelen = strlen(objname);
    fwrite(&namelen, sizeof(int), 1, *fp);
    fwrite(objname, sizeof(char), namelen, *fp);
    fseek(*op, 0, SEEK_END);
    objsize = ftell (*op);
    fseek(*op, 0, SEEK_SET);
    // Move bytes
    // if objsize < BLOCK, doesnt loop
    repeats = objsize / BLOCK;
    remain = objsize % BLOCK;
    for (i = 0; i < repeats; i++) {
        fread(buffer, BLOCK, 1, *op);
        fwrite(buffer, BLOCK, 1, *fp);
    }
    fread(buffer, remain, 1, *op);
    fwrite(buffer, remain, 1, *fp);
    fclose(*op);

    return 1;
}

// Return DB_ERROR(-1) -> Error Opening db
// Return 0 -> Invalid db
// Return 1 -> OK
int open (FILE **fp, char dbname[]) {
    int check;
    
    // Close any pre-existing open file
    if (*fp != NULL && ftell(*fp) >= 0) {
       close(&(*fp));
    }
    *fp = fopen(dbname, "rb+");
    // A file, with same name, already exists
    if (*fp != NULL) {
        check = DbValid(*fp);
        // Its db
        if (check) {
            return 1;
        }
        // Its not db
        close(&(*fp));
        return 0;
    }
    // Open new db
    else {
        *fp = fopen(dbname, "wb+");
        if (ferror(*fp)) {
            #ifdef DEBUG
                fprintf(stderr, "\nNew DB. Error in opening:\nFunction: %s\nLine: %d\n", __func__, __LINE__); 
            #endif
            return DB_ERROR;
        }
        check = metadata(&(*fp));
        if (!check) {
            #ifdef DEBUG
                fprintf(stderr, "\nNew DB. Error in Metadata:\nFunction: %s\nLine: %d\n", __func__, __LINE__); 
            #endif
            return DB_ERROR;
        }
    }
    return 1;
}
// Return -1 -> No db
// Return 0 -> No fname
// Return -2 -> Object name already in db
int import (FILE **fp, char fname[], char objname[]) {
    FILE *op;
    int check;
    // No open db
    if (*fp == NULL) {
        return DB_ERROR;
    }
    op = fopen (fname, "rb");
    // No existing file
    if (op == NULL ) {
        return 0;
    }

    check = find_name (&(*fp), objname);
    if (check) {
        return -2;
    }
    fseek(*fp, 0, SEEK_END);
    #ifdef DEBUG
        fprintf(stderr, "\n%ld\n", ftell(*fp)); 
    #endif
    check = move_block (&(*fp), &op, objname);



    return 1;
}
// Return DB_ERROR(-1) -> No open DB
// Return 1 -> OK
int close(FILE **fp) {
    if (*fp == NULL) {
        return DB_ERROR;
    }
    fclose(*fp);
    *fp = NULL;
    return 1;
}