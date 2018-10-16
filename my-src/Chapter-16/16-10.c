#include "apue.h"
#include <sys/socket.h>

#define MAXSLEEP 128

int
connect_retry(int sockfd, const struct sockaddr *addr, socklen_t alen) {
    int numsec;

    /*
     * 指数退避算法连接
     */
    for (numsec = 1; numsec < MAXSLEEP; numsec <<= 1) {
        if (connect(sockfd, addr, alen) == 0) {
            /* connect recepted */
            return 0;
        }

        /* 重试之前先delay */
        if (numsec <= MAXSLEEP/2)
            sleep(numsec);
    }

    return -1;
}
