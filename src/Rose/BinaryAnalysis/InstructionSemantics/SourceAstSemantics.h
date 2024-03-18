// Turn instruction semantics into a C source AST
#ifndef ROSE_BinaryAnalysis_InstructionSemantics_SourceAstSemantics_H
#define ROSE_BinaryAnalysis_InstructionSemantics_SourceAstSemantics_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/BasicTypes.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/NullSemantics.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {

/** Generate C source AST from binary AST.
 *
 *  This semantic domain is used by the @ref Rose::BinaryAnalysis::BinaryToSource "BinaryToSource" analysis to generate
 *  low-level C source code from a binary.  The semantic values of this domain are C expressions as source code strings. When a
 *  RISC operator, such as "add" is invoked on two semantic values, say C expressions "123" and "x", the result is a new value
 *  that holds a larger C expression, such as "(123 + x)".  The concept is quite simple, but in practice this domain needs to
 *  handle three additional things:
 *
 *  @li It must be able to operate on values whose size is other than 8, 16, 32, and 64, the types commonly available in C.
 *  @li It must work around undefined behavior of C's shift operators since shifting in the binary domains is well defined.
 *  @li It must translate the multi-state mode of instruction semantics to a single-state of the C program.
 *
 *  <b>Sizes other than 8, 16, 32, and 64:</b> The semantic values know their exact size in bits and generate C code that uses
 *  the smallest allowable type to represent the value, one of @c uint8_t, @c uint16_t, @c uint32_t, or @c uint64_t.  All
 *  values are unsigned for consistency, and operations such as sign extension are coded explicitly (this is how it happens in
 *  the instruction semantics layers, and the C code is a reflection of those operations).  The generated C code uses masking
 *  (bit-wise AND) to ensure that unused high-order bits of the C value are zero (e.g., when storing a 5-bit value in a uint8_t
 *  the value will be masked with 0x1f).
 *
 *  <b>Undefined behavior of C shift operations:</b> The C language does not define the behavior of shift operators when the
 *  shift count is as wide or wider than the lhs operand. But since the CPU defines these operations, and since one of the
 *  points of this translation is to be able to recompile a binary specimen for a different architecture, the translation needs
 *  to generate well-defined behavior in these cases. Therefore, all shift operations are protected with conditional code in
 *  the C output.
 *
 *  <b>Multi-state vs. single-state:</b> Instruction semantics can operate on multiple machine states at once. For instance, an
 *  x86 PUSH instruction might update the stack pointer register before writing to the stack, but then use the stack pointer
 *  during the write operations. In some other operation it might update the stack pointer but then use the new value. The
 *  generated C program has only a single state object: the register global variables and global memory variable. Therefore,
 *  the generated code performs all calculations up front using static single assignment (SSA) and then generates the side
 *  effects that update the C program state.
 *
 *  For clues about how to use this domain, see @ref Rose::BinaryAnalysis::BinaryToSource. In general, one constructs the
 *  domain and processes one instruction at a time.  For each instruction, the domain's state is reset to an initial value,
 *  then the instruction is processed, then the side effect list is examined to generate the C code for the instruction. */
namespace SourceAstSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Value type
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer for a binary-to-source semantic value. */
typedef Sawyer::SharedPointer<class SValue> SValuePtr;

/** Semantic values for generating C source code ASTs.
 *
 *  Each SValue contains a string representation of a C expression.
 *
 *  Future work is planned to replace the string representation with a C source AST, in which case each SValue will have a
 *  pointer to a partly-constructed AST.  In general, the parent pointers in these AST nodes are not initialized because many
 *  of these nodes are being shared across multiple subtrees.  ROSE's instruction semantics framework relies very heavily on
 *  node sharing, but the AST was not designed for this. Do not expect AST traversals and analysis to work correctly for ASTs
 *  pointed to by an SValue. But never fear, the SValue's AST will be fixed by the time whole function ASTs are generated. */
class SValue: public BaseSemantics::SValue {
public:
    /** Base type. */
    using Super = BaseSemantics::SValue;

    /** Shared-ownership pointer. */
    using Ptr = SValuePtr;

protected:
    static size_t nVariables_;
    std::string ctext_;

protected:
    // An undefined or unspecified value is a C variable that's not initialized.
    explicit SValue(size_t nbits);

    // An integer value, various types depending on width
    SValue(size_t nbits, uint64_t number);

    // Copy constructor deep-copies the AST.
    SValue(const SValue &other);
    
public:
    /** Instantiate a prototypical SValue.
     *
     *  This SValue will be used only for its virtual constructors and will never appear in an expression. */
    static SValuePtr instance();

