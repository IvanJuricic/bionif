#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>

#define BUFF_LEN 255

char* append_string(char *s1, char *s2);

int main() {

    FILE *fp;
    int prevSize = 0, entries = 0;
    char buff[BUFF_LEN], *sequence, *tmp;
    bool firstEntry = true;

    fp = fopen("data", "r");

    while(fgets(buff, BUFF_LEN, (FILE *)fp) != NULL) {
        if(buff[0] == '>' && firstEntry == true) {
            firstEntry = false;
            continue;
        }
        else if(sequence != NULL && buff[0] == '>' && firstEntry == false) {
            // Add sequence to cuckoo filter
            printf("Sekvenca %d => len: %ld\n", entries, strlen(sequence));
            sequence = NULL;
            entries++;
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

    printf("Done!\n");
    
    return 0;

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