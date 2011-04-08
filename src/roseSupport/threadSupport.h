/* Thread support for ROSE */
#ifndef ROSE_threadSupport_H
#define ROSE_threadSupport_H

/* Design rules:
 *   1. All public symbols in this file should use the "RTS_" prefix (ROSE Thread Support).
 *
 *   2. All constructs that have a user-supplied compound statement as a "body", have a matching "_END" macro.  For instance,
 *      RTS_MUTEX starts a mutual exclusion for a critical section, which ends with an RTS_MUTEX_END macro.  The END macros
 *      generally take no arguments.
 *
 *   3. Locally scoped symbols defined by the macros have names beginning with "RTS_". This is generally followed by the first
 *      letter of the rest of the macro name, a letter "s" or "p" for "shared" or "private", and an underscore.  For example,
 *      within the RTS_MUTEX macros, a private (non-shared) variable might be named "RTS_Mp_mutex".
 *
 *   4. Constructs that allow a user-supplied compound statement as a "body" should allow the body to "break" or "throw". Both
 *      forms of premature exit should behave as if the body executed to completion (except throw will throw the exception
 *      again automatically).
 *
 *   5. New functionality shall have types, constants, and functions reminiscent of the POSIX threads interface, but whose
 *      names begin with "RTS_" rather than "pthread_".
 */

/* Needed for ROSE_HAVE_PTHREAD_H definition */
#include "rosePublicConfig.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>

/* Figure out whether ROSE can support multi-threading and what kind of support library is available. */
#ifdef ROSE_HAVE_PTHREAD_H
#  define ROSE_THREADS_ENABLED
#  define ROSE_THREADS_POSIX
#  include <pthread.h>
#else
#  undef  ROSE_THREADS_ENABLED
#endif

/* This warning is in a public header file so that end users will see it when they compile against a version of ROSE that
 * doesn't have multi-thread support.  It would be imprudent to move this to one of the library's *.C files because then
 * an end user might spend substantial time trying to figure out why his multi-threaded program fails nondeterministically when
 * the ROSE documentation advertises that certain functions are thread safe.
 *
 * Unfortunately, due to the way ROSE's header files are organized, threadSupport.h will be included by pretty much every
 * ROSE library source file because every file includes _all_ the Sage node definitions (instead of only the ones it needs),
 * and a few of those nodes (e.g., SgFile) depend on something defined by the Disassembler class. The Disassembler supports
 * multi threading and therefore includes this header. Therefore every ROSE library source file will spit out this warning. */
#ifndef ROSE_THREADS_ENABLED
#  warning "Multi-thread support is not enabled. ROSE will not be thread safe even for functions that advertise safety."
#endif

/******************************************************************************************************************************
 *                                      Paired macros for using pthread_mutex_t
 ******************************************************************************************************************************/

/** Protect a critical section with a mutual exclusion lock.
 *
 *  This macro should be used within ROSE whenever we need to obtain a lock for a critical section. The critical section should
 *  end with a matching RTS_MUTEX_END macro.  The suggested code style is to use curly braces and indentation to help visually
 *  line up the RTS_MUTEX with the RTS_MUTEX_END, such as:
 *
 *  @code
 *  RTS_MUTEX(class_mutex) {
 *      critical_section_goes_here;
 *  } RTS_MUTEX_END;
 *  @endcode
 *
 *  The critical section should not exit the construct except through the RTS_MUTEX_END macro.  In other words, the critical
 *  section should not have "return" statements, longjmps, or any "goto" that branches outside the critical section.  However,
 *  "break" statements and exceptions are supported.
 *
 *  If the mutex is an error checking mutex then ROSE will assert that the lock is not already held by this thread. If the
 *  mutex is recursive then the lock will be obtained recursively if necessary.
 */
