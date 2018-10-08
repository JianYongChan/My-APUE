#include "apue.h"

static void sig_pipe(int);

// 父进程中标准输入读取两个数，然后写入pipe
// 子进程从pipe读取父进程写入的两个数，将其相加的结果写入pipe
// 另一边父进程在等待子进程的结果，收到以后将其写入到标准输出
// 然后父进程继续从标准输入读取两个数，如此循环
int main(void) {
    int     n, fd1[2], fd2[2];
    pid_t   pid;
    char    line[MAXLINE];

    if (signal(SIGPIPE, sig_pipe) == SIG_ERR)
        err_sys("signal error");

    if (pipe(fd1) < 0 || pipe(fd2) < 0)
        err_sys("pipe error");

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid > 0) {
        close(fd1[0]);
        close(fd2[1]);

        while (fgets(line, MAXLINE, stdin) != NULL) {
            n = strlen(line);
            if (write(fd1[1], line, n) != n)
                err_sys("write error to pipe");
            if ((n = read(fd2[0], line, MAXLINE)) < 0)
                err_sys("write error from pipe");
            if (n == 0) {
                err_msg("child close pipe");
                break;
            }
            line[n] = 0; // 没有这一句会有问题
            // 此处需要注意
            // fputs返回的非负值都表示成功
            // 而EOF表示失败
            if (fputs(line, stdout) == EOF)
                err_sys("fputs error");
        }

        // 此处也需要注意
        // fgets和gets函数并不区分EOF和error
        // 任何一个发生都是返回NULL
        // 所以调用者必须使用ferror函数来确定是EOF还是error发生了
        if (ferror(stdin)) 
            err_sys("fgets error on stdin");
    } else {
        close(fd1[1]);
        close(fd2[0]);
        if (fd1[0] != STDIN_FILENO) {
            if (dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO)
                err_sys("dup2 error to stdin");
            close(fd1[0]);
        }

        if (fd2[1] != STDOUT_FILENO) {
            if (dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO)
                err_sys("dup2 error to stdout");
            close(fd2[1]);
        }

        // gcc -o add2 15-17.c -lapue
        if (execl("./add2", "add2", (char*)0) < 0)
            err_sys("execl error");
    }

    exit(0);
}

static void
sig_pipe(int signo) {
    printf("SIGPIPE caught\n");
    exit(1);
}
