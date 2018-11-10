#include "apue.h"
#include <termios.h>

int
main(void) {
    struct termios  term;
    long            vdisable;

    // 仅当标准输入是终端设备时才修改终端特殊字符
    if (isatty(STDIN_FILENO) == 0)
        err_quit("standard input is not a terminal device");

    // 获取_POSIX_VDISABLE值
    if ((vdisable = fpathconf(STDIN_FILENO, _PC_VDISABLE)) < 0)
        err_quit("fpathconf error or _POSIX_VDISABLE not in effect");

    // 获取termios结构
    if (tcgetattr(STDIN_FILENO, &term) < 0)
        err_sys("tcgetattr error");

    // 禁用中断
    // 注意这和忽略中断信号是不同的
    // 这里只是禁用使终端驱动程序产生SIGINT信号的特殊字符(control + C)
    // 但是我们仍然可以使用kill函数将信号发送至进程
    term.c_cc[VINTR] = vdisable;
    term.c_cc[VEOF]  = 2;

    // 设置属性
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0)
        err_sys("tcsetattr error");

    exit(0);
}
