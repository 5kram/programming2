/* Database With Digital Objects */
/* Through Binary Files */
#include <stdio.h>
#include "objdb.h"
#include <stdlib.h>
#define NAME_LEN 255
#define DB_ERROR -1
#define YES 1
#define NO 0
#define IMPORT 0
#define FIND 1
/*#define DEBUG*/

int main (int argc, char *argv[]) {
    char option, dbname[NAME_LEN], fname[NAME_LEN], objname[NAME_LEN], name[NAME_LEN];
    int check, i = 0, num = 0;
    FILE *fp = NULL;
    FindResult *result;
    do {
        scanf(" %c", &option);
        switch(option) {
            case 'o': {
                scanf("%s", dbname);
                check = open(&fp, dbname);
                if (check == -1) {
                    printf("\nError opening %s.\n", dbname);
                }
                #ifdef DEBUG
                    if (check == 1) {
                        printf("\nBASE OK\n");
                    }
                #endif
                if (check == 0) {
                    printf("\nInvalid db file %s.\n", dbname);
                }
                if (check == -2) {
                    printf("\nNo open db file.\n");
                }
                break;
            }
            case 'i': {
                scanf (" %s %s", fname, objname);
                check = import(&fp, fname, objname);
                if (check == 0) {
                    printf("\nFile %s not found.\n", fname);
                }
                if (check == -1) {
                    printf("\nNo open db file.\n");
                }
                if (check == -2) {
                    printf("\nObject %s already in db.\n", objname);
                }
                break;
            }
            case 'f': {
                scanf(" %s", name);
                result = find(&fp, name, FIND);
                if (result->num_results == -1) {
                    printf("\nNo open db file.\n");
                    deleteResult(result, IMPORT);
                    break;
                }
                i = 0;
                printf("\n##\n");
                if (result->num_results > 0) {
                    while(num < result->num_results - 1 || i < result->names_len - 1) {
                        if(result->names_buffer[i] == ' ') {
                            printf("\n");
                            i++;
                            num++;
                        }
                        putchar(result->names_buffer[i]);
                        i++;
                    }
                    printf("\n");
                }
                deleteResult(result, FIND);
                break;
            }
            case 'e': {
                scanf(" %s %s", objname, fname);
                check = export(&fp, objname, fname);
                if (check == -1) {
                    printf("\nNo open db file.\n");
                }
                if (check == 0) {
                    printf("\nObject %s not in db.\n", objname);
                }
                if (check == -2) {
                    printf("\nCannot open file %s.\n", fname);
                }
                #ifdef DEBUG
                if (check == 1) {
                    printf("\nEXPORT OK.\n");
                }
                #endif
                break;
            }
            case 'd': {
                scanf(" %s", name);
                check = delete(&fp, name, dbname);
                if (check == -1) {
                    printf("\nNo open db file.\n");
                }
                if (check == 0) {
                    printf("\nObject %s not in db.\n", name);
                }
                break;
            }
            case 'c': {
                check =  closef(&fp);
                if (check == -1) {
                    printf("\nNo open db file.\n");
                }
                #ifdef DEBUG
                else {
                    printf("\nCLOSE OK\n");
                }
                #endif
                break;
            }
            case 'q': {
                closef(&fp);
                return 0;
            }
            default : {
                continue;
            }
        }
    }
    while(1);

    return 0;
}
