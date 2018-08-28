#include <stdlib.h>
#include <pthread.h>

#define NHASH 29
#define HASH(id) (((unsigned long)id)%NHASH)

struct foo *fh [NHASH];

pthread_mutex_t hashlock = PTHREAD_MUTEX_INITIALIZER;

struct foo {
    int             f_count; /* 由hashlock 保护 */
    pthread_mutex_t f_lock;
    int             f_id;
    struct foo      *f_next; /* 由hashlock 保护 */
};

struct foo*
foo_alloc(int id) {
    struct foo *fp;
    int        idx;

    if ((fp = malloc(sizeof(struct foo))) != NULL) {
        fp->f_count = 1;
        fp->f_id    = id;
        if (pthread_mutex_init(&fp->f_lock, NULL) != 0) {
            free(fp);
            return NULL;
        }
        idx = HASH(id);
        pthread_mutex_lock(&hashlock);
        fp->f_next = fh[idx];
        fh[idx] = fp;
        pthread_mutex_lock(&fp->f_lock);
        pthread_mutex_unlock(&hashlock);
        /* ... continue initialization */
        pthread_mutex_unlock(&fp->f_lock);
    }
    return fp;
}

void
foo_hold(struct foo *fp) {
    pthread_mutex_lock(&fp->f_lock);
    fp->f_count++;
    pthread_mutex_unlock(&fp->f_lock);
}

struct foo *
foo_find(int id) {
    struct foo *fp;

    pthread_mutex_lock(&hashlock);
    for (fp = fh[HASH(id)]; fp != NULL; fp = fp->f_next) {
        if (fp->f_id == id) {
            foo_hold(fp);
            break;
        }
    }
    pthread_mutex_unlock(&hashlock);
    return fp;
}

void
foo_release(struct foo *fp) {
    struct foo *tfp;
    int        idx;

    /*
     * 和11-11.c相对比
     * 这里锁的粒度较粗
     * 在11-11.c中，检查f_count就用了一次f_lock锁，
     * 不直接锁住hash表是因为如果f_lock递减后不为0 就不用删除该structure，
     * 就不会对整个hash表做改变，只是对一个foo structure改变，所以只需要锁住一个foo。
     * 而这里直接就锁住了整个hash表，当然也就锁住了其中的foo structure了
     */
    pthread_mutex_lock(&hashlock);
    if (--fp->f_count == 0) {
        idx = HASH(fp->f_id);
        tfp = fh[idx];
        if (tfp == fp) {
            fh[idx] = fp->f_next;
        } else {
            while (tfp->f_next != fp)
                tfp = tfp->f_next;
            tfp->f_next = fp->f_next;
        }
        pthread_mutex_unlock(&hashlock);
        pthread_mutex_destroy(&fp->f_lock);
        free(fp);
    } else {
        pthread_mutex_unlock(&hashlock);
    }
}
