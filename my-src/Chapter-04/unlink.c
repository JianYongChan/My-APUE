#include "apue.h"
#include <fcntl.h>

int main(void) {
    if (open("tempfile", O_RDWR) < 0) {
        err_sys("open error");
    }
    if (unlink("tempfile") < 0) {
        err_sys("unlink error");
    }
    printf("file unlinked\n");
    // `unlink`了tempfile之后，该进程仍然持有该文件
    // 所以该文件内容并不会被删除
    sleep(15);
    // 此时文件的引用计数为0，而且没有进程打开该文件
    // 所以文件内容被删除
    printf("done");
    exit(0);
}