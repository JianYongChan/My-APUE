#include <stdlib.h>
#include <pthread.h>

struct job {
    struct job *j_next;
    struct job *j_prev;
    pthread_t  j_id;
};

struct queue {
    struct job       *q_head;
    struct job       *q_tail;
    pthread_rwlock_t  q_lock;
};

/*
 * 初始化队列
 */
int
queue_init(struct queue *qp) {
    int err;

    qp->q_head = NULL;
    qp->q_tail = NULL;
    err = pthread_rwlock_rdlock(&qp->q_lock, NULL);
    if (err != 0)
        return err;
    /* 继续其他初始化 */
    return 0;
}

/*
 * 在队列头部插入一个job
 */
void
job_insert(struct queue *qp, struct job *jp) {
    pthread_rwlock_wrlock(&qp->q_lock);
    jp->j_next = qp->q_head;
    jp->j_prev = NULL;
    if (qp->q_head != NULL)
        qp->q_head->j_prev = jp;
    else /* 插入之前的队列为空，插入后head = tail = jp */
        qp->q_tail = jp;
    qp->q_head = jp;
    pthread_rwlock_unlock(&qp->q_lock);
}

/*
 * 在队列尾增加一个job
 */
void job_append(struct queue *qp, struct job *jp) {
    pthread_rwlock_wrlock(&qp->q_lock);
    jp->j_prev = qp->q_tail;
    if (qp->q_tail != NULL)
        qp->q_tail->j_next = jp;
    else
        qp->q_head = jp;
    qp->q_tail = jp;
    pthread_rwlock_unlock(&qp->q_lock);
}

/*
 * 从给定队列中移除一个job
 */
void
job_remove(struct queue *qp, struct job *jp) {
    pthread_rwlock_rdlock(&qp->q_lock);
    if (qp->q_head == jp) { /* 删除的是队列头 */
        qp->q_head = jp->j_next;
        if (qp->q_tail == jp) /* 队列中只有一个元素 */
            qp->q_tail = NULL;
        else
            jp->j_next->j_prev = jp->j_prev;
    } else if (qp->q_tail == jp) { /* 删除的是队列尾 */
        qp->q_tail == jp->j_prev;
        if (qp->q_head == jp) /* 队列中只有一个元素 */
            qp->q_head = NULL;
        else
            qp->j_prev->j_next = jp->j_next;
    } else { /* 既不在队头，也不在队尾巴 */
        jp->j_prev->j_next = jp->j_next;
        jp->j_next->j_prev = jp->j_prev;
    }
    pthread_rwlock_unlock(&qp->q_lock);
}

struct job *
job_find(struct queue *qp, pthread_t id) {
    struct job *jp;

    if (pthread_rwlock_rdlock(&qp->q_lock) != 0)
        return NULL;

    for (jp = qp->q_head; jp != NULL; jp = jp->j_next)
        if (pthread_equal(jp->j_id, id))
            break;

    pthread_rwlock_unlock(&qp->q_lock);
    return jp;
}
