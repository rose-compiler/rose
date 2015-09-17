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
#ifdef _REENTRANT                                       // Does user want multi-thread support? (e.g., g++ -pthread)
# ifdef ROSE_HAVE_PTHREAD_H                             // Do we have POSIX threads? Consider using Boost Threads instead.
#  define ROSE_THREADS_ENABLED
#  define ROSE_THREADS_POSIX
#  include <pthread.h>
# else
#  undef  ROSE_THREADS_ENABLED
# endif
#else
# undef ROSE_THREADS_ENABLED
#endif

/* The __attribute__ mechanism is only supported by GNU compilers */
#ifndef __GNUC__
#define  __attribute__(x)  /*NOTHING*/
#define  __attribute(x)    /*NOTHING*/
#endif

/******************************************************************************************************************************
 *                                      Layered Synchronization Primitives
 ******************************************************************************************************************************/

/** Layers where syncrhonization primitives are defined.
 *
 *  When a thread intends to acquire multiple locks at a time, it must acquire those locks in a particular order to prevent
 *  deadlock.  Deadlock can occur when thread 1 attempts to acquire lock A and then B, while thread 2 attempts to acquire lock
 *  B and then A.  By defining every lock to belong to a particular software layer, we can impose a partial ordering on the
 *  locks and enforce the requirement that a thread obtain locks in that order.  To use the previous example, if lock A belongs
 *  to layer X and lock B to layer Y, then a rule that says "locks of layer X must be acquired before locks of layer Y when
 *  attempting to acquire both at once" would be sufficient to prevent deadlock.  This mechanism makes no attempt to define an
 *  acquisition order for locks of the same layer (at least not at this time).
 *
 *  When a thread acquires locks from more than one layer at a time, they must be acquired in descending order by layer (they
 *  can be released in any order).  If a thread attempts to aquire a lock whose layer is greater than the minimum layer for
 *  which it already holds a lock, then an error message is emitted and the process aborts.
 *
 *  New layers can be added to this enum and the RTS_LAYER_NLAYERS constant can be increased if necessary.  When a layer's
 *  number is changed, all of ROSE must be recompiled.  The constant name is used in error messages. Names ending with "_CLASS"
 *  refer to synchronization primities that are class data members (or global), while those ending with "_OBJ" belong to a
 *  particular object.
 *
 *  Layer zero is special and is the default layer for all syncronization primitives not explicitly associated with any layer.
 *  Locks in layer zero can be acquired in any order without generating an error message (so silent deadlock is a distinct
 *  possibility). */
enum RTS_Layer {
    RTS_LAYER_DONTCARE = 0,

    /* ROSE library layers, 100-199 */
    RTS_LAYER_ROSE_CALLBACKS_LIST_OBJ   = 100,          /**< ROSE_Callbacks::List class */
    RTS_LAYER_ROSE_SMT_SOLVERS          = 115,          /**< SMTSolver class */

    /* Simulator layers (see projects/simulator), 200-220
     *
     * Constraints:
     *     RSIM_PROCESS_OBJ        < RSIM_PROCESS_CLONE_OBJ
     *     RSIM_SIGNALHANDLING_OBJ < RSIM_PROCESS_OBJ
     */

    /* User layers.  These are for people that might want to use the ROSE Thread Support outside ROSE, such as in ROSE
     * projects.   We leave it up to them to organize how they'll use the available layers. */
    RTS_LAYER_USER_MIN                  = 250,          /**< Minimum layer for end-user usage. */
    RTS_LAYER_USER_MAX                  = 299,          /**< Maximum layer for end-user usage. */

    /* Max number of layers (i.e., 0 through N-1) */
    RTS_LAYER_NLAYERS                   = 300
};

/** Check for layering violations.  This should be called just before any attempt to acquire a lock.  The specified layer
 *  should be the layer of the lock being acquired.  Returns true if it is OK to acquire the lock, false if doing so could
 *  result in deadlock.  Before returning false, an error message is printed to stderr.
 *
 *  Note that this function is a no-op when the compiler does not support the "__thread" type qualifier, nor any other
 *  qualifier as detected by the ROSE configure script.  Currently, this is a no-op on Mac OS X. [RPM 2011-05-04] */
bool RTS_acquiring(RTS_Layer);

/** Notes the release of a lock.  This function should be called before or after each release of a lock.  The layer number is
 *  that of the lock which is release. */
void RTS_releasing(RTS_Layer);

#endif /* !ROSE_threadSupport_H !*/
