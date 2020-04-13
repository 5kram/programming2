// Macro for conversion to uppercase
#include <stdio.h>
#define UPPERCASE(a) (a + 'A' - 'a')
#define N 25

int main (int argc, char *argv[]) {
    char str[N*N], *p, buffer[N];

    sprintf(buffer, "%%%ds", N-1);
	printf("Please Enter\n");
	scanf(buffer, str);
    p = str;
    
    do {
        *p = UPPERCASE(*p);
        p++;
    }
    while (*p != '\0');
    printf("Please Exit\n%s\n", str);

    return 0;
}