#include "apue.h"
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>


#define PORT 3737
#define BUFSZ 64

int main(void) {
    int                  c_sockfd;
    struct sockaddr_in   s_addr;
    char                 msg_buf[BUFSZ];       
    int                  send_len;
    struct timeval       tv;


    if ((c_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_sys("create socket error");
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(PORT);
    s_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(c_sockfd, (struct sockaddr *)&s_addr, sizeof(s_addr)) < 0)
        err_sys("connect error");

    for ( ; ; ) {
        if (gettimeofday(&tv, NULL) < 0)
            err_sys("gettimeofday error");
        sprintf(msg_buf, "tv_sec: %ld\ttv_usec: %d\n", tv.tv_sec, tv.tv_usec);
        if (send(c_sockfd, (void*)msg_buf, BUFSZ, 0) < 0)
            err_sys("send error");
        sleep(1);
    }
}
