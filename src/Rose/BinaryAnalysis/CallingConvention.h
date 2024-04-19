#ifndef ROSE_BinaryAnalysis_CallingConvention_H
#define ROSE_BinaryAnalysis_CallingConvention_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Alignment.h>
#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/ConcreteLocation.h>
#include <Rose/BinaryAnalysis/Disassembler/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Rose/BinaryAnalysis/RegisterParts.h>
#include <Rose/BinaryAnalysis/Variables.h>
#include <Rose/Exception.h>

#include <Sawyer/Optional.h>
#include <Sawyer/Result.h>
#include <Sawyer/SharedObject.h>
#include <Sawyer/SharedPointer.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

// Clean up global namespace pollution
#undef ABSOLUTE

namespace Rose {
namespace BinaryAnalysis {

// Forwards
namespace Disassembler {
class Base;
} // namespace

/** Support for binary calling conventions.
 *
 *  This namespace has three main classes and a number of auxiliary classes. The main classes are:
 *
 *  @li @ref Definition describes a particular calling convention based on external documentation of that calling convention.
 *
 *  @li @ref Dictionary is a collection of calling convention definitions.
 *
 *  @li @ref Analysis performs an analysis and can determine which definitions best match the characteristics of some
 *      specified function. */
namespace CallingConvention {

/** Initialize diagnostics.
 *
 *  This is normally called as part of ROSE's diagnostics initialization, but it doesn't hurt to call it often. */
void initDiagnostics();

/** Facility for diagnostic output.
 *
 *  The facility can be controlled directly or via ROSE's command-line. */
extern Sawyer::Message::Facility mlog;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Miscellaneous small types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** The order that arguments are pushed onto the stack. */
enum class StackParameterOrder {
    LEFT_TO_RIGHT,                                      /**< Stack parameters pushed left to right (Pascal order). */
    RIGHT_TO_LEFT,                                      /**< Stack parameters pushed right to left (C order). */
    UNSPECIFIED,                                        /**< Stack parameter order is unknown or unspecified. */
};

/** The direction in which the stack grows. */
enum class StackDirection {
    GROWS_UP,                                           /**< A push increments the stack pointer. */
    GROWS_DOWN,                                         /**< A push decrements the stack pointer. */
};

/** Who is responsible for popping stack parameters. */
enum class StackCleanup {
    BY_CALLER,                                          /**< The caller pops all stack parameters. */
    BY_CALLEE,                                          /**< The called function pops all stack parameters. */
    UNSPECIFIED,                                        /**< Stack parameter cleanup is unknown or unspecified. */
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Exceptions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Error related to calling convention problems. */
class Exception: public Rose::Exception {
public:
    /** Construct error with specified message. */
    Exception(const std::string &mesg)
        : Rose::Exception(mesg) {}
    virtual ~Exception() throw() {}
};

/** Error occuring when parsing a declaration. */
class ParseError: public Exception {
public:
    /** Construct error with specified message. */
    ParseError(const std::string &mesg)
        : Exception(mesg) {}
    virtual ~ParseError() throw() {}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Definition
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Information about calling conventions.
 *
 *  A definition typically comes from external documentation rather than direct analysis. */
class Definition: public Sawyer::SharedObject {
public:
    /** Reference counting pointer to calling convention definition. */
    using Ptr = DefinitionPtr;

private:
    std::string name_;                                  // Official short name of the convention, like "stdcall".
    std::string comment_;                               // Long name, like "Windows Borland x86-32 fastcall"
    Sawyer::Optional<size_t> bitsPerWord_;              // Optionally override the word width from the architecture
    std::vector<ConcreteLocation> nonParameterInputs_;  // Inputs that are not considered normal function parameters
    std::vector<ConcreteLocation> inputParameters_;     // Input (inc. in-out) parameters; additional stack-based are implied
    std::vector<ConcreteLocation> outputParameters_;    // Return values and output parameters.
    StackParameterOrder stackParameterOrder_ = StackParameterOrder::UNSPECIFIED; // Order of arguments on the stack
    RegisterDescriptor stackPointerRegister_;           // Optionally override the stack pointerregister from the architecture
    size_t nonParameterStackSize_ = 0;                  // Size in bytes of non-parameter stack area
    Alignment stackAlignment_;                          // Stack alignment in bytes
    StackDirection stackDirection_ = StackDirection::GROWS_DOWN; // Direction that stack grows from a PUSH operation
    StackCleanup stackCleanup_ = StackCleanup::UNSPECIFIED;      // Who cleans up stack parameters?
    ConcreteLocation thisParameter_;                    // Object pointer for calling conventions that are object methods
    std::set<RegisterDescriptor> calleeSavedRegisters_; // Register that the callee must restore before returning
    std::set<RegisterDescriptor> scratchRegisters_;     // Caller-saved registers
    ConcreteLocation returnAddressLocation_;            // Where is the function return address stored at function entry?
    RegisterDescriptor instructionPointerRegister_;     // Where is the next instruction address stored?

