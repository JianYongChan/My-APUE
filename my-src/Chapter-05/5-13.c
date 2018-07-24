#include "apue.h"
#include <errno.h>

void make_temp(char *template);

int main(void) {
    // template的后6位必须是"XXXXXX"
    // 而且由于template会被修改，所以它应该是一个字符数组，而不是字符串常量
    char    good_template[] = "/tmp/dirXXXXXX";
    char    *bad_template = "/tmp/dirXXXXXX";

    printf("trying to create first temp file...\n");
    make_temp(good_template);
    printf("trying to create second temp file...\n");
    // 由于试图修改字符串常量，所以会segmentation fault
    make_temp(bad_template);

    exit(0);
}

void make_temp(char *template) {
    int         fd;
    struct stat sbuf;

    if ((fd = mkstemp(template)) < 0)
        err_sys("can't create temp file");
    printf("temp name = %s\n", template);
    close(fd);
    // 不同于tempfile函数，mkstemp函数创建的临时文件不会自动删除
    // 需要自己unlink
    if (stat(template, &sbuf) < 0) {
        // ENOENT: no such file or directory
        if (errno == ENOENT)
            printf("file doesn't exist");
        else
            err_sys("stat failed");
    } else {
        printf("file exists\n");
        unlink(template);
    }
}