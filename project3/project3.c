/* Database With Digital Objects */
/* Through Binary Files */
#include <stdio.h>
#include "objdb.h"
#include <stdlib.h>
#define NAME_LEN 255
#define DB_ERROR -1
#define YES 1
#define NO 0
#define DEBUG

int main (int argc, char *argv[]) {
    char option, dbname[NAME_LEN], fname[NAME_LEN], objname[NAME_LEN], name[NAME_LEN];
    int check;
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
                check = import(fp, fname, objname);
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
                result = find(fp, name);
                printf("\n##\n");
                /*
                PRINT
                */
                break;
            }
            case 'e': {
                break;
            }
            case 'd': {
                break;
            }
            case 'c': {
                check =  close (fp);
                if (check == -1) {
                    printf("\nNo open db file.\n");
                }
                #ifdef DEBUG
                else {
                    printf("\nCLEAR OK\n");
                }
                #endif
                break;
            }
            case 'q': {
                /*free(fp_array); */
                close(fp);
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