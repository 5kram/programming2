#include "objdb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DB_ERROR -1;
#define MN_SIZE 4
#define BLOCK 512
#define NAME_LEN 256
/*#define DEBUG */

/*
 * Return DB_ERROR(-1) -> No open DB
 * Return 1 -> OK
 */
int close(FILE *fp) {
    if (fp == NULL) {
        return DB_ERROR;
    }
    fclose(fp);
    return 1;
}

void fexit(FILE *fp, const char func[], const int line) {
    fprintf(stderr, "\nError in function: %s\nLine: %d\n", func, line);
    close(fp);
    EXIT_FAILURE;
}

/*
 * Validation of DB
 * Checks the Existance of Magic Number
 * Returns 0 -> Not Valid
 * Returns 1 -> Valid
 */
int db_valid(FILE *fp) {
    int i, val[] = {0xda, 0x7a, 0xba, 0x53}, buf[MN_SIZE] = {0};
    
    fseek(fp, 0, SEEK_SET);
    for (i = 0; i < MN_SIZE; i++) {
        if (fread(&buf[i], 1, 1, fp) != 1) {
            fexit(fp, __func__, __LINE__);
        }
    }
    for (i = 0; i < MN_SIZE; i++) {
        if (val[i] != buf[i]) {
            return 0;
        }
    }
    return 1;
}

/*
 * Metadata Insertion
 * Return 0 -> ERROR
 * Return 1 -> OK
 */
int metadata (FILE *fp) {
    int i, val[] = {0xda, 0x7a, 0xba, 0x53};

    fseek(fp, 0, SEEK_SET);
    for (i = 0; i < MN_SIZE; i++) {
        if (fwrite(&val[i], 1, 1, fp) != 1) {
            fexit(fp, __func__, __LINE__);
        }
    }
    fflush(fp);
    fseek(fp, 0, SEEK_SET);
    return 1;
}

/*
 * Return 0 -> End of file
 * Return 1 -> Not end of file
 */
int fend (FILE *fp) {
    #ifdef DEBUG
        fprintf(stderr, "\nEnd: %ld? Function: %s, Line: %d\n", ftell(fp), __func__, __LINE__);
    #endif
    if (getc(fp) == EOF) {
        return 0;
    }
    fseek(fp, -1, SEEK_CUR);
    #ifdef DEBUG
        fprintf(stderr, "\nNo End: %ld Function: %s, Line: %d\n", ftell(fp), __func__, __LINE__);
    #endif
    return 1;
}

/*
 * TODO documentation *
 */
