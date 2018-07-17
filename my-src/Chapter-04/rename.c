#include "apue.h"
#include <errno.h>

int main(void) {
    // 文件名不能改为已经存在的目录名
    if (rename("file", "dir") < 0) {
        printf("Error: %s\n", strerror(errno));
    }

    // 目录不能改为已经存在的文件名
    if (rename("dir", "file") < 0) {
        printf("Error: %s\n", strerror(errno));
    }

    // 新目录不能以旧目录作为路径前缀
    if (rename("dir/test", "dir/test/test-2") < 0) {
        printf("Error: %s\n", strerror(errno));
    }

}