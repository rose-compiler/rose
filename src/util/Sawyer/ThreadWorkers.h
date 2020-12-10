// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/Exception.h>
#include <Sawyer/Graph.h>
#include <Sawyer/Map.h>
#include <Sawyer/Sawyer.h>
#include <Sawyer/Stack.h>

#include <boost/foreach.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/version.hpp>
#include <set>

namespace Sawyer {

/** Work list with dependencies.
 *
 *  This class takes a graph of tasks. The vertices are the tasks that need to be worked on, and an edge from vertex @em
 *  a to vertex @em b means work on @em a depends on @em b having been completed.  Vertices that participate in a cycle cannot
 *  be worked on since there is no way to resolve their dependencies; in this case, as much work as possible is performed.
 *
 *  See also, the @ref workInParallel function which is less typing since template parameters are inferred. */
template<class DependencyGraph, class Functor>
class ThreadWorkers {
    boost::mutex mutex_;                                // protects the following members after the constructor
    DependencyGraph dependencies_;                      // outstanding dependencies
    bool hasStarted_;                                   // set when work has started
    bool hasWaited_;                                    // set when wait() is called
    Container::Stack<size_t> workQueue_;                // outstanding work identified by vertex ID of dependency graph
    boost::condition_variable workInserted_;            // signaled when work is added to the queue or all work is consumed
    size_t nWorkers_;                                   // number of worker threads allocated
    boost::thread *workers_;                            // worker threads
    size_t nItemsStarted_;                              // number of work items started
    size_t nItemsFinished_;                             // number of work items that have been completed already
    size_t nWorkersRunning_;                            // number of workers that are currently busy doing something
    size_t nWorkersFinished_;                           // number of worker threads that have returned
    std::set<size_t> runningTasks_;                     // tasks (vertex IDs) that are running

public:
    /** Default constructor.
     *
     *  This constructor initializes the object but does not start any worker threads.  Each object can perform work a single
     *  time, which is done by calling @ref run (synchronous) or @ref start and @ref wait (asynchronous). */
    ThreadWorkers()
        : hasStarted_(false), hasWaited_(false), nWorkers_(0), workers_(NULL), nItemsStarted_(0), nItemsFinished_(0),
          nWorkersRunning_(0), nWorkersFinished_(0) {}

    /** Constructor that synchronously runs the work.
     *
     *  This constructor creates up to the specified number of worker threads to run the work described in the @p dependencies
     *  (at least one thread, but not more than the number of vertices in the graph). If @p nWorkers is zero then the system's
     *  hadware concurrency is used. If the dependency graph contains a cycle then as much work as possible is performed
     *  and then a @ref Exception::ContainsCycle "ContainsCycle" exception is thrown.
     *
     *  The dependency graph is copied into this class so that the class can modify it as tasks are completed.  The @p functor
     *  can be a class with @c operator(), a function pointer, or a lambda expression.  If a class is used, it must be copyable
     *  and each worker thread will be given its own copy.  The functor is invoked with two arguments: the ID number of the
     *  task being processed, and a reference to a copy of the task (vertex value) in the dependency graph.  The ID number is
     *  the vertex ID number in the @p dependencies graph.
     *
     *  The constructor does not return until all possible non-cyclic work has been completed. This object can only perform
     *  work a single time. */
    ThreadWorkers(const DependencyGraph &dependencies, size_t nWorkers, Functor functor)
        : hasStarted_(false), hasWaited_(false), nWorkers_(0), workers_(NULL), nItemsStarted_(0), nItemsFinished_(0),
          nWorkersRunning_(0), nWorkersFinished_(0) {
        try {
            run(dependencies, nWorkers, functor);
        } catch (const Exception::ContainsCycle&) {
            delete[] workers_;
            throw;                                      // destructor won't be called
        }
    }

    /** Destructor.
     *
     *  The destructor waits for all possible non-cyclic work to complete before returning. */
    ~ThreadWorkers() {
        wait();
        delete[] workers_;
    }

