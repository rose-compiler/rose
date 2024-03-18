// Turn instruction semantics into part of the AST
#ifndef ROSE_BinaryAnalysis_InstructionSemantics_StaticSemantics_H
#define ROSE_BinaryAnalysis_InstructionSemantics_StaticSemantics_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/BasicTypes.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/NullSemantics.h>

#include <SgAsmRiscOperation.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {

/** Generate static semantics and attach to the AST.
 *
 *  ROSE normally uses dynamically generated semantics, where the user instantiates a state (where register and memory values
 *  are stored), attaches the state to a semantic domain (risc operators and an associated value type), and the "executes" an
 *  instruction and looks at the effect that occurred on the state.  For instance, if one wants to see whether an instruction
 *  is a RET-like instruction, one constructs an initial state, executes the instruction in the chosen domain (e.g., symbolic),
 *  and then examines the final state. If the final state's instruction pointer register has a value which equal to the value
 *  stored in memory just past (above or below according to stack direction) the stack pointer register, then the instruction
 *  is behaving like an x86 RET instruction, at least on architectures that use this calling convention.
 *
 *  On the other hand, some people like to work with semantics that are represented statically as part of the AST. In this case,
 *  each instruction has a non-null @ref SgAsmInstruction::semantics property that returns a list (@ref SgAsmExprListExp) of
 *  semantic expression trees consisting mostly of @ref SgAsmRiscOperation nodes.  The list is in the order that the instruction's
 *  side effects occur.  Many subtrees of the @c semantics property are similar to each other, but cannot be shared due to the
 *  design of ROSE AST (each node has a single parent pointer). Thus the AST with static semantics attached can be many times larger
 *  than when using dynamic semantics.
 *
 *  By default, ROSE does not generate the static semantics, and each instruction's @ref SgAsmInstruction::semantics property will
 *  be null.  Semantics can be added to any instruction by @ref BaseSemantics::Dispatcher::processInstruction "executing" the
 *  instruction in this StaticSemantics domain.  Each time the instruction is executed in this domain its previous semantics are
 *  thrown away and recalculated, so you should generally only do this once; that's the nature that makes these semantics "static".
 *  If you want to calculate static semantics for lots of instructions, which is often the case, the @ref attachInstructionSemantics
 *  functions can do that: they process an entire AST, adding semantics to all the instructions they find. */
