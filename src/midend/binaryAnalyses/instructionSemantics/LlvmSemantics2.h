#ifndef Rose_LlvmSemantics2_H
#define Rose_LlvmSemantics2_H

#include "SymbolicSemantics2.h"
#include "DispatcherX86.h"

namespace BinaryAnalysis {
namespace InstructionSemantics2 {

/** A semantic domain to generate LLVM. */
namespace LlvmSemantics {

typedef std::vector<RegisterDescriptor> RegisterDescriptors;

typedef InsnSemanticsExpr::LeafNode LeafNode;
typedef InsnSemanticsExpr::LeafNodePtr LeafNodePtr;
typedef InsnSemanticsExpr::InternalNode InternalNode;
typedef InsnSemanticsExpr::InternalNodePtr InternalNodePtr;
typedef InsnSemanticsExpr::TreeNodePtr TreeNodePtr;
typedef InsnSemanticsExpr::TreeNodes TreeNodes;

typedef SymbolicSemantics::SValuePtr SValuePtr;
typedef SymbolicSemantics::SValue SValue;

typedef BaseSemantics::RegisterStateGenericPtr RegisterStatePtr;
typedef BaseSemantics::RegisterStateGeneric RegisterState;

typedef SymbolicSemantics::MemoryStatePtr MemoryStatePtr;
typedef SymbolicSemantics::MemoryState MemoryState;

typedef BaseSemantics::StatePtr StatePtr;
typedef BaseSemantics::State State;

typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

class RiscOperators: public SymbolicSemantics::RiscOperators {
private:
    typedef Map<uint64_t /*hash*/, LeafNodePtr /*term*/> Rewrites;
    Rewrites rewrites;                                  // maps expressions to LLVM variables
    RegisterStatePtr prev_regstate;                     // most recently emitted register state
    RegisterDescriptors important_registers;            // registers that should be emitted to LLVM
    InsnSemanticsExpr::TreeNodes  mem_writes;           // memory write operations (OP_WRITE expressions)

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit RiscOperators(const BaseSemantics::SValuePtr &protoval, SMTSolver *solver=NULL)
        : SymbolicSemantics::RiscOperators(protoval, solver) {
        set_name("Llvm");
    }

    explicit RiscOperators(const BaseSemantics::StatePtr &state, SMTSolver *solver=NULL)
        : SymbolicSemantics::RiscOperators(state, solver) {
        set_name("Llvm");
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiates a new RiscOperators object and configures it to use semantic values and states that are defaults for
     *  LlvmSemantics. */
    static RiscOperatorsPtr instance(const RegisterDictionary *regdict, SMTSolver *solver=NULL) {
        BaseSemantics::SValuePtr protoval = SValue::instance();
        BaseSemantics::RegisterStatePtr registers = RegisterState::instance(protoval, regdict);
        BaseSemantics::MemoryStatePtr memory = MemoryState::instance(protoval);
        BaseSemantics::StatePtr state = State::instance(registers, memory);
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }

    /** Instantiates a new RiscOperators object with specified prototypical values. */
    static RiscOperatorsPtr instance(const BaseSemantics::SValuePtr &protoval, SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(protoval, solver));
    }

