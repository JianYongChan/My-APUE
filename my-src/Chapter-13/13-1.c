#include "apue.h"
#include <syslog.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>

void
daemonize(const char *cmd) {
    int               i, fd0, fd1, fd2;
    pid_t             pid;
    struct rlimit     rl;
    struct sigaction  sa;

    /*
     * 继承而来的mask可能不合自己的要求
     */
    umask(0);

    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
        err_quit("%s cannot get file limit", cmd);

    /*
     * fork之后退出父进程，子进程则继续运行
     * 由于子进程是fork新得到的pid，所以他不可能已经是某个进程组的组长了
     * 所以它可以创建会话
     */
    if ((pid = fork()) < 0)
        err_quit("%s: cannot fork", cmd);
    else if (pid != 0) /* 父进程 */
        exit(0);
    setsid();

    /*
     * POSIX.1规定要向新孤儿进程组中的处于停止状态的每个进程发送SIGHUP信号
     * 而SIGHUP默认终止进程
     */
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
        err_quit("%s: cannot ignore SIGHUP", cmd);
    if ((pid = fork()) < 0)
        err_quit("%s: cannot fork", cmd);
    else if (pid == 0) /* 父进程 */
        exit(0);

    /*
     * 防止从父进程处继承而来的当前目录在一个挂载的文件系统中
     * 从而导致该文件系统不可卸载
     */
    if (chdir("/") < 0)
        err_quit("%s: cannot change directory to /", cmd);

    /*
     * 关闭所有从父进程继承而来的文件描述符
     * 可以先找到最高文件描述符值，再关闭直到该值的所有fd
     */
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (i = 0; i < rl.rlim_max; ++i)
        close(i);

    /*
     * 由于daemon并不与任何终端设备相关联，所以其输出无从显示
     * 也无法从交互式用户那里接收输入。
     * 即使daemon是从交互式回话启动的，但是由于它是后台运行的，
     * 所以登录会话的终止并不影响daemon。
     * 如果其他用户在同一终端设备上登录，我们不希望在该终端上见到守护进程的输出，
     * 也不希望他们在终端上的输入被daemon读取
     * 这样可以打开/dev/null，使其具有fd 0 1 2
     * 这样，任何一个试图读标准输入，写标准输出或标准错误的库例程都不会产生任何效果
     */
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 | fd1 != 1 | fd2 != 2) {
        syslog(LOG_ERR, "unexpected file descriptors %d %d %d", fd0, fd1, fd2);
        exit(1);
    }
}
