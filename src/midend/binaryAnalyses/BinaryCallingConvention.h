#ifndef ROSE_BinaryAnalysis_CallingConvention_H
#define ROSE_BinaryAnalysis_CallingConvention_H

#include <BaseSemantics2.h>
#include <Partitioner2/Partitioner.h>

namespace rose {
namespace BinaryAnalysis {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      CallingConvention
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Information about calling conventions. */
class CallingConvention {
public:
    /** Abstract parameter location.
     *
     *  Describes a parameter or return value location, which can be a register, a memory location at a constant address (e.g.,
     *  global variable), or a memory location relative to some register (e.g., on a stack).  Location descriptors are
     *  immutable.
     *
     *  The same type is used for input parameters, output parameters, and in-out parameters. Return values are a kind of
     *  output parameter, although the API usually does not include the return value when it talks about "parameters". */
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
    public:
        /** Default constructed no-location.
         *
         *  This default constructor is useful for indicating no location or for using an STL container, such as @c
         *  std::vector, that requires a default constructor.  The @ref isValid predicate will return false for
         *  default-constructed locations. */
        ParameterLocation()
            : type_(NO_LOCATION), offset_(0) {}

        /** Constructs a parameter in a register location. */
        explicit ParameterLocation(const RegisterDescriptor &reg)
            : type_(REGISTER), reg_(reg), offset_(0) {}

