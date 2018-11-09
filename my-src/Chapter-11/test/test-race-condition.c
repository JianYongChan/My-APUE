#include "apue.h"
#include <pthread.h>

static int glob = 0;

static void *
thr_fn(void *arg) {
    int loops = *((int*)arg);
    int loc, j;

    for (j = 0; j< loops; ++j) {
        loc = glob;
        loc++;
        glob = loc;
    }

    return NULL;
}

int main(int argc, char *argv[]) {

}
