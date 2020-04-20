#include "objdb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DB_ERROR -1;
#define MN_SIZE 4
#define BLOCK 512
#define NAME_LEN 255
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
// Return 1 -> End of file
// Return 0 -> Not end of file
int fend (FILE **fp) {
    fseek(*fp, 1, SEEK_CUR);
    #ifdef DEBUG
        fprintf(stderr, "\nEnd: %ld? Function: %s, Line: %d\n", ftell(*fp), __func__, __LINE__); 
    #endif
    if (feof(*fp)) {
        return 1;
    }
    fseek(*fp, -1, SEEK_CUR);
    return 0;
}
// Return 1 -> name exists in db
// Rerurn 0 -> name doesnt exist in db
// fp in correct position
int find_name (FILE **fp, char name[]) {
    int objnamelen = 0, objsize = 0;
    char objname[NAME_LEN] = {0};

    fseek(*fp, MN_SIZE + 1, SEEK_SET);
    do {    
        fread(&objnamelen, sizeof(int), 1, *fp);
        fread(objname, sizeof(char), objnamelen, *fp);
        #ifdef DEBUG
            fprintf(stderr, "\n%d, %s, Function: %s, Line: %d\n", objnamelen, objname, __func__, __LINE__); 
        #endif
        if (!strcmp(name, objname)) {
            return 1;
        }
        fread(&objsize, sizeof(int), 1, *fp);
        fseek(*fp, objsize, SEEK_CUR);
    }
    while (fend(&(*fp)));

    return 0;
}
// fp in correct position
int move_block (FILE **fp, FILE **op, char objname[]) {
    char buffer[BLOCK] = {0};
    int namelen = 0, objsize = 0, repeats = 0, remain = 0, i;
    fseek(*fp, 0, SEEK_END);
    #ifdef DEBUG
        fprintf(stderr, "\nImport in end: %ld, Function: %s, Line: %d\n", ftell(*fp), __func__, __LINE__); 
    #endif
    // Inserts objs info into data
    // Size of name + name + size of obj
    namelen = strlen(objname);
    fwrite(&namelen, sizeof(int), 1, *fp);
    fwrite(objname, sizeof(char), namelen, *fp);
    fseek(*op, 0, SEEK_END);
    objsize = ftell (*op);
    fseek(*op, 0, SEEK_SET);
    fwrite(&objsize, sizeof(int), 1, *fp);
    #ifdef DEBUG
        fprintf(stderr, "\n%d, %s, %d, Function: %s, Line: %d\n", namelen, objname, objsize, __func__, __LINE__); 
    #endif
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