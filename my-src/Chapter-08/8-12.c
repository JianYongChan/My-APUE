#include "apue.h"

static void charatatime(char *);

/* 这个程序中父子进程都输出一个字符串
 * 由于输出依赖于内核使这两个进程运行的顺序以及每个进程运行的时间长度
 * 所以该程序包含有一个竞争条件
 */

int main(void) {
    pid_t pid;

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid == 0) {
        charatatime("output from child\n");
    } else {
        charatatime("output from parent\n");
    }

    exit(0);
}

static void charatatime(char *str) {
    char *ptr;
    int   c;

    /* 设置成不带缓冲的，且每次输出一个字符而不是整个字符串
     * 于是每个字符输出都需调用一次write
     * 这样就可以观察到父子进程输出的字符串可能混合在一起
     */
    setbuf(stdout, NULL);
    for (ptr = str; (c = *ptr++) != 0; )
        putc(c, stdout);
}
