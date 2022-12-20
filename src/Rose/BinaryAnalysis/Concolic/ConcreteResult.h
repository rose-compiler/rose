#ifndef ROSE_BinaryAnalysis_Concolic_ConcreteResult_H
#define ROSE_BinaryAnalysis_Concolic_ConcreteResult_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>

#include <Sawyer/SharedObject.h>
#include <Sawyer/SharedPointer.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Base class for user-defined concrete execution results.
 *
 *  Regardless of what data a subclass might add, all concrete execution results have a floating-point "rank" used to sort them
 *  when creating the list of test cases that should next run in the concolic (combined concrete and symbolic) executor. The
 *  convention is that those with lower ranks will run before those with higher ranks, although subclasses of @ref
 *  ExecutionManager can override this. The rank should be a real number (not NaN).
 *
 *  The subclasses must provide @c boost::serialization support which is used to store the user-defined results in the database
 *  and to reconstitute results objects from the database. Since this is a relatively expensive operation, the rank is also
 *  stored separately within the database. */
class ConcreteResult: public Sawyer::SharedObject {
public:
    /** Shared ownership pointer. */
    using Ptr = ConcreteResultPtr;

private:
    double rank_;
    bool isInteresting_;

protected:
    // Allocating constructors are named "instance" in the subclasses
    ConcreteResult();

    explicit ConcreteResult(double rank);

public:
    virtual ~ConcreteResult();

public:
    /** Property: Relative rank of test case concrete execution result.
     *
     *  The rank is used to prioritize which test cases will run first in concolic execution. Lower ranks have higher priority
     *  than higher ranks.
     *
     *  The rank is never NaN.
     *
     * @{ */
    double rank() const;
    void rank(double r);
    /** @} */

    /** Property: Whether this concrete execution is interesting.
     *
     * @{ */
    bool isInteresting() const;
    void isInteresting(bool b);
    /** @} */

private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(rank_);
        s & BOOST_SERIALIZATION_NVP(isInteresting_);
    }
};

} // namespace
} // namespace
} // namespace

#endif
#endif
