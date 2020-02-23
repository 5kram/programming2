// Dynamic Memory Handling 
// Data Structure:
// University Register, Name of Student, Number of Fails  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define YES 1
#define NO 0

struct entry {
    long unsigned int uni_register;
    char name[64];
    short unsigned int fails;
};
typedef struct entry entry;

struct database {
    entry *entries;     // Entry ptr
    int size;           // Size of db
    int sorted;         // Sorted or not
    int students;       // No of students
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
// If sorted -> binary search
// If not -> linear search
// If found returns position
// If not returns the size of db
int find (struct database *db, int uni_register, int sorted) {
    int i, start, middle, end, comparisons = 0;
    // Sorted -> Binary Search
    if (sorted == YES) {
        //printf("Sorted");
        start = 0;
        end = db->students - 1;
       
        do {
            middle = start + (end - start) / 2;
            //printf("start -> %d, end -> %d, students -> %d\n", start, end, db->students);
            comparisons++;
            if (db->entries[middle].uni_register == uni_register) {
                fprintf(stderr, "\n%d\n", comparisons);
                return middle;
            }
            comparisons++;
            if (db->entries[middle].uni_register < uni_register) {
                start = middle + 1;
            }
            else {
                end = middle - 1;
            }
        }
        while (start <= end);
        fprintf(stderr, "\n%d\n", comparisons);
        return db->size;
    }
    // Unsorted -> Linear Search
    else {
        for (i = 0; i < db->students; i++) {
            if (db->entries[i].uni_register == uni_register) {
                return i;
            }
        }
        return db->size;
    }
    
}
// Memory Handling
// Option = 0 -> Called by add function(add memory)
// Option = 1 -> Called by rmv function(rmv memory)
entry *MemoryCheck (struct database *db, int option, int fluctuation) {
    entry *ptr;

    ptr = db->entries;
    // Remove Condition
    if (db->size - db->students == fluctuation && option == 1) {
        // Remove -> Downsize Memory
        fluctuation = - fluctuation;
        ptr = (entry *)realloc(db->entries, (db->size + fluctuation) * sizeof(entry));
        db->size = db->size + fluctuation;
        return ptr;
    }
    // Add Condition
    else if (db->size - db->students == 0 && option == 0) {
        ptr = (entry *)realloc(db->entries, (db->size + fluctuation) * sizeof(entry));
        db->size = db->size + fluctuation;
        return ptr;
    }
    
    return ptr;
}

int add (struct database *db, long unsigned int uni_register, char name[64], short unsigned int fails, int fluctuation) {
    int pos = 0, i;
    entry *ptr;
    
    for (i = 0; i < strlen(name); i++) {
        name[i] = toupper(name[i]);
    }
    pos = find(db, uni_register, db->sorted);
    //printf("pos -> %d, size -> %d", pos, db->size);
    if (pos < db->size) {
        printf("\nA-NOK %lu, %d %d\n", uni_register, db->students, db->size);
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
    printf("\nA-OK %lu, %d %d\n", uni_register, db->students, db->size);

    return 0;
}

int print (database *db) {
    int i;

    printf("\n#\n");
    for (i = 0; i < db->students; i++) {
        printf("%ld %s %hu\n", db->entries[i].uni_register, db->entries[i].name, db->entries[i].fails);
    }
    return 1;
}

void clear (database *db) {

    //db->entries = NULL;
    free(db->entries);
    db->size = 0;
    db->students = 0;
   
}
// Insertion Sort
void sort (database *db) {
    int i, j, comparisons = 0;
    entry temp;

    for (i = 1; i <= db->students; i++) {
        for (j = i - 1; j >= 0; j--) {
            comparisons++;
            if(db->entries[j].uni_register < db->entries[j - 1].uni_register) {
                temp.uni_register = db->entries[j - 1].uni_register;
                strcpy(temp.name, db->entries[j - 1].name);
                temp.fails = db->entries[j - 1].fails;
                db->entries[j - 1].uni_register = db->entries[j].uni_register;
                strcpy(db->entries[j - 1].name, db->entries[j].name);
                db->entries[j - 1].fails = db->entries[j].fails;
                db->entries[j].uni_register =  temp.uni_register;
                strcpy(db->entries[j].name, temp.name);
                db->entries[j].fails = temp.fails;
            }
        }
    }
    db->sorted = YES;
    printf("\nS-OK\n");
    fprintf(stderr, "\n%d\n", comparisons);
}

int mod (database *db, long unsigned int uni_register, short unsigned int fails) {
    int pos;

    pos = find (db, uni_register, db->sorted);
    if (pos == db->size) {
        return 0;
    }
    else {
        db->entries[pos].fails = fails;
        return 1;
    }

}
// Last Registration moves to Removed position
// +No NULL in-between
// -Unsorted
int rmv(database *db, long unsigned int uni_register, int fluctuation) {
    int pos;
    entry *ptr;

    pos = find (db, uni_register, db->sorted);
    if (pos == db->size) {
        printf("\nR-NOK %lu, %d %d\n", uni_register, db->students, db->size);
    }
    else {
        db->entries[pos].uni_register = db->entries[db->students - 1].uni_register;
        strcpy(db->entries[pos].name, db->entries[db->students -1].name);
        db->entries[pos].fails = db->entries[db->students - 1].fails;
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
                pos = find(db, uni_register, db->sorted);
                if (pos < db->size) {
                    printf("\nF-OK %s %hu\n", db->entries[pos].name, db->entries[pos].fails);
                }
                else {
                    printf("\nF-NOK %lu\n", uni_register);
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