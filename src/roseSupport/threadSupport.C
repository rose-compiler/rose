#include "threadSupport.h"
#include "stringify.h"

#include <errno.h>
#include <stdarg.h>
#include <string.h>

/******************************************************************************************************************************
 *                                      Layered Synchronization Primitives
 ******************************************************************************************************************************/

static const unsigned LockedLayers_MAGIC = 0xee2894f0;

struct LockedLayers {
    unsigned magic;
    bool active;
    size_t total_locks;
    size_t nlocks[RTS_LAYER_NLAYERS];
    RTS_Layer min_locked_layer;
};

static __thread LockedLayers locked_layers;

bool
RTS_acquiring(RTS_Layer layer)
{
    bool retval = true;

    if (0==locked_layers.magic)
        locked_layers.magic = LockedLayers_MAGIC;

    assert(locked_layers.magic==LockedLayers_MAGIC);
    assert(layer>=0 && layer<RTS_LAYER_NLAYERS);

    if (0==layer) {
        /* totally ignore this lock */
    } else if (!locked_layers.active) {
        locked_layers.active = true;
        locked_layers.min_locked_layer = layer;
        locked_layers.nlocks[layer]++;
        locked_layers.total_locks++;
    } else if (layer>locked_layers.min_locked_layer) {
        locked_layers.nlocks[layer]++;
        locked_layers.total_locks++;
        fprintf(stderr,
                "\n"
                "--------------------------------------------------------------------------------\n"
                "ERROR: ROSE Thread Support (RTS) requires that a thread obtain synchronization\n"
                "       locks in a particular order, according to the layer to which those locks\n"
                "       belong, to prevent possible deadlock.  The calling thread has violated\n"
                "       this policy by attempting to acquire locks incorrectly, namely:\n"
                "       a lock in\n"
                "           %s (%d)\n"
                "       is being requested after having acquired a lock in\n"
                "           %s (%d).\n"
                "--------------------------------------------------------------------------------\n",
                stringifyRTS_Layer(layer).c_str(), (int)layer,
                stringifyRTS_Layer(locked_layers.min_locked_layer).c_str(), (int)locked_layers.min_locked_layer);
        retval = false;
    } else {
        locked_layers.nlocks[layer]++;
        locked_layers.total_locks++;
        locked_layers.min_locked_layer = layer;
    }

    return retval;
}

void
RTS_releasing(RTS_Layer layer)
{

#if 1   /* DEBUGGING [RPM 2011-04-22] */
    if (layer<0 || layer>=RTS_LAYER_NLAYERS) {
        fprintf(stderr, "ERROR: Layer=%d\n", (int)layer);
        assert(!"ROBB");
        abort();
    }
#endif
    assert(locked_layers.magic==LockedLayers_MAGIC);
    assert(layer>=0 && layer<RTS_LAYER_NLAYERS);

    if (0==layer) {
        /* totally ignore this lock */
    } else {
        assert(locked_layers.active);           /* did we forget to call RTS_acquire()? */
        assert(locked_layers.total_locks>0);
        assert(locked_layers.nlocks[layer]>0);

        --locked_layers.nlocks[layer];
        --locked_layers.total_locks;

        if (0 == locked_layers.total_locks) {
            locked_layers.min_locked_layer = RTS_LAYER_DONTCARE;
            locked_layers.active = false;
        } else if (0==locked_layers.nlocks[layer]) {
            for (int i=locked_layers.min_locked_layer+1; i<RTS_LAYER_NLAYERS; i++) {
                if (locked_layers.nlocks[i]) {
                    locked_layers.min_locked_layer = (RTS_Layer)i;
                    break;
                }
            }
        }
    }
}



/******************************************************************************************************************************
 *                                      Mutual Exclusion
 ******************************************************************************************************************************/

#ifdef ROSE_THREADS_ENABLED
int
RTS_mutex_init(RTS_mutex_t *mutex, RTS_Layer layer, pthread_mutexattr_t *attr)
{
    mutex->magic = RTS_MUTEX_MAGIC;
    mutex->layer = layer;
    return pthread_mutex_init(&mutex->mutex, attr);
}
#else
int
RTS_mutex_init(RTS_mutex_t*, RSIM_Layer, void*)
{
    return ENOSYS;
}
#endif
    
