// Parallel sorting using multiple threads. See ParallelSort::quicksort() near the end of this file.
#ifndef ROSE_ParallelSort_H
#define ROSE_ParallelSort_H
#include <RoseFirst.h>

#include <boost/thread.hpp>
#include <list>
#include <vector>

namespace Rose {

/** Algorithms for parallel sorting.
 *
 *  These algorithms use multiple threads to sort large arrays of values in parallel.  Their usage is similar to std::sort
 *  in terms of the arguments, but they also take an additional argument to specify the number of threads.  The following
 *  algorithms are implemented:
 *
 *  <ul>
 *   <li>quicksort()</li>
 *  </ul>
 */
namespace ParallelSort {

// This stuff is all private but useful for any parallel sort algorithm.
namespace Private {

// A unit of work.  The values to be worked on are specified by a begin (inclusive) and end (exclusive) iterator.
template<class RandomAccessIterator>
struct Work {
    RandomAccessIterator begin, end;
    Work(RandomAccessIterator begin, RandomAccessIterator end): begin(begin), end(end) {}
};

// Information about a sorting job.  A job is generated when the user requests a sort, and each job may have multiple threads.
template<class RandomAccessIterator, class Compare>
struct Job {
    Compare compare;                                    // functor to compare two values, like for std::sort()
    boost::condition_variable worklistInsertion;        // signaled when something is added to the worklist
    static const int multiThreshold = 10000;            // size at which to start multi-threading
    boost::mutex mutex;                                 // protects all of the following data members
    std::list<Work<RandomAccessIterator> > worklist;    // non-overlapping stuff that needs to be sorted
    size_t npending;                                    // number of workers that are processing jobs that aren't in worklist
    Job(Compare compare): compare(compare), npending(0) {}
};

// Somewhat like std::partition(). Partitions the iterator range into two parts according to the value at the pivot iterator
// and returns an iterator for the beginning of the second part.  The values of the first part will all be less than the pivot
// value, and the values of the second part will all be greater than or equal to the pivot value.  The values are in no
// particular order.
template<class RandomAccessIterator, class Compare>
RandomAccessIterator partition(RandomAccessIterator begin, RandomAccessIterator end,
                               RandomAccessIterator pivot, Compare compare) {
    assert(begin < end);
    assert(pivot >= begin && pivot < end);
    std::swap(*pivot, *(end-1));                        // temporarily move the pivot out of the way
    for (RandomAccessIterator i=pivot=begin; i+1<end; ++i) {
        if (compare(*i, *(end-1)))
            std::swap(*i, *pivot++);
    }
    std::swap(*(end-1), *pivot);
    return pivot;
}

// Add work to the work list
template<class RandomAccessIterator, class Compare>
void addWork(Job<RandomAccessIterator, Compare> &job, const Work<RandomAccessIterator> &work) {
    boost::lock_guard<boost::mutex> lock(job.mutex);
    job.worklist.push_back(work);
    job.worklistInsertion.notify_one();
}

// Sorts one unit of work, adding additional items to the worklist if necessary.
template<class RandomAccessIterator, class Compare>
void quicksort(Job<RandomAccessIterator, Compare> &job, Work<RandomAccessIterator> work) {
    while (work.end - work.begin > 1) {
        if (work.end - work.begin < job.multiThreshold) {
            std::sort(work.begin, work.end, job.compare);
            return;
        } else {
            RandomAccessIterator pivot = work.begin + (work.end - work.begin) / 2; // assuming fairly even distribution
            pivot = partition(work.begin, work.end, pivot, job.compare);
            addWork(job, Work<RandomAccessIterator>(pivot+1, work.end));
            work.end = pivot;
        }
    }
}

// A worker thread.
template<class RandomAccessIterator, class Compare>
struct Worker {
    Job<RandomAccessIterator, Compare> &job;
    size_t id;                                          // only for debugging
    Worker(Job<RandomAccessIterator, Compare> &job, size_t id): job(job), id(id) {}
    void operator()() {
        boost::unique_lock<boost::mutex> lock(job.mutex);
        while (true) {
            // Get the next unit of work. If no workers are working and the worklist is empty then we're all done.
            while (job.worklist.empty() && job.npending>0)
                job.worklistInsertion.wait(lock);
            if (job.worklist.empty())
                return;
            Work<RandomAccessIterator> work = job.worklist.front();
            job.worklist.pop_front();
            ++job.npending;

            // Sort that unit of work
            lock.unlock();
            quicksort(job, work);
            lock.lock();

            // Indicate that the work is completed
            assert(job.npending>0);
            if (0==--job.npending && job.worklist.empty())
                job.worklistInsertion.notify_all();
        }
    }
};

} // namespace


/** Sort values in parallel.  Sorts the values between @p begin (inclusive) and @p end (exclusive) according to the comparator
 *  @p compare using @p nthreads threads.  Multi-threading is only used if the size of the range of values exceeds a certain
 *  threshold.
 *
 *  Note: using normal C++ iterators with debugging support will result in slower execution the more threads are used because
 *  the iterator dereference operators serialize some sanity checks which causes lock contention.  It is best to do the sanity
 *  check once up front, then then call the sort function with pointers.  For example:
 *
 * @code
 *  std::vector<Thing> data;
 *
 *  // This method might get slower as nthreads increases
 *  quicksort(data.begin(), data.end(), thingComparer, nthreads);
 *
 *  // But this should scale well provided data.size() is large enough.
 *  quicksort(&data[0], &data[0]+data.size(), thingComparer, nthreads);
 * @endcode
 */
template<class RandomAccessIterator, class Compare>
void quicksort(RandomAccessIterator begin, RandomAccessIterator end, size_t nThreads, Compare compare) {
    assert(begin < end);
    using namespace Private;

    Job<RandomAccessIterator, Compare> job(compare);
    addWork(job, Work<RandomAccessIterator>(begin, end));

    // Start worker threads (we can't assume containers with move semantics, so use an array)
    size_t nworkers = std::max(nThreads, (size_t)1) - 1;
    boost::thread *workers = new boost::thread[nworkers];
    for (size_t i=0; i<nworkers; ++i)
        workers[i] = boost::thread(Worker<RandomAccessIterator, Compare>(job, i+1));
    
    // Participate in the work ourselves (we might be the only thread!)
    Worker<RandomAccessIterator, Compare>(job, 0)();

    // Wait for all the threads to finish
    for (size_t i=0; i<nworkers; ++i)
        workers[i].join();
}
    
} // namespace
} // namespace

#endif
