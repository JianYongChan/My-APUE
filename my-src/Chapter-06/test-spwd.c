#include "apue.h"
#include <shadow.h>

// 需root权限
int main(int argc, char *argv[]) {
    struct spwd *ptr;

    if (argc != 2) {
        err_quit("usage: %s <name>", argv[0]);
    }
    if ((ptr = getspnam("ctom")) != NULL) {
        printf("name %s\n", ptr->sp_namp);
        printf("passwd: %s\n", ptr->sp_pwdp);
        printf("expire: %lu\n", ptr->sp_expire);
        printf("warn: %lu\n", ptr->sp_warn);
        printf("lstchg: %lu\n", ptr->sp_lstchg);
        printf("inactivity: %lu\n", ptr->sp_inact);
        printf("max: %lu\n", ptr->sp_max);
        printf("min: %lu\n", ptr->sp_min);
    }
}