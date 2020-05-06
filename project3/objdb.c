#include "objdb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define DB_ERROR -1;
#define MN_SIZE 4
#define BLOCK 512
#define NAME_LEN 256
#define OTHER 0
#define FIND 1
#define CHAR -1
#define INT 0
/*#define DEBUG*/

/* Close the file.
 * Check if the file is closed. If not, close it. 
 */
int closef(FILE **fp) {
    if (*fp == NULL) {
        return DB_ERROR;
    }
    fclose(*fp);
    *fp = NULL;
    return 1;
}

/* Exit the program. 
 * Called when an error has occured.
 */
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
    /* Stop when bytes wanted read or an error has occured. */
    while (bytes_wanted == nmemb && !feof(stream) && !ferror(stream)) {
        bytes_written = fread(ptr, size_t_, nmemb, stream);
        bytes_wanted += bytes_written;
    }
    if (feof(stream) || ferror(stream)) {
        fexit(&stream, __func__, __LINE__);
    }
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
    /* Stop when bytes wanted written or an error has occured. */
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

/* Create Metadata.
 * Insert the Magic Number in the begining of the database.
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
 */
int fend(FILE *fp) {
    if (getc(fp) == EOF) {
        return 0;
    }
    fseek(fp, -1, SEEK_CUR);
    return 1;
}

/* Find the size of the database.
 */
int find_db_size(FILE *fp) {
    fseek(fp, 0, SEEK_END);
    return ftell (fp);
}

/* Find the size of an object.
 */
int find_obj_size(FILE *fp, int object_position) {
    int objnamelen = 0, object_size = 0;

    fseek(fp, object_position, SEEK_SET);
    readf(&objnamelen, INT, 1, fp);
    fseek(fp, objnamelen * sizeof(char), SEEK_CUR);
    readf(&object_size, INT, 1, fp);

    return object_size;
}

 /* Move bytes in blocks.
 * Compute how many repeats needed, to move whole blocks(512 bytes).
 * Read(Write) from(to) start_source(start_dest) block-bytes and increase the size of start_source(start_dest) by "block".
 * Move blocks "repeats" times.
 * Compute how many bytes remain and move them, similarly.
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

/* Fill the result-struct with data(number of results and the object position).
 */
void fill_struct_wdetails(FindResult **result, FILE *fp, int num_results, int objnamelen) {
    int object_position = 0;
    num_results++;
    (*result)->num_results = num_results;
    fseek(fp, -(objnamelen * sizeof(char) + sizeof(int)), SEEK_CUR);
    object_position = ftell(fp);
    (*result)->object_position = object_position;
}

/* Fill the result-struct with data(names, number of results and the length of buffer).
 * Create offset in names_buffer.
 */
void fill_struct_wnames(FindResult **result, char *names_buffer, int num_results, int names_len) {
    (*result)->names_buffer = (char*)malloc(names_len + 1);
    (*result)->num_results = num_results;
    memcpy((*result)->names_buffer, names_buffer, names_len);
    (*result)->names_len = names_len;
    (*result)->names_buffer[names_len] = '\0';
}

/* If called by find: find every object name which contain (or is equal with) the name, the user gave.
 * Return a struct with the names in a format: [(name1)(space)(name2)(space)\0] and the number of names found.
 * If called by other(import/export/delete): find the exact name, the user gave and return the struct with num_results = 1.
 * In case there is not any open database return the struct with num_results = -1.
 */
FindResult *find(FILE **fp, char name[], int called_by) {
    int objnamelen = 0, objsize = 0, names_len = 0, names_buffer_len = NAME_LEN, num_results = 0, i;
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
        /* If called by find: search all objects names if the name, the user gave, is part of them. */ 
        if (called_by == FIND) {
            /* If name is contained in this object name. */
            if (strstr (objname, name) != NULL || strcmp(name, "*") == 0) {
                i = 0;
                /* Reallocate space in names_buffer to store `objnamelen` + 1 bytes.
                 * names_buffer_len increased by objnamelen + 1 each time a name is stored. */
                names_buffer_len = names_buffer_len + objnamelen + 1;
                names_buffer = realloc(names_buffer, names_buffer_len);
                /* Copy objname to names_buffer, create offset in names, increment num_results. */
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
                fill_struct_wdetails(&result, *fp, num_results, objnamelen);
                return result;
            }
        }
        /* Skip the actual object. */
        readf(&objsize, INT, 1, *fp);
        fseek(*fp, objsize, SEEK_CUR);
    }
    /* If called by import and havent found that exact name in the database.
     * Return num_results = 0. */
    if (called_by == OTHER) {
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
    fill_struct_wnames(&result, names_buffer, num_results, names_len);

    free(names_buffer);
    return result;
}

