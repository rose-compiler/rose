#include "threadSupport.h"
#include "stringify.h"

#include <errno.h>
#include <stdarg.h>
#include <string.h>

using namespace rose;

/******************************************************************************************************************************
 *                                      Layered Synchronization Primitives
 ******************************************************************************************************************************/

static const unsigned LockedLayers_MAGIC = 0xee2894f0;

struct LockedLayers {
    unsigned magic;                             /* Always LockedLayers_MAGIC */
    bool active;                                /* True when this struct is being used, false initially */
    size_t total_locks;                         /* Sum across nlocks[] */
    size_t nlocks[RTS_LAYER_NLAYERS];           /* Number of locks acquired by this thread for each layer. */
    RTS_Layer min_locked_layer;                 /* Minimum index where nlocks[] is non-zero */
};

#ifdef ROSE_THREAD_LOCAL_STORAGE
static ROSE_THREAD_LOCAL_STORAGE LockedLayers locked_layers; /* Every thread must have its own private copy. */
#endif

bool
RTS_acquiring(RTS_Layer layer)
{
    bool retval = true;

#ifdef ROSE_THREAD_LOCAL_STORAGE
    static const bool allow_recursion = false;

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
        if (!allow_recursion || 0==locked_layers.nlocks[layer]) {
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
        }
        locked_layers.nlocks[layer]++;
        locked_layers.total_locks++;
    } else {
        locked_layers.nlocks[layer]++;
        locked_layers.total_locks++;
        locked_layers.min_locked_layer = layer;
    }
#else
#  ifdef _MSC_VER
#    pragma message("Layered lock aquisition is not asserted.")
#  else
#    warning "Layered lock aquisition is not asserted."
#  endif
#endif

    assert(retval); /* DEBUGGING [2011-04-29] */
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

#ifdef ROSE_THREAD_LOCAL_STORAGE
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
#endif
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
RTS_mutex_init(RTS_mutex_t*, RTS_Layer, void*)
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