FindResult *find(FILE *fp, char name[]) {
    unsigned int objnamelen = 0, objsize = 0;
    char objname[NAME_LEN] = {0};
    unsigned int num_results = 0;
    unsigned int *names = NULL;
    unsigned int names_len = 0;
    char *names_buffer = NULL;
    names = (unsigned int*)names_buffer;
    /* unsigned int last_empty_name = 0; */
    unsigned int names_buffer_len = 0;
    
    FindResult *result = (FindResult*)malloc(sizeof(FindResult));
    fseek(fp, MN_SIZE, SEEK_SET);
    while (fend(fp)) {
        objname[0] = '\0';
        /* TODO discuss integer endian format */
        if (fread(&objnamelen, sizeof(unsigned int), 1, fp) != 1) {
            fexit(fp, __func__, __LINE__);
        }

        /* Check if object size is within limits */
        if (objnamelen >= NAME_LEN) {
            fexit(fp, __func__, __LINE__);
        }

        if (fread(objname, sizeof(char), objnamelen, fp) != objnamelen) {
            fexit(fp, __func__, __LINE__);
        }
        objname[objnamelen] = '\0';

        /* If name is contained in this object name */
        if (strstr (objname, name) != NULL) {
            /* fp_array[i] = (int *)malloc(sizeof(int)); */
            fprintf(stderr, "%d, %s\n", objnamelen, objname);


            /* Check if we have space in the names array */
            if ((names_len) == num_results) {
                /* Hack to handle the first allocation */
                

                /* Double names array */
                names = realloc(names, names_len + 10);
            }

            /* TODO check if we have enough space in names_buffer to store `objnamelen` bytes, if not reallocate
             * like names, double on each realloc
             * names_buffer_len increased by objnamelen + 1 each time a name is stored
             * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
             * */
            if (1) {
                if (names_buffer_len == 0) {
                    names_buffer_len = 10;
                }
                names_buffer = realloc(names_buffer, names_buffer_len * 2);
            } 
            /* TODO copy objname to names_buffer, create offset in names, increment num_results*/
            /*
             * names_buffer = [13b \0 __13b__ \0 cool_13b \0 . . . . . . ]
             * names = [      0^,    4^,       12^] those are integer offsets
             *
             * for example if you want the __13b__ name (index '1') you do:
             *      char *name = names_buffer + names[1]; // it's like doing `names_buffer + 4`
             * */
            /* names_len ok
             * names_buffer ok
             * names_buffer_len nok
             * names nok
             * */ 
            strcat(names_buffer, objname);
            names_len = names_len + objnamelen + 1;
            names[num_results] = names_buffer[names_len];
            
            
            fprintf(stderr, "names_buffer: %s &names[%d]: %s, names_len: %d\n", names_buffer, num_results, (char*)names[num_results], names_len);
            num_results++;
            names_len++;
        }

        /* Skip the actual object */
        if (fread(&objsize, sizeof(unsigned int), 1, fp) != 1 ) {
            fexit(fp, __func__, __LINE__);
        }
        fseek(fp, objsize, SEEK_CUR);
    }
    /*
    fprintf(stderr, "names_buffer: %s names[1]: %d\n", names_buffer, names[1]);
    */
    /* TODO create */
    result->names_buffer = malloc(sizeof(names_buffer));
    result->num_results = num_results;
    result->names = names;
    result->names_buffer = names_buffer; 
    /*if(num_results != 0) {
        free(names);
        free(names_buffer);
    }*/
    return result;
}

void deleteResult(FindResult *result) {
    /* TODO free*/
    free(result);
}

/*
 * Return 1 -> name exists in db
 * Return 0 -> name doesn't exist in db
 * fp in correct position
 * Option 0 -> Called by find func
 * Option 1 -> Calles by other func
 */
int find_name(FILE *fp, char name[], int option) {
    unsigned int objnamelen = 0, objsize = 0;
    char objname[NAME_LEN] = {0};

    fseek(fp, MN_SIZE, SEEK_SET);
    while (fend(fp)) {
        objname[0] = '\0';
        if (fread(&objnamelen, sizeof(int), 1, fp) != 1) {
            fexit(fp, __func__, __LINE__);
        }

        #ifdef DEBUG
            fprintf(stderr, "Expecting object name of %d bytes, Function: %s, Line: %d\n", objnamelen, __func__, __LINE__);
        #endif

        /* Check if object size is within limits */
        if (objnamelen >= NAME_LEN) {
            fexit(fp, __func__, __LINE__);
        }

        if (fread(objname, sizeof(char), objnamelen, fp) != objnamelen) {
            fprintf(stderr, "Unexpected object %s size %d", objname, objnamelen);
            fexit(fp, __func__, __LINE__);
        }
        objname[objnamelen] = '\0';
        #ifdef DEBUG
            fprintf(stderr, "Read object %s sized %d bytes, Function: %s, Line: %d\n", objname, objnamelen, __func__, __LINE__);
        #endif
        if (option && !strcmp(name, objname)) {
            return 1;
        }
        
        if (fread(&objsize, sizeof(int), 1, fp) != 1 ) {
            fexit(fp, __func__, __LINE__);
        }
        fseek(fp, objsize, SEEK_CUR);
    }

    return 0;
}
/*
 * fp in correct position
 */
