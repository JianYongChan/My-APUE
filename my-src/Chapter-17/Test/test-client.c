#include "apue.h"

#define BUFSZ 1024

extern int cli_conn(const char *name);

int
main(int argc, char *argv[]) {
    char *name;
    int cli_fd;
    char buf[BUFSZ];
    int message_len;
    pid_t pid;
    
    if (argc != 2) 
        err_quit("usage: ./client socket-name");

    TELL_WAIT();
    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid > 0) { /* 父进程 */
        WAIT_CHILD();
        if ((cli_fd = cli_conn("my_server")) < 0)
            err_sys("cli_conn error");
        if ((message_len = read(cli_fd, buf, sizeof(buf))) < 0)
            err_sys("cli_conn error");
        fprintf(stdout, buf);
        waitpid(pid, NULL, 0);
    } else { /* 子进程 */
        if ((cli_fd = cli_conn("my_server")) < 0)
            err_sys("cli_conn error");
        if ((message_len = read(cli_fd, buf, sizeof(buf))) < 0)
            err_sys("cli_conn error");
        fprintf(stdout, buf);
        TELL_PARENT(getppid());
        exit(0);
    }
    return 0;
}
