/*
 * longjmp函数在信号处理程序中经常被用来返回到程序的主程序中, 而不是从该处理程序返回
 * 但是调用longjmp函数有一个问题。
 * 当进入信号捕捉函数，此时当前信号被自动地加入到该进程的信号屏蔽字中，
 * 这阻止了后来产生当这种信号中断该信号处理程序
 */

#include "apue.h"
#include <setjmp.h>
#include <time.h>

static void sig_usr1(int);
static void sig_alrm(int);
static sigjmp_buf jmpbuf;
/*
 * sig_atomic_t是又ISO C标准定义的变量类型
 * 在写这种变量的时候不会被中断
 * 这种类型的变量总是是用volatile修饰
 * 这是因为该变量将由两个不同当控制线程——main函数和异步执行当信号处理程序访问
 */
static volatile sig_atomic_t canjump;

int main(void) {
    // 为SIGUSR1, SIGALRM设置信号处理程序
    if (signal(SIGUSR1, sig_usr1) == SIG_ERR)
        err_sys("signal(SIGUSR1) error");
    if (signal(SIGALRM, sig_alrm)  == SIG_ERR)
        err_sys("signal(SIGALRM) error");

    // 打印调用进程信号屏蔽字中的信号名(当前主要是SIGINT, SIGQUIT, SIGUSR1, SIGALRM)
    pr_mask("starting main: ");

    /*
     * sigsetjmp的第二个参数不为0
     * 表示要在第一个参数中保存进程的当前信号屏蔽字
     * 且这种情况下，siglongjmp会从其中恢复保存的信号屏蔽字
     */
    if (sigsetjmp(jmpbuf, 1)) {
        pr_mask("ending main: "); // 从siglongjmp中返回时才会运行到这里
        exit(0);
    }
    /*
     * 仅在调用完sigsetjmp之后才设置canjump为非0值
     * 并且在信号处理程序中检测此变量，仅当它为非0值时才调用siglongjmp
     * 使得jmpbuf尚未被sigsetjmp初始化时，防止调用信号处理程序
     */
    canjump = 1;

    for ( ; ;  )
        pause();
}

static void sig_usr1(int signo) {
    time_t starttime;

    if (canjump == 0)
        return;

    pr_mask("starting sig_usr1: ");

    alarm(3);
    starttime = time(NULL);
    for ( ; ;  )
        if (time(NULL) > starttime+5)
            break;
    pr_mask("finishing sig_usr1: ");

    canjump = 0;
    siglongjmp(jmpbuf, 1);
}

static void sig_alrm(int signo) {
    pr_mask("in sig_alrm");
}
