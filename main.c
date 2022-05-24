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
void insert_sequence_hash_to_table(HashTable* table, char* sequence);

void free_item(HashTableItem* item);
void free_table(HashTable* table);


int main() {

    FILE *fp;
    int entries = 0, seqHash, idx;
    char buff[BUFF_LEN], *sequence, *tmp;
    bool firstEntry = true;

    HashTable *hashTable;
    hashTable = create_hash_table(200);

    fp = fopen("data", "r");

    while(fgets(buff, BUFF_LEN, (FILE *)fp) != NULL) {
        if(buff[0] == '>' && firstEntry == true) {
            firstEntry = false;
            continue;
        }
        else if(sequence != NULL && buff[0] == '>' && firstEntry == false) {
            // Add sequence to cuckoo filter
            insert_sequence_hash_to_table(hashTable, sequence);
            //seqHash = create_fingerprint(sequence);
            //idx = hash1(seqHash);
            //printf("Sekvenca %d => len: %ld, hash: %u\n", entries, strlen(sequence), seqHash);
            printf("Sekvenca %d => hash: %d\n", entries, idx);
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

void* search_for_item(HashTable* table, char* sequence) {
    int seqHash = create_fingerprint(sequence);
    int idx = hash1(seqHash);

    HashTableItem* item = table->items[idx];
 
    // Ensure that we move to a non NULL item
    if (item != NULL) {
        if (item->value == seqHash)
            printf("Sequence found!\n");
    }
    printf("Sequence NOT found!\n");
}

// Add item to table
void insert_sequence_hash_to_table(HashTable* table, char* sequence) {
    // Create the item
    int seqHash = create_fingerprint(sequence);
    int idx = hash1(seqHash);

    HashTableItem* item = create_item(idx, seqHash);
    HashTableItem* current_item = table->items[idx];
    
    if (current_item == NULL) {
        // Key does not exist.
        if (table -> count == table -> size) {
            // Hash Table Full
            printf("Insert Error: Hash Table is full\n");
            free_item(item);
            return;
        }
        
        // Insert directly
        table->items[idx] = item; 
        table->count++;
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

// Sequence fingerprint creation
unsigned int create_fingerprint(unsigned char *s) {
    unsigned int hash = 5381, c;

    while (c = *s++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

// Hash table function
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