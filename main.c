#include "lib.h"

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
    printf("Num of collisions %d\n", get_num_of_collisions());

    run_checks(num_entries, k, hashTable1, hashTable2, sequences);

    printf("\tTABLICA 1\n");
    print_table(hashTable1);
    printf("\tTABLICA 2\n");
    print_table(hashTable2);

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