    // The architecture with which this calling convention definition is associated. It is referenced via weak pointer because the
    // architecture typically contains a list of calling convention definitions, so if we had referred here to the architecture with
    // a shared pointer we'd have a cycle. Architecture objects are typically not deleted since they're registered with the library.
    std::weak_ptr<const Architecture::Base> architecture_;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned version) {
        s & BOOST_SERIALIZATION_NVP(name_);
        s & BOOST_SERIALIZATION_NVP(comment_);
        s & BOOST_SERIALIZATION_NVP(bitsPerWord_);
        s & BOOST_SERIALIZATION_NVP(inputParameters_);
        s & BOOST_SERIALIZATION_NVP(outputParameters_);
        s & BOOST_SERIALIZATION_NVP(stackParameterOrder_);
        s & BOOST_SERIALIZATION_NVP(stackPointerRegister_);
        s & BOOST_SERIALIZATION_NVP(nonParameterStackSize_);
        s & BOOST_SERIALIZATION_NVP(stackDirection_);
        s & BOOST_SERIALIZATION_NVP(stackCleanup_);
        s & BOOST_SERIALIZATION_NVP(thisParameter_);
        s & BOOST_SERIALIZATION_NVP(calleeSavedRegisters_);
        s & BOOST_SERIALIZATION_NVP(scratchRegisters_);
        if (version >= 1) {
            s & BOOST_SERIALIZATION_NVP(returnAddressLocation_);
            s & BOOST_SERIALIZATION_NVP(instructionPointerRegister_);
        }
    }
#endif
    
protected:
    /** Default constructor.
     *
     *  Constructs a new calling convention with no name or parameters. */
    Definition();

    /** Construct a new calling convention.
     *
     *  The name of the calling convention usually comes from the documentation (see @name) and is a single word. The comment
     *  is a more complete name for the convention perhaps including the operating system and architecture but not containing
     *  line termination. */
    Definition(const std::string &name, const std::string &comment, const Architecture::BaseConstPtr&);

public:
    ~Definition();

public:
    /** Allocating constructor. */
    static Ptr instance(const std::string &name, const std::string &comment, const Architecture::BaseConstPtr&);

public:
    /** Property: Architecture with which this definition is associated.
     *
     *  Returns a non-null architecture from a weak pointer, aborting if the architecture has been deleted. Since Architecture
     *  objects contain a dictionary of all their calling conventions, the architecture's lifetime normally exceeds the lifetime
     *  of a calling convention definition and the weak pointer will be valid. */
    Architecture::BaseConstPtr architecture() const;

    /** Property: Register dictionary for the architecture.
     *
     *  This property is read-only, set by the constructor. The register dictionary is not null. */
    RegisterDictionaryPtr registerDictionary() const;

    /** Property: Short name of calling convention.
     *
     *  Standard calling conventions have documented names, like "stdcall" and that's what should appear here. They're usually
     *  a single word.  It is not necessary to include information like compiler, OS, wordsize, etc. Instead, the calling
     *  convention dictionary will have more than one convention named "stdcall" for the various architectures, one for x86
     *  32-bit, another for x86-64, etc.  Also, the @ref comment property can contain a more complete name.
     *
     * @{ */
    const std::string& name() const { return name_; }
    void name(const std::string &s) { name_ = s; }
    /** @} */

    /** Property: Full name of calling convention.
     *
     *  Whereas the @ref name property contains a single-word name, this property contains the full name of the calling
     *  convention. The full name may include an operating system, compiler, architecture, word-size etc. It should not contain
     *  any line termination.
     *
     * @{ */
    const std::string& comment() const { return comment_; }
    void comment(const std::string &s) { comment_ = s; }
    /** @} */

