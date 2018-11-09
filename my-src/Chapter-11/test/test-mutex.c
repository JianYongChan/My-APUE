#include "apue.h"
#include <pthread.h>

int share = 37;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *
thr_fn(void *arg) {
    printf("thread starting\n");
    pthread_mutex_lock(&mutex);
    printf("modify `share` \n");
    share = 7;
    pthread_mutex_unlock(&mutex);

    return (void*)1;
}

int 
main(void) {
    int err;
    pthread_t tid;

    printf("sizeof(pthread_t) = %lu\n", sizeof(pthread_t));

    err = pthread_create(&tid, NULL, thr_fn, NULL);
    if (err != 0)
        err_exit(err, "cannot create thread");
    pthread_mutex_lock(&mutex);
    printf("modify `share`\n");
    share = 13;
    pthread_mutex_unlock(&mutex);
    pthread_join(tid, NULL);
    printf("share = %d\n", share);
}
