#include "apue.h"
#include <grp.h>

int main(int argc, char *argv[]) {
    struct group *ptr;

    if (argc != 2) {
        err_quit("usage: %s <name>", argv[0]);
    }

    if ((ptr = getgrnam(argv[1])) != NULL) {
        printf("group name: %s\n", ptr->gr_name);
        printf("gid: %d\n", ptr->gr_gid);
        printf("passwd: %s\n", ptr->gr_passwd);
        printf("members: ");
        char **member = ptr->gr_mem;
        for (int i = 0; member[i] != NULL; ++i) {
            printf("<%s> ", member[i]);
        }
        printf("\n");
    }

    return (0);
}
