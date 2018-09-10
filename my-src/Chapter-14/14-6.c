#include "apue.h"
#include <fcntl.h>


/*
 * 如果有一把锁，它阻塞由参数制定的锁请求
 * 则此函数返回持有这把锁的进程的pid
 * 否则此函数返回0
 *
 * 需要注意的是进程不能使用此函数测试它自己是否在文件的某一部分持有一把锁
 * 因为F_GETLK返回信息提示是否有现有的锁阻止调用进程设置它自己的锁
 * 而如果是进程对自己已经设置了的锁再次进行设置(F_SETLK, F_SETLKW)，则会替换，而不会被阻塞
 * 所以F_GETLK命令决不会报告调用进程持有自己的锁
 */
pid_t
lock_test(int fd, int type, off_t offset, int whence, off_t len) {
    struct flock lock;

    lock.l_type = type;
    lock.l_start = offset;
    lock.l_whence = whence;
    lock.l_len = len;

    if (fcntl(fd, F_GETLK, &lock) < 0)
        err_sys("fcntl error");

    if (lock.l_type == F_UNLCK)
        return 0;

    return lock.l_pid;
}