    /** Instantiate an undefined value.
     *
     *  Undefined values are distinct C variables that are never initialized. */
    static SValuePtr instance_undefined(size_t nbits);

    /** Instantiate an integer constant. */
    static SValuePtr instance_integer(size_t nbits, uint64_t value);

public:
    virtual BaseSemantics::SValuePtr bottom_(size_t nbits) const override;
    virtual BaseSemantics::SValuePtr undefined_(size_t nbits) const override;
    virtual BaseSemantics::SValuePtr unspecified_(size_t nbits) const override;
    virtual BaseSemantics::SValuePtr number_(size_t nbits, uint64_t value) const override;
    virtual BaseSemantics::SValuePtr boolean_(bool value) const override;
    virtual BaseSemantics::SValuePtr copy(size_t new_width=0) const override;

    virtual Sawyer::Optional<BaseSemantics::SValuePtr>
    createOptionalMerge(const BaseSemantics::SValuePtr&, const BaseSemantics::MergerPtr&, const SmtSolverPtr&) const override;

public:
    /** Promote a base instance to an instance of this class. */
    static SValuePtr promote(const BaseSemantics::SValuePtr&);

public:
    virtual bool isBottom() const override;
    virtual void hash(Combinatorics::Hasher&) const override;
    virtual void print(std::ostream&, BaseSemantics::Formatter&) const override;

public:
    // These are not needed since this domain never tries to compare semantic values.
    virtual bool may_equal(const BaseSemantics::SValuePtr &/*other*/,
                           const SmtSolverPtr& = SmtSolverPtr()) const override;

    virtual bool must_equal(const BaseSemantics::SValuePtr &/*other*/,
                            const SmtSolverPtr& = SmtSolverPtr()) const override;

    virtual void set_width(size_t /*nbits*/) override;
    virtual bool is_number() const override;
    virtual uint64_t get_number() const override;

public:
    /** Name of integer type used for value.
     *
     *  @{ */
    static std::string unsignedTypeNameForSize(size_t nbits);
    static std::string signedTypeNameForSize(size_t nbits);
    /** @} */

public:
    /** C source text associated with this semantic value.
     *
     * @{ */
    virtual const std::string& ctext() const;
    virtual void ctext(const std::string&);
    /** @} */
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      State
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// No state is necessary for this domain because all instruction side effects are immediately attached to the AST that's
//  being generated rather than being stored in some state.

typedef BaseSemantics::RegisterStateGeneric RegisterState;      /**< Register state used by this domain. */
typedef BaseSemantics::RegisterStateGenericPtr RegisterStatePtr; /**< Pointer to register states used by this domain. */

typedef NullSemantics::MemoryState MemoryState;         /**< Memory state used by this domain. */
typedef NullSemantics::MemoryStatePtr MemoryStatePtr;   /**< Pointer to memory states used by this domain. */

typedef BaseSemantics::State State;                     /**< State used by this domain. */
typedef BaseSemantics::StatePtr StatePtr;               /**< Pointer to states used by this domain. */


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RiscOperators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer for basic semantic operations. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** Basic semantic operations.
 *
 *  These are the implementations of the RISC operators inherited from the base class, plus some additional functionality
 *  specific to this domain.
 *
 *  The semantic state is split between the @ref BaseSemantics::State object normally attached to semantic domains, and the
 *  side effect list stored in this RiscOperators object.  Neither is complete by itself. */
class RiscOperators: public BaseSemantics::RiscOperators {
public:
    /** Base type. */
    using Super = BaseSemantics::RiscOperators;

    /** Shared-ownership pointer. */
    using Ptr = RiscOperatorsPtr;

public:
    /** Side effect. */
    struct SideEffect {
        BaseSemantics::SValuePtr location;              /**< Optional affected location. */
        BaseSemantics::SValuePtr temporary;             /**< Optional Temporary variable. */
        BaseSemantics::SValuePtr expression;            /**< Expression. */

        ~SideEffect();

        // Default constructor. Not normally used, but needed by <code>std::vector</code>. (DON'T DOCUMENT)
        SideEffect();

        // Used internally, not neede by users since data members are public.
        SideEffect(const BaseSemantics::SValuePtr &location, const BaseSemantics::SValuePtr &temporary,
                   const BaseSemantics::SValuePtr &expression);

        /** Predicate to determine whether side effect is valid. */
        bool isValid() const;

        /** Predicate to determine whether side effect is rather a substitution. Substitutions are side effects in that they
         *  change the value of C variables, but the changes happen in an inner scope and don't affect the C program's main
         *  state. */
        bool isSubstitution() const;
    };

