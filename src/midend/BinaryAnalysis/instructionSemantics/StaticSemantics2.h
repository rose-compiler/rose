// Turn instruction semantics into part of the AST
#ifndef Rose_StaticSemantics2_H
#define Rose_StaticSemantics2_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include "Disassembler.h"
#include "NullSemantics2.h"
#include "SageBuilderAsm.h"

namespace Rose {
namespace BinaryAnalysis {              // documented elsewhere
namespace InstructionSemantics2 {       // documented elsewhere

/** Generate static semantics and attach to the AST.
 *
 *  ROSE normally uses dynamically generate semantics, where the user instantiates a state (where register and memory values
 *  are stored), attaches the state to a semantic domain (risc operators and an associated value type), and the "executes" an
 *  instruction and looks at the effect that occurred on the state.  For instance, if one wants to see whether an instruction
 *  is a RET-like instruction, one constructs an initial state, executes the instruction in the chosen domain (e.g., symbolic),
 *  and then examines the final state. If the final state's instruction pointer register has a value which equal to the value
 *  stored in memory just past (above or below according to stack direction) the stack pointer register, then the instruction
 *  is behaving like an x86 RET instruction.
 *
 *  On the other hand, some people like to work with semantics that are represented statically as part of the AST. In this
 *  case, each instruction has a non-null @ref SgAsmInstruction::get_semantics "semantics" property that returns a list (@ref
 *  SgAsmExprListExp) of semantic expression trees consisting mostly of @ref SgAsmRiscOperation nodes.  The list is in the
 *  order that the instruction's side effects occur.  Many subtrees of the @c semantics property are similar to each other, but
 *  cannot be shared due to the design of ROSE AST (each node has a single parent pointer). Thus the AST with static semantics
 *  attached can be many times larger than when using dynamic semantics.
 *
 *  By default, ROSE does not generate the static semantics, and each instruction's @ref SgAsmInstruction::get_semantics
 *  "semantics" property will be null.  Semantics can be added to any instruction by @ref
 *  BaseSemantics::RiscOperators::processInstruction "executing" the instruction in this StaticSemantics domain.  Each time the
 *  instruction is executed in this domain its previous semantics are thrown away and recalculated, so you should generally
 *  only do this once; that's the nature that makes these semantics "static".  If you want to calculate static semantics for
 *  lots of instructions, which is often the case, the @ref attachInstructionSemantics functions can do that: they process an
 *  entire AST, adding semantics to all the instructions they find. */
namespace StaticSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Free functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Build and attach static semantics to all instructions.
 *
 *  Traverses the specified AST to find instructions that have no static semantics yet. For each such instruction, static
 *  semantics are calculated and the instruction's @ref SgAsmInstruction::get_semantics "semantics" property is set to
 *  non-null and will contain a list of instruction side effects.
 *
 *  If processing lots of unrelated instructions, it is best to use the version of this function that takes a dispatcher, or
 *  virtual CPU, so that the same dispatcher can be used over and over.  There is no register or memory state associated with a
 *  StaticSemantics dispatcher since all side effects are attached to the AST instead of being written to some state.
 *
 * @{ */
void attachInstructionSemantics(SgNode *ast, Disassembler*);
void attachInstructionSemantics(SgNode *ast, const BaseSemantics::DispatcherPtr&);
/** @} */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Value type
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer for a static-semantics value. See @ref heap_object_shared_ownership. */
typedef Sawyer::SharedPointer<class SValue> SValuePtr;

/** Semantic values for generating static semantic ASTs.
 *
 *  Each SValue has a pointer to a partly-constructed AST.  In general, the parent pointers in these AST nodes are not
 *  initialized because many of these nodes are being shared across multiple subtrees.  ROSE's instruction semantics framework
 *  relies very heavily on node sharing, but the AST was not designed for this. Do not expect AST traversals and analysis to
 *  work correctly for ASTs pointed to by an SValue. But never fear, the SValue's AST will be fixed when it's actually attached
 *  to an instruction node. */
class SValue: public BaseSemantics::SValue {
protected:
    SgAsmExpression *ast_;

protected:
    SValue(size_t nbits, SgAsmRiscOperation::RiscOperator op): BaseSemantics::SValue(nbits) {
        static uint64_t nVars = 0;
        ast_ = SageBuilderAsm::buildRiscOperation(op, SageBuilderAsm::buildValueU64(nVars++));
    }

    SValue(size_t nbits, uint64_t number): BaseSemantics::SValue(nbits) {
        SgAsmType *type = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_LSB, nbits,
                                                                           false /*unsigned*/));
        ast_ = SageBuilderAsm::buildValueInteger(number, type);
    }

