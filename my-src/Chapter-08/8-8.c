#include "apue.h"
#include <sys/wait.h>

/* 如果一个进程fork一个子进程，但不要等待子进程终止
 * 也不希望子进程处于僵尸状态直到父进程终止
 * 实现这一要求的窍门是fork两次
 */
int main(void) {
    pid_t pid;

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid == 0) { /* 第一个子进程 */
        if ((pid = fork()) < 0) /* 第一个子进程里继续fork */
            err_sys("fork error");
        else if (pid > 0) /* 第一个子进程 */
            exit(0);

        /* 第二个子进程
         * 该进程的父进程是第一个子进程
         * 第一个子进程虽然一个exit，但是它尚未被其父进程所回收
         * 所以这里需要等待一段时间让第一个子进程的父进程将第一个子进程给回收了
         * 这样第二个子进程就会被init进程所收养，printf打印的进程号为1
         * 否则的话，第二个子进程可能会比第一个子进程的父进程先指向，printf打印的就是第一个子进程的id
         */
        sleep(2);
        printf("second child, parent pid = %ld\n", (long)getppid());
        exit(0);
    }

    /* 父进程 */
    // sleep(20)可以让我们通过ps -elf | grep "a.out"来查看僵尸进程(defunct)
    // sleep(20);
    // 回收处理第一个子进程
    if (waitpid(pid, NULL, 0) != pid)
        err_sys("waitpid error");

    exit(0);
}
