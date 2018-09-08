#include "apue.h"
#include <errno.h>

static void
sig_hup(int signo) {
    printf("SIGHUP received, pid = %ld\n", (long)getpid());
}

/*
 * tcgetprgp返回前台进程组ID，它与在fd上打开的终端相关联
 */
static void
pr_ids(char *name) {
    printf("%s: pid = %ld, ppid= %ld, pgrp = %ld, tpgrp = %ld\n",
            name, (long)getpid(), (long)getppid(), (long)getpgrp(), (long)tcgetpgrp(STDIN_FILENO));
    fflush(stdout);
}

int
main(void) {
    char    c;
    pid_t   pid;

    pr_ids("parent");
    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid > 0) { /* 父进程 */
        // 让子进程在父进程终止之前运行的权宜之计
        sleep(5);
    } else {
        pr_ids("child");
        /*
         * POSIX.1要求向新孤儿进程组中处于停止状态的每一个进程发送挂断信号(SIGHUP)
         * 接着又向其发送继续信号
         * 对SIGHUP的系统默认动作是终止该进程，所以需要提供一个signal handler捕捉它
         * 所以期望sig_hup中的printf会在pr_ids中的printf之前执行
         */
        signal(SIGHUP, sig_hup);
        kill(getpid(), SIGTSTP); /* 停止自己，相当于Ctrl+Z */
        pr_ids("child");
        /*
         * 当后台进程组试图读控制终端时，对该进程组产生SIGTTIN
         * 但是在这里，这是一个孤儿进程组，如果内核用此信号停止他
         * 则此进程组中的进程就再也不会继续
         * POSIX.1规定，read返回出错，errno被设置为EIO
         */
        if (read(STDIN_FILENO, &c, 1) != 1)
            printf("read error %d on controlling TTy\n", errno);
    }

    exit(0);
}