    /** Start workers and return.
     *
     *  This method saves a copy of the dependencies, initializes a work list, and starts worker threads.  The vertices of the
     *  dependency graph are the tasks to be performed by the workers and the edges represent dependencies between the
     *  tasks. An edge from vertex @em a to vertex @em b means that task @em a cannot start until work on task @em b has
     *  finished.
     *
     *  The tasks in @p dependencies are processed by up to @p nWorkers threads created by this method and destroyed when
     *  work is complete. This method creates at least one thread (if there's any work), but never more threads than the total
     *  amount of work. If @p nWorkers is zero then the system's hardware concurrency is used. It returns as soon as those
     *  workers are created.
     *
     *  Each object can perform work only a single time. */
    void start(const DependencyGraph &dependencies, size_t nWorkers, Functor functor) {
        boost::lock_guard<boost::mutex> lock(mutex_);
        if (hasStarted_)
            throw std::runtime_error("work can start only once per object");
        hasStarted_ = true;
        dependencies_ = dependencies;
        if (0 == nWorkers)
            nWorkers = boost::thread::hardware_concurrency();
        nWorkers_ = std::max((size_t)1, std::min(nWorkers, dependencies.nVertices()));
        nItemsStarted_ = nWorkersFinished_ = 0;
        runningTasks_.clear();
        fillWorkQueueNS();
        startWorkersNS(functor);
    }

    /** Wait for work to complete.
     *
     *  This call blocks until all possible non-cyclic work is completed. If no work has started yet then it returns
     *  immediately. */
    void wait() {
        boost::unique_lock<boost::mutex> lock(mutex_);
        if (!hasStarted_ || hasWaited_)
            return;
        hasWaited_ = true;
        lock.unlock();

        for (size_t i=0; i<nWorkers_; ++i)
            workers_[i].join();

        lock.lock();
        if (dependencies_.nEdges() != 0)
            throw Exception::ContainsCycle("task dependency graph contains cycle(s)");
        dependencies_.clear();
    }

    /** Synchronously processes tasks.
     *
     *  This is simply a wrapper around @ref start and @ref wait.  It performs work synchronously, returning only after all
     *  possible work has completed. If the dependency graph contained cycles then a @ref Exception::ContainsCycle
     *  "ContainsCycle" exception is thrown after all possible non-cyclic work is finished. */
    void run(const DependencyGraph &dependencies, size_t nWorkers, Functor functor) {
        start(dependencies, nWorkers, functor);
        wait();
    }

    /** Test whether all possible work is finished.
     *
     *  Returns false if work is ongoing, true if all possible non-cyclic work is finished or no work was ever started. */
    bool isFinished() {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return !hasStarted_ || nWorkersFinished_ == nWorkers_;
    }

    /** Number of tasks that have started.
     *
     *  This is the number of tasks that have been started, some of which may have completed already. Although this function is
     *  thread-safe, the returned data might be out of date by time the caller accesses it. */
    size_t nStarted() {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return nItemsStarted_;
    }

    /** Number of tasks that have completed.
     *
     *  Although this function is thread-safe, the returned data might be out of date by time the caller accesses it. */
    size_t nFinished() {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return nItemsFinished_;
    }

    /** Tasks currently running.
     *
     *  Returns the set of tasks (dependency graph vertex IDs) that are currently running. Although this function is
     *  thread-safe, the returned data might be out of date by time the caller accesses it. */
    std::set<size_t> runningTasks() {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return runningTasks_;
    }
    
    /** Number of worker threads.
     *
     *  Returns a pair of numbers. The first is the total number of worker threads that are allocated, and the second is the
     *  number of threads that are busy working.  The second number will never be larger than the first. */
    std::pair<size_t, size_t> nWorkers() {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return std::make_pair(nWorkers_-nWorkersFinished_, nWorkersRunning_);
    }
    
private:
    // Scan the dependency graph and fill the work queue with vertices that have no dependencies.
    void fillWorkQueueNS() {
        ASSERT_require(workQueue_.isEmpty());
        BOOST_FOREACH (const typename DependencyGraph::Vertex &vertex, dependencies_.vertices()) {
            if (vertex.nOutEdges() == 0)
                workQueue_.push(vertex.id());
        }
    }

    // Start worker threads
    void startWorkersNS(Functor functor) {
        workers_ = new boost::thread[nWorkers_];
        for (size_t i=0; i<nWorkers_; ++i)
            workers_[i] = boost::thread(startWorker, this, functor);
    }

    // Worker threads execute here
    static void startWorker(ThreadWorkers *self, Functor functor) {
        self->worker(functor);
    }

