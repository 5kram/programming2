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
// If option = 1 -> find
// If option = 0 -> add
// If option = -1 -> rmv
// Returns curr

simplist *simplist_find (struct simplist *head, unsigned short classID, char option) {
    struct simplist *curr, *prev;
    
    for (curr = head, prev = head; curr != NULL; curr = curr->nxt) {
        if (curr->classID == classID) {
            if (option == 1 || option == 0) {
                return curr;
            }
            else {
                return prev;
            }
        }
        if (curr->classID > classID) {
            if (option == 0) {
                return prev;
            }
        }
        prev = curr;
    }

    if (option == 0) {
        return prev;
    }
    else {
        return NULL;
    }
}

int simplist_add (struct simplist **head, unsigned short classID) {
    struct simplist *curr, *prev;
    // Empty list
    if (*head == NULL) {
        //printf("UNIHEAD -> %d\n", classID);
        curr = (struct simplist *)malloc(sizeof(struct simplist));
        curr->classID = classID;
        curr->nxt = *head;
        *head = curr;
        return 0;
    }
    // Returns the prev pointer
    // prev->classID < classID < nxt->classID
    prev = simplist_find (*head, classID, 0);
    // ClassID already exists
    if (prev->classID == classID) {
        printf("Already exists\n");
        return 1;
    }
    // First in the list
    if (prev == *head && classID < prev->classID) {
        //printf("HEAD -> %d\n", classID);
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
    
    return 0;
}

int simplist_rmv (struct simplist **head, unsigned short classID) {
    struct simplist *prev, *curr;

    prev = simplist_find (*head, classID, -1);
    
        if (prev == *head && prev->classID == classID) {
            curr = prev;
            //printf("HEAD-> %d\n",prev->nxt->classID);

            *head = prev->nxt;
            
            free(curr);
            return 1;
        }
        else if (prev != NULL) {
            //printf("prev-> %d prev->nxt-> %d prev->nxt->nxt-> %d\n", prev->classID, prev->nxt->classID, prev->nxt->nxt->classID); 
            curr = prev->nxt;
            prev->nxt = prev->nxt->nxt;
            free (curr);
        }
        
    



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
            case 'r': {
                printf("Enter ClassID: ");
                scanf(" %d", &classID);
                check = simplist_rmv (&head, classID);
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