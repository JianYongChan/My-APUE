#include "apue.h"
#include <fcntl.h>

int main(int argc, char *argv[]) {
    int val;

    if (argc != 2) {
        err_quit("usage: ./a.out <descriptor#>");
    }

    // 指定命令行第二个参数为文件描述符
    // F_GETFL将对应于fd的文件状态标志返回
    if ((val = fcntl(atoi(argv[1]), F_GETFL, 0)) < 0) {
        err_sys("fcntl error for fd %d", atoi(argv[1]));
    }

    // O_RDONLY, O_WRONLY, O_RDWR这三个访问标志的值分别为0, 1, 2
    // 而它们加上O_EXEC, O_SEARCH这5个互斥，一个文件的访问方式只能取这5个值之一
    // 所以首先要使用屏蔽字O_ACCMODE(=3)取得访问位
    // 然后再进行比较
    switch(val & O_ACCMODE) {
        case O_RDONLY:
            printf("read only");
            break;

        case O_WRONLY:
            printf("write only");
            break;

        case O_RDWR:
            printf("read write");
            break;

        default:
            err_dump("unkonwn access mode");
    }

    if (val & O_APPEND) {
        printf(", append");
    }
    if (val & O_NONBLOCK) {
        printf(", nonblocking");
    }
    if (val & O_SYNC) {
        printf(", synchronous writes");
    }

#if !defined(_POSIX_C_SOURCE) && defined(O_FSYNC) && (O_FSYNC != O_SYNC)
    if (val & O_FSYNC) {
        printf(", synchronous writes");
    }
#endif

    putchar('\n');
    exit(0);
}