#include "apue.h"
#include <time.h>

int main() {
    time_t t;
    struct tm *tmp;
    char buf[64];

    t = time(NULL);
    if ((tmp = localtime(&t)) == NULL)
        err_sys("localtime error");
    if (strftime(buf, 64, "%a %b %d %H:%M:%S %Z %Y", tmp) == 0)
        err_sys("strftime error");
    printf("%s\n", buf);
}
