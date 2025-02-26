#ifndef ROSE_BinaryAnalysis_CallingConvention_Definition_H
#define ROSE_BinaryAnalysis_CallingConvention_Definition_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/CallingConvention/BasicTypes.h>

#include <Rose/BinaryAnalysis/Architecture/BasicTypes.h>
#include <Rose/BinaryAnalysis/Alignment.h>
#include <Rose/BinaryAnalysis/ByteOrder.h>
#include <Rose/BinaryAnalysis/ConcreteLocation.h>
#include <Rose/BinaryAnalysis/RegisterDescriptor.h>

#include <Sawyer/Optional.h>

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/serialization/access.hpp>
#endif

#include <set>
#include <string>
#include <vector>

namespace Rose {
namespace BinaryAnalysis {
namespace CallingConvention {

/** Information about calling conventions.
 *
 *  A definition typically comes from external documentation rather than direct analysis. */
class Definition: public Sawyer::SharedObject {
public:
    /** Reference counting pointer to calling convention definition. */
    using Ptr = DefinitionPtr;

private:
    // General information
    std::string name_;                                  // Official short name of the convention, like "stdcall".
    std::string comment_;                               // Long name, like "Windows Borland x86-32 fastcall"
    Sawyer::Optional<size_t> bitsPerWord_;              // Optionally override the word width from the architecture

    // The architecture with which this calling convention definition is associated. It is referenced via weak pointer because the
    // architecture typically contains a list of calling convention definitions, so if we had referred here to the architecture with
    // a shared pointer we'd have a cycle. Architecture objects are typically not deleted since they're registered with the library.
    std::weak_ptr<const Architecture::Base> architecture_;

    // Used mostly during analysis
    std::vector<ConcreteLocation> nonParameterInputs_;  // Inputs that are not considered normal function parameters
    std::vector<ConcreteLocation> inputParameters_;     // Input (inc. in-out) parameters; additional stack-based are implied
    std::vector<ConcreteLocation> outputParameters_;    // Return values and output parameters.
    StackParameterOrder stackParameterOrder_ = StackParameterOrder::UNSPECIFIED; // Order of arguments on the stack
    RegisterDescriptor stackPointerRegister_;           // Optionally override the stack pointer register from the architecture
    size_t nonParameterStackSize_ = 0;                  // Size in bytes of non-parameter stack area
    Alignment stackAlignment_;                          // Stack alignment in bytes
    StackDirection stackDirection_ = StackDirection::GROWS_DOWN; // Direction that stack grows from a PUSH operation
    StackCleanup stackCleanup_ = StackCleanup::UNSPECIFIED;      // Who cleans up stack parameters?
    ConcreteLocation thisParameter_;                    // Object pointer for calling conventions that are object methods
    std::set<RegisterDescriptor> calleeSavedRegisters_; // Register that the callee must restore before returning
    std::set<RegisterDescriptor> scratchRegisters_;     // Caller-saved registers
    ConcreteLocation returnAddressLocation_;            // Where is the function return address stored at function entry?
    RegisterDescriptor instructionPointerRegister_;     // Where is the next instruction address stored?

    // These are used to figure out where function arguments and return values are stored given a function declaration. For
    // instance, the first two arguments that are 32 bits wide get stored in register 1 and register 2, and other arguments are
    // stored on the stack.
    AllocatorPtr returnValueAllocator_;
    AllocatorPtr argumentValueAllocator_;

protected:
#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
    friend class boost::serialization::access;
    template<class S> void serialize(S&, unsigned);
#endif

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

    /** Property: Allocator for return values.
     *
     *  Describes where/how return the value(s) are stored.
     *
     *  @{ */
    AllocatorPtr returnValueAllocator() const;
    void returnValueAllocator(const AllocatorPtr&);
    /** @} */

    /** Property: Allocator for argument values.
     *
     *  Describe where/how function arguments are stored.
     *
     *  @{ */
    AllocatorPtr argumentValueAllocator() const;
    void argumentValueAllocator(const AllocatorPtr&);
    /** @} */

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

} // namespace
} // namespace
} // namespace

#endif
#endif
