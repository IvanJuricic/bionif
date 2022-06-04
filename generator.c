#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int BUFF_SIZE = 0;

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

int main(int argc, char *argv[]) {

    FILE *original, *generated_10, *generated_20, *generated_50, *generated_100, *generated_1000, *tmp;

    if(argc != 2) {
        printf("\n\tUsage: ./test.o filename\n");
        exit(-1);
    }

    char *filename = argv[argc-1];

    char *tmp_filename = "tmp";

    char *gen_filename_10 = "test_seq_10";
    char *gen_filename_20 = "test_seq_20";
    char *gen_filename_50 = "test_seq_50";
    char *gen_filename_100 = "test_seq_100";
    char *gen_filename_1000 = "test_seq_1000";

    for(int j = 0; j < 5; j++) {
        
        char *gen_file_name;
        FILE *gen_file;

        if(j == 0) {
            BUFF_SIZE = 10;
            gen_file_name = gen_filename_10;
        }
        else if(j == 1) {
            BUFF_SIZE = 20;
            gen_file_name = gen_filename_20;
        }
        else if(j == 2) {
            BUFF_SIZE = 50;
            gen_file_name = gen_filename_50;
        }
        else if(j == 3) {
            printf("Tu smo\n");
            BUFF_SIZE = 100;
            gen_file_name = gen_filename_100;
        }
        else if(j == 4) {
            BUFF_SIZE = 1000;
            gen_file_name = gen_filename_1000;
        }

        char buff[BUFF_SIZE + 1];

        original = fopen(filename, "r");
        tmp = fopen(tmp_filename, "w+");
        gen_file = fopen(gen_file_name, "w+");

        while(fgets(buff, BUFF_SIZE + 1, (FILE *)original) != NULL) {
            //printf("%s\n", buff);
            if(strlen(buff) == BUFF_SIZE) {
                //printf("strlen buff: %ld, buff size: %d\n", strlen(buff), BUFF_SIZE);
                fprintf(tmp, "%s\n", buff);
            }
        }

        char *rand_sequence = malloc(sizeof(char) * BUFF_SIZE);

        for(int i = 0; i < 5000; i++) {
            
            tmp = fopen(tmp_filename, "r");
            rand_string(rand_sequence, BUFF_SIZE);
            
            while(fgets(buff, BUFF_SIZE + 1, (FILE *)tmp) != NULL) {
                if(strlen(buff) == BUFF_SIZE && strcmp(buff, rand_sequence) != 0) {
                    fprintf(gen_file, "%s\n", rand_sequence);
                    break;
                }
            }

            fclose(tmp);
        }

        fclose(gen_file);
        free(rand_sequence);
        rand_sequence = NULL;
    }

    printf("Exiting!\n");

    return 0;
}