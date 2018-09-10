#include "apue.h"
#include <errno.h>
#include <fcntl.h>

char buf [500000];

/*
 * 终端驱动程序一次能接受的数据量随系统而变
 * 具体结果还会因登录系统使用的方式的不同而不同
 * 这里将从标准输入得到的数据通过标准输出输出
 * 将每次输出的字节数和errno通过错误输出输出
 *
 * ./a.out < /etc/services > temp.file # 数据输出至文件，调试信息输出至终端
 *
 * ./a.out < /etc/services 2>setderr.out # 数据输出至终端，调试信息输出至文件
 */

int
main(void) {
    int     ntowrite, nwrite;
    char    *ptr;

    ntowrite = read(STDIN_FILENO, buf, sizeof(buf));
    fprintf(stderr, "read %d bytes\n", ntowrite);

    /* 设置非阻塞 */
    set_fl(STDOUT_FILENO, O_NONBLOCK);

    ptr = buf;
    while (ntowrite > 0) {
        errno = 0;
        nwrite = write(STDOUT_FILENO, ptr, sizeof(buf));
        fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno);

        if (nwrite > 0) {
            ptr += nwrite;
            ntowrite -= nwrite;
        }
    }

    /* 清楚非阻塞位 */
    clr_fl(STDOUT_FILENO, O_NONBLOCK);
    exit(0);
}
