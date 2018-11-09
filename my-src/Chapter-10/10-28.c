/*
 * system函数的第二个版本
 * POSIX.1要求system忽略SIGINT和SIGQUIT，阻塞SIGCHLD
 */

#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

int mysystem2(const char *cmdstring) {
    pid_t               pid;
    int                 status;
    struct sigaction    ignore, saveintr, savequit;
    sigset_t            chldmask, savemask;

    if (cmdstring == NULL)
        return 1;

    ignore.sa_handler = SIG_IGN;
    sigemptyset(&ignore.sa_mask);
    ignore.sa_flags = 0;
    /*
     * 忽略SIGINT，SIGQUIT信号
     * 并且保存其原处理程序
     */
    if (sigaction(SIGINT, &ignore, &saveintr) < 0)
        return -1;
    if (sigaction(SIGQUIT, &ignore, &savequit) < 0)
        return -1;
    /*
     * 阻塞SIGCHLD，并保存原信号屏蔽字
     */
    sigemptyset(&chldmask);
    sigaddset(&chldmask, SIGCHLD);
    if (sigprocmask(SIG_BLOCK, &chldmask, &savemask) < 0)
        return -1;

    if ((pid = fork()) < 0) {
        status = -1;
    } else if (pid == 0) { /* 子进程 */
        /*
         * 子进程中恢复对SIGINT,SIGQUIT信号的处理方式
         * 以及恢复原信号屏蔽字
         */
        sigaction(SIGINT, &saveintr, NULL);
        sigaction(SIGQUIT, &savequit, NULL);
        sigprocmask(SIG_SETMASK, &savemask, NULL);

        execl("/bin/sh", "sh", "-c", cmdstring, (char*)0);
        /*
         * 使用_exit而不是exit，是为了防止任一标准I/O缓冲(这些缓冲会在fork中由父进程复制到子进程)
         * 在子进程中被冲洗
         */
        _exit(127); /* exec错误 */
    } else { /* 父进程 */
        while (waitpid(pid, &status, 0) < 0) {
            if (errno != EINTR) {
                status = -1;
                break;
            }
            printf ("waitpid interrupted\n");
        }
    }
    
    /*
     * 父进程中恢复对SIGINT, SIGQUIT的处理程序
     * 并且恢复原信号屏蔽字
     */
    if (sigaction(SIGINT, &saveintr, NULL) < 0)
        return -1;
    if (sigaction(SIGQUIT, &saveintr, NULL) < 0)
        return -1;
    if (sigprocmask(SIG_SETMASK, &savemask, NULL) < 0)
        return -1;

    return status;
}
