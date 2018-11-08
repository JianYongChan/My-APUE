#include "open.h"
#include <fcntl.h>

#define BUFFSIZE 8192

/*
 * 先从标准输入读取一个路径名
 * 然后将该文件复制到标准输出
 *
 * 调用csopen函数来联系open服务器进程，从其返回一个打开描述符
 */
int
main(int argc, char *argv[]) {
    int     n, fd;
    char    buf[BUFFSIZE];
    char    line[MAXLINE];

    while (fgets(line, MAXLINE, stdin) != NULL) {
        if (line[strlen(line)-1] == '\n')
            line[strlen(line)-1] = 0;
        // 得到文件描述符
        if ((fd = csopen(line, O_RDONLY)) < 0)
            continue;

        // 将文件描述符对应的文件的内容显示到标准输出
        while ((n = read(fd, buf, BUFFSIZE)) > 0)
            if (write(STDOUT_FILENO, buf, n) != n)
                err_sys("write error");
        if (n < 0)
            err_sys("read error");
        close(fd);
    }

    exit(0);
}
