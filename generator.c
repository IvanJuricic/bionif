#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_SIZE 50

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

    FILE *original, *generated, *tmp;

    if(argc != 2) {
        printf("\n\tUsage: ./test.o filename\n");
        exit(-1);
    }

    char *filename = argv[argc-1];
    char *tmp_filename = "tmp";
    char *gen_filename = "test_seq_50";
    char buff[BUFF_SIZE + 1];

    original = fopen(filename, "r");
    generated = fopen(gen_filename, "w+");
    tmp = fopen(tmp_filename, "w+");

    while(fgets(buff, BUFF_SIZE + 1, (FILE *)original) != NULL) {
        //printf("%s\n", buff);
        if(strlen(buff) == BUFF_SIZE) {
            //printf("strlen buff: %ld, buff size: %d\n", strlen(buff), BUFF_SIZE);
            fprintf(tmp, "%s\n", buff);
        }
    }

    char *rand_sequence = malloc(sizeof(char) * BUFF_SIZE);

    for(int i = 0; i < 1000; i++) {
        tmp = fopen(tmp_filename, "r");

        rand_string(rand_sequence, BUFF_SIZE);
        
        while(fgets(buff, BUFF_SIZE + 1, (FILE *)tmp) != NULL) {
            if(strlen(buff) == BUFF_SIZE && strcmp(buff, rand_sequence) != 0) {
                //printf("Rand seq => %ld\n", strlen(rand_sequence));
                fprintf(generated, "%s\n", rand_sequence);
                break;
            }
        }
    }

    free(rand_sequence);
    rand_sequence = NULL;

    fclose(generated);
    fclose(tmp);

    return 0;
}