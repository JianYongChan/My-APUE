// compile: gcc -o ruptime 16-16.c 16-11.c -lapue
// execute: ./ruptime ccdemacbook-pro

#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>

#define BUFLEN 128

// 16-11.c
extern int connect_retry(int, int, int, const struct sockaddr*, socklen_t);

void
print_uptime(int sockfd) {
    int     n;
    char    buf[BUFLEN];

    while ((n = recv(sockfd, buf, BUFLEN, 0)) > 0)
        write(STDOUT_FILENO, buf, n);
    if (n < 0)
        err_sys("recv error");
}

int
main(int argc, char *argv[]) {
    struct addrinfo     *ailist, *aip;
    struct addrinfo     hint;
    int                 sockfd, err;

    if (argc != 2)
        err_quit("usage: ./ruptime hostname");
    memset(&hint, 0, sizeof(hint));
    // hint用来筛选出符号特定条件的地址
    // hint是一个用于过滤地址的模板，包括ai_family, ai_flags, ai_protocol和ai_socket字段
    // 剩余的整数字段必须设置为0，指针字段必须设置为空
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;
    // 如果getaddrinfo失败，不能使用perror或者strerror来生成错误消息
    // 而是要调用gai_strerror来将返回的错误码转换成错误消息
    //
    // 此外这里getaddrinfo的service字段，书上用的是"ruptime"
    // 但是这个service并没有注册到/etc/services中去，所以函数一直failed
    // 这里我将其设置为端口号4399(小游戏)而不是服务名
    if ((err = getaddrinfo(argv[1], "4399", &hint, &ailist)) != 0)
        err_quit("getaddrinfo error: %s", gai_strerror(err));
    /*
     * 如果服务器支持多重网络接口或者多重网络协议
     * 函数getaddrinfo可能会返回多个候选地址供使用
     * 此时，轮流尝试每个地址，直到找到一个允许连接到服务的地址
     */
    for (aip = ailist; aip != NULL; aip = aip->ai_next) {
        if ((sockfd = connect_retry(aip->ai_family, SOCK_STREAM, 0, aip->ai_addr, aip->ai_addrlen)) < 0) {
            err = errno;
        } else {
            print_uptime(sockfd);
            exit(0);
        }
    }

    err_exit(err, "cannot connect to %s", argv[1]);
}
