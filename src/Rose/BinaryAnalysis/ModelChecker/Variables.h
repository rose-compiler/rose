#ifndef ROSE_BinaryAnalysis_ModelChecker_Variables_H
#define ROSE_BinaryAnalysis_ModelChecker_Variables_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER

#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Rose/BinaryAnalysis/Variables.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** Information about a global or local variable.
 *
 *  This contains either a local or global variable and the concrete memory location at which the variable is located. */
class FoundVariable {
    AddressInterval where_;                              // addresses for the variable
    Variables::GlobalVariable gvar_;                     // global variable if found
    Variables::StackVariable lvar_;                      // local variable if found and no global variable

public:
    ~FoundVariable();

    /** Default constructor. */
    FoundVariable();

    /** Construct an object that refers to a global variable. */
    explicit FoundVariable(const Variables::GlobalVariable&);

    /** Construct an object that refers to a local variable.
     *
     *  The @c AddressInterval describes where the variable occurs in memory since the @c StackVariable argument only contains
     *  information about the location of the variable with respect to the stack frame. */
    FoundVariable(const AddressInterval&, const Variables::StackVariable&);

    /** Property: Location of variable.
     *
     *  If this object is valid, then this returns the non-empty region of memory that contains this variable. Otherwise an
     *  empty interval is returned. */
    const AddressInterval& where() const;

    /** Property: Local variable.
     *
     *  If this object is valid and contains a local variable, then that variable is returned. Otherwise an empty local
     *  variable is returned. */
    const Variables::StackVariable& stackVariable() const;

    /** Property: Global variable.
     *
     *  If this object is valid and contains a global variable, then that variable is returned. Otherwise an empty global
     *  variable is returned. */
    const Variables::GlobalVariable& globalVariable() const;

    /** Property: Containing function.
     *
     *  For local variables, this is an optional function where the variable is defined. Otherwise returns a null pointer. */
    Partitioner2::FunctionPtr function() const;

    /** True if this object contains a variable.
     *
     * @{ */
    bool isValid() const;
    explicit operator bool() const;
    /** @} */

    /** True if this object does not contain a variable. */
    bool operator!() const;

    /** Stream textual representation to output. */
    void print(std::ostream&) const;

    /** Convert to string. */
    std::string toString() const;
};

std::ostream&
operator<<(std::ostream&, const FoundVariable&);

} // namespace
} // namespace
} // namespace

#endif
#endif
