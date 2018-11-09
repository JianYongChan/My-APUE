#include "apue.h"
#include <pthread.h>

#define NHASH 29
#define HASH(id) (((unsigned long)id)%NHASH)

struct foo *fh [NHASH];

pthread_mutex_t hashlock = PTHREAD_MUTEX_INITIALIZER;

struct foo {
    int             f_count;
    pthread_mutex_t f_lock;
    int             f_id;
    struct foo      *f_next; /* 由hashlock保护 */
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
        /*
         * 对散列表的锁解锁之前，先锁定新结构中的互斥量
         * 因为新的结构是放在全局的hash表中的，所以在初始化完成之前，需要阻塞其他的线程访问新结构
         */
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

/*
 * 返回0表示释放了对象，-1表示没有
 */
int
foo_release(struct foo *fp) {
    struct foo *tfp;
    int        idx;

    pthread_mutex_lock(&fp->f_lock);
    if (fp->f_count == 1) { /* 最后一个引用 */
        pthread_mutex_unlock(&fp->f_lock);
        /*
         * 为什么不直接对fp->flock一锁到底呢？
         * 偏要先上f_lock锁->检查f_count->解f_lock锁->上hash锁->上f_lock锁？
         */
        pthread_mutex_lock(&hashlock);
        pthread_mutex_lock(&fp->f_lock);
        /*
         * 由于上面曾对f_lock解锁了，可能该对象在解锁后被别的线程改变
         * 所以需要再检查一次引用计数
         */
        if (fp->f_count != 1) {
            fp->f_count--;
            pthread_mutex_unlock(&fp->f_lock);
            pthread_mutex_unlock(&hashlock);
            return -1; /* 由于引用计数没有递减到0，所以不删除 */
        }
        /* 从hash表中删除结构 */
        idx = HASH(fp->f_id);
        tfp = fh[idx];
        if (tfp == fp) { /* 被删除的是链表头 */
            fh[idx] = fp->f_next;
        } else {
            while (tfp->f_next != fp)
                tfp = tfp->f_next;
            tfp->f_next = fp->f_next;
        }
        pthread_mutex_unlock(&hashlock);
        pthread_mutex_unlock(&fp->f_lock);
        pthread_mutex_destroy(&fp->f_lock);
        free(fp);
        return 0;
    } else { /* 引用计数不会减少至0，无需释放对象内存 */
        fp->f_count--;
        pthread_mutex_unlock(&fp->f_lock);
        return -1;
    }
}


void *
thr_fn1(void *arg) {
    struct foo *fp;
    int ret;

    fp = foo_find(7);
    if (fp == NULL) {
        printf("thread-1 cannot find foo(7)\n");
    } else {
        printf("thread-1 found foo(7)\n");
    }
    ret = foo_release(fp);
    if (ret == 0)
        printf("thread-1 release foo of id 7\n");
    else
        printf("thread-1 cannot release foo of id 7\n");

    return (void*)1;
}

void *
thr_fn2(void *arg) {
    struct foo *fp;
    int ret;

    fp = foo_find(7);
    if (fp == NULL) {
        printf("thread-2 cannot find foo(7)\n");
    } else {
        printf("thread-2 found foo(7)\n");
    }
    ret = foo_release(fp);
    if (ret == 0)
        printf("thread-2 release foo of id 7\n");
    else
        printf("thread-2 cannot release foo of id 7\n");

    return (void*)2;
}

int
main(void) {
    int       err;
    pthread_t tid1, tid2;

    foo_alloc(1);
    foo_alloc(2);
    foo_alloc(2);
    foo_alloc(7);

    err = pthread_create(&tid1, NULL, thr_fn1, NULL);
    if (err != 0)
        err_exit(err, "cannot create pthread-1");

    err = pthread_create(&tid2, NULL, thr_fn2, NULL);
    if (err != 0)
        err_exit(err, "cannot create pthread-2");

}