#ifdef ROSE_THREADS_ENABLED
#  define RTS_MUTEX(MUTEX)                                                                                                     \
    do {        /* standard CPP macro protection */                                                                            \
        pthread_mutex_t *RTS_Mp_mutex = &(MUTEX); /* saved for when we need to unlock it */                                    \
        int RTS_Mp_err = pthread_mutex_lock(RTS_Mp_mutex);                                                                     \
        assert(0==RTS_Mp_err);                                                                                                 \
        do {    /* so we can catch "break" statements */                                                                       \
            try {                                                                                                              \

/** End an RTS_MUTEX construct. */
#  define RTS_MUTEX_END                                                                                                        \
            } catch (...) {                                                                                                    \
                RTS_Mp_err = pthread_mutex_unlock(RTS_Mp_mutex);                                                               \
                assert(0==RTS_Mp_err);                                                                                         \
                throw;                                                                                                         \
            }                                                                                                                  \
        } while (0);                                                                                                           \
        RTS_Mp_err = pthread_mutex_unlock(RTS_Mp_mutex);                                                                       \
        assert(0==RTS_Mp_err);                                                                                                 \
    } while (0)
#else
#  define RTS_MUTEX(MUTEX)                                                                                                     \
    do {
#  define RTS_MUTEX_END                                                                                                        \
    } while (0)
#endif

/******************************************************************************************************************************
 *                                      Types and functions for mutexes
 ******************************************************************************************************************************/

#if !defined(ROSE_THREADS_POSIX) && !defined(PTHREAD_MUTEX_INITIALIZER)
typedef int pthread_mutex_t;
#define PTHREAD_MUTEX_INITIALIZER 0
#endif

/*******************************************************************************************************************************
 *                                      Paired macros for using RTS_rwlock_t
 *******************************************************************************************************************************/

/** Protect a critical section with a read-write lock.
 *
 *  These macros should be used within ROSE whenever we need to protect a critical section among two kinds of access: reading
 *  and writing.
 *
 *  This construct allows at most one thread to hold a write lock, or multiple threads to hold read locks.  Write locks are
 *  granted only when no other thread holds a read or write lock, and a request for a write lock blocks (becomes pending) if
 *  the lock cannot be granted.  Read locks are granted only when no write lock is already granted to another thread, and no
 *  write lock is pending.
 *
 *  Like POSIX read-write locks, RTS_rwlock_t allows a single thread to obtain multiple read locks recursively. Unlike POSIX
 *  read-write locks, RTS_rwlock_t also allows the following:
 *
 *  <ul>
 *    <li>A lock (read or write) is granted to a thread which already holds a write lock.  The POSIX implementation deadlocks
 *        in this situation.  This feature is useful in ROSE when a read-write lock guards access to data members of an object,
 *        and the object methods can be invoked recursively.</li>
 *    <li>The RTS_rwlock_unlock() function releases locks in the reverse order they were granted and should only be called by
 *        the thread to which the lock was granted.</li>
 *  </ul>
 *
 *  In particular, this implementation does not allow a thread which holds only read locks to be granted a write lock (i.e., no
 *  lock upgrading). Like POSIX read-write locks, this situation will lead to deadlock.
 *
 *  The RTS_READ macro should be paired with an RTS_READ_END macro; the RTS_WRITE macro should be paired with an RTS_WRITE_END
 *  macro.  The RTS_RWLOCK macro is a generalization of RTS_READ and RTS_WRITE where its second argument is either the word
 *  "rdlock" or "wrlock", respectively. It should be paired with an RTS_RWLOCK_END macro.
 *
 *  The critical section may exit only via "break" statement, throwing an exception, or falling through the end.  Exceptions
 *  thrown by the critical section will release the lock before rethrowing the exception.
 *
 *  A simple example demonstrating how locks can be obtained recursively.  Any number of threads can be operating on a single,
 *  common object concurrently and each of the four defined operations remains atomic.
 *
 *  @code
 *  class Stack {
 *  public:
 *      Stack() {
 *          RTS_rwlock_init(&rwlock, NULL);
 *      }
 *  
 *      MD5sum sum() const {
 *          MD5sum retval;
 *          RTS_READ(rwlock) {
 *              for (size_t i=0; i<stack.size(); i++)
 *                  retval.composite(stack[i]);
 *          } RTS_READ_END;
 *          return retval;
 *      }
 *
 *      void push(const std::string &s) {
 *          RTS_WRITE(rwlock) {
 *              stack.push_back(s);
 *          } RTS_WRITE_END;
 *      }
 *  
 *      // swap top and third from top; toss second from top
 *      // throw exception when stack becomes too small
 *      void adjust() {
 *          RTS_WRITE(rwlock) {
 *              std::string s1 = pop(); // may throw
 *              (void) pop();           // may throw
 *              std::string s2 = pop(); // may throw
 *              push(s1);
 *              push(s2);
 *          } RTS_WRITE_END;
 *      }
 *
 *      // adjust() until sum specified termination condition
 *      // throw exception when stack becomes too small
 *      void adjust_until(const MD5sum &term) {
 *          RTS_WRITE(rwlock) {
 *              while (sum()!=term)    // recursive read lock
 *                  adjust();          // recursive write lock; may throw
 *          } RTS_WRITE_END;
 *      }
 *  
 *  private:
 *      std::vector<std::string> stack;
 *      mutable RTS_rwlock_t rwlock;   // mutable so sum() can be const as user would expect
 *  };
 *  @endcode
 */
