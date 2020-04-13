// Dynamic Memory Handling
// Hash Function
// Doubly Linked List with Terminal Sentinel
// Singly Linked List
// Data Structure:
// University Register, Name of Student, Number of Fails, Courses
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_SIZE 250001
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
    struct entry *nxt;
    struct entry *prv;
};
typedef struct entry entry;

struct database {
    entry **entries;     // Entry ptr
    int size;           // Size of db
    int sorted;         // Sorted or not
    int students;       // No of students
};
typedef struct database database;

struct doublist {
    entry **head;
    int size;
    int largest_bucket;
    int list_size[MAX_SIZE];
    int cleared;
    int min_size;
};
typedef struct doublist doublist;

void database_init (database **db, int db_size) {
    
    *db = (database *)malloc(sizeof(database));
    (*db)->size = db_size;
    (*db)->entries = (entry **)calloc((*db)->size , sizeof(entry*));
    (*db)->sorted = 0;
    (*db)->students = 0;
    
    return ;
}

void slinit (struct simplist **head) {
    *head = NULL;
}

void doublist_init (doublist **dl, int size) {
    entry *sentinel;
    int i;

    (*dl) = (doublist *)malloc(sizeof(doublist));
    (*dl)->head = (entry**)calloc(size, sizeof(entry*));
    // Sentinel init
    for (i = 0; i < size; i++) {
        sentinel = (entry *)malloc(sizeof(entry));
        strcpy(sentinel->name, "-1");
        sentinel->nxt = sentinel;
        sentinel->prv = sentinel;
        (*dl)->head[i] = sentinel;
        (*dl)->list_size[i] = 0;
    }
    (*dl)->largest_bucket = 0;
    (*dl)->size = size;
    (*dl)->cleared = NO;
    return ;
}

unsigned long hash (char *str, int size) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return (hash % size);
}
// Finds Largest Bucket
int find_bucket (doublist *dl) {
    int i;
    
    dl->largest_bucket = 0;
    for (i = 0; i < dl->size; i++) {
        if (dl->largest_bucket < dl->list_size[i]) {
            dl->largest_bucket = dl->list_size[i];
        }
    }
    return dl->largest_bucket;
}
// Find -> option = -1
// Rmv -> option = 0
// Add -> option = 1
entry *doublist_find (doublist *dl, char name[], int option, int index) {
    entry *curr = NULL;
    int comparisons;

    strcpy(dl->head[index]->name, name);
    // Condition of equality
    if (option == 0 || option == -1) {
        for (curr = dl->head[index]->nxt, comparisons = 0; strcmp(curr->name, name); curr = curr->nxt, comparisons++) ;
        strcpy(dl->head[index]->name, "-1");
        if (option == -1) {
            fprintf(stderr, "\n$%d\n", comparisons);
        }
        if (curr == dl->head[index]) {
            return NULL;
        }
        else {
            return curr;  
        }
    }
    // Condition of Unequality
    else {
        for (curr = dl->head[index]->nxt; !(strcmp(curr->name, name) >= 0); curr = curr->nxt) ;
        strcpy(dl->head[index]->name, "-1");
        return curr;
    }

}

void find_by_name (doublist *dl, char name[], int size) {
    entry *curr;
    int index, i;

    for (i = 0; i < strlen(name); i++) {
        name[i] = toupper(name[i]);
    }
    index = hash(name, size);
    curr = doublist_find (dl, name, -1, index);
    if (curr != NULL) {
        printf("\nN-OK %s\n", curr->name);
        do {
            printf("%lu %hu\n", curr->uni_register, curr->fails);
            curr = curr->nxt;
        }
        while (!strcmp(curr->name, name) && curr != dl->head[index]);
    }
    else {
        printf("\nN-NOK %s\n", name);
    }
}

entry *doublist_add (doublist *dl, char name[], long unsigned int uni_register, short unsigned int fails, int classes) {
    entry *prev_new, *new, *curr = NULL;
    int index;

    new = (entry *)calloc(1, sizeof(entry));
    // Data handling
    new->uni_register = uni_register;
    strcpy(new->name, name);
    new->fails = fails;
    new->classes = classes;
    index = hash(name, dl->size);
    curr = doublist_find (dl, new->name, 1, index);
    // Condition for finding the biggest uni_register, if same name
    while (!strcmp(curr->name, name) && curr->uni_register < uni_register && curr != dl->head[index])
     {
        curr = curr->nxt;
    }
    // Pointer handling
    prev_new = curr->prv;
    new->nxt = curr;
    new->prv = prev_new;
    curr->prv = new;
    prev_new->nxt = new;

    dl->list_size[index]++;
    dl->largest_bucket = find_bucket(dl);
    return new;
}

