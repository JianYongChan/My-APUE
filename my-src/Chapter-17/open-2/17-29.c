#include "17-26.h"
#include <sys/select.h>

void
loop(void) {
    int     i, n, maxfd, maxi, listenfd, clifd, nread;
    char    buf[MAXLINE];
    uid_t   uid;
    fd_set  rset, allset;

    FD_ZERO(&allset);

    // 创建服务器进程与客户进程连接的端点
    if ((listenfd = serv_listen(CS_OPEN)) < 0)
        log_sys("serv_listen error");
    FD_SET(listenfd, &allset); // 将listenfd加入到监听读的fd_set中
    maxfd = listenfd;
    maxi  = -1;

    /*
     * 在select返回后，可能会发生下面两种情况：
     *
     *   （1）描述符listenfd可以读取，这意味着一个新客户进程已调用了cli_conn。
     *        为了处理这种情况，我们将调用serv_accept
     *        然后为客户进程更新client数组以及与该进程相关的簿记信息
     *   （2）一个现有的客户进程的连接可以随时读取，这意味着该客户进程已经终止，或者改客户进程已发送一个新请求。
     *        如果read返回0(文件结束)，则表示客户进程已经终止了
     *        如果read返回大于0，则表示有一个新请求需处理，可以调用request来处理
     */
    for ( ; ; ) {
        rset = allset; // 每次都会修改rset
        if ((n = select(maxfd+1, &rset, NULL, NULL, NULL)) < 0)
            log_sys("select error");
        if (FD_ISSET(listenfd, &rset)) {
            // 接受新的客户端连接
            if ((clifd = serv_accept(listenfd, &uid)) < 0)
                log_sys("serv_accept error: %d", clifd);
            i = client_add(clifd, uid);
            FD_SET(clifd, &allset);
            if (clifd > maxfd)
                maxfd = clifd;
            if (i > maxi)
                maxi = i;
            log_msg("new connection: uid %d, fd %d", uid, clifd);
            continue;
        }

        for (i = 0; i <= maxi; i++) {
            if ((clifd = client[i].fd) < 0)
                continue;
            if (FD_ISSET(clifd, &rset))  {
                if ((nread = read(clifd, buf, MAXLINE)) < 0) {
                    log_sys("read error on fd %d", clifd);
                } else if (nread == 0) { /* read返回0表示客户进程已经终止了 */
                    log_msg("closed: uid %d, fd %d", client[i].uid, clifd);
                    client_del(clifd);
                    FD_CLR(clifd, &allset);
                    close(clifd);
                } else { /* read返回大于0表示有一个新请求需要处理 */
                    handle_request(buf, nread, clifd, client[i].uid);
                }
            }
        }
    }
}
