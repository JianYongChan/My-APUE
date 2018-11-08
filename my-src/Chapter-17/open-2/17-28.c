#include "17-26.h"
#include <syslog.h>

int     debug, oflag, client_size, log_to_stderr;
char    errmsg[MAXLINE];
char    *pathname;
Client  *client = NULL;

int
main(int argc, char *argv[]) {
    int     c;

    log_open("open.serv", LOG_PID, LOG_USER);

    // opterr是getopt函数支持的4个外部变量之一
    // 如果一个选项发生了错误，getopt默认会打印一条出错信息
    // 应用程序可以设置opterr为0来禁止这个行为
    opterr = 0;
    // getopt函数一般用在循环体内
    // 循环知道返回-1时退出
    while ((c = getopt(argc, argv, "d")) != EOF) {
        switch(c) {
            case 'd': /* debug模式，不以守护进程运行 */
                // log_to_stderr控制log_sys函数是打印出错信息在标准输出还是log文件
                debug = log_to_stderr = 1;
                break;
            case '?': /* 遇到无效的选项时，getopt返回一个?问号 */
                // optopt也getopt函数支持的4个外部变量之一
                // 如果处理选项时发生了错误，getopt会设置optopt指向出错的选项字符串
                err_quit("unrecognized option -%c", optopt);
        }
    }

    // 如果没有设置-d选项，则表示要以守护进程方式运行
    if (debug == 0)
        daemonize("opend");

    loop();
}
