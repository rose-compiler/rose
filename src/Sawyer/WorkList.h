// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_WorkList_H
#define Sawyer_WorkList_H

#include <Sawyer/Synchronization.h>
#include <deque>

namespace Sawyer {

/** First-in-first-out work queue.
 *
 *  This simple work queue is meant to demonstrate the interface used by @ref processWorkList.  New work is inserted at the
 *  back of the FIFO and consumed from the front. Only one type and three public member functions are necessary. The member
 *  functions must be thread safe if any of them are called from worker threads, otherwise they don't need to be thread safe
 *  since the @ref processWorkList will syncronize calls to this object. */
template<class T>
class WorkFifo {
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;         // protects the following data members
    std::deque<T> items_;                               // ordered list of work to be completed

public:
    /** The type that represents one unit of work. */
    typedef T Item;

    /** Insert one unit of work into the queue. */
    void insert(const Item &item) {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        items_.push_back(item);
    }

    /** Test whethere the queue is empty. */
    bool isEmpty() const {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        return items_.empty();
    }

    /** Remove and return the next item of work.
     *
     *  This is only called when @ref isEmpty returns true.
     *
     *  This function is allowed to block to wait for other work to complete, which can be useful when the work list has more work
     *  to do but all that work depends on the completion of work that's already underway.  Determining when the other work
     *  completes can be done by having the @ref processWorkList functor signal a condition variable on which this function is
     *  waiting. */
    Item next() {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        ASSERT_forbid(items_.empty());
        Item item = items_.front();
        items_.pop_front();
        return item;
    }
};

/** Process work from a work list in parallel.
 *
 *  Processes each item in the work list by passing the item and the list of pending work to the provided functor. Up to @p
 *  maxWorkers work items are processed in parallel. The @p workList is responsible for holding the list of outstanding work
 *  items and hands them out one at a time according some priority. The @ref WorkFifo is an example class that satisfies the @p
 *  WorkItems interface and hands out work items in the same order they were created.  Since the functor is given a reference
 *  to the workList, wihch contains all pending work (work that hasn't started yet), the functor is able to modify the work
 *  list, such as adding additional work items. */
template<class WorkItems, class Functor>
void processWorkList(WorkItems &workList, size_t maxWorkers, Functor f) {
#if SAWYER_MULTI_THREADED
    boost::mutex mutex;
    boost::condition_variable cond;
    size_t nActiveWorkers = 0;

    if (0 == maxWorkers)
        maxWorkers = boost::thread::hardware_concurrency();
    ASSERT_require(maxWorkers > 0);

    while (1) {
        // Wait for work items or workers to be available.
        boost::unique_lock<boost::mutex> lock(mutex);
        while (!workList.isEmpty() && nActiveWorkers >= maxWorkers)
            cond.wait(lock);

        // Start as much work as possible
        while (!workList.isEmpty() && nActiveWorkers < maxWorkers) {
            // Assuming lambda's are not available, we work around.
            struct Worker {
                static void doWork(Functor f, const typename WorkItems::Item &item, WorkItems *workList,
                                   boost::mutex *mutex, boost::condition_variable *cond, size_t *nActiveWorkers) {
                    f(item, *workList);
                    boost::lock_guard<boost::mutex> lock(*mutex);
                    --*nActiveWorkers;
                    cond->notify_one();
                }
            };

            boost::thread thrd(Worker::doWork, f, workList.next(), &workList, &mutex, &cond, &nActiveWorkers);
            thrd.detach();
            ++nActiveWorkers;
        }

        // Return if there's nothing to do
        if (workList.isEmpty() && 0 == nActiveWorkers)
            break;
    }
#else
    while (!workList.isEmpty())
        f(workList.next(), workList);
#endif
}

} // namespace
#endif
