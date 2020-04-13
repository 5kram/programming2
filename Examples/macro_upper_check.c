// Macro uppercase with check
#include <stdio.h>
#define UPPERCASE(a) ((a == ' ' && a != '\n') ? a : a + 'A' - 'a')

int main (int argc, char *argv[]) {
    char c;

	printf("Please Enter\n");
    do {
        c = getchar ();
        putchar (UPPERCASE(c));
    }
    while (c != '\n');
    printf("\n");

    return 0;
}