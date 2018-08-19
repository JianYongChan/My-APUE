/*
 * APUE-3e上说
 * 当对一个停止的进程产生一个SIGCONT信号时，该进程就继续
 * 即使该信号是被阻塞的或者是忽略的
 * 特此验证一下
 */
#include "apue.h"

static void sig_cont(int signo) {
    printf("Caught SIGCONT\n");
}

int main(void) {
    struct sigaction newact;

    /*
     * SIGCONT被忽略
     */
    newact.sa_handler = SIG_IGN;
    sigemptyset(&newact.sa_mask);
    sigaction(SIGCONT, &newact, NULL);

    kill(getpid(), SIGSTOP);

    printf("Continue1...\n");

    /*
     * SIGCONT被阻塞
     */
    newact.sa_handler = sig_cont;
    sigaddset(&newact.sa_mask, SIGCONT);
    sigaction(SIGCONT, &newact, NULL);

    kill(getpid(), SIGSTOP);

    printf("Continue2...\n");
}
