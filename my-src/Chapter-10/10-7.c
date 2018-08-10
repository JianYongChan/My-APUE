/* 使用pause和alarm模拟sleep
 * 但是这种简单实现有3个问题
 * 1. 如果在调用sleep1之前设置了alarm(并且没有超时)，则该设置会被sleep1中的第一次alarm所擦除。
 *    所以在sleep1中第一次alarm时就要记录其返回值left，
 *    如果left小于本次seconds，说明sleep1之前的alarm会在sleep1睡眠的seconds秒内会超时，
 *    所以其实sleep1无需睡眠seconds，而只需要睡眠left秒
 *    如果大于，则说明sleep1外设置的alarm不会打扰sleep1所产生的睡眠，
 *    所以需要在sleep1返回之前重新设置闹钟，使其在预定时间(即sleep1前的alarm设定的时间)超时
 * 2. sleep1修改了对SIGALRM信号的处理方案，可能原来对SIGALRM信号是默认，现在却变成了捕捉(捕捉处理例程为sig_alrm)
 * 3. 在sleep1中第一次调用alarm和pause有竞争条件。
 *    在一个繁忙的系统中，对于sleep1，可能alarm会在pause之前超时并调用了sig_alrm，那么在调用pause之后，调用者将永远被挂起
 */
#include <signal.h>
#include <unistd.h>

static void sig_alrm(int signo) {

}

unsigned int sleep1(unsigned int seconds) {
    if (signal(SIGALRM, sig_alrm) == SIG_ERR)
        return (seconds);
    alarm(seconds);
    pause();
    return (alarm(0));
}

int main(void) {
    sleep1(7);
}