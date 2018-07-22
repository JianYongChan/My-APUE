#include "apue.h"

int main(void) {
    // 每行最大长度
    char buf[MAXLINE];
    while (fgets(buf, MAXLINE, stdin) != NULL) {
        if (fputs(buf, stdout) == EOF) {
            err_sys("output error");
        }
    }
    
    // 区分出错还是达到文件尾端
    if (ferror(stdin)) {
        err_sys("input error");
    }

    exit(0);
}