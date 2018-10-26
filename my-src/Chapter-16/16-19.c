/*
 * 首先使用`lsof -i:4399`查看端口是否被ruptimed监听，以及NODE属性
 * 然后./ruptime NODE(刚刚查看到的)
 */
#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>

#define BUFLEN   128
#define TIMEOUT  20

void
sig_alrm(int signo) {

}

void
print_uptime(int sockfd, struct addrinfo *aip) {
    int     n;
    char    buf[BUFLEN];

    buf[0] = 0;
    if (sendto(sockfd, buf, 1, 0, aip->ai_addr, aip->ai_addrlen) <  0)
        err_sys("sendto error");
    alarm(TIMEOUT);
    if ((n = recvfrom(sockfd, buf, BUFLEN, 0, NULL, NULL)) < 0) {
        if (errno != EINTR)
            alarm(0);
        err_sys("recvfrom error");
    }
    alarm(0); // 取消之前尚未结束的⏰
    write(STDOUT_FILENO, buf, BUFLEN);
}

/*
 * 对于面向连接的协议，需要在交换数据之前连接到服务器。
 * 对于server来说，到来的连接请求已经足够判断出所需提供给client的服务。
 * 但是对于基于数据报的服务，需要有一种方法通知server来执行服务。
 * 本例中，只是简单地向server发送了1字节的数据。
 * 服务器将接收它，从数据包中得到地址，并使用这个地址来传送它的响应。
 */
int
main(int argc, char *argv[]) {
    struct addrinfo     *ailist, *aip;
    struct addrinfo     hint;
    int                 sockfd, err;
    struct sigaction    sa;

    if (argc != 2)
        err_quit("usage: ruptime hostname");

    /*
     * 如果server不在运行状态，客户端调用recvfrom便会无限期阻塞
     * 为了避免无限期阻塞，可以在调用recvfrom之前设置警告闹钟
     */
    sa.sa_handler = sig_alrm;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGALRM, &sa, NULL) < 0)
        err_sys("sigaction error");
    memset(&hint, 0, sizeof(hint));
    hint.ai_socktype = SOCK_DGRAM;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;
    if ((err = getaddrinfo(argv[1], "4399", &hint, &ailist)) != 0)
        err_quit("getaddrinfo error: %s", gai_strerror(errno));

    for (aip = ailist; aip != NULL; aip = ailist->ai_next) {
        if ((sockfd = socket(aip->ai_family, SOCK_DGRAM, 0)) < 0) {
            err = errno;
        } else {
            print_uptime(sockfd, aip);
            exit(0);
        }
    }

    fprintf(stderr, "cannot contact %s: %s\n", argv[1], strerror(err));
    exit(1);
}
