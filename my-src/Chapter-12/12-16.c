/*
 * 10-23.c程序的另一个版本
 * 不依赖信号处理程序中断主控线程
 * 而是有专门的独立控制线程进行信号处理
 * 在互斥量的保护下改动quitflag的值
 * 这样主控线程不会再调用pthread_cond_signal时错失唤醒调用
 * 在主控线程中使用相同的互斥量来检查标志的值
 * 并原子地释放互斥量，等待条件的发生
 */

#include "apue.h"
#include <pthread.h>

int         quitflag;
sigset_t    mask;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t waitloc = PTHREAD_COND_INITIALIZER;

void *
thr_fn(void *arg) {
    int err, signo;

    for ( ; ;  ) {
        /*
         * 线程在调用sigwait之前必须阻塞那些它正在等待的信号
         * sigwait会原子地取消信号集的阻塞状态，直到有新的信号被递送
         * 在返回之前，sigwait将恢复线程的信号屏蔽字
         * 如果信号在被调用的时候没有被阻塞
         * 那么线程在sigwait的调用之前会出现一个时间窗
         * 在这个时间窗中，信号就可以被法师给线程
         */
        err = sigwait(&mask, &signo);
        if (err != 0)
            err_exit(err, "sigwait failed");
        switch(signo) {
            case SIGINT:
                printf("\ninterrupt\n");
                break;
            case SIGQUIT:
                pthread_mutex_lock(&lock);
                quitflag = 1;
                pthread_mutex_unlock(&lock);
                pthread_cond_signal(&waitloc);
                return 0;
            default:
                printf("unexpected signal %d\n", signo);
                exit(1);
        }
    }
}

int
main(void) {
    int         err;
    sigset_t    oldmask;
    pthread_t   tid;

    /*
     * 在主线程开始时阻塞SIGINT, SIGQUIT
     * 当创建线程进行信号处理时，新建线程会继承现有的信号屏蔽字
     * 因为sigwait会解除信号的阻塞状态，所以只有一个线程可以用于信号的接收
     * 这可以使我们队主线程编码时不必担心来自这些信号的中断
     */
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);
    if ((err = pthread_sigmask(SIG_BLOCK, &mask, &oldmask)) != 0)
        err_exit(err, "SIG_BLOCK error");

    err = pthread_create(&tid, NULL, thr_fn, 0);
    if (err != 0)
        err_exit(err, "cannot create thread");

    /*
     * 调用者把锁住的互斥量传给wait函数
     * 函数然后自动把调用线程放到条件变量的等待列表上，对互斥量解锁
     * 这就关闭了条件检查和线程进入休眠状态的等待条件改变这两个操作之间的时间通道
     * 这样线程就不会错过条件的任何变化
     * pthread_cond_wait返回时，互斥量再次被锁住
     */
    pthread_mutex_lock(&lock);
    while (quitflag == 0)
        pthread_cond_wait(&waitloc, &lock);
    pthread_mutex_unlock(&lock);

    quitflag = 0;

    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        err_sys("SIG_SETMASK error");

    exit(0);
}
