#include "lib.h"

int num_of_collisions = 0, sequences = 0, hash_table_size = 0, unsuccessful_relocation = 0;

int get_num_of_collisions() {
    return num_of_collisions; 
}

int get_num_of_unsuccessful_relocations() {
    return unsuccessful_relocation; 
}

void run_checks(int file_type, int seq_len, HashTable* table, char** sequences) {
    
    if(file_type == 0) {
        for(int i = 0; i < 5; i++) printf("%d ", find_sequence(table, sequences[i]));
        printf("\n");

        delete_sequence(table, sequences[2]);
        delete_sequence(table, sequences[4]);

        for(int i = 0; i < 5; i++) printf("%d ", find_sequence(table, sequences[i]));
    } else if(file_type == 1) {
        char *str = malloc(seq_len*sizeof(char));
        int counter = 0;
        printf("User input *************************** =====>  %d\n", seq_len);
        printf("Looking for known sequences: \n");
        for(int i = 0; i < 5; i++) printf("%d ", find_sequence(table, sequences[i]));
        
        delete_sequence(table, sequences[2]);
        delete_sequence(table, sequences[4]);

        for(int i = 0; i < 5; i++) printf("%d ", find_sequence(table, sequences[i]));
        
        printf("Looking for random sequences....\n");
        
        for(int i = 0; i < 1000000; i++) {
            rand_string(str, seq_len);
            if(find_sequence(table, str)) {
                counter++;
            }
        }
        printf("Found %d. sequence!\n", counter);    
        
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
FileDescriptor *check_dna_file(char *filename) {
    int num_sequences = 0;
    char buff[BUFF_LEN];
    FileDescriptor *fd;

    FILE *fp;
    fp = fopen(filename, "r");
    
    while(fgets(buff, BUFF_LEN, fp) != NULL) {
        if(buff[0] == '>') num_sequences++;
    }

    // File with more short sequences
    if(num_sequences > 1) {
        fd -> file_entries = num_sequences;
        fd ->file_type = 0;
    }
    // File with one long sequence
    else {
        // Determine how large the user wants the k-mer
        printf("\tLong sequence detected\n");
        int k = get_user_input();
        fd -> user_input = k;
        num_sequences = 0;
        fd -> file_type = 1;
        fp = fopen(filename, "r");
        while(fgets(buff, k, fp) != NULL) {
            if(buff[0] == '>') continue;
            else if(buff != NULL) num_sequences++;
        }
        fd -> file_entries = num_sequences / 4;
    }

    printf("Check done!\t%d entries found!\n", num_sequences);
    
    if(fp != NULL) fclose(fp);
    
    return fd;
}

// Delete item
bool delete_sequence(HashTable *table, char *sequence) {
    unsigned int tmp  = get_int_from_sequence(sequence);
    
    // Get last two MSB for fingerprint
    unsigned short fingerprint = (tmp >> 16) & 0xffff;

    unsigned int idx1 = hash1(tmp) % table -> size;
    unsigned int idx2 = (idx1 ^ hash1(fingerprint)) % table -> size;

    if(table -> items[idx1] != NULL) {
        for(int i = 0; i < 4; i++) {
            if(table -> items[idx1] -> value[i] == fingerprint) {
                table -> items[idx1] -> value[i] = 0;
                printf("deleted seq %d\n", fingerprint);
                return true;
            }
        }
    }

    if(table -> items[idx2] != NULL) {
        for(int i = 0; i < 4; i++) {
            if(table -> items[idx2] -> value[i] == fingerprint) {
                table -> items[idx2] -> value[i] = 0;
                printf("deleted seq %d\n", fingerprint);
                return true;
            }
        }
    }

    return false;
}

// Search for item
bool find_sequence(HashTable *table, char *sequence) {
    unsigned int tmp  = get_int_from_sequence(sequence);
    
    // Get last two MSB for fingerprint
    unsigned short fingerprint = (tmp >> 16) & 0xffff;

    unsigned int idx1 = hash1(tmp) % table -> size;
    unsigned int idx2 = (idx1 ^ hash1(fingerprint)) % table -> size;

    if(table -> items[idx1] != NULL) {
        for(int i = 0; i < 4; i++) {
            if(table -> items[idx1] -> value[i] == fingerprint) return true;
        }
    }

    if(table -> items[idx2] != NULL) {
        for(int i = 0; i < 4; i++) {
            if(table -> items[idx2] -> value[i] == fingerprint) return true;
        }
    }

    return false;

}

// Add item to table
void insert_sequence_hash_to_table(HashTable* table, char* sequence) {
    unsigned int tmp  = get_int_from_sequence(sequence);
    
    // Get last two MSB for fingerprint
    unsigned short fingerprint = (tmp >> 16) & 0xffff;

    unsigned int idx1 = hash1(tmp) % table -> size;
    unsigned int idx2 = (idx1 ^ hash1(fingerprint)) % table -> size;

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
    
    //printf("Nema mjesta, relociranje....\n");
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

            table_index = (table_index ^ hash1(prev_fingerprint)) % table -> size;

            //printf("novi index => %d\n", table_index);
            num_of_collisions++;

            if(table -> items[table_index] != NULL) {
                for(int j = 0; j < 4; j++) {
                    if(table -> items[table_index] -> value[j] == 0) {
                        table -> items[table_index] -> value[j] = prev_fingerprint;
                        sequences++;
                        printf("Added seq %d!\n", sequences);
                        return;
                    }
                }
            }
        }

    }

    unsuccessful_relocation++;
   

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
            printf("Index:%d, Key:%u, Value:%d %d %d %d\n", i, table->items[i]->key, table->items[i]->value[0], table->items[i]->value[1], table->items[i]->value[2], table->items[i]->value[3]);
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
