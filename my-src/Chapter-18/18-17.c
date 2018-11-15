#include <signal.h>
#include <stdio.h>
#include <termios.h>

#define MAX_PASS_LEN    8   /* max #chars for user to enter */

/*
 * 为了读取口令，该函数必须关闭回显
 */
char *
getpass(const char *prompt) {
    static char     buf[MAX_PASS_LEN+1];
    char            *ptr;
    sigset_t        sig, osig;
    struct termios  ts, ots;
    FILE            *fp;
    int             c;

    // 只是读、写控制终端
    // 如果不能以读、写模式打开次设备则出错返回
    if ((fp = fopen(ctermid(NULL), "r+")) == NULL)
        return NULL;
    // 使用标准I/O读、写控制终端
    // 我们将流设置为不带缓冲的，否则在流的读、写之间可能会有某些交叉，这样就需要多次调用fflush函数
    setbuf(fp, NULL);

    // 阻塞SIGINT和SIGSTP两个信号
    // 如果不这样做，在输入INTR字符时就会使程序异常终止，并使终端仍处于禁止回显状态
    // 与此类似，输入SUSP字符时将时程序停止，并且在禁止回显状态返回到shell
    sigemptyset(&sig);
    sigaddset(&sig, SIGINT);
    sigaddset(&sig, SIGTSTP);
    sigprocmask(SIG_BLOCK, &sig, &osig); // 注意保存旧的sigset

    tcgetattr(fileno(fp), &ts); // 获取终端属性
    ots = ts; // 保存旧的属性值
    ts.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL); // 禁止回显
    tcsetattr(fileno(fp), TCSAFLUSH, &ts); // 对终端设置新的属性
    fputs(prompt, fp); // 显示提示符

    ptr = buf;
    while ((c = getc(fp)) != EOF && c != '\n')
        if (ptr < &buf[MAX_PASS_LEN])
            *ptr++ = c;
    *ptr = 0;
    putc('\n', fp);

    tcsetattr(fileno(fp), TCSAFLUSH, &ots);
    // 解除对SIGINT、SITSTP信号的阻塞
    sigprocmask(SIG_SETMASK, &osig, NULL);
    fclose(fp);
    return buf;
}