entry *doublist_rmv (doublist *dl, char name[], int index) {
    entry *curr;

    curr = doublist_find (dl, name, 0, index);
    curr->prv->nxt = curr->nxt;
    curr->nxt->prv = curr->prv;
    
    return curr;
}

void doublist_print (doublist *dl) {
    entry *curr;
    int i, j;

    for (i = 0; i < dl->size; i++) {
        printf("\n%d %d", i, dl->list_size[i]);
        for (j = 0, curr = dl->head[i]->nxt; j < dl->list_size[i]; curr = curr->nxt, j++) {
            if (!strcmp(curr->name, "-1")) {
                break;
            }
            printf(" [%lu %s %hu]", curr->uni_register, curr->name, curr->fails);
        }
        printf("\n");
    }
    printf("\n");
}

// If option = 1 -> find
// If option = 0 -> add
// If option = -1 -> rmv
// Returns curr
simplist *slist_find (struct simplist *head, unsigned short classID, char option) {
    struct simplist *curr, *prev;
    
    for (curr = head, prev = head; curr != NULL; curr = curr->nxt) {
        if (curr->classID == classID) {
            // find, add need the exact node
            if (option == 1 || option == 0) {
                return curr;
            }
            // rmv needs the prev node
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
    // head rmved
    if (prev == *head && prev->classID == classID) {
        curr = prev;
        *head = prev->nxt;
        free(curr);
        return 1;
    }
    // !head rmved
    else if (prev != NULL) {
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
        printf("%d\n", curr->classID);
        curr = curr->nxt;
    }
}
// Called by Rehash -> 1 (free(all))
// Called by Clear -> 0
void doublist_clear (doublist **dl, int option) {
    int i, j;
    entry *curr;

    for (i = 0; i < (*dl)->size; i++) {
        for (j = 0, curr = (*dl)->head[i]->nxt; j < (*dl)->list_size[i]; curr = curr->nxt, j++) {
            if (!strcmp(curr->name, "-1")) {
                break;
            } 
            free(curr);
        }
        (*dl)->list_size[i] = 0;
        if (option) {
            free((*dl)->head[i]);
        }
        
    }
    (*dl)->size = (*dl)->min_size;
    (*dl)->largest_bucket = 0;
    (*dl)->cleared = YES;
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
                    //fprintf(stderr, "\n$%d\n", comparisons);
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
            //fprintf(stderr, "\n$%d\n", comparisons);
        }
        return db->size;
    }
    // Unsorted -> Linear Search
    else {
        for (i = 0; i < db->students; i++) {
            comparisons++;
            if (db->entries[i]->uni_register == uni_register) {
                if (main_find == YES) {
                    //fprintf(stderr, "\n$%d\n", comparisons);
                }
                return i;
            }
        }
        if (main_find == YES) {
            //fprintf(stderr, "\n$%d\n", comparisons);
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
        // Empty list
        if (!db->entries[pos]->classes) {
            return 0;
        }
        check = slist_rmv (&(db->entries[pos]->head), classID);
        if (check) {
            printf("\nU-OK %lu %hu\n", uni_register, classID);
            db->entries[pos]->classes--;
            return 1;
        }
        else {
            return 0;
        }
    }

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
        curr = slist_find ((db->entries[pos]->head), classID, 1);
        if (curr != NULL) {
            return 1;
        }
        else {
            return 0;
        }
    }
}

