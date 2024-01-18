#ifndef ROSE_BinaryAnalysis_ModelChecker_PathPredicate_H
#define ROSE_BinaryAnalysis_ModelChecker_PathPredicate_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER

#include <Rose/BinaryAnalysis/ModelChecker/BasicTypes.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Path predicate.
 *
 *  A path predicate is a user-defined function that returns true or false for a specified path. It is used for such
 *  things as deciding whether a path should be added to a queue.
 *
 *  For instance, when the model checker is about to add a path to the work queue, it will first consult a predicate
 *  to decide if it should do that.  The predicate might reject the path based on attributes such as:
 *
 *  @li Prune paths that have too many steps.
 *
 *  @li Prune paths that go through the same CFG node too many times, thus limiting loop iterations and recursion.
 *
 *  @li Prune paths where the SMT solver takes too long, since exploring further from such a path usually makes the
 *  solver take even longer.
 *
 *  @li Prune paths whose states are becoming too large. An alternative is to reduce the size of large states as part
 *  of executing a path vertex. */
class PathPredicate {
public:
    /** Shared ownership pointer. */
    using Ptr = PathPredicatePtr;

protected:
    PathPredicate();
public:
    virtual ~PathPredicate();

public:
    /** The predicate.
     *
     *  Tests the path for some condition and returns true or false and an optional reason which is a short C-style string
     *  containing only horizontal, printable characters without leading or trailing white space.  The string must not
     *  be null, but may be empty.
     *
     *  For convenience, the predicate can be applied using either the function operator or the @ref test method, although it's
     *  the operator that gets overloaded in the subclasses.
     *
     *  Thread safety: All implementations must be thread safe.
     *
     *  @{ */
    virtual std::pair<bool, const char*> operator()(const SettingsPtr&, const PathPtr&) = 0;
    virtual std::pair<bool, const char*> test(const SettingsPtr&, const PathPtr&) final; // virtual only so we can say final
    /** @} */

    /** Reset internal data.
     *
     *  Some predicates keep track of various statistics, such as how many paths were rejected because they had too many
     *  steps.  This function resets that kind of information. The base implementation does nothing.
     *
     *  Thread safety: All implementations must be thread safe. */
    virtual void reset() {}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Predicate that's always true.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Path predicate that's always true.
 *
 *  This predicate returns true for every path. */
class AlwaysTrue: public PathPredicate {
public:
    using Ptr = AlwaysTruePtr;

public:
    /** Allocating constructor. */
    static Ptr instance();

    virtual std::pair<bool, const char*> operator()(const SettingsPtr&, const PathPtr&) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Default predicate guarding the work queue.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Default work queue predicate.
 *
 *  Any paths inserted into the work queue, other than initial paths inserted by the user when configuring the model checker,
 *  are checked by a predicate, and if the predicate returns false then the path is not inserted.  This predicate is the
 *  default. */
class WorkPredicate: public PathPredicate {
public:
    using Ptr = WorkPredicatePtr;

private:
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;         // protects the following data members
    size_t kLimitReached_ = 0;                          // number of times the K limit was hit
    size_t timeLimitReached_ = 0;                       // number of times the time limit was hit

public:
    /** Allocating constructor. */
    static Ptr instance();

    /** Property: Number of times K limit was reached.
     *
     *  This property stores the number of times that this predicate returned false due to the path length limit being
     *  reached.
     *
     *  Thread safety: This method is thread safe. */
    size_t kLimitReached() const;

    /** Property: Number of times the time limit was reached.
     *
     *  This property stores the number of times that this predicate returned false due to the path time limit being
     *  reached.
     *
     *  Thread safety: This method is thread safe. */
    size_t timeLimitReached() const;

public: // overrides
    virtual std::pair<bool, const char*> operator()(const SettingsPtr&, const PathPtr&) override;
    virtual void reset() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Default predicate guarding the "interesting" queue.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Default interesting predicate.
 *
 *  This predicate is used as the default for determining whether paths are added to the "interesting" queue. It returns true
 *  if the path's final node references one or more tags. */
class HasFinalTags: public PathPredicate {
public:
    using Ptr = HasFinalTagsPtr;

public:
    /** Allocating constructor. */
    static Ptr instance();

    virtual std::pair<bool, const char*> operator()(const SettingsPtr&, const PathPtr&) override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
