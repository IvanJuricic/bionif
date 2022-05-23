#include <stdio.h>
#include <malloc.h>

#define BUFF_LEN 255

int main() {

    FILE *fp;
    int entries = 0;
    char buff[BUFF_LEN], *sequences;

    fp = fopen("data", "r");

    while(fgets(buff, BUFF_LEN, (FILE *)fp) != NULL) {
        if(buff[0] == '>') continue;
        entries++;
        printf("%s", buff);
    }
    printf("\n");
    
    return 0;

}