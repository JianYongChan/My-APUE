#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define handler_errno_en(en, msg) \
    do {                          \
        errno = en;               \
        perror(msg);              \
        exit(EXIT_FAILURE);       \
    } while(0)

static int done = 0;
static int cleanup_pop_arg = 0;
static int cnt = 0;

static void
cleanup_handle(void *arg) {
    printf("Called clean-up handler\n");
    cnt = 0;
}

static void *
thread_start(void *arg) {
    time_t start, curr;

    printf("New thread created\n");

    pthread_cleanup_push(cleanup_handle, NULL);

    curr = start = time(NULL);

    while (!done) {
        pthread_testcancel();
        if (curr < time(NULL)) {
            curr = time(NULL);
            printf("cnt = %d\n", cnt);
            ++cnt;
        }
    }

    pthread_cleanup_pop(cleanup_pop_arg);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t thr;
    int s;
    void *res;

    s = pthread_create(&thr, NULL, thread_start, NULL);
    if (s != 0)
        handler_errno_en(s, "pthread_create");

    sleep(2);

    if (argc > 1) {
        if (argc > 2)
            cleanup_pop_arg = atoi(argv[2]);
        done = 1;
    } else {
        printf("Canceling thread\n");
        s = pthread_cancel(thr);
        if (s != 0)
            handler_errno_en(s, "pthread_cancle");
    }

    s = pthread_join(thr, &res);
    if (s != 0)
        handler_errno_en(s, "pthread_join");

    if (res = PTHREAD_CANCELED)
        printf("Thread was canceled; cnt = %d\n", cnt);
    else
        printf("Thread terminated normally; cnt = %d\n", cnt);
    exit(EXIT_SUCCESS);
}
