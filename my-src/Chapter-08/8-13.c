#include "apue.h"

static void charatatime(char *);


int main(void) {
    pid_t pid;

    TELL_WAIT();

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid == 0) { /* 子进程 */
        WAIT_PARENT(); /* 等待父进程先运行完毕 */
        charatatime("output from child\n");
    } else { /* 父进程 */
        charatatime("output from parent\n");
        TELL_CHILD(pid); /* 父进程运行完毕，通知子进程 */
    }

    exit(0);
}

static void charatatime(char *str) {
    char *ptr;
    int   c;

    /* 设置为不带缓存 */
    setbuf(stdout, NULL);
    for (ptr = str; (c = *ptr++) != 0; )
        putc(c, stdout);
}
