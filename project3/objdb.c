#include "objdb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DB_ERROR -1;
#define MN_SIZE 4
#define BLOCK 512
#define NAME_LEN 255
//#define DEBUG

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
void fexit (FILE **fp,const char func[],const int line) {
    fprintf(stderr, "\nError in function: %s\nLine: %d\n", func, line);
    close(&(*fp));
    EXIT_FAILURE;
} 
// Validation of DB
// Checks the Existance of Magic Number
// Returns 0 -> Not Valid
// Returns 1 -> Valid
int DbValid(FILE *fp) {
    int i, val[] = {218, 122, 186, 83}, buf[MN_SIZE] = {0};
    
    fseek(fp, 0, SEEK_SET);
    for (i = 0; i < MN_SIZE; i++) {
        if (fread(&buf[i], 1, 1, fp) != 1) {
            fexit(&fp, __func__, __LINE__);
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
            fexit(&(*fp), __func__, __LINE__);
        }
        fflush(*fp);
    }   
    return 1;
}
// Return 0 -> End of file
// Return 1 -> Not end of file
int fend (FILE **fp) {
    #ifdef DEBUG
        fprintf(stderr, "\nEnd: %ld? Function: %s, Line: %d\n", ftell(*fp), __func__, __LINE__); 
    #endif
    if (getc(*fp) == EOF) {
        return 0;
    }
    fseek(*fp, -1, SEEK_CUR);
    #ifdef DEBUG
        fprintf(stderr, "\nNo End: %ld Function: %s, Line: %d\n", ftell(*fp), __func__, __LINE__); 
    #endif
    return 1;
}

// Return 1 -> name exists in db
// Rerurn 0 -> name doesnt exist in db
// Return -1(DB_ERROR) -> 
// fp in correct position
int find_name (FILE **fp, char name[]) {
    int objnamelen = 0, objsize = 0;
    char objname[NAME_LEN] = {0};

    fseek(*fp, MN_SIZE + 1, SEEK_SET);
    while (fend(&(*fp))) {
        objname[1] = '\0';
        if (fread(&objnamelen, sizeof(int), 1, *fp) != 1) {
            fexit(&(*fp), __func__, __LINE__);
        }
        if (fread(objname, sizeof(char), objnamelen, *fp) != objnamelen) {
            fexit(&(*fp), __func__, __LINE__);
        }
        objname[objnamelen] = '\0';
        #ifdef DEBUG
            fprintf(stderr, "\n%d, %s, Function: %s, Line: %d\n", objnamelen, objname, __func__, __LINE__); 
        #endif
        if (!strcmp(name, objname)) {
            return 1;
        }
        
        if (fread(&objsize, sizeof(int), 1, *fp) != 1 ) {
            fexit(&(*fp), __func__, __LINE__);
        }
        fseek(*fp, objsize, SEEK_CUR);
    }

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
    if (fwrite(&namelen, sizeof(int), 1, *fp) != 1) {
        fexit(&(*fp), __func__, __LINE__);
    }
    fflush(*fp);
    if (fwrite(objname, sizeof(char), namelen, *fp) != namelen) {
        fexit(&(*fp), __func__, __LINE__);
    }
    fflush(*fp);
    fseek(*op, 0, SEEK_END);
    objsize = ftell (*op);
    fseek(*op, 0, SEEK_SET);
    if (fwrite(&objsize, sizeof(int), 1, *fp) != 1) {
       fexit(&(*fp), __func__, __LINE__);
    }
    fflush(*fp);
    #ifdef DEBUG
        fprintf(stderr, "\n%d, %s, %d, Function: %s, Line: %d\n", namelen, objname, objsize, __func__, __LINE__); 
    #endif
    // Move bytes
    // if objsize < BLOCK, doesnt loop
    repeats = objsize / BLOCK;
    remain = objsize % BLOCK;
    for (i = 0; i < repeats; i++) {
        if (fread(buffer, BLOCK, 1, *op) !=  1) {
            fexit(&(*op), __func__, __LINE__);
        }
        if (fwrite(buffer, BLOCK, 1, *fp) != 1) {
            fexit(&(*fp), __func__, __LINE__);
        }
        fflush(*fp);
    }
    if (remain != 0) {
        if (fread(buffer, remain, 1, *op) != 1) {
            fexit(&(*op), __func__, __LINE__);
        }
        if (fwrite(buffer, remain, 1, *fp) != 1) {
            fexit(&(*fp), __func__, __LINE__);
        }
    }
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
