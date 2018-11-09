#include "apue.h"

int
main(void) {
    char *str1 = "./xibei.out";
    char *str2;

    if ((str2 = strrchr(str1, '/')) != NULL)
        printf("str2: %s\n", str2);
}
