/*
 * 本来是打算：
 * 临界区代码内阻塞SIGINT，
 * 完成临界区代码后进程就pause，
 * 以等待刚才被阻塞的SIGINT(如果刚才发了SIGINT的话)解除阻塞后唤醒该进程，
 * 但是使用sigprocmask解除阻塞和pause使进程休眠这两个动作不是不可分割的，
 * 所以有可能刚才被阻塞的SIGINT在这两个动作之间就递送了过来，
 * 那么就没有起到唤醒被pause的进程的功能
 */

#include "apue.h"
#include <errno.h>
#include <string.h>

int main(void) {
    sigset_t newmask, oldmask;

    sigemptyset(&newmask);
    sigaddset(&newmask, SIGINT);

    /* 阻塞SIGINT并且保存当前信号屏蔽字 */
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
        printf("SIG_BLOCK error: %s\n", strerror(errno));

    /* 临界区代码 */
    
    /* 恢复信号原来的屏蔽字 */
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        printf("SIG_SETMASK error: %s\n", strerror(errno));

    pause();    /* 等待信号产生 */
}
