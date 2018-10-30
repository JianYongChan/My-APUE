/*
 * 将地址绑定到UNIX域套接字
 * UNIX域套接字由sockaddr_un结构表示
 * sockaddr_un结构的sun_path成员包含一个路径名
 * 当我们将一个地址绑定到一个UNIX域套接字时，系统会用该路径名创建一个S_IFSOCK类型的文件
 * 该文件仅用于向客户进程告示套接字名字
 */
#include "apue.h"
#include <sys/socket.h>
#include <sys/un.h>

int
main(void) {
    int                 fd, size;
    struct sockaddr_un  un;

    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, "foo.socket");
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        err_sys("socket error");
    /*
     * 由于sockaddr_un结构中sun_path之前的成员与实现有关
     * 所以使用<stddef.h>头文件中的offsetof宏计算sun_path成员从结构开始处的偏移量
     */
    size = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path); 
    if (bind(fd, (struct sockaddr*)&un, size) < 0)
        err_sys("bind error");
    printf("UNIX domain socket bound\n");
    exit(0);
}
