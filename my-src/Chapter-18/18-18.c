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
    // 如果该程序产生了其他用户可能读取的core文件，
    // 或者如果其他某个进程能够设法读取该进程的存储空间，它们就可能会读取到这个明文口令
    //
    // 明文是指我们在getpass打印的提示符处键入的口令
    while (*ptr != 0)
        *ptr++ = 0;

    exit(0);
}
