#ifndef ROSE_BinaryAnalysis_Concolic_SystemCall_H
#define ROSE_BinaryAnalysis_Concolic_SystemCall_H
#include <Concolic/BasicTypes.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** System call.
 *
 *  A system object describes how a particular system call behaves. It has two main parts: data members describing exactly
 *  which system call is being refernced, and data members that describe the behavior of the system call such as its return
 *  value. */
class SystemCall: public Sawyer::SharedObject, public Sawyer::SharedFromThis<SystemCall> {
public:
    /** Reference-counting pointer to a @ref SystemCall. */
    typedef Sawyer::SharedPointer<SystemCall> Ptr;

private:
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;         // protects the following data members
    std::string timestamp_;                             // time of creation
    TestCasePtr testCase_;                              // each system call belongs to a test case

    // Identification data members -- these identify which system call we're talking about.
    size_t callNumber_;                                 // serial number of the system call
    size_t functionId_;                                 // system call function identifier, per operating system ISA
    rose_addr_t callSite_;                              // location of system call in the program

    // Operational data members -- these describe how the system call behaves
    int returnValue_;                                   // system call direct return value

protected:
    SystemCall();

public:
    ~SystemCall();

    /* Allocating constructor. */
    static Ptr instance();

    /** Property: Owning test case.
     *
     *  Each system call belongs to exactly one test case.
     *
     *  @{ */
    TestCasePtr testCase();
    void testCase(TestCaseId);
    void testCase(const TestCasePtr&);
    /** @} */

    /** Property: Database creation timestamp string.
     *
     *  Time stamp string describing when this object was created in the database, initialized the first time the object is
     *  written to the database. If a value is assigned prior to writing to the database, then the assigned value is used
     *  instead. The value is typically specified in ISO-8601 format (except a space is used to separate the date and time for
     *  better readability, as in RFC 3339). This allows dates to be sorted chronologically as strings.
     *
     *  Thread safety: This method is thread safe.
     *
     * @{ */
    std::string timestamp() const;
    void timestamp(const std::string&);
    /** @} */

    /** Property: Call sequence number.
     *
     *  System calls are numbered sequentially in the order they occur, starting from zero.
     *
     *  Thread safety: This method is thread safe.
     *
     * @{ */
    size_t callSequenceNumber() const;
    void callSequenceNumber(size_t);
    /** @} */

    /** Property: System call function identifier.
     *
     *  Each system call function has a unique integer that identifies the behavior. For instance, on Linux x86 32-bit, the
     *  identifier 1 refers to the @c __NR_exit system call.
     *
     * @{ */
    size_t functionId() const;
    void functionId(size_t);
    /** @} */

    /** Property: Call site virtual address.
     *
     *  This is the address where the system call occurs.
     *
     * @{ */
    rose_addr_t callSite() const;
    void callSite(rose_addr_t);
    /** @} */

    /** Property: System call direct return value.
     *
     *  This is the concrete, direct return value of the system call. The system call may also have side effects that are not
     *  described by the return value, such as writing to memory whose pointer was supplied as an argument.
     *
     * @{ */
    int returnValue() const;
    void returnValue(int);
    /** @} */

    /** Returns a printable name of a system call for diagnostic output.
     *
     * Returns a string suitable for printing to a terminal, containing the word "syscall" and some additional identifying
     * information. If a database is specified, then the database ID is also shown. */
    std::string printableName(const DatabasePtr &db = DatabasePtr());
};

#include <Sawyer/SharedObject.h>
#include <Sawyer/SharedPointer.h>
#include <string>

} // namespace
} // namespace
} // namespace

#endif
#endif
