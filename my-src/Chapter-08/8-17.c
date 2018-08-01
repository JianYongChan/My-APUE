#include "apue.h"

int main(int argc, char *argv[]) {
    int           i;
    char        **ptr;
    extern char **environ;
    /* sleep(30)之后就可以看到书中提到的提示符先打印，第二个子进程后printf的情况 */
    // sleep(30);

    /* 打印所有命令行参数 */
    for (i = 0; i < argc; ++i) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }

    /* 打印所有环境变量 */
    for (ptr = environ; *ptr != 0; ptr++) {
        printf("%s\n", *ptr);
    }

    exit(0);
}