int list_courses (struct database *db, long unsigned int uni_register) {
    int pos;

    pos = find (db, uni_register, db->sorted, 0);
    if (pos == db->size) {
        return 0;
    }
    else {
        printf("\nL-OK %s\n", db->entries[pos]->name);
        slist_print (db->entries[pos]->head);
        return 1;
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
int print (database *db, doublist *dl) {
    //int i;
    double loadf;
    printf("\n#\n");
    /*
    for (i = 0; i < db->students; i++) {
        printf("%ld %s %hu\n", db->entries[i]->uni_register, db->entries[i]->name, db->entries[i]->fails);
        //slist_print (db->entries[i]->head);
    }
    */
    loadf = (double)db->students / dl->size;
    printf("%d %d %.2lf %d", dl->size, db->students, loadf, dl->largest_bucket);
    doublist_print(dl);

    return 1;
}
// add -> 1
// rmv -> 0
doublist *rehash (database *db, doublist *dl, int option) {
    int i, new_size, j, old;
    entry *curr;
    doublist *new_dl = NULL;
    
    old  = dl->size;
    if (option) {
        new_size = 2 * dl->size;
    }
    else {
        new_size = dl->size / 2;
        if (new_size < dl->min_size) {
            new_size = dl->min_size;
        }
    }
    dl->size = new_size;
    // Creates new doublist with the new size
    doublist_init(&new_dl, new_size);
    new_dl->size = dl->size;
    new_dl->min_size = dl->min_size;
    new_dl->largest_bucket = 0;
    for (i = 0; i < db->students; i++) {  
        curr = doublist_add (new_dl, db->entries[i]->name, db->entries[i]->uni_register, db->entries[i]->fails, db->entries[i]->classes);
        db->entries[i] = curr;
        //slist_clear(&(db->entries[i]->head));
    }
    new_dl->cleared = dl->cleared;
    // Delete old dl // Didnt run with doublist_clear
    for (i = 0; i < old; i++) {
        for (j = 0, curr = (dl)->head[i]->nxt; j < (dl)->list_size[i]; curr = curr->nxt, j++) {
            if (!strcmp(curr->name, "-1")) {
                break;
            }
            free(curr);
        }
        free((dl)->head[i]);
    }
    free((dl)->head);
    free(dl);
   
    return new_dl;
}

void add (database **db, long unsigned int uni_register, char name[64], short unsigned int fails, int fluctuation, doublist **dl) {
    int pos = 0, i, classes;
    entry **ptr;
    entry *entry_ptr;
    double lf;
    
    for (i = 0; i < strlen(name); i++) {
        name[i] = toupper(name[i]);
    }
    pos = find(*db, uni_register, (*db)->sorted, 0);
    if (pos < (*db)->size) {
        printf("\nA-NOK %lu, %d %d\n", uni_register, (*db)->students, (*db)->size);
        return ;
    }
    ptr = MemoryCheck(*db, 0, fluctuation);
    if (ptr == NULL) {
        return ;
    }

    (*db)->entries = ptr;
    // Dynamically Allocate Memory for a specific entry
    /*
    entry_ptr = (entry *)malloc(sizeof(entry));
    entry_ptr->uni_register = uni_register;
    strcpy(entry_ptr->name, name);
    entry_ptr->fails = fails;
    entry_ptr->classes = 0;
    
    db->entries[db->students] = entry_ptr;
    db->students++;
    db->sorted = NO;
    index = hash(name, dl->size);
    */
    classes = 0;
    entry_ptr = doublist_add (*dl, name, uni_register, fails, classes);
    (*db)->entries[(*db)->students] = entry_ptr;
    (*db)->students++;
    (*db)->sorted = NO;
    // Checks if rehash needing
    lf = (double)(*db)->students / (*dl)->size;
    if (lf >= 4) {
        *dl = rehash ((*db), (*dl), 1);
    }
    
    printf("\nA-OK %lu, %d %d\n", uni_register, (*db)->students, (*db)->size);

    return ;
}


// free all -> 1
// clear -> 0
void clear (database **db, doublist **dl, int option) {
    int i;
    
    for (i = 0; i < (*db)->students; i++) {
        if ((*db)->entries[i]->classes > 0) {
            (*db)->entries[i]->classes = 0;
            slist_clear (&((*db)->entries[i]->head));
        }
    }
    doublist_clear (&(*dl), option);
    (*db)->size = 0;
    (*db)->students = 0;
    (*db)->entries = NULL;
    (*db)->sorted= NO;
    if (option) {
        free((*dl)->head);
        free(*dl);
        free((*db)->entries);
        free(*db); 
    }
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
doublist *rmv (database *db, doublist *dl, long unsigned int uni_register, int fluctuation) {
    int pos, index;
    entry **ptr, *deleted;
    double lf;

    pos = find (db, uni_register, db->sorted, 0);
    if (pos == db->size) {
        
        return NULL;
    }
    else {
        index = hash (db->entries[pos]->name, dl->size);
        dl->list_size[index]--;
        dl->largest_bucket = find_bucket (dl);
        
        // Is not the last in database
        // Need to move the last one in the "gap"
        if (pos != db->students - 1) { 
            db->entries[pos]->name[0] = '\0';
            db->entries[pos]->uni_register = 0;
            // Filling the gap with the last student
            db->entries[pos]->uni_register = db->entries[db->students - 1]->uni_register;
            strcpy(db->entries[pos]->name, db->entries[db->students -1]->name);
            db->entries[pos]->fails = db->entries[db->students - 1]->fails;
            
            deleted = doublist_rmv (dl, db->entries[pos]->name, index);
            // Correcting the pointers
            deleted->nxt = db->entries[db->students - 1]->nxt;
            deleted->prv = db->entries[db->students - 1]->prv;
            deleted->head = db->entries[db->students - 1]->head;
            deleted->classes = db->entries[db->students - 1]->classes;
            // **if the deleted is the nxt or prv of the last entry 
            if (deleted != db->entries[db->students - 1]) {
                db->entries[db->students - 1]->prv->nxt = deleted;
                db->entries[db->students - 1]->nxt->prv = deleted;
            }
            slist_clear (&(db->entries[pos]->head));
            free(db->entries[db->students - 1]);
        }
        // Is the last in database
        else {
            deleted = doublist_rmv (dl, db->entries[pos]->name, index);
            free(deleted);
        }
        
        ptr = MemoryCheck(db, 1, fluctuation);
        if (ptr == NULL) {
            return NULL;
        }
        db->entries = ptr;
        db->students--;
        db->sorted = NO;
        // Checks if rehash needing
        lf = (double)db->students / dl->size;
        if (lf == 1) {
            dl = rehash (db, dl, 0);
        }
        printf("\nR-OK %lu, %d %d\n", uni_register, db->students, db->size);
    }
    
    return dl;
}

int main (int argc, char *argv[]) {
    database *db;
    char option, name[64];
    long unsigned int uni_register;
    short unsigned int fails;
    int fluctuation, pos;
    unsigned short classID;
    doublist *dl, *check;

    // Init
    fluctuation = atoi(argv[2]);
    database_init(&db, atoi(argv[1]));
    doublist_init(&dl, atoi(argv[3]));
    dl->min_size = atoi(argv[3]);
    do {
        scanf (" %c", &option);
        switch(option) {
            case 'a': {
                scanf(" %lu %s %hu", &uni_register, name, &fails);
                if (dl->cleared == YES) {
                   doublist_init(&dl, atoi(argv[3]));
                   dl->cleared = NO;
                }
                add(&db, uni_register, name, fails, fluctuation, &dl);
                break;
            }
            case 'g': {
                scanf(" %lu %hu", &uni_register, &classID);
                pos = reg (db, uni_register, classID);
                if (pos == -1) {
                    printf("\nG-NOK %lu\n", uni_register);
                }
                else if (pos) {
                    printf("\nG-OK %lu %hu\n", uni_register, classID);
                }
                else {
                    printf("\nG-NOK %hu\n", classID);
                }
                break;
            }
            case 'n': {
                scanf(" %s", name);
                find_by_name (dl, name, atoi(argv[3]));
                break;
            }
            case 'r': {
                scanf(" %lu", &uni_register);
                check = rmv (db, dl, uni_register, fluctuation);
                if (check == NULL) {
                    printf("\nR-NOK %lu, %d %d\n", uni_register, db->students, db->size);
                }
                else {
                    dl = check;
                }
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
            case 'l': {
                scanf(" %lu", &uni_register);
                pos = list_courses (db, uni_register);
                if (!pos) {
                    printf("\nL-NOK %lu\n", uni_register);
                }
                break;
            }
            case 'p': {
                pos = print (db, dl);
                break;
            }
            case 'c': {
                clear (&db, &dl, 0);
                printf("\nC-OK\n");
                break;
            }
            case 'q': {
                clear (&db, &dl, 1);
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