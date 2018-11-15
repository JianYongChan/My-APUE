#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

struct devdir {
    struct devdir   *d_next;
    char            *d_name;
};

static struct devdir    *head;
static struct devdir    *tail;
static char             pathname[_POSIX_PATH_MAX+1];

/*
 * 此处用的技术是读/dev目录，寻找具有相同设备号和i-node的表项
 * 
 * 每个文件系统都有一个唯一的设备号(stat.st_dev)，
 * 文件系统中的每个目录项都有一个唯一的i-node(stat.st_ino)
 */
static void
add(char *dirname) {
    struct devdir   *ddp;
    int             len;

    len = strlen(dirname);

    /*
     * 跳过.，..和/dev/fd
     */
    if ((dirname[len-1] == '.') &&
            (dirname[len-2] == '/' || 
            (dirname[len-2] == '.' && dirname[len-3] == '/'))) {
        return;
    }
    if (strcmp(dirname, "/dev/fd") == 0)
        return;
    // 注意像devdir这样的struct，里面有指针成员
    // 先为devdir分配空间，然后为其中的d_name分配空间
    // free的时候先free掉d_name，然后释放devdir
    if ((ddp = malloc(sizeof(struct devdir))) == NULL)
        return;
    if ((ddp->d_name = strdup(dirname)) == NULL) {
        free(ddp);
        return;
    }

    // 添加到链表的尾端
    ddp->d_next = NULL;
    if (tail == NULL) {
        head = ddp;
        tail = ddp;
    } else {
        tail->d_next = ddp;
        tail = ddp;
    }
}

/*
 * 清空链表
 */
static void
cleanup(void) {
    struct devdir   *ddp, *nddp;

    ddp = head;
    while (ddp != NULL) {
        nddp = ddp->d_next;
        free(ddp->d_name);
        free(ddp);
        ddp = nddp;
    }
    head = NULL;
    tail = NULL;
}

/*
 * 在dirname所表示的目录下进行搜索
 * 碰到子目录则将其添加到链表中去
 * 这里只是将直接的下一级子目录加入链表，而不会将子目录的子目录加入链表
 * 不过在my_ttyname中，如果调用search敌人搜索/dev目录时没有直接找到，则会对链表中的每个目录项都调用searchdir，在调用过程中又会将/dev的子目录的子目录挂到链表上去
 *
 * 比如在my_ttyname中第一次调用searchdir时，dirname = "/dev"
 * 调用返回后，可能链表结构是这样的：
 *
 * [/dev/subdir1]->[/dev/subsdir2]->NULL
 *
 * 这时候检查返回值发现没有找到，那么对链表中的每个目录项进行搜索，先搜索/dev/subdir1，搜索返回后可能链表结构是这样的：
 *
 * [/dev/subdir1]->[/dev/subdir2]->[dev/subdir1/subsubdir1]->[/dev/subdir1/subsubdir2]
 *
 * 一直往下搜索，知道把所有目录找完了。
 */
static char *
searchdir(char *dirname, struct stat *fdstatp) {
    struct stat         devstat;
    DIR                 *dp;
    int                 devlen;
    struct dirent       *dirp;

    strcpy(pathname, dirname);
    if ((dp = opendir(dirname)) == NULL)
        return NULL;
    // dirname为/dev
    // 将'/'串到其后为/dev/
    strcat(pathname, "/");
    devlen = strlen(pathname);
    // readdir用来读取下一个目录项
    while ((dirp = readdir(dp)) != NULL) {
        strncpy(pathname+devlen, dirp->d_name, _POSIX_PATH_MAX-devlen);
        /*
         * /dev/stdin，/dev/stdout，/dev/stdout是/dev/fd目录中文件的符号链接
         * 需要跳过
         */
        if (strcmp(pathname, "/dev/stdin") == 0 ||
            strcmp(pathname, "/dev/stdout") == 0 ||
            strcmp(pathname, "/dev/stderr") == 0)
           continue; 
        if (stat(pathname, &devstat) < 0)
            continue;
        if (S_ISDIR(devstat.st_mode)) {
            add(pathname);
            continue;
        }
        if (devstat.st_ino == fdstatp->st_ino &&
            devstat.st_dev == fdstatp->st_dev) {
            closedir(dp);
            return pathname;
        }
    }

    closedir(dp);
    return NULL;
}

/*
 * 使用fstat获取fd所引用的设备的stat信息
 * 然后在/dev目录下面查找和给定stat信息相同的设备
 */
char *
my_ttyname(int fd) {
    struct stat     fdstat;
    struct devdir   *ddp;
    char            *rval;

    if (isatty(fd) == 0)
        return NULL;
    if (fstat(fd, &fdstat) < 0)
        return NULL;
    if (S_ISCHR(fdstat.st_mode) == 0)
        return NULL;
    rval = searchdir("/dev", &fdstat);
    if (rval == NULL) {
        for (ddp = head; ddp != NULL; ddp = ddp->d_next)
            if ((rval = searchdir(ddp->d_name, &fdstat)) != NULL)
                break;
    }

    cleanup();
    return rval;
}
