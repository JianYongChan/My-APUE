#include "apue.h"

int main(void) {
    char name[L_tmpnam], line[MAXLINE];
    FILE *fp;

    // 产生一个文件名并打印
    printf("%s\n", tmpnam(NULL));

    tmpnam(name);
    printf("%s\n", name);

    // 创建临时文件
    if ((fp = tmpfile()) == NULL) {
        err_sys("tmpfile error");
    }
    fputs("one line of output\n", fp);
    // 将流设置到文件起始位置
    rewind(fp);
    if (fgets(line, sizeof(line), fp) == NULL) {
        err_sys("fges error");
    }
    fputs(line, stdout);

    // 去/tmp文件夹下查看是否有file*临时文件
    // 当然是看不到的，因为创建文件后会立即unlink它
    sleep(15);

    exit(0);
}