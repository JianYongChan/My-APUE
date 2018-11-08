#include "17-26.h"

#define NALLOC 10

static void
client_alloc(void) {
    int     i;

    if (client == NULL)
        client = malloc(NALLOC *sizeof(Client));
    else
        client = realloc(client, (client_size+NALLOC)*sizeof(Client));
    if (client == NULL)
        err_sys("cannot alloc for client array");

    for (i = client_size; i < client_size+NALLOC; i++)
        client[i].fd = -1;

    client_size += NALLOC;
}

int
client_add(int fd, uid_t uid) {
    int     i;

    // 尚未初始化client
    if (client == NULL)
        client_alloc();
again:
    for (i = 0; i < client_size; i++) {
        if (client[i].fd == -1) {
            client[i].fd = fd;
            client[i].uid = uid;
            return i;
        }
    }

    // client数组满了导致没能添加
    // 则扩容后继续尝试添加
    client_alloc();
    goto again;
}

void
client_del(int fd) {
    int     i;

    for (i = 0; i < client_size; i++) {
        if (client[i].fd == fd) {
            client[i].fd = -1;
            return;
        }
    }

    log_quit("cannot find client entry for fd %d", fd); // 没有找到fd
}
