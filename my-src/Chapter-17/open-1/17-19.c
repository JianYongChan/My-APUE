#include "17-17.h"
#include <sys/uio.h>

/*
 * 客户进程创建一个fd管道，然后调用fork和exec来调用服务器进程。
 * 客户进程使用一端经fd管道发送请求，服务器进程使用另一端经fd管道回送响应。
 *
 * （1）客户进程通过fd管道向服务器发送"open <pathname> <openmode>\0"形式的请求
 *      <openmode>是数值，以ASCII十进制数表示，是open的第二个参数。
 *      该请求以null字符终止
 * （2）服务器进程调用send_fd或者send_err回送打开描述符或者出错消息
 */

int
csopen(char *name, int oflag) {
    pid_t           pid;
    int             len;
    char            buf[10];
    struct iovec    iov[3];
    static int      fd[2] = {-1, -1}; // 为什么设置成static ?

    if (fd[0] < 0) {
        if (fd_pipe(fd) < 0) {
            err_ret("fd_pipe error");
            return -1;
        }
        if ((pid = fork()) < 0) {
            err_ret("fork error");
            return -1;
        } else if (pid == 0) { /* 子进程 */
            close(fd[0]); // 关闭读端
            // 将标准输入和标准输出都复制到fd[1]
            if (fd[1] != STDIN_FILENO) {
                if (dup2(fd[1], STDIN_FILENO) != STDIN_FILENO)
                    err_sys("dup2 error to stdin");
            }
            if (fd[1] != STDOUT_FILENO) {
                if (dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO)
                    err_sys("dup2 error to stdout");
            }
            // 调用服务器进程
            // 该进程负责打开文件并会送文件描述符
            if (execl("./opend", "opend", (char*)0) < 0)
                err_sys("execl error");
        }
        // 父进程
        close(fd[1]); // 关闭写端
    }
    // iov[3]分别是open, <pathname>, <oflag>
    sprintf(buf, " %d", oflag);
    iov[0].iov_base = CL_OPEN " "; /* 字符串连接 */
    iov[0].iov_len  = strlen(CL_OPEN) + 1;
    iov[1].iov_base = name;
    iov[1].iov_len  = strlen(name);
    iov[2].iov_base = buf;
    iov[2].iov_len  = strlen(buf)+1;
    len = iov[0].iov_len + iov[1].iov_len + iov[2].iov_len;
    if (writev(fd[0], &iov[0], 3) != len) { // 搞不懂fd[0]不是读端么，怎么还可以写？
        err_ret("writev error");
        return -1;
    }

    // 返回由服务器进程回送的fd
    return (recv_fd(fd[0], write));
}
