# 第6章练习

* 如果系统使用了阴影文件，那么如何取得加密口令？

如果使用了阴影文件，那么`/etc/passwd`文件中的密码字段就是无效的，就需要从`/etc/shadow`文件中查找对于用户名的加密口令。

像下面的程序，它获取的加密口令就是无效的(只是一个占位符)，应该从`/etc/shadow`中继续查找。

``` C
#include "apue.h"
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <stddef.h>
#include <string.h>
struct passwd* mygetpwnam(const char *name
) {    struct passwd *ptr;
    setpwent();
    while((ptr = getpwent()) != NULL) {        if (strcmp(name, ptr->pw_name) ==
0) {
            break;
        }    }
    endpwent();
    return(ptr);
}

int main(int argc, char *argv[]) {
    struct passwd *ptr;

    if (argc != 2) {
        err_quit("usage: %s <name>", argv[0]);
    }
    if((ptr = mygetpwnam(argv[1])) != NULL) {
        printf("name: %s\n", ptr->pw_name);
        printf("passwd: %s\n", ptr->pw_passwd);
        printf("uid: %d\n", ptr->pw_uid);
        printf("gid: %d\n", ptr->pw_gid);
        printf("comment: %s\n", ptr->pw_gecos);
        printf("home: %s\n", ptr->pw_dir);
        printf("shell: %s\n", ptr->pw_shell);
    }
}
```

* 假设具有超级用户权限，并且系统使用了阴影文件，又该如何取得加密口令呢？

有了超级用户权限，就表示她就算不是`/etc/shadow`文件的所有者或者组成员也可以查看该文件了。

* 编程调用`uname`并输出`utsname`结构中的所有字段。将该输出与`uname(1)`命令的输出结果进行比较

``` C
#include "apue.h"
#include <sys/uname.h>

int main(void) {
    struct utsname nameinfo;
    char hostname[NAMELEN];

    if (uname(&nameinfo) < 0) {
        err_sys("uname error");
    }
    printf("sysname: %s\n", nameinfo.sysname);
    printf("nodename: %s\n", nameinfo.nodename);
    printf("release: %s\n", nameinfo.release);
    printf("version: %s\n", nameinfo.version);
    printf("machine: %s\n", nameinfo.machine);
    printf("domain name: %s\n", nameinfo.__domainname);
}
```

* 计算可由`time_t`数据类型表示的最近时间。如果超出了这一时间将会如何？

`time_t`在我的机器(Ubuntu 18.04 64bit)上为`long int`类型，为64位，可以表示的大小-2^64 ~ 2^64 -1，超出了就会回滚。

* 编程获取当前时间，并使用`strftime`将输出结果转换为类似于`date(1)`命令的默认输出。将环境变量`TZ`设置为不同值，观察输出结果。

``` C
#include "apue.h"                     5.c -lapue
#include <time.h>

int main() {
    time_t t;
    struct tm *tmp;
    char buf[64];

    t = time(NULL);
    if ((tmp = localtime(&t)) == NULL)
        err_sys("localtime error");
    if (strftime(buf, 64, "%a %b %d %H:%M:%S %Z %Y", tmp) == 0)
        err_sys("strftime error");
    printf("%s\n", buf);
}
```