/*
 * 验证子进程会继承父进程的信号屏蔽字
 */

#include "apue.h"

static void sig_int(int signo) {
    pr_mask("in sig_int: ");
}

int main(void) {
    sigset_t newmask, oldmask;
    pid_t pid;
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGINT);

    if (signal(SIGINT, sig_int) == SIG_ERR)
        err_sys("signal(SIGINT) error");
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
        err_sys("SIG_BLOCK error");

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid == 0) { /* 子进程 */
        pr_mask("in child: ");
        kill(getpid(), SIGINT);
        sleep(7);
        if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
            err_sys("SIG_SETMASK error");
    } else { /* 父进程 */
        waitpid(pid, NULL, 0);
        pr_mask("in parent");
    }

    exit(0);
}
