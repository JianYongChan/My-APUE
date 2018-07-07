#include "apue.h"
#include <errno.h>

int main(int argc, char *argv[]) {
    fprintf(stderr, "EACCES: %s\n", strerror(EACCES)); /* Permission denied */
    errno = ENOENT;  /* No such file or directory */
    // 将程序名作为参数传递给`perror`
    // 这是一个标准的Unix惯例
    // 使用这种方法时，在程序作为管道的一部分时
    // exe < inputfile | exe2 | exe3 > outputfile
    // 就可以找出是哪个程序产生了错误
    perror(argv[0]);
    exit(0);
}
