#include "apue.h"
#include <syslog.h>
#include <errno.h>

extern int lockfile(int);
extern int already_running(void);

void
reread(void) {
    /* 重读配置文件 */
}

void
sig_term(int signo) {
    syslog(LOG_INFO, "got SIGTERM, exsiting");
    exit(0);
}

void
sig_hup(int signo) {
    syslog(LOG_INFO, "Re-reading configureation file");
    reread();
}

int
main(int argc, char *argv[]) {
    char             *cmd;
    struct sigaction sa;

    if ((cmd = strrchr(argv[0], '/')) == NULL)
        cmd = argv[0];
    else
        cmd++;

    daemonize(cmd);

    if (already_running()) {
        syslog(LOG_ERR, "daemon already running");
        exit(1);
    }

    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGHUP);
    sa.sa_flags = 0;
    if (sigaction(SIGTERM, &sa, NULL) < 0) {
        syslog(LOG_ERR, "cannot catch SIGTERM: %s", strerror(errno));
        exit(1);
    }

    /* 继续daemon的其余部分 */
    exit(0);
}
