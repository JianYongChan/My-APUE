#include "apue.h"

#define BSZ 48

int main(void) {
    FILE *fp;
    char buf[BSZ];

    memset(buf, 'a', BSZ-2);
    buf[BSZ-2] = '\0';
    buf[BSZ-1] = 'X';
    // 把文件截断至0长度，为读和写打开
    if ((fp = fmemopen(buf, BSZ, "w+")) == NULL)
        err_sys("fmemopen error");
    printf("initial buffer contents: %s\n", buf);
    fprintf(fp, "hello, world");
    printf("before flush: %s\n", buf);
    // 流冲洗后缓冲区才会变化
    fflush(fp);
    // 由于截断了文件，所以buf应该为"hello, world"
    printf("after flush: %s\n", buf);
    printf("len of string in buf = %ld\n", (long)strlen(buf));

    memset(buf, 'b', BSZ-2);
    buf[BSZ-2] = '\0';
    buf[BSZ-1] = 'X';
    fprintf(fp, "hello, world");
    // fseek到了文件开头，同时fseek也会引起缓冲区冲洗
    fseek(fp, 0, SEEK_SET);
    printf("after fseek: %s\n", buf);
    printf("len of string in buf = %ld\n", (long)strlen(buf));

    memset(buf, 'c', BSZ-2);
    buf[BSZ-2] = '\0';
    buf[BSZ-1] = 'X';
    fprintf(fp, "hello, world");
    fclose(fp);
    // 由于fseek到缓冲区开头再写入
    // 所以此时"hello, world"在前面，后面是一串"c"
    printf("after close: %s\n", buf);
    printf("len of string in buf = %ld\n", (long)strlen(buf));

    return (0);
}