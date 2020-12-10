// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_Synchronization_H
#define Sawyer_Synchronization_H

#include <Sawyer/Sawyer.h>
#include <Sawyer/Map.h>
#include <Sawyer/Type.h>

#if SAWYER_MULTI_THREADED
    // It appears as though a certain version of GNU libc interacts badly with C++03 GCC and LLVM compilers. Some system header
    // file defines _XOPEN_UNIX as "1" and __UINTPTR_TYPE__ as "unsigned long int" but doesn't provide a definition for
    // "uintptr_t".  This triggers a compilation error in <boost/atomic/atomic.hpp> for boost-1.54 because it assumes that
    // "uintptr_t" is available based on the preprocessor macros and the included files.  These errors occur (at a minimum) on
    // Debian 8.2 and 8.3 using C++03 mode of gcc-4.8.4, gcc-4.9.2, or llvm-3.5.
    #include <boost/version.hpp>
    #if __cplusplus < 201103L && BOOST_VERSION == 105400
        #include <stdint.h>                             //  must be included before <boost/thread.hpp>
    #endif

    #include <boost/thread.hpp>
    #include <boost/thread/barrier.hpp>
    #include <boost/thread/condition_variable.hpp>
    #include <boost/thread/mutex.hpp>
    #include <boost/thread/locks.hpp>
    #include <boost/thread/once.hpp>
    #include <boost/thread/recursive_mutex.hpp>
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

// Used internally as a mutex in a single-threaded environment. Although it doesn't make sense to be able lock or unlock a
// mutex in a single-threaded environment, incrementing a data member for each unlock might be useful and it works in
// conjunction with NullLockGuard to prevent compilers from warning about unused variables which, at least in the
// multi-threaded environment, are used only for their RAII side effects.
class NullMutex {
    size_t n;
public:
    NullMutex(): n(0) {}
    void lock() {}
    void unlock() { ++n; }
    bool try_lock() { return true; }
};

