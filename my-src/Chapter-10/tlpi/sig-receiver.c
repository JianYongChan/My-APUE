#include "apue.h"

#define NSIG 32

extern void print_sigset(FILE *, const char *, const sigset_t *);

static int sig_cnt[NSIG];

static volatile sig_atomic_t got_sigint = 0;

static void
sig_handler(int signo)
{
    if (signo == SIGINT)
        got_sigint = 1;
    else
        sig_cnt[signo]++;
}

int
main(int argc, char **argv)
{
    int         i, num_secs;
    sigset_t    pending_mask, blocking_mask, empty_mask;

    printf("%s: PID is %ld\n", argv[0], (long)getpid());

    // 为每个信号都设置同一个处理函数
    for (i = 1; i < NSIG; i++)
        (void)signal(i, sig_handler);

    // 第二个命令行参数表示要睡眠的时间
    if (argc > 1) {
        num_secs = atoi(argv[1]);

        // 阻塞所有信号，当然SIGKILL和SIGSTOP是怎么都没法阻塞的
        sigfillset(&blocking_mask);
        if (sigprocmask(SIG_SETMASK, &blocking_mask, NULL) < 0)
            err_sys("blocking signals error");

        printf("%s: sleeping for %d seconds\n", argv[0], num_secs);
        sleep(num_secs);

        // 获取到睡眠期间发送方发来的信号(但是是处于待决状态)
        if (sigpending(&pending_mask) < 0)
            err_sys("get pending signals error");

        printf("%s: pending signals are: \n", argv[0]);
        print_sigset(stdout, "\t\t", &pending_mask);

        // 对所有信号都取消阻塞
        sigemptyset(&empty_mask);
        if (sigprocmask(SIG_SETMASK, &empty_mask, NULL) < 0)
            err_sys("clear blocking signal mask error");
    }

    // 直到捕捉到SIGINT才退出
    while (!got_sigint)
        continue;

    // 打印接收到的信号和次数
    for (i = 1; i < NSIG; i++)
        if (sig_cnt[i] != 0)
            printf("%s: signal %d caught %d time%s\n", argv[0], i, sig_cnt[i],
                    argv[i]==0 ? "" : "s");

    exit(EXIT_SUCCESS);
}