int
RTS_mutex_lock(RTS_mutex_t *mutex)
{
#ifdef ROSE_THREADS_ENABLED
    assert(mutex->magic==RTS_MUTEX_MAGIC);
    if (!RTS_acquiring(mutex->layer))
        abort();
    int retval = pthread_mutex_lock(&mutex->mutex);
    if (retval!=0)
        RTS_releasing(mutex->layer);
    return retval;
#else
    return ENOSYS;
#endif
}

int
RTS_mutex_unlock(RTS_mutex_t *mutex)
{
#ifdef ROSE_THREADS_ENABLED
    assert(mutex->magic==RTS_MUTEX_MAGIC);
    int retval = pthread_mutex_unlock(&mutex->mutex);
    if (retval==0)
        RTS_releasing(mutex->layer);
    return retval;
#else
    return ENOSYS;
#endif
}




/******************************************************************************************************************************
 *                                      Read-write locks
 ******************************************************************************************************************************/

#ifdef ROSE_THREADS_ENABLED
int
RTS_rwlock_init(RTS_rwlock_t *lock, RTS_Layer layer, pthread_rwlockattr_t *attrs)
{
    memset(lock, 0, sizeof(*lock));
    lock->magic = RTS_RWLOCK_MAGIC;
    lock->layer = layer;

    int status;
    status = pthread_rwlock_init(&lock->rwlock, attrs);
    if (status) return status;

    status = RTS_mutex_init(&lock->mutex, RTS_LAYER_DONTCARE, NULL);
    return status;
}
#else
int
RTS_rwlock_init(RTS_rwlock_t*, void *attrs)
{
    return ENOSYS;
}
#endif

int
RTS_rwlock_wrlock(RTS_rwlock_t *lock)
{
#ifdef ROSE_THREADS_ENABLED
    assert(lock->magic == RTS_RWLOCK_MAGIC);

    /* Check whether the lock is recursive in this thread. If so, then allow the request to proceed by simply updating some
     * information in the lock object. */
    bool recursive = false;
    RTS_MUTEX(lock->mutex) {
        if (lock->nlocks>0 && pthread_equal(lock->owner, pthread_self())) {
            recursive = true;
            lock->nlocks++;
        }
    } RTS_MUTEX_END;

    /* Obtain the write lock. If this is a recursive request then this thread already holds the lock. Otherwise we need to
     * obtain the lock and record which thread holds it so recursion can be detected. */
    if (!recursive) {
        if (!RTS_acquiring(lock->layer))
            abort();
        int status = pthread_rwlock_wrlock(&lock->rwlock);
        if (status) {
            RTS_releasing(lock->layer);
            return status;
        }
        RTS_MUTEX(lock->mutex) {
            lock->nlocks = 1;
            lock->owner = pthread_self();
        } RTS_MUTEX_END;
    }
    return 0;
#else
    return ENOSYS;
#endif
}

int
RTS_rwlock_rdlock(RTS_rwlock_t *lock)
{
#ifdef ROSE_THREADS_ENABLED
    assert(lock->magic == RTS_RWLOCK_MAGIC);

    /* Check whether this thread already holds a write lock. If so, then allow the request to proceed by simply updating some
     * information in the lock object. */
    bool recursive = false;
    RTS_MUTEX(lock->mutex) {
        if (lock->nlocks>0 && pthread_equal(lock->owner, pthread_self())) {
            recursive = true;
            lock->nlocks++;
        }
    } RTS_MUTEX_END;

    /* Obtain the read lock. If "recursive" is true then the thread already holds the write lock and we can proceed. Otherwise
     * we need to obtain a read lock. Per POSIX Thread specs, read locks handle recursion without any special work on our part
     * if there is no current write lock. */
    if (!recursive) {
        if (!RTS_acquiring(lock->layer))
            abort();
        int status = pthread_rwlock_rdlock(&lock->rwlock);
        if (status) {
            RTS_releasing(lock->layer);
            return status;
        }
    }

    return 0;
#else
    return ENOSYS;
#endif
}

