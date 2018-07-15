#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    int ret;
    int fd;

    if ((fd = open("/home/ctom", O_EXCL)) < 0) {
        printf("Error: %s\n", strerror(errno));
        return -1;
    } else {
        printf("fd = %d\n", fd);
    }

    if ((ret = fchmodat(fd, "xibei.cc", S_IRWXG, 0)) < 0) {
        printf("Error: %s\n", strerror(errno));
        return -1;
    }

    if ((ret = fchmodat(AT_FDCWD, "xibei.cc", S_IRWXG, 0)) < 0) {
        printf("Error: %s\n", strerror(errno));
        return -1;
    }
}
