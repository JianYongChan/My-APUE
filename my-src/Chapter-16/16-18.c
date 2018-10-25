#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <syslog.h> // file: /var/log/system.log
#include <sys/socket.h>
#include <sys/wait.h>


#define QLEN 10

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

extern int init_server(int, const struct sockaddr *, socklen_t, int);

void
serve(int sockfd) {
    int     client_fd, status;
    pid_t   pid;

    set_cloexec(sockfd);
    for ( ; ;  ) {
        if ((client_fd = accept(sockfd, NULL, NULL)) < 0) {
            syslog(LOG_ERR, "ruptimed: accept error: %s", strerror(errno));
            exit(1);
        }

        if ((pid = fork()) < 0) {
            syslog(LOG_ERR, "ruptimed: fork error: %s", strerror(errno));
            exit(1);
        } else if (pid == 0) { /* 子进程 */
            // 将stderr, stdout绑定到client_fd
            if (dup2(client_fd, STDOUT_FILENO) != STDOUT_FILENO ||
                    dup2(client_fd, STDERR_FILENO) != STDERR_FILENO) {
                syslog(LOG_ERR, "ruptimed: unexpected error");
                exit(1);
            }

            close(client_fd);
            execl("/usr/bin/uptime", "uptime", (char*)0);
            syslog(LOG_ERR, "ruptimed: unexpected return from exec: %s", strerror(errno));
        } else {
            close(client_fd);
            waitpid(pid, &status, 0);
        }
    }
}

int
main(int argc, char *argv[]) {
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;
    int             sockfd, err, n;
    char            *host;

    if (argc != 1)
        err_quit("usage: ruptimed");
    if ((n = sysconf(_SC_HOST_NAME_MAX)) < 0)
        n = HOST_NAME_MAX;
    if ((host = malloc(n)) == NULL)
        err_sys("malloc error");
    if (gethostname(host, n) < 0)
        err_ret("gethostname error");
    daemonize("ruptimed");
    memset(&hint, 0, sizeof(hint));
    hint.ai_flags = AI_CANONNAME;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;
    if ((err = getaddrinfo(host, "4399", &hint, &ailist)) != 0) {
        syslog(LOG_ERR, "ruptimed: getaddrinfo error: %s", gai_strerror(errno));
        exit(1);
    }
    for (aip = ailist; aip != NULL; aip = ailist->ai_next) {
        if ((sockfd = init_server(SOCK_STREAM, aip->ai_addr, aip->ai_addrlen, QLEN)) > 0) {
            serve(sockfd);
            exit(0);
        }
    }

    exit(1);
}
