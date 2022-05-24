#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define BUFF_LEN 255

// Structs
typedef struct {
    unsigned int key, value;
} HashTableItem;

typedef struct {
    HashTableItem **items;
    int size;
    int count;
} HashTable;

// Function declarations
char* append_string(char *s1, char *s2);
unsigned int create_fingerprint(unsigned char *s);
unsigned int hash1(unsigned int x);
HashTableItem* create_hash_item(unsigned int key, unsigned int value);
HashTable* create_hash_table(int size);
void insert_sequence_hash_to_table(HashTable* table1, HashTable* table2, char* sequence);
void print_table(HashTable* table);
void free_item(HashTableItem* item);
void free_table(HashTable* table);
void search_for_item(HashTable* table1, HashTable* table2, char* sequence);
void delete_item(HashTable* table1, HashTable* table2, char* sequence);
int check_dna_file(char *filename);
int get_user_input();

int main(int argc, char *argv[]) {

    if(argc != 2) {
        printf("\n\tUsage: ./a.out filename\n");
        exit(-1);
    }

    char *filename = argv[argc-1];
    
    FILE *fp;
    int counter = 0, seqHash, idx, num_entries, k;
    char buff[BUFF_LEN], *sequence, *tmp, *test;
    bool firstEntry = true;

    char *sequences[5];

    HashTable *hashTable1, *hashTable2;
    hashTable1 = create_hash_table(200);
    hashTable2 = create_hash_table(200);

    num_entries = check_dna_file(filename);
    fp = fopen(filename, "r");
    
    if(num_entries > 1) {
        printf("\t%d entries found!\n", num_entries);
        while(fgets(buff, BUFF_LEN, (FILE *)fp) != NULL) {
            if(buff[0] == '>' && firstEntry == true) {
                firstEntry = false;
                continue;
            }
            else if(sequence != NULL && buff[0] == '>' && firstEntry == false) {
                // Remember first 5 sequences for searching
                if(counter < 5) {
                    sequences[counter] = malloc(strlen(sequence) + 1);
                    strcpy(sequences[counter], sequence);
                }
                // Add sequence to cuckoo filter
                insert_sequence_hash_to_table(hashTable1, hashTable2, sequence);
                printf("Added sequence num: %d, seq len: %ld\n", counter, strlen(sequence));
                free(sequence);
                sequence = NULL;
                counter++;
                continue;
            }else if(sequence == NULL) {
                sequence = (char *)malloc(strlen(buff));
                strcpy(sequence, buff);
                continue;
            } else {
                tmp = append_string(sequence, buff);
                sequence = tmp;
            }
        }
    } else if (num_entries == 1) {
        // Determine how large the user wants the k-mer
        printf("\tLong sequence detected\n");
        k = get_user_input();
        /*
        while(fgets(buff, k, (FILE *)fp) != NULL) {
            if(buff[0] == '>' && firstEntry == true) {
                firstEntry = false;
                continue;
            }
            else if(sequence != NULL && buff[0] == '>' && firstEntry == false) {
                // Remember first 5 sequences for searching
                if(counter < 5) {
                    sequences[counter] = malloc(strlen(sequence) + 1);
                    strcpy(sequences[counter], sequence);
                }
                // Add sequence to cuckoo filter
                insert_sequence_hash_to_table(hashTable1, hashTable2, sequence);
                printf("Sequence %d => len: %ld\n", counter, strlen(sequence));
                free(sequence);
                sequence = NULL;
                counter++;
                continue;
            }else if(sequence == NULL) {
                sequence = (char *)malloc(strlen(buff));
                strcpy(sequence, buff);
                continue;
            } else {
                tmp = append_string(sequence, buff);
                sequence = tmp;
            }
        }*/
    }

    for(int i = 0; i < 5; i++) {
        //printf("\n%s\n", sequences[i]);
        search_for_item(hashTable1, hashTable2, sequences[i]);
    }
    delete_item(hashTable1, hashTable2, sequences[2]);
    delete_item(hashTable1, hashTable2, sequences[4]);
    for(int i = 0; i < 5; i++) {
        //printf("\n%s\n", sequences[i]);
        search_for_item(hashTable1, hashTable2, sequences[i]);
    }
    //print_table(hashTable);
    //printf("Checking for entries....\n");
    //search_for_item(hashTable1, hashTable2, entry);
    //delete_item(hashTable1, hashTable2, entry);
    //search_for_item(hashTable1, hashTable2, entry);
    if(fp != NULL) fclose(fp);
    for(int i = 0; i < 5; i++) {
        if(sequences[i] != NULL) {
            free(sequences[i]);
            sequences[i] = NULL;
        }
    }

    printf("Done!\n");
    
    return 0;

}

int get_user_input() {
    int tmp;
    
    printf("Choose the length of k-mer\n");
    printf("\tPress 1 for k = 10\n");
    printf("\tPress 2 for k = 20\n");
    printf("\tPress 3 for k = 50\n");
    printf("\tPress 4 for k = 100\n");
    printf("\tPress 5 for k = 1000\n");

    tmp = getchar();

    switch (tmp)
    {
    case '1':
        return 10;
        break;

    case '2':
        return 20;
        break;

    case '3':
        return 50;
        break;

    case '4':
        return 100;
        break;

    case '5':
        return 1000;
        break;
    
    default:
        printf("Wrong input!\nExiting!\n");
        exit(-1);
    }

}

