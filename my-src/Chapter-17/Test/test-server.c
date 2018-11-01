#include "apue.h"

extern int serv_listen(const char *name);
extern int serv_accept(int fd, uid_t *uidptr);

#define BUFSZ 1024
int
main(int argc, char *argv[]) {
    const char *name = "my_server";
    char buf[BUFSZ];
    int listen_fd, cli_fd;
    uid_t cli_uid;
    int message_sz;

    if ((listen_fd = serv_listen(name)) < 0)
        err_sys("serv_listen error");

    while (1) {
        if ((cli_fd = serv_accept(listen_fd, &cli_uid)) < 0)
            err_sys("serv_accept error");
        message_sz = sprintf(buf, "server: hello client %u\n", cli_uid);
        buf[message_sz] = 0;
        if (write(cli_fd, buf, BUFSZ) < 0)
            err_sys("write error");
    }
    return 0;
}
