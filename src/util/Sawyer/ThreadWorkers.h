// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/Graph.h>
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
 *  This class takes a lattice of work item vertices. An edge from one vertex to another indicates that the source vertex
 *  is a work item that depends on the target vertex, and that work cannot begin on the source vertex until work is completed
 *  on the target vertex. */
template<class DependencyGraph, class Functor>
class ThreadWorkers {
    boost::mutex mutex_;                                // protects the following members after the constructor
    DependencyGraph dependencies_;                      // outstanding dependencies
    bool hasStarted_;                                   // set when work has started
    Container::Stack<size_t> workQueue_;                // outstanding work identified by vertex ID of dependency graph
    boost::condition_variable workInserted_;            // signaled when work is added to the queue or all work is consumed
    size_t nWorkers_;                                   // number of worker threads allocated
    boost::thread *workers_;                            // worker threads
    size_t nItemsStarted_;                              // number of work items started

public:
    ThreadWorkers(const DependencyGraph &dependencies, size_t nWorkers, Functor functor)
        : hasStarted_(false), nWorkers_(std::max((size_t)1, nWorkers)), workers_(NULL), nItemsStarted_(0) {
        if (hasCycles(dependencies))
            throw std::runtime_error("worker dependency graph has cycle(s)");
        dependencies_ = dependencies;
        run(functor);
    }

    // Synchronously run the work, returning when its finished. The arguments are the same as for "start" and "wait".
    void run(Functor functor) {
        start(functor);
        wait();
    }

    // Start working. Returns as soon as workers have started.  The functor is invoked for each item of work that needs to be
    // completed; it takes one argument: a copy of the vertex in the dependency graph.
    void start(Functor functor) {
        boost::lock_guard<boost::mutex> lock(mutex_);
        ASSERT_forbid(hasStarted_);
        hasStarted_ = true;
        fillWorkQueueNS();
        if (!workQueue_.isEmpty())
            startWorkersNS(functor);
    }

    // Wait for work to complete. Blocks and returns when all workers are finished.
    void wait() {
        {
            boost::lock_guard<boost::mutex> lock(mutex_);
            ASSERT_require(hasStarted_);
        }
        for (size_t i=0; i<nWorkers_; ++i)
            workers_[i].join();
        ASSERT_require(workQueue_.isEmpty());
        ASSERT_require(nItemsStarted_ == dependencies_.nVertices());
        delete[] workers_;
        workers_ = NULL;
        dependencies_.clear();
    }

private:
    // Check for cycles in the dependency graph
    // FIXME[Robb Matzke 2015-11-11]: This should be a general-purpose graph algorithm
    bool hasCycles(const DependencyGraph &forest) {
        typedef Container::Algorithm::DepthFirstForwardGraphTraversal<const DependencyGraph> Traversal;
        std::vector<bool> processed(forest.nVertices(), false);
        std::vector<size_t> vertexOnPath(forest.nVertices(), false);
        for (size_t i=0; i<processed.size(); ++i) {
            if (!processed[i]) {
                vertexOnPath[i] = true;
                for (Traversal t(forest, forest.findVertex(i), Container::Algorithm::EDGE_EVENTS); t; ++t) {
                    size_t targetVertexId = t.edge()->target()->id();
                    if (t.event() == Container::Algorithm::ENTER_EDGE) {
                        if (vertexOnPath[targetVertexId])
                            return true; // this is a back edge, thus a cycle
                        ++vertexOnPath[targetVertexId];
                        processed[targetVertexId] = true;
                    } else {
                        --vertexOnPath[targetVertexId];
                    }
                }
            }
        }
        return false;
    }
    
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
            if (nItemsStarted_ >= dependencies_.nVertices())
                return;
            size_t workItemId = workQueue_.pop();
            typename DependencyGraph::VertexValue workItem = dependencies_.findVertex(workItemId)->value();
            ++nItemsStarted_;

            // Do the work
            lock.unlock();
            functor(workItemId, workItem);
            lock.lock();

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

template<class DependencyGraph, class Functor>
void
workInParallel(const DependencyGraph &dependencies, size_t nWorkers, Functor functor) {
    ThreadWorkers<DependencyGraph, Functor>(dependencies, nWorkers, functor);
}


} // namespace
