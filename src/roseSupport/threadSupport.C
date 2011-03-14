#include "threadSupport.h"
#include <stdarg.h>
#include <string.h>

/******************************************************************************************************************************
 *                                      Read-write locks
 ******************************************************************************************************************************/

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



/******************************************************************************************************************************
 *                                      Message output
 ******************************************************************************************************************************/

RTS_rwlock_t RTS_Message::rwlock = RTS_RWLOCK_INITIALIZER;
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
