#include "apue.h"

int globval = 6;

int main(void) {
    int   var;
    pid_t pid;

    var = 88;
    printf("before vfork\n");
    if ((pid = vfork()) < 0) {
        err_sys("vfork error");
    } else if (pid == 0) {
        globval++;
        var++;
        _exit(0);
    }
    printf("pid = %ld, glob = %d, var = %d\n", (long)getpid(), globval, var);
    exit(0);
}
