#include "apue.h"
#include <sys/msg.h>

#define MAXMSG 512

struct mymesg {
    long mtype;         // 消息的类型
    char mtext[MAXMSG]; // 消息的数据部分
};

int
main(int argc, char *argv[]) {
    key_t           key;
    long            qid;
    size_t          nbytes;
    struct mymesg   m;

    if (argc != 3) {
        fprintf(stderr, "usage: ./sendmsg KEY message\n");
        exit(1);
    }
    // 将string转换为long
    key = strtol(argv[1], NULL, 0);
    if ((qid = msgget(key, 0)) < 0)
        err_sys("cannot open queue key %s", argv[1]);
    memset(&m, 0, sizeof(m));
    strncpy(m.mtext, argv[2], MAXMSG-1);
    nbytes = strlen(m.mtext);
    m.mtype = 1;
    if (msgsnd(qid, &m, nbytes, 0) < 0)
        err_sys("cannot send message");

    exit(0);
}
