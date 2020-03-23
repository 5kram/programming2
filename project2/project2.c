// Dynamic Memory Handling 
// Data Structure:
// University Register, Name of Student, Number of Fails  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define YES 1
#define NO 0

struct simplist {
    unsigned short classID;
    struct simplist *nxt;
};
typedef struct simplist simplist;

struct entry {
    long unsigned int uni_register;
    char name[64];
    short unsigned int fails;
    int classes;
    struct simplist *head;
};
typedef struct entry entry;

struct database {
    entry **entries;     // Entry ptr
    int size;           // Size of db
    int sorted;         // Sorted or not
    int students;       // No of students
};
typedef struct database database;

entry **database_init (database *db, int db_size) {
    entry **ptr;

    db->size = db_size;
    ptr = (entry **)realloc(db->entries, db->size * sizeof(entry*));
    db->entries = NULL;
    db->sorted = 0;
    db->students = 0;
    
    return ptr;
}

void slinit (struct simplist **head) {
    *head = NULL;
}
// If option = 1 -> find
// If option = 0 -> add
// If option = -1 -> rmv
// Returns curr

simplist *slist_find (struct simplist *head, unsigned short classID, char option) {
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

int slist_add (struct simplist **head, unsigned short classID) {
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
    prev = slist_find (*head, classID, 0);
    // ClassID already exists
    if (prev->classID == classID) {
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

int slist_rmv (struct simplist **head, unsigned short classID) {
    struct simplist *prev, *curr;

    prev = slist_find (*head, classID, -1);
    
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
            return 1;
        }

    return 0;
}

void slist_print (struct simplist *head) {
    simplist *curr;

    curr = head;
    while (curr != NULL) {
        printf(" -> %d", curr->classID);
        curr = curr->nxt;
    }
    printf("\n");
}

void slist_clear (struct simplist **head) {
    simplist *curr, *prev;

    curr = *head; 
    while (curr != NULL) {
        prev = curr;
        curr = prev->nxt;
        free(prev);
    }
    *head = NULL;
}

// If sorted -> binary search
// If not -> linear search
// If found returns position
// If not returns the size of db
// If called from main -> counts the comparisons
int find (struct database *db, long unsigned int uni_register, int sorted, int main_find) {
    int i, start, middle, end, comparisons = 0;
    // Sorted -> Binary Search
    if (sorted == YES) {
        start = 0;
        end = db->students - 1;
       
        do {
            middle = start + (end - start) / 2;
            comparisons++;
            if (db->entries[middle]->uni_register == uni_register) {
                if (main_find == YES) {
                    fprintf(stderr, "\n$%d\n", comparisons);
                }
                return middle;
            }
            comparisons++;
            if (db->entries[middle]->uni_register < uni_register) {
                start = middle + 1;
            }
            else {
                end = middle - 1;
            }
        }
        while (start <= end);
        if (main_find == YES) {
            fprintf(stderr, "\n$%d\n", comparisons);
        }
        return db->size;
    }
    // Unsorted -> Linear Search
    else {
        for (i = 0; i < db->students; i++) {
            comparisons++;
            if (db->entries[i]->uni_register == uni_register) {
                if (main_find == YES) {
                    fprintf(stderr, "\n$%d\n", comparisons);
                }
                return i;
            }
        }
        if (main_find == YES) {
            fprintf(stderr, "\n$%d\n", comparisons);
        }
        return db->size;
    }
}

int reg (struct database *db, long unsigned int uni_register, unsigned short classID) {
    int pos, check;

    pos = find (db, uni_register, db->sorted, 0);
    if (pos == db->size) {
        return -1;
    }
    else {
        // Head Init
        if (!db->entries[pos]->classes) {
            db->entries[pos]->head = NULL;
        }
        check = slist_add (&(db->entries[pos]->head), classID);
        if (check) {
            return 0;
        }
        db->entries[pos]->classes++;
    }
    
    return 1;
}

int unreg (struct database *db, long unsigned int uni_register, unsigned short classID) {
    int pos, check;

    pos = find (db, uni_register, db->sorted, 0);
    if (pos == db->size) {
        return -1;
    }
    else {
        check = slist_rmv (&(db->entries[pos]->head), classID);
        if (check) {
            printf("\nU-OK %lu %hu\n", uni_register, classID);
            return 1;
        }
        else {
            return 0;
        }
    }

    //return 0;
}

int isreg (struct database *db, long unsigned int uni_register, unsigned short classID) {
    struct simplist *curr;
    int pos;

    pos = find (db, uni_register, db->sorted, 0);
    if (pos == db->size) {
        printf("\nI-NOK %lu\n", uni_register);
        return -1;
    }
    else {
        // **********CHECK POINTER***********
        curr = slist_find ((db->entries[pos]->head), classID, 1);
        if (curr != NULL) {
            return 1;
        }
        else {
            return 0;
        }
    }

}

// Memory Handling
// Option = 0 -> Called by add function(add memory)
// Option = 1 -> Called by rmv function(rmv memory)
entry **MemoryCheck (struct database *db, int option, int fluctuation) {
    entry **ptr;

    ptr = db->entries;
    // Remove Condition
    if (db->size - db->students == fluctuation && option == 1) {
        // Remove -> Downsize Memory
        fluctuation = - fluctuation;
        ptr = (entry **)realloc(db->entries, (db->size + fluctuation) * sizeof(entry*));
        db->size = db->size + fluctuation;
        return ptr;
    }
    // Add Condition
    else if (db->size - db->students == 0 && option == 0) {
        ptr = (entry **)realloc(db->entries, (db->size + fluctuation) * sizeof(entry*));
        db->size = db->size + fluctuation;
        return ptr;
    }
    
    return ptr;
}

int add (struct database *db, long unsigned int uni_register, char name[64], short unsigned int fails, int fluctuation) {
    int pos = 0, i;
    entry **ptr;
    entry *entry_ptr;
    
    for (i = 0; i < strlen(name); i++) {
        name[i] = toupper(name[i]);
    }
    pos = find(db, uni_register, db->sorted, 0);
    if (pos < db->size) {
        printf("\nA-NOK %lu, %d %d\n", uni_register, db->students, db->size);
        return 1;
    }
    ptr = MemoryCheck(db, 0, fluctuation);
    if (ptr == NULL) {
        return 1;
    }

    db->entries = ptr;
    // Dynamically Allocate Memory for a specific entry
    entry_ptr = (entry *)malloc(sizeof(entry));
    entry_ptr->uni_register = uni_register;
    strcpy(entry_ptr->name, name);
    entry_ptr->fails = fails;
    entry_ptr->classes = 0;
    
    db->entries[db->students] = entry_ptr;
    db->students++;
    db->sorted = NO;

    printf("\nA-OK %lu, %d %d\n", uni_register, db->students, db->size);

    return 0;
}

int print (database *db) {
    int i;

    printf("\n#\n");
    for (i = 0; i < db->students; i++) {
        printf("%ld %s %hu\n", db->entries[i]->uni_register, db->entries[i]->name, db->entries[i]->fails);
        slist_print (db->entries[i]->head);
    }
    
    return 1;
}

void clear (database *db) {
    int i;
    
    for (i = 0; i < db->students; i++) {
        slist_clear (&(db->entries[i]->head));
    }
    free(db->entries);
    db->size = 0;
    db->students = 0;
    db->entries = NULL;
    db->sorted= NO;
   
}
// Insertion Sort
void sort (database *db) {
    int i, j, swap = 0, paws = 0;
    entry temp;

    for (i = 1; i < db->students; i++) {
        temp.uni_register = db->entries[i]->uni_register;
        temp.fails = db->entries[i]->fails;
        strcpy(temp.name, db->entries[i]->name);
        j = i - 1;
        swap++;
        while (j >= 0 && db->entries[j]->uni_register > temp.uni_register) {
            db->entries[j + 1]->uni_register = db->entries[j]->uni_register;
            db->entries[j + 1]->fails = db->entries[j]->fails;
            strcpy(db->entries[j + 1]->name, db->entries[j]->name);
            j = j - 1;
            swap++;
        }
        db->entries[j + 1]->uni_register = temp.uni_register;
        db->entries[j + 1]->fails = temp.fails;
        strcpy(db->entries[j + 1]->name, temp.name);
        paws++;
    }
    // Already sorted
    if (swap == paws) {
        fprintf(stderr, "\n$%d\n", swap);
    }
    // Unsorted
    else {
        fprintf(stderr, "\n$%d\n", swap - paws);
    }
    printf("\nS-OK\n");
    db->sorted = YES;
}

int mod (database *db, long unsigned int uni_register, short unsigned int fails) {
    int pos;

    pos = find (db, uni_register, db->sorted, 0);
    if (pos == db->size) {
        return 0;
    }
    else {
        db->entries[pos]->fails = fails;
        return 1;
    }
}
// Last Registration moves to Removed position
// + No NULL in-between
// - Unsorted
int rmv (database *db, long unsigned int uni_register, int fluctuation) {
    int pos;
    entry **ptr;

    pos = find (db, uni_register, db->sorted, 0);
    if (pos == db->size) {
        printf("\nR-NOK %lu, %d %d\n", uni_register, db->students, db->size);
    }
    else {
        slist_clear (&(db->entries[pos]->head));
        db->entries[pos]->name[0] = '\0';
        db->entries[pos]->uni_register = db->entries[db->students - 1]->uni_register;
        strcpy(db->entries[pos]->name, db->entries[db->students -1]->name);
        db->entries[pos]->fails = db->entries[db->students - 1]->fails;
        free(db->entries[db->students - 1]);
        
        ptr = MemoryCheck(db, 1, fluctuation);
        if (ptr == NULL) {
            return 1;
        }
        db->entries = ptr;
        db->students--;
        db->sorted = NO;
        printf("\nR-OK %lu, %d %d\n", uni_register, db->students, db->size);
    }
    
    return 0;
}

int main (int argc, char *argv[]) {
    struct database *db;
    char option, name[64];
    long unsigned int uni_register;
    short unsigned int fails;
    int fluctuation, pos;
    unsigned short classID;

    db = (database *)malloc(sizeof(database));
    fluctuation = atoi(argv[2]);
    db->entries = database_init(db, atoi(argv[1]));

    do {
        scanf (" %c", &option);
        switch(option) {
            case 'a': {
                scanf(" %lu %s %hu", &uni_register, name, &fails);
                add(db, uni_register, name, fails, fluctuation);
                break;
            }
            case 'g': {
                scanf(" %lu %hu", &uni_register, &classID);
                pos = reg (db, uni_register, classID);
                if (pos == -1) {
                    printf("\nG-NOK %lu\n", uni_register);
                }
                else if (pos) {
                    printf("\nG_OK %lu %hu\n", uni_register, classID);
                }
                else {
                    printf("\nG-NOK %hu\n", classID);
                }
                break;
            }
            case 'r': {
                scanf(" %lu", &uni_register);
                pos = rmv (db, uni_register, fluctuation);
                break;
            }
            case 'm': {
                scanf(" %lu %hu", &uni_register, &fails);
                pos = mod (db, uni_register, fails);
                if (pos == 0) {
                    printf("\nM-NOK %lu\n", uni_register);
                }
                else {
                    printf("\nM-OK %lu\n", uni_register);
                }
                break;
            }
            case 's': {
                sort (db);
                break;
            }
            case 'f': {
                scanf(" %lu", &uni_register);
                pos = find(db, uni_register, db->sorted, 1);
                if (pos < db->size) {
                    printf("\nF-OK %s %hu\n", db->entries[pos]->name, db->entries[pos]->fails);
                }
                else {
                    printf("\nF-NOK %lu\n", uni_register);
                }
                break;
            }
            case 'i': {
                scanf(" %lu %hu", &uni_register, &classID);
                pos = isreg (db, uni_register, classID);
                if (pos == 1) {
                    printf("\nYES\n");
                }
                else if (pos == 0) {
                    printf("\nNO\n");
                }
                break;
            }
            case 'u': {
                scanf(" %lu %hu", &uni_register, &classID);
                pos = unreg (db, uni_register, classID);
                if (pos == -1) {
                    printf("\nU-NOK %lu\n", uni_register);
                }
                else if (pos == 0){
                    printf("\nU-NOK %hu\n", classID);
                }
                break;
            }
            case 'p': {
                pos = print (db);
                break;
            }
            case 'c': {
                clear (db);
                printf("\nC-OK\n");
                break;
            }
            case 'q': {
                clear (db);
                return 0;
            }
            default : {
                continue;
            }
        }
    }
    while (1);

    return 0;
}
