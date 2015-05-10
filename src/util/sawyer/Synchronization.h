// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          github.com:matzke1/sawyer.




#ifndef Sawyer_Synchronization_H
#define Sawyer_Synchronization_H

#include <sawyer/Sawyer.h>

#if SAWYER_MULTI_THREADED
#   include <boost/thread.hpp>
#   include <boost/thread/barrier.hpp>
#   include <boost/thread/condition_variable.hpp>
#   include <boost/thread/mutex.hpp>
#   include <boost/thread/locks.hpp>
#   include <boost/thread/once.hpp>
#   include <boost/thread/recursive_mutex.hpp>
#endif

namespace Sawyer {

/** Tag indicating that an algorithm or API should assume multiple threads.
 *
 *  Indicating that multiple threads are present by the use of this tag does not necessarily ensure that the affected algorithm
 *  or API is completely thread-safe. The alrogihm or API's documentation will expound on the details. */
struct MultiThreadedTag {};

/** Tag indicating that an algorithm or API can assume only a single thread.
 *
 *  This typically means that the algorithm or API will not perform any kind of synchronization itself, but requires that the
 *  callers coordinate to serialize calls to the algorithm or API. */
struct SingleThreadedTag {};

// Used internally as a mutex in a single-threaded environment.
class NullMutex {
public:
    void lock() {}
    void unlock() {}
    bool try_lock() { return true; }
};

// Used internally as a lock guard in a single-threaded environment.
class NullLockGuard {
public:
    NullLockGuard(NullMutex) {}
};

// Used internally as a barrier in a single-threaded environment.
class NullBarrier {
public:
    explicit NullBarrier(unsigned count) {
        if (count > 1)
            throw std::runtime_error("barrier would deadlock");
    }
    bool wait() {
        return true;
    }
};

/** Locks multiple mutexes. */
template<typename Mutex>
class LockGuard2 {
    Mutex &m1_, &m2_;
public:
    LockGuard2(Mutex &m1, Mutex &m2): m1_(m1), m2_(m2) {
#if SAWYER_MULTI_THREADED
        boost::lock(m1, m2);
#endif
    }
    ~LockGuard2() {
        m1_.unlock();
        m2_.unlock();
    }
};


/** Traits for thread synchronization. */
template<typename SyncTag>
struct SynchronizationTraits {};

template<>
struct SynchronizationTraits<MultiThreadedTag> {
#if SAWYER_MULTI_THREADED
    enum { SUPPORTED = 1 };
    typedef boost::mutex Mutex;
    typedef boost::recursive_mutex RecursiveMutex;
    typedef boost::lock_guard<boost::mutex> LockGuard;
    typedef boost::lock_guard<boost::recursive_mutex> RecursiveLockGuard;
    typedef boost::condition_variable_any ConditionVariable;
    typedef boost::barrier Barrier;
#else
    enum { SUPPORTED = 0 };
    typedef NullMutex Mutex;
    typedef NullMutex RecursiveMutex;
    typedef NullLockGuard LockGuard;
    typedef NullLockGuard RecursiveLockGuard;
    //typedef ... ConditionVariable; -- does not make sense to use this in a single-threaded program
    typedef NullBarrier Barrier;
#endif
};


template<>
struct SynchronizationTraits<SingleThreadedTag> {
    enum { SUPPORTED = 0 };
    typedef NullMutex Mutex;
    typedef NullMutex RecursiveMutex;
    typedef NullLockGuard LockGuard;
    typedef NullLockGuard RecursiveLockGuard;
    //typedef ... ConditionVariable; -- does not make sense to use this in a single-threaded program
    typedef NullBarrier Barrier;
};

// Used internally.
SAWYER_EXPORT SAWYER_THREAD_TRAITS::RecursiveMutex& bigMutex();

} // namespace
#endif