#ifdef ROSE_THREADS_ENABLED
#  define RTS_RWLOCK(RWLOCK, HOW)                                                                                              \
    do {        /* standard CPP macro protection */                                                                            \
        RTS_rwlock_t *RTS_Wp_rwlock = &(RWLOCK); /* saved for when we need to unlock it */                                     \
        int RTS_Wp_err = RTS_rwlock_##HOW(RTS_Wp_rwlock);                                                                      \
        assert(0==RTS_Wp_err);                                                                                                 \
        do {    /* so we can catch "break" statements */                                                                       \
            try {
                
#  define RTS_RWLOCK_END                                                                                                       \
            } catch (...) {                                                                                                    \
                RTS_Wp_err = RTS_rwlock_unlock(RTS_Wp_rwlock);                                                                 \
                assert(0==RTS_Wp_err);                                                                                         \
                throw;                                                                                                         \
            }                                                                                                                  \
        } while (0);                                                                                                           \
        RTS_Wp_err = RTS_rwlock_unlock(RTS_Wp_rwlock);                                                                         \
        assert(0==RTS_Wp_err);                                                                                                 \
    } while (0)

#else
#  define RTS_RWLOCK(RWLOCK, HOW)                                                                                              \
    do {
#  define RTS_RWLOCK_END                                                                                                       \
    } while (0)
#endif

#define RTS_READ(RWLOCK)        RTS_RWLOCK(RWLOCK, rdlock)                      /**< See RTS_RWLOCK. */
#define RTS_READ_END            RTS_RWLOCK_END                                  /**< See RTS_RWLOCK. */
#define RTS_WRITE(RWLOCK)       RTS_RWLOCK(RWLOCK, wrlock)                      /**< See RTS_RWLOCK. */
#define RTS_WRITE_END           RTS_RWLOCK_END                                  /**< See RTS_RWLOCK. */

/*******************************************************************************************************************************
 *                                      Types and functions for RTS_rwlock_t
 *
 * Programmers should generally use the RTS_READ and RTS_WRITE macros in their source code. The symbols defined here are
 * similar to pthread_rwlock* symbols and are mostly to support RTS_READ and RTS_WRITE macros (like how the pthread_mutex*
 * symbols support the RTS_MUTEX macro).
 *******************************************************************************************************************************/

/** A read-write lock for ROSE Thread Support.  As with POSIX Thread types, this type should be treated as opaque and
 *  initialized with RTS_RWLOCK_INITIALIZER or RTS_rwlock_init().  It is used as the argument for RTS_READ and RTS_WRITE
 *  macros. */
#ifdef ROSE_THREADS_ENABLED
struct RTS_rwlock_t {
    pthread_rwlock_t rwlock;                    /* the main read-write lock */
    pthread_mutex_t mutex;                      /* mutex to protect the following data members */
    static const size_t max_nlocks = 64;        /* max number of locks: 8*sizeof(lock_types) */
    size_t nlocks;                              /* number of write locks held */
    pthread_t owner;                            /* thread that currently holds the write lock */
};
#else
struct RTS_rwlock_t {
    int dummy;
};
#endif

/** Static initializer for an RTS_rwlock_t instance, similar in nature to PTHREAD_RWLOCK_INITIALIZER. */
#ifdef ROSE_THREADS_ENABLED
#  define RTS_RWLOCK_INITIALIZER { PTHREAD_RWLOCK_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, 0/*...*/ }
#else
#  define RTS_RWLOCK_INITIALIZER { 0 }
#endif

