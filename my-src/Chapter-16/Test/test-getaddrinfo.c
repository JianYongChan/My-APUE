#include "apue.h"
#include <sys/socket.h>
#include <netdb.h>
#include <assert.h>


static void pr_addrinfo(const struct addrinfo *aip) {
    assert(aip != NULL);
    printf("family: ");
    switch(aip->ai_family) {
        case AF_INET:
            printf("AF_INET");
            break;
        case AF_INET6:
            printf("AF_INET6");
            break;
        case AF_UNIX:
            printf("AF_UNIX");
            break;
        case AF_UNSPEC:
            printf("AF_UNSPEC");
            break;
        default:
            printf("Unknown");
    }
    printf("\n");

    printf("protocol: ");
    switch(aip->ai_protocol) {
        case IPPROTO_IP:
            printf("ipv4");
            break;
        case IPPROTO_RAW:
            printf("raw");
            break;
        case IPPROTO_TCP:
            printf("tcp");
            break;
        case IPPROTO_UDP:
            printf("udp");
            break;
        case IPPROTO_ICMP:
            printf("icmp");
            break;
        case IPPROTO_IPV6:
            printf("ipv6");
            break;
        default:
            printf("unknown");
    }
    printf("\n");

    printf("socket type: ");
    switch(aip->ai_socktype) {
        case SOCK_RAW:
            printf("raw");
            break;
        case SOCK_DGRAM:
            printf("datagram");
            break;
        case SOCK_STREAM:
            printf("stream");
            break;
        case SOCK_SEQPACKET:
            printf("seqpacket");
            break;
        default:
            printf("unknown");
    }
    printf("\n");
}

int
main(void) {
    struct addrinfo ai;
    const char *host = "localhost";
    const char *service = "echo";

    if (getaddrinfo(host, service, &ai, NULL) < 0)
        err_sys("getaddrinfo errror");

    pr_addrinfo(&ai);
}