    /** Instantiates a new RiscOperators object with specified state. */
    static RiscOperatorsPtr instance(const BaseSemantics::StatePtr &state, SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   SMTSolver *solver=NULL) const /*override*/ {
        return instance(protoval, solver);
    }

    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr &state,
                                                   SMTSolver *solver=NULL) const /*override*/ {
        return instance(state, solver);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer cases
public:
    /** Run-time promotion of a base RiscOperators pointer to Llvm operators. This is a checked-converstion--it will fail
     *  if @p x does not point to a LlvmSemantics::RiscOperators object. */
    static RiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr &x) {
        RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
        assert(retval!=NULL);
        return retval;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods we override from the super class
public:
    virtual BaseSemantics::SValuePtr readMemory(const RegisterDescriptor &segreg, const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &cond, size_t nbits) /*override*/;
    virtual void writeMemory(const RegisterDescriptor &segreg, const BaseSemantics::SValuePtr &addr,
                             const BaseSemantics::SValuePtr &data, const BaseSemantics::SValuePtr &cond) /*override*/;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // New methods to control/query the machine state
public:
    /** Reset to initial state. */
    virtual void reset();

    /** Return the list of registers that needs to be emitted to LLVM.  Regardless of what registers are written (e.g., AX),
     *  return a list of registers that we're keeping track of in global variables (e.g., EAX). */
    virtual const RegisterDescriptors& get_important_registers();

    /** Return the list of important registers that are stored.  This is a subset of the value returned by
     *  get_important_registers(). */
    virtual RegisterDescriptors get_stored_registers();

    /** Return the list of important registers that have been modified since the last call to make_current(). */
    virtual RegisterDescriptors get_modified_registers();

    /** Return the value of the instruction pointer. */
    virtual SValuePtr get_instruction_pointer();

    /** Return the list of memory writes that have occured since the last call to make_current(). Each item in the list is an
     *  OP_WRITE symbolic expression, and the list is in the order the write occurred (oldest to most recent).  The memory
     *  states are each  unique and not used for anything in particular. */
    virtual const InsnSemanticsExpr::TreeNodes& get_memory_writes() { return mem_writes; }

    /** Mark the current state as having been emitted. */
    virtual void make_current();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // New methods to emit the machine state
public:
    /** Output LLVM global register declarations for the specified registers. */
    virtual void emit_register_declarations(std::ostream&, const RegisterDescriptors&);

    /** Output LLVM global register definitions for the specified registers. */
    virtual void emit_register_definitions(std::ostream&, const RegisterDescriptors&);

    /** Output LLVM global variable reads that are needed to define the specified registers.  Since registers are stored in
     *  global variables and we routinely emit more than one register definition at a time, we need to first make sure that any
     *  global prerequisites for the definitions are saved in temporaries.  This is to handle cases like the following, where
     *  the values of @p eax and @p ebx are swapped using @p edx as a temporary:
     *
     * @code
     *  mov edx, eax
     *  mov eax, ebx
     *  mov ebx, edx
     * @endcode
     *
     *  If we emit all the register definitions at the end of these three x86 instructions, we should get LLVM output similar
     *  to the following:
     *
     * @code
     *  %1 = @eax
     *  %2 = @ebx
     *  @eax = %2
     *  @ebx = %1
     *  @edx = %1
     * @endcode
     *
     *  The dictionary is used to convert register names (that appear in symbolic variable comments, like "eax_0") to register
     *  descriptors.  If a register reference (like "eax_0") corresponds to a register that we're not interested in, then don't
     *  bother emitting a temporary variable for the register.
     */
    virtual void emit_prerequisites(std::ostream&, const RegisterDescriptors&, const RegisterDictionary*);

    /** Output an LLVM branch instruction. */
    virtual void emit_next_eip(std::ostream&);

    /** Output changed memory state. */
    virtual void emit_memory_writes(std::ostream&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // New methods to return snippets of LLVM as strings or expressions
public:
    /** Obtain the LLVM type name for an integer. */
    virtual std::string llvm_integer_type(size_t nbits);

    /** Convert a variable or integer to an LLVM term. */
    virtual std::string llvm_term(const TreeNodePtr&);

    /** Create a temporary variable. */
    virtual LeafNodePtr next_temporary(size_t nbits);

    /** Obtain the name for an LLVM label. */
    virtual std::string next_label();

    /** Obtain a label for a virtual address. */
    virtual std::string addr_label(rose_addr_t);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // New methods to emit LLVM code for an expression.
public:
    /** Emit LLVM statements for an expression. This method transcodes an expression to LLVM and returns an LLVM terminal node
     * (a constant or variable).  The terminal node is not part of the output.  For instance, an expression like:
     *
     * @code
     *  (add[32] v1[32] v2[32] (negate[32] v3[32]))
     * @endcode
     *
     *  Would produce the following LLVM
     *
     * @code
     *  %4 = add i32 %1, i32 %2
     *  %5 = sub i32 0, i32 %3
     *  %6 = add i32 %4, i32 %5
     * @endcode
     *
     * and returns the symbolic variable "v6".
     * @{ */
    virtual LeafNodePtr emit_expression(std::ostream&, const SValuePtr&);
    virtual LeafNodePtr emit_expression(std::ostream&, const TreeNodePtr&);
    /** @} */

protected:
    /** Emit an assignment and add a rewrite rule.  The left hand side is a new LLVM temporary variable (which is returned) and
     *  the assignment is emitted even if the right hand side is also a variable.  A rewrite rule is added so that future
     *  appearances of the right hand side will be replaced by the left hand side in calls to emit_expression(). */
    virtual LeafNodePtr emit_assignment(std::ostream&, const TreeNodePtr &rhs);

    /** Emit an operation as LLVM instructions.  These "emit" methods take operands that are symbolic expressions and output
     *  LLVM instructions that implement the expression.  The return value is either an LLVM term (variable or integer) or
     *  an expression.  Expressions are only returned in cases where the operation is a no-op, such as for the symbolic
     *  expression "(add (negate v3))" where the "add" is a no-op that simply returns its only argument.  Because these "emit"
     *  methods might return an expression, it is customary to call emit_expression() on their return value.
     *  @{ */
    virtual TreeNodePtr emit_zero_extend(std::ostream&, const TreeNodePtr &value, size_t nbits);
    virtual TreeNodePtr emit_sign_extend(std::ostream&, const TreeNodePtr &value, size_t nbits);
    virtual TreeNodePtr emit_truncate(std::ostream&, const TreeNodePtr &value, size_t nbits);
    virtual TreeNodePtr emit_unsigned_resize(std::ostream&, const TreeNodePtr &value, size_t nbits);
    virtual TreeNodePtr emit_binary(std::ostream&, const std::string &llvm_op, const TreeNodePtr&, const TreeNodePtr&);
    virtual TreeNodePtr emit_signed_binary(std::ostream&, const std::string &llvm_op, const TreeNodePtr&, const TreeNodePtr&);
    virtual TreeNodePtr emit_unsigned_binary(std::ostream&, const std::string &llvm_op, const TreeNodePtr&, const TreeNodePtr&);
    virtual TreeNodePtr emit_logical_right_shift(std::ostream&, const TreeNodePtr &value, const TreeNodePtr &amount);
    virtual TreeNodePtr emit_logical_right_shift_ones(std::ostream&, const TreeNodePtr &value, const TreeNodePtr &amount);
    virtual TreeNodePtr emit_arithmetic_right_shift(std::ostream&, const TreeNodePtr &value, const TreeNodePtr &amount);
    virtual TreeNodePtr emit_left_shift(std::ostream&, const TreeNodePtr &value, const TreeNodePtr &amount);
    virtual TreeNodePtr emit_left_shift_ones(std::ostream&, const TreeNodePtr &value, const TreeNodePtr &amount);
    virtual TreeNodePtr emit_extract(std::ostream&, const TreeNodePtr &value, const TreeNodePtr &from, size_t result_nbits);
    virtual TreeNodePtr emit_invert(std::ostream&, const TreeNodePtr &value);
    virtual TreeNodePtr emit_left_associative(std::ostream&, const std::string &llvm_op, const TreeNodes &operands);
    virtual TreeNodePtr emit_concat(std::ostream&, TreeNodes operands);
    virtual TreeNodePtr emit_divide(std::ostream&, const std::string llvm_op, const TreeNodePtr&, const TreeNodePtr&);
    virtual TreeNodePtr emit_signed_modulo(std::ostream&, const TreeNodePtr&, const TreeNodePtr&);
    virtual TreeNodePtr emit_unsigned_modulo(std::ostream&, const TreeNodePtr&, const TreeNodePtr&);
    virtual TreeNodePtr emit_signed_multiply(std::ostream&, const TreeNodes &operands);
    virtual TreeNodePtr emit_unsigned_multiply(std::ostream&, const TreeNodes &operands);
    virtual TreeNodePtr emit_memory_read(std::ostream&, const TreeNodePtr &address, size_t nbits);
    virtual TreeNodePtr emit_ite(std::ostream&, const TreeNodePtr &cond, const TreeNodePtr&, const TreeNodePtr&);
    /** @} */
};

typedef boost::shared_ptr<class Transcoder> TranscoderPtr;

/** Translates machine instructions to LLVM. */
class Transcoder {
private:
    RiscOperatorsPtr operators;
    BaseSemantics::DispatcherPtr dispatcher;

protected:
    explicit Transcoder(const BaseSemantics::DispatcherPtr &dispatcher): dispatcher(dispatcher) {
        operators = RiscOperators::promote(dispatcher->get_operators());
    }

public:
    /** Factory method to create a new transcoder for an arbitrary machine architecture. The supplied dispatcher must use
     *  an LlvmSemantics::RiscOperators or subclass thereof. */
    static TranscoderPtr instance(const BaseSemantics::DispatcherPtr &dispatcher) {
        return TranscoderPtr(new Transcoder(dispatcher));
    }

    /** Factory method to create a new transcoder for 32-bit X86 instructions. */
    static TranscoderPtr instanceX86() {
        const RegisterDictionary *regdict = RegisterDictionary::dictionary_pentium4();
        SMTSolver *solver = NULL;
        RiscOperatorsPtr ops = RiscOperators::instance(regdict, solver);
        BaseSemantics::DispatcherPtr dispatcher = DispatcherX86::instance(ops);
        return instance(dispatcher);
    }

    /** Translate a single machine instruction to LLVM instructions.  LLVM instructions are emitted to the specified stream
     *  or returned as a string.
     * @{ */
    void transcodeInstruction(SgAsmInstruction*, std::ostream&);
    std::string transcodeInstruction(SgAsmInstruction*);
    /** @} */

    /** Transcode a basic block of machine instructions to LLVM instructions.  LLVM instructions are emitted to the specified
     *  stream or returned as a string.
     * @{ */
    void transcodeBasicBlock(SgAsmBlock*, std::ostream&);
    std::string transcodeBasicBlock(SgAsmBlock*);
    /** @} */

    /** Transcode an entire function to LLVM instructions.  LLVM instructions are emitted to the specified stream or returned
     *  as a string.
     * @{ */
    void transcodeFunction(SgAsmFunction*, std::ostream&);
    std::string transcodeFunction(SgAsmFunction*);
    /** @} */
};

} // namespace
} // namespace
} // namespace

#endif
