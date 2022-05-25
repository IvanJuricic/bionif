#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define BUFF_LEN 255
#define HASH_TABLE_SIZE 150

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
unsigned int KR_v2_hash(char *s);
HashTableItem* create_hash_item(unsigned int key, unsigned char value);
HashTable* create_hash_table(int size);
void insert_sequence_hash_to_table(HashTable* table1, HashTable* table2, char* sequence);
void print_table(HashTable* table);
void free_item(HashTableItem* item);
void free_table(HashTable* table);
void search_for_item(HashTable* table1, HashTable* table2, char* sequence);
void delete_item(HashTable* table1, HashTable* table2, char* sequence);
int check_dna_file(char *filename);
int get_user_input();
static char *rand_string(char *str, size_t size);
void run_checks(int num_sequences, int seq_len, HashTable* table1, HashTable* table2, char** sequences);
void get_bytes(unsigned int val);
int set_byte(unsigned int val, unsigned char byte);

int num_of_collisions = 0;

int main(int argc, char *argv[]) {
/*
    if(argc != 2) {
        printf("\n\tUsage: ./a.out filename\n");
        exit(-1);
    }

    char *filename = argv[argc-1];*/
    char *filename = "data";
    srand(time(NULL));
    
    FILE *fp;
    int counter = 0, seqHash, idx, num_entries, k;
    char buff[BUFF_LEN], *sequence, *tmp, *test;
    bool firstEntry = true;

    char *sequences[5];

    HashTable *hashTable1, *hashTable2;
    hashTable1 = create_hash_table(HASH_TABLE_SIZE);
    hashTable2 = create_hash_table(HASH_TABLE_SIZE);

    num_entries = check_dna_file(filename);
    fp = fopen(filename, "r");
    
    if(num_entries > 1) {
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
                //printf("Added sequence num: %d, seq len: %ld\n", counter, strlen(sequence));
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
        
        while(fgets(buff, k, (FILE *)fp) != NULL) {
            if(buff[0] == '>' && firstEntry == true) {
                firstEntry = false;
                continue;
            }
            else if(sequence != NULL && firstEntry == false && strlen(sequence) >= k) {
                // Remember first 5 sequences for searching
                if(counter < 5) {
                    sequences[counter] = malloc(strlen(sequence) + 1);
                    strcpy(sequences[counter], sequence);
                }
                // Add sequence to cuckoo filter
                insert_sequence_hash_to_table(hashTable1, hashTable2, sequence);
                //printf("Sequence num %d => %s => len: %ld\n", counter, sequence, strlen(sequence));
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
    }

    printf("\tAdded %d sequences!\n", counter);
    printf("Num of collisions %d\n", num_of_collisions);

    run_checks(num_entries, k, hashTable1, hashTable2, sequences);

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

void run_checks(int num_sequences, int seq_len, HashTable* table1, HashTable* table2, char** sequences) {
    
    if(num_sequences > 1) {
        for(int i = 0; i < 5; i++) search_for_item(table1, table2, sequences[i]);

        delete_item(table1, table2, sequences[2]);
        delete_item(table1, table2, sequences[4]);

        for(int i = 0; i < 5; i++) search_for_item(table1, table2, sequences[i]);
    } else if(num_sequences == 1) {
        char *str = malloc(seq_len*sizeof(char));
        rand_string(str, seq_len);
        printf("Generated sequence %s\n", str);
        free(str);
        str = NULL;
    }
    
}

// Get k-mer length
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

    printf("Check done!\t%d entries found!\n", num_sequences);
    
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
HashTableItem* create_hash_item(unsigned int key, unsigned char value) {
    
    unsigned int tmp, tmp2;
    HashTableItem *item = (HashTableItem *) malloc(sizeof(HashTableItem));
    
    tmp = item -> value;

    printf("\nBytes ===> \n");
    get_bytes(tmp);
    tmp2 = set_byte(tmp, value);
    get_bytes(tmp2);
    printf("Return %d\n", tmp2);
    
    item -> key = key;
    item -> value = value; 
    
    return item;
}

int set_byte(unsigned int val, unsigned char byte) {
    char tmp;
    int x;
    for(int i = 0; i < 4; i++) {
        if(i = 0) tmp = (val >> 24);
        else {
            tmp = (val >> 8*(3-i)) & 0xff;
        }
        if(tmp == 0x00) {
            x = ((val >> 8*(3-i)) & 0xff) | byte;
            return x;
        }   
    }
    return -1;
}

void get_bytes(unsigned int val) {
    char *bytes;
    bytes = malloc(4*sizeof(char));

    bytes[0] = (val >> 24);
    bytes[1] = (val >> 16) & 0xff;
    bytes[2] = (val >> 8) & 0xff;
    bytes[3] = val & 0xff;

    for(int i = 0; i < 4; i++) {
        printf("%02x ", bytes[i]);
    }
    printf("\n");

    return;
}

// Search for item in hash table
void search_for_item(HashTable* table1, HashTable* table2, char* sequence) {
    if(sequence == NULL) {
        printf("Sequence invalid!\n");
        return;
    }
    int tmp = create_fingerprint(sequence);
    // Get MSB
    unsigned char seqHash = (tmp >> 24) & 0xff;
    int idx1 = KR_v2_hash(sequence) % HASH_TABLE_SIZE;
    int idx2 = (idx1 ^ hash1(seqHash)) % HASH_TABLE_SIZE;

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
    int tmp = create_fingerprint(sequence);
    // Get MSB
    unsigned char seqHash = (tmp >> 24) & 0xff;
    int idx1 = KR_v2_hash(sequence) % HASH_TABLE_SIZE;
    int idx2 = (idx1 ^ hash1(seqHash)) % HASH_TABLE_SIZE;

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
    int tmp = create_fingerprint(sequence);
    // Get MSB
    unsigned char seqHash = (tmp >> 24) & 0xff;
    //printf("Sizeof int %ld\nSizeof hash %d\n", sizeof(int), seqHash);
    //printf("seq hash to be stored => hex:%x, dec: %d\n", seqHash, seqHash);
    int idx1 = KR_v2_hash(sequence) % HASH_TABLE_SIZE;
    int idx2 = (idx1 ^ hash1(seqHash)) % HASH_TABLE_SIZE;

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
    } else {
        num_of_collisions++;
    }
}

// Random sequence generator
static char *rand_string(char *str, size_t size)
{
    const char charset[] = "ATCG";
    if (size) {
        //--size;
        for (size_t n = 0; n <= size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
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

// Hash function for strings
unsigned int KR_v2_hash(char *s)
{
    // Source: https://stackoverflow.com/a/45641002/5407270
    //printf("Sequence %s\n", s);
    unsigned int hashval = 0;
    for (hashval = 0; *s != '\0'; s++)
        hashval = *s + 31*hashval;
    return hashval;
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