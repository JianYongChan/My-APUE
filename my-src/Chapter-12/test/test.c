#include <stdio.h>
#include <errno.h>
#include <string.h>

int
main(void) {
    printf("ENOSPC: %s\n", strerror(ENOSPC));
    printf("ENOENT: %s\n", strerror(ENOENT));

    return 0;
}
