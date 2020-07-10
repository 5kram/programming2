// Dynamic Memory Handling 
// Data Structure:
// University Register, Name of Student, Number of Fails  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#define YES 1
#define NO 0

struct entry {
    long unsigned int uni_register;
    char name[64];
    short unsigned int fails;
};
typedef struct entry entry;

struct database {
    entry **entries;     // Entry ptr
    int size;           // Size of db
    int sorted;         // Sorted or not
    int students;       // No of students
};
typedef struct database database;

entry **database_init(database **db, int db_size) {
    entry **ptr;

    
    ptr = (entry **)malloc(db_size * sizeof(entry*));
    (*db)->size = db_size;
    (*db)->entries = NULL;
    (*db)->sorted = 0;
    (*db)->students = 0;
    
    return ptr;
}

// If sorted -> binary search
// If not -> linear search
// If found returns position
// If not returns the size of db
// If called from main -> counts the comparisons
int find(struct database *db, int uni_register, int sorted, int main_find) {
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

// Memory Handling
// Option = 0 -> Called by add function(add memory)
// Option = 1 -> Called by rmv function(rmv memory)
entry **MemoryCheck(struct database *db, int option, int fluctuation) {
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

int add(struct database *db, long unsigned int uni_register, char name[64], short unsigned int fails, int fluctuation) {
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
    
    db->entries[db->students] = entry_ptr;
    db->students++;
    db->sorted = NO;
    printf("\nA-OK %lu, %d %d\n", uni_register, db->students, db->size);

    return 0;
}

int print(database *db) {
    int i;

    printf("\n#\n");
    for (i = 0; i < db->students; i++) {
        printf("%ld %s %hu\n", db->entries[i]->uni_register, db->entries[i]->name, db->entries[i]->fails);
    }
    return 1;
}

void clear(database **db) {
    int i;
    for (i = 0; i < (*db)->students; i++) {
        free((*db)->entries[i]);
    }
    
    (*db)->size = 0;
    (*db)->students = 0;
    (*db)->entries = NULL;
    (*db)->sorted= NO;
    
    }

// Insertion Sort
void sort(database *db) {
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

int mod(database *db, long unsigned int uni_register, short unsigned int fails) {
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
int rmv(database *db, long unsigned int uni_register, int fluctuation) {
    int pos, name_len = 0;
    entry **ptr;

    pos = find (db, uni_register, db->sorted, 0);
    if (pos == db->size) {
        printf("\nR-NOK %lu, %d %d\n", uni_register, db->students, db->size);
    }
    else {
        //db->entries[pos]->name[0] = '\0';
        db->entries[pos]->uni_register = db->entries[db->students - 1]->uni_register;
        //strcpy(db->entries[pos]->name, db->entries[db->students -1]->name);
        name_len = strlen(db->entries[db->students -1]->name);
        memmove(db->entries[pos]->name, db->entries[db->students -1]->name, name_len + 1);
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

int main(int argc, char *argv[]) {
    struct database *db = NULL;
    char option, name[64];
    long unsigned int uni_register;
    short unsigned int fails;
    int fluctuation, pos;
    double time;

    db = (database *)malloc(sizeof(database));
    fluctuation = atoi(argv[2]);
    db->entries = database_init(&db, atoi(argv[1]));

    do {
        scanf(" %c", &option);
        switch(option) {
            case 'a': {
                scanf(" %lu %s %hu", &uni_register, name, &fails);
                add(db, uni_register, name, fails, fluctuation);
                break;
            }
            case 'r': {
                scanf(" %lu", &uni_register);
                pos = rmv(db, uni_register, fluctuation);
                break;
            }
            case 'm': {
                scanf(" %lu %hu", &uni_register, &fails);
                pos = mod(db, uni_register, fails);
                if (pos == 0) {
                    printf("\nM-NOK %lu\n", uni_register);
                }
                else {
                    printf("\nM-OK %lu\n", uni_register);
                }
                break;
            }
            case 's': {
                clock_t start = clock();
                sort(db);
                clock_t end = clock();
                time = (double)(end - start) / CLOCKS_PER_SEC;
                fprintf(stderr, "#timeS#%f\n", time);
                break;
            }
            case 'f': {
                scanf(" %lu", &uni_register);
                clock_t start = clock();
                pos = find(db, uni_register, db->sorted, 1);
                clock_t end = clock();
                time = (double)(end - start) / CLOCKS_PER_SEC;
                fprintf(stderr, "#timeF#%f\n", time);
                if (pos < db->size) {
                    printf("\nF-OK %s %hu\n", db->entries[pos]->name, db->entries[pos]->fails);
                }
                else {
                    printf("\nF-NOK %lu\n", uni_register);
                }
                break;
            }
            case 'p': {
                pos = print(db);
                break;
            }
            case 'c': {
                clear(&db);
                printf("\nC-OK\n");
                break;
            }
            case 'q': {
                clear(&db);
                free(db);
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
