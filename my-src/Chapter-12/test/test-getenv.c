#include "apue.h"
#include <pthread.h>

extern char *getenv_1(const char *name);  /* non-reentrant version */
extern char *getenv_2(const char *name);  /* reentrant version using user-defined buffer*/
extern char *getenv_3(const char *name);  /* reentrant version using thread-specific data */

void *
thr_fn1(void *arg) {
    const char *name = (char*)arg;
    char *value;

    value = getenv_3(name);
    if (value != NULL)
        printf("%s=%s\n",name, value);
    else
        printf("%s not found\n", name);

    return (void*)1;
}


void *
thr_fn2(void *arg) {
    const char *name = (char*)arg;
    char *value;

    value = getenv_3(name);
    if (value != NULL)
        printf("%s=%s\n",name, value);
    else
        printf("%s not found\n", name);

    return (void*)2;
}

int
main(void) {
    int err;
    pthread_t tid1, tid2;
    char *name1 = "lover";
    char *name2 = "self";

    if (putenv("lover=xibei") != 0)
        err_sys("putenv error");
    if (putenv("self=tangmou") != 0)
        err_sys("putenv error");

    err = pthread_create(&tid1, NULL, thr_fn1, (void*)name1);
    if (err != 0)
        err_exit(err, "failed to create thread-1");
    err = pthread_create(&tid2, NULL, thr_fn2, (void*)name2);
    if (err != 0)
        err_exit(err, "failed to create thread-2");

    err = pthread_join(tid1, NULL);
    if (err != 0)
        err_exit(err, "failed to join thread-1");
    err = pthread_join(tid2, NULL);
    if (err != 0)
        err_exit(err, "failed to join thread-2");

    return 0;
}