int move_block (FILE *fp, FILE *op, char objname[]) {
    char buffer[BLOCK] = {0};
    int namelen = 0, objsize = 0, repeats = 0, remain = 0, i;
    fseek(fp, 0, SEEK_END);
    #ifdef DEBUG
        fprintf(stderr, "\nImport in end of: %ld, Function: %s, Line: %d\n", ftell(fp), __func__, __LINE__);
    #endif
    /* Inserts objs info into data */
    /* Size of name + name + size of obj */
    namelen = strlen(objname);
    if (fwrite(&namelen, sizeof(int), 1, fp) != 1) {
        fexit(fp, __func__, __LINE__);
    }
    fflush(fp);
    if (fwrite(objname, sizeof(char), namelen, fp) != namelen) {
        fexit(fp, __func__, __LINE__);
    }
    fflush(fp);
    fseek(op, 0, SEEK_END);
    objsize = ftell (op);
    fseek(op, 0, SEEK_SET);
    if (fwrite(&objsize, sizeof(int), 1, fp) != 1) {
       fexit(fp, __func__, __LINE__);
    }
    fflush(fp);
    #ifdef DEBUG
        fprintf(stderr, "\n%d, %s, %d, Function: %s, Line: %d\n", namelen, objname, objsize, __func__, __LINE__); 
    #endif
    /* Move bytes */
    /* if objsize < BLOCK, doesnt loop */
    repeats = objsize / BLOCK;
    remain = objsize % BLOCK;
    for (i = 0; i < repeats; i++) {
        if (fread(buffer, BLOCK, 1, op) !=  1) {
            fexit(op, __func__, __LINE__);
        }
        if (fwrite(buffer, BLOCK, 1, fp) != 1) {
            fexit(fp, __func__, __LINE__);
        }
        fflush(fp);
    }
    if (remain != 0) {
        if (fread(buffer, remain, 1, op) != 1) {
            fexit(op, __func__, __LINE__);
        }
        if (fwrite(buffer, remain, 1, fp) != 1) {
            fexit(fp, __func__, __LINE__);
        }
        fflush(fp);
    }
    
    return 1;
}

/*
 * Return DB_ERROR(-1) -> Error Opening db
 * Return 0 -> Invalid db
 * Return 1 -> OK
 */
int open(FILE **fp, char dbname[]) {
    int check;
    
    /* Close any pre-existing open file */
    if (*fp != NULL && ftell(*fp) >= 0) {
       close(*fp);
    }
    /* Try to open file */
    *fp = fopen(dbname, "rb+");
    /* If file does not exist, try to create it */
    if (*fp == NULL) {
        *fp = fopen(dbname, "wb+");
        if (ferror(*fp)) {
            #ifdef DEBUG
            fprintf(stderr, "\nNew DB. Error in opening:\nFunction: %s\nLine: %d\n", __func__, __LINE__);
            #endif
            return DB_ERROR;
        }
        /* Create metadata */
        check = metadata(*fp);
        if (!check) {
            #ifdef DEBUG
                fprintf(stderr, "\nNew DB. Error in Metadata:\nFunction: %s\nLine: %d\n", __func__, __LINE__);
            #endif
            close(*fp);
            return DB_ERROR;
        }
    }

    check = db_valid(*fp);
    /* Its db */
    if (check) {
        return 1;
    } else { /* Its not db */
        close(*fp);
        return 0;
    }
}

/*
 * Return -1 -> No db
 * Return 0 -> No fname
 * Return -2 -> Object name already in db
 */
int import(FILE *fp, char fname[], char objname[]) {
    FILE *op;
    int check;
    fprintf(stderr, "\nImport Function: %s, Line: %d\n", fname, __LINE__);
    /* No open db */
    if (fp == NULL) {
        return DB_ERROR;
    }
    fprintf(stderr, "\nImport Function: %s, Line: %d\n", fname, __LINE__);
    op = fopen(fname, "rb");
    /* No existing file */
    if (op == NULL ) {
        return 0;
    }
    fprintf(stderr, "\nImport Function: %s, Line: %d\n", fname, __LINE__);
    check = find_name(fp, objname, 1);
    if (check) {
        fclose(op);
        return -2;
    }
    fprintf(stderr, "\nImport Function: %s, Line: %d\n", fname, __LINE__);
    check = move_block(fp, op, objname);
    fclose(op);
    return 1;
}