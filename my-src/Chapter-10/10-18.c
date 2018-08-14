#include "apue.h"

/*
 * 使用sigaction函数来实现signal函数
 */

Sigfunc *my_signal(int signo, Sigfunc *func) {
    struct sigaction act, oact;

    act.sa_handler = func;
    sigemptyset(&act.sa_mask); // 必须用sigemptyset初始化，因为act.sa_mask并不保证它做同样的事情
    act.sa_flags = 0;
    /*
     * 对SIGALRM以外的信号都设置SA_RESTATb标志，
     * 如此以来，被这些信号中断的系统调用都能自动重启动
     * 而不对SIGALRM设置SA_RESTART标志的原因是，
     * 我们希望能够使用SIGALRM对I/O操作设置时间限制
     */
    if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT;
#endif
    } else {
        act.sa_flags |= SA_RESTART;
    }
    if (sigaction(signo, &act, &oact) < 0)
        return (SIG_ERR);

    return (oact.sa_handler);
}