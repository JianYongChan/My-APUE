#include "apue.h"
#include <stdio.h>
#include <fcntl.h>

int main() {
    int fd;
    char buf1[] = "abcdefghijk";
    char buf2[] = "ABCDEFGHIJK";

    /* 以只读方式打开 */
    if ((fd = open("lover2.txt", O_RDONLY | O_CREAT)) < 0) {
        err_sys("open error");
    } else {
        printf("success!\n");
    }

    if (write(fd, buf1, strlen(buf1)) < strlen(buf1)) {
        err_sys("write error");
    }
    printf("buf1 = %s\n", buf1);
    printf("len of buf1 = %lu\n", strlen(buf1));
}