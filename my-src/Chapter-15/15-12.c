#include "apue.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>

static pid_t *childpid = NULL;

static int maxfd;

FILE *
my_popen(const char *cmdstring, const char *type) {
    int i;
    int pfd[2];
    pid_t pid;
    FILE *fp;

    // type只能是"w"或者"r"
    if ((type[0] != 'w' && type[0] != 'r') || type[1] != 0) {
        errno = EINVAL;
        return NULL;
    }

    // 第一次调用my_popen
    if (childpid == NULL) {
        // open_max返回
        maxfd = open_max();
        // calloc分配的内存用0字节填充
        // 分配maxfd个pid_t大小的内存
        if ((childpid = calloc(maxfd, sizeof(pid_t))) == NULL)
            return NULL;
    }

    if (pipe(pfd) < 0)
        return NULL;
    // 不使用那些其值>=maxfd的管道文件描述符
    if (pfd[0] >= maxfd || pfd[1] >= maxfd) {
        close(pfd[0]);
        close(pfd[1]);
        errno = EMFILE; // 表明这里很多文件描述符是打开的
        return NULL;
    }

    if ((pid = fork()) < 0) {
        return NULL;
    } else if (pid == 0) { /* 子进程 */
        if (*type == 'r') {
            // type为"r"，则返回的文件指针连接到cmdstring的标准输出，是可读的
            // 那么cmdstring就产生输出，所以需要关闭读端(pfd[0])，并将写端(pdf[1])复制到标准输出
            close(pfd[0]);
            if (pfd[1] != STDOUT_FILENO) {
                dup2(pfd[1], STDOUT_FILENO);
                close(pfd[1]);
            }
        } else {
            // type为"w"，则返回的文件指针连接到cmdstring的标准输入，是可写的
            // 那么cmdstring就读取输入，所以需要关闭写端(pdf[1])，并将读端复制到标准输入
            close(pfd[1]);
            if (pfd[0] != STDIN_FILENO) {
                dup2(pfd[0], STDIN_FILENO);
                close(pfd[0]);
            }
        }

        // POSIX.1要求popen关闭那些以前调用popen打开的，现在仍然在子进程中打开着的I/O流
        for (i = 0; i < maxfd; ++i) {
            if (childpid[i] > 0)
                close(i);
        }

        // sh -c cmdstr
        execl("/bin/sh", "sh", "-c", cmdstring, (char*)0);
        _exit(127);
    }

    /* 父进程 */
    if (*type == 'r') {
        close(pfd[1]);
        if ((fp = fdopen(pfd[0], type)) == NULL) {
            return NULL;
        }
    } else {
        close(pfd[0]);
        if ((fp = fdopen(pfd[1], type)) == NULL)
            return NULL;
    }

    childpid[fileno(fp)] = pid;
    return fp;
}

int
my_pclose(FILE *fp) {
    int     fd, stat;
    pid_t   pid;

    if (childpid == NULL) {
        errno = EINVAL;
        return -1;
    }

    fd = fileno(fp);
    if (fd > maxfd) {
        errno = EINVAL;
        return -1;
    }
    if ((pid = childpid[fd]) == 0) {
        errno = EINVAL;
        return -1;
    }

    childpid[fd] = 0;
    if (fclose(fp) == EOF)
        return -1;

    while (waitpid(pid, &stat, 0) < 0)
        if (errno != EINTR)
            return  -1;

    return stat;
}
