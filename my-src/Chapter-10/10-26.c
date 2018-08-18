/*
 * system函数的第一个版本
 * 这个版本存在一些问题：
 * 当用system运行另一个程序时，不应使父子进程都捕捉终端产生都两个信号：
 * SIGINT, SIGQUIT，这两个信号只应该发送给正在运行都程序：子进程。
 * 原因有两个：
 *      1. 由system执行的程序可能使交互式命令，比如ed
 *      2. system的调用者在程序执行时放弃了控制，等待该程序的结束，所以
 *         system的调用者就不应该接收这两个信号
 * 所以POSIX.1规定system函数的调用者在等待命令完成时应当忽略这两个信号。
 */
#include "apue.h"
#include <errno.h>

static void sig_int(int signo) {
    printf("Caught SIGINT\n");
}

static void sig_chld(int signo) {
    printf("Caught SIGCHLD");
}

int mysystem(const char *cmdstring) {
    pid_t pid;
    int   status;

    if (cmdstring == NULL)
        return 1;

    if ((pid = fork()) < 0) {
        status = -1;
    } else if (pid == 0) { /* 子进程 */
        /* -c选项告诉shell程序读取下一个命令行参数(此处为cmdstring)作为命令输入
         * 而不是从标准输入或从一个给定的文件中读命令
         */
        execl("/bin/sh", "sh", "-c", cmdstring, (char*)0);
        /* 用_exit而不用exit，
         * 是为了防止任一标准I/O缓冲(这些缓冲会在fork中由父进程复制到子进程)在子进程中被冲洗
         */
        _exit(127);
    } else {
        while (waitpid(pid, &status, 0) < 0) {
            if (errno == EINTR) {
                status = -1;
                break;
            }
        }
    }

    return status;
}


int main(void) {
    if (signal(SIGINT, sig_int) == SIG_ERR)
        err_sys("signal(SIGINT) error");
    if (signal(SIGCHLD, sig_chld) == SIG_ERR)
        err_sys("signal(SIGCHLD) error");
    if (mysystem("/bin/ed") < 0)
        err_sys("system() error");

    exit(0);
}
