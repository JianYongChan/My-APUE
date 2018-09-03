/*
 * non-reentrant version of getenv
 * the envbuf is static and shared by all threads
 */

#include <limits.h>
#include <string.h>
#include <stdio.h>

#define MAXSTRINGSZ 4096

static char envbuf[MAXSTRINGSZ];

extern char **environ;

char *
getenv_1(const char *name) {
    int i, len;

    len = strlen(name);
    for (i = 0; environ[i] != NULL; ++i) {
        if ((strncmp(name, environ[i], len) == 0) &&
                environ[i][len] == '=') {
            strncpy(envbuf, &environ[i][len+1], MAXSTRINGSZ-1);
            return envbuf;
        }
    }

    return NULL;
}
