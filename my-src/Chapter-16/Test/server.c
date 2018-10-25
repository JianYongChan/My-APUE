#include "apue.h"
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 3737
#define BUFSZ 64

int main(void) {
    int                 s_sockfd, c_sockfd;
    struct sockaddr_in  s_addr, c_addr;
    int                 reuse = 1;
    socklen_t           c_len;
    int                 msg_len;
    char                msg_buf[BUFSZ];
    char                ip_buf[INET_ADDRSTRLEN];

    if ((s_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_sys("socket error");

    // 允许地址重用
    setsockopt(s_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    s_addr.sin_family = AF_INET;         // ipv4
    s_addr.sin_port = htons(PORT);
    s_addr.sin_addr.s_addr = INADDR_ANY; // 所有地址

    if (bind(s_sockfd, (struct sockaddr*)&s_addr, sizeof(s_addr)) < 0)
        err_sys("bind error");

    if (listen(s_sockfd, 10) < 0)
        err_sys("listen error");

    for ( ; ;  ) {
        printf("waiting for connecton\n");
        fflush(stdout);

        // 调用accept所返回的sockfd连接到调用connect的client
        // 这个新的c_sockfd和原始套接字描述符s_sockfd具有相同的套接字类型和地址族
        // 传给accept的s_sockfd并没有关联到这个连接，而是继续保持可用状态并接收其他连接
        c_len = sizeof(c_addr);
        if ((c_sockfd = accept(s_sockfd, (struct sockaddr*)&c_addr, &c_len)) < 0) {
            err_sys("accept error");
        }

        printf("get new connecton\n");
        if (inet_ntop(AF_INET, (void*)&c_addr.sin_addr, ip_buf, INET_ADDRSTRLEN) == NULL)
            err_sys("inet_ntop error");
        printf("IP: %s\n", ip_buf);
        printf("Port: %d\n", c_addr.sin_port);
        fflush(stdout);

        while ((msg_len = recv(c_sockfd, msg_buf, BUFSZ, 0)) > 0)
            write(STDOUT_FILENO, msg_buf, msg_len);
        if (msg_len < 0)
            err_sys("recv error");

    }

    close(s_sockfd);
    return 0;
}