    /** Property: Word size in bits.
     *
     *  This is the natural width of a word measured in bits. The value is positive and usually a multiple of eight.
     *
     *  If this property has no value, then it defaults to the word size of the architecture.
     *
     * @{ */
    size_t bitsPerWord() const;
    void bitsPerWord(const Sawyer::Optional<size_t>&);
    /** @} */

    /** Non-parameter inputs.
     *
     *  This is the list of all allowed inputs that are not function parameters. For instance, things like the instruction
     *  pointer which the caller initializes to be the entry point of the function, or the x86 direction flag "df" which is
     *  normally set by the caller but not considered a function argument.
     *
     * @{ */
    const std::vector<ConcreteLocation>& nonParameterInputs() const { return nonParameterInputs_; }
    std::vector<ConcreteLocation>& nonParameterInputs() { return nonParameterInputs_; }
    /** @} */

    /** Erase all parameters.
     *
     *  Removes all input parameters, output parameters, and object pointer parameter. */
    void clearParameters() {
        nonParameterInputs_.clear();
        clearInputParameters();
        clearOutputParameters();
        thisParameter_ = ConcreteLocation();
    }

    /** Property: Enumerated input parameters.
     *
     *  Returns the vector of input (and in-out) parameters that have been enumerated; does not include implied stack
     *  parameters.  This property is read-only; see also @ref appendInputParameter and @ref clearInputParameters. */
    const std::vector<ConcreteLocation>& inputParameters() const { return inputParameters_; }

    /** Compute the set of input registers.
     *
     *  The returned list is all registers that might serve as function inputs, both function input parameters and
     *  non-parameter inputs. */
    RegisterParts inputRegisterParts() const;

    /** Erase enumerated input parameters.
     *
     *  Removes all enumerated input parameters.  In-out parameters, those input parameters that are also listed as outputs,
     *  are only removed from the input parameter list, effectively converting them to output-only parameters. */
    void clearInputParameters() { inputParameters_.clear(); }

    /** Append input parameter.
     *
     *  This method appends a new parameter to the list of enumerated input or in-out parameters. Generally only non-stack
     *  parameters need to be enumerated since any parameter with a higher index is assumed to be located on the stack.
     *
     * @{ */
    void appendInputParameter(const ConcreteLocation&);
    void appendInputParameter(RegisterDescriptor reg) {
        appendInputParameter(ConcreteLocation(reg));
    }
    void appendInputParameter(RegisterDescriptor reg, int64_t offset) {
        appendInputParameter(ConcreteLocation(reg, offset));
    }
    void appendInputParameter(rose_addr_t va) {
        appendInputParameter(ConcreteLocation(va));
    }
    /** @} */

    /** Property: List of output parameters.
     *
     *  Returns the vector of output (and in-out) parameters.  This property is read-only; see also @ref appendOutputParameter
     *  and @ref clearOutputParameters. */
    const std::vector<ConcreteLocation>& outputParameters() const { return outputParameters_; }

    /** Computes the set of output registers. */
    RegisterParts outputRegisterParts() const;

    /** Erase output parameters.
     *
     *  Removes all output parameters.  In-out parameters, those output parameters that are also listed as inputs, are only
     *  removed from the output parameter list, effectively converting them to input-only parameters. */
    void clearOutputParameters() { outputParameters_.clear(); }

    /** Append output parameter.
     *
     *  This method appends a new output parameter. Output parameters may be the same as input parameters. For instance, if an
     *  input parameter is the EAX register then EAX can also be an output parameter, such as during a Linux system call where
     *  the input is the system call number and the output is the system call return value. Outputs written to the stack need
     *  not be enumerated in the calling convention dictionary.
     *
     * @{ */
    void appendOutputParameter(const ConcreteLocation&);
    void appendOutputParameter(RegisterDescriptor reg) {
        appendOutputParameter(ConcreteLocation(reg));
    }
    void appendOutputParameter(RegisterDescriptor reg, int64_t offset) {
        appendOutputParameter(ConcreteLocation(reg, offset));
    }
    void appendOutputParameter(rose_addr_t va) {
        appendOutputParameter(ConcreteLocation(va));
    }
    /** @} */

