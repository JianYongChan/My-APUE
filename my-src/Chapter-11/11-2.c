#include "apue.h"
#include <pthread.h>

pthread_t ntid;

void printids(const char *s) {
    pid_t       pid;
    pthread_t   tid;

    pid = getpid();
    /*
     * 为什么不直接使用全局变量ntid
     * 因为新线程可能在主线程调用pthread_create返回之前就运行了
     * 那么此时新线程看到的ntid就是未初始化的
     */
    tid = pthread_self();
    printf("%s pid %lu tid %lu (0x%lx)\n",
            s,
            (unsigned long)pid,
            (unsigned long)tid,
            (unsigned long)tid);
}

void* thr_fn(void *arg) {
    printids("new thread: ");
    return ((void*)0);
}

int main(void) {
    int     err;

    err = pthread_create(&ntid, NULL, thr_fn, NULL);
    if (err != 0)
        err_exit(err, "cannot create thread");
    printids("main thread");
    /*
     * 为什么主线程要sleep
     * 因为如果主线程不休眠，那么它就可能会退出
     * 这样可能新线程还没有运行，整个进程就已经终止了
     */
    sleep(1);
    exit(0);
}
