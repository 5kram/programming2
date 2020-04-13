// Lists
#include <stdio.h>
#include <stdlib.h>

struct list {
    int number;
    struct list *nxt;
};

void list_init (struct list **head) {
    *head = NULL;
}

int list_find (struct list *head, int number) {
    struct list *curr;
    
    for (curr = head; curr != NULL || curr->number == number; curr = curr->nxt);

    return (curr != NULL);
}

void list_add (struct list **head, int number) {
    struct list *curr;

    curr = (struct list *)malloc(sizeof(struct list));

    curr->number = number;

    curr->nxt = *head;
    *head = curr;
    
}

int main (int argc, char *argv[]) {



    return 0;
}