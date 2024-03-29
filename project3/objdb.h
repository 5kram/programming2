#ifndef _OBJDB_H
    #define _OBJDB_H
    #include <stdio.h>

    typedef struct {
        int num_results;
        int names_len;
        char *names_buffer;
        int object_position;
        int object_size;
    } FindResult;

    int main(int argc, char *argv[]);
    void readf(void *ptr, int size_t_, int nmemb, FILE *stream);
    void writef(void *ptr, int size_t_, int nmemb, FILE *stream);
    void fexit(FILE **fp,const char func[],const int line);
    int open(FILE **fp, char dbname[]);
    int metadata(FILE *fp);
    int fend(FILE *fp);
    int find_db_size(FILE *fp);
    int find_obj_size(FILE *fp, int object_position);
    void move_512(FILE *dest, FILE *source, int objsize, long int start_dest, long int start_source);
    void fill_struct_wnames(FindResult **result, char *names_buffer, int num_results, int names_len);
    void fill_struct_wdetails(FindResult **result, FILE *fp, int num_results, int objnamelen);
    FindResult *find(FILE **fp, char name[], int called_by);
    void deleteResult(FindResult *result, int called_by);
    int move_in_db(FILE *fp, FILE *op, char objname[]);
    int move_from_db(FILE *fp, FILE *op, char fname[], int object_position);
    int move_within_db(FILE *fp, int object_position, int objnamelen);
    int import(FILE **fp, char fname[], char objname[]);
    int export(FILE **fp, char objname[], char fname[]);
    int delete(FILE **fp, char name[], char dbname[]);
    int closef(FILE **fp);
#endif