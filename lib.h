#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define BUFF_LEN 255
#define HASH_TABLE_SIZE 100

// Structs
typedef struct {
    unsigned int key;
    // Store 4 2-byte values from DNA hash fingerprints
    unsigned short value[4];
} HashTableItem;

typedef struct {
    HashTableItem **items;
    int size;
    int count;
} HashTable;

// Function declarations
char* append_string(char *s1, char *s2);
unsigned int get_int_from_sequence(unsigned char *s);
unsigned int hash1(unsigned int x);
unsigned int KR_v2_hash(char *s);
HashTableItem* create_hash_item(unsigned int key, unsigned char value);
HashTable* create_hash_table(int size);
void insert_sequence_hash_to_table(HashTable* table, char* sequence);
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
int set_byte(unsigned int *val, unsigned char byte);
int get_num_of_collisions();
bool check_for_duplicates(HashTable *hashTable, unsigned int key, unsigned char byteUpper, unsigned char byteLower);
int check_hash_table(HashTable *hashTable, int key);

// Long int
int get_free_memory_index(unsigned long int value);
void get_bytes_long_int(unsigned long int val);
unsigned long int set_byte_long_int(unsigned long int value, unsigned char byteUpper, unsigned char byteLower, int position);
HashTableItem* create_hash_item_long_int(HashTable *hashTable, unsigned int key);
