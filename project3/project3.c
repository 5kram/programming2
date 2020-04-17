// Database With Digital Objects
// Through Binary Files
#include <stdio.h>
#include "objdb.h"
#define BLOCK 512
#define NAME_LEN 255
#define DB_ERROR -1
#define YES 1
#define NO 0


int main (int argc, char *argv[]) {
    char option, dbname[NAME_LEN];
    int check;

    do {
        scanf(" %c", &option);
        switch(option) {
            case 'o': {
                scanf("%s", dbname);
                check = open (dbname);
                if (check == -1) {
                    printf("\nError opening %s.\n", dbname);
                }
                break;
            }
            case 'i': {
                break;
            }
            case 'f': {
                break;
            }
            case 'e': {
                break;
            }
            case 'd': {
                break;
            }
            case 'c': {
                break;
            }
            case 'q': {
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