#include "objdb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define DB_ERROR -1;
#define MN_SIZE 4
#define BLOCK 512
#define NAME_LEN 256
#define IMPORT 0
#define FIND 1
#define CHAR -1
#define INT 0
/*#define DEBUG*/

/* Close the file.
 * Return DB_ERROR(-1) -> No open DB.
 * Return 1 -> OK. 
 */
int closef(FILE **fp) {
    if (*fp == NULL) {
        return DB_ERROR;
    }
    fclose(*fp);
    *fp = NULL;
   
    return 1;
}

/* Called when an error has occured. */
void fexit(FILE **fp, const char func[], const int line) {
    #ifdef DEBUG
        fprintf(stderr, "\nError in function: %s\nLine: %d\n", func, line);
    #endif
    closef(&(*fp));
    exit(EXIT_FAILURE);
}

/* Read function.
 * Check if fread returns the wanted bytes.
 * If less bytes returned, call it again, until they are all read.
 */
void readf(void *ptr, int size_t_, int nmemb, FILE *stream) {
    int bytes_written = 0, bytes_wanted;

    bytes_wanted = nmemb;
    
    if (size_t_ == CHAR) {
        size_t_ = sizeof(char);
    }
    else if (size_t_ == INT) {
        size_t_ = sizeof(int);
    }
    
    while (bytes_wanted == nmemb && !feof(stream) && !ferror(stream)) {
        bytes_written = fread(ptr, size_t_, nmemb, stream);
        bytes_wanted += bytes_written;
    }/*
    if (feof(stream) || ferror(stream)) {
        fexit(&stream, __func__, __LINE__);
    }*/
}

/* Write function.
 * Check if fwrite returns the wanted bytes.
 * If less bytes returned, call it again, until they are all written.
 */
void writef(void *ptr, int size_t_, int nmemb, FILE *stream) {
    int bytes_written = 0, bytes_wanted;

    bytes_wanted = nmemb;
   
    if (size_t_ == CHAR) {
        size_t_ = sizeof(char);
    }
    else if (size_t_ == INT) {
        size_t_ = sizeof(int);
    }
    
    while (bytes_wanted == nmemb && !feof(stream) && !ferror(stream)) {
        bytes_written = fwrite(ptr, size_t_, nmemb, stream);
        bytes_wanted += bytes_written;
    }
    fflush(stream);
    if (feof(stream) || ferror(stream)) {
        fexit(&stream, __func__, __LINE__);
    }
    
}

/* Validation of database.
 * Check the existance of Magic Number.
 * Returns 0 -> Not Valid.
 * Returns 1 -> Valid.
 */
int db_valid(FILE *fp) {
    int i = 0, val[] = {0xda, 0x7a, 0xba, 0x53}, buf[MN_SIZE] = {0}, fp_size = 0;

    /* Check if file has less than MN_SIZE bytes. */
    fseek(fp, 0, SEEK_END);
    fp_size = ftell(fp);
    if (fp_size < MN_SIZE) {
        return 0;
    }
    /* Read the first 4 bytes and compare them with MN. 
     * If 0 bytes were read instead of 1, continue reading. */
    fseek(fp, 0, SEEK_SET);
    while(i < MN_SIZE) {
        readf(&buf[i], 1, 1, fp);
        if (val[i] != buf[i]) {
            return 0;
        }
        i++;
    }
    
    return 1;
}

/* Metadata Insertion in the begining of the database (Magic Number).
 * Return 0 -> ERROR.
 * Return 1 -> OK.
 */
int metadata(FILE *fp) {
    int i, val[] = {0xda, 0x7a, 0xba, 0x53};

    fseek(fp, 0, SEEK_SET);
    for (i = 0; i < MN_SIZE; i++) {
        writef(&val[i], 1, 1, fp);
    }
    fseek(fp, 0, SEEK_SET);
    return 1;
}

/* Check if the file pointer is in the end of the file. 
 * Return 0 -> End of file.
 * Return 1 -> Not end of file.
 */