#ifdef ROSE_THREADS_ENABLED
/** Intializes an RTS_rwlock_t in a manner similar to pthread_rwlock_init(). */
int RTS_rwlock_init(RTS_rwlock_t *rwlock, pthread_rwlockattr_t *wrlock_attrs);
#else
int RTS_rwlock_init(RTS_rwlock_t *rwlock, void                 *wrlock_attrs);
#endif
              
/** Obtain a read lock.
 *
 *  The semantics are identical to pthread_rwlock_rdlock() documented in "The Open Group Base Specifications Issue 6: IEEE Std
 *  1003.1, 2004 Edition" [1] with the following changes:
 *
 *  <ul>
 *    <li>If the calling thread holds a write lock, then the read lock is automatically granted.  A single process can be both
 *        reading and writing, and may hold multiple kinds locks.</li>
 *    <li>Calls to RTS_rwlock_unlock() release the recursive locks in the opposite order they were obtained.</li>
 *  </ul>
 *
 *  [1] http://pubs.opengroup.org/onlinepubs/009695399/functions/pthread_rwlock_rdlock.html
 */
int RTS_rwlock_rdlock(RTS_rwlock_t *rwlock);

/** Obtain a write lock.
 * 
 *  The semantics are identical to pthread_rwlock_wrlock() documented in "The Open Group Base Specifications Issue 6: IEEE Std
 *  1003.1, 2004 Edition" [1] with the following changes:
 *
 *  <ul>
 *    <li>If the calling thread already holds a write lock, then a recursive write lock is automatically granted.  A single
 *        process can hold multiple write locks.</li>
 *    <li>Calls to RTS_rwlock_unlock() release the recursive locks in the opposite order they were obtained.</li>
 *  </ul>
 *
 *  Note that a write lock will not be granted if a thread already holds only read locks.  Attempting to obtain a write lock in
 *  this situation will result in deadlock.
 *
 *  [1] http://pubs.opengroup.org/onlinepubs/009695399/functions/pthread_rwlock_wrlock.html
 */
int RTS_rwlock_wrlock(RTS_rwlock_t *rwlock);

/** Release a read or write lock.
 *
 *  The semantics are identical to pthread_rwlock_unlock() documented in "The Open Group Base Specification Issue 6: IEEE Std
 *  1003.1, 2004 Edition" [1] with the following additions:
 *
 *  <ul>
 *    <li>Recursive locks are released in the oposite order they were acquired.</li>
 *  </ul>
 *
 *  [1] http://pubs.opengroup.org/onlinepubs/009695399/functions/pthread_rwlock_unlock.html
 */
int RTS_rwlock_unlock(RTS_rwlock_t *rwlock);



/*******************************************************************************************************************************
 *                                      Paired macros for initialization functions
 *******************************************************************************************************************************/


/** Initializer synchronization.
 *
 *  Sometimes we want a critical section to be executed only by the first thread to call the function and all other threads
 *  that might call the same function should block until the first caller completes.  These macros can be used for that
 *  purpose.
 *
 *  The MUTEX is briefly locked to inspect the state of initilization, and then unlocked before the user-supplied body is
 *  executed.  The user is permitted to obtain the mutex lock again in the body if desired, although this is only necessary if
 *  other code paths (outside the RTS_INIT construct) might interfere with the body.
 *
 *  If ALLOW_RECURSION is true, then a recursive call from the body will jump over the RTS_INIT construct without doing
 *  anything (other than briefly obtaining the mutex lock to inspect the state of initialization). Otherwise a recursive call
 *  from the body is considered a logic error and the process will be aborted. For convenience, we define two additional
 *  macros, RTS_INIT_RECURSIVE and RTS_INIT_NONRECURSIVE, which may be used in place of the two-argument RTS_INIT macro.
 *
 *  The user-supplied body may exit prematurely either by a "break" statement or by throwing an exception.  In either case, the
 *  initialization is assumed to have completed and the body will not be executed by any other future call.  Any other kind of
 *  premature exit from the body (return, goto, longjmp, etc) results in undefined behavior.
 *
 *  Example code. Consider a class method which is responsible for one-time initialization of certain class data
 *  structures. This initialization function is called by nearly every other method in the class, and therefore anything that
 *  the initialization function does might result in a recursive call to the initializer.
 *
 *  @code
 *  static void
 *  SomeClass::initclass()
 *  {
 *      RTS_INIT_RECURSIVE(class_mutex) {
 *          register_subclass(new Subclass1);
 *          register_subclass(new Subclass2);
 *          register_subclass(new Subclass3);
 *      } RTS_INIT_END;
 *  }
 *  @endcode
 */
