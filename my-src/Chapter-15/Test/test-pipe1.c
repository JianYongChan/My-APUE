#include "apue.h"

int main() {
    int         fd[2];
    struct stat statbuf;

    if (pipe(fd) < 0)
        err_sys("pipe error");
    if (fstat(fd[0], &statbuf) < 0) {
        err_sys("fstat error");
    } else {
        if (S_ISFIFO(statbuf.st_mode))
            printf("FIFO\n");
        else 
            printf("NOT FIFO\n");
    }
}
