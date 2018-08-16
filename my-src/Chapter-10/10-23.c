/*
 * sigsuspend的另一种应用是等待一个信号处理程序设置一个全局变量
 * 此程序用于捕捉中断信号和退出信号，
 * 但是希望仅当捕捉到退出信号时，才唤醒主例程
 */
#include "apue.h"

volatile sig_atomic_t quitflag; /* 由信号处理程序将其设置为非0值 */

/*
 * 用于SIGINT和SIGQUIT的信号处理程序
 */
static void sig_int(int signo) {
    if (signo == SIGINT)
        printf("\ninterrupt\n");
    else if (signo == SIGQUIT)
        quitflag = 1;
}

int main(void) {
    sigset_t newmask, oldmask, zeromask;

    if (signal(SIGINT, sig_int) == SIG_ERR)
        err_sys("signal(SIGINT) error");
    if (signal(SIGQUIT, sig_int) == SIG_ERR)
        err_sys("signal(SIGQUIT) error");

    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGQUIT);

    /*
     * 阻塞SIGQUIT并且保存当前的信号屏蔽字
     * 而后进程被挂起
     */
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
        err_sys("SIG_BLOCK error");

    while (quitflag == 0)
        sigsuspend(&zeromask);

    /*
     * SIGQUIT被捕获了
     * 而且现在SIGQUIT还是被阻塞
     */
    quitflag = 0;
    
    /*
     * 恢复原信号屏蔽字(不阻塞SIGQUIT)
     */
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        err_sys("SIG_SETMASK error");

    exit(0);
}
