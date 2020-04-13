// Macro Stringize
#include <stdio.h>
#define message_for_what(a, b, c, d) \
printf("New message for you guys: " #a ", " #b ", " #c ".\n" #d ".\n")

int main (int argc, char *argv[]) {
    
    message_for_what(John, Mary, You, Letsstringize);

    return 0;
}