#ifdef ROSE_THREADS_ENABLED
#  define RTS_INIT(MUTEX, ALLOW_RECURSION)                                                                                     \
    do {                                                                                                                       \
        static bool RTS_Is_initialized=false, RTS_Is_initializing=false;        /* "s"==shared; "p"=private */                 \
        static pthread_t RTS_Is_initializer;                                                                                   \
        static pthread_cond_t RTS_Is_condition=PTHREAD_COND_INITIALIZER;                                                       \
        pthread_mutex_t *RTS_Ip_mutex = &(MUTEX);                                                                              \
        bool RTS_Ip_initialized, RTS_Ip_initializing;                                                                          \
        bool RTS_Ip_allow_recursion = (ALLOW_RECURSION);                                                                       \
                                                                                                                               \
        /* First critical section is only to obtain the initialization status and update it to "initializing" if necessary. We \
         * must release the lock before the RTS_I_LOCK body is executed in case we need to handle recursive calls to the       \
         * RTS_I_LOCK construct. */                                                                                            \
        RTS_MUTEX(MUTEX) {                                                                                                     \
            if (!(RTS_Ip_initialized=RTS_Is_initialized) && !(RTS_Ip_initializing=RTS_Is_initializing)) {                      \
                RTS_Is_initializing = true; /* but leave private copy false so we can detect changed state */                  \
                RTS_Is_initializer = pthread_self();                                                                           \
            }                                                                                                                  \
        } RTS_MUTEX_END;                                                                                                       \
                                                                                                                               \
        if (!RTS_Ip_initialized) {                                                                                             \
            if (!RTS_Ip_initializing) {                                                                                        \
                do { /* so we catch "break" statements in user-supplied code. */                                               \
                    try {


/** End an RTS_INIT construct. */
#  define RTS_INIT_END                                                                                                         \
                    } catch (...) {                                                                                            \
                        /* If the user supplied body throws an exception, consider the body to have completed. */              \
                        RTS_MUTEX(*RTS_Ip_mutex) {                                                                             \
                            RTS_Is_initialized = true;                                                                         \
                            RTS_Is_initializing = false;                                                                       \
                            pthread_cond_broadcast(&RTS_Is_condition);                                                         \
                        } RTS_MUTEX_END;                                                                                       \
                        throw;                                                                                                 \
                    }                                                                                                          \
                } while (0);                                                                                                   \
                RTS_MUTEX(*RTS_Ip_mutex) {                                                                                     \
                    RTS_Is_initialized = true;                                                                                 \
                    RTS_Is_initializing = false;                                                                               \
                    pthread_cond_broadcast(&RTS_Is_condition);                                                                 \
                } RTS_MUTEX_END;                                                                                               \
            } else if (pthread_equal(pthread_self(), RTS_Is_initializer)) {                                                    \
                /* This was a recursive call resulting from the user-supplied RTS_I body and we must allow it to continue      \
                 * unimpeded to prevent deadlock. We don't need to be holding the MUTEX lock to access RTS_Is_initializer      \
                 * because it's initialized only the first time through the critical section above--and the only way to reach  \
                 * this point is by this or some other thread having already initialized RTS_Is_initializer. */                \
                if (!RTS_Ip_allow_recursion) {                                                                                 \
                    fprintf(stderr, "RTS_I_LOCK body made a recursive call. Aborting...\n");                                   \
                    abort();                                                                                                   \
                }                                                                                                              \
            } else {                                                                                                           \
                /* This is some other thread which is not in charge of initializing the class, bug which arrived to the        \
                 * RTS_I_LOCK before the first thread completed the initialization. */                                         \
                RTS_MUTEX(*RTS_Ip_mutex) {                                                                                     \
                    while (!RTS_Is_initialized)                                                                                \
                        pthread_cond_wait(&RTS_Is_condition, RTS_Ip_mutex);                                                    \
                } RTS_MUTEX_END;                                                                                               \
            }                                                                                                                  \
        }                                                                                                                      \
    } while (0)
