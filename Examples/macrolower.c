// Macro for conversion to lowercase
#define UPPERCASE(a) (a + 'A' - 'a')
#define N 25

int main (int argc, char *argv[]) {
    char str[N];

    printf("Please Enter\n");
    scanf(" %s", str);
    do {
        *str = LOWERCASE(*str);
    }
    while (*str != '\0');
    printf("Please Exit\n%s", str);


    return 0;
}