#include "apue.h"

char    *getpass(const char *);

int
main(void) {
    char    *ptr;

    if ((ptr = getpass("Enter password: ")) == NULL)
        err_sys("getpass error");
    printf("password: %s\n", ptr);

    // 如果调用getpass函数的程序使用的是明文口令
    // 那么为了安全起见，在程序完成后应在内存中清除它
    while (*ptr != 0)
        *ptr++ = 0;

    exit(0);
}
