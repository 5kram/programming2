#ifndef _OBJDB_H
    #define _OBJDB_H
    #include <stdio.h>

    typedef struct {
        int num_results;
        int names_len;
        char *names_buffer;
    } FindResult;

    int main(int argc, char *argv[]);
    void read(void *ptr, int size_t_, int nmemb, FILE *stream);
    void write(void *ptr, int size_t_, int nmemb, FILE *stream);
    void fexit(FILE **fp,const char func[],const int line);
    int open(FILE **fp, char dbname[]);
    int metadata(FILE *fp);
    int fend(FILE *fp);
    FindResult *find(FILE **fp, char name[], int called_by);
    void deleteResult(FindResult *result, int called_by);
    int move_in_db(FILE *fp, FILE *op, char objname[]);
    int move_from_db(FILE *fp, FILE *op, char fname[]);
    int import(FILE **fp, char fname[], char objname[]);
    int export (FILE **fp, char objname[], char fname[]);
    int close(FILE **fp);
#endif