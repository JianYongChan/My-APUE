#include "apue.h"
#include <ctype.h>
#include <fcntl.h>
#include <aio.h>
#include <errno.h>

#define BSZ 4096
#define NBUF 8

/*
 * 用在buf结构中，表示buf的状态(执行的操作)
 */
enum rwop {
    UNUSED = 0,         // buf尚未被使用
    READ_PENDING = 1,   // 数据已经被读入buf中了
    WRITE_PENDING = 2   // buf中的数据已经aio_write写了
};

struct buf {
    enum rwop       op;         // buf用于的操作(未被使用，read, write)
    int             last;       // 数据的最后一个buf
    struct aiocb    aiocb;
    unsigned char   data[BSZ];  // aiocb中的缓冲区
};

struct buf bufs[NBUF];          // 8个buf

/*
 * 将a~z和A~Z循环右移13个字符
 */
unsigned char
translate(unsigned char c) {
    if (c >= 'n')
        c -= 13;
    else if (c >= 'a')
        c += 13;
    else if (c >= 'N')
        c -= 13;
    else if (c == '\n') // 和书上不同的是我这里不处理换行，方便对照
        return c;
    else
        c += 13;

    return c;
}

int
main(int argc, char *argv[]) {
    int               ifd, ofd, i, j, n, err, numop;
    struct stat       sbuf;
    const struct buf  *aiolist[NBUF];
    off_t             off = 0;

    if (argc != 3)
        err_quit("usage %s infile outfile", argv[0]);
    if ((ifd = open(argv[1], O_RDONLY)) < 0)
        err_sys("cannot open %s", argv[1]);
    if ((ofd = open(argv[2], O_RDWR|O_CREAT|O_TRUNC, FILE_MODE)) < 0)
        err_sys("cannot create %s", argv[2]);
    if (fstat(ifd, &sbuf) < 0)
        err_sys("fstat error");

    /* 初始化bufs */
    for (int i = 0; i < NBUF; ++i) {
        bufs[i].op = UNUSED;
        bufs[i].aiocb.aio_buf = bufs[i].data;
        bufs[i].aiocb.aio_sigevent.sigev_notify = SIGEV_NONE;
        aiolist[i] = NULL;
    }

    numop = 0;
    for ( ; ;  ) {
        for (i = 0; i < NBUF; ++i) {
            switch (bufs[i].op) {
                case UNUSED: // 该buf区处于空闲状态
                    if (off < sbuf.st_size) { // 输入文件中还有数据等待读取
                        bufs[i].op = READ_PENDING; // 将该buf设置为已经读取数据状态
                        bufs[i].aiocb.aio_fildes = ifd;
                        /*
                         * 由于输入文件中的字符和输出文件中的对应字符的偏移量是相等的，
                         * 所以这里就简单地把读取数据时的数据偏移量用在接下来的数据写入文件操作中去
                         */
                        bufs[i].aiocb.aio_offset = off;
                        off += BSZ; // 从输入文件中读取4096字节
                        if (off >= sbuf.st_size) // 文件中所有数据都被读取完了，说明该buf是最后一个
                            bufs[i].last = 1;
                        bufs[i].aiocb.aio_nbytes = BSZ;
                        // 前面都在设置状态
                        // 这里才是读取数据到buf的aiocb中的缓冲区
                        if (aio_read(&bufs[i].aiocb) < 0)
                            err_sys("aio_read failed");
                        // 这里使用的是基于列表的异步I/O
                        // 需要将指向aiocb的指针加入到列表中去
                        aiolist[i] = &bufs[i].aiocb;
                        numop++;
                    }
                    break;

                case READ_PENDING: // 该buf已经读取了数据，接下来就是写入
                    // EINPROGRESS表示异步读、写或者同步操作仍然在等待
                    if ((err = aio_error(&bufs[i].aiocb)) == EINPROGRESS)
                        continue;
                    if (err != 0) {
                        if (err == -1) // 对aio_error操作调用失败，errno设置错误码
                            err_sys("aio_error failed");
                        else // 返回值是相关的异步操作失败返回的错误码
                            err_exit(err, "read failed");
                    }

                    // 获取异步操作的返回值
                    if ((n = aio_return(&bufs[i].aiocb)) < 0)
                        err_sys("aio_return failed");
                    if (n != BSZ && !bufs[i].last)
                        err_quit("short read (%d/%d)", n, BSZ);
                    // 翻译buf的aiocb的缓冲区，并写之
                    for (j = 0; j < n; j++)
                        bufs[i].data[j] = translate(bufs[i].data[j]);
                    // 设置buf的状态以及aiocb的和写入操作相关的信息
                    bufs[i].op = WRITE_PENDING;
                    bufs[i].aiocb.aio_fildes = ofd;
                    bufs[i].aiocb.aio_nbytes = n;
                    // 将缓冲区中已经翻译了的的数据写入
                    if (aio_write(&bufs[i].aiocb) < 0)
                        err_sys("aio_write failed");
                    break;

                case WRITE_PENDING:
                    if ((err = aio_error(&bufs[i].aiocb)) == EINPROGRESS)
                        continue;
                    if (err != 0) {
                        if (err == -1)
                            err_sys("aio_error failed");
                        else
                            err_exit(err, "write failed");
                    }

                    if ((n = aio_return(&bufs[i].aiocb)) < 0)
                        err_sys("aio_return failed");
                    if (n != bufs[i].aiocb.aio_nbytes)
                        err_quit("short write (%d/%d)", n, BSZ);
                    aiolist[i] = NULL;
                    bufs[i].op = UNUSED; // 复用buf
                    numop--; // 一个读写操作完成
                    break;
            }
        }

        if (numop == 0) {
            if (off >= sbuf.st_size)
                break;
        } else {
            if (aio_suspend(aiolist, NBUF, NULL) < 0)
                err_sys("aio_suspend failed");
        }
    }

    bufs[0].aiocb.aio_fildes = ofd;
    if (aio_fsync(O_SYNC, &bufs[0].aiocb) < 0)
        err_ret("aio_fsync failed");
    exit(0);
}