int fend(FILE *fp) {
    if (getc(fp) == EOF) {
        return 0;
    }
    fseek(fp, -1, SEEK_CUR);
    return 1;
}

 /* Move bytes in blocks.
 * Compute how many repeats needed, to move whole blocks(512 bytes).
 * Move blocks "repeats" times.
 * Compute how many bytes remain and move them.
 */
void move_512(FILE *dest, FILE *source, int objsize, long int start_dest, long int start_source) {
    int repeats = 0, remain = 0, i;
    char buffer[BLOCK] = {0};

    fseek(source, 0, start_source);
    repeats = objsize / BLOCK;
    remain = objsize % BLOCK;
    for (i = 0; i < repeats; i++) {
        fseek(source, start_source, SEEK_SET);
        readf(buffer, BLOCK, 1, source);
        fseek(dest, start_dest, SEEK_SET);
        writef(buffer, BLOCK, 1, dest);
        start_source += BLOCK;
        start_dest += BLOCK;
    }
    if (remain != 0) {
        fseek(source, start_source, SEEK_SET);
        readf(buffer, remain, 1, source);
        fseek(dest, start_dest, SEEK_SET);
        writef(buffer, remain, 1, dest);
    }
}

/* If called by find: find every object name which contain (or is equal with) the name, the user gave.
 * Return a struct with the names in a format: [(name1)(space)(name2)(space)\0] and the number of names found.
 * If called by import: find the exact name, the user gave and return the struct with num_results = 1.
 * In case there is not any open database return the struct with num_results = -1.
 */
FindResult *find(FILE **fp, char name[], int called_by) {
    int objnamelen = 0, objsize = 0, names_len = 0, names_buffer_len = 100, num_results = 0, object_position;
    char objname[NAME_LEN] = {0}, *names_buffer = NULL;
    
    
    FindResult *result = (FindResult*)malloc(sizeof(FindResult));
    /* Check if there is open db. */
    if (*fp == NULL) {
        result->num_results = -1;
        return result;
    }
    fseek(*fp, MN_SIZE, SEEK_SET);
    while (fend(*fp)) {
        objname[0] = '\0';
        readf(&objnamelen, INT, 1, *fp);
        /* Check if object size is within limits. */
        if (objnamelen >= NAME_LEN) {
            fexit(&(*fp), __func__, __LINE__);
        }

        readf(objname, CHAR, objnamelen, *fp);
        objname[objnamelen] = '\0';
        #ifdef DEBUG
                fprintf(stderr, "objnamelen: %d, objname: %s\n", objnamelen, objname);
        #endif
        /* If called by find: search all objects names if the name, the user gave, is part of them. */ 
        if (called_by == FIND) {
            /* If name is contained in this object name. */
            if (strstr (objname, name) != NULL || strcmp(name, "*") == 0) {
                #ifdef DEBUG
                fprintf(stderr, "objnamelen: %d, objname: %s\n", objnamelen, objname);
                #endif
                
                /* Reallocate space in names_buffer to store `objnamelen` + 1 bytes.
                 * names_buffer_len increased by objnamelen + 1 each time a name is stored. */
                names_buffer_len = names_buffer_len + objnamelen + 1;
                names_buffer = realloc(names_buffer, names_buffer_len);
                
                /* Copy objname to names_buffer, create offset in names, increment num_results. */
                int i = 0;
                do {
                    names_buffer[names_len + i] = objname[i];
                    i++;
                }
                while (objname[i] != '\0');

                names_len = names_len + objnamelen + 1;
                names_buffer[names_len - 1] = ' ';
                num_results++;
            }
        }
        /* If called by import, return num_results = 1. */
        else {
            if (!strcmp(name, objname)) {
                num_results++;
                result->num_results = num_results;
                fseek(*fp, -(objnamelen * sizeof(char) + sizeof(int)), SEEK_CUR);
                object_position = ftell(*fp);
                result->object_position = object_position;
                return result;
            }
        }

        /* Skip the actual object. */
        readf(&objsize, INT, 1, *fp);
        fseek(*fp, objsize, SEEK_CUR);
    }
    /* If called by import and havent found that exact name in the database.
     * Return num_results = 0. */
    if (called_by == IMPORT) {
        result->num_results = num_results;
        return result;
    }
    /* Create names_buffer offset. */
    if (num_results == 0) {
        names_buffer = realloc(names_buffer, 10);
        names_buffer[0] = ' ';
        names_len++;
    }
    else {
        names_buffer[names_len] = '\0';
    }
    /* Fill the struct with data, that been computed above. */
    result->names_buffer = (char*)malloc(names_len + 1);
    result->num_results = num_results;
    memcpy(result->names_buffer, names_buffer, names_len);
    result->names_len = names_len;
    result->names_buffer[names_len] = '\0';
    free(names_buffer);
    return result;
}

