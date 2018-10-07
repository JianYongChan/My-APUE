#include "apue.h"

void sig_pipe(int signo) {
    fprintf(stdout, "Caught SIGPIPE\n");
}

// 如果对一个已经关闭了读端的pipe进行写操作
// 会产生SIGPIPE信号
// 如果忽略该信号或者捕捉该信号并从信号处理函数中返回，write会返回-1，而且errno被设置为EPIPE
int
main(int argc, char *argv[]) {
    pid_t   pid;
    int     fd[2];

    if (argc != 2)
        err_quit("usage: %s filename", argv[0]);

    if (pipe(fd) < 0)
        err_sys("pipe error");

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid > 0) { /* parent */
        FILE *fp;
        char line[MAXLINE];

        if (signal(SIGPIPE, sig_pipe) == SIG_ERR) { // 为SIGPIPE注册信号处理函数
            err_sys("signal SIGPIPE error");
        }

        close(fd[0]); // 父进程关闭读端
        if ((fp = fopen(argv[1], "r")) == NULL) {
            err_sys("parent fopen error");
        } else {
            while (fgets(line, MAXLINE, fp) != NULL) {
                int sz = strlen(line);
                if (write(fd[1], line, sz) != sz)
                    err_sys("write error");
            }
        }

        if (ferror(fp))
            err_sys("fgets error");
        close(fd[1]);
        if (waitpid(pid, NULL, 0) < 0)
            err_sys("waitpid error");
        exit(0);
    } else {
        close(fd[0]);
        close(fd[1]);

        exit(0);
    }
}
