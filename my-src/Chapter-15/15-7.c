#include "apue.h"

/*
 * 在调用fork之前创建两个管道
 * 父进程在调用TELL_CHILD时经由上一个管道写一个字符p
 * 子进程在调用TELL_PARENT时经由下一个管道写一个字符c
 * 相应的WAIT_XXX函数调用read读取一个字符，没有读到字符则阻塞
 * |-------------|            |-------------|
 * |    PARENT   |            |    CHILD    |
 * |-------------|            |-------------|
 * |             |     p      |             |
 * |   pfd1[1]   |----------->|   pfd1[0]   |
 * |             |     c      |             |
 * |   pfd2[0]   |<-----------|   pfd2[1]   |
 * |-------------|            |-------------|
 */

static int  pfd1[2], pfd2[2];

void
TELL_WAIT(void) {
    if (pipe(pfd1) < 0 || pipe(pfd2) < 0)
        err_sys("pipe error");
}

void
TELL_PARENT(pid_t pid) {
    if (write(pfd2[1], "c", 1) != 1)
        err_sys("write error");
}

void
WAIT_PARENT(void) {
    char c;

    if (read(pfd1[0], &c, 1) != 1)
        err_sys("read error");

    if (c != 'p')
        err_quit("WAIT_PARENT: incorrect data");
}

void
TELL_CHILD(pid_t pid) {
    if (write(pfd1[1], "p", 1) != 1)
        err_sys("write error");
}

void
WAIT_CHILD(void) {
    char    c;

    if (read(pfd2[0], &c, 1) != 1)
        err_sys("read error");

    if (c != 'c')
        err_quit("WAIT_CHILD: incorrect data");
}