    SValue(const SValue &other): BaseSemantics::SValue(other) {
        SgTreeCopy deep;
        SgNode *copied = ast_->copy(deep);
        ASSERT_require(isSgAsmExpression(copied));
        ast_ = isSgAsmExpression(copied);
    }
    
public:
    /** Instantiate a prototypical SValue.
     *
     *  This SValue will be used only for its virtual constructors and will never appear in an expression. */
    static SValuePtr instance() {
        return SValuePtr(new SValue(1, SgAsmRiscOperation::OP_undefined));
    }

    /** Instantiate a data-flow bottom value. */
    static SValuePtr instance_bottom(size_t nbits) {
        return SValuePtr(new SValue(nbits, SgAsmRiscOperation::OP_bottom));
    }

    /** Instantiate an undefined value.
     *
     *  Undefined values end up being a SgAsmRiscOperation of type OP_undefined which has a single child which is an integer
     *  identification number.  This allows two such nodes in the AST to be resolved to the same or different undefined value
     *  by virtue of their ID number. */
    static SValuePtr instance_undefined(size_t nbits) {
        return SValuePtr(new SValue(nbits, SgAsmRiscOperation::OP_undefined));
    }

    /** Instantiate an unspecified value.
     *
     *  Unspecified values end up being a SgAsmRiscOperation of type OP_unspecified which has a single child which is an
     *  integer identification number.  This allows two such nodes in the AST to be resolved to the same or different
     *  unspecified value by virtue of their ID number. */
    static SValuePtr instance_unspecified(size_t nbits) {
        return SValuePtr(new SValue(nbits, SgAsmRiscOperation::OP_unspecified));
    }

    /** Instantiate an integer constant. */
    static SValuePtr instance_integer(size_t nbits, uint64_t value) {
        return SValuePtr(new SValue(nbits, value));
    }

public:
    virtual BaseSemantics::SValuePtr bottom_(size_t nbits) const ROSE_OVERRIDE {
        return instance_bottom(nbits);
    }
    virtual BaseSemantics::SValuePtr undefined_(size_t nbits) const ROSE_OVERRIDE {
        return instance_undefined(nbits);
    }
    virtual BaseSemantics::SValuePtr unspecified_(size_t nbits) const ROSE_OVERRIDE {
        return instance_unspecified(nbits);
    }
    virtual BaseSemantics::SValuePtr number_(size_t nbits, uint64_t value) const ROSE_OVERRIDE {
        return instance_integer(nbits, value);
    }
    virtual BaseSemantics::SValuePtr boolean_(bool value) const ROSE_OVERRIDE {
        return instance_integer(1, value ? 1 : 0);
    }
    virtual BaseSemantics::SValuePtr copy(size_t new_width=0) const ROSE_OVERRIDE {
        SValuePtr retval(new SValue(*this));
        if (new_width!=0 && new_width!=retval->get_width())
            retval->set_width(new_width);
        return retval;
    }
    virtual Sawyer::Optional<BaseSemantics::SValuePtr>
    createOptionalMerge(const BaseSemantics::SValuePtr&, const BaseSemantics::MergerPtr&,
                        const SmtSolverPtr&) const ROSE_OVERRIDE {
        throw BaseSemantics::NotImplemented("StaticSemantics is not suitable for dataflow analysis", NULL);
    }

public:
    /** Promote a base value to a static semantics value. */
    static SValuePtr promote(const BaseSemantics::SValuePtr &v) { // hot
        SValuePtr retval = v.dynamicCast<SValue>();
        ASSERT_not_null(retval);
        return retval;
    }

public:
    // These are not needed since this domain never tries to compare semantic values.
    virtual bool may_equal(const BaseSemantics::SValuePtr &other,
                           const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        ASSERT_not_reachable("no implementation necessary");
    }

