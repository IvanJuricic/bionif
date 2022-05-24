#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>

#define BUFF_LEN 255

// Structs
typedef struct {
    unsigned int key;
    char *value;
} HashTableItem;

typedef struct {
    HashTableItem **items;
    int size;
    int count;
} HashTable;

// Function declarations
char* append_string(char *s1, char *s2);
unsigned int create_fingerprint(unsigned char *s);

HashTableItem add_hash_item(unsigned int key, char *value);

int main() {

    FILE *fp;
    int entries = 0, seqHash;
    char buff[BUFF_LEN], *sequence, *tmp;
    bool firstEntry = true;

    fp = fopen("data", "r");

    while(fgets(buff, BUFF_LEN, (FILE *)fp) != NULL) {
        if(buff[0] == '>' && firstEntry == true) {
            firstEntry = false;
            continue;
        }
        else if(sequence != NULL && buff[0] == '>' && firstEntry == false) {
            // Add sequence to cuckoo filter
            seqHash = hash_func(sequence);
            printf("Sekvenca %d => len: %ld, hash: %u\n", entries, strlen(sequence), seqHash);
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
HashTableItem* create_hash_item(unsigned int key, char *value) {
    HashTableItem *item = (HashTableItem *) malloc(sizeof(HashTableItem));
    
    item -> key = key;
    item -> value = (char *) malloc(strlen(value) + 1);
    strcpy(item -> value, value);

    return item;
}

// Sequence fingerprint creation
unsigned int create_fingerprint(unsigned char *s) {
    unsigned int hash = 5381, c;

    while (c = *s++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
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