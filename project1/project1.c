// Dynamic Memory Database with students
// and their number of  register and failed subjects
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define YES 1
#define NO 0

struct entry {
    long unsigned int uni_register;
    char name[64];
    short unsigned int fails;
};
typedef struct entry entry;
// entry ptr, size of db, indicate if db is sorted, no of students
struct database {
    entry *entries;
    int size;
    int sorted;
    int students;
};
typedef struct database database;

entry *database_init (database *db, int db_size) {
    entry *ptr;

    db->size = db_size;
    ptr = (entry *)realloc(db->entries, db->size * sizeof(entry));
    db->entries = NULL;
    db->sorted = 0;
    db->students = 0;
    
    return ptr;
}

int find (struct database *db, int uni_register) {
    int i;
    // If found -> position
    // If not -> size of db
    for (i = 0; i < db->size; i++) {
        if (db->entries[i].uni_register == uni_register) {
            return i;
        }
    }

    return db->size;
}
// Memory Handling
// Option = 0 -> Called by add function(add memory)
// Option = 1 -> Called by rmv function(rmv memory)
entry *MemoryCheck (struct database *db, int option, int fluctuation) {
    entry *ptr;
    // Remove Condition || Add Condition
    if (db->size - db->students == fluctuation || db->size - db->students == 0) {
        // Remove -> Downsize Memory
        if (option == 1) {
            fluctuation = - fluctuation;
        }
        ptr = (entry *)realloc(db->entries, (db->size + fluctuation) * sizeof(entry));
    }
    
    return ptr;
}

int add (struct database *db, long unsigned int uni_register, char name[64], short unsigned int fails, int fluctuation) {
    int pos = 0;
    entry *ptr;
    
    pos = find(db, uni_register);
    if (pos < db->size) {
        printf("\nA-NOK %lu, %d, %d\n", uni_register, db->students, db->size);
        return 1;
    }
    ptr = MemoryCheck(db, 0, fluctuation);
    if (ptr == NULL) {
        return 1;
    }
    db->entries = ptr;
    db->entries[db->students].uni_register = uni_register;
    strcpy(db->entries[db->students].name, name);
    db->entries[db->students].fails = fails;
    db->students++;
    db->sorted = NO;
    printf("\nA-OK %lu, %d, %d\n", uni_register, db->students, db->size);

    return 0;
}

int main (int argc, char *argv[]) {
    struct database *db;
    char option, name[64];
    long unsigned int uni_register;
    short unsigned int fails;
    int fluctuation;

    db = (database *)malloc(sizeof(database));
    //db->size = atoi(argv[1]);
    db->entries = database_init(db, atoi(argv[1]));
    fluctuation = atoi(argv[2]);

    do {
        scanf (" %c", &option);
        switch(option) {
            case 'a': {
                scanf(" %lu %s %hu", &uni_register, name, &fails);
                add(db, uni_register, name, fails, fluctuation);
                break;
            }
            case 'r': {
                scanf(" %lu", &uni_register);

                break;
            }
            case 'm': {
                scanf(" %lu %hu", &uni_register, &fails);

                break;
            }
            case 's': {

                break;
            }
            case 'f': {
                scanf(" %lu", &uni_register);

                break;
            }
            case 'p': {
                
                break;
            }
            case 'c': {

                break;
            }
            case 'q': {
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