    /** Property: Stack parameter order.
     *
     *  Stack-based input parameters need not be enumerated. If N input parameters are enumerated, numbered 0 through N-1, then
     *  parameters N and above are assumed to be on the stack and are called the "implied stack parameters".  This
     *  stackParameterOrder property describes whether a source code statement like <code>function_call(a, b, c)</code> pushes
     *  @c a first (@c LEFT_TO_RIGHT) or @c first (@c RIGHT_TO_LEFT).
     *
     *  If the enumerated input parameter list contains any stack parameters then implied stack parameters are not allowed.
     *
     * @{ */
    StackParameterOrder stackParameterOrder() const { return stackParameterOrder_; }
    void stackParameterOrder(StackParameterOrder x) { stackParameterOrder_ = x; }
    /** @} */

    /** Property: Register for implied stack parameters.
     *
     *  This property holds the register that should be used for implied stack parameters. For instance, on 32-bit x86 this is
     *  probably ESP, where the base address is the ESP value immediately after the @c call instruction.
     *
     *  The value of this property defaults to the stack pointer register for the architecture. Setting the property to an empty
     *  register descriptor causes the default value from the architecture to be used instead.
     *
     *  @{ */
    RegisterDescriptor stackPointerRegister() const;
    void stackPointerRegister(RegisterDescriptor);
    /** @} */

    /** Property: Size of non-parameter stack area.
     *
     *  This is the size in bytes of the final non-parameter information pushed onto the stack by the function call
     *  instruction. For instance, the 32-bit x86 @c call instruction pushes a 4-byte return address, but some architectures
     *  use a link register instead, pushing nothing onto the stack.
     *
     * @{ */
    size_t nonParameterStackSize() const {
        return nonParameterStackSize_;
    }
    void nonParameterStackSize(size_t nBytes) {
        nonParameterStackSize_ = nBytes;
    }    
    /** @} */

    /** Property: Direction that stack grows for a push operation.
     *
     *  Most stacks grow downward for each push operation, so down is the default. Some odd architecture might grow up
     *  instead. In either case, it is assumed that the stack pointer is pointing to the last item pushed rather than one past
     *  the last item.
     *
     * @{ */
    StackDirection stackDirection() const { return stackDirection_; }
    void stackDirection(StackDirection x) { stackDirection_ = x; }
    /** @} */

    /** Property: Order of bytes for multi-byte values in memory.
     *
     *  This property is read only. It's value comes from the architecture set by the constructor. */
    ByteOrder::Endianness byteOrder() const;

    /** Property: Who pops stack parameters.
     *
     *  This property indicates whether the caller is responsible for popping stack parameters, or whether the called function
     *  is responsible for popping stack parameters.  In either case, the non-parameter stack area (usually a return address)
     *  is popped by the called function.
     *
     * @{ */
    StackCleanup stackCleanup() const { return stackCleanup_; }
    void stackCleanup(StackCleanup x) { stackCleanup_ = x; }
    /** @} */

    /** Property: Stack alignment.
     *
     *  This is the stack alignment measured in bytes for the stack pointer before the caller pushes any non-parameters (e.g.,
     *  return address) or parameters.
     *
     * @{ */
    const Alignment& stackAlignment() const;
    void stackAlignment(const Alignment&);
    /** @} */

    /** Property: Object pointer parameter.
     *
     *  Object oriented code has method calls that are usually implemented as functions that take an extra parameter that isn't
     *  always explicitly listed in the high-level source code.  This property describes where the object pointer is located
     *  and should not be included in the list of input parameters.  The reason for not including it in the list of input
     *  parameters is because of the rule that implicit input parameter are allowed only if there are no explicit stack-based
     *  input parameters, and we want to be able to support having a stack-based object pointer along with implicit input
     *  parameters.
     *
     *  It is permissible for an object method to not list its object pointer as an object pointer, but rather treat it as a
     *  normal explicit or implicit input parameter. Doing so will make the calling convention look like its a plain function
     *  rather than an object method.
     *
     * @{ */
    const ConcreteLocation& thisParameter() const { return thisParameter_; }
    void thisParameter(const ConcreteLocation &x) { thisParameter_ = x; }
    void thisParameter(RegisterDescriptor reg) {
        thisParameter(ConcreteLocation(reg));
    }
    void thisParameter(RegisterDescriptor reg, int64_t offset) {
        thisParameter(ConcreteLocation(reg, offset));
    }
    void thisParameter(rose_addr_t va) {
        thisParameter(ConcreteLocation(va));
    }
    /** @} */

