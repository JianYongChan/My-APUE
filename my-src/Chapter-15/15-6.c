#include "apue.h"
#include <sys/wait.h>

#define DEF_PAGER   "/bin/more"

int
main(int argc, char *argv[]) {
    int     n;
    int     fd[2];
    pid_t   pid;
    char    *pager, *argv0;
    char    line[MAXLINE];
    FILE    *fp;

    if (argc != 2)
        err_quit("usage: %s <pathname>", argv[0]);

    if ((fp = fopen(argv[1], "r")) == NULL)
        err_sys("cannot open %s", argv[1]);
    if (pipe(fd) < 0)
        err_sys("pipe error");

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid > 0) {
        close(fd[0]); // 父进程关闭读端

        /* 将argv[1]表示的文件的内容写入管道 */
        while (fgets(line, MAXLINE, fp) != NULL) {
            n = strlen(line);
            if (write(fd[1], line, n) != n)
                err_sys("write error");
        }
        /* 为什么要有这一句 */
        if (ferror(fp))
            err_sys("fgets error");
        close(fd[1]); // 写完了就把写端也关闭
        if (waitpid(pid, NULL, 0) < 0)
            err_sys("waitpid error");

        exit(0);
    } else {
        close(fd[1]); // 子进程关闭写端
        /*
         * 这里将fd[0]复制到标准输入
         * 在复制之前应该比较该描述符是否已经具有所希望的值
         * 如果该描述符具有所希望的值，则dup2返回而不关闭该fd，然后关闭该fd的副本
         */
        if (fd[0] != STDIN_FILENO) {
            if (dup2(fd[0], STDIN_FILENO) != STDIN_FILENO)
                err_sys("dup2 error to stdin");
            close(fd[0]);
        }

        /* 首先尝试使用环境变量PAGER获得用户分页程序名称
         * 如果不成功，则使用/bin/more
         */
        if ((pager = getenv("PAGER")) == NULL)
            pager = DEF_PAGER;
        // strrchr定位到'/'最后一次出现的位置
        // 没有出现则返回NULL
        if ((argv[0] = strrchr(pager, '/')) != NULL)
            argv0++; // 跳过最后一个斜杠(路径分隔符)
        else
            argv0 = pager; // 没有斜杠
        if (execlp(pager, argv0, (char*)0) < 0)
            err_sys("execl error for %s", pager);
    }
    exit(0);
}