namespace StaticSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Free functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Build and attach static semantics to all instructions.
 *
 *  Traverses the specified AST to find instructions that have no static semantics yet. For each such instruction, static semantics
 *  are calculated and the instruction's @ref SgAsmInstruction::semantics property is set to non-null and will contain a list of
 *  instruction side effects.
 *
 *  If processing lots of unrelated instructions, it is best to use the version of this function that takes a dispatcher, or
 *  virtual CPU, so that the same dispatcher can be used over and over.  There is no register or memory state associated with a
 *  StaticSemantics dispatcher since all side effects are attached to the AST instead of being written to some state.
 *
 * @{ */
void attachInstructionSemantics(SgNode *ast, const Architecture::BaseConstPtr&);
void attachInstructionSemantics(SgNode *ast, const BaseSemantics::DispatcherPtr&);
/** @} */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Value type
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer for a static-semantics value. */
typedef Sawyer::SharedPointer<class SValue> SValuePtr;

/** Semantic values for generating static semantic ASTs.
 *
 *  Each SValue has a pointer to a partly-constructed AST.  In general, the parent pointers in these AST nodes are not
 *  initialized because many of these nodes are being shared across multiple subtrees.  ROSE's instruction semantics framework
 *  relies very heavily on node sharing, but the AST was not designed for this. Do not expect AST traversals and analysis to
 *  work correctly for ASTs pointed to by an SValue. But never fear, the SValue's AST will be fixed when it's actually attached
 *  to an instruction node. */
class SValue: public BaseSemantics::SValue {
public:
    /** Base type. */
    using Super = BaseSemantics::SValue;

    /** Shared-ownership pointer. */
    using Ptr = SValuePtr;

protected:
    SgAsmExpression *ast_;

protected:
    SValue(size_t nbits, SgAsmRiscOperation::RiscOperator op);
    SValue(size_t nbits, uint64_t number);
    SValue(const SValue &other);

public:
    /** Instantiate a prototypical SValue.
     *
     *  This SValue will be used only for its virtual constructors and will never appear in an expression. */
    static SValuePtr instance();

    /** Instantiate a data-flow bottom value. */
    static SValuePtr instance_bottom(size_t nbits);

    /** Instantiate an undefined value.
     *
     *  Undefined values end up being a SgAsmRiscOperation of type OP_undefined which has a single child which is an integer
     *  identification number.  This allows two such nodes in the AST to be resolved to the same or different undefined value
     *  by virtue of their ID number. */
    static SValuePtr instance_undefined(size_t nbits);

    /** Instantiate an unspecified value.
     *
     *  Unspecified values end up being a SgAsmRiscOperation of type OP_unspecified which has a single child which is an
     *  integer identification number.  This allows two such nodes in the AST to be resolved to the same or different
     *  unspecified value by virtue of their ID number. */
    static SValuePtr instance_unspecified(size_t nbits);

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
    /** Promote a base value to a static semantics value. */
    static SValuePtr promote(const BaseSemantics::SValuePtr&);

public:
    // These are not needed since this domain never tries to compare semantic values.
    virtual bool may_equal(const BaseSemantics::SValuePtr &/*other*/, const SmtSolverPtr& = SmtSolverPtr()) const override;
    virtual bool must_equal(const BaseSemantics::SValuePtr &/*other*/, const SmtSolverPtr& = SmtSolverPtr()) const override;
    virtual void set_width(size_t /*nbits*/) override;
    virtual bool isBottom() const override;
    virtual bool is_number() const override;
    virtual uint64_t get_number() const override;
    virtual void hash(Combinatorics::Hasher&) const override;
    virtual void print(std::ostream&, BaseSemantics::Formatter&) const override;

public:
    /** Property: Abstract syntax tree.
     *
     * @{ */
    virtual SgAsmExpression* ast();
    virtual void ast(SgAsmExpression*);
    /** @} */
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      State
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// No state necessary for this domain. All instruction side effects are immediately attached to the SgAsmInstruction node
// rather than being stored in some state.

typedef NullSemantics::RegisterState RegisterState;
typedef NullSemantics::RegisterStatePtr RegisterStatePtr;

typedef NullSemantics::MemoryState MemoryState;
typedef NullSemantics::MemoryStatePtr MemoryStatePtr;

typedef NullSemantics::State State;
typedef NullSemantics::StatePtr StatePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RiscOperators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer for basic semantic operations. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** Basic semantic operations.
 *
 *  Each operation builds a new AST by creating a new node and attaching existing children to that node.  The parent pointers
 *  of the children are not updated, and as described in @ref StaticSemantics::SValue, therefore do not form proper ROSE
 *  abstract syntax trees.
 *
 *  The @ref writeRegister, @ref writeMemory, and a handful of other operators that represent instruction side effects are
 *  inserted into the instruction's list of static sematics.  This is the point at which this semantic domain takes an
 *  improperly formed ROSE AST from an SValue and fixes it so it has proper parent pointers and does not share nodes with other
 *  subtrees. */
class RiscOperators: public BaseSemantics::RiscOperators {
public:
    /** Base type. */
    using Super = BaseSemantics::RiscOperators;

    /** Shared-ownership pointer. */
    using Ptr = RiscOperatorsPtr;

protected:
    RiscOperators(const BaseSemantics::SValuePtr &protoval, const SmtSolverPtr&);

    RiscOperators(const BaseSemantics::StatePtr&, const SmtSolverPtr&);

public:
    ~RiscOperators();

    /** Instantiates a new RiscOperators object and configures it to use semantic values and states that are defaults for
     *  StaticSemantics. */
    static RiscOperatorsPtr instanceFromRegisters(const RegisterDictionaryPtr&, const SmtSolverPtr &solver = SmtSolverPtr());

    /** Instantiates a new RiscOperators object with specified prototypical values.  An SMT solver may be specified as the
     *  second argument because the base class expects one, but it is not used for static semantics. See @ref solver for
     *  details. */
    static RiscOperatorsPtr instanceFromProtoval(const BaseSemantics::SValuePtr &protoval,
                                                 const SmtSolverPtr &solver = SmtSolverPtr());

    /** Instantiates a new RiscOperators object with specified state.  An SMT solver may be specified as the second argument
     *  because the base class expects one, but it is not used for static semantics. See @ref solver for details. */
    static RiscOperatorsPtr instanceFromState(const BaseSemantics::StatePtr&, const SmtSolverPtr &solver = SmtSolverPtr());

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const override;

    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr &state,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Run-time promotion of a base RiscOperators pointer to static operators. This is a checked conversion--it
     *  will fail if @p x does not point to a StaticSemantics::RiscOperators object. */
    static RiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Supporting functions
protected:
    /** Create a new SValue.
     *
     *  Nearly all the basic RISC operations will call this function to create a new SValue given a RISC operation enum and
     *  arguments.  The @p nbits is the width of the SValue and is a necessary part of semantic values but will not necessarily
     *  become part of the final AST.
     *
     * @{ */
    BaseSemantics::SValuePtr makeSValue(size_t nbits, SgAsmExpression*);
    BaseSemantics::SValuePtr makeSValue(size_t nbits, SgAsmRiscOperation::RiscOperator);
    BaseSemantics::SValuePtr makeSValue(size_t nbits, SgAsmRiscOperation::RiscOperator,
                                        const BaseSemantics::SValuePtr &a);
    BaseSemantics::SValuePtr makeSValue(size_t nbits, SgAsmRiscOperation::RiscOperator,
                                        const BaseSemantics::SValuePtr &a,
                                        const BaseSemantics::SValuePtr &b);
    BaseSemantics::SValuePtr makeSValue(size_t nbits, SgAsmRiscOperation::RiscOperator,
                                        const BaseSemantics::SValuePtr &a,
                                        const BaseSemantics::SValuePtr &b,
                                        const BaseSemantics::SValuePtr &c);
    BaseSemantics::SValuePtr makeSValue(size_t nbits, SgAsmRiscOperation::RiscOperator,
                                        const BaseSemantics::SValuePtr &a,
                                        const BaseSemantics::SValuePtr &b,
                                        const BaseSemantics::SValuePtr &c,
                                        const BaseSemantics::SValuePtr &d);
    /** @} */

    /** Save instruction side effect.
     *
     *  Any semantic value which is an ultimate side effect of an instruction must get saved as a subtree of the instruction
     *  node in the AST.  This is the point at which the improperly formed SValue AST gets transformed into an AST that follows
     *  ROSE's rules for an AST, namely, a @c parent property that points to the parent in the AST and thus no sharing of nodes
     *  between subtrees. */
    void saveSemanticEffect(const BaseSemantics::SValuePtr&);
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override all operator methods from base class.  These are the RISC operators that are invoked by a Dispatcher.
public:
    virtual void startInstruction(SgAsmInstruction*) override;
    virtual BaseSemantics::SValuePtr filterCallTarget(const BaseSemantics::SValuePtr &a) override;
    virtual BaseSemantics::SValuePtr filterReturnTarget(const BaseSemantics::SValuePtr &a) override;
    virtual BaseSemantics::SValuePtr filterIndirectJumpTarget(const BaseSemantics::SValuePtr &a) override;
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
    virtual BaseSemantics::SValuePtr isEqual(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr isNotEqual(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr isUnsignedLessThan(const BaseSemantics::SValuePtr&,
                                                        const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr isUnsignedLessThanOrEqual(const BaseSemantics::SValuePtr&,
                                                               const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr isUnsignedGreaterThan(const BaseSemantics::SValuePtr&,
                                                           const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr isUnsignedGreaterThanOrEqual(const BaseSemantics::SValuePtr&,
                                                                  const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr isSignedLessThan(const BaseSemantics::SValuePtr&,
                                                      const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr isSignedLessThanOrEqual(const BaseSemantics::SValuePtr&,
                                                             const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr isSignedGreaterThan(const BaseSemantics::SValuePtr&,
                                                         const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr isSignedGreaterThanOrEqual(const BaseSemantics::SValuePtr&,
                                                                const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr unsignedExtend(const BaseSemantics::SValuePtr &a_, size_t new_width) override;
    virtual BaseSemantics::SValuePtr signExtend(const BaseSemantics::SValuePtr &a_, size_t new_width) override;
    virtual BaseSemantics::SValuePtr add(const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) override;
    virtual BaseSemantics::SValuePtr subtract(const BaseSemantics::SValuePtr &a_,
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