int
RTS_rwlock_unlock(RTS_rwlock_t *lock)
{
#ifdef ROSE_THREADS_ENABLED
    assert(lock->magic == RTS_RWLOCK_MAGIC);

    /* Check whether this thread already holds a write lock plus one or more recursive locks.  If so, then all we need to do is
     * update some information in the lock object.
     *
     * No mutex lock is necessary here because one of the following is true:
     *    1. No thread holds a write lock, in which case there's nothing for us to adjust within "lock" and "lock" is not
     *       changing because all pending write lock requests are blocked due to our holding the read lock, or
     *    2. We hold a write lock, in which case no other thread holds a lock and all pending write lock requests are blocked.
     */
    RTS_MUTEX(lock->mutex) {
        if (lock->nlocks>0) {
            assert(pthread_equal(lock->owner, pthread_self()));
            lock->nlocks--;
        }

        if (0==lock->nlocks) {
            int status = pthread_rwlock_unlock(&lock->rwlock);
            assert(0==status);
            RTS_releasing(lock->layer);
        }
    } RTS_MUTEX_END;

    return 0;
#else
    return ENOSYS;
#endif
}



/******************************************************************************************************************************
 *                                      Message output
 ******************************************************************************************************************************/

RTS_rwlock_t RTS_Message::rwlock = RTS_RWLOCK_INITIALIZER(RTS_LAYER_RTS_MESSAGE_CLASS);
RTS_Message *RTS_Message::in_multi = NULL;
bool RTS_Message::sol = true;

void
RTS_Message::ctor()
{}

void
RTS_Message::dtor()
{
    RTS_WRITE(rwlock) {
        if (in_multi==this) {
            if (f)
                fputc('\n', f);
            in_multi = NULL;
            sol = true;
        }
    } RTS_WRITE_END;
    delete[] buffer;
}

void
RTS_Message::terminate()
{
    /* Internal functions assume that lock is already acquired. */
    if (f && !sol) {
        if (in_multi!=NULL) {
            if (!in_multi->name.empty())
                fprintf(f, " <%s continued below>\n", in_multi->name.c_str());
            in_multi->interrupted = true;
        } else {
            fputc('\n', f);
        }
        in_multi = NULL;
        sol = true;
    }
}

void
RTS_Message::prefix()
{
    /* Internal functions assume that lock is already acquired. */
    if (f && p)
        (*p)(f);
}

void
RTS_Message::output_lines(const char *s)
{
    /* Internal functions assume that lock is already acquired. */
    while (s && *s) {
        if (sol)
            prefix();
        const char *t = strchr(s, '\n');
        if (t) {
            fwrite(s, (t+1)-s, 1, f);
            sol = true;
        } else {
            fputs(s, f);
            sol = false;
        }
        s = t ? t+1 : NULL;
    }
    if (sol)
        in_multi = false;
}

/* We need to identical va_list args because some systems do not allow it to be reused after calling vsnprintf() */
void
RTS_Message::format(const char *fmt, va_list ap1, va_list ap2)
{
    if (!buffer) {
        bufsz = 128;
        buffer = new char[bufsz];
    }
    size_t nchars = vsnprintf(buffer, bufsz, fmt, ap1);
    if (nchars >= bufsz) {
        bufsz = std::max(2*bufsz, nchars+1);
        delete[] buffer;
        buffer = new char[bufsz];
        nchars = vsnprintf(buffer, bufsz, fmt, ap2);
        assert(nchars < bufsz);
    }
}

RTS_Message &
RTS_Message::multipart(const std::string &name, const char *fmt, ...)
{
    if (!f)
        return *this;

    va_list ap1, ap2;
    va_start(ap1, fmt);
    va_start(ap2, fmt);
    format(fmt, ap1, ap2);
    va_end(ap1);
    va_end(ap2);

    if (0==bufsz || !buffer[0])
        return *this;

    RTS_WRITE(rwlock) {
        terminate();
        output_lines(buffer);
        this->name = name;
        in_multi = sol ? NULL : this;
        interrupted = false;
    } RTS_WRITE_END;
    return *this;
}

