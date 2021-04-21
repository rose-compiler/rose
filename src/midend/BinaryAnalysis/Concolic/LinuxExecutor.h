#ifndef ROSE_BinaryAnalysis_Concolic_LinuxExecutor_H
#define ROSE_BinaryAnalysis_Concolic_LinuxExecutor_H
#include <Concolic/BasicTypes.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <boost/serialization/export.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>
#include <Concolic/ConcreteExecutor.h>
#include <Sawyer/Optional.h>
#include <Sawyer/SharedPointer.h>
#include <string>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Concrete executor for Linux ELF executables. */
class LinuxExecutor: public ConcreteExecutor {
public:
    /** Reference counting pointer to a @ref LinuxExecutor. */
    typedef Sawyer::SharedPointer<LinuxExecutor> Ptr;

    /** Holds an optional personality-value (i.g., indicating if address randomization should be turned off). */
    typedef Sawyer::Optional<unsigned long> Persona;

    /** Base class for user-defined Linux concrete execution results. */
    class Result: public ConcreteExecutorResult {
    protected:
        int         exitStatus_;   /**< Exit status as returned by waitpid[2]. */
        std::string exitKind_;     /**< Textual representation how the Result exited */
        std::string capturedOut_;  /**< Output written to STDOUT */
        std::string capturedErr_;  /**< Output written to STDERR */

    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ConcreteExecutorResult);
            s & BOOST_SERIALIZATION_NVP(exitStatus_);
            s & BOOST_SERIALIZATION_NVP(exitKind_);
            s & BOOST_SERIALIZATION_NVP(capturedOut_);
            s & BOOST_SERIALIZATION_NVP(capturedErr_);
        }

    public:
        Result(double rank, int exitStatus);

        Result() {} // required for boost serialization

        /** Property: Exit status of the executable.
         *
         *  The executable exit status is the value returned by waitpid[2] and contains not only the argument of the program's
         *  "exit" function, but also information about whether the program called exit or was terminated by a signal, and
         *  whether the signal produced a core dump.
         *
         * @{ */
        int exitStatus() const { return exitStatus_; }
        void exitStatus(int x);
        /** @} */

        /** Property: Output to STDOUT and STDERR of the executable
         *
         * @{ */
        std::string out() const             { return capturedOut_; }
        void out(const std::string& output) { capturedOut_ = output; }

        std::string err() const             { return capturedErr_; }
        void err(const std::string& output) { capturedErr_ = output; }
        /** @} */

        /** Property: textual representation of how a test exited.
         *            The property is set together with exitStatus.
         * @{ */
        std::string exitKind() const            { return exitKind_; }
        /* @} */
    };

protected:
    bool useAddressRandomization_;                      // enable/disable address space randomization in the OS

protected:
    explicit LinuxExecutor(const DatabasePtr&);

public:
    ~LinuxExecutor();

    /** Allocating constructor. */
    static Ptr instance(const DatabasePtr&);

    /** Property: Address space randomization.
     *
     *  This property controls whether the specimen is executed with or without OS address space randomization. It's usually
     *  better to turn off randomization for repeatable results, and it is therefore off by default.
     *
     * @{ */
    bool useAddressRandomization() const { return useAddressRandomization_; }
    void useAddressRandomization(bool b) { useAddressRandomization_ = b; }
    /** @} */

    virtual
    ConcreteExecutorResult* execute(const TestCasePtr&) ROSE_OVERRIDE;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