#else
#  define RTS_INIT(MUTEX, ALLOW_RECURSION)                                                                                     \
    do {                                                                                                                       \
        static bool RTS_Is_initialized=false;                                                                                  \
        if (!RTS_Is_initialized) {                                                                                             \
            RTS_Is_initialized = true;                                                                                         \
            do {
#  define RTS_INIT_END                                                                                                         \
            } while (0);                                                                                                       \
        }                                                                                                                      \
    } while (0)
#endif

#define RTS_INIT_RECURSIVE(MUTEX)       RTS_INIT(MUTEX, true)                   /**< See RTS_INIT */
#define RTS_INIT_NONRECURSIVE(MUTEX)    RTS_INIT(MUTEX, false)                  /**< See RTS_INIT */


/******************************************************************************************************************************
 *                                      Support for messages
 ******************************************************************************************************************************/

/** Support for messages in a multi-threaded program.  This class helps messages (debugging, info, etc) produced by a
 *  multi-threaded application into meaningful output.  A typical problem one encounters is that messages, especially multi
 *  line messages produced over an indeterminate period of time (e.g., due to intervening blocking calls) get interleaved with
 *  messages from other threads and become nonsensical.  This class makes such messages understandable, provided all messages
 *  are generated with this class.
 *
 *  Note:  For the sake of determining when two messages interfere with each other, we assume that all files are sending output
 *  to the same place.  This works fine in most cases where messages are being sent to stdout and/or stderr, which often both
 *  appear in the terminal, but can result in extraneous multi-part interrupt/resume phrases when output really is going to
 *  different places. */
class RTS_Message {
public:
    /** The Prefix class is used to generate prefix text for every line of output. It is invoked with one argument: a non-null
     *  file handle.  The functor should output a short phrase (without a line feed) to identify which thread is producing the
     *  message which follows. */
    class Prefix {
    public:
        virtual ~Prefix() {};
        virtual void operator()(FILE*) {}
    };

    /** Creates a new message object.  Message objects are generally not safe to be called from multiple threads. Instead, each
     *  thread creates its own object as necessary on the stack, and destroys it when finished (usually implicitly).  A single
     *  object can handle at most one outstanding multi-part message; it can, however, output multiple messages sequentially.
     *
     *  The provided file handle is used for all output by this object.  A null file handle is permitted, in which case the
     *  output methods will become no-ops.
     *
     *  In order to associate each line of message output with a particular thread, each line of output begins with a
     *  prefix. The prefix is generated by the supplied, optional functor invoked with one argument: the file handle specified
     *  in the constructor. */
    RTS_Message(FILE *f, Prefix *p)
        : f(f), p(p), buffer(NULL), bufsz(0), interrupted(false) {
        ctor();
    }

    ~RTS_Message() {
        dtor();
    }

    /** Sets the file handle to use for subsequent messages. A null pointer disables output. */
    void set_file(FILE *f) {
        this->f = f;
    }

    /** Returns the file handle being used for messages. Returns null if messages are disabled for this object. */
    FILE *get_file() const {
        return f;
    }

    /** Begins a multi-part message. The content of a multi-part message is created by an initial call to multipart() and zero
     *  or more subsequent calls to more().  The concatentation of these calls may result in one or more lines of text, each of
     *  which is introduced by invoking the optional prefix generator supplied in the constructor. The lines of the message
     *  need not have a one-to-one correspondence with the multipart() and more() invocations.
     *
     *  If some other message is output via method invocations on some other RTS_Message object while this object's multi-part
     *  message is in the middle of a line (and between calls to multipart() or more()), this message is considered to be in an
     *  interrupted state.  When a multi-part message is interrupted, the phrase " <%s continued below>\n" is output to
     *  terminate the line, where "%s" is the message name provided as the first argument to multipart().  When an interrupted
     *  message is resumed by a later call to more(), the additional text will be introduced by first invoking the optional
     *  prefix generator, then writing the phrase "<%s resumed> ".  If a name is not provided then the continued and resumed
     *  phrases are not emitted.
     *
     *  The @p fmt and following arguments are the same as for printf().
     *
     *  No output is produced and the lock is not acquired if the file pointer provided in the constructor is a null pointer.
     *
     *  No output is produced if the message is zero length.
     *
     *  The return value is always this object, allowing most of these methods to be strung together in a single expression. */
    RTS_Message& multipart(const std::string &name, const char *fmt, ...);

