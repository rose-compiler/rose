#ifndef ROSE_BinaryAnalysis_ModelChecker_PathPrioritizer_H
#define ROSE_BinaryAnalysis_ModelChecker_PathPrioritizer_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER

#include <Rose/BinaryAnalysis/ModelChecker/Types.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** Base class for prioritizing work.
 *
 *  This class sorts execution paths by increasing priority and is used in various priority queues. For instance, it is
 *  used to prioritize the execution tree search order, and to prioritize the order in which interesting results are
 *  returned. */
class PathPrioritizer {
public:
    using Ptr = PathPrioritizerPtr;

    PathPrioritizer();
    virtual ~PathPrioritizer();

    /** Priority predicate.
     *
     *  Given two paths, return true if path @p better should be explored before path @p worse, and false otherwise. This
     *  must be a strict weak ordering.
     *
     *  Note that the ordering predicate for C++ heaps is opposite the predicate for sorting containers with operator<. In
     *  other words, the item with the highest priority (not the lowest) is the one that will be taken next from the heap.
     *
     *  Thread safety: This method is thread safe. The base implementation is thread safe and all subclass implementations
     *  must also be thread safe. */
    virtual bool operator()(const PathPtr &worse, const PathPtr &better) const;
};

/** Prioritize shorter paths.
 *
 *  Paths that are shorter (fewer steps) get higher priority. When used with the work queue, this results in a breadth-first
 *  search of the virtual execution tree. This is also useful for returning the "interesting" results since one often wants
 *  to give priorities to those results that are simpler (i.e., shorter). */
class ShortestPathFirst: public PathPrioritizer {
public:
    using Ptr = ShortestPathFirstPtr;

    static Ptr instance();

    bool operator()(const PathPtr &worse, const PathPtr &better) const override;
};

/** Prioritize longer paths.
 *
 *  Paths that are longer (more steps) get higher priority. When used with the work queue, this results in a depth-first
 *  search of the virtual execution tree. */
class LongestPathFirst: public PathPrioritizer {
public:
    using Ptr = LongestPathFirstPtr;

    static Ptr instance();

    bool operator()(const PathPtr &worse, const PathPtr &better) const override;
};

/** Prioritize paths by how long they take.
 *
 *  Gives priority to paths that are faster to evaluate. */
class FastestPathFirst: public PathPrioritizer {
public:
    using Ptr = FastestPathFirstPtr;

    static Ptr instance();

    bool operator()(const PathPtr &worse, const PathPtr &better) const override;
};

/** Prioritize paths randomly.
 *
 *  This uses the randomly-generated node ID to sort paths. */
class RandomPathFirst: public PathPrioritizer {
public:
    using Ptr = RandomPathFirstPtr;

    static Ptr instance();

    bool operator()(const PathPtr &worse, const PathPtr &better) const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
