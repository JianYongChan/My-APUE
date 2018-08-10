/* 对于SIGCLD信号，如果将其设置为捕捉，
 * 则内核**立即**检查是否有子进程准备好被等待，
 * 如果有，则调用SIGCLD处理程序。
 * 在此例程中，因为接收到了SIGCLD信号所以进入信号处理程序，
 * 在信号处理程序开始处中重新建立了对于SIGCLD的处理程序，
 * 此时内核检查是否有子进程准备好被等待，由于我们正在处理一个SIGCLD信号，且尚未调用wait，
 * 所以又会产生另一个对信号处理程序的调用，就这样递归下去
 * 解决方法是在wait之后再重新建立SIGCLD信号处理程序
 * 
 * 但是在FreeBSD 8.0, Mac OS X 10.6.8, Linux3.2.0没有出现此问题
 * 这是因为虽然SIGCLD和SIGCHLD被定义为相同的值，但是当一个进程安排捕捉SIGCHLD，
 * 并且已经有进程准备好由其父进程等待时，该系统并不调用SIGCHLD的处理程序。
 */


#include "apue.h"
#include <sys/wait.h>

static void sig_cld(int);

int main(void) {
    pid_t pid;

    if (signal(SIGCLD, sig_cld) == SIG_ERR)
        perror("signal error");
    if ((pid = fork()) < 0) {
        perror("fork error");
    } else if (pid == 0) { /* 子进程 */
        sleep(2);
        _exit(0);
    }

    pause(); /* 父进程 */
    exit(0);
}

static void sig_cld(int signo) {
    pid_t pid;
    int   status;

    printf("SIGCLD received\n");

    // 由于进程在每次接到信号对其进行处理时，都会将该信号重置为默认值
    // 所以需要重新建立信号处理例程
    if (signal(SIGCLD, sig_cld) == SIG_ERR)
        perror("signal error");
    
    if ((pid = wait(&status)) < 0) /* 获取子进程状态 */
        perror("wait error");
    
    printf("pid = %d\n", pid);
}