    /** Continues a multi-part message. Each new line in the message begins with a prefix generated by invoking the optional
     *  prefix functor supplied in the constructor.  If this multi-part message was interrupted since the last output, then the
     *  phrase "<%s resumed> " is printed first, where "%s" is the name supplied to the previous multipart() invocation.  If no
     *  name was supplied then no resumed phrase is emitted.
     *
     *  The @p fmt and following arguments are the same as for printf().
     *
     *  No output is produced and the lock is not acquired if the file pointer provided in the constructor is a null pointer.
     *
     *  No output is produced if the message is zero length.
     *
     *  The return value is always this object, allowing most of these methods to be strung together in a single expression. */
    RTS_Message& more(const char *fmt, ...);

    /** Terminates a multi-part message. This doesn't generally need to be called. If not called, and brief() is used after the
     *  logical end of the multipart messsage, then the next call to multipart() or mesg() will result in a continuation
     *  phrase. */
    void multipart_end();

    /** Outputs a single-line message. This method acquires the class lock, outputs the specified message, outputs a line feed
     *  if necessary, and then releases the lock.  The message may consist of multiple lines, and each line will be prefixed by
     *  invoking the optional prefix functor specified in the constructor.  This message is allowed to interrupt other
     *  multi-part messages that might be under construction from either this object, or another object perhaps even in another
     *  thread.
     *
     *  The @p fmt and following arguments are the same as for printf().
     *
     *  No output is produced and the lock is not acquired if the file pointer provided in the constructor is a null pointer.
     *
     *  No output is produced if the message is zero length.
     *
     *  The return value is always this object, allowing most of these methods to be strung together in a single expression. */
    RTS_Message& mesg(const char *fmt, ...);

    /** Outputs a short note. A note is a single-line (usually only a few words) phrase that will be output enclosed in square
     *  brackets and no white space on either side (unless white space was already produced by some other method).  If this
     *  message interrupts a multi-part message from this same object then only the square brackets and message are printed;
     *  otherwise the optional, constructor-supplied prefix functor is invoked to provide context information between the
     *  opening square bracket and the message.  When this kind of message interrupts a multi-part message, this message is
     *  output in-line with the multi-part and is not considered to truly interrupt the multi-part message.
     *
     *  The @p fmt and following arguments are the same as for printf().
     *
     *  No output is produced and the lock is not acquired if the file pointer provided in the constructor is a null pointer.
     *
     *  No output is produced if the message is zero length.
     *
     *  The return value is always this object, allowing most of these methods to be strung together in a single expression. */
    RTS_Message& brief(const char *fmt, ...);

    /** Obtains the lock and starts a brief message. The lock is released by calling brief_end().  This method is provided for
     *  situations when a brief message cannot be specified in a single format, or when output requires functions that are not
     *  part of this class. */
    void brief_begin(const char *fmt, ...);

    /** Complets a brief message and releases the lock. This must be called once for each brief_begin() invocation. */
    void brief_end(const char *fmt, ...);

    /** Obtain the message class lock.  Although all of the message-producing methods automatically obtain a class lock to
     *  prevent other threads from interfering, it is sometimes useful to explicitly obtain the lock.  For instance, to produce
     *  specially formatted multi-line output using plain old printf and friends, one would obtain the lock, produce output,
     *  and then release the lock.  The lock should be held for a short amount of time since it precludes other threads from
     *  producing messages.
     *
     *  The lock is recursive; it must be unlocked the same number of times it was locked. The normal message producing methods
     *  of this class may be called while the lock is held.
     *
     *  Returns zero on success; error number on failure. */
    int lock();