// Used internally as a lock guard in a single-threaded environment.
class NullLockGuard {
    NullMutex &mutex_;
public:
    NullLockGuard(NullMutex &m)
        : mutex_(m) {
        lock();
    }
    ~NullLockGuard() {
        unlock();
    }
    void lock() {
        mutex_.lock();
    }
    void unlock() {
        mutex_.unlock();
    }
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

template<>
class LockGuard2<NullMutex> {
public:
    LockGuard2(NullMutex&, NullMutex&) {}
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
    typedef boost::unique_lock<boost::mutex> UniqueLock;
    typedef boost::lock_guard<boost::recursive_mutex> RecursiveLockGuard;
    typedef boost::condition_variable_any ConditionVariable;
    typedef boost::barrier Barrier;
#else
    enum { SUPPORTED = 0 };
    typedef NullMutex Mutex;
    typedef NullMutex RecursiveMutex;
    typedef NullLockGuard LockGuard;
    typedef NullLockGuard UniqueLock;
    typedef NullLockGuard RecursiveLockGuard;
    //typedef ... ConditionVariable; -- does not make sense to use this in a single-threaded program
    typedef NullBarrier Barrier;
#endif
    typedef Sawyer::LockGuard2<Mutex> LockGuard2;
};


template<>
struct SynchronizationTraits<SingleThreadedTag> {
    enum { SUPPORTED = 0 };
    typedef NullMutex Mutex;
    typedef NullMutex RecursiveMutex;
    typedef NullLockGuard LockGuard;
    typedef NullLockGuard UniqueLock;
    typedef NullLockGuard RecursiveLockGuard;
    //typedef ... ConditionVariable; -- does not make sense to use this in a single-threaded program
    typedef NullBarrier Barrier;
    typedef Sawyer::LockGuard2<Mutex> LockGuard2;
};

// Used internally.
SAWYER_EXPORT SAWYER_THREAD_TRAITS::RecursiveMutex& bigMutex();

/** Thread-safe random number generator.
 *
 *  Generates uniformly distributed pseudo-random size_t values. The returned value is greater than zero and less than @p n,
 *  where @p n must be greater than zero except when seeding the sequence. To seed the sequence, call with @p n equal to zero
 *  and a non-zero @p seed; all other calls must supply a zero @p seed value. This function uses the fastest available method
 *  for returning random numbers in a multi-threaded environment.  This function is thread-safe. */
SAWYER_EXPORT size_t fastRandomIndex(size_t n, size_t seed = 0);

/** Thread local data per object instance.
 *
 *  This is useful when you have a class non-static data member that needs to be thread-local.
 *
 *  @code
 *  struct MyClass {
 *      static SAWYER_THREAD_LOCAL int foo; // static thread-local using __thread, thread_local, etc.
 *      MultiInstanceTls<int> bar; // per-instance thread local
 *  };
 *
 *  MyClass a, b;
 *
 *  a.bar = 5;
 *  b.bar = 6;
 *  assert(a.bar + 1 == b.bar);
 *  @endcode
 *
 *  where @c SAWYER_THREAD_LOCAL is a macro expanding to, perhaps, "__thread". C++ only allows thread-local global variables
 *  or static member data, as with @c foo above. That means that @c a.foo and @c b.foo alias one another. But if you need
 *  some member data to be thread-local per object, you can declare it as @c MultiInstanceTls<T>. For instance, @c a.bar and @c
 *  b.bar are different storage locations, and are also thread-local. */
template<typename T>
class MultiInstanceTls {
    // The implementation needs to handle the case when this object is created on one thread and used in another thread. The
    // constructor, running in thread A, creates a thread-local repo which doesn't exist in thread B using this object.
    //
    // This is a pointer to avoid lack of thread-local dynamic initialization prior to C++11, and to avoid lack of well defined
    // order when initializing and destroying global variables in C++.
    typedef Type::UnsignedInteger<8*sizeof(void*)>::type IntPtr;
    typedef Container::Map<IntPtr, T> Repo;
    static SAWYER_THREAD_LOCAL Repo *repo_;             // no mutex necessary since this is thread-local

public:
    /** Default-constructed value. */
    MultiInstanceTls() {
        if (!repo_)
            repo_ = new Repo;
        repo_->insert(reinterpret_cast<IntPtr>(this), T());
    }

    /** Initialize value. */
    /*implicit*/ MultiInstanceTls(const T& value) {
        if (!repo_)
            repo_ = new Repo;
        repo_->insert(reinterpret_cast<IntPtr>(this), value);
    }

    /** Assignment operator. */
    MultiInstanceTls& operator=(const T &value) {
        if (!repo_)
            repo_ = new Repo;
        repo_->insert(reinterpret_cast<IntPtr>(this), value);
        return *this;
    }

    ~MultiInstanceTls() {
        if (repo_)
            repo_->erase(reinterpret_cast<IntPtr>(this));
    }

    /** Get interior object.
     *
     * @{ */
    T& get() {
        if (!repo_)
            repo_ = new Repo;
        return repo_->insertMaybeDefault(reinterpret_cast<IntPtr>(this));
    }
    const T& get() const {
        if (!repo_)
            repo_ = new Repo;
        return repo_->insertMaybeDefault(reinterpret_cast<IntPtr>(this));
    }
    /** @} */

    T& operator*() {
        return get();
    }

    const T& operator*() const {
        return get();
    }

    T* operator->() {
        return &get();
    }

    const T* operator->() const {
        return &get();
    }

    /** Implicit conversion to enclosed type.
     *
     *  This is so that the data member can be used as if it were type @c T rather than a MultiInstanceTls object. */
    operator T() const {
        if (!repo_)
            repo_ = new Repo;
        return repo_->insertMaybeDefault(reinterpret_cast<IntPtr>(this));
    }
};

template<typename T>
SAWYER_THREAD_LOCAL Container::Map<Type::UnsignedInteger<8*sizeof(void*)>::type, T>* MultiInstanceTls<T>::repo_;

} // namespace
#endif
