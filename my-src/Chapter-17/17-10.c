#include "apue.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#define     CLI_PATH        "/var/tmp/"
#define     CLI_PERM        S_IRWXU     /* 用户读、写和执行 */

int
cli_conn(const char *name) {
    int                 fd, len, err, rval;
    struct sockaddr_un  un, sun; 
    int                 do_unlink = 0;

    if (strlen(name) >= sizeof(un.sun_path)) {
        err = ENAMETOOLONG;
        return -1;
    }

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        return -1;

    // 填写client的地址信息
    // 注意无需填写某些平台(比如macOS)提供的sun_len字段
    // 因为OS会用传送给bind函数的地址长度参数来填写
    memset(&un, 0, sizeof(un));
    un.sun_family = AF_UNIX;
    // 文件名格式: /var/tmp/uid
    sprintf(un.sun_path, "%s%05ld", CLI_PATH, (long)getpid());
    len = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);

    unlink(un.sun_path); /* 如果文件已经存在 */
    /*
     * bind将名字赋给client套接字
     * 这样就在文件系统中创建文件来表示这个套接字
     * 所以这之后处理失败需要unlink
     */
    if (bind(fd, (struct sockaddr*)&un, len) < 0) {
        rval = -2;
        goto errout;
    }
    // 设置socket文件权限为仅用户读、写和执行
    if (chmod(un.sun_path, CLI_PERM) < 0) {
        rval = -3;
        do_unlink = 1;
        goto errout;
    }

    // 填写server的地址信息
    // 此处使用的是server进程众所周知的路径名
    memset(&sun, 0, sizeof(sun));
    sun.sun_family = AF_UNIX;
    strcpy(sun.sun_path, name);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(name);
    if (connect(fd, (struct sockaddr*)&sun, len) < 0) {
        rval = -4;
        do_unlink = 1;
        goto errout;
    }
    return fd;

errout:
    err = errno;
    close(fd);
    if (do_unlink)
        unlink(un.sun_path);
    errno = err;
    return rval;
}
