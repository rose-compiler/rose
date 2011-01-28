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
 *      forms of premature exit should behave as if the body executed to completion (except in the case of throw, the exception
 *      is raised again automatically).
 */

#include <pthread.h>

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
#define RTS_MUTEX(MUTEX)                                                                                                       \
    do {        /* standard CPP macro protection */                                                                            \
        pthread_mutex_t *RTS_Mp_mutex = &MUTEX; /* saved for when we need to unlock it */                                      \
        int RTS_Mp_err = pthread_mutex_lock(RTS_Mp_mutex);                                                                     \
        do {    /* so we can catch "break" statements */                                                                       \
            assert(0==RTS_Mp_err);                                                                                             \
            try {                                                                                                              \

/** End an RTS_MUTEX construct. */
#define RTS_MUTEX_END                                                                                                          \
            } catch (...) {                                                                                                    \
                RTS_Mp_err = pthread_mutex_unlock(RTS_Mp_mutex);                                                               \
                assert(0==RTS_Mp_err);                                                                                         \
                throw;                                                                                                         \
            };                                                                                                                 \
        } while (0);                                                                                                           \
        RTS_Mp_err = pthread_mutex_unlock(RTS_Mp_mutex);                                                                       \
        assert(0==RTS_Mp_err);                                                                                                 \
    } while (0)


/** Initializer synchronization.
 *
 *  Sometimes we want a critical section to be executed only by the first thread to call the function and all other threads
 *  that might call the same function should block until the first caller completes.  These macros can be used for that
 *  purpose.
 *
 *  The MUTEX is briefly locked to inspect and initilization state, and then unlocked before the user-supplied body is
 *  executed.  The user is permitted to obtain the mutex lock again in the body if desired, although this is only necessary if
 *  other code paths (outside the RTS_INIT construct) might interfere with the body.
 *
 *  If ALLOW_RECURSION is true, then a recursive call from the body will jump over the RTS_INIT construct without doing
 *  anything (other than briefly obtaining the mutex lock to inspect the initialized state). Otherwise a recursive call from
 *  the body is considered a logic error and the process will be aborted. For convenience, we define two additional macros:
 *  RTS_INIT_RECURSIVE and RTS_INIT_NONRECURSIVE.
 *
 *  The user-supplied body may exit prematurely either by a "break" statement or by throwing an exception.  In either case, the
 *  initialization is assumed to have completed and the body will not be executed by any other future call.
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
 *  @endcode
 */
#define RTS_INIT(MUTEX, ALLOW_RECURSION)                                                                                       \
    do {                                                                                                                       \
        static bool RTS_Is_initialized=false, RTS_Is_initializing=false;        /* "s"==shared; "p"=private */                 \
        static pthread_t RTS_Is_initializer;                                                                                   \
        static pthread_cond_t RTS_Is_condition=PTHREAD_COND_INITIALIZER;                                                       \
        pthread_mutex_t *RTS_Ip_mutex = &MUTEX;                                                                                \
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
#define RTS_INIT_END                                                                                                           \
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

#define RTS_INIT_RECURSIVE(MUTEX)       RTS_INIT(MUTEX, true)                   /**< See RTS_INIT */
#define RTS_INIT_NONRECURSIVE(MUTEX)    RTS_INIT(MUTEX, false)                  /**< See RTS_INIT */


#endif /* !ROSE_threadSupport_H !*/
