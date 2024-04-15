#ifndef ROSE_BinaryAnalysis_Concolic_I386Linux_ExitStatusResult_H
#define ROSE_BinaryAnalysis_Concolic_I386Linux_ExitStatusResult_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>

#include <Rose/BinaryAnalysis/Concolic/ConcreteResult.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

#include <string>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace I386Linux {

/** Base class for user-defined Linux concrete execution results. */
class ExitStatusResult: public Concolic::ConcreteResult {
    using Super = Concolic::ConcreteResult;

public:
    /** Shared ownership pointer. */
    using Ptr = ExitStatusResultPtr;

protected:
    int         exitStatus_;   /**< Exit status as returned by waitpid[2]. */
    std::string exitKind_;     /**< Textual representation how the Result exited */
    std::string capturedOut_;  /**< Output written to STDOUT */
    std::string capturedErr_;  /**< Output written to STDERR */

private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
        s & BOOST_SERIALIZATION_NVP(exitStatus_);
        s & BOOST_SERIALIZATION_NVP(exitKind_);
        s & BOOST_SERIALIZATION_NVP(capturedOut_);
        s & BOOST_SERIALIZATION_NVP(capturedErr_);
    }

protected:
    ExitStatusResult(double rank, int exitStatus);

    // required for boost serialization
    ExitStatusResult();

public:
    ~ExitStatusResult();

public:
    /** Allocating constructor. */
    static Ptr instance(double rank, int exitStatus);

    /** Property: Exit status of the executable.
     *
     *  The executable exit status is the value returned by waitpid[2] and contains not only the argument of the program's
     *  "exit" function, but also information about whether the program called exit or was terminated by a signal, and whether
     *  the signal produced a core dump.
     *
     * @{ */
    int exitStatus() const;
    void exitStatus(int);
    /** @} */

    /** Property: Output to STDOUT and STDERR of the executable
     *
     * @{ */
    std::string out() const;
    void out(const std::string&);

    std::string err() const;
    void err(const std::string&);
    /** @} */

    /** Property: textual representation of how a test exited.
     *
     * This property is set together with exitStatus. */
    std::string exitKind() const;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
