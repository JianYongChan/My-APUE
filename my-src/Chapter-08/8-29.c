/* 这个程序在我的系统(Ubuntu 18.04 LTS)上运行输出的第一列乱码，不知何故 */

#include "apue.h"
#include <sys/acct.h>

#if defined(BSD) /* FreeBSD中的不同结构体 */
#define acct acctv2
#define ac_flag ac_trailer.ac_flag
#define FMT "%-*.*s e = %.0f, chars = %7ld, %c %c %c %c\n"
#elif defined(HAS_AC_STAT)
#define FMT "%-*.*s e = %6ld, chars = %7ld, stat = %3u: %c %c %c %c\n"
#else
#define FMT "%-*.*s e = %6ld, chars = %7ld, %c %c %c %c\n"
#endif
#if defined(LINUX)
#define acct acct_v3 /* Linux下的不同结构体 */
#endif

#if !defined(HAS_ACORE)
#define ACORE 0
#endif
#if !defined(HAS_AXSIG)
#define AXSIG 0
#endif

#if !defined(BSD)
/* comp_t是3位以8为底的指数以及13位小数 */
static unsigned long compt2ulong(comp_t comptime) {
    unsigned long   val;
    int             exp;

    val = comptime & 0x1fff;     /* 13bit的小数 */
    exp = (comptime >> 13 ) & 7; /* 3bit的指数 */
    while (exp-- > 0)
        val *= 8; /* 指数以8为底 */

    return val;
}
#endif

int main(int argc, char *argv[]) {
    struct acct acdata;
    FILE        *fp;

    if (argc != 2)
        err_quit("usage: %s filename\n", argv[0]);
    if ((fp = fopen(argv[1], "r")) == NULL)
        err_sys("can't open %s", argv[1]);
    /* fread可以用于读取完整的结构体 */
    while (fread(&acdata, sizeof(acdata), 1, fp) == 1) {
        printf(FMT, (int)sizeof(acdata.ac_comm),
            (int)sizeof(acdata.ac_comm), acdata.ac_comm,
#if defined(BSD)
            acdata.ac_etime, acdata.ac_io,
#else
            compt2ulong(acdata.ac_etime), compt2ulong(acdata.ac_io),
#endif
#if defined(HAS_AC_STAT)
            (unsigned char) acdata.ac_stat,
#endif
            acdata.ac_flag & ACORE ? 'D' : ' ',
            acdata.ac_flag & AXSIG ? 'X' : ' ',
            acdata.ac_flag & AFORK ? 'F' : ' ',
            acdata.ac_flag & ASU   ? 'S' : ' ');

    }
    /* fread返回所读的对象数如果少于1
     * 则可能是因为出错或者到达了文件尾
     * 此时应调用ferror或者feof判断是哪一种情况
     */
    if (ferror(fp))
        err_sys("read error");

    exit(0);
}