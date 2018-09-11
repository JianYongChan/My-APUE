#include "apue.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>

int
main(int argc, char *argv[]) {
    int             fd;
    pid_t           pid;
    char            buf[5];
    struct stat     statbuf;

    if (argc != 2) {
        fprintf(stderr, "usage: %s filename\n", argv[0]);
        exit(1);
    }

    if ((fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, FILE_MODE))< 0) 
        err_sys("open error");
    if (write(fd, "abcdef", 6) != 6)
        err_sys("write error");

    /* 打开sgid，并关闭组执行位 */
    if (fstat(fd, &statbuf) < 0)
        err_sys("fstat error");
    if (fchmod(fd, (statbuf.st_mode & ~S_IXGRP | S_ISGID)) < 0)
        err_sys("fchmod error");

    TELL_WAIT();
    if ((pid = fork()) < 0) {
        err_ret("fork error");
    } else if (pid > 0) { /* 父进程 */
        /* 对整个文件加写锁 */
        if (write_lock(fd, 0, SEEK_SET, 0) < 0)
            err_ret("write_lock error");
        TELL_CHILD(pid);
        if (waitpid(pid, NULL, 0) < 0)
            err_sys("waitpid error");
    } else { /* 子进程 */
        WAIT_PARENT();

        /* 设置为非阻塞 */
        set_fl(fd, O_NONBLOCK);

        if (read_lock(fd, 0, SEEK_SET, 0) != -1)
            err_sys("child: read_lock succeeded");
        printf("read_lock of already-locked region returns %d\n", errno);

        // 由于子进程从父进程继承了fd
        // 而且它们两个共享文件偏移量
        // 所以要lseek才能读取到数据
        if (lseek(fd, 0, SEEK_SET) == -1)
            err_sys("lseek error");
        // 由于父进程已经对该文件加了写锁
        // 所以如果是强制性锁，子进程`没有持有锁就read`的行为会失败(read返回负值，errno被设置为EAGAIN)
        // 如果是建议性锁，则不会出错
        if (read(fd, buf, 2) < 0)
            err_sys("read failed (mandatory locking works)");
        else
            printf("read OK(no mandatory locking) buf = %2.2s\n", buf);
    }

    exit(0);
}
