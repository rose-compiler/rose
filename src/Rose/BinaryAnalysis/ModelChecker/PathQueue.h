#ifndef ROSE_BinaryAnalysis_ModelChecker_PathQueue_H
#define ROSE_BinaryAnalysis_ModelChecker_PathQueue_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER

#include <Rose/BinaryAnalysis/ModelChecker/BasicTypes.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** List of path endpoints in an execution tree.
 *
 *  A queue of execution tree vertices ordered by some user-defined metric. The metric is defined by the @ref
 *  PathPrioritizer supplied as a constructor argument. */
class PathQueue final {
public:
    /** Visitor for traversing a queue. */
    class Visitor {
    public:
        virtual ~Visitor() {}

        /** Callback.
         *
         *  This callback is invoked for each path of the queue in no particular order. If any call to the callback
         *  returns false, then the traversal is terminated. The queue is locked for the duration of the traversal. */
        virtual bool operator()(const PathPtr&) = 0;
    };

private:
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;     // protects all following data members
    PathPrioritizerPtr prioritizer_;
    std::vector<PathPtr> paths_;

public:
    PathQueue() = delete;

    /** Empty queue constructor.
     *
     *  The prioritizer must not be null. */
    PathQueue(const PathPrioritizerPtr&);
    ~PathQueue();

public:
    /** Property: Prioritization predicate.
     *
     *  The prioritization predicate controls the order that paths are sorted in this priority queue. It's normally
     *  specified when the queue is created, but can in fact be changed afterward. Changing it blocks access to this queue
     *  by other threads while the members of this queue are resorted.
     *
     *  The prioritizer cannot be a null pointer.
     *
     *  Thread safety: This method is thread safe.
     *
     *  @{ */
    PathPrioritizerPtr prioritizer() const;
    void prioritizer(const PathPrioritizerPtr&);
    /** @} */

    /** Property: Size of queue.
     *
     *  This returns the number of paths in this queue.
     *
     *  Thread safety: This method is thread safe. */
    size_t size() const;

    /** Test emptiness.
     *
     *  Returns true if and only if this queue contains no paths.
     *
     *  Thread safety: This method is thread safe. */
    bool isEmpty() const;

    /** Reset to empty state.
     *
     *  Removes all items but does not change the prioritizer.
     *
     *  Thread safety: This method is thread safe. */
    void reset();

    /** Insert a path.
     *
     *  Insert a path into this queue. The path must not already be a member of the queue.
     *
     *  Thread safety: This method is thread safe. */
    void insert(const PathPtr&);

    /** Take the next path from this queue.
     *
     *  Returns a pointer to the next path, the one with the highest priority according to the path prioritizer specified
     *  in the constructor. If this queue is empty, then a null pointer is returned. The returned path is removed from this
     *  queue.
     *
     *  Thread safety: This method is thread safe. */
    PathPtr takeNext();

    /** Visit each path in the queue.
     *
     *  The visitor functor is called for each path in the queue in no particular order until either all paths are visited or
     *  one of the visitor calls returns false.
     *
     *  Thread safety: This method is thread safe. The queue is locked for the duration of the traversal. */
    void traverse(Visitor&) const;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
