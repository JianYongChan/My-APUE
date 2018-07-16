#include "apue.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int main(void) {
    int dir_fd;

    if ((dir_fd = open("/home/ctom", O_EXCL)) < 0) {
        err_quit("open failed");
    }

    /* 分别使用普通用户和root对xibei.cc进行修改，只有root成功 */
    if ((chown("/home/ctom/xibei.cc", 0, 0)) < 0) {
        printf("Error: %s\n", strerror(errno));
    }

    /* 修改符号链接本身而不是其所指向的文件 */
    if ((lchown("/home/ctom/xibei.cc.slink", 1000, 1000)) < 0) {
        printf("Error: %s\n", strerror(errno));
    }

    /* 工作目录并不是fd指向的目录 */
    if ((fchownat(dir_fd, "xibei.cc", 1000, 1000, AT_SYMLINK_NOFOLLOW)) < 0) {
        printf("Error: %s\n", strerror(errno));
    }

    exit(0);
}
