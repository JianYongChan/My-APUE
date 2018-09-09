#include "apue.h"
#include <pthread.h>
#include <syslog.h>

sigset_t mask;

extern int algready_running(void);
extern void daemonize(const char *);

void
reread(void) {
    /* 重读其配置文件 */
}

void *
thr_fn(void *arg) {
    int err, signo;

    for ( ; ;  ) {
        err = sigwait(&mask, &signo);
        if (err != 0) {
            syslog(LOG_ERR, "sigwait failed");
            exit(1);
        }
        switch (signo) {
            case SIGHUP:
                syslog(LOG_INFO, "Re-reading configureation file");
                reread();
                break;
            case SIGTERM:
                syslog(LOG_INFO, "got SIGTERM; exsiting");
                exit(0);
            default:
                syslog(LOG_INFO, "unexptected signal %d\n", signo);
        }
    }

    return 0;
}

int
main(int argc, char *argv[]) {
    int              err;
    pthread_t        tid;
    char             *cmd;
    struct sigaction sa;

    if ((cmd = strrchr(argv[0], '/')) == NULL)
        cmd = argv[0];
    else
        cmd++;

    // 使之成为守护进程
    daemonize(cmd);


    if (algready_running()) {
        syslog(LOG_ERR, "daemon algready running");
        exit(1);
    }

    /*
     * 阻塞所有信号，然后创建一个线程用于处理线程
     * 该线程的唯一工作就是等待SIGHUP和SIGTERM
     * 当接收到SIGHUP时，该线程重读其配置文件
     * 当接收到SIGTERM时，会记录消息并退出
     */
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
        err_quit("%s: cannot restore SIGHUP default");
    sigfillset(&mask); // 添加所有的信号至信号屏蔽字
    if ((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0)
        err_exit(err, "SIG_BLOCK error");

    err = pthread_create(&tid, NULL, thr_fn, 0);
    if (err != 0)
        err_exit(err, "cannot creat ethread");

    exit(0);
}