// Check if there is one long sequence (returns 1) or multiple (returns num of entries)
int check_dna_file(char *filename) {
    int num_sequences = 0;
    char buff[BUFF_LEN];

    FILE *fp;
    fp = fopen(filename, "r");
    
    while(fgets(buff, BUFF_LEN, fp) != NULL) {
        if(buff[0] == '>') num_sequences++;
    }

    printf("Check done!\n");

    return num_sequences;
}

// Create hash table and init everything to NULL
HashTable* create_hash_table(int size) {
    HashTable* table = (HashTable*) malloc(sizeof(HashTable));
    
    table -> size = size;
    table -> count = 0;
    table -> items = (HashTableItem**) calloc(table->size, sizeof(HashTableItem*));
    
    for (int i=0; i< table->size; i++)
        table -> items[i] = NULL;
 
    return table;
}

// Create hash item from hashed sequence 
HashTableItem* create_hash_item(unsigned int key, unsigned int value) {
    HashTableItem *item = (HashTableItem *) malloc(sizeof(HashTableItem));
    
    item -> key = key;
    item -> value = value; 
    
    return item;
}

// Search for item in hash table
void search_for_item(HashTable* table1, HashTable* table2, char* sequence) {
    if(sequence == NULL) {
        printf("Sequence invalid!\n");
        return;
    }
    int seqHash = create_fingerprint(sequence);
    int idx1 = hash1(seqHash) % 200;
    int idx2 = (idx1 ^ hash1(seqHash)) % 200;

    // Check if element exists in first table
    if(table1 -> items[idx1] != NULL) {
        if(table1 -> items[idx1] -> value == seqHash){
            printf("Sequence found!\n");
            return;
        }
    } else if(table2 -> items[idx2] != NULL) {
        if(table2 -> items[idx2] -> value == seqHash) {
            printf("Sequence found!\n");
            return;
        }
    }
    printf("Sequence NOT found!\n");
    return;
}

// Delete item in hash table
void delete_item(HashTable* table1, HashTable* table2, char* sequence) {
    if(sequence == NULL) {
        printf("Sequence invalid!\n");
        return;
    }
    int seqHash = create_fingerprint(sequence);
    int idx1 = hash1(seqHash) % 200;
    int idx2 = (idx1 ^ hash1(seqHash)) % 200;

    if(table1 -> items[idx1] -> value == seqHash){
        table1 -> items[idx1] = NULL;
        free(table1 -> items[idx1]);
        table1 -> count--;
        printf("Deleted item from hash table 1 at index %d\n", idx1);
        return;
    } else if(table2 -> items[idx2] -> value == seqHash) {
        table2 -> items[idx2] = NULL;
        free(table2 -> items[idx2]);
        table2 -> count--;
        printf("Deleted item from hash table 1 at index %d\n", idx2);
        return;
    } else {
        printf("No such entry stored!\n");
        return;
    }
}

// Add item to table
void insert_sequence_hash_to_table(HashTable* table1, HashTable* table2, char* sequence) {
    int seqHash = create_fingerprint(sequence);
    int idx1 = hash1(seqHash) % 200;
    int idx2 = (idx1 ^ hash1(seqHash)) % 200;

    if (table1 -> items[idx1] == NULL) {
        // Key does not exist.
        if (table1 -> count == table1 -> size) {
            // Hash Table Full
            printf("Insert Error: Hash Table is full\n");
            //free_item(item);
            return;
        }
        HashTableItem* item = create_hash_item(idx1, seqHash);
        // Insert directly
        table1 -> items[idx1] = item; 
        table1 -> count++;
    } else if(table2 -> items[idx2] == NULL){
        if(table2 -> count == table2 -> size) {
            // Hash Table Full
            printf("Insert Error: Hash Table is full\n");
            //free_item(item);
            return;
        }
        HashTableItem* item = create_hash_item(idx2, seqHash);
        // Insert directly
        table2 -> items[idx2] = item; 
        table2 -> count++;
    } 
}

// Free hash table item
void free_item(HashTableItem* item) {
    free(item);
}

// Free hash table
void free_table(HashTable* table) {
    for (int i = 0; i < table -> size; i++) {
        HashTableItem* item = table -> items[i];
        if (item != NULL)
            free_item(item);
    }
 
    free(table->items);
    free(table);
}

// Print table
void print_table(HashTable* table) {
    printf("\nHash Table\n-------------------\n");
    for (int i=0; i<table->size; i++) {
        if (table->items[i]) {
            printf("Index:%d, Key:%u, Value:%u\n", i, table->items[i]->key, table->items[i]->value);
        }
    }
    printf("-------------------\n\n");
}

// Sequence fingerprint creation
unsigned int create_fingerprint(unsigned char *s) {
    unsigned int hash = 5381, c;

    while (c = *s++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

// Hash table 1 function
unsigned int hash1(unsigned int x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

// Append DNA sequences if longer than buffer limit
char* append_string(char *s1, char *s2) {
    int len_s1 = strlen(s1), len_s2 = strlen(s2);
    int s_len = len_s1 + len_s2 + 1;
    
    char *tmp = calloc(s_len, sizeof(char));
    
    for(int i = 0; i < len_s1; i++) tmp[i] = s1[i];
    for(int i = 0; i < len_s2; i++) tmp[len_s1 + i] = s2[i];

    tmp[s_len - 1] = '\0';

    return tmp;

}