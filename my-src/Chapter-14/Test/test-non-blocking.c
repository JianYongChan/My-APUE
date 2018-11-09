#include "apue.h"
#include <fcntl.h>

int
main(int argc, char *argv[]) {
    int     fd;
    int     val;

    if (argc != 2) {
        fprintf(stderr, "usage: %s filename\n", argv[0]);
        exit(1);
    }

    if ((fd = open(argv[1], O_RDWR | O_CREAT, FILE_MODE)) < 0)
        err_sys("open error");

    // 设置为非阻塞
    if ((val = fcntl(fd, F_GETFL, 0)) < 0)
        err_sys("F_GETFL error");
    val |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, val) < 0)
        err_sys("F_SETFL error");
}
