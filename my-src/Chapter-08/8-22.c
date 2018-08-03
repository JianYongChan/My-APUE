#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>

/* 不带有信号处理的版本 */
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

int main() {
    mysystem("date >> date.txt");

    return 0;
}
