#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

int main(void) {
    time_t t;
    struct tm *tmp;
    char buf1[16];
    char buf2[64];

    time(&t);
    tmp = localtime(&t);
    // 若buf空间足够，则返回存入buf的字节数
    // 否则返回0
    // %r: 本地时间(12小时制), %a: 缩写的weekday, %b: 缩写的月份名
    // %d: monthday, %Y: 年份
    if (strftime(buf1, 16, "time and date: %r, %a %b %d, %Y", tmp) == 0)
        printf("buffer length 16 is too small\n");
    else
        printf("%s\n", buf1);
    if (strftime(buf2, 64, "time and date: %r, %a %b %d, %Y", tmp) == 0)
        printf("buffer length 64 is too small\n");
    else
        printf("%s\n", buf2);

    exit(0);
}
