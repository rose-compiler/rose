#ifndef ROSE_BinaryAnalysis_CallingConvention_H
#define ROSE_BinaryAnalysis_CallingConvention_H

#include <BaseSemantics2.h>
#include <BinaryStackVariable.h>
#include <Partitioner2/BasicTypes.h>
#include <Registers.h>
#include <RegisterParts.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <Sawyer/SharedObject.h>
#include <Sawyer/SharedPointer.h>

namespace Rose {
namespace BinaryAnalysis {

// Forwards
class Disassembler;

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
enum StackParameterOrder {
    LEFT_TO_RIGHT,                                      /**< Stack parameters pushed left to right (Pascal order). */
    RIGHT_TO_LEFT,                                      /**< Stack parameters pushed right to left (C order). */
    ORDER_UNSPECIFIED,                                  /**< Stack parameter order is unknown or unspecified. */
};

/** The direction in which the stack grows. */
enum StackDirection {
    GROWS_UP,                                           /**< A push increments the stack pointer. */
    GROWS_DOWN,                                         /**< A push decrements the stack pointer. */
};

/** Who is responsible for popping stack parameters. */
enum StackCleanup {
    CLEANUP_BY_CALLER,                                  /**< The caller pops all stack parameters. */
    CLEANUP_BY_CALLEE,                                  /**< The called function pops all stack parameters. */
    CLEANUP_UNSPECIFIED,                                /**< Stack parameter cleanup is unknown or unspecified. */
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      ParameterLocation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Abstract parameter location.
 *
 *  This class is used to describe the location of a parameter or return value location in a calling convention definition
 *  outside any analysis (locations resulting from an analysis often contain more information). The location can be a register,
 *  a memory location at a constant address (e.g., global variable), or a memory location relative to some register (e.g., on a
 *  stack).  Location descriptors are immutable.
 *
 *  The same type is used for input parameters, output parameters, and in-out parameters. Return values are a kind of
 *  output parameter, although the API usually does not include the return value when it talks about "parameters". */
  #undef ABSOLUTE
class ParameterLocation {
public:
    /** Type of location. */
    enum Type {
        NO_LOCATION,                                /**< Used by default-constructed locations. */
        REGISTER,                                   /**< Parameter is in a register. */
        STACK,                                      /**< Parameter in memory relative to a register. E.g., stack. */
        ABSOLUTE                                    /**< Parameter is at a fixed memory address. */
    };

private:
    Type type_;
    RegisterDescriptor reg_;                        // The argument register, or the stack base register.
    union {
        int64_t offset_;                            // Offset from stack base register for stack-based locations.
        rose_addr_t va_;                            // Absolute address
    };

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(type_);
        s & BOOST_SERIALIZATION_NVP(reg_);
        if (STACK==type_) {
            s & BOOST_SERIALIZATION_NVP(offset_);
        } else {
            s & BOOST_SERIALIZATION_NVP(va_);
        }
    }
#endif
    
public:
    /** Default constructed no-location.
     *
     *  This default constructor is useful for indicating no location or for using an STL container, such as @c
     *  std::vector, that requires a default constructor.  The @ref isValid predicate will return false for
     *  default-constructed locations. */
    ParameterLocation()
        : type_(NO_LOCATION), offset_(0) {}

    /** Constructs a parameter in a register location. */
    explicit ParameterLocation(RegisterDescriptor reg)
        : type_(REGISTER), reg_(reg), offset_(0) {}

    /** Constructs a parameter at a register-relative memory address. */
    ParameterLocation(RegisterDescriptor reg, int64_t offset)
        : type_(STACK), reg_(reg), offset_(offset) {}

    /** Constructs a parameter at a fixed memory address. */
    explicit ParameterLocation(rose_addr_t va)
        : type_(ABSOLUTE), va_(va) {}

    /** Type of parameter location. */
    Type type() const { return type_; }

    /** Predicate to determine if location is valid.
     *
     *  Returns false for default-constructed locations, true for all others. */
    bool isValid() const {
        return type() != NO_LOCATION;
    }

