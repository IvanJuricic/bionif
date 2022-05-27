#include "lib.h"

int num_of_collisions = 0;

int get_num_of_collisions() {
    return num_of_collisions; 
}

void run_checks(int num_sequences, int seq_len, HashTable* table1, HashTable* table2, char** sequences) {
    
    if(num_sequences > 1) {
        for(int i = 0; i < 5; i++) search_for_item(table1, table2, sequences[i]);

        //delete_item(table1, table2, sequences[2]);
        //delete_item(table1, table2, sequences[4]);

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
    
    unsigned int tmp;
    int ret_value;
    HashTableItem *item = (HashTableItem *) malloc(sizeof(HashTableItem));
    
    tmp = item -> value;

    printf("%02x\n", value);
    get_bytes(tmp);
    ret_value = set_byte(&tmp, value);
    get_bytes(tmp);
    
    item -> key = key;
    item -> value = value; 
    
    return item;
}

int set_byte(unsigned int *val, unsigned char byte) {
    for(int i = 0; i < 4; i++) {
	char tmp = (*val >> 8*(3-i)) & 0xff;
        if(tmp == 0x00) {
            unsigned int byteInt = byte;
            byteInt <<= 8*(3-i);
            *val = *val | byteInt;
            return 1;
        }   
    }
    return -1;
}

void get_bytes(unsigned int val) {
    char bytes[4];
    
    bytes[0] = (val >> 24);
    bytes[1] = (val >> 16) & 0xff;
    bytes[2] = (val >> 8) & 0xff;
    bytes[3] = val & 0xff;

    for(int i = 0; i < 4; i++) {
        printf("%x ", bytes[i] & 0xff);
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
    // Get MSB for solution using int
    unsigned char seqHash = (tmp >> 24) & 0xff;
    // Get last two MSB for sol using long int
    unsigned char upperByte, lowerByte;
    upperByte = (tmp >> 24) & 0xff;
    lowerByte = (tmp >> 16) & 0xff;

    //printf("Sizeof int %ld\nSizeof hash %d\n", sizeof(int), seqHash);
    //printf("seq hash to be stored => hex:%x, dec: %d\n", seqHash, seqHash);
    int idx1 = KR_v2_hash(sequence) % HASH_TABLE_SIZE;
    int idx2 = (idx1 ^ hash1(seqHash)) % HASH_TABLE_SIZE;

    unsigned long int prev_val, after_val;

    int ret = check_hash_table(table1, idx1);

    // Check hash table one for available space
    if(ret == -1) {
        HashTableItem* item = create_hash_item_long_int(table1, idx1);
        
        table1 -> items[idx1] = item;
        table1 -> count++;
        
        prev_val = table1 -> items[idx1] -> value;

        after_val = set_byte_long_int(prev_val, upperByte, lowerByte, 0);
        
        if(after_val == -1) printf("Error inserting!\n");

        table1 -> items[idx1] -> value = after_val;

        printf(ANSI_COLOR_GREEN "Seq added at idx %d: %lx!\n", idx1, after_val);
        //printf(ANSI_COLOR_RESET);

        return;

    } else if (ret > -1) {

        prev_val = table1 -> items[idx1] -> value;

        if(check_for_duplicates(table1, idx1, upperByte, lowerByte)) return;

        after_val = set_byte_long_int(prev_val, upperByte, lowerByte, ret);
        
        if(after_val == -1) printf("Error inserting!\n");

        printf(ANSI_COLOR_GREEN "Seq added at idx %d: %lx!\n", idx1, after_val);

        //printf("Updateano!\n");
        table1 -> items[idx1] -> value = after_val;

        return;
    } else if (ret == -2) {
        printf("Table 1 full!\n");
    }
    
    ret = check_hash_table(table2, idx2);

    // Check hash table two for available space
    if(ret == -1) {
        HashTableItem* item = create_hash_item_long_int(table2, idx2);
        
        table2 -> items[idx2] = item;
        table2 -> count++;
        
        prev_val = table2 -> items[idx2] -> value;

        after_val = set_byte_long_int(prev_val, upperByte, lowerByte, 0);
        
        if(after_val == -1) printf("Error inserting!\n");

        table2 -> items[idx2] -> value = after_val;

        printf(ANSI_COLOR_CYAN "Seq added at idx %d: %lx!\n", idx2, after_val);

        //printf("Dodano tablica 2!\n");
        return;

    } else if (ret > -1){

        prev_val = table2 -> items[idx2] -> value;

        if(check_for_duplicates(table2, idx2, upperByte, lowerByte)) return;

        after_val = set_byte_long_int(prev_val, upperByte, lowerByte, ret);
        
        if(after_val == -1) printf("Error inserting!\n");

        table2 -> items[idx2] -> value = after_val;

        printf(ANSI_COLOR_CYAN "Seq added at idx %d: %lx!\n", idx2, after_val);

        return;
    } else if(ret == -2) {
        printf("Table 2 full!\n");
    }
    
    /*
    if (table1 -> items[idx1] == NULL) {
        // Key does not exist.
        if (table1 -> count == table1 -> size) {
            // Hash Table Full
            printf("Insert Error: Hash Table is full\n");
            //free_item(item);
            return;
        }
        HashTableItem* item = create_hash_item_long_int(idx1, upperByte, lowerByte);
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
        HashTableItem* item = create_hash_item_long_int(idx2, upperByte, lowerByte);
        // Insert directly
        table2 -> items[idx2] = item; 
        table2 -> count++;
    } else {
        //unsigned long int val = table1 -> items[idx1] -> value;
        //int index = get_free_memory_index(val);
        //printf("Index => %d\n", index);
        //if()
        num_of_collisions++;
    }*/
}


bool check_for_duplicates(HashTable *hashTable, unsigned int key, unsigned char byteUpper, unsigned char byteLower) {
    unsigned long int val = hashTable -> items[key] -> value;

    unsigned short bytes = 0;

    bytes >>= 8;
    bytes |= byteUpper;
    bytes <<= 8;
    bytes |= byteLower;
    
    for(int i = 0; i < 4; i++) {
        unsigned short tmp = (val >> 16*(3-i)) & 0xffff;
        //printf("tmp: %02x, byte: %02x\n", tmp, bytes);
        if(tmp == bytes) {
            printf("Possible duplicate!\n");
            return true;
        }
    }
    return false;
}

// Check for space in hash table
int check_hash_table(HashTable *hashTable, int key) {
    unsigned long int tmp;
    int index;

    if(hashTable -> count == hashTable -> size) {
        // Hash Table Full
        printf("Insert Error: Hash Table is full\n");
        //free_item(item);
        return -2;
    }
    
    if( hashTable -> items[key] != NULL) {
        tmp = hashTable -> items[key] -> value;
        index = get_free_memory_index(tmp);
        return index;

    } else return -1;
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

// LONG INT IMPL
// Create hash item from hashed sequence 
HashTableItem* create_hash_item_long_int(HashTable *hashTable, unsigned int key) {
    
    unsigned long int *value;
    unsigned long int ret_value;

    HashTableItem *item;
    
    int ret = check_hash_table(hashTable, key);
    if(ret == -1) {
        item = (HashTableItem *) malloc(sizeof(HashTableItem));
        item -> key = key;
        item -> value = 0;
    }
    //printf("Upper byte: %02x\nLower byte: %02x\n", byteUpper, byteLower);
    //get_bytes_long_int(value);
    
    //ret_value = set_byte_long_int(value, byteUpper, byteLower);
    /*
    if(ret_value == 1) {
        item -> value = *value;
    } else {
        
        printf("Error setting bytes!\n");
        //item -> value = *value;
    }*/
     
    
    return item;
}

unsigned long int set_byte_long_int(unsigned long int value, unsigned char byteUpper, unsigned char byteLower, int position) {
    unsigned long int val, helper;
    unsigned short blank = 0x0000;

    val = value;

    /*for(int i = 0; i < 4; i++) {
        unsigned short tmp = (val >> 16*(3-i)) & 0xffff;
        //printf("%d. two bytes: %x\n", i, tmp);
        if(tmp == blank) index = i;
    }*/

    helper = val;
    helper >>= 16 * (3 - position);
    helper >>= 8;
    helper |= byteUpper;
    helper <<= 8;
    helper |= byteLower;
    helper <<= 16 * (3 - position);
    val = helper;
    
    return val;


    /*
    for(int i = 0; i < 4; i++) {
	    unsigned long int tmp = (*val >> 16*(3-i)) & 0xffff;
        if(tmp == 0x00) {
            unsigned long int byteInt = byteUpper;
            byteInt <<= 8;
            byteInt |= byteLower;
            byteInt <<= 16*(3-i);
            *val = *val | byteInt;
            return 1;
        }   
    }
    return -1;*/
}

void get_bytes_long_int(unsigned long int val) {
    char bytes[8];
    
    for(int i = 0; i < 8; i++) bytes[i] = (val >> 8*(7-i)) & 0xff;
	
    for(int i = 0; i < 8; i++) {
        printf("%x ", bytes[i] & 0xff);
    }
    printf("\n");

    return;
}

// LONG INT IMPL
int get_free_memory_index(unsigned long int value) {
    //get_bytes_long_int(value);
    unsigned short bytes;
    for(int i = 0; i < 4; i++) {
        unsigned short tmp = (value >> 16*(3-i)) & 0xffff;
        if(tmp == 0x0000) return i;
    }
    return -1;
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
