#ifndef ROSE_BinaryAnalysis_CallingConvention_Declaration_H
#define ROSE_BinaryAnalysis_CallingConvention_Declaration_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/CallingConvention/BasicTypes.h>

#include <Rose/BinaryAnalysis/ConcreteLocation.h>

#include <Sawyer/Result.h>
#include <Sawyer/SharedObject.h>

#include <string>
#include <vector>

class SgAsmType;

namespace Rose {
namespace BinaryAnalysis {
namespace CallingConvention {

/** Function declaration.
 *
 *  A function declaration consists of a return type, a list of argument types, and a calling convention. The declaration can
 *  be built directly from those pieces, or parsed from a C-like function declaration.
 *
 *  Although the specification language for this API is C-like, it does not necessarily follow C semantics. For instance, if you're
 *  describing a C function that takes a 16-bit `unsigned short` argument, and this function is intended to describe the situation
 *  on a machine where C `int` is 32 bits, then the declaration string for the constructor should give the type as `u32` because the
 *  C language specifies that `unsigned short` is promoted to `unsigned int` in the function call. */
class Declaration: public Sawyer::SharedObject {
public:
    /** Shared-ownership pointer. */
    using Ptr = DeclarationPtr;

private:
    std::string name_;                                  // optional declaration name
    std::string comment_;                               // optional declaration comment
    std::string sourceCode_;                            // string from which this declaration was parsed
    DefinitionPtr callingConvention_;                   // required calling convention definition
    SgAsmType *returnType_ = nullptr;                   // type of the function return value
    std::vector<std::pair<SgAsmType*, std::string>> arguments_; // types (and names) of the arguments

public:
    ~Declaration();
protected:
    explicit Declaration(const DefinitionPtr&);
public:
    /** Create a function declaration by parsing a C-like declaration.
     *
     *  The string is of the form "RETURN_TYPE(ARG_TYPE_1, ...)".
     *
     *  The following types are recognized:
     *
     *  @li `uN` indicates an unsigned integral type whose width is `N` bits, where `N` must be 8, 16, 32, or 64.
     *  @li `iN` indicates a signed integral type whose width is `N` bits, where `N` must be 8, 16, 32, or 64.
     *  @li `fN` indicates a floating-point type whose width is `N` bits, where `N` must be 32 or 64.
     *  @li `T*` indicates a pointer to type `T`.
     *  @li `void` as a return type indicates that the function does not return a value. The type `void*` can be used as an argument
     *  type to indicate that the argument is a pointer to something that is unknown. The pointer width is equal to the default
     *  word width defined in the calling convention.
     *
     *  An argument type may be followed by an argument name, in which case the name must be unique for this declaration. The return
     *  type may be followed by a function name.
     *
     *  This intentionally small list of types might be extended in the future.
     *
     *  Examples:
     *
     *  @code
     *   auto decl1 = Declaration::instance(cdecl, "f32(f32, f32)");
     *   auto decl2 = Declaration::instance(cdecl, "u32 strlen(u8* str)");
     *  @endcode */
    static Ptr instance(const DefinitionPtr&, const std::string&);

public:
    /** Property: Name.
     *
     *  Optional name for this declaration.
     *
     *  @{ */
    const std::string& name() const;
    void name(const std::string&);
    /** @} */

    /** Property: Comment.
     *
     *  Optional comment for this declaration.
     *
     *  @{ */
    const std::string& comment() const;
    void comment(const std::string&);
    /** @} */

    /** Source code from which declaration was parsed. */
    const std::string& toString() const;

    /** Property: Calling convention.
     *
     *  The calling convention is never null. */
    DefinitionPtr callingConvention() const;

    /** Property: Return type.
     *
     *  The return type is never null. If the function doesn't return a value then the return type is an instance of
     *  @ref SgAsmVoidType. The return type is read-only, set by the constructor. */
    SgAsmType* returnType() const;

    /** Property: Number of arguments.
     *
     *  The number of arguments is read-only, set by the constructor. */
    size_t nArguments() const;

    /** Property: Argument type.
     *
     *  Returns the type for the specified argument. The type is never null, nor can it be an instance of @ref SgAsmVoidType. The
     *  zero-origin index must be valid for the number of arguments (see @ref nArguments). This property is read-only, set by the
     *  constructor. */
    SgAsmType* argumentType(size_t index) const;

    /** Property: Optional argument name.
     *
     *  Returns an optional name for the argument. If an argument has a name, then it is guaranteed to be unique among all the
     *  argument names in this declaration. The zero-origin index must be valid for the number of arguments (see @ref
     *  nArguments). This property is read-only, set by the constructor. */
    const std::string& argumentName(size_t index) const;

    /** Property: The type and name for each argument. */
    const std::vector<std::pair<SgAsmType*, std::string>>& arguments() const;

public:
    /** Return the concrete location for a function argument, or an error string.
     *
     *  Locations that are relative to the stack pointer assume that instruction pointer is at the first instruction of the called
     *  function and the instruction has not yet been executed.
     *
     *  @{ */
    Sawyer::Result<ConcreteLocation, std::string> argumentLocation(size_t index) const;
    Sawyer::Result<ConcreteLocation, std::string> argumentLocation(const std::string &argName) const;
    /** @} */

};

} // namespace
} // namespace
} // namespace

#endif
#endif
