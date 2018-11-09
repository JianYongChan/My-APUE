#include "apue.h"

extern void daemonize(const char *);

int
main(int argc, char *argv[]) {
    daemonize("fuck");
    while (1) {
        sleep(30);
    }
}
