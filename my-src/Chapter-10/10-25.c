#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * POSIX风格的abort函数
 */
void abort(void) {
    sigset_t         mask;
    struct sigaction action;

    /*
     *
     */
    sigaction(SIGABRT, NULL, &action);
    if (action.sa_hanler == SIG_IGN) {
        action.sa_handler = SIG_DFL;
    }
}
