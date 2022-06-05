#include "lib.h"

int main(int argc, char *argv[]) {

    if(argc != 3) {
        printf("\n\tUsage: ./test.o filename sequence_length\n");
        exit(-1);
    }

    //int seq_lengths = [10, 20, 50, 100, 1000, 10000];

    char *filename = argv[argc-2];
    int seq_len = atoi(argv[argc-1]);
    //char *filename = "data";
    srand(time(NULL));

    FILE *fp;
    int counter = 0, idx, num_entries, k, num_of_items_to_be_stored, user_input;
    char buff[BUFF_LEN], *sequence, *tmp, *test;
    bool firstEntry = true;

    char *sequences[5];
    
    double time_spent = 0.0;
           
    FileDescriptor *fd;
    fd = check_dna_file(filename, seq_len);
    
    fp = fopen(filename, "r");

    num_of_items_to_be_stored = fd -> file_entries;
    user_input = fd -> user_input;
    
    HashTable *hashTable;
    hashTable = create_hash_table(num_of_items_to_be_stored);
    
    time_spent = 0;
    clock_t begin = clock();

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
                time_spent = 0;
                clock_t begin = clock();
                insert_sequence_hash_to_table(hashTable, sequence);
                clock_t end = clock();
                time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
 
                printf("The elapsed time for INSERT is %f seconds\n", time_spent);
                
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
        while(fgets(buff, user_input + 1, (FILE *)fp) != NULL) {
            if(sequence != NULL && strlen(sequence) == user_input) {
                // Add sequence to cuckoo filter
                insert_sequence_hash_to_table(hashTable, sequence);
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

    clock_t end = clock();
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;

    printf("The elapsed time for INSERTING %d sequences is %f seconds\n", get_num_sequences(), time_spent);

    get_table_statistics(hashTable);

    check_false_positives(hashTable, seq_len);

    printf("Done!\n");
    
    return 0;

}
