#include "apue.h"

static void charatatime(char *str) {
    char *ptr;
    int c;
    
    /*
     * 将标准输出设置为不带缓冲的
     * 于是对每个字符都需要调用一次write
     * 这样就可以使内核尽可能多地在子进程和父进程之间切换
     * 以演示竞争条件
     */
    setbuf(stdout, NULL);
    for (ptr = str; (c = *ptr++)!= 0; )
        putc(c ,stdout);
}

static void test_1() {
    pid_t pid;

    /*
     * 在TELL_WAIT中设置捕获并且阻塞SIGUSR1和SIGUSR2
     * 同时子进程会继承父进程的信号屏蔽字
     */
    TELL_WAIT();

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid == 0) { /* 子进程 */ 
        /*
         * 发送SIGUSR2给父进程
         * 由于在WIAT_PARENT中检测sigflag是否为0，如果为0则sigsuspend解除对SIGUSR1,SIGUSR2的阻塞
         */
        WAIT_PARENT();
        charatatime("output from child\n");
        
    } else { /* 父进程 */
        charatatime("output from parent\n");
        /*
         * 在TELL_CHILD中父进程给子进程发送SIGUSR1信号
         * 由于子进程处于挂起状态，而且解除了对SIGUSR1的阻塞(以及SIGUSR2)
         * 所以子进程捕捉到SIGUSR1之后进入处理函数sig_usr
         * 并在其中改变sigflag
         * 所以子进程不再挂起，而是继续运行
         */
        TELL_CHILD(pid);
    }
    exit(0);
}

static void test_2() {
    pid_t pid;

    /*
     * 在TELL_WAIT中设置捕获并且阻塞SIGUSR1和SIGUSR2
     * 同时子进程会继承父进程的信号屏蔽字
     */
    TELL_WAIT();

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid == 0) { /* 子进程 */ 
        /*
         * 发送SIGUSR2给父进程
         */
        charatatime("output from child\n");
        TELL_PARENT(getppid());
        
    } else { /* 父进程 */
        WAIT_CHILD();
        charatatime("output from parent\n");
    }
    exit(0);
}

int main(void) {
#if 1
    /* 父进程先，子进程后 */
    test_1();
#else
    /* 子进程先，父进程后 */
    test_2();
#endif

    exit(0);
}
