#include "apue.h"
#include <sys/wait.h>

char *env_init[] = { "USRER=unkonwn", "PATH=/tmp", NULL };

int main(void) {
    pid_t pid;

    if ((pid = fork()) < 0){
        err_sys("fork error");
    } else if (pid == 0) { /* 第一个子进程 */
        if (execle("/home/ctom/ilinux/study/readbooks/APUE/my-src/Chapter-08/echoall", 
                    "echoall",
                    "my arg1",
                    "MY ARG2",
                    (char*)0,
                    env_init) < 0) {
            err_sys("execle error");
        }
    }

    /* 父进程，等待第一个子进程结束 */
    if (waitpid(pid, NULL, 0) < 0)
        err_sys("wait error");

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid == 0) { /* 第二个子进程 */
        if (execlp("./echoall", "echoall", "only 1 arg", (char*)0) < 0) {
            err_sys("execlp error");
        }
    }

    exit(0);
}
