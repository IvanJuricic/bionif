#include "lib.h"

int main(int argc, char *argv[]) {

    if(argc != 2) {
        printf("\n\tUsage: ./test.o filename\n");
        exit(-1);
    }

    char *filename = argv[argc-1];
    //char *filename = "data";
    srand(time(NULL));
    
    FILE *fp;
    int counter = 0, idx, num_entries, k, num_of_items_to_be_stored, user_input;
    char buff[BUFF_LEN], *sequence, *tmp, *test;
    bool firstEntry = true;

    char *sequences[5];

    FileDescriptor *fd = (FileDescriptor *)malloc(sizeof(FileDescriptor));
    fd = check_dna_file(filename);
    
    fp = fopen(filename, "r");

    num_of_items_to_be_stored = fd -> file_entries;
    user_input = fd -> user_input;
    
    printf("User input %d\n", fd -> user_input);

    HashTable *hashTable;
    hashTable = create_hash_table(num_of_items_to_be_stored);
    //hashTable2 = create_hash_table(HASH_TABLE_SIZE);
    
    if(fd -> file_type == 0) {
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
                insert_sequence_hash_to_table(hashTable, sequence);
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
    } else if (fd -> file_type == 1) {
        while(fgets(buff, BUFF_LEN, (FILE *)fp) != NULL) {
            if(buff[0] == '>' && firstEntry == true) {
                firstEntry = false;
                continue;
            }
            else if(sequence != NULL && firstEntry == false && strlen(sequence) >= user_input) {
                // Remember first 5 sequences for searching
                if(counter < 5) {
                    sequences[counter] = malloc(strlen(sequence) + 1);
                    strcpy(sequences[counter], sequence);
                }
                // Add sequence to cuckoo filter
                insert_sequence_hash_to_table(hashTable, sequence);
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

    //printf("\tAdded %d sequences!\n", counter);
    printf("Num of collisions %d\n", get_num_of_collisions());
    printf("Num of unsucessfull relocations: %d\n", get_num_of_unsuccessful_relocations());
    int free_spaces = 0;
    for(int i = 0; i < HASH_TABLE_SIZE; i++) {
        for(int j = 0; j < 4; j++) {
            if(hashTable -> items[i] != NULL) {
                if(hashTable -> items[i] -> value[j] == 0) free_spaces++;
            }
        }
    }

    printf("Free spaces %d\n", free_spaces);

    run_checks(fd -> file_type, fd -> user_input, hashTable, sequences);

    printf("\tTABLICA 1\n");
    //print_table(hashTable);

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
