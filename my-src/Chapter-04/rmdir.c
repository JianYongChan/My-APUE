#include "apue.h"
#include <errno.h>

int main(void) {
    // rmdir只能删除空目录
    // 空目录指的是只有.和..的目录
    if (rmdir("Bar") < 0) {
        printf("error: %s\n", strerror(errno));
    }

    if (rmdir("foo") < 0) {
        printf("error: %s\n", strerror(errno));
    }
}