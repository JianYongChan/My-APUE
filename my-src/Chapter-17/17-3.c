/*
 * XSI消息队列的使用存在一个问题，即不能将其和poll和select一起使用，因为它不能关联到文件描述符。
 * 然而，socket是和fd相关联的，消息到达时，可以使用socket来通知。
 */
#include "apue.h"
#include <poll.h>
#include <pthread.h>
#include <sys/msg.h>
#include <sys/socket.h>

#define NQ      3       /* number of queues */
#define MAXMSG  512     /* maximum message size */
#define KEY     0x123   /* key for first message queue */

struct threadinfo {
    int qid;
    int fd;
};

// 见APUE P453
struct mymesg {
    long mtype;
    char mtext[MAXMSG];
};

void *helper(void *arg) {
    int                 n;
    struct mymesg       m;
    struct threadinfo   *tip = arg;

    for ( ; ; ) {
        memset(&m, 0, sizeof(m));
        // msgrcv返回消息数据部分的长度
        // 第4个参数type指定想要哪一种消息，0表示队列中的第一个消息
        if ((n = msgrcv(tip->qid, &m, MAXMSG, 0, MSG_NOERROR)) < 0)
            err_sys("msgrcv error");
        if (write(tip->fd, m.mtext, n) < 0)
            err_sys("write error");
    }
}

/*
 * main函数中创建了一些消息队列和UNIX域套接字
 * 并为每个消息队列开启了一个线程
 * 每个线程都会在msgrcv调用中阻塞，当消息到达时，线程会把它写入一个UNIX域套接字的一端
 * 然后main函数在一个无线循环中用poll来轮询选择一个套接字端点
 * 当某个套接字可读时，程序可从套接字中读取数据将其打印到标准输出上
 */
int
main() {
    int                 i, n, err;
    int                 fd[2];
    int                 qid[NQ];
    struct pollfd       pfd[NQ];
    struct threadinfo   ti[NQ];
    pthread_t           tid[NQ];
    char                buf[MAXMSG];

    for (i = 0; i < NQ; i++) {
        // msgget返回消息队列ID
        if ((qid[i] = msgget((KEY+i), IPC_CREAT | 0666)) < 0)
            err_sys("msg error");

        printf("queue ID %d is %d\n", i, qid[i]);

        if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd) < 0)
            err_sys("socketpair error");
        pfd[i].fd = fd[0];
        pfd[i].events = POLLIN; // 可以不阻塞地读高优先级数据以外的数据
        ti[i].qid = qid[i];
        ti[i].fd = fd[1];
        // 为每个消息队列都创建一个线程
        if ((err = pthread_create(&tid[i], NULL, helper, &ti[i])) != 0)
            err_exit(err, "pthread_create error");
    }

    for ( ; ; ) {
        // timeout设置为-1表示永远等待
        if (poll(pfd, NQ, -1) < 0)
            err_sys("poll error");
        // 对每个fd，都检查其是否已经准备好被read
        // 如果准备好了，则将接收到的数据打印到标准输出
        for (i = 0; i < NQ; i++) {
            if (pfd[i].events & POLLIN) {
                if ((n = read(pfd[i].fd, buf, sizeof(buf))) < 0)
                    err_ret("read error");
                buf[n] = 0;
                printf("queue id %d, message %s\n", qid[i], buf);
            }
        }
    }

    exit(0);
}
