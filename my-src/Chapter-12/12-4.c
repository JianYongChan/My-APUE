#include "apue.h"
#include <pthread.h>

int
makethread(void *(*fn)(void*), void *arg) {
    int             err;
    pthread_t       tid;
    pthread_attr_t  attr;
    
    err = pthread_attr_init(&attr);
    if (err != 0)
        return err;
    // 设置成"以分离状态启动线程"
    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (err == 0)
        err = pthread_create(&tid, &attr, fn, arg);
    pthread_attr_destroy(&attr); // 注意这里没有检查destroy是否成功

    return err;
}


static void *
thr_fn1(void *arg) {
    printf("thread-1\n");
    return (void*)1;
}

static void *
thr_fn2(void *arg) {
    printf("thread-2\n");
    return (void*)2;
}

int
main() {
    int err;

    err = makethread(thr_fn1, (void*)1);
    if (err != 0)
        err_exit(err, "failed to make thread-1");
    err = makethread(thr_fn2, (void*)2);
    if (err != 0)
        err_exit(err, "failed to make thread-2");

    sleep(3); // 防止主线程退出(而其余两个线程还没有开始运行)而导致进程退出
    return 0;
}
