#include "apue.h"
#include <sys/resource.h>

// #是ISO C的字符串创建运算符
// 在这里用以为每个资源名产生字符串值
// doit(RLIMIT_CORE)被扩展为pr_limits("RLIMIT_CORE", RLIMIT_CORE);
#define     doit(name)  pr_limits(#name, name)

static void pr_limits(char*, int);

int main(void) {
#ifdef RLIMIT_AS
    /* 进程总的可用存储空间的最大长度(字节) */
    doit(RLIMIT_AS);
#endif

    /* core文件的最大字节数 */
    doit(RLIMIT_CORE);
    /* CPU时间的最大量值(秒) */
    doit(RLIMIT_CPU);
    /* 数据段的最大字节长度 */
    doit(RLIMIT_DATA);
    /* 可用创建的文件的最大字节长度 */
    doit(RLIMIT_FSIZE);

#ifdef RLIMIT_MEMLOCK
    /* 一个进程使用`mlock`能够锁定在存储空间中的最大字节长度 */
    doit(RLIMIT_MEMLOCK);
#endif

#ifdef RLIMIT_MSGQUEUE
    /* 进程为POSIX消息队列可分配的最大存储字节数 */
    doit(RLIMIT_MSGQUEUE);
#endif

#ifdef RLIMIT_NICE
    /* 为了影响进程的调度优先级，nice值可设置的最大限制 */
    doit(RLIMIT_NICE);
#endif

    /* 每个进程能打开的最大文件数 */
    doit(RLIMIT_NOFILE);

#ifdef RLIMIT_NPROC
    /* 每个实际用户id可拥有的最大子进程数 */
    doit(RLIMIT_NPROC);
#endif

#ifdef RLIMIT_NPTS
    /* 用户可同时打开的伪终端的最大数量 */
    doit(RLIMIT_NPTS);
#endif

#ifdef RLIMIT_RSS
    /* 最大驻内存集字节长度 */
    doit(RLIMIT_RSS);
#endif

#ifdef RLIMIT_SBSIZE
    /* 任一时刻，一个用户可以占用的套接字缓冲区的最大长度 */
    doit(RLIMIT_SBSIZE);
#endif

#ifdef RLIMIT_SIGPENDING
    /* 一个进程可排队的信号最大数量 */
    doit(RLIMIT_SIGPENDING);
#endif

#ifdef RLIMIT_SWAP
    /* 用户可消耗的交换空间的最大字节数 */
    doit(RLIMIT_SWAP);
#endif

    /* 栈的最大字节长度 */
    doit(RLIMIT_STACK);

#ifdef RLIMIT_VMEM
    /* RLIMIT_AS的同义词 */
    doit(RLIMIT_VMEM);
#endif

    exit(0);
}

static void pr_limits(char *name, int resource) {
    struct rlimit      limit;
    unsigned long long lim;

    if (getrlimit(resource, &limit) < 0)
        err_sys("getrlimit error");
    // -表示左对其
    printf("%-17s  ", name);
    if (limit.rlim_cur == RLIM_INFINITY) {
        printf("(infinite)  ");
    }
    else {
        lim = limit.rlim_cur;
        printf("%10lld  ", lim);
    }
    if (limit.rlim_max == RLIM_INFINITY) {
        printf("(infinite)");
    } else {
        lim = limit.rlim_max;
        printf("%10lld", lim);
    }
    putchar((int)'\n');
}
