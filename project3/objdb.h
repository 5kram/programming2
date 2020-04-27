

#ifndef _OBJDB_H
#define _OBJDB_H
#include <stdio.h>

typedef struct {
    int num_results;
    char *names_buffer;
    int names_len;
} FindResult;

int main(int argc, char *argv[]);
void fexit(FILE *fp,const char func[],const int line);
int open(FILE **fp, char dbname[]);
int metadata(FILE *fp);
int fend(FILE *fp);
FindResult *find(FILE *fp, char name[]);
void deleteResult(FindResult *result);
int find_name(FILE *fp, char objname[], int option);
int move_block(FILE *fp, FILE *op, char objname[]);
int import(FILE *fp, char fname[], char objname[]);
int close(FILE **fp);


#endif