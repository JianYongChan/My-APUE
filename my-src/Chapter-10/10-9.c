#include "apue.h"
#include <setjmp.h>

static jmp_buf env_alrm;

unsigned int sleep2(unsigned int);
static void  sig_int(int);
static void  sig_alrm(int);

int main(void) {
    unsigned int unslept;

    if (signal(SIGINT, sig_int) == SIG_ERR)
        err_sys("signal(SIGINT) error");
    unslept = sleep2(3);
    printf("sleep2 returned: %d\n", unslept);
    exit(0);
}

static void sig_int(int signo) {
    int          i, j;
    volatile int k; /* 将k声明为volatile阻止编译程序去除循环语句 */

    printf("\nsig_int starting\n");
    for (int i = 0; i < 300000; ++i)
        for (j = 0; j < 4000; ++j)
            k += i*j;
    printf("sig_int finished\n");
}

unsigned int sleep2(unsigned int seconds) {
    if (signal(SIGALRM, sig_alrm) == SIG_ERR)
        return (seconds);
    if (setjmp(env_alrm) == 0) {
        alarm(seconds);
        pause();
    }

    return alarm(0);
}

static void sig_alrm(int signo) {
    // 第二个参数为跳转到setjmp处时setjmp返回的值
    longjmp(env_alrm, 1);
}