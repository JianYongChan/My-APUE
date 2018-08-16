/**
 * 使用信号来实现父子进程之间的同步
 * 此程序中使用了SIGUSR1和SIGUSR2两个信号
 * SIGUSR1由父进程发送给子进程，SIGUSR2由子进程发送给父进程
 */

#include "apue.h"

static volatile sig_atomic_t sigflag; /* 由信号处理程序将其设置为非0值 */
static sigset_t newmask, oldmask, zeromask;

/*
 * 用于SIGUSR1和SIGUSR2的信号处理程序
 */
static void sig_usr(int signo) {
    sigflag = -1;
}

void TIME_WAIT(void) {
    if (signal(SIGUSR1 , sig_usr) == SIG_ERR)
        err_sys("signal(SIGUSR1) error");
    if (signal(SIGUSR2, sig_usr) == SIG_ERR)
        err_sys("signal(SIGUSR2) error");
    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    sigaddset(&newmask, SIGUSR2);

    /*
     * 阻塞SIGUSR1和SIGUSR2
     */
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
        err_sys("SIG_BLOCK error");
}

void TELL_PARENT(pid_t pid) {
    kill(pid, SIGUSR2); /* 告诉父进程我子进程已经好了 */
}

void WIAT_PARENT(void) {
    while (sigflag == 0)
        sigsuspend(&zeromask);
    sigflag = 0;

    /*
     * 恢复原信号屏蔽字
     */
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        err_sys("SIG_SETMASK error");
}

void TELL_CHILD(pid_t pid) {
    kill(pid, SIGUSR1); /* 告诉子进程我父进程已经好了 */
}

void WAIT_CHILD(void) {
    while (sigflag == 0)
        sigsuspend(&zeromask);
    sigflag = 0;

    /*
     * 恢复原信号屏蔽字
     */
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        err_sys("SIG_SETMASK error");
}
