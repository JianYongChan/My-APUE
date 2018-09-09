#include "apue.h"


void
pr_ids(const char *name) {
    printf("%s: pid = %ld, ppid = %ld, pgid = %ld, sid = %ld\n",
            name, (long)getpid(), (long)getppid(), (long)getpgrp(), (long)getsid(0));
}

int
main(void) {
    pid_t   pid;

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid == 0) {
        pr_ids("before setsid, child");
        setsid();
        pr_ids("aftet setsid, child");
    } else {
        pr_ids("before setsid, parent");
        setsid();
        pr_ids("after setsid, parent");
        waitpid(pid, NULL, 0);
    }
}
