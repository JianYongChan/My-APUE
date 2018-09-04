#include "apue.h"
#include <pthread.h>

static int glob = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static void *
thr_fn(void *arg) {
    int loops = *((int*)arg);
    int loc, j, err;

    for (j = 0; j < loops; ++j) {
        err = pthread_mutex_lock(&lock);
        if (err != 0)
            err_exit(err, "faild to lock mutex");
        loc = glob;
        ++loc;
        glob = loc;
        err = pthread_mutex_unlock(&lock);
        if (err != 0)
            err_exit(err, "faild to unlock mutex");
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
