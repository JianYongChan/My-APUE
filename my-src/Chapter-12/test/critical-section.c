#include "apue.h"
#include <pthread.h>

static int glob = 0;

static void *
thr_fn(void *arg) {
    int loops = *((int*)arg);
    int loc, j;

    /*
     * 两个线程运行该函数
     * 当thread-1获取到了glob并将其赋值给loc后时间片满了
     * 调度程序运行thread-2，它将glob增长一些之后又运行thread-1
     * thread-1执行glog=loc，这就将thread-2产生的递增效果给丢失了
     */
    for (j = 0; j < loops; ++j) {
        loc = glob;
        ++loc;
        glob = loc;
    }

    return NULL;
}

int
main(int argc, char *argv[]) {
    int         loops, err;
    pthread_t   tid1, tid2;

    if (argc > 1)
        loops = atoi(argv[1]);
    else
        loops = 100000000;

    err = pthread_create(&tid1, NULL, thr_fn, (void*)&loops);
    if (err != 0)
        err_exit(err, "cannot create thread-1");
    err = pthread_create(&tid2, NULL, thr_fn, (void*)&loops);
    if (err != 0)
        err_exit(err, "cannot create thread-2");

    err = pthread_join(tid1, NULL);
    if (err != 0)
        err_exit(err, "cannot join thread-1");
    err = pthread_join(tid2, NULL);
    if (err != 0)
        err_exit(err, "cannot join thread-2");

    printf("glob = %d\n", glob);
    exit(0);
}