    /** Property: Location of return address.
     *
     *  This property stores the location where the function return address is stored. I.e., the location contains the address
     *  to which this function returns after being called.
     *
     * @{ */
    const ConcreteLocation& returnAddressLocation() const { return returnAddressLocation_; }
    void returnAddressLocation(const ConcreteLocation &x) { returnAddressLocation_ = x; }
    /** @} */

    /** Property: Register that points to next instruction to execute.
     *
     *  @{ */
    RegisterDescriptor instructionPointerRegister() const { return instructionPointerRegister_; }
    void instructionPointerRegister(RegisterDescriptor x) { instructionPointerRegister_ = x; }
    /** @} */

    /** Property: Callee-saved registers.
     *
     *  This is the set of registers that the called function must preserve across the call, either by not modifying them or by
     *  saving and then restoring them.  Registers that are used to return values should obviously not be listed here since
     *  they need to be modified in order to return a value.
     *
     *  Registers that are not restored but which are also not listed as output parameters (return values) are assumed to be
     *  scratch registers.  Status flags are typically in this category, as are the stack pointer and instruction pointer. Most
     *  calling conventions have at least a few general purpose registers that can be used as scratch space.
     *
     * @{ */
    const std::set<RegisterDescriptor>& calleeSavedRegisters() const { return calleeSavedRegisters_; }
    std::set<RegisterDescriptor>& calleeSavedRegisters() { return calleeSavedRegisters_; }
    /** @} */

    /** Compute the set of callee-saved registers. */
    RegisterParts calleeSavedRegisterParts() const;

    /** Property: Scratch registers.
     *
     *  This is the set of registers that are not parameters or return values but which are nonetheless can be modified and not
     *  restored by the called function.  These are also known as caller-saved registers since the caller must save and restore
     *  the value across the call if necessary.
     *
     * @{ */
    const std::set<RegisterDescriptor>& scratchRegisters() const { return scratchRegisters_; }
    std::set<RegisterDescriptor>& scratchRegisters() { return scratchRegisters_; }
    /** @} */

    /** Computes the set of scratch registers. */
    RegisterParts scratchRegisterParts() const;

    /** Returns all registers mentioned in this definition.
     *
     *  The registers are returned as a RegisterParts, which tracks which bits of registers are present but not which
     *  individual registers were inserted into the container.  For instance, if x86 AX is inserted first, then inserting the
     *  overlapping AL and AH registers is a no-op since their bits are already present.
     *
     *  The return value does not include registers that are incidental to a parameter's location. For instance, the stack
     *  pointer register is not included in this list unless its listed as a return register, callee-saved register, or scratch
     *  register. */
    RegisterParts getUsedRegisterParts() const;

    /** Print detailed information about this calling convention.
     *
     *  If a register dictionary is supplied then that dictionary is used instead of any dictionary already attached to this
     *  definition. This feature is mostly for backward compatibility.
     *
     * @{ */
    void print(std::ostream&) const;
    void print(std::ostream&, const RegisterDictionaryPtr &regDict) const;
    /** @} */
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Dictionary
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** A ordered collection of calling convention definitions. */
typedef std::vector<Definition::Ptr> Dictionary;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Declaration
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Function declaration.
 *
 *  A function declaration consists of a return type, a list of argument types, and a calling convention. The declaration can
 *  be built directly from those pieces, or parsed from a C-like function declaration. */
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Analysis
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Function calling convention.
 *
 *  This class encapsulates all information about calling conventions including the analysis functions and the data types. */
class Analysis {
private:
    InstructionSemantics::BaseSemantics::DispatcherPtr cpu_;
    RegisterDictionaryPtr regDict_;                     // Names for the register parts
    Definition::Ptr defaultCc_;                         // Default calling convention for called functions

