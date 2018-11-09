#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include "apue.h"


int
main(void) {
    pid_t pid;
    int   fd_to_send;
    int   fd[2];
    char  buf[1024];

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd) < 0)
        err_sys("socketpair error");

    TELL_WAIT();
    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid == 0) { /* 子进程 */
        close(fd[1]);

        if ((fd_to_send = open("/etc/services", O_RDONLY)) < 0)
            err_sys("open error");
        if (send_fd(fd[0], fd_to_send) < 0)
            err_sys("send_fd error");
        // 设置位置为距离文件尾4K字节处
        if (lseek(fd_to_send, -4096, SEEK_END) < 0)
            err_sys("lseek error");

        TELL_PARENT(getppid());
    } else { /* 父进程 */
        close(fd[0]);
        WAIT_CHILD();

        if ((fd_to_send = recv_fd(fd[1], write)) < 0)
            err_sys("recv_fd error");
        // 验证父进程读取的是否是该文件的最后4K字节
        while (read(fd_to_send, buf, 1024) != 0)
            write(STDOUT_FILENO, buf, strlen(buf));
        if (waitpid(pid, NULL, 0) < 0)
            err_sys("waitpid error");
        close(fd[1]);
    }
}
