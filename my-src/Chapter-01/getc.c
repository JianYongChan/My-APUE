#include "apue.h"

int main(void) {
    int c;

    while ((c = getc(stdin)) != EOF) {
        if (putc(c, stdout) == EOF) {
            err_sys("output error");
        }
    }

    // man 3 ferror
    // `ferror`函数为输入输出流(stream)测试error indicator
    // 如果设置了error indicator就返回非0值
    if (ferror(stdin)) {
        err_sys("input error");
    }

    exit(0);
}