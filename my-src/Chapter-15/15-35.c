#include "./slock.h"
#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

struct slock *
s_alloc() {
    struct slock *sp;
    static int cnt;

    if ((sp = malloc(sizeof(struct slock))) == NULL)
        return NULL;
    // 根据进程ID和计数器cnt来创建名字
    // 这里不用刻意用互斥量去保护计数器
    // 因为当两个竞争线程同时调用s_alloc并以同一个名字结束时，O_EXEL标志会使一个成功而另一个失败
    // 失败的线程会将errno设置为EEXIST，所以对于这种情况，我们只是再次尝试
    do {
        snprintf(sp->name, sizeof(sp->name), "/%ld.%d", (long)getpid(), cnt++);
        sp->semp = sem_open(sp->name, O_CREAT | O_EXCL, S_IRWXU, 1);
    } while ((sp->semp == SEM_FAILED) && (errno == EEXIST));
    if (sp->semp == SEM_FAILED) {
        free(sp);
        return NULL;
    }
    // 我们打开一个信号量后对其unlink，这样销毁了名字
    // 所以导致其他进程不能再次访问它，这也简化了进程结束时的清理工作
    sem_unlink(sp->name);
    return sp;
}

void
s_free(struct slock *sp) {
    sem_close(sp->semp);
    free(sp);
}

int
s_lock(struct slock *sp) {
    return (sem_wait(sp->semp));
}

int
s_trylock(struct slock *sp) {
    return (sem_trywait(sp->semp));
}

int
s_unlock(struct slock *sp) {
    return (sem_post(sp->semp));
}
