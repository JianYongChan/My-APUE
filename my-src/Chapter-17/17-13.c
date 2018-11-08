/*
 * 当一个进程(通常是server进程)想将一个fd传送给另一个进程时，
 * 可以调用send_fd或send_err
 * 等待接收的进程(client 进程)调用recv_fd
 *
 * 客户进程调用recv_fd接收fd
 * 如果一切正常(发送者调用了send_fd)，则函数返回值为非负的fd
 * 否则，返回值是由send_err发送的status(-1 ~ -255的一个负值)
 * 另外，如果服务器进程发送了一条出错消息，则客户端调用它自己的userfunc函数处理该消息
 * 通常将write函数指定为userfunc
 *
 * 这3个函数使用我们自己定义的协议。
 * 为发送一个fd，send_fd先发送2字节0，然后是实际fd
 * 为发送一条出错消息，send_err首先发送errmsg，然后是1字节0，最后是status字节的绝对值(1~255)
 * recv_fd函数读取套接字中所有字节直到遇到null字符。
 * null字符之前的所有字符都传送给调用者的userfunc
 * recv_fd读取的下一个字节是status，如果status字节为0，则表示一个fd已传送过来，否则表示没有fd可接收
 */
#include "apue.h"
#include <sys/socket.h>

#define CONTROLLEN CMSG_LEN(sizeof(int))

static struct cmsghdr *cmptr = NULL;

int
send_fd(int fd, int fd_to_send) {
    struct iovec    iov[1];
    struct msghdr   msg;
    char            buf[2];

    iov[0].iov_base = buf;
    iov[0].iov_len = 2;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    if (fd_to_send < 0) {
        msg.msg_control = NULL;
        msg.msg_controllen = 0;
        buf[1] = -fd_to_send;
        if (buf[1] == 0)
            buf[1] = 1;
    } else {
        if (cmptr == NULL && (cmptr = malloc(CONTROLLEN)) == NULL)
            return -1;
        cmptr->cmsg_level  = SOL_SOCKET;
        cmptr->cmsg_type   = SCM_RIGHTS; // 表明在传送访问权
        cmptr->cmsg_len    = CONTROLLEN;
        msg.msg_control    = cmptr;
        msg.msg_controllen = CONTROLLEN;
        *(int *)CMSG_DATA(cmptr) = fd_to_send;
        buf[1] = 0;
    }

    buf[0] = 0;
    if (sendmsg(fd, &msg, 0) != 2)
        return -1;
    return 0;
}
