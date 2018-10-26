#include "apue.h"
#include <errno.h>
#include <sys/socket.h>

int init_server(int type, const struct sockaddr *addr, socklen_t alen) {
    int fd, err;
    int reuse = 1;

    if ((fd = socket(addr->sa_family, type, 0)) < 0)
        return -1;
    /*
     * 为了启用SO_REUSEADDR选项，设置了一个非0的整数值，并将该整数地址作为var参数传递给setsockopt
     * 将len参数设置成了一个整数大小来表明val所指的对象的大小
     */
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
        goto errout;
    if (bind(fd, addr, alen) < 0)
        goto errout;
    if (type == SOCK_STREAM || type == SOCK_SEQPACKET)
        if (listen(fd, alen) < 0)
            goto errout;
    return fd;

errout:
    err = errno;
    close(fd);
    errno = err;
    return -1;
}