    /** Register part of location.
     *
     *  Returns the register where the parameter is stored (for register parameters) or the register holding the base
     *  address for register-relative memory parameters.  Returns an invalid (default constructed) register descriptor when
     *  invoked on a default constructed location or a fixed memory addresses. */
    RegisterDescriptor reg() const {
        return reg_;
    }

    /** Offset part of location.
     *
     *  Returns the signed byte offset from the base register for register-relative memory parameters.  The memory address
     *  of the parameter is the contents of the base register plus this byte offset. Returns zero for register parameters,
     *  parameters stored at fixed memory addresses, and default constructed locations. */
    int64_t offset() const {
        return STACK == type_ ? offset_ : (int64_t)0;
    }

    /** Fixed address location.
     *
     *  Returns the address for a parameter stored at a fixed memory address.  Returns zero for register parameters,
     *  register-relative (stack) parameters, and default constructed locations. */
    rose_addr_t address() const {
        return ABSOLUTE == type_ ? va_ : (rose_addr_t)0;
    }

    /** Equality.
     *
     *  Two locations are equal if they are the same type and register, offset, and/or address as appropriate to the type. */
    bool operator==(const ParameterLocation &other) const {
        return type_ == other.type_ && reg_ == other.reg_ && offset_ == other.offset_; // &va_ == &offset_
    }

    /** Inequality.
     *
     *  Two locations are unequal if they have different types, registers, offsets, or addresses. */
    bool operator!=(const ParameterLocation &other) const {
        return type_ != other.type_ || reg_ != other.reg_ || offset_ != other.offset_; // &va_ == &offset_
    }

    /** Print location.
     *
     * @{ */
    void print(std::ostream &out, const RegisterDictionary *regdict) const {
        print(out, RegisterNames(regdict));
    }
    void print(std::ostream &out, const RegisterNames &regnames) const {
        switch (type_) {
            case NO_LOCATION: out <<"nowhere"; break;
            case REGISTER: out <<regnames(reg_); break;
            case STACK: out <<"mem[" <<regnames(reg_) <<"+" <<offset_ <<"]"; break;
            case ABSOLUTE: out <<"mem[" <<StringUtility::addrToString(va_) <<"]"; break;
        }
    }
    /** @} */

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Definition
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Reference counting pointer to calling convention definition. */
typedef Sawyer::SharedPointer<class Definition> DefinitionPtr;

/** Information about calling conventions.
 *
 *  A definition typically comes from external documentation rather than direct analysis. */
class Definition: public Sawyer::SharedObject {
public:
    /** Reference counting pointer to calling convention definition. */
    typedef Sawyer::SharedPointer<Definition> Ptr;

private:
    std::string name_;                                  // Official short name of the convention, like "stdcall".
    std::string comment_;                               // Long name, like "Windows Borland x86-32 fastcall"
    size_t wordWidth_;                                  // Natural width word size in bits
    const RegisterDictionary *regDict_;                 // Register dictionary used when this definition was created
    std::vector<ParameterLocation> inputParameters_;    // Input (inc. in-out) parameters; additional stack-based are implied
    std::vector<ParameterLocation> outputParameters_;   // Return values and output parameters.
    StackParameterOrder stackParameterOrder_;           // Order of arguments on the stack
    RegisterDescriptor stackPointerRegister_;           // Base pointer for implied stack parameters
    size_t nonParameterStackSize_;                      // Size in bytes of non-parameter stack area
    size_t stackAlignment_;                             // Stack alignment in bytes (zero means unknown)
    StackDirection stackDirection_;                     // Direction that stack grows from a PUSH operation
    StackCleanup stackCleanup_;                         // Who cleans up stack parameters?
    ParameterLocation thisParameter_;                   // Object pointer for calling conventions that are object methods
    std::set<RegisterDescriptor> calleeSavedRegisters_; // Register that the callee must restore before returning
    std::set<RegisterDescriptor> scratchRegisters_;     // Caller-saved registers

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(name_);
        s & BOOST_SERIALIZATION_NVP(comment_);
        s & BOOST_SERIALIZATION_NVP(wordWidth_);
        s & BOOST_SERIALIZATION_NVP(regDict_);
        s & BOOST_SERIALIZATION_NVP(inputParameters_);
        s & BOOST_SERIALIZATION_NVP(outputParameters_);
        s & BOOST_SERIALIZATION_NVP(stackParameterOrder_);
        s & BOOST_SERIALIZATION_NVP(stackPointerRegister_);
        s & BOOST_SERIALIZATION_NVP(nonParameterStackSize_);
        s & BOOST_SERIALIZATION_NVP(stackAlignment_);
        s & BOOST_SERIALIZATION_NVP(stackDirection_);
        s & BOOST_SERIALIZATION_NVP(stackCleanup_);
        s & BOOST_SERIALIZATION_NVP(thisParameter_);
        s & BOOST_SERIALIZATION_NVP(calleeSavedRegisters_);
        s & BOOST_SERIALIZATION_NVP(scratchRegisters_);
    }
#endif
    
protected:
    /** Default constructor.
     *
     *  Constructs a new calling convention with no name or parameters. */
    Definition()
        : wordWidth_(0), regDict_(NULL), stackParameterOrder_(ORDER_UNSPECIFIED), nonParameterStackSize_(0),
          stackAlignment_(0), stackDirection_(GROWS_DOWN), stackCleanup_(CLEANUP_UNSPECIFIED) {}

