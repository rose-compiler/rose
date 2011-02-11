#include "threadSupport.h"
#include <string.h>

int
RTS_rwlock_init(RTS_rwlock_t *lock, pthread_rwlockattr_t *attrs)
{
    memset(lock, 0, sizeof(*lock));

    int status;
    status = pthread_rwlock_init(&lock->rwlock, attrs);
    if (status) return status;

    status = pthread_mutex_init(&lock->mutex, NULL);
    return status;
}

int
RTS_rwlock_wrlock(RTS_rwlock_t *lock)
{
    /* Check whether the lock is recursive in this thread. If so, then allow the request to proceed by simply updating some
     * information in the lock object. */
    bool recursive = false;
    RTS_MUTEX(lock->mutex) {
        if (lock->nlocks>0 && pthread_equal(lock->owner, pthread_self())) {
            assert(lock->nlocks < lock->max_nlocks && "too much recursion");
            recursive = true;
            lock->nlocks++;
        }
    } RTS_MUTEX_END;

    /* Obtain the write lock. If this is a recursive request then this thread already holds the lock. Otherwise we need to
     * obtain the lock and record which thread holds it so recursion can be detected. */
    if (!recursive) {
        int status = pthread_rwlock_wrlock(&lock->rwlock);
        if (status) return status;
        RTS_MUTEX(lock->mutex) {
            lock->nlocks = 1;
            lock->owner = pthread_self();
        } RTS_MUTEX_END;
    }
    return 0;
}

int
RTS_rwlock_rdlock(RTS_rwlock_t *lock)
{
    /* Check whether this thread already holds a write lock. If so, then allow the request to proceed by simply updating some
     * information in the lock object. */
    bool recursive = false;
    RTS_MUTEX(lock->mutex) {
        if (lock->nlocks>0 && pthread_equal(lock->owner, pthread_self())) {
            assert(lock->nlocks < lock->max_nlocks && "too much recursion");
            recursive = true;
            lock->nlocks++;
        }
    } RTS_MUTEX_END;

    /* Obtain the read lock. If "recursive" is true then the thread already holds the write lock and we can proceed. Otherwise
     * we need to obtain a read lock. Per POSIX Thread specs, read locks handle recursion without any special work on our part
     * if there is no current write lock. */
    if (!recursive) {
        int status = pthread_rwlock_rdlock(&lock->rwlock);
        if (status) return status;
    }

    return 0;
}

int
RTS_rwlock_unlock(RTS_rwlock_t *lock)
{
    /* Check whether this thread already holds a write lock plus one or more recursive locks.  If so, then all we need to do is
     * update some information in the lock object.
     *
     * No mutex lock is necessary here because one of the following is true:
     *    1. No thread holds a write lock, in which case there's nothing for us to adjust within "lock" and "lock" is not
     *       changing because all pending write lock requests are blocked due to our holding the read lock, or
     *    2. We hold a write lock, in which case no other thread holds a lock and all pending write lock requests are blocked.
     */
    if (lock->nlocks>0) {
        assert(pthread_equal(lock->owner, pthread_self()));
        lock->nlocks--;
    }

    if (0==lock->nlocks) {
        int status = pthread_rwlock_unlock(&lock->rwlock);
        if (status) return status;
    }

    return 0;
}
