#include "apue.h"

int main(void) {
    pid_t   pid;

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid != 0) { /* 父进程 */
        sleep(2);
        exit(2);
    }

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid != 0) { /* 第一个子进程 */
        sleep(4);
        abort(); /* 终止并转储 */
    }

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid != 0) { /* 第二个子进程 */
        execl("/bin/dd", "dd", "if=/etc/passwd", "of=/dev/null", NULL);
        exit(7); /* 此处不可达 */
    }

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid != 0) { /* 第三个子进程 */
        sleep(8);
        exit(0); /* 正常终止 */
    }


    sleep(6);                  /* 第四个子进程 */
    kill(getpid(), SIGKILL);
    exit(6);
}