    virtual bool must_equal(const BaseSemantics::SValuePtr &other,
                            const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        ASSERT_not_reachable("no implementation necessary");
    }
    
    virtual void set_width(size_t nbits) ROSE_OVERRIDE {
        ASSERT_not_reachable("no implementation necessary");
    }

    virtual bool isBottom() const ROSE_OVERRIDE {
        return false;
    }
    
    virtual bool is_number() const ROSE_OVERRIDE {
        return false;
    }

    virtual uint64_t get_number() const ROSE_OVERRIDE {
        ASSERT_not_reachable("no implementation necessary");
    }

    virtual void print(std::ostream&, BaseSemantics::Formatter&) const ROSE_OVERRIDE;

public:
    /** Property: Abstract syntax tree.
     *
     * @{ */
    virtual SgAsmExpression* ast() {
        return ast_;
    }
    virtual void ast(SgAsmExpression *x) {
        ASSERT_not_null(x);
        ASSERT_require(x->get_parent() == NULL);
        ast_ = x;
    }
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

/** Shared-ownership pointer for basic semantic operations. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** Basic semantic operations.
 *
 *  Each operation builds a new AST by creating a new node and attaching existing children to that node.  The parent pointers
 *  of the children are not updated, and as described in @ref SValue, therefore do not form proper ROSE abstract syntax trees.
 *
 *  The @ref writeRegister, @ref writeMemory, and a handful of other operators that represent instruction side effects are
 *  inserted into the instruction's list of static sematics.  This is the point at which this semantic domain takes an
 *  improperly formed ROSE AST from an SValue and fixes it so it has proper parent pointers and does not share nodes with other
 *  subtrees. */
class RiscOperators: public BaseSemantics::RiscOperators {
protected:
    RiscOperators(const BaseSemantics::SValuePtr &protoval, const SmtSolverPtr &solver)
        : BaseSemantics::RiscOperators(protoval, solver) {
        name("StaticSemantics");
        (void) SValue::promote(protoval); // make sure its dynamic type is a StaticSemantics::SValue
    }

    RiscOperators(const BaseSemantics::StatePtr &state, const SmtSolverPtr &solver)
        : BaseSemantics::RiscOperators(state, solver) {
        name("StaticSemantics");
        (void) SValue::promote(state->protoval()); // values must have StaticSemantics::SValue dynamic type
    }

public:
    /** Instantiates a new RiscOperators object and configures it to use semantic values and states that are defaults for
     *  StaticSemantics. */
    static RiscOperatorsPtr instance(const RegisterDictionary *regdict, const SmtSolverPtr &solver = SmtSolverPtr()) {
        BaseSemantics::SValuePtr protoval = SValue::instance();
        BaseSemantics::RegisterStatePtr registers = RegisterState::instance(protoval, regdict);
        BaseSemantics::MemoryStatePtr memory = MemoryState::instance(protoval, protoval);
        BaseSemantics::StatePtr state = State::instance(registers, memory);
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }

    /** Instantiates a new RiscOperators object with specified prototypical values.  An SMT solver may be specified as the
     *  second argument because the base class expects one, but it is not used for static semantics. See @ref solver for
     *  details. */
    static RiscOperatorsPtr instance(const BaseSemantics::SValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr()) {
        return RiscOperatorsPtr(new RiscOperators(protoval, solver));
    }

