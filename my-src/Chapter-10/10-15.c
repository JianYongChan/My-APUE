#include "apue.h"

static void sig_quit(int);

int main(void) {
    sigset_t newmask, oldmask, pendmask;

    /*
     * 为SIGQUIT设置信号处理程序
     */
    if (signal(SIGQUIT, sig_quit) == SIG_ERR)
        err_sys("can't catch SIGQUIT");
    
    /* 
     * 阻塞SIGQUIT，并且保存当前的信号mask用以后面恢复信号mask
     * 此处应该保存旧的信号屏蔽字，而不应该在后面恢复的时候仅仅使用SIG_UNBLOCK解除对SIGQUIT的阻塞
     * 因为如果别人调用该程序，则有可能在该程序之前就已经屏蔽了SIG_QUIT了
     */
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGQUIT);
    // 如果&oldmask是个非空指针，则进程当前点信号屏蔽字(mask)通过oldmask返回
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
        err_sys("SIG_BLOCK error");

    sleep(5);
    
    // sigpending返回未决的信号集
    if (sigpending(&pendmask) < 0)
        err_sys("sigpending error");
    if (sigismember(&pendmask, SIGQUIT))
        printf("\nSIGQUIT pending\n");

    /*
     * 恢复旧的信号mask(该mask未阻塞SIGQUIT)
     */
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        err_sys("SIG_SETMASK error");
    printf("SIGQUIT unblocked\n");

    sleep(5);
    exit(0);
}

static void sig_quit(int signo) {
    printf("caught SIGQUIT\n");
    // 设置SITQUIT信号处理程序
    if (signal(SIGQUIT, SIG_DFL) == SIG_ERR)
        err_sys("cannot reset SIGQUIT");
}