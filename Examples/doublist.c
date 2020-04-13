// Double List
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define Q "quit"

struct entry {
    char name[64];
    long unsigned int uni_register;
    struct entry *nxt;
    struct entry *prv;
};
typedef struct entry entry;

struct doublist {
    entry *head;
    int size;
    int largest_bucket;
    int list_size[];
};
typedef struct doublist doublist;

unsigned long hash (char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
};
typedef struct hash hash;

entry *find_name (entry *list, char name[], doublist *dl, int index) {
    int i = 0;
    entry *curr;
    
    for (curr = list; strcmp(curr->name, Q); curr = curr->nxt);

}

int doublist_add (doublist *dl, char name[], int size) {
    entry *curr;
    unsigned long index;
    entry *list;

    index = hash(name) % size;
    list = (entry *)dl[index].head;

    curr = (entry *)malloc(sizeof(entry));
    curr->nxt = NULL;
    curr->prv = NULL;
    // Empty List
    if (!dl->list_size[index]) {
        dl[index].head = curr;
        dl->list_size[index]++;
    }
    else {
        curr =
    }
    curr->nxt = head->nxt;
    curr->prv = head;
    head->nxt->prv = curr;
    head->nxt = curr;
    printf("%s\n", curr->name);

    return 0;
}




int main (int argc, char *argv[]) {
    char option, name[64];
    int position, check, i = 0;
    doublist *dl;
    entry *entries;
    
    for (i = 0; i < atoi(argv[1]); i++) {
        dl->list_size[i] = 0;
    }
    dl->largest_bucket = 0;
    dl->size = atoi(argv[1]);
    do {
        printf("Enter option: ");
        scanf(" %c", &option);
        switch (option) {
            case 'a': {
                printf("Enter position & name: ");
                scanf(" %s", name);
                entries = (entry *)malloc(sizeof(entry));

                strcpy(entries[i].name, name);
                //entries[i].uni_register = uni_register;
                i++;
                check = doublist_add(head, name);
                if (check) {
                    printf("ola ok\n");
                }
                break;
            }
            case 'p': {
               
                break;
            }
            /*case 'f': {
                printf("Enter ClassID: ");
                scanf(" %d", &classID);
                curr = slist_find (head, classID, 1);
                if (curr == NULL) {
                    printf("NOT FOUND\n");
                }
                else {
                    printf("FOUND\n");
                }
                break;
            }
            case 'r': {
                printf("Enter ClassID: ");
                scanf(" %d", &classID);
                check = slist_rmv (&head, classID);
                break;
            }
            case 'c': {
                slist_clear (&head);
                break;
            }
            case 'q': {
                slist_clear (&head);
               return 0;
            } */
        }
    }
    while (1);


    return 0;
}