#ifndef ROSE_BinaryAnalysis_Concolic_SystemCall_H
#define ROSE_BinaryAnalysis_Concolic_SystemCall_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>
#include <Rose/BinaryAnalysis/SymbolicExpr.h>

#include <Sawyer/Optional.h>
#include <Sawyer/SharedObject.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Description of system calls.
 *
 *  This class describes various things about system calls. For instance, a system call like SYS_getpid should return the same
 *  value every time it's called since a process ID is constant for the life of the process. */
class SystemCall: public Sawyer::SharedObject {
public:
    /** Reference counting pointer. */
    using Ptr = SystemCallPtr;

private:
    bool exitsProcess_ = false;                         // Does this system call cause its process to exit?

    struct {
        bool enabled = false;                           // Does this system call always return the same value?
        Sawyer::Optional<uint64_t> concrete;            // Constant concrete return if known -- the first concrete value returned
        SymbolicExpr::Ptr symbolic;                     // Constant symbolic return if known -- the variable for the first return
    } constantReturn_;

protected:
    SystemCall();

public:
    /** Default allocating constructor. */
    static SystemCallPtr instance();

    ~SystemCall();

public:
    /** Property: Whether this system call exits the process.
     *
     * @{ */
    bool exitsProcess() const;
    void exitsProcess(bool);
    /** @} */

    /** Property: Whether a system call always returns the same value.
     *
     * @{ */
    bool isConstantReturn() const;
    void isConstantReturn(bool);
    /** @} */

    /** Property: Concrete value that's always returned.
     *
     *  If a system call always returns the same value, then this is the concrete value that it returns. If the syscall returns
     *  a constant value but that concrete value is not yet known, then this property holds nothing.
     *
     * @{ */
    const Sawyer::Optional<uint64_t>& constantReturnConcrete() const;
    void constantReturnConcrete(uint64_t);
    /** @} */

    /** Property: Symbolic value that's always returned.
     *
     *  If a system call always returns the same value, then this is the symbolic variable that represents that value. If no
     *  variable has been assigned yet, then this property is a null pointer.
     *
     * @{ */
    SymbolicExpr::Ptr constantReturnSymbolic() const;
    void constantReturnSymbolic(const SymbolicExpr::Ptr&);
    /** @} */
};

} // namespace
} // namespace
} // namespace

#endif
#endif
