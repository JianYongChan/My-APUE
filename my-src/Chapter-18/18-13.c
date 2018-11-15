#include <termios.h>

int
isatty(int fd) {
    struct termios  ts;

    // 使用一个终端专用函数检测fd是否引用了一个终端
    return (tcgetattr(fd, &ts) != -1);
}
