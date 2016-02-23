// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/Graph.h>
#include <Sawyer/GraphAlgorithm.h>
#include <Sawyer/GraphTraversal.h>
#include <Sawyer/Sawyer.h>
#include <Sawyer/Stack.h>

#include <boost/foreach.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <set>

namespace Sawyer {

/** Work list with dependencies.
 *
 *  This class takes a lattice of work items. The vertices are the items that need to be worked on, and an edge from vertex @em
 *  a to vertex @em b means work on @em a depends on @em b having been completed.
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
     *  (at least one thread, but not more than the number of items on which to work). If @p nWorkers is zero then the system's
     *  hadware concurrency is used. The dependencies must be a forest of lattices or else an <code>std::runtime_error</code>
     *  is thrown and no work is performed.
     *
     *  The lattice is copied into this class so that the class can modify it as work items are completed.  The @p functor can
     *  be a class with @c operator(), a function pointer, or a lambda expression.  If a class is used, it must be copyable and
     *  each worker thread will be given its own copy.  The functor is invoked with two arguments: the ID number of the work
     *  item being processed, and a reference to a copy of the work item in the dependency graph.  The ID number is the vertex
     *  ID number in the @p dependencies graph.
     *
     *  The constructor does not return until all work has been completed. This object can only perform work a single time. */
    ThreadWorkers(const DependencyGraph &dependencies, size_t nWorkers, Functor functor)
        : hasStarted_(false), hasWaited_(false), nWorkers_(0), workers_(NULL), nItemsStarted_(0), nItemsFinished_(0),
          nWorkersRunning_(0), nWorkersFinished_(0) {
        run(dependencies, nWorkers, functor);
    }

    /** Destructor.
     *
     *  The destructor waits for work to complete before returning. */
    ~ThreadWorkers() {
        wait();
        delete[] workers_;
    }

    /** Start workers and return.
     *
     *  This method verifies that the @p dependencies is a forest of lattices, the vertices of which are the items on which to
     *  work, and the edges of which represent dependencies.  An edge from vertex @em a to vertex @em b means that work item
     *  @em a cannot start until work on item @em b has finished.  If @p dependencies has cycles then an
     *  <code>std::runtime_error</code> is thrown before any work begins.
     *
     *  The work items in @p dependencies are processed by up to @p nWorkers threads created by this method and destroyed when
     *  work is complete. This method creates at least one thread (if there's any work), but never more threads than the total
     *  amount of work. If @p nWorkers is zero then the system's hardware concurrency is used. It returns as soon as those
     *  workers are created.
     *
     *  Each object can perform work only a single time. */
    void start(const DependencyGraph &dependencies, size_t nWorkers, Functor functor) {
        if (Container::Algorithm::graphContainsCycle(dependencies))
            throw std::runtime_error("work dependency graph has cycles");

        boost::lock_guard<boost::mutex> lock(mutex_);
        if (hasStarted_)
            throw std::runtime_error("work can start only once per object");
        hasStarted_ = true;
        dependencies_ = dependencies;
        if (0 == nWorkers)
            nWorkers = boost::thread::hardware_concurrency();
        nWorkers_ = std::max((size_t)1, std::min(nWorkers, dependencies.nVertices()));
        nItemsStarted_ = nWorkersFinished_ = 0;
        fillWorkQueueNS();
        startWorkersNS(functor);
    }

    /** Wait for work to complete.
     *
     *  This call blocks until all work is completed. If no work has started yet then it returns immediately. */
    void wait() {
        boost::unique_lock<boost::mutex> lock(mutex_);
        if (!hasStarted_ || hasWaited_)
            return;
        hasWaited_ = true;
        lock.unlock();

        for (size_t i=0; i<nWorkers_; ++i)
            workers_[i].join();

        lock.lock();
        dependencies_.clear();
    }

    /** Synchronously processes work items.
     *
     *  This is simply a wrapper around @ref start and @ref wait.  It performs work synchronously, returning only after the
     *  work has completed. */
    void run(const DependencyGraph &dependencies, size_t nWorkers, Functor functor) {
        start(dependencies, nWorkers, functor);
        wait();
    }

    /** Test whether all work is finished.
     *
     *  Returns false if work is ongoing, true if work is finished or was never started. */
    bool isFinished() {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return !hasStarted_ || nWorkersFinished_ == nWorkers_;
    }

    /** Number of work items remaining to run.
     *
     *  This is the number of work items that have been started or completed. */
    size_t nStarted() {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return nItemsStarted_;
    }

    /** Number of work items that have completed. */
    size_t nFinished() {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return nItemsFinished_;
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
            while (nItemsStarted_ < dependencies_.nVertices() && workQueue_.isEmpty())
                workInserted_.wait(lock);
            if (nItemsStarted_ >= dependencies_.nVertices()) {
                ++nWorkersFinished_;
                return;
            }
            size_t workItemId = workQueue_.pop();
            typename DependencyGraph::VertexValue workItem = dependencies_.findVertex(workItemId)->value();
            ++nItemsStarted_;

            // Do the work
            ++nWorkersRunning_;
            lock.unlock();
            functor(workItemId, workItem);
            lock.lock();
            ++nItemsFinished_;
            --nWorkersRunning_;

            // Adjust remove dependency edges from the dependency lattice
            typename DependencyGraph::VertexIterator vertex = dependencies_.findVertex(workItemId);
            std::set<typename DependencyGraph::ConstVertexIterator> candidateWorkItems;
            BOOST_FOREACH (const typename DependencyGraph::Edge &edge, vertex->inEdges())
                candidateWorkItems.insert(edge.source());
            dependencies_.clearInEdges(vertex);

            // For vertices that newly have no dependencies, add them to the work queue
            size_t newWorkInserted = 0;
            BOOST_FOREACH (const typename DependencyGraph::ConstVertexIterator &candidate, candidateWorkItems) {
                if (candidate->nOutEdges() == 0) {
                    workQueue_.push(candidate->id());
                    ++newWorkInserted;
                }
            }
            
            // Notify other workers
            if (0 == newWorkInserted && workQueue_.isEmpty()) {
                workInserted_.notify_all();
            } else if (1 == newWorkInserted) {
                workInserted_.notify_one();
            } else if (newWorkInserted > 1) {
                workInserted_.notify_all();
            }
        }
    }
};

/** Performs work in parallel.
 *
 *  Creates up to the specified number of worker threads to run the work described in the @p dependencies
 *  (at least one thread, but not more than the number of items on which to work). The dependencies must be a forest of
 *  lattices or else an <code>std::runtime_error</code> is thrown and no work is performed.
 *
 *  The @p functor can be a class with @c operator(), a function pointer, or a lambda expression.  If a class is used, it must
 *  be copyable and each worker thread will be given its own copy.  The functor is invoked with two arguments: the ID number of
 *  the work item being processed, and a reference to a copy of the work item in the dependency graph.  The ID number is the
 *  vertex ID number in the @p dependencies graph.
 *
 *  The call does not return until all work has been completed. */
template<class DependencyGraph, class Functor>
void
workInParallel(const DependencyGraph &dependencies, size_t nWorkers, Functor functor) {
    ThreadWorkers<DependencyGraph, Functor>(dependencies, nWorkers, functor);
}


} // namespace