    /** Side effects in the order they occur. */
    typedef std::vector<SideEffect> SideEffects;

private:
    SideEffects sideEffects_;                           // Side effects, including substitutions
    bool executionHalted_;                              // Stop adding inputs and outputs?

protected:
    RiscOperators(const BaseSemantics::SValuePtr &protoval, const SmtSolverPtr&);

    RiscOperators(const BaseSemantics::StatePtr&, const SmtSolverPtr&);

public:
    /** Instantiates a new RiscOperators object and configures it to use semantic values and states that are defaults for
     *  SourceAstSemantics. */
    static RiscOperatorsPtr instanceFromRegisters(const RegisterDictionaryPtr&, const SmtSolverPtr &solver = SmtSolverPtr());

    /** Instantiates a new RiscOperators object with specified prototypical values.  An SMT solver may be specified as the
     *  second argument because the base class expects one, but it is not used for this semantic domain. See @ref solver for
     *  details. */
    static RiscOperatorsPtr instanceFromProtoval(const BaseSemantics::SValuePtr &protoval,
                                                 const SmtSolverPtr &solver = SmtSolverPtr());

    /** Instantiates a new RiscOperators object with specified state.  An SMT solver may be specified as the second argument
     *  because the base class expects one, but it is not used for this semantic domain. See @ref solver for details. */
    static RiscOperatorsPtr instanceFromState(const BaseSemantics::StatePtr&, const SmtSolverPtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const override;

    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr&,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Run-time promotion of a base RiscOperators instance to an instance of this semantic domain's operators. This is a
     *  checked conversion--it will fail if @p x does not point to a SourceAstSemantics::RiscOperators object. */
    static RiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Supporting functions
public:
    /** Create a new SValue. */
    BaseSemantics::SValuePtr makeSValue(size_t nbits, SgNode*, const std::string &ctext = "");

    /** Save a side effect.
     *
     *  A side effect is either a call to a special source function that doesn't return a value, or an assignment to a register
     *  or memory location.  In the latter case (assignment) a new temporary variable is created and returned.  This temporary
     *  can be used instead of the original expression if desired. */
    BaseSemantics::SValuePtr saveSideEffect(const BaseSemantics::SValuePtr &expression,
                                            const BaseSemantics::SValuePtr &location = BaseSemantics::SValuePtr());

    /** Save input value.
     *
     *  This substitutes one value for another. These records are stored as side effects: although they don't change the single
     *  global state of the C program, they do change a local state. These substitutions generally become the static single
     *  assignment (SSA) part of the generated C code. */
    BaseSemantics::SValuePtr substitute(const BaseSemantics::SValuePtr &expression);

    /** Accumulated side effects and substitutions.
     *
     *  Returns the side effects and substitutions in the order they occurred. */
    const SideEffects& sideEffects() const;

    /** Reset state to initial conditions. */
    void resetState();

    /** Global variable name for a register.
     *
     *  No attempt is made to ensure that the register really has a valid global variable. The rule is that if the register
     *  exists as a single location in the register state then it has a global variable. */
    std::string registerVariableName(RegisterDescriptor);

    /** Reset to initial state. */
    void reset();

    /** Halt execution.
     *
     *  This causes all subsequent side effects (but not substitutions) to be discarded. For instance, the x86 HLT instruction
     *  will call this and the effect is that the translation assumes that no side effects generated after the point that this
     *  method is called are ever actually reached. Resetting the state via @ref resetState will also change the execution
     *  state from halted to running. */
    void haltExecution() { executionHalted_ = true; }

    /** Return a bit mask.
     *
     *  The resuling mask has a type that is @p nBits wide, and it has @p nSet bits set and shifted left @p sa.  The @p nSet
     *  and @p sa are optimized for cases of 0 and 1 and generate correct C shift operators when they are too large for the
     *  result type. */
    BaseSemantics::SValuePtr makeMask(size_t nBits, size_t nSet, size_t sa=0);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override all operator methods from base class.  These are the RISC operators that are invoked by a Dispatcher.
public:
    virtual BaseSemantics::SValuePtr unspecified_(size_t nbits) override;
    virtual void hlt() override;
    virtual void cpuid() override;
    virtual BaseSemantics::SValuePtr rdtsc() override;
    virtual BaseSemantics::SValuePtr and_(const BaseSemantics::SValuePtr &a_,
                                          const BaseSemantics::SValuePtr &b_) override;
    virtual BaseSemantics::SValuePtr or_(const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) override;
    virtual BaseSemantics::SValuePtr xor_(const BaseSemantics::SValuePtr &a_,
                                          const BaseSemantics::SValuePtr &b_) override;
    virtual BaseSemantics::SValuePtr invert(const BaseSemantics::SValuePtr &a_) override;
    virtual BaseSemantics::SValuePtr extract(const BaseSemantics::SValuePtr &a_,
                                             size_t begin_bit, size_t end_bit) override;
    virtual BaseSemantics::SValuePtr concat(const BaseSemantics::SValuePtr &a_,
                                            const BaseSemantics::SValuePtr &b_) override;
    virtual BaseSemantics::SValuePtr leastSignificantSetBit(const BaseSemantics::SValuePtr &a_) override;
    virtual BaseSemantics::SValuePtr mostSignificantSetBit(const BaseSemantics::SValuePtr &a_) override;
    virtual BaseSemantics::SValuePtr rotateLeft(const BaseSemantics::SValuePtr &a_,
                                                const BaseSemantics::SValuePtr &sa_) override;
    virtual BaseSemantics::SValuePtr rotateRight(const BaseSemantics::SValuePtr &a_,
                                                 const BaseSemantics::SValuePtr &sa_) override;
    virtual BaseSemantics::SValuePtr shiftLeft(const BaseSemantics::SValuePtr &a_,
                                               const BaseSemantics::SValuePtr &sa_) override;
    virtual BaseSemantics::SValuePtr shiftRight(const BaseSemantics::SValuePtr &a_,
                                                const BaseSemantics::SValuePtr &sa_) override;
    virtual BaseSemantics::SValuePtr shiftRightArithmetic(const BaseSemantics::SValuePtr &a_,
                                                          const BaseSemantics::SValuePtr &sa_) override;
    virtual BaseSemantics::SValuePtr equalToZero(const BaseSemantics::SValuePtr &a_) override;
    virtual BaseSemantics::SValuePtr iteWithStatus(const BaseSemantics::SValuePtr &sel_,
                                                   const BaseSemantics::SValuePtr &a_,
                                                   const BaseSemantics::SValuePtr &b_,
                                                   IteStatus&) override;
    virtual BaseSemantics::SValuePtr unsignedExtend(const BaseSemantics::SValuePtr &a_, size_t new_width) override;
    virtual BaseSemantics::SValuePtr signExtend(const BaseSemantics::SValuePtr &a_, size_t new_width) override;
    virtual BaseSemantics::SValuePtr add(const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) override;
    virtual BaseSemantics::SValuePtr addWithCarries(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_,
                                                    const BaseSemantics::SValuePtr &c_,
                                                    BaseSemantics::SValuePtr &carry_out/*out*/) override;
    virtual BaseSemantics::SValuePtr negate(const BaseSemantics::SValuePtr &a_) override;
    virtual BaseSemantics::SValuePtr signedDivide(const BaseSemantics::SValuePtr &a_,
                                                  const BaseSemantics::SValuePtr &b_) override;
    virtual BaseSemantics::SValuePtr signedModulo(const BaseSemantics::SValuePtr &a_,
                                                  const BaseSemantics::SValuePtr &b_) override;
    virtual BaseSemantics::SValuePtr signedMultiply(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) override;
    virtual BaseSemantics::SValuePtr unsignedDivide(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) override;
    virtual BaseSemantics::SValuePtr unsignedModulo(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) override;
    virtual BaseSemantics::SValuePtr unsignedMultiply(const BaseSemantics::SValuePtr &a_,
                                                      const BaseSemantics::SValuePtr &b_) override;
    virtual void interrupt(int majr, int minr) override;
    virtual BaseSemantics::SValuePtr readRegister(RegisterDescriptor reg,
                                                  const BaseSemantics::SValuePtr &dflt) override;
    virtual BaseSemantics::SValuePtr peekRegister(RegisterDescriptor reg,
                                                  const BaseSemantics::SValuePtr &dflt) override;
    virtual void writeRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &a) override;
    virtual BaseSemantics::SValuePtr readMemory(RegisterDescriptor segreg,
                                                const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt,
                                                const BaseSemantics::SValuePtr &cond) override;
    virtual BaseSemantics::SValuePtr peekMemory(RegisterDescriptor segreg,
                                                const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt) override;
    virtual void writeMemory(RegisterDescriptor segreg,
                             const BaseSemantics::SValuePtr &addr,
                             const BaseSemantics::SValuePtr &data,
                             const BaseSemantics::SValuePtr &cond) override;
};

    
} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