    /** Construct a new calling convention.
     *
     *  The name of the calling convention usually comes from the documentation (see @name) and is a single word. The comment
     *  is a more complete name for the convention perhaps including the operating system and architecture but not containing
     *  line termination. */
    Definition(size_t wordWidth, const std::string &name, const std::string &comment, const RegisterDictionary *regDict)
        : name_(name), comment_(comment), wordWidth_(wordWidth), regDict_(regDict), stackParameterOrder_(ORDER_UNSPECIFIED),
          nonParameterStackSize_(0), stackAlignment_(0), stackDirection_(GROWS_DOWN), stackCleanup_(CLEANUP_UNSPECIFIED) {
        ASSERT_require2(0 == (wordWidth & 7) && wordWidth > 0, "word size must be a positive multiple of eight");
    }

public:
    /** Allocating constructor. */
    static Ptr instance(size_t wordWidth, const std::string &name, const std::string &comment, const RegisterDictionary *regs) {
        return Ptr(new Definition(wordWidth, name, comment, regs));
    }

public:
    /** Returns a predefined, cached calling convention.
     *
     * @{ */
    static Ptr x86_32bit_cdecl();
    static Ptr x86_64bit_cdecl();
    static Ptr x86_32bit_stdcall();
    static Ptr x86_64bit_stdcall();
    static Ptr x86_32bit_fastcall();
    static Ptr x86_64bit_sysv();
    static Ptr ppc_32bit_ibm();
    /** @} */

    /** Constructs a new pre-defined calling convention based on a register dictionary.
     *
     * @{ */
    static Ptr x86_cdecl(const RegisterDictionary*);
    static Ptr x86_stdcall(const RegisterDictionary*);
    static Ptr x86_fastcall(const RegisterDictionary*);
    static Ptr ppc_ibm(const RegisterDictionary*);
    /** @} */

