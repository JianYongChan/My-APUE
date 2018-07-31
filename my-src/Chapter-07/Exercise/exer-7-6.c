#include <stdio.h>
#include <stdlib.h>

int main(void) {
    long *lptr;
    lptr = calloc(1, sizeof(long));
    printf("%ld\n", lptr[0]);

    int **ptr = calloc(1, sizeof(void*));
    if (ptr[0] == NULL)
        printf("null ptr\n");
    else
        printf("not null ptr");
}
