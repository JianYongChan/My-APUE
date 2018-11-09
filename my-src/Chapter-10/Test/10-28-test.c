#include "apue.h"

extern int mysystem2(const char *);

static void sig_int(int signo) {
    printf("Caught SIGINT\n");
}

static void sig_chld(int signo) {
    printf("Caught SIGCHLD\n");
}

int main(void) {
    if (signal(SIGINT, sig_int) == SIG_ERR)
        err_sys("signal(SIGINT) error");
    if (signal(SIGCHLD, sig_chld) == SIG_ERR)
        err_sys("signal(SIGCHLD) error");
    if (mysystem2("./getppid") < 0)
        err_sys("mysystem2() error");

    exit(0);
}
