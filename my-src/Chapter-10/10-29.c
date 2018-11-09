#include "apue.h"

static void sig_alrm(int signo) {
    /* Nothing to do, just returnging wake up sigsuspend() */
}

unsigned int mysleep3(unsigned int seconds) {
    struct sigaction  newact, oldact;
    sigset_t          newmask, oldmask, susmask;
    unsigned int      unslept;

    /*
     * 设置对SIGALRM的处理程序
     * 并保存原处理方案
     */
    newact.sa_handler = sig_alrm;
    sigemptyset(&newact.sa_mask);
    newact.sa_flags = 0;
    sigaction(SIGALRM, &newact, &oldact);

    /*
     * 阻塞SIGALRM并保存原信号屏蔽字
     */
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGALRM);
    sigprocmask(SIG_BLOCK, &newmask, &oldmask);

    alarm(seconds);
    susmask = oldmask;

    /* 确保SIGALRM没有被阻塞 */
    sigdelset(&susmask, SIGALRM);

    /* 等待信号被捕获 */
    sigsuspend(&susmask);

    /*
     * 某个信号被捕获
     * SIGALRM现在已解除阻塞
     */
    unslept = alarm(0);

    /*
     * 恢复原信号屏蔽字
     */
    sigprocmask(SIG_SETMASK, &oldmask, NULL);
    
    return unslept;
}
