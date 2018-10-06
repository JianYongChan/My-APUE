#ifndef APUE_SLOCK_H__
#define APUE_SLOCK_H__

#include <semaphore.h>
#include <limits.h>


// 假设我们将要创建自己是锁
// 这种锁能够被一个线程加锁而被另一个线程解锁
// 则它的结构可以是这样：
struct slock {
    sem_t *semp;
    char   name[_POSIX_NAME_MAX];
};

#endif /* APUE_SLOCK_H__ */