    bool hasResults_;                                   // Are the following data members initialized?
    bool didConverge_;                                  // Are the following data members valid (else only approximations)?
    RegisterParts restoredRegisters_;                   // Registers accessed but restored
    RegisterParts inputRegisters_;                      // Registers that serve as possible input parameters
    RegisterParts outputRegisters_;                     // Registers that hold possible return values
    Variables::StackVariables inputStackParameters_;    // Stack variables serving as function inputs
    Variables::StackVariables outputStackParameters_;   // Stack variables serving as possible return values
    Sawyer::Optional<int64_t> stackDelta_;              // Change in stack across entire function
    // Don't forget to update clearResults() and serialize() if you add more.

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(cpu_);
        s & BOOST_SERIALIZATION_NVP(regDict_);
        s & BOOST_SERIALIZATION_NVP(defaultCc_);
        s & BOOST_SERIALIZATION_NVP(hasResults_);
        s & BOOST_SERIALIZATION_NVP(didConverge_);
        s & BOOST_SERIALIZATION_NVP(restoredRegisters_);
        s & BOOST_SERIALIZATION_NVP(inputRegisters_);
        s & BOOST_SERIALIZATION_NVP(outputRegisters_);
        s & BOOST_SERIALIZATION_NVP(inputStackParameters_);
        s & BOOST_SERIALIZATION_NVP(outputStackParameters_);
        s & BOOST_SERIALIZATION_NVP(stackDelta_);
    }
#endif

public:
    /** Default constructor.
     *
     *  This creates an analyzer that is not suitable for analysis since it doesn't know anything about the architecture it
     *  would be analyzing. This is mostly for use in situations where an analyzer must be constructed as a member of another
     *  class's default constructor, in containers that initialize their contents with a default constructor, etc. */
    Analysis();
    ~Analysis();

    /** Construct an analyzer using a specified disassembler.
     *
     *  This constructor chooses a symbolic domain and a dispatcher appropriate for the disassembler's architecture. */
    explicit Analysis(const Disassembler::BasePtr&);

    /** Construct an analysis using a specified dispatcher.
     *
     *  This constructor uses the supplied dispatcher and associated semantic domain. For best results, the semantic domain
     *  should be a symbolic domain that uses @ref InstructionSemantics::BaseSemantics::MemoryCellList "MemoryCellList" and
     *  @ref InstructionSemantics::BaseSemantics::RegisterStateGeneric "RegisterStateGeneric". These happen to also be the
     *  defaults used by @ref InstructionSemantics::SymbolicSemantics. */
    explicit Analysis(const InstructionSemantics::BaseSemantics::DispatcherPtr&);

    /** Property: Default calling convention.
     *
     *  The data-flow portion of the analysis uses analysis results previously computed for called functions. If a called
     *  function has no previous analysis result then a default calling convention can be specified for this property and that
     *  convention's definition determines how the called function modifies the current function's data-flow state.
     *
     * @{ */
    Definition::Ptr defaultCallingConvention() const { return defaultCc_; }
    void defaultCallingConvention(const Definition::Ptr &x) { defaultCc_ = x; }
    /** @} */

    /** Analyze one function.
     *
     *  This analysis method uses @ref Partitioner2 data structures which are generally faster than using the AST. The
     *  specified function need not be attached to the partitioner. Results of the analysis are stored in this analysis
     *  object to be queried after the analysis completes. */
    void analyzeFunction(const Partitioner2::PartitionerConstPtr&, const Sawyer::SharedPointer<Partitioner2::Function>&);

    /** Whether a function has been analyzed.
     *
     *  Returns true if this analysis object holds results from analyzing a function. The results might be only approximations
     *  depending on whether @ref didConverge also returns true. */
    bool hasResults() const { return hasResults_; }

    /** Whether the analysis results are valid.
     *
     *  Returns true if @ref hasResults is true and the analysis converged to a solution.  If the analysis did not converge
     *  then the other results are only approximations. */
    bool didConverge() const { return didConverge_; }

    /** Clear analysis results.
     *
     *  Resets the analysis results so it looks like this analyzer is initialized but has not run yet. When this method
     *  returns, @ref hasResults and @ref didConverge will both retun false. */
    void clearResults();

    /** Clears everything but results.
     *
     *  This resets the virtual CPU to the null pointer, possibly freeing some memory if the CPU isn't being used for other
     *  things. Once the CPU is removed it's no longer possible to do more analysis. */
    void clearNonResults();

    /** Property: Register dictionary.
     *
     *  The register dictionary provides names for register parts.  If a dictionary is provided before @ref analyzeFunction is
     *  called then that dictionary is used if possible, otherwise @ref analyzeFunction assigns a new dictionary. In any case,
     *  this property is non-null after a call to @ref analyzeFunction.
     *
     * @{ */
    RegisterDictionaryPtr registerDictionary() const;
    void registerDictionary(const RegisterDictionaryPtr &d);
    /** @} */

    /** Callee-saved registers.
     *
     *  Returns the set of registers that are accessed by the function but which are also reset to their original values before
     *  the function returns. A callee-saved register will not be included in the set of input or output registers. */
    const RegisterParts& calleeSavedRegisters() const { return restoredRegisters_; }