    void worker(Functor functor) {
        while (1) {
            // Get the next item of work
            boost::unique_lock<boost::mutex> lock(mutex_);
            while (nItemsFinished_ < nItemsStarted_ && workQueue_.isEmpty())
                workInserted_.wait(lock);
            if (nItemsFinished_ == nItemsStarted_ && workQueue_.isEmpty()) {
                ++nWorkersFinished_;
                return;
            }
            ASSERT_forbid(workQueue_.isEmpty());
            size_t workItemId = workQueue_.pop();
            typename DependencyGraph::ConstVertexIterator workVertex = dependencies_.findVertex(workItemId);
            ASSERT_require(workVertex->nOutEdges() == 0);
            typename DependencyGraph::VertexValue workItem = workVertex->value();
            ++nItemsStarted_;

            // Do the work
            ++nWorkersRunning_;
            runningTasks_.insert(workItemId);
            lock.unlock();
            functor(workItemId, workItem);
            lock.lock();
            ++nItemsFinished_;
            --nWorkersRunning_;
            runningTasks_.erase(workItemId);

            // Look for more work as we remove some dependency edges. Watch out for parallel edges (self edges not possible).
            Container::Map<size_t, typename DependencyGraph::ConstVertexIterator> candidateWorkItems;
            BOOST_FOREACH (const typename DependencyGraph::Edge &edge, workVertex->inEdges())
                candidateWorkItems.insert(edge.source()->id(), edge.source());
            dependencies_.clearInEdges(workVertex);
            size_t newWorkInserted = 0;
            BOOST_FOREACH (const typename DependencyGraph::ConstVertexIterator &candidate, candidateWorkItems.values()) {
                if (candidate->nOutEdges() == 0) {
                    workQueue_.push(candidate->id());
                    ++newWorkInserted;
                }
            }

            // Notify other workers
            if (0 == newWorkInserted) {
                if (workQueue_.isEmpty())
                    workInserted_.notify_all();
            } else if (1 == newWorkInserted) {
                // we'll do the new work ourself
            } else if (2 == newWorkInserted) {
                workInserted_.notify_one();
            } else {
                workInserted_.notify_all();
            }
        }
    }
};

/** Performs work in parallel.
 *
 *  Creates up to the specified number of worker threads to run the tasks described in the @p dependencies graph (at least one
 *  thread, but not more than the number of items on which to work).  The dependencies is a graph whose vertices represent
 *  individual tasks to be performed by worker threads, and whose edges represent dependencies between the task. An edge from
 *  task @em a to task @em b means that task @em b must complete before task @em a can begin.  If the dependencies graph
 *  contains cycles then as much work as possible is performed and then a @ref Exception::ContainsCycle "ContainsCycle"
 *  exception is thrown.
 *
 *  The @p functor can be a class with @c operator(), a function pointer, or a lambda expression.  If a class is used, it must
 *  be copyable and each worker thread will be given its own copy.  The functor is invoked with two arguments: the ID number of
 *  the task being processed, and a reference to a copy of the task (vertex value) in the dependency graph.  The ID number is
 *  the vertex ID number in the @p dependencies graph.
 *
 *  If a @p monitor is provided, it will be called once every @p period milliseconds the the following arguments: the @p
 *  dependencies graph, @p nWorkers, and the set of @p dependencies vertex IDs (<code>std::set<size_t></code>) that are
 *  currently running.
 *
 *  The call does not return until all work has been completed.
 *
 *  @{ */
template<class DependencyGraph, class Functor>
void
workInParallel(const DependencyGraph &dependencies, size_t nWorkers, Functor functor) {
    ThreadWorkers<DependencyGraph, Functor>(dependencies, nWorkers, functor);
}


template<class DependencyGraph, class Functor, class Monitor>
void
workInParallel(const DependencyGraph &dependencies, size_t nWorkers, Functor functor,
               Monitor monitor, boost::chrono::milliseconds period) {
    ThreadWorkers<DependencyGraph, Functor> workers;
    workers.start(dependencies, nWorkers, functor);
    while (!workers.isFinished()) {
        monitor(dependencies, nWorkers, workers.runningTasks());
#if BOOST_VERSION >= 105000
        boost::this_thread::sleep_for(period);
#else
        // For ROSE's sake, don't make this a compile-time error just yet. [Robb Matzke 2018-04-24]
        ASSERT_not_reachable("this old version of boost is not supported");
#endif
    }
    monitor(dependencies, nWorkers, std::set<size_t>());
    workers.wait();
}
/** @} */


} // namespace