    /** Instantiates a new RiscOperators object with specified state.  An SMT solver may be specified as the second argument
     *  because the base class expects one, but it is not used for static semantics. See @ref solver for details. */
    static RiscOperatorsPtr instance(const BaseSemantics::StatePtr &state, const SmtSolverPtr &solver = SmtSolverPtr()) {
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        return instance(protoval, solver);
    }

    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr &state,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        return instance(state, solver);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Run-time promotion of a base RiscOperators pointer to static operators. This is a checked conversion--it
     *  will fail if @p x does not point to a StaticSemantics::RiscOperators object. */
    static RiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr &x) {
        RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }

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
    virtual void startInstruction(SgAsmInstruction*) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr filterCallTarget(const BaseSemantics::SValuePtr &a) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr filterReturnTarget(const BaseSemantics::SValuePtr &a) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr filterIndirectJumpTarget(const BaseSemantics::SValuePtr &a) ROSE_OVERRIDE;
    virtual void hlt() ROSE_OVERRIDE;
    virtual void cpuid() ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr rdtsc() ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr and_(const BaseSemantics::SValuePtr &a_,
                                          const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr or_(const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr xor_(const BaseSemantics::SValuePtr &a_,
                                          const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr invert(const BaseSemantics::SValuePtr &a_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr extract(const BaseSemantics::SValuePtr &a_,
                                             size_t begin_bit, size_t end_bit) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr concat(const BaseSemantics::SValuePtr &a_,
                                            const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr leastSignificantSetBit(const BaseSemantics::SValuePtr &a_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr mostSignificantSetBit(const BaseSemantics::SValuePtr &a_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr rotateLeft(const BaseSemantics::SValuePtr &a_,
                                                const BaseSemantics::SValuePtr &sa_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr rotateRight(const BaseSemantics::SValuePtr &a_,
                                                 const BaseSemantics::SValuePtr &sa_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr shiftLeft(const BaseSemantics::SValuePtr &a_,
                                               const BaseSemantics::SValuePtr &sa_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr shiftRight(const BaseSemantics::SValuePtr &a_,
                                                const BaseSemantics::SValuePtr &sa_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr shiftRightArithmetic(const BaseSemantics::SValuePtr &a_,
                                                          const BaseSemantics::SValuePtr &sa_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr equalToZero(const BaseSemantics::SValuePtr &a_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr ite(const BaseSemantics::SValuePtr &sel_,
                                         const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr isEqual(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr isNotEqual(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr isUnsignedLessThan(const BaseSemantics::SValuePtr&,
                                                        const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr isUnsignedLessThanOrEqual(const BaseSemantics::SValuePtr&,
                                                               const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr isUnsignedGreaterThan(const BaseSemantics::SValuePtr&,
                                                           const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr isUnsignedGreaterThanOrEqual(const BaseSemantics::SValuePtr&,
                                                                  const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr isSignedLessThan(const BaseSemantics::SValuePtr&,
                                                      const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr isSignedLessThanOrEqual(const BaseSemantics::SValuePtr&,
                                                             const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr isSignedGreaterThan(const BaseSemantics::SValuePtr&,
                                                         const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr isSignedGreaterThanOrEqual(const BaseSemantics::SValuePtr&,
                                                                const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr unsignedExtend(const BaseSemantics::SValuePtr &a_, size_t new_width) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr signExtend(const BaseSemantics::SValuePtr &a_, size_t new_width) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr add(const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr subtract(const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr addWithCarries(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_,
                                                    const BaseSemantics::SValuePtr &c_,
                                                    BaseSemantics::SValuePtr &carry_out/*out*/) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr negate(const BaseSemantics::SValuePtr &a_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr signedDivide(const BaseSemantics::SValuePtr &a_,
                                                  const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr signedModulo(const BaseSemantics::SValuePtr &a_,
                                                  const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr signedMultiply(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr unsignedDivide(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr unsignedModulo(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr unsignedMultiply(const BaseSemantics::SValuePtr &a_,
                                                      const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual void interrupt(int majr, int minr) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr readRegister(RegisterDescriptor reg,
                                                  const BaseSemantics::SValuePtr &dflt) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr peekRegister(RegisterDescriptor reg,
                                                  const BaseSemantics::SValuePtr &dflt) ROSE_OVERRIDE;
    virtual void writeRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &a) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr readMemory(RegisterDescriptor segreg,
                                                const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt,
                                                const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr peekMemory(RegisterDescriptor segreg,
                                                const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt) ROSE_OVERRIDE;
    virtual void writeMemory(RegisterDescriptor segreg,
                             const BaseSemantics::SValuePtr &addr,
                             const BaseSemantics::SValuePtr &data,
                             const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;
};

    
} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