/* Free results from find. */
void deleteResult(FindResult *result, int called_by) {
    if (called_by == FIND) {
        free(result->names_buffer);
    }
    free(result);
    
}

/* Move in blocks of 512 bytes the object(Object Pointer) to the end of the database(File Pointer).
 * When the object is imported the format in the database is [..(size of name)(name)(size of object)(content of object)EOF].
 */
int move_in_db(FILE *fp, FILE *op, char objname[]) {
    int namelen = 0, objsize = 0;

    fseek(fp, 0, SEEK_END);
     #ifdef DEBUG
        fprintf(stderr, "\nFile position: %ld, Function: %s, Line: %d\n", ftell(fp), __func__, __LINE__); 
    #endif
    /* Insert objects info in to the database in the following order.
     * Size of name, name, size of object and lastly the actual object. */
    namelen = strlen(objname);
    writef(&namelen, INT, 1, fp);
    writef(objname, CHAR, namelen, fp);
    fseek(op, 0, SEEK_END);
    objsize = ftell (op);
    fseek(op, 0, SEEK_SET);
    writef(&objsize, INT, 1, fp);
    #ifdef DEBUG
        fprintf(stderr, "\nnamelen: %d, objname: %s, objsize: %d, Function: %s, Line: %d\n", namelen, objname, objsize, __func__, __LINE__); 
    #endif

    move_512(fp, op, objsize, ftell(fp), ftell(op));
    
    return 1;
}

/* Move an object, which is inside the database, to a new file. 
 * Move the object in blocks of 512 bytes.
 */
int move_from_db(FILE *fp, FILE *op, char object_name[], int object_position) {
    int objnamelen = 0, object_size = 0;

    fseek(fp, object_position, SEEK_SET);
    #ifdef DEBUG
        fprintf(stderr, "\nobject position: %d", object_position);
    #endif
    readf(&objnamelen, INT, 1, fp);
    fseek(fp, objnamelen * sizeof(char), SEEK_CUR);
    readf(&object_size, INT, 1, fp);
    #ifdef DEBUG
        fprintf(stderr, "\nobject size: %d\n", object_size);
    #endif
   
    move_512(op, fp, object_size, ftell(op), ftell(fp));

    return 1;
}

int move_within_db(FILE *fp, int object_position) {
    int database_size = 0, objnamelen = 0, object_size = 0, moving_size = 0, size_obj_occupy = 0;

    fseek(fp, 0, SEEK_END);
    database_size = ftell(fp);
    fseek(fp, object_position, SEEK_SET);
    readf(&objnamelen, INT, 1, fp);
    fseek(fp, objnamelen * sizeof(char), SEEK_CUR);
    readf(&object_size, INT, 1, fp);
    size_obj_occupy =  2 * sizeof(int) + objnamelen * sizeof(char) + object_size;
    moving_size = database_size - (object_position + size_obj_occupy);
    #ifdef DEBUG
        fprintf(stderr, "\nobject position: %d, objnamelen: %d\n", object_position, objnamelen);
    #endif
    /* Destination identify source.
     * Move whats between the end of the object and the end of the file. */
    move_512(fp, fp, moving_size, object_position, object_position + size_obj_occupy);
    

    return database_size - size_obj_occupy;
}

