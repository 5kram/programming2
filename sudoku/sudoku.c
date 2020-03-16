// Sudoku
#include <stdio.h>
#include <stdlib.h>
#define MAX 9
#define MIN 1

int main (int argc, char *argv[]) {
    int i, j, table[MAX][MAX], usedRow[MAX], k;

    for (i = 0; i < MAX; i++) {
        usedRow[i] = 0;
    }

    for (i = MIN - 1; i < MAX; i++) {
        for (j = MIN - 1; j < MAX; j++) {
            if (j == MIN - 1) {
                table[i][j] = (rand() % (MAX - MIN + 1)) + MIN;
                usedRow[table[i][j]] = 1;
                
            }
            else {
                do {
                    usedRow[table[i][j - 1]] = 1;
                    table[i][j] = (rand() % (MAX - MIN + 1)) + MIN;
                }
                while (usedRow[table[i][j]]);
            }
        }
        for (j = MIN; j <= MAX; j++) {
            usedRow[j] = 0;
        }
    }
    printf("\n");
    for (i = MIN - 1; i < MAX; i++) {
        for (j = MIN - 1; j < MAX; j++) {
            printf("%d  ", table[i][j]);
            if (!((j + 1) % 3)) {
                printf("|  ");
            }
        }
        if (!((i + 1) % 3)) {
            printf("\n");
            for (k = MIN - 1; k < 4 * MAX - 2; k++) {
                if ((k == 9 || k == 21 || k == 33)) {
                    printf("|");
                }
                else {
                    printf("-");
                }
            }
            //printf("\n");
        }
        printf("\n");
    }
    printf("\n");

    return 0;
}