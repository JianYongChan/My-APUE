#include "apue.h"

/*
 * signal的另外一个版本
 * 力图阻止被中断的系统调用重新启动
 */
Sigfunc *signal_intr(int signo, Sigfunc *func) {
    struct sigaction act, oact;

    act.sa_flags = 0;
    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
#ifdef SA_INTERRUPT
    act.sa_flags |= SA_INTERRUPT;
#endif
    if (sigaction(signo, &act, &oact) < 0)
        return SIG_ERR;

    return oact.sa_handler;
}