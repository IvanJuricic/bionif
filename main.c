#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>

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
void* search_for_item(HashTable* table, char* sequence);
void delete_item(HashTable* table, char* sequence);

int main() {

    FILE *fp;
    int entries = 0, seqHash, idx;
    char buff[BUFF_LEN], *sequence, *tmp, *test;
    bool firstEntry = true;

    char *entry = "TGAGCCGACGTCGCGGCCTCTCCGGCTGACGTGCTGGCTGCGCGTGCTGAGTCCGCAGCATCAGTCGCACAGGTTGCCGCCTCACGGGCAGATGTGCCGGCATCGCCGGCTGACTTCTTCGCGGCAGCCGTGTTCTGTGCCACCACGGACGCGTTACGCGCCACCTCTTCCACCATCAGTTCAAAACGACGCAGTGCCTCCGGACGGGC";

    HashTable *hashTable1, *hashTable2;
    hashTable1 = create_hash_table(200);
    hashTable2 = create_hash_table(200);

    fp = fopen("data", "r");

    while(fgets(buff, BUFF_LEN, (FILE *)fp) != NULL) {
        if(buff[0] == '>' && firstEntry == true) {
            firstEntry = false;
            continue;
        }
        else if(sequence != NULL && buff[0] == '>' && firstEntry == false) {
            // Add sequence to cuckoo filter
            //entry = sequence;
            insert_sequence_hash_to_table(hashTable1, hashTable2, sequence);
            //seqHash = create_fingerprint(sequence);
            //idx = hash1(seqHash);
            //printf("Sekvenca %d => len: %ld, hash: %u\n", entries, strlen(sequence), seqHash);
            //printf("Sekvenca %d => hash: %d\n", entries, idx);
            sequence = NULL;
            entries++;
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

    //print_table(hashTable);
    printf("Checking for entries....\n");
    search_for_item(hashTable1, entry);

    printf("Done!\n");
    
    return 0;

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
void* search_for_item(HashTable* table, char* sequence) {
    int seqHash = create_fingerprint(sequence);
    int idx = hash1(seqHash) % 400;

    HashTableItem* item = table -> items[idx];
 
    // Ensure that we move to a non NULL item
    if (item != NULL) {
        printf("Item value => %u, sequence value => %u\n", item->value, seqHash);
        /*if (item->value == seqHash) {
            printf("Sequence found!\n");
        }*/
    }
    
    printf("Sequence NOT found!\n");
}

// Delete item in hash table
void delete_item(HashTable* table, char* sequence) {
    // Deletes an item from the table
    int seqHash = create_fingerprint(sequence);
    int idx = hash1(seqHash) % 400;

    HashTableItem* item = table -> items[idx];
    
    if (item == NULL) {
        // Does not exist. Return
        return;
    }
    else {
        if (item -> key == idx) {
            // No collision chain. Remove the item
            // and set table index to NULL
            table -> items[idx] = NULL;
            free_item(item);
            table->count--;
            return;
        }
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
        printf("Dodajem u PRVU tablicu\n");
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
        printf("Dodajem u DRUGU tablicu\n");
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