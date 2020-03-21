// Singly Linked List
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct simplist {
    unsigned short classID;
    int classes;
    struct simplist *nxt;
};
typedef struct simplist simplist;

void simplinit (struct simplist **head) {
    *head = NULL;
}
// If option = 1 -> find || rmv
// If option = 0 -> add
// Returns curr

simplist *simplist_find (struct simplist *head, unsigned short classID, char option) {
    struct simplist *curr, *prev;
    
    for (curr = head, prev = head; curr != NULL; curr = curr->nxt) {
        if (option) {
            if (curr->classID == classID) {
                return curr;
            }
        }
        else {
           if (curr->classID == classID) {
               return -1;
           }
           if (curr->classID > classID) {
               printf("prev -> %d\n", prev->classID);
               return prev;
           }
        }
        prev = curr;
    }
    printf("Telos\n");
    if (option) {
        return NULL;
    }
    else {
        //printf("returned curr->classID: %d\n", curr->classID);
        return prev;
    }
}

int simplist_add (struct simplist **head, unsigned short classID) {
    struct simplist *curr, *prev;
    
    if (*head == NULL) {
        printf("UNIHEAD -> %d\n", classID);
        curr = (struct simplist *)malloc(sizeof(struct simplist));
        curr->classID = classID;
        curr->nxt = *head;
        *head = curr;
        return 0;
    }
    // ClassID already exists
    prev = simplist_find (*head, classID, 0);
    if (prev == -1) {
        printf("Already exists\n");
        return 1;
    }
    if (prev == *head && classID < prev->classID) {
        printf("HEAD -> %d\n", classID);
        curr = (struct simplist *)malloc(sizeof(struct simplist));
        curr->classID = classID;
        curr->nxt = *head;
        *head = curr;
    }
    else {
        curr = (struct simplist *)malloc(sizeof(struct simplist));
        curr->classID = classID;
        curr->nxt = prev->nxt;
        prev->nxt = curr;   
    }
    /*
    curr = (struct simplist *)malloc(sizeof(struct simplist));
    curr->classID = classID;
    curr->nxt = *head;
    *head = curr;
    */
    return 0;
}

void print_list (struct simplist *head) {
    simplist *curr;

    curr = head;
    while (curr != NULL) {
        printf(" -> %d", curr->classID);
        curr = curr->nxt;
    }
    printf("\n");
}

int main (int argc, char *argv[]) {
    //struct simplist;
    char option;
    int classID, check;
    struct simplist *head = NULL;
    struct simplist *curr;

    do {
        printf("Enter option: ");
        scanf(" %c", &option);
        switch (option) {
            case 'a': {
                printf("Enter classID: ");
                scanf(" %d", &classID);
                check = simplist_add(&head, classID);
                if (check) {
                    printf("DOING\n");
                }
                break;
            }
            case 'p': {
                print_list(head);
                break;
            }
            case 'f': {
                printf("Enter ClassID: ");
                scanf(" %d", &classID);
                curr = simplist_find (head, classID, 1);
                if (curr == NULL) {
                    printf("NOT FOUND\n");
                }
                else {
                    printf("FOUND\n");
                }
                break;
            }
            case 'q': {
                return 0;
            }
        }
    }
    while (1);


    return 0;
}