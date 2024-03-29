#include <stdio.h>
#include <stdlib.h>

typedef struct {
        int *a;
        int size;
} DbImpl;


typedef DbImpl* Db;

void init(Db *p, int size) {
        printf("p = %p\n", p);
        printf("&p = %p\n", &p);
        printf("*p = %p\n", *p);

        *p = malloc(sizeof(DbImpl));
        (*p)->a = calloc(size, sizeof(int));;
        (*p)->size = size;
        for (int i = 0; i < size; i++) {
                (*p)->a[i] = 0;
        }
        printf("p = %p\n", p);
        printf("&p = %p\n", &p);
        printf("*p = %p\n", *p);
}

void fill(Db self) {
        for (int i = 0; i < self->size; i++) {
                self->a[i] = i;
        }
}

void print_a(Db p) {
        for (int i = 0; i < p->size; i++) {
                printf("i = %d\n", p->a[i]);
        }
}

void free_db(Db *p) {
        if (*p == NULL) {
                return;
        }
        free((*p)->a);
        free(*p);
        *p = NULL;
}

int main (int argc, char *argv[]) {
        Db ptr = NULL;
        printf("ptr = %p\n", ptr);
        printf("&ptr = %p\n", &ptr);
        init(&ptr, 10);
        printf("ptr = %p\n", ptr);
        printf("&ptr = %p\n", &ptr);
        print_a(ptr);
        fill(ptr);
        ptr.fill();
        print_a(ptr);
        free_db(&ptr);
        return 0;
}
     