    /** Releases the message class lock.  This should be called once for each time lock() was called.  When the lock is
     *  released, we need to also indicate whether the output cursor is at the start of a line; @p sol should be true only at
     *  the start of a line.
     *
     *  Returns zero on success; error number on failure. */
    int unlock(bool sol=true);

private:
    RTS_Message() {abort();}
    RTS_Message& operator=(RTS_Message&) {abort();}
    void ctor();                                        /**< Constructor helper. */
    void dtor();                                        /**< Desctructor helper. */
    void terminate();                                   /**< Terminate line if necessary. */
    void prefix();                                      /**< Print line prefix regardless of "sol" state. */
    void output_lines(const char *s);                   /**< Output one or more lines, prefix as necessary. */
    void format(const char *fmt, va_list, va_list);    /**< Formats a message into the "buffer" data member. */

    static RTS_rwlock_t rwlock;                         /**< Class-wide rwlock used as a recursive mutex. */
    static RTS_Message *in_multi;                       /**< Non-null when we inside a multi-line mesg (not between lines). */
    static bool sol;                                    /**< True when we're at the start of a line. */

    FILE *f;                                            /**< Optional file to which we write messages. */
    Prefix *p;                                          /**< Optional line prefix functor. */
    char *buffer;                                       /**< Heap-allocated buffer for holding formatted message. */
    size_t bufsz;                                       /**< Allocated size of "buffer". */
    std::string name;                                   /**< Name supplied to latest multipart() invocation. */
    bool interrupted;                                   /**< True if multi-part message was interrupted. */
};

/******************************************************************************************************************************
 *                                      Paired macros for messages
 ******************************************************************************************************************************/

//@{
/** Provides a locked context for messaging.  Normal message output methods do not require locking since they perform the
 *  locking implicitly.  However, one needs to provide some kind of customized output not otherwise possible, a lock needs to
 *  be obtained while that output is produced. See RTS_Messsage::lock() for details.
 *
 *  The RTS_MESSAGE macro takes an RTS_Message object as an argument, and locks it until the corresponding RTS_MESSAGE_END
 *  macro. The RTS_MESSAGE_END macro takes one argument: a Boolean expression which evaluates to true if no output was produced
 *  or the output ended with a line-feed.
 *
 *  The body between the RTS_MESSAGE and RTS_MESSAGE_END macro should not exit non-locally except by "break" or throwing an
 *  exception, both of which release the lock.
 *
 *  Example:
 *
 *  @code
 *  RTS_Message mesg(stderr, NULL);
 *  RTS_MESSAGE(mesg) {
 *     for (size_t i=0; i<n; i++) {
 *         fprintf(stderr, "    arg(%d) = %d\n", i, arg[i]);
 *     }
 *  } RTS_MESSAGE_END(true);
 *  @endcode
 *
 *  Note that the previous example could have more easily been obtained with the following, except that in a multi-threaded
 *  application another thread might interject its own output between these lines.
 *  @code
 *  RTS_Message mesg(stderr, NULL);
 *  for (size_t i=0; i<n; i++) {
 *      mesg.mesg("    arg(%d) = %d\n", i, arg[i]);
 *  }
 *  @endcode
 */
#ifdef ROSE_THREADS_ENABLED
#  define RTS_MESSAGE(MESG)                                                                                                    \
    do {                                                                                                                       \
        RTS_Message *RTS_Mp_mesg = &(MESG);                                                                                    \
        int RTS_Mp_err = RTS_Mp_mesg->lock();                                                                                  \
        assert(0==RTS_Mp_err);                                                                                                 \
        do {                                                                                                                   \
            try {

#  define RTS_MESSAGE_END(SOL)                                                                                                 \
            } catch (...) {                                                                                                    \
                RTS_Mp_err = RTS_Mp_mesg->unlock((SOL));                                                                       \
                assert(0==RTS_Mp_err);                                                                                         \
                throw;                                                                                                         \
            }                                                                                                                  \
        } while (0);                                                                                                           \
        RTS_Mp_err = RTS_Mp_mesg->unlock((SOL));                                                                               \
        assert(0==RTS_Mp_err);                                                                                                 \
    } while (0)
#else
#  define RTS_MESSAGE(MESG)                                                                                                    \
    do {
#  define RTS_MESSAGE_END(SOL)                                                                                                 \
    } while (0)
#endif
//@}


        
#endif /* !ROSE_threadSupport_H !*/
