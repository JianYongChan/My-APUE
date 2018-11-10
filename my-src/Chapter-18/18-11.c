#include "apue.h"
#include <termios.h>

int
main(void) {
    struct termios  term;
    
    if (tcgetattr(STDIN_FILENO, &term) < 0)
        err_sys("tcgetattr error");

    // CSIZE是一个屏蔽字标志，它指定发送和接收的每个字节的位数
    switch(term.c_cflag & CSIZE) {
        case CS5:
            printf("5 bits/byte\n");
            break;
        case CS6:
            printf("6 bits/byte\n");
            break;
        case CS7:
            printf("7 bits/byte\n");
            break;
        case CS8:
            printf("8 bits/byte\n");
            break;
        default:
            printf("unknown bytes/byte\n");
    }

    term.c_cflag &= ~CSIZE;     /* 将其置0 */
    term.c_cflag |= CS8;        /* 设置为8 bits/byte */
    if (tcsetattr(STDIN_FILENO, TCSANOW, &term) < 0)
        err_sys("tcsetattr error");

    exit(0);
}
