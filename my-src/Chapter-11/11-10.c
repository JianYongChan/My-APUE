#include <stdlib.h>
#include <pthread.h>

struct foo {
    int             f_count;
    pthread_mutex_t f_lock;
    int             f_id;
};

struct foo *
foo_alloc(int id) {
    struct foo *fp;
    if ((fp = malloc(sizeof(struct foo))) != NULL) {
        fp->f_count = 1;
        fp->f_id    = id;
        if (pthread_mutex_init(&fp->f_lock, NULL) != 0) {
            free(fp);
            return NULL;
        }
    }
    return fp;
}

void
foo_hold(struct foo *fp) {
    pthread_mutex_lock(&fp->f_lock);
    fp->f_count++;
    pthread_mutex_unlock(&fp->f_lock);
}

/*
 * 减少引用计数
 * 需要注意的是，如果有另外一个线程在调用foo_hold时阻塞等待mutex
 * 此时即使该对象的引用计数为0，也不能释放对象的内存
 * 可以通过确保对象在释放内存前不会被找到这种方式来避免这个问题
 */
void
foo_release(struct foo *fp) {
    pthread_mutex_lock(&fp->f_lock);
    if (--fp->f_count == 0) {
        pthread_mutex_unlock(&fp->f_lock);
        pthread_mutex_destroy(&fp->f_lock);
        free(fp);
    } else {
        pthread_mutex_unlock(&fp->f_lock);
    }
}
