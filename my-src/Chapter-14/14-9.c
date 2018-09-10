#include <unistd.h>
#include <fcntl.h>

int
lockfile(int fd) {
    struct flock fl;

    fl.l_type = F_WRLCK;
    /*
     * l_start为0，l_whence为SEEK_SET，指定文件起始位置
     * 再加上l_len为0，则可以为整个文件加锁
     */
    fl.l_start = 0; // 文件开始
    fl.l_whence = SEEK_SET; // 文件开始
    fl.l_len = 0; // 表示锁的范围可以扩展到最大可能偏移量
    
    return (fcntl(fd, F_SETLK, &fl));
}
