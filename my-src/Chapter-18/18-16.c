#include "apue.h"

extern char *my_ttyname(int fd);

int main(void) {
    char    *name;

    for (int i = 0; i <= 2; i++) {
        if (isatty(i)) {
            name = my_ttyname(i);
            if (name == NULL)
                name = "undefined";
        } else {
            name = "not a tty";
        }

        printf("fd %d: %s\n", i, name);
    }

    exit(0);
}