/* Free results.
 * names_buffer only get used when called by find(in main).
 */
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
    /* Insert objects info in to the database in the following order.
    * Size of name, name, size of object and lastly the actual object. */
    fseek(fp, 0, SEEK_END);
    namelen = strlen(objname);
    writef(&namelen, INT, 1, fp);
    writef(objname, CHAR, namelen, fp);
    objsize = find_db_size(op);
    fseek(op, 0, SEEK_SET);
    writef(&objsize, INT, 1, fp);
    move_512(fp, op, objsize, ftell(fp), ftell(op));
    
    return 1;
}

/* Move an object, which is inside the database, to a new file. Move the object in blocks of 512 bytes.
 * Move the object in object_position from fp, to op.
 */
int move_from_db(FILE *fp, FILE *op, char object_name[], int object_position) {
    int object_size = 0;

    object_size = find_obj_size(fp, object_position);
    move_512(op, fp, object_size, ftell(op), ftell(fp));

    return 1;
}

/* Re-position one or more objects in the database.
 * Move the object(s) in blocks of 512 bytes.
 * Find the end of the object in object_position + move what comes after in object_position.
 */
int move_within_db(FILE *fp, int object_position, int objnamelen) {
    int database_size = 0, object_size = 0, moving_size = 0, size_obj_occupy = 0;

    database_size = find_db_size(fp);
    object_size = find_obj_size(fp, object_position);
    size_obj_occupy =  2 * sizeof(int) + objnamelen * sizeof(char) + object_size;
    moving_size = database_size - (object_position + size_obj_occupy);
    /* Destination identify source.
     * Move whats between the end of the object and the end of the file. */
    move_512(fp, fp, moving_size, object_position, object_position + size_obj_occupy);

    return database_size - size_obj_occupy;
}

/* Create or open, if already exists, a database.
 * When create a new database: first create its metadata (Magic Number at the begining of the file).
 * When open an already existing base: check if its a valid database (contains the MN at the begining).
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
            return DB_ERROR;
        }
        /* Create metadata. */
        function_res = metadata(*fp);
        if (!function_res) {
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
    result = find(&(*fp), objname, OTHER);
    if (result->num_results > 0) {
        deleteResult(result, OTHER);
        fclose(op);
        return -2;
    }
    function_res = move_in_db(*fp, op, objname);
    if (!function_res) {
        fexit(&op, __func__, __LINE__);
    }

    deleteResult(result, OTHER);
    fclose(op);
    return 1;
}

/* Export an object from the database to another file.
 * Check if an object with that name exists in the database.
 * If so, create the destination file and move the object there. 
 */
int export (FILE **fp, char objname[], char fname[]) {
    FILE *op;
    FindResult *result;
    int object_position = 0;

    /* No open db. */
    if (*fp == NULL) {
        return DB_ERROR;
    }
    result = find(&(*fp), objname, OTHER);
    if (result->num_results == 0) {
        deleteResult(result, OTHER);
        return 0;
    }
    op = fopen(fname, "wbx");
    /* File already exists or couldnt be opened. */
    if (op == NULL ) {
        return -2;
    }
    object_position = result->object_position;
    move_from_db(*fp, op, fname, object_position);
    
    deleteResult(result, OTHER);
    fclose(op);
    return 1;
}

/* Delete an object from the database.
 * Find the object in tha database, if exists, and overwrite it with the following objects, if exist.
 * Fix the size of the database accordingly.
 */
int delete(FILE **fp, char name[], char dbname[]) {
    FindResult *result;
    int object_position = 0, database_size = 0, objnamelen = 0;

    /* No open db. */
    if (*fp == NULL) {
        return DB_ERROR;
    }
    result = find(&(*fp), name, OTHER);
    if (result->num_results == 0) {
        deleteResult(result, OTHER);
        return 0;
    }
    object_position = result->object_position;
    objnamelen = strlen(name);
    database_size = move_within_db(*fp, object_position, objnamelen);

    closef(&(*fp));
    truncate(dbname, database_size);
    *fp = fopen(dbname, "rb+");
    deleteResult(result, OTHER);
    return 1;
}