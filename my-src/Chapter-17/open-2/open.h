#ifndef OPEN_H__
#define OPEN_H__

#include "apue.h"
#include <errno.h>

#define CL_OPEN "open"
#define CS_OPEN "/tmp/opend.socket" /* 服务器总所周知的套接字名 */

int csopen(char *, int);

#endif /* OPEN_H__ */