        /** Constructs a parameter at a register-relative memory address. */
        ParameterLocation(const RegisterDescriptor &reg, int64_t offset)
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
        const RegisterDescriptor& reg() const {
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

    /** The order that arguments are pushed onto the stack. */
    enum StackParameterOrder {
        LEFT_TO_RIGHT,                                  /**< Stack parameters pushed left to right (Pascal order). */
        RIGHT_TO_LEFT,                                  /**< Stack parameters pushed right to left (C order). */
        ORDER_UNSPECIFIED,                              /**< Stack parameter order is unknown or unspecified. */
    };

    /** The direction in which the stack grows. */
    enum StackDirection {
        GROWS_UP,                                       /**< A push increments the stack pointer. */
        GROWS_DOWN,                                     /**< A push decrements the stack pointer. */
    };

    /** Who is responsible for popping stack parameters. */
    enum StackCleanup {
        CLEANUP_CALLER,                                 /**< The caller pops all stack parameters. */
        CLEANUP_CALLEE,                                 /**< The called function pops all stack parameters. */
        CLEANUP_UNSPECIFIED,                            /**< Stack parameter cleanup is unknown or unspecified. */
    };

private:
    std::string name_;                                  // Official short name of the convention, like "stdcall".
    std::string comment_;                               // Long name, like "Windows Borland x86-32 fastcall"
    size_t wordSize_;                                   // Natural word size in bits
    std::vector<ParameterLocation> inputParameters_;    // Input (inc. in-out) parameters; additional stack-based are implied
    std::vector<ParameterLocation> outputParameters_;   // Return values and output parameters.
    StackParameterOrder stackParameterOrder_;           // Order of arguments on the stack
    RegisterDescriptor stackPointerRegister_;           // Base pointer for implied stack parameters
    size_t nonParameterStackSize_;                      // Size in bits of non-parameter stack area
    StackDirection stackDirection_;                     // Direction that stack grows from a PUSH operation
    StackCleanup stackCleanup_;                         // Who cleans up stack parameters?
    ParameterLocation thisParameter_;                   // Object pointer for calling conventions that are object methods
    std::set<RegisterDescriptor> calleeSavedRegisters_; // Register that the callee must restore before returning

public:
    /** Initialize diagnostics.
     *
     *  This is normally called as part of ROSE's diagnostics initialization, but it doesn't hurt to call it often. */
    static void initDiagnostics();

    /** Facility for diagnostic output.
     *
     *  The facility can be controlled directly or via ROSE's command-line. */
    static Sawyer::Message::Facility mlog;

public:
    /** Default constructor.
     *
     *  Constructs a new calling convention with no name or parameters. */
    CallingConvention()
        : wordSize_(0), stackDirection_(GROWS_DOWN), stackCleanup_(CLEANUP_UNSPECIFIED) {}

    /** Construct a new calling convention.
     *
     *  The name of the calling convention usually comes from the documentation (see @name) and is a single word. The comment
     *  is a more complete name for the convention perhaps including the operating system and architecture but not containing
     *  line termination. */
    CallingConvention(size_t wordSize, const std::string &name, const std::string &comment)
        : name_(name), comment_(comment), wordSize_(wordSize), stackDirection_(GROWS_DOWN), stackCleanup_(CLEANUP_UNSPECIFIED) {
        ASSERT_require2(0 == (wordSize & 7) && wordSize > 0, "word size must be a positive multiple of eight");
    }

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
     *  This is the natural size of a word measured in bits. When searching for a matching calling convention only those
     *  calling conventions having the desired word size are considered.
     *
     * @{ */
    size_t wordSize() const { return wordSize_; }
    void wordSize(size_t nBits) {
        ASSERT_require2(nBits > 0 && 0 == (nBits & 7), "word size must be a positive multiple of eight");
        wordSize_ = nBits;
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
    void appendInputParameter(const RegisterDescriptor &reg) {
        appendInputParameter(ParameterLocation(reg));
    }
    void appendInputParameter(const RegisterDescriptor &reg, int64_t offset) {
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
    void appendOutputParameter(const RegisterDescriptor &reg) {
        appendOutputParameter(ParameterLocation(reg));
    }
    void appendOutputParameter(const RegisterDescriptor &reg, int64_t offset) {
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
    void stackPointerRegister(const RegisterDescriptor &r) { stackPointerRegister_ = r; }
    /** @} */

    /** Property: Size of non-parameter stack area.
     *
     *  This is the size in bits of the final non-parameter information pushed onto the stack by the function call
     *  instruction. For instance, the 32-bit x86 @c call instruction pushes a 32-bit return address, but some architectures
     *  use a link register instead.
     *
     * @{ */
    size_t nonParameterStackSize() const {
        return nonParameterStackSize_;
    }
    void nonParameterStackSize(size_t nBits) {
        ASSERT_require2(0 == (nBits & 7), "must be divisible by eight");
        nonParameterStackSize_ = nBits;
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
    void thisParameter(const RegisterDescriptor &reg) {
        thisParameter(ParameterLocation(reg));
    }
    void thisParameter(const RegisterDescriptor &reg, int64_t offset) {
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

    /** Print detailed information about this calling convention. */
    void print(std::ostream&, const RegisterDictionary*) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Deprecated API
    //
    // This is mostly here for backward compatibility with the original API design, which was then implemented in SEI's pharos
    // library.  Unless otherwise noted, these were all deprecated as of 2015-08-14 in favor of CamelCase and dropping the
    // "get" and "set" from properties that have names that are nouns.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    size_t get_word_size() const ROSE_DEPRECATED("use wordSize property instead") {
        return wordSize();
    }
    void set_word_size(size_t w) ROSE_DEPRECATED("use wordSize property instead") {
        wordSize(w);
    }

    const std::string& get_name() const ROSE_DEPRECATED("use \"name\" property instead") {
        return name();
    }
    void set_name(const std::string &n) ROSE_DEPRECATED("use \"name\" property instead") {
        name(n);
    }

    // The comment property stores a full name without any particular format. It may or may not include the compiler name
    // depending on whether the compiler name is significant.  If it includes a compiler name, it may or may not include a
    // compiler version.
    const std::string &get_compiler() const ROSE_DEPRECATED("use \"comment\" property instead") {
        return comment();
    }
    void set_compiler(const std::string &c) ROSE_DEPRECATED("use \"comment\" property instead") {
        comment(c);
    }

    const std::string &get_comment() const ROSE_DEPRECATED("use \"comment\" property instead") {
        return comment();
    }
    void set_comment(const std::string &c) ROSE_DEPRECATED("use \"comment\" property instead") {
        comment(c);
    }

    enum ParameterOrder {/*NO_STRINGIFY*/
        ORDER_LTR       = LEFT_TO_RIGHT,
        ORDER_RTL       = RIGHT_TO_LEFT,
        ORDER_UNKNOWN   = ORDER_UNSPECIFIED,
    };

    ParameterOrder get_param_order() const ROSE_DEPRECATED("use stackParameterOrder property instead") {
        return (ParameterOrder)stackParameterOrder();
    }
    void set_param_order(ParameterOrder order) ROSE_DEPRECATED("use stackParameterOrder property instead") {
        stackParameterOrder((StackParameterOrder)order);
    }

    enum ThisPointerLocation {/*NO_STRINGIFY*/
        THIS_FIRST_PARAM,
        THIS_REGISTER,
        THIS_NOT_APPLICABLE,
        THIS_UNKNOWN,
    };

    ThisPointerLocation get_this_location() const ROSE_DEPRECATED("use thisParameter property instead") {
        const ParameterLocation &loc = thisParameter();
        switch (loc.type()) {
            case ParameterLocation::NO_LOCATION: return THIS_NOT_APPLICABLE; // or maybe THIS_UNKNOWN
            case ParameterLocation::REGISTER: return THIS_REGISTER;
            case ParameterLocation::STACK: return THIS_FIRST_PARAM; // assume its the first parameter
            case ParameterLocation::ABSOLUTE: return THIS_UNKNOWN; // not supported in old API
        }
    }
    // We can't set the location type independent of the location.
    //void set_this_location(ThisPointerLocation loc) ROSE_DEPRECATED("use thisParameter property instead") {

    const RegisterDescriptor* get_this_register() const ROSE_DEPRECATED("use thisParameter property instead") {
        if (thisParameter().type() != ParameterLocation::REGISTER)
            return NULL;
        return &thisParameter().reg();
    }
    void set_this_register(const RegisterDescriptor *reg) ROSE_DEPRECATED("use thisParameter property instead") {
        if (NULL == reg) {
            thisParameter(ParameterLocation());         // no location
        } else {
            thisParameter(*reg);
        }
    }

    enum ReturnValueLocation {/*NO_STRINGIFY*/
        RETVAL_STACK,
        RETVAL_REGISTER,
        RETVAL_NOT_APPLICABLE,
        RETVAL_UNKNOWN,
    };

    ReturnValueLocation get_retval_location() const ROSE_DEPRECATED("use outputParameters property instead") {
        if (outputParameters().empty())
            return RETVAL_NOT_APPLICABLE;
        const ParameterLocation &loc = outputParameters().front();
        switch (loc.type()) {
            case ParameterLocation::NO_LOCATION: return RETVAL_UNKNOWN;
            case ParameterLocation::REGISTER: return RETVAL_REGISTER;
            case ParameterLocation::STACK: return RETVAL_STACK;
            case ParameterLocation::ABSOLUTE: return RETVAL_UNKNOWN; // not supported in old API
        }
        ASSERT_not_reachable("invalid return value location");
    }
    // We can't set the return value location type independent of the location.
    // void set_retval_location(ReturnValueLocation loc) ROSE_DEPRECATED("use appendOutputParameter instead");

    const RegisterDescriptor* get_retval_register() const ROSE_DEPRECATED("use outputParameters property instead") {
        if (outputParameters().empty())
            return NULL;
        if (outputParameters().front().type() != ParameterLocation::REGISTER)
            return NULL;
        return &outputParameters().front().reg();
    }
    void set_retval_register(const RegisterDescriptor *reg) ROSE_DEPRECATED("use appendOutputParameter instead") {
        clearOutputParameters();
        if (NULL != reg)
            appendOutputParameter(*reg);
    }

    StackCleanup get_stack_cleanup() const ROSE_DEPRECATED("use stackCleanup property instead") {
        return stackCleanup();
    }
    void set_stack_cleanup(StackCleanup cleanup) ROSE_DEPRECATED("use stackCleanup property instead") {
        stackCleanup(cleanup);
    }

    // These aren't needed for disassembly, only for compilers.
    //size_t get_stack_alignment() const { return stack_alignment; }
    //void set_stack_alignment(size_t alignment) { stack_alignment = alignment; }

    typedef std::vector<RegisterDescriptor> RegisterVector;

    // We can't return a reference like the original API because this object doesn't keep a vector of just the register
    // parameters.
    RegisterVector get_reg_params() const ROSE_DEPRECATED("use inputParameters property instead") {
        RegisterVector retval;
        BOOST_FOREACH (const ParameterLocation &loc, inputParameters_) {
            if (loc.type() == ParameterLocation::REGISTER)
                retval.push_back(loc.reg());
        }
        return retval;
    }
    void add_reg_param(const RegisterDescriptor *reg) ROSE_DEPRECATED("use appendInputParameter instead") {
        appendInputParameter(reg ? *reg : RegisterDescriptor());
    }

    typedef std::set<RegisterDescriptor> RegisterSet;

    const RegisterSet& get_nonvolatile() const ROSE_DEPRECATED("use calleeSavedRegisters property instead") {
        return calleeSavedRegisters();
    }
    void add_nonvolatile(const RegisterDictionary *dict, const std::string &name)
        ROSE_DEPRECATED("use calleeSavedRegisters property instead") {
        const RegisterDescriptor *rd = dict->lookup(name);
        ASSERT_not_null2(rd, "unable to find register \"" + StringUtility::cEscape(name) + "\"");
        calleeSavedRegisters().insert(*rd);
    }
    void add_nonvolatile(const RegisterDescriptor *rd) ROSE_DEPRECATED("use calleeSavedRegisters property instead") {
        ASSERT_require2(rd && rd->is_valid(), "invalid nonvolatile register");
        calleeSavedRegisters().insert(*rd);
    }
    void add_nonvolatile(const RegisterSet &regs) ROSE_DEPRECATED("use calleeSavedRegisters property instead") {
        calleeSavedRegisters().insert(regs.begin(), regs.end());
    }

    // Write information about this calling convention to the debug log stream.
    void report(const RegisterDictionary *regdict) const ROSE_DEPRECATED("use \"print\" instead") {
        print(std::cout, regdict);
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      CallingConventionAnalysis
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Function calling convention.
 *
 *  This class encapsulates all information about calling conventions including the analysis functions and the data types. */
class CallingConventionAnalysis {
public:

    // Location of an argument or return value
    class ValueLocation {
        RegisterDescriptor reg_;                        // Argument is passed in a register.
        Sawyer::Optional<rose_addr_t> addr_;            // Argument is in a global variable.
        Sawyer::Optional<int64_t> spOffset_;            // Argument is on the stack.
        size_t nBits_;                                  // Size of argument

    public:
        static ValueLocation inRegister(const RegisterDescriptor&);
        static ValueLocation inGlobal(rose_addr_t address, size_t nBits);
        static ValueLocation inStack(int64_t offset, size_t nBits);

        bool isInRegister() const;
        bool isInGlobal() const;
        bool isInStack() const;

        const RegisterDescriptor& registerDescriptor() const;
        rose_addr_t globalAddress() const;
        int64_t stackOffset() const;
        size_t nBits() const;
    };

private:
    std::vector<ValueLocation> argLocs_;                // Locations for each argument.
    bool hasVarArgs_;                                   // Variable number of arguments (argLocs_ are the required args).
    std::set<RegisterDescriptor> modifiedRegisters_;    // Registers modified by this function.
    std::set<RegisterDescriptor> restoredRegisters_;    // Registers saved and restored by this function.
    Sawyer::Optional<int64_t> stackDelta_;              // Stack delta (bytes) for this function inc. return address.
    std::set<ValueLocation> returnValues_;              // Location of return values.
    InstructionSemantics2::BaseSemantics::DispatcherPtr cpu_;

public:
    CallingConventionAnalysis(const InstructionSemantics2::BaseSemantics::DispatcherPtr &cpu)
        : cpu_(cpu) {}

    CallingConventionAnalysis(Disassembler *d) {
        init(d);
    }

    void analyzeFunction(const Partitioner2::Partitioner&, const Partitioner2::Function::Ptr&);

    typedef Sawyer::Container::Set<RegisterDescriptor> RegisterSet;

    RegisterSet registersRead(const InstructionSemantics2::BaseSemantics::StatePtr&);
    RegisterSet registersWritten(const InstructionSemantics2::BaseSemantics::StatePtr&);
    RegisterSet registersReadWritten(const InstructionSemantics2::BaseSemantics::StatePtr&);

    RegisterSet registersReadWithoutWrite(const InstructionSemantics2::BaseSemantics::StatePtr&);
    RegisterSet registersWriteWithoutRead(const InstructionSemantics2::BaseSemantics::StatePtr&);

    RegisterSet registersCalleeSaved(const InstructionSemantics2::BaseSemantics::DispatcherPtr &cpu,
                                     const InstructionSemantics2::BaseSemantics::StatePtr &initState,
                                     const InstructionSemantics2::BaseSemantics::StatePtr &finalState,
                                     const RegisterSet &registers);
    
private:
    void init(Disassembler*);
};

} // namespace
} // namespace

#endif
