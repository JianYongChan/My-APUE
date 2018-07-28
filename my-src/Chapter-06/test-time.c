#include "apue.h"
#include <time.h>
#include <sys/time.h>

int main(void) {
    time_t t = time(NULL);
    printf("time: %lu\n", t);

    struct timespec spec;
    if (clock_gettime(CLOCK_REALTIME, &spec) < 0)
        err_sys("clock_gettime error");
    printf("nsec: %lu\n", spec.tv_nsec);
    printf("sec: %lu\n", spec.tv_sec);

    struct tm *tm_ptr;
    if ((tm_ptr = gmtime(&t)) == NULL)
        err_sys("gmtime error");
    printf("sec: %d\n", tm_ptr->tm_sec);
    printf("min: %d\n", tm_ptr->tm_min);
    printf("hour: %d\n", tm_ptr->tm_hour);
    printf("mday: %d\n", tm_ptr->tm_mday);
    printf("mon: %d\n", tm_ptr->tm_mon);
    printf("year: %d\n", tm_ptr->tm_year);
    printf("wday: %d\n", tm_ptr->tm_wday);
    printf("tm_yday: %d\n", tm_ptr->tm_yday);
    printf("isdst: %d\n", tm_ptr->tm_isdst);

    /* localtime将日历时间转换为本地时间 */
    if ((tm_ptr = localtime(&t)) == NULL) 
        err_sys("localtime error");
    printf("sec: %d\n", tm_ptr->tm_sec);
    printf("min: %d\n", tm_ptr->tm_min);
    printf("hour: %d\n", tm_ptr->tm_hour);
    printf("mday: %d\n", tm_ptr->tm_mday);
    printf("mon: %d\n", tm_ptr->tm_mon);
    printf("year: %d\n", tm_ptr->tm_year);
    printf("wday: %d\n", tm_ptr->tm_wday);
    printf("tm_yday: %d\n", tm_ptr->tm_yday);
}
