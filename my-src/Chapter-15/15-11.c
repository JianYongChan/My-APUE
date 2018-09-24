#include "apue.h"
#include <sys/wait.h>

/* 如果shell变量PAGER已经定义且其值非空，则使用其值，否则使用字符串more */
#define PAGER "${PAGER:-more}"

int
main(int argc, char *argv[]) {
    char line[MAXLINE];
    FILE *fpin, *fpout;

    if (argc != 2)
        err_quit("usage %s <pathname>", argv[0]);
    if ((fpin = fopen(argv[1], "r")) == NULL)
        err_sys("cannot open %s", argv[1]);
    /* 以w方式popen，那么返回的FILE指针连接到PAGER的标准输入 */
    if ((fpout = popen(PAGER, "w")) == NULL)
        err_sys("popen error");

    while (fgets(line, MAXLINE, fpin) != NULL) {
        if (fputs(line, fpout) == EOF)
            err_sys("fputs error to pipe");
    }
    if (ferror(fpin))
        err_sys("fgets error");
    if (pclose(fpout) == -1)
        err_sys("pclose error");

    exit(0);
}