RTS_Message &
RTS_Message::more(const char *fmt, ...)
{
    if (!f)
        return *this;

    va_list ap1, ap2;
    va_start(ap1, fmt);
    va_start(ap2, fmt);
    format(fmt, ap1, ap2);
    va_end(ap1);
    va_end(ap2);

    if (0==bufsz || !buffer[0])
        return *this;

    RTS_WRITE(rwlock) {
        if (in_multi && in_multi!=this)
            terminate();
        if (interrupted) {
            prefix();
            if (!name.empty())
                fprintf(f, "<%s resumed> ", name.c_str());
            sol = false;
        }
        output_lines(buffer);
        in_multi = sol ? NULL : this;
        interrupted = false;
    } RTS_WRITE_END;
    return *this;
}

void
RTS_Message::multipart_end()
{
    RTS_WRITE(rwlock) {
        name = "";
        if (in_multi==this) {
            if (f && !sol)
                fputc('\n', f);
            in_multi = NULL;
            sol = true;
        }
    } RTS_WRITE_END;
}

RTS_Message &
RTS_Message::mesg(const char *fmt, ...)
{
    if (!f)
        return *this;

    va_list ap1, ap2;
    va_start(ap1, fmt);
    va_start(ap2, fmt);
    format(fmt, ap1, ap2);
    va_end(ap1);
    va_end(ap2);

    if (0==bufsz || !buffer[0])
        return *this;

    RTS_WRITE(rwlock) {
        terminate();
        output_lines(buffer);
        if (!sol)
            output_lines("\n");
        in_multi = false;
    } RTS_WRITE_END;
    return *this;
}

void
RTS_Message::brief_begin(const char *fmt, ...)
{
    if (!f)
        return;

    va_list ap1, ap2;
    va_start(ap1, fmt);
    va_start(ap2, fmt);
    format(fmt, ap1, ap2);
    va_end(ap1);
    va_end(ap2);

    char *eol = buffer ? strchr(buffer, '\n') : NULL;
    if (eol)
        *eol = '\0';

    /* Obtain lock; to be released by brief_end() */
    int err = RTS_rwlock_wrlock(&rwlock);
    assert(0==err);

    fputc('[', f);
    if (in_multi!=this)
        prefix();
    if (buffer)
        fputs(buffer, f);
    sol = false;
}

void
RTS_Message::brief_end(const char *fmt, ...)
{
    if (!f)
        return;
    
    va_list ap1, ap2;
    va_start(ap1, fmt);
    va_start(ap2, fmt);
    format(fmt, ap1, ap2);
    va_end(ap1);
    va_end(ap2);

    char *eol = buffer ? strchr(buffer, '\n') : NULL;
    if (eol)
        *eol = '\0';

    /* Lock was acquired by brief_begin() */
    if (buffer)
        fputs(buffer, f);
    fputc(']', f);
    sol = false;

    int err = RTS_rwlock_unlock(&rwlock);
    assert(0==err);
}

RTS_Message &
RTS_Message::brief(const char *fmt, ...)
{
    if (!f)
        return *this;

    va_list ap1, ap2;
    va_start(ap1, fmt);
    va_start(ap2, fmt);
    format(fmt, ap1, ap2);
    va_end(ap1);
    va_end(ap2);

    if (0==bufsz || !buffer[0])
        return *this;

    char *eol = strchr(buffer, '\n');
    if (eol)
        *eol = '\0';

    RTS_WRITE(rwlock) {
        fputc('[', f);
        if (in_multi!=this)
            prefix();
        fputs(buffer, f);
        fputc(']', f);
        sol = false;
    } RTS_WRITE_END;
    return *this;
}

int
RTS_Message::lock()
{
    return RTS_rwlock_wrlock(&rwlock);
}

int
RTS_Message::unlock(bool sol)
{
    this->sol = sol;
    in_multi = sol ? NULL : this;
    return RTS_rwlock_unlock(&rwlock);
}
