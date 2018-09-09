#include "apue.h"

int
main(void) {
    pid_t   pid;

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid == 0) { /* 子进程 */
        printf("child, pid = %d\n", getpid());
        printf("child, group id = %d\n", getpgid(0));
        setpgid(pid, 0);
        printf("after setpgid, child, pid = %d\n", getpid());
        printf("after setpgid, child group id =  %d\n", getpgrp());
        exit(0);
    } { /* 父进程 */
        printf("parent, pid = %d\n", getpid());
        printf("parent, group id =  %d\n", getpgrp());
        if (waitpid(pid, NULL, 0) < 0)
            err_sys("waitpid error");
    }

}
