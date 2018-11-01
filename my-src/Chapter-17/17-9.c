#include "apue.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <errno.h>

#define STALE 30

// 在这个函数中需要对套接字文件以及用户做一些检验
// 如果通过了所有这些检验，则可认为客户进程的身份是该套接字的所有者
// 尽管这种方法并不完善，但这是对当前系统所能做到的最佳方案
int
serv_accept(int listenfd, uid_t *uidptr) {
    int                 clifd, err, rval;
    socklen_t           len;
    time_t              staletime;
    struct sockaddr_un  un;
    struct stat         statbuf;
    char                *name;

    if ((name = malloc(sizeof(un.sun_path) + 1)) == NULL)
        return -1;
    len = sizeof(un);
    if ((clifd = accept(listenfd, (struct sockaddr*)&un, &len)) < 0) {
        free(name);
        return -2;
    }

    len -= offsetof(struct sockaddr_un, sun_path); /* pathname长度 */
    memcpy(name, un.sun_path, len);
    name[len] = 0;
    if (stat(name, &statbuf) < 0) {
        rval = -3;
        goto errout;
    }

#ifdef S_ISSOCK
    // 验证该文件的确是一个套接字文件
    if (S_ISSOCK(statbuf.st_mode) == 0) { /* SVR4并没有定义 S_ISSOCK*/
        rval = -4;
        goto errout;
    }
#endif

    // 验证该文件的确只能为用户所读、写以及执行
    if ((statbuf.st_mode & (S_IRWXG | S_IRWXO)) ||
        (statbuf.st_mode & S_IRWXU) != S_IRWXU) {
        rval = -5; /* not rwx------ */
        goto errout;
    }

    // 验证与套接字相关联的3个时间参数不比当前时间早30s
    staletime = time(NULL) - STALE;
    if (statbuf.st_atime < staletime ||
        statbuf.st_ctime < staletime ||
        statbuf.st_mtime < staletime) {
        rval = -6; /* i-node is too old */
        goto errout;
    }

    if (uidptr != NULL)
        *uidptr = statbuf.st_uid;
    unlink(name);
    free(name);
    return (clifd);

errout:
    err = errno;
    close(clifd);
    free(name);
    errno = err;
    return (rval);
}
