#include "apue.h"
#include <sys/wait.h>

int main(void) {
    char buf[MAXLINE];
    pid_t pid;
    int status;

    printf("%% "); /* 提示符(printf 需要使用%%打印%) */
    // 由于`fgets`返回的每一行都以换行符终止，随后跟一个null字节
    // 而`execlp`函数要求的参数是以null结束而不是以换行符结束的
    // 所以需要使用null代替换行符
    while (fgets(buf, MAXLINE, stdin) != NULL) {
        if (buf[strlen(buf) - 1] == '\n') {
            buf[strlen(buf) - 1] = 0; /* 使用null代替换行 */
        }

        // `fork`对父进程返回新的子进程的pid
        // 对子进程则返回0
        // 所以说`fork`被调用一次，返回两次
        if ((pid = fork()) < 0) {
            err_sys("fork error");
        } else if (pid == 0) { /* 子进程 */
            // 子进程调用execlp执行新程序文件
            execlp(buf, buf, (char *)0);
            err_ret("couldn't execute: %s", buf);
            exit(127);
        }

        /* 父进程 */
        // 父进程等待子进程终止
        if ((pid = waitpid(pid, &status, 0)) < 0) {
            err_sys("waitpid error");
        }
        printf("%% ");
    }

    exit(0);
}