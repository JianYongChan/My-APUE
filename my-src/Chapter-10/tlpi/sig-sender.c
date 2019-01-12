#include "apue.h"

int main(int argc, char **argv) {
    int     num_sigs, sig, j;
    pid_t   pid;

    if (argc < 4 || strcmp(argv[1], "--help") == 0)
        err_quit("%s pid num-sigs signo [signo-2]", argv[0]);
    pid = (long)atoi(argv[1]);
    num_sigs = atoi(argv[2]);
    sig = atoi(argv[3]);

    // 将信号发送给进程
    printf("%s: sending signal %d to process %ld %d times\n",
            argv[0], sig, (long)pid, num_sigs);

    for (j = 0; j < num_sigs; j++) {
        if (kill(pid, sig) == -1)
            err_sys("kill process %d with signal %d failed", pid, sig);
    }

    if (argc > 4)
        if (kill(pid, atoi(argv[4])) == -1)
            err_sys("kill process %d with signal %d failed", pid, atoi(argv[4]));

    printf("%s: exiting\n", argv[0]);
    exit(EXIT_SUCCESS);
}
