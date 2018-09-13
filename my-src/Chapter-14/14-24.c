#include "apue.h"

ssize_t
readn(int fd, void *ptr, size_t n) {
    size_t      nleft;
    ssize_t     nread;

    nleft = n;
    while (nleft > 0) {
        if ((nread = read(fd, ptr, nleft)) < 0) {
            if (nleft == n)
                return -1; // 一个字节都没有读
            else // 出错了，返回已经读到的字节数
                break;
        } else if (nread == 0) {
            break; // 到达文件尾(EOF)
        }
        // 这里就算没有读到制定的数据量(但是返回值>0)就继续读
        nleft -= nread;
        ptr += nread;
    }

    return (n-nleft); // 已经读到的字节数
}

ssize_t
writen(int fd, const void *ptr, size_t n) {
    size_t      nleft;
    ssize_t     nwritten;

    nleft = n;
    while (nleft > 0) {
        if ((nwritten = write(fd, ptr, nleft)) < 0) {
            if (nleft == n)
                return -1;
            else
                break;
        } else if (nwritten == 0) {
            break;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }

    return (n-nleft);
}
