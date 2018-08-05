#include "apue.h"

int main(void) {
    char *user;

    if ((user = getlogin()) == NULL)
        err_sys("getlogin error");
    printf("user identification: %s\n", user);

    exit(0);
}