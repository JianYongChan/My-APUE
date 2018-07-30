#include "apue.h"

int main(void) {
    printf("TZ=%s\n", getenv("TZ"));
    printf("TMPDIR=%s\n", getenv("TMPDIR"));
    printf("TERM=%s\n", getenv("TERM"));
    printf("SHELL=%s\n", getenv("SHELL"));
    printf("PWD=%s\n", getenv("PWD"));
    printf("PATH=%s\n", getenv("PATH"));
    printf("NLSPATH=%s\n", getenv("NLSPATH"));
    printf("MSGVERB=%s\n", getenv("MSGVERB"));
    printf("LOGNAME=%s\n", getenv("LOGNAME"));
    printf("LINES=%s\n", getenv("LINES"));
    printf("LC_TIME=%s\n", getenv("LC_TIME"));
    printf("LC_NUMERIC=%s\n", getenv("LC_NUMERIC"));
    printf("LC_MONETARY=%s\n", getenv("LC_MONETARY"));
    printf("LC_MESSAGES=%s\n", getenv("LC_MESSAGES"));
    printf("LC_CTYPE=%s\n", getenv("LC_CTYPE"));
    printf("LC_COLLATE=%s\n", getenv("LC_COLLATE"));
    printf("LC_ALL=%s\n", getenv("LC_ALL"));
    printf("LANG=%s\n", getenv("LANG"));
    printf("HOME=%s\n", getenv("HOME"));
    printf("DATEMSK=%s\n", getenv("DATEMSK"));
    printf("COLUMNS=%s\n", getenv("COLUMNS"));

    if (putenv("lover=xibei") != 0)
        err_sys("putenv error");
    printf("lover=%s\n", getenv("lover"));

    if (setenv("lover", "c.c.", 0) < 0)
        err_sys("setenv error");
    printf("lover=%s\n", getenv("lover"));
    if (setenv("lover", "pengdaxi", 1) < 0)
        err_sys("setenv error");
    printf("lover=%s\n", getenv("lover"));

    if (unsetenv("lover") < 0)
        err_sys("unsetenv error");
    printf("lover=%s\n", getenv("lover"));
}