    /** Input registers.
     *
     *  Returns the set of registers that the function uses as input values. These are the registers that the function reads
     *  without first writing and which the function does not restore before returning. */
    const RegisterParts& inputRegisters() const { return inputRegisters_; }

    /** Output registers.
     *
     *  Returns the set of registers into which the function writes potential return values.  These are the registers to which
     *  the function writes with no following read.  An output register is not also a calle-saved register. */
    const RegisterParts& outputRegisters() const { return outputRegisters_; }

    /** Input stack parameters.
     *
     *  Locations for stack-based parameters that are used as inputs to the function. */
    const Variables::StackVariables& inputStackParameters() const { return inputStackParameters_; }

    /** Output stack parameters.
     *
     *  Locations for stack-based parameters that are used as outputs of the function. */
    const Variables::StackVariables& outputStackParameters() const { return outputStackParameters_; }

    /** Concrete stack delta.
     *
     *  This is the amount added to the stack pointer by the function.  For caller-cleanup this is usually just the size of the
     *  non-parameter area (the return address). Stack deltas are measured in bytes. */
    Sawyer::Optional<int64_t> stackDelta() const { return stackDelta_; }

    /** Determine whether a definition matches.
     *
     *  Returns true if the specified definition is compatible with the results of this analysis. */
    bool match(const Definition::Ptr&) const;

    /** Find matching calling convention definitions.
     *
     *  Given an ordered list of calling convention definitons (a calling convention dictionary) return a list of definitions
     *  that is consistent with the results of this analysis.  The definitions in the returned list are in the same order as
     *  those in the specified dictionary. */
    Dictionary match(const Dictionary&) const;

    /** Print information about the analysis results.
     *
     *  The output is a single line of comma-separated values if @p multiLine is true. Otherwise, the top-level commas are
     *  replaced by linefeeds. */
    void print(std::ostream&, bool multiLine=false) const;

private:
    // Finish constructing
    void init(const Disassembler::BasePtr&);

    // Recompute the restoredRegisters_ data member.
    void updateRestoredRegisters(const InstructionSemantics::BaseSemantics::StatePtr &initialState,
                                 const InstructionSemantics::BaseSemantics::StatePtr &finalState);

    // Recompute the inputRegisters_ data member after updateRestoredRegisters is computed.
    void updateInputRegisters(const InstructionSemantics::BaseSemantics::StatePtr &state);

    // Recompute the outputRegisters_ data member after updateRestoredRegisters is computed.
    void updateOutputRegisters(const InstructionSemantics::BaseSemantics::StatePtr &state);

    // Recompute the input and output stack variables
    void updateStackParameters(const Partitioner2::FunctionPtr &function,
                               const InstructionSemantics::BaseSemantics::StatePtr &initialState,
                               const InstructionSemantics::BaseSemantics::StatePtr &finalState);

    // Recomputes the stack delta
    void updateStackDelta(const InstructionSemantics::BaseSemantics::StatePtr &initialState,
                          const InstructionSemantics::BaseSemantics::StatePtr &finalState);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Free functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Read a function argument from a semantic state. */
InstructionSemantics::BaseSemantics::SValuePtr
readArgument(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&, const Definition::Ptr&, size_t argNumber);

/** Write a function argument to a semantic state. */
void writeArgument(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&, const Definition::Ptr&,
                   size_t argNumber, const InstructionSemantics::BaseSemantics::SValuePtr &value);

/** Read the return value that a function is returning. */
InstructionSemantics::BaseSemantics::SValuePtr
readReturnValue(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&, const Definition::Ptr&);

/** Write a value to a function return semantic state. */
void writeReturnValue(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&, const Definition::Ptr&,
                      const InstructionSemantics::BaseSemantics::SValuePtr &returnValue);

/** Simulate a function return.
 *
 *  The RISC operator's current state is adjusted as if a function with the specified calling convention returned. */
void simulateFunctionReturn(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&, const Definition::Ptr&);

/** Print a definition. */
std::ostream& operator<<(std::ostream&, const Definition&);

/** Print analysis information. */
std::ostream& operator<<(std::ostream&, const Analysis&);

} // namespace
} // namespace
} // namespace

// Class versions must be at global scope
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::CallingConvention::Definition, 1);

#endif
#endif
