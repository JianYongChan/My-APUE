#include "apue.h"
#include <sys/utsname.h>

#define NAMELEN 64

int main(void) {
    struct utsname nameinfo;
    char hostname[NAMELEN];

    if (uname(&nameinfo) >= 0) {
        printf("sysname: %s\n", nameinfo.sysname);
        printf("nodename: %s\n", nameinfo.nodename);
        printf("release: %s\n", nameinfo.release);
        printf("version: %s\n", nameinfo.version);
        printf("machine: %s\n", nameinfo.machine);
        printf("domain name: %s\n", nameinfo.__domainname);
    } else {
        err_sys("uanme error");
    }

    if (gethostname(hostname, NAMELEN) >= 0) {
        printf("hostname: %s\n", hostname);
    } else {
        err_sys("gethostname error");
    }

    return 0;
}