    /** Property: Register dictionary.
     *
     *  The register dictionary imparts names to the various register descriptors.
     *
     * @{ */
    const RegisterDictionary* registerDictionary() const { return regDict_; }
    void registerDictionary(const RegisterDictionary *d) { regDict_ = d; }
    /** @} */

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
     *  This is the natural width of a word measured in bits. When searching for a matching calling convention only those
     *  calling conventions having the desired word width are considered.
     *
     *  This is named "width" instead of "size" because the binary analysis API generally uses "width" to measure in units of
     *  bits, while "size" measures in units of bytes.
     *
     * @{ */
    size_t wordWidth() const { return wordWidth_; }
    void wordWidth(size_t nBits) {
        ASSERT_require2(nBits > 0 && 0 == (nBits & 7), "word size must be a positive multiple of eight");
        wordWidth_ = nBits;
    }
    /** @} */

    /** Erase all parameters.
     *
     *  Removes all input parameters, output parameters, and object pointer parameter. */
    void clearParameters() {
        clearInputParameters();
        clearOutputParameters();
        thisParameter_ = ParameterLocation();
    }

    /** Property: Enumerated input parameters.
     *
     *  Returns the vector of input (and in-out) parameters that have been enumerated; does not include implied stack
     *  parameters.  This property is read-only; see also @ref appendInputParameter and @ref clearInputParameters. */
    const std::vector<ParameterLocation>& inputParameters() const { return inputParameters_; }

    /** Compute the set of input registers. */
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
    void appendInputParameter(const ParameterLocation&);
    void appendInputParameter(RegisterDescriptor reg) {
        appendInputParameter(ParameterLocation(reg));
    }
    void appendInputParameter(RegisterDescriptor reg, int64_t offset) {
        appendInputParameter(ParameterLocation(reg, offset));
    }
    void appendInputParameter(rose_addr_t va) {
        appendInputParameter(ParameterLocation(va));
    }
    /** @} */

    /** Property: List of output parameters.
     *
     *  Returns the vector of output (and in-out) parameters.  This property is read-only; see also @ref appendOutputParameter
     *  and @ref clearOutputParameters. */
    const std::vector<ParameterLocation>& outputParameters() const { return outputParameters_; }

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
    void appendOutputParameter(const ParameterLocation&);
    void appendOutputParameter(RegisterDescriptor reg) {
        appendOutputParameter(ParameterLocation(reg));
    }
    void appendOutputParameter(RegisterDescriptor reg, int64_t offset) {
        appendOutputParameter(ParameterLocation(reg, offset));
    }
    void appendOutputParameter(rose_addr_t va) {
        appendOutputParameter(ParameterLocation(va));
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
     *  probably ESP, where the base address is the ESP value immediately after the @c call instruction.  This property need
     *  not be defined if implied stack parameters are not possible.
     *
     * @{ */
    const RegisterDescriptor stackPointerRegister() const { return stackPointerRegister_; }
    void stackPointerRegister(RegisterDescriptor r) { stackPointerRegister_ = r; }
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
     *  return address) or parameters.  A value of zero means the alignment is unknown or unspecified.
     *
     * @{ */
    size_t stackAlignment() const { return stackAlignment_; }
    void stackAlignment(size_t nBytes) { stackAlignment_ = nBytes; }
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
    const ParameterLocation& thisParameter() const { return thisParameter_; }
    void thisParameter(const ParameterLocation &x) { thisParameter_ = x; }
    void thisParameter(RegisterDescriptor reg) {
        thisParameter(ParameterLocation(reg));
    }
    void thisParameter(RegisterDescriptor reg, int64_t offset) {
        thisParameter(ParameterLocation(reg, offset));
    }
    void thisParameter(rose_addr_t va) {
        thisParameter(ParameterLocation(va));
    }
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
     *  definition. This feature is mostly for backward compatibility. */
    void print(std::ostream&, const RegisterDictionary *regDict = NULL) const;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Dictionary
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** A ordered collection of calling convention definitions. */
typedef std::vector<Definition::Ptr> Dictionary;

/** Common calling conventions for amd64 (x86-64). */
const Dictionary& dictionaryAmd64();

/** Common calling conventions for ARM. */
const Dictionary& dictionaryArm();

/** Common calling conventions for m68k. */
const Dictionary& dictionaryM68k();

/** Common calling conventions for MIPS. */
const Dictionary& dictionaryMips();

/** Common calling conventions for PowerPC-32. */
const Dictionary& dictionaryPowerpc32();

/** Common calling conventions for PowerPC-64. */
const Dictionary& dictionaryPowerpc64();

/** Common calling conventions for 32-bit x86. */
const Dictionary& dictionaryX86();


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Analysis
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Function calling convention.
 *
 *  This class encapsulates all information about calling conventions including the analysis functions and the data types. */
class Analysis {
private:
    InstructionSemantics2::BaseSemantics::DispatcherPtr cpu_;
    const RegisterDictionary *regDict_;                 // Names for the register parts
    Definition::Ptr defaultCc_;                         // Default calling convention for called functions

    bool hasResults_;                                   // Are the following data members initialized?
    bool didConverge_;                                  // Are the following data members valid (else only approximations)?
    RegisterParts restoredRegisters_;                   // Registers accessed but restored
    RegisterParts inputRegisters_;                      // Registers that serve as possible input parameters
    RegisterParts outputRegisters_;                     // Registers that hold possible return values
    StackVariables inputStackParameters_;               // Stack variables serving as function inputs
    StackVariables outputStackParameters_;              // Stack variables serving as possible return values
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
    Analysis()
        : regDict_(NULL), hasResults_(false), didConverge_(false) {}

    /** Construct an analyzer using a specified disassembler.
     *
     *  This constructor chooses a symbolic domain and a dispatcher appropriate for the disassembler's architecture. */
    explicit Analysis(Disassembler *d)
        : regDict_(NULL), hasResults_(false), didConverge_(false) {
        init(d);
    }

    /** Construct an analysis using a specified dispatcher.
     *
     *  This constructor uses the supplied dispatcher and associated semantic domain. For best results, the semantic domain
     *  should be a symbolic domain that uses @ref InstructionSemantics2::BaseSemantics::MemoryCellList "MemoryCellList" and
     *  @ref InstructionSemantics2::BaseSemantics::RegisterStateGeneric "RegisterStateGeneric". These happen to also be the
     *  defaults used by @ref InstructionSemantics2::SymbolicSemantics. */
    explicit Analysis(const InstructionSemantics2::BaseSemantics::DispatcherPtr &cpu)
        : cpu_(cpu), regDict_(NULL), hasResults_(false), didConverge_(false) {}

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
    void analyzeFunction(const Partitioner2::Partitioner&, const Sawyer::SharedPointer<Partitioner2::Function>&);

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
    const RegisterDictionary* registerDictionary() const { return regDict_; }
    void registerDictionary(const RegisterDictionary *d) { regDict_ = d; }
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
    const StackVariables& inputStackParameters() const { return inputStackParameters_; }

    /** Output stack parameters.
     *
     *  Locations for stack-based parameters that are used as outputs of the function. */
    const StackVariables& outputStackParameters() const { return outputStackParameters_; }

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
    void init(Disassembler*);

    // Recompute the restoredRegisters_ data member.
    void updateRestoredRegisters(const InstructionSemantics2::BaseSemantics::StatePtr &initialState,
                                 const InstructionSemantics2::BaseSemantics::StatePtr &finalState);

    // Recompute the inputRegisters_ data member after updateRestoredRegisters is computed.
    void updateInputRegisters(const InstructionSemantics2::BaseSemantics::StatePtr &state);

    // Recompute the outputRegisters_ data member after updateRestoredRegisters is computed.
    void updateOutputRegisters(const InstructionSemantics2::BaseSemantics::StatePtr &state);

    // Recompute the input and output stack variables
    void updateStackParameters(const InstructionSemantics2::BaseSemantics::StatePtr &initialState,
                               const InstructionSemantics2::BaseSemantics::StatePtr &finalState);

    // Recomputes the stack delta
    void updateStackDelta(const InstructionSemantics2::BaseSemantics::StatePtr &initialState,
                          const InstructionSemantics2::BaseSemantics::StatePtr &finalState);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Free functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream&, const Definition&);
std::ostream& operator<<(std::ostream&, const Analysis&);

} // namespace
} // namespace
} // namespace

#endif
