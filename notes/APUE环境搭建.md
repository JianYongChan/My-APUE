# 缘起

今天(2018-7-7)开始学习[APUE](http://www.apuebook.com/apue3e.html)，首先得把环境搭建起来。

## 环境搭建

* 首先下载源码

[apue-src](http://www.apuebook.com/code3e.html)

* 解压编译

``` shell
tar -xzvf src.3e.tar.gz
cd apue.3e && make
```

* 安装

``` shell
sudo cp ./include/apue.h /usr/include/
sudo cp ./lib/libapue.a /usr/local/lib/
```

* 测试

``` C
#include "apue.h"
#include <dirent.h>

int main(int argc, char *argv[]) {
    DIR           *dp;
    struct dirent *dirp;

    if (argc != 2) {
        err_quit("usage: ls directory_name");
    }

    if ((dp = opendir(argv[1])) == NULL) {
        err_sys("cannot open %s", argv[1]);
    }
    while ((dirp = readdir(dp)) != NULL) {
        printf("%s\n", dirp->d_name);
    }

    closedir(dp);
    exit(0);
}
```

``` shell
gcc -o exe -lapue
./exe
```