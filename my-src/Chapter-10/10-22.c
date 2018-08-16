#include "apue.h"

static void sig_int(int);

int main(void) {
    sigset_t newmask, oldmask, waitmask;

    pr_mask("program start: ");

    if (signal(SIGINT, sig_int) == SIG_ERR)
        err_sys("signal(SIGINT) error");
    sigemptyset(&waitmask);
    sigaddset(&waitmask, SIGUSR1);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGINT);

    /*
     * 阻塞SIGINT并且保存当前当信号屏蔽字
     */
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
        err_sys("SIG_BLOCK error");

    /*
     * 临界区代码
     * 临界区内阻塞SIGINT信号
     */
    pr_mask("in critical region: ");

    /*
     * 进程暂停，允许除了SIGUSR1之外的所有信号被递送至此
     */
    if (sigsuspend(&waitmask) != -1)
        err_sys("sigsuspend error");

    pr_mask("after return from sigsuspend: ");

    /*
     * 重置信号屏蔽字
     * 由于sigsuspend返回之后会恢复进程的信号屏蔽字(此处也就是newmask)
     * 所以下面的if是解除了对SIGINT的阻塞
     */
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        err_sys("SIG_SETMASK error");

    /*
     * 继续处理
     */
    pr_mask("program exit: ");

    exit(0);
}

static void sig_int(int signo) {
    /*
     * 在SIGINT的信号处理程序中，该处理程序所处理的信号(此处即SIGINT)
     * 会自动被加入到该进程的信号屏蔽字中
     */
    pr_mask("\n in sig_int: ");
}
