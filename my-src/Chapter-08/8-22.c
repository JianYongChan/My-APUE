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
        execl("/bin/sh", "sh", "-c", cmdstring, (char*)0);
        /* 为什么用_exit不用exit？*/
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
