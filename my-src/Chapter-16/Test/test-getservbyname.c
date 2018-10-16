#include "apue.h"
#include <netdb.h>

int
main(int argc, char *argv[]) {
    struct servent *sp;
    char *name, *proto;

    // /etc/services
    // ./a.out echo ddp
    if (argc != 3)
        err_quit("usage: %s service-name protocol-name", argv[0]);

    name = argv[1];
    proto = argv[2];
    if((sp = getservbyname(name, proto)) == NULL)
        err_sys("getservbyname error");
    printf("name: %s\n", sp->s_name);
    printf("port: %d\n", sp->s_port);
    printf("proto: %s\n", sp->s_proto);

    exit(0);
}
