#include "apue.h"
#include <errno.h>

void pr_mask(const char *str) {
    sigset_t sigset;
    int      errno_saves;

    errno_saves = errno;
    if (sigprocmask(0, NULL, &sigset) < 0) {
        err_ret("sigprocmask error");
    } else {
        if (sigismember(&sigset, SIGINT))
            printf(" SIGINT");
        if (sigismember(&sigset, SIGQUIT))
            printf(" SIGQUIT");
        if (sigismember(&sigset, SIGUSR1))
            printf(" SIGUSER1");
        if (sigismember(&sigset, SIGALRM))
            printf(" SIGALRM");

        printf("\n");
    }

    errno = errno_saves;
}