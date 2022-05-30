#include "lib.h"

int num_of_collisions = 0, sequences = 0;

int get_num_of_collisions() {
    return num_of_collisions; 
}

void run_checks(int num_sequences, int seq_len, HashTable* table1, HashTable* table2, char** sequences) {
    
    if(num_sequences > 1) {
        //for(int i = 0; i < 5; i++) search_for_item(table1, table2, sequences[i]);

        //delete_item(table1, table2, sequences[2]);
        //delete_item(table1, table2, sequences[4]);

        //for(int i = 0; i < 5; i++) search_for_item(table1, table2, sequences[i]);
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

// Add item to table
void insert_sequence_hash_to_table(HashTable* table, char* sequence) {
    unsigned int tmp  = get_int_from_sequence(sequence);
    
    // Get last two MSB for fingerprint
    unsigned short fingerprint = (tmp >> 16) & 0xffff;

    unsigned int idx1 = hash1(tmp) % HASH_TABLE_SIZE;
    unsigned int idx2 = (idx1 ^ hash1(fingerprint)) % HASH_TABLE_SIZE;

    if( table -> items[idx1] != NULL) {
        HashTableItem* item = table -> items[idx1];
        
        // Check for duplicates or direct insert
        for(int i = 0; i < 4; i++) {
            if(item -> value[i] == fingerprint) {
                printf("Duplikat!!\n");
                num_of_collisions++;
                sequences++;
                return;
            } else if(item -> value[i] == 0) {
                item -> value[i] = fingerprint;
                sequences++;
                printf("Added seq %d!\n", sequences);
                return;
            }
        }

    } else {
        // Check if hash table is full
        // If not, create new entry
        if(table -> count == table -> size) {
            // Hash Table Full
            printf("Insert Error: Hash Table is full\n");
            //free_item(item);
            return;
        }
        
        HashTableItem* item = (HashTableItem *) malloc(sizeof(HashTableItem));
        item -> key = idx1;
        item -> value[0] = fingerprint;

        // Store item to hash table
        table -> items[idx1] = item;
        table -> count++;

        sequences++;
        printf("Added seq %d!\n", sequences);

        return;
    }

    if( table -> items[idx2] != NULL) {
        HashTableItem* item = table -> items[idx2];
        
        // Check for duplicates or direct insert
        for(int i = 0; i < 4; i++) {
            if(item -> value[i] == fingerprint) {
                printf("Duplikat!!\n");
                num_of_collisions++;
                sequences++;
                return;
            } else if(item -> value[i] == 0) {
                item -> value[i] = fingerprint;
                sequences++;
                printf("Added seq %d!\n", sequences);
                return;
            }
        }

    } else {
        // Check if hash table is full
        // If not, create new entry
        if(table -> count == table -> size) {
            // Hash Table Full
            printf("Insert Error: Hash Table is full\n");
            //free_item(item);
            return;
        }
        
        HashTableItem* item = (HashTableItem *) malloc(sizeof(HashTableItem));
        item -> key = idx2;
        item -> value[0] = fingerprint;

        // Store item to hash table
        table -> items[idx2] = item;
        table -> count++;

        sequences++;
        printf("Added seq %d!\n", sequences);

        return;
    }
    
    printf("Nema mjesta, relociranje....\n");
    //sequences--;

    // Get random index
    unsigned int table_index;
    int r = rand() % 2;
    if (r == 0) table_index = idx1;
    else table_index = idx2;
    
    for(int i = 0; i < 400; i++) {
        int index = rand() % 3;
        if(table -> items[table_index] != NULL) {
            HashTableItem *item = table -> items[table_index];
            unsigned short prev_fingerprint = item -> value[index];
            item -> value[index] = fingerprint;

            table_index ^= hash1(prev_fingerprint) % HASH_TABLE_SIZE;

            printf("novi index => %d\n", table_index);

            if(table -> items[table_index] != NULL) {
                for(int j = 0; j < 4; j++) {
                    if(table -> items[table_index] -> value[j] == 0) {
                        table -> items[table_index] -> value[j] = prev_fingerprint;
                        printf("Relocirano!\n");
                        return;
                    }
                }
            }
        }

    }

    printf("Kurcina\n");
   

    return;
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
            printf("Index:%d, Key:%u, Value:%lx\n", i, table->items[i]->key, table->items[i]->value);
        }
    }
    printf("-------------------\n\n");
}

// Sequence fingerprint creation
unsigned int get_int_from_sequence(unsigned char *s) {
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
