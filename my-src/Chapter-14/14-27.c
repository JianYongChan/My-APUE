#include "apue.h"
#include <fcntl.h>
#include <sys/mman.h>

// 为了限制使用内存的量，我们每次最多复制1GB的数据
#define COPYINCR    (1024*1024*1024)    /* 1GB */

int
main(int argc, char *argv[]) {
    int         fdin, fdout;
    void        *src, *dst;
    size_t      copysz; // 实际上每次复制的数据量
    struct stat sbuf;
    off_t       fsz = 0; // 表示已经复制了的数据量

    if (argc != 3)
        err_quit("usage: %s <fromfile> <tofile>", argv[0]);

    if ((fdin = open(argv[1], O_RDONLY)) < 0)
        err_sys("cannot open %s for reading", argv[1]);

    if ((fdout = open(argv[2], O_RDWR|O_CREAT|O_TRUNC, FILE_MODE)) < 0)
        err_sys("cannot creat %s for writing", argv[2]);

    // 用于获取输入文件的长度
    if (fstat(fdin, &sbuf) < 0)
        err_sys("fstat error");

    // 因为不能用mmap将数据添加到文件中，所以必须先加长输出文件的长度至和输入文件等长
    if (ftruncate(fdout, sbuf.st_size) < 0)
        err_sys("ftruncate error");

    while (fsz < sbuf.st_size) {
        if ((sbuf.st_size - fsz) > COPYINCR)
            copysz = COPYINCR;
        else
            copysz = sbuf.st_size - fsz;

        if ((src = mmap(0, copysz, PROT_READ, MAP_SHARED, fdin, fsz)) == MAP_FAILED)
            err_sys("mmap error for input");
        if ((dst = mmap(0, copysz, PROT_READ|PROT_WRITE, MAP_SHARED, fdout, fsz)) == MAP_FAILED)
            err_sys("mmap error for output");

        memcpy(dst, src, copysz);
        // 复制后一部分数据之前，需要解除前一部分数据的映射
        munmap(src, copysz);
        munmap(dst, copysz);
        fsz += copysz;
    }

    exit(0);
}
