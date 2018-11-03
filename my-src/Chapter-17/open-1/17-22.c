#include "17-20.h"
#include <fcntl.h>

void
handle_request(char *buf, int nread, int fd) {
    int newfd;

    // buf的格式：open pathname oflag\0
    if (buf[nread-1] != 0) {
        snprintf(errmsg, MAXLINE-1, "request not nullterminated: %*.*s\n", nread, nread, buf);
        send_err(fd, -1, errmsg);
        return;
    }

    // 将字符串分解成标准的argv型参数表，并调用用户处理函数
    if (buf_args(buf, cli_args) < 0) {
        send_err(fd, -1, errmsg);
        return;
    }
    if ((newfd = open(pathname, oflag)) < 0) {
        snprintf(errmsg, MAXLINE-1, "cannot open %s: %s\n", pathname, strerror(errno));
        send_err(fd, -1, errmsg);
        return;
    }
    if (send_fd(fd, newfd) < 0)
        err_ret("send_fd error");
    close(newfd); // 关闭fd，但是不会关闭文件，因为fd已经发送出去了(即使还没有收到)
}