/* Create or open, if already exists, a database.
 * When create a new database: first create its metadata (Magic Number at the begining of the file).
 * When open an already existing base: check if its a valid database (contains the MN at the begining).
 * If there is error opening the database, return DB_ERROR. If the file exists but its not a database, return 0.
 * A valid database created or existed and opened with no error, return 1.
 */
int open(FILE **fp, char dbname[]) {
    int function_res;
    
    /* Close any pre-existing open file. */
    if (*fp != NULL && ftell(*fp) >= 0) {
       closef(&(*fp));
    }
    /* Try to open file. */
    *fp = fopen(dbname, "rb+");
    /* If file does not exist, try to create it. */
    if (*fp == NULL) {
        *fp = fopen(dbname, "wb+");
        if (ferror(*fp)) {
            #ifdef DEBUG
            fprintf(stderr, "\nNew DB. Error in opening:\nFunction: %s\nLine: %d\n", __func__, __LINE__);
            #endif
            return DB_ERROR;
        }
        /* Create metadata. */
        function_res = metadata(*fp);
        if (!function_res) {
            #ifdef DEBUG
                fprintf(stderr, "\nNew DB. Error in Metadata:\nFunction: %s\nLine: %d\n", __func__, __LINE__);
            #endif
            closef(&(*fp));
            return DB_ERROR;
        }
    }
    /* Check if the database is valid. */
    function_res = db_valid(*fp);
    if (function_res) {
        return 1;
    }
    else {
        closef(&(*fp));
        return 0;
    }
}

/* Import an object in the database.
 * Get the name of the object which will be imported, check if a file with that name exists. 
 * Then, get a name, which, the user, wants the object to get named after, when the object gets in the base.
 * Check if there is an object with that name in the base, if not, call function move_block to import the object with the given name in the base.
 * Return -1 -> No db.
 * Return 0 -> No fname.
 * Return -2 -> Object name already in db.
 */
int import(FILE **fp, char fname[], char objname[]) {
    FILE *op;
    int function_res;
    FindResult *result;

    /* No open db. */
    if (*fp == NULL) {
        return DB_ERROR;
    }

    op = fopen(fname, "rb");
    /* No existing file. */
    if (op == NULL ) {
        return 0;
    }

    result = find(&(*fp), objname, IMPORT);
    
    if (result->num_results > 0) {
        deleteResult(result, IMPORT);
        fclose(op);
        return -2;
    }
    deleteResult(result, IMPORT);

    function_res = move_in_db(*fp, op, objname);
    if (!function_res) {
        fexit(&op, __func__, __LINE__);
    }
    fclose(op);
    return 1;
}

/* Documentation.
 * Return DB_ERROR -> no open database.
 * Return 0 -> object not found.
 * Return -2 -> file already exists/cant open file.
 */
int export (FILE **fp, char objname[], char fname[]) {
    FILE *op;
    FindResult *result;
    int object_position = 0;

    /* No open db. */
    if (*fp == NULL) {
        return DB_ERROR;
    }

    result = find(&(*fp), objname, IMPORT);
    if (result->num_results == 0) {
        deleteResult(result, IMPORT);
        return 0;
    }
    op = fopen(fname, "wbx");
    /* File already exists or couldnt be opened. */
    if (op == NULL ) {
        return -2;
    }

    object_position = result->object_position;
    move_from_db(*fp, op, fname, object_position);
    
    deleteResult(result, IMPORT);
    fclose(op);
    return 1;
}

/* Delete an object from the database.
 * Find the object in tha database, if exists, and overwrite it with the following objects, if exist.
 * Fix the size of the database accordingly.
 */
int delete(FILE **fp, char name[], char dbname[]) {
    FindResult *result;
    int object_position = 0, untouched = 0;

    /* No open db. */
    if (*fp == NULL) {
        return DB_ERROR;
    }
    result = find(&(*fp), name, IMPORT);
    if (result->num_results == 0) {
        deleteResult(result, IMPORT);
        return 0;
    }
    object_position = result->object_position;
    untouched = move_within_db(*fp, object_position);

    closef(&(*fp));
    truncate(dbname, untouched);
    *fp = fopen(dbname, "rb+");
    
    deleteResult(result, IMPORT);
    return 1;
}