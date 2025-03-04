#ifndef ROSE_BinaryAnalysis_Concolic_M68kSystem_TracingResult_H
#define ROSE_BinaryAnalysis_Concolic_M68kSystem_TracingResult_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>

#include <Rose/BinaryAnalysis/Concolic/ConcreteResult.h>
#include <Sawyer/Trace.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace M68kSystem {

/** M68k concrete execution results. */
class TracingResult: public Concolic::ConcreteResult {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    using Super = Concolic::ConcreteResult;

public:
    /** Reference counting pointer. */
    using Ptr = TracingResultPtr;

    /** Type for instruction traces. */
    using Trace = Sawyer::Container::Trace<Address, Sawyer::Container::TraceMapIndexTag>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    Trace trace_;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Serialization
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    friend class boost::serialization::access;

    // This isn't the most efficient way to store the trace, but we use it for now because Sawyer::Container::Trace doesn't
    // implement an internal seriazliation. So we convert the trace to a list of addresses and then save the addresses.
    template<class S>
    void save(S &s, const unsigned /*version*/) const {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
        std::vector<Address> vas = trace_.toVector();
        s & BOOST_SERIALIZATION_NVP(vas);
    }

    template<class S>
    void load(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
        std::vector<Address> vas;
        s & BOOST_SERIALIZATION_NVP(vas);
        trace_.clear();
        for (Address va: vas)
            trace_.append(va);
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
    TracingResult();                                    // for boost::serialization
    explicit TracingResult(double rank);
public:
    ~TracingResult();

public:
    static Ptr instance(double rank);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Instruction trace.
     *
     *  This contains the trace of the addresses of instructions that were executed.
     *
     * @{ */
    const Trace& trace() const;
    Trace& trace();
    /** @} */
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
