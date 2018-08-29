#include <sys/time.h>
#include <stdlib.h>

void
maketimeout(struct timespec *tsp, long minutes) {
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    tsp->tv_sec = tv.tv_sec;
    tsp->tv_nsec = tv.tv_usec * 1000;
    tsp->tv_sec += minutes * 60;
}
