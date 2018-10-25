#include "apue.h"
#include <errno.h>
#include <sys/socket.h>

/*
 * 将一个客户端的socket关联上一个地址没有多少新意，可以让系统选一个默认的地址
 * 然而，对于服务器，需要给一个接收客户端的服务器socket关联上一个众所周知的地址。
 * 客户端应该有一种方法来发现连接服务器所需要的地址。
 * 最简单的方法就是服务器保留一个地址并注册在/etc/services或者某个名字服务中。
 */
int init_server(int type, const struct sockaddr *addr, socklen_t alen, int qlen) {
    int fd;
    int err = 0;

    if ((fd = socket(addr->sa_family, type, 0)) < 0) // 创建套接字
        return -1;
    if (bind(fd, addr, alen) < 0)
        goto errout;
    if (type == SOCK_STREAM || type == SOCK_SEQPACKET) { // 将套接字绑定到地址
        if (listen(fd, qlen) < 0)
            goto errout;
    }
    return fd;

errout:
    // 保存errno，防止被close覆盖
    err = errno;
    close(fd);
    errno = err;
    return -1;
}
