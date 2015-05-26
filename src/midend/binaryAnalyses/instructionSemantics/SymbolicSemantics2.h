#ifndef Rose_SymbolicSemantics2_H
#define Rose_SymbolicSemantics2_H

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#include "BaseSemantics2.h"
#include "SMTSolver.h"
#include "InsnSemanticsExpr.h"

#include <map>
#include <vector>

namespace rose {
namespace BinaryAnalysis {              // documented elsewhere
namespace InstructionSemantics2 {       // documented elsewhere

/** A fully symbolic semantic domain.
*
*  This semantic domain can be used to emulate the execution of a single basic block of instructions.  It is similar in nature
*  to PartialSymbolicSemantics, but with a different type of semantics value (SValue): instead of values being a constant or
*  variable with offset, values here are expression trees.
*
*  <ul>
*    <li>SValue: the values stored in registers and memory and used for memory addresses.</li>
*    <li>MemoryCell: an address-expression/value-expression pair for memory.</li>
*    <li>MemoryState: the collection of MemoryCells that form a complete memory state.</li>
*    <li>RegisterState: the collection of registers that form a complete register state.</li>
*    <li>State: represents the state of the virtual machine&mdash;its registers and memory.</li>
*    <li>RiscOperators: the low-level operators called by instruction dispatchers (e.g., DispatcherX86).</li>
*  </ul>
*
*  If an SMT solver is supplied a to the RiscOperators then that SMT solver will be used to answer various questions such as
*  when two memory addresses can alias one another.  When an SMT solver is lacking, the questions will be answered by very
*  naive comparison of the expression trees. */
namespace SymbolicSemantics {

typedef InsnSemanticsExpr::LeafNode LeafNode;
typedef InsnSemanticsExpr::LeafNodePtr LeafNodePtr;
typedef InsnSemanticsExpr::InternalNode InternalNode;
typedef InsnSemanticsExpr::InternalNodePtr InternalNodePtr;
typedef InsnSemanticsExpr::TreeNodePtr TreeNodePtr;
typedef std::set<SgAsmInstruction*> InsnSet;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Semantic values
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Smart pointer to an SValue object.  SValue objects are reference counted and should not be explicitly deleted. */
typedef Sawyer::SharedPointer<class SValue> SValuePtr;

/** Formatter for symbolic values. */
class Formatter: public BaseSemantics::Formatter {
public:
    InsnSemanticsExpr::Formatter expr_formatter;
};

/** Type of values manipulated by the SymbolicSemantics domain.
 *
 *  Values of type type are used whenever a value needs to be stored, such as memory addresses, the values stored at those
 *  addresses, the values stored in registers, the operands for RISC operations, and the results of those operations.
 *
 *  An SValue points to an expression composed of the TreeNode types defined in InsnSemanticsExpr.h, and also stores the set of
 *  instructions that were used to define the value.  This provides a framework for some simple forms of value-based def-use
 *  analysis. See get_defining_instructions() for details.
 * 
 *  @section Unk_Uinit Unknown versus Uninitialized Values
 *
 *  One sometimes needs to distinguish between registers (or other named storage locations) that contain an
 *  "unknown" value versus registers that have not been initialized. By "unknown" we mean a value that has no
 *  constraints except for its size (e.g., a register that contains "any 32-bit value").  By "uninitialized" we
 *  mean a register that still contains the value that was placed there before we started symbolically evaluating
 *  instructions (i.e., a register to which symbolic evaluation hasn't written a new value).
 *
 *  An "unknown" value might be produced by a RISC operation that is unable/unwilling to express its result
 *  symbolically.  For instance, the RISC "add(A,B)" operation could return an unknown/unconstrained result if
 *  either A or B are unknown/unconstrained (in other words, add(A,B) returns C). An unconstrained value is
 *  represented by a free variable. ROSE's SymbolicSemantics RISC operations never return unconstrained values, but
 *  rather always return a new expression (add(A,B) returns the symbolic expression A+B). However, user-defined
 *  subclasses of ROSE's SymbolicSemantics classes might return unconstrained values, and in fact it is quite
 *  common for a programmer to first stub out all the RISC operations to return unconstrained values and then
 *  gradually implement them as they're required.  When a RISC operation returns an unconstrained value, it should
 *  set the returned value's defining instructions to the CPU instruction that caused the RISC operation to be
 *  called (and possibly the union of the sets of instructions that defined the RISC operation's operands).
 *
 *  An "uninitialized" register (or other storage location) is a register that hasn't ever had a value written to
 *  it as a side effect of a machine instruction, and thus still contains the value that was initialially stored
 *  there before analysis started (perhaps by a default constructor).  Such values will generally be unconstrained
 *  (i.e., "unknown" as defined above) but will have an empty defining instruction set.  The defining instruction
 *  set is empty because the register contains a value that was not generated as the result of simulating some
 *  machine instruction.
 *
 *  Therefore, it is possible to destinguish between an uninitialized register and an unconstrained register by
 *  looking at its value.  If the value is a variable with an empty set of defining instructions, then it must be
 *  an initial value.  If the value is a variable but has a non-empty set of defining instructions, then it must be
 *  a value that came from some RISC operation invoked on behalf of a machine instruction.
 *
 *  One should note that a register that contains a variable is not necessarily unconstrained: another register
 *  might contain the same variable, in which case the two registers are constrained to have the same value,
 *  although that value could be anything.  Consider the following example:
 *
 *  Step 1: Initialize registers. At this point EAX contains v1[32]{}, EBX contains v2[32]{}, and ECX contains
 *  v3[32]{}. The notation "vN" is a variable, "[32]" means the variable is 32-bits wide, and "{}" indicates that
 *  the set of defining instructions is empty. Since the defining sets are empty, the registers can be considered
 *  to be "uninitialized" (more specifically, they contain initial values that were created by the symbolic machine
 *  state constructor, or by the user explicitly initializing the registers; they don't contain values that were
 *  put there as a side effect of executing some machine instruction).
 *
 *  Step 2: Execute an x86 "I1: MOV EAX, EBX" instruction that moves the value stored in EBX into the EAX register.
 *  After this instruction executes, EAX contains v2[32]{I1}, EBX contains v2[32]{}, and ECX contains
 *  v3[32]{}. Registers EBX and ECX continue to have empty sets of defining instructions and thus contain their
 *  initial values.  Reigister EAX refers to the same variable (v2) as EBX and therefore must have the same value
 *  as EBX, although that value can be any 32-bit value.  We can also tell that EAX no longer contains its initial
 *  value because the set of defining instructions is non-empty ({I1}).
 *
 *  Step 3: Execute the imaginary "I2: FOO ECX, EAX" instruction and presume that it performs an operation using
 *  ECX and EAX and stores the result in ECX.  The operation is implemented by a new user-defined RISC operation
 *  called "doFoo(A,B)". Furthermore, presume that the operation encoded by doFoo(A,B) cannot be represented by
 *  ROSE's expression trees either directly or indirectly via other expression tree operations. Therefore, the
 *  implementation of doFoo(A,B) is such that it always returns an unconstrained value (i.e., a new variable):
 *  doFoo(A,B) returns C.  After this instruction executes, EAX and EBX continue to contain the results they had
 *  after step 2, and ECX now contains v4[32]{I2}.  We can tell that ECX contains an unknown value (because its
 *  value is a variable)  that is 32-bits wide.  We can also tell that ECX no longer contains its initial value
 *  because its set of defining instructions is non-empty ({I2}).
 */
class SValue: public BaseSemantics::SValue {
protected:
    /** The symbolic expression for this value.  Symbolic expressions are reference counted. */
    TreeNodePtr expr;

    /** Instructions defining this value.  Any instruction that saves the value to a register or memory location
     *  adds itself to the saved value. */
    InsnSet defs;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit SValue(size_t nbits): BaseSemantics::SValue(nbits) {
        expr = LeafNode::create_variable(nbits);
    }
    SValue(size_t nbits, uint64_t number): BaseSemantics::SValue(nbits) {
        expr = LeafNode::create_integer(nbits, number);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new prototypical value. Prototypical values are only used for their virtual constructors. */
    static SValuePtr instance() {
        return SValuePtr(new SValue(1));
    }

    /** Instantiate a new undefined value of specified width. */
    static SValuePtr instance(size_t nbits) {
        return SValuePtr(new SValue(nbits));
    }

    /** Instantiate a new concrete value. */
    static SValuePtr instance(size_t nbits, uint64_t value) {
        return SValuePtr(new SValue(nbits, value));
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual allocating constructors
public:
    virtual BaseSemantics::SValuePtr undefined_(size_t nbits) const ROSE_OVERRIDE {
        return instance(nbits);
    }
    virtual BaseSemantics::SValuePtr number_(size_t nbits, uint64_t value) const ROSE_OVERRIDE {
        return instance(nbits, value);
    }
    virtual BaseSemantics::SValuePtr boolean_(bool value) const ROSE_OVERRIDE {
        return number_(1, value?1:0);
    }
    virtual BaseSemantics::SValuePtr copy(size_t new_width=0) const ROSE_OVERRIDE {
        SValuePtr retval(new SValue(*this));
        if (new_width!=0 && new_width!=retval->get_width())
            retval->set_width(new_width);
        return retval;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Promote a base value to a SymbolicSemantics value.  The value @p v must have a SymbolicSemantics::SValue dynamic type. */
    static SValuePtr promote(const BaseSemantics::SValuePtr &v) { // hot
        SValuePtr retval = v.dynamicCast<SValue>();
        ASSERT_not_null(retval);
        return retval;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override virtual methods...
public:
    virtual bool may_equal(const BaseSemantics::SValuePtr &other, SMTSolver *solver=NULL) const ROSE_OVERRIDE;
    virtual bool must_equal(const BaseSemantics::SValuePtr &other, SMTSolver *solver=NULL) const ROSE_OVERRIDE;

    // It's not possible to change the size of a symbolic expression in place. That would require that we recursively change
    // the size of the InsnSemanticsExpr, which might be shared with many unrelated values whose size we don't want to affect.
    virtual void set_width(size_t nbits) ROSE_OVERRIDE {
        ASSERT_require(nbits==get_width());
    }

    virtual bool is_number() const ROSE_OVERRIDE {
        return expr->is_known();
    }

    virtual uint64_t get_number() const ROSE_OVERRIDE;

    virtual void print(std::ostream&, BaseSemantics::Formatter&) const ROSE_OVERRIDE;

    virtual std::string get_comment() const ROSE_OVERRIDE;
    virtual void set_comment(const std::string&) const ROSE_OVERRIDE;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Additional methods first declared in this class...
public:
    /** Substitute one value for another throughout a value. For example, if this value is "(add esp_0, -12)" and we substitute
     *  "esp_0" with "(add stack_frame 4)", this method would return "(add stack_frame -8)".  It is also possible for the @p
     *  from value to be a more complicated expression. This method attempts to match @p from at all nodes of this expression
     *  and substitutes at eac node that matches.  The @p from and @p to must have the same width. */
    virtual SValuePtr substitute(const SValuePtr &from, const SValuePtr &to) const;

    /** Adds instructions to the list of defining instructions.  Adds the specified instruction and defining sets into this
     *  value and returns a reference to this value. See also add_defining_instructions().
     *  @{ */
    virtual void defined_by(SgAsmInstruction *insn, const InsnSet &set1, const InsnSet &set2, const InsnSet &set3) {
        add_defining_instructions(set3);
        defined_by(insn, set1, set2);
    }
    virtual void defined_by(SgAsmInstruction *insn, const InsnSet &set1, const InsnSet &set2) {
        add_defining_instructions(set2);
        defined_by(insn, set1);
    }
    virtual void defined_by(SgAsmInstruction *insn, const InsnSet &set1) {
        add_defining_instructions(set1);
        defined_by(insn);
    }
    virtual void defined_by(SgAsmInstruction *insn) {
        add_defining_instructions(insn);
    }
    /** @} */

    /** Returns the expression stored in this value.  Expressions are reference counted; the reference count of the returned
     *  expression is not incremented. */
    virtual const TreeNodePtr& get_expression() const {
        return expr;
    }

    /** Changes the expression stored in the value.
     * @{ */
    virtual void set_expression(const TreeNodePtr &new_expr) {
        expr = new_expr;
    }
    virtual void set_expression(const SValuePtr &source) {
        set_expression(source->get_expression());
    }
    /** @} */

    /** Returns the set of instructions that defined this value.  The return value is a flattened lattice represented as a set.
     *  When analyzing this basic block starting with an initial default state:
     *
     *  @code
     *  1: mov eax, 2
     *  2: add eax, 1
     *  3: mov ebx, eax;
     *  4: mov ebx, 3
     *  @endcode
     *
     *  the defining set for EAX will be instructions {1, 2} and the defining set for EBX will be {4}.  Defining sets for other
     *  registers are the empty set. */
    virtual const InsnSet& get_defining_instructions() const {
        return defs;
    }

    /** Adds definitions to the list of defining instructions. Returns the number of items added that weren't already in the
     *  list of defining instructions.  @{ */
    virtual size_t add_defining_instructions(const InsnSet &to_add);
    virtual size_t add_defining_instructions(const SValuePtr &source) {
        return add_defining_instructions(source->get_defining_instructions());
    }
    virtual size_t add_defining_instructions(SgAsmInstruction *insn);
    /** @} */

    /** Set definint instructions.  This discards the old set of defining instructions and replaces it with the specified set.
     *  @{ */
    virtual void set_defining_instructions(const InsnSet &new_defs) {
        defs = new_defs;
    }
    virtual void set_defining_instructions(const SValuePtr &source) {
        set_defining_instructions(source->get_defining_instructions());
    }
    virtual void set_defining_instructions(SgAsmInstruction *insn);
    /** @} */
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Register state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef BaseSemantics::RegisterStateGeneric RegisterState;
typedef BaseSemantics::RegisterStateGenericPtr RegisterStatePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Smart pointer to a MemoryState object.  MemoryState objects are reference counted and should not be explicitly deleted. */
typedef boost::shared_ptr<class MemoryState> MemoryStatePtr;

/** Byte-addressable memory.
 *
 *  This class represents an entire state of memory via a list of memory cells.  The memory cell list is sorted in
 *  reverse chronological order and addresses that satisfy a "must-alias" predicate are pruned so that only the
 *  must recent such memory cell is in the table.
 *
 *  A memory write operation prunes away any existing memory cell that must-alias the newly written address, then
 *  adds a new memory cell to the front of the memory cell list.
 *
 *  A memory read operation scans the memory cell list in reverse chronological order to obtain the list of cells that
 *  may-alias the address being read (stopping when it hits a must-alias cell).  If no must-alias cell is found, then a new
 *  cell is added to the memory and the may-alias list.  In any case, if the may-alias list contains exactly one cell, that
 *  cell's value is returned; otherwise a CellCompressor is called.  The default CellCompressor either returns a McCarthy
 *  expression or the default value depending on whether an SMT solver is being used. */
class MemoryState: public BaseSemantics::MemoryCellList {
public:
    /** Functor for handling a memory read that found more than one cell that might alias the requested address. */
    struct CellCompressor {
        virtual ~CellCompressor() {}
        virtual SValuePtr operator()(const SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                     BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps,
                                     const MemoryCellList::CellList &cells) = 0;
    };
    
    /** Functor for handling a memory read whose address matches more than one memory cell.  This functor returns a symbolic
     * expression that consists of a read operation on a memory state.  The returned expression is essentially a McCarthy
     * expression that encodes this if-then-else structure:
     *
     * @code
     *  define readMemory(Address A): {
     *     if A == Cell[0].address then return Cell[0].value
     *     else if A == Cell[1].address then return Cell[1].value
     *     else if A == Cell[2].address then return Cell[2].value
     *     ...
     *  }
     * @endcode
     */
    struct CellCompressorMcCarthy: CellCompressor {
        virtual SValuePtr operator()(const SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                     BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps,
                                     const CellList &cells) ROSE_OVERRIDE;
    };

    /** Functor for handling a memory read whose address matches more than one memory cell.  Simply returns the @p dflt value. */
    struct CellCompressorSimple: CellCompressor {
        virtual SValuePtr operator()(const SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                     BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps,
                                     const CellList &cells) ROSE_OVERRIDE;
    };

    /** Functor for handling a memory read whose address matches more than one memory cell.  This is the default cell
     *  compressor and simply calls either CellCompressionMcCarthy or CellCompressionSimple depending on whether an SMT
     *  solver is being used. */
    struct CellCompressorChoice: CellCompressor {
        CellCompressorMcCarthy cc_mccarthy;
        CellCompressorSimple cc_simple;
        virtual SValuePtr operator()(const SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                     BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps,
                                     const CellList &cells) ROSE_OVERRIDE;
    };

protected:
    CellCompressor *cell_compressor;            /**< Callback when a memory read aliases multiple memory cells. */
    static CellCompressorChoice cc_choice;      /**< The default cell compressor. Static because we use its address. */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    MemoryState(const BaseSemantics::MemoryCellPtr &protocell)
        : BaseSemantics::MemoryCellList(protocell), cell_compressor(&cc_choice) {}

    MemoryState(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval)
        : BaseSemantics::MemoryCellList(addrProtoval, valProtoval), cell_compressor(&cc_choice) {}

    MemoryState(const MemoryState &other)
        : BaseSemantics::MemoryCellList(other), cell_compressor(other.cell_compressor) {}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiates a new memory state having specified prototypical cells and value. */
    static MemoryStatePtr instance(const BaseSemantics::MemoryCellPtr &protocell) {
        return MemoryStatePtr(new MemoryState(protocell));
    }

    /** Instantiates a new memory state having specified prototypical value.  This constructor uses BaseSemantics::MemoryCell
     * as the cell type. */
    static  MemoryStatePtr instance(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval) {
        return MemoryStatePtr(new MemoryState(addrProtoval, valProtoval));
    }

    /** Instantiates a new deep copy of an existing state. */
    static MemoryStatePtr instance(const MemoryStatePtr &other) {
        return MemoryStatePtr(new MemoryState(*other));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    /** Virtual constructor. Creates a memory state having specified prototypical value.  This constructor uses
     * BaseSemantics::MemoryCell as the cell type. */
    virtual BaseSemantics::MemoryStatePtr create(const BaseSemantics::SValuePtr &addrProtoval,
                                                 const BaseSemantics::SValuePtr &valProtoval) const ROSE_OVERRIDE {
        return instance(addrProtoval, valProtoval);
    }

    /** Virtual constructor. Creates a new memory state having specified prototypical cells and value. */
    virtual BaseSemantics::MemoryStatePtr create(const BaseSemantics::MemoryCellPtr &protocell) const ROSE_OVERRIDE {
        return instance(protocell);
    }

    /** Virtual copy constructor. Creates a new deep copy of this memory state. */
    virtual BaseSemantics::MemoryStatePtr clone() const ROSE_OVERRIDE {
        return MemoryStatePtr(new MemoryState(*this));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Recasts a base pointer to a symbolic memory state. This is a checked cast that will fail if the specified pointer does
     *  not have a run-time type that is a SymbolicSemantics::MemoryState or subclass thereof. */
    static MemoryStatePtr promote(const BaseSemantics::MemoryStatePtr &x) {
        MemoryStatePtr retval = boost::dynamic_pointer_cast<MemoryState>(x);
        ASSERT_not_null(retval);
        return retval;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods we inherited
public:
    /** Read a byte from memory.
     *
     *  In order to read a multi-byte value, use RiscOperators::readMemory(). */
    virtual BaseSemantics::SValuePtr readMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &dflt,
                                                BaseSemantics::RiscOperators *addrOps,
                                                BaseSemantics::RiscOperators *valOps) ROSE_OVERRIDE;

    /** Write a byte to memory.
     *
     *  In order to write a multi-byte value, use RiscOperators::writeMemory(). */
    virtual void writeMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &value,
                             BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) ROSE_OVERRIDE;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first declared in this class
public:
    /** Callback for handling a memory read whose address matches more than one memory cell.  See also,
     * cell_compression_mccarthy(), cell_compression_simple(), cell_compression_choice().
     * @{ */
    CellCompressor* get_cell_compressor() const { return cell_compressor; }
    void set_cell_compressor(CellCompressor *cc) { cell_compressor = cc; }
    /** @} */
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Complete state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef BaseSemantics::State State;
typedef BaseSemantics::StatePtr StatePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RISC operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Smart pointer to a RiscOperators object.  RiscOperators objects are reference counted and should not be explicitly
 *  deleted. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** Defines RISC operators for the SymbolicSemantics domain.
 *
 *  These RISC operators depend on functionality introduced into the SValue class hierarchy at the SymbolicSemantics::SValue
 *  level. Therefore, the prototypical value supplied to the constructor or present in the supplied state object must have a
 *  dynamic type which is a SymbolicSemantics::SValue.
 *
 *  The RiscOperators object also controls whether use-def information is computed and stored in the SValues.  The default is
 *  to not compute this information.  The set_compute_usedef() method can be used to enable this feature.
 *
 *  Each RISC operator should return a newly allocated semantic value so that the caller can adjust definers for the result
 *  without affecting any of the inputs. For example, a no-op that returns its argument should be implemented like this:
 *
 * @code
 *  BaseSemantics::SValuePtr noop(const BaseSemantics::SValuePtr &arg) {
 *      return arg->copy();     //correct
 *      return arg;             //incorrect
 *  }
 * @endcode
 */
class RiscOperators: public BaseSemantics::RiscOperators {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit RiscOperators(const BaseSemantics::SValuePtr &protoval, SMTSolver *solver=NULL)
        : BaseSemantics::RiscOperators(protoval, solver), compute_usedef(false), omit_cur_insn(false),
          compute_memwriters(true) {
        set_name("Symbolic");
        (void) SValue::promote(protoval); // make sure its dynamic type is a SymbolicSemantics::SValue
    }

    explicit RiscOperators(const BaseSemantics::StatePtr &state, SMTSolver *solver=NULL)
        : BaseSemantics::RiscOperators(state, solver), compute_usedef(false), omit_cur_insn(false),
          compute_memwriters(true) {
        set_name("Symbolic");
        (void) SValue::promote(state->get_protoval()); // values must have SymbolicSemantics::SValue dynamic type
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiates a new RiscOperators object and configures it to use semantic values and states that are defaults for
     * SymbolicSemantics. */
    static RiscOperatorsPtr instance(const RegisterDictionary *regdict, SMTSolver *solver=NULL) {
        BaseSemantics::SValuePtr protoval = SValue::instance();
        BaseSemantics::RegisterStatePtr registers = RegisterState::instance(protoval, regdict);
        BaseSemantics::MemoryStatePtr memory = MemoryState::instance(protoval, protoval);
        BaseSemantics::StatePtr state = State::instance(registers, memory);
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }

    /** Instantiates a new RiscOperators object with specified prototypical values.  An SMT solver may be specified as the
     *  second argument for convenience. See set_solver() for details. */
    static RiscOperatorsPtr instance(const BaseSemantics::SValuePtr &protoval, SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(protoval, solver));
    }

    /** Instantiates a new RiscOperators object with specified state.  An SMT solver may be specified as the second argument
     *  for convenience. See set_solver() for details. */
    static RiscOperatorsPtr instance(const BaseSemantics::StatePtr &state, SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   SMTSolver *solver=NULL) const ROSE_OVERRIDE {
        return instance(protoval, solver);
    }

    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr &state,
                                                   SMTSolver *solver=NULL) const ROSE_OVERRIDE {
        return instance(state, solver);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Run-time promotion of a base RiscOperators pointer to symbolic operators. This is a checked conversion--it
     *  will fail if @p x does not point to a SymbolicSemantics::RiscOperators object. */
    static RiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr &x) {
        RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Inherited methods for constructing values.
public:
    virtual BaseSemantics::SValuePtr boolean_(bool b) {
        SValuePtr retval = SValue::promote(BaseSemantics::RiscOperators::boolean_(b));
        if (compute_usedef && !omit_cur_insn)
            retval->defined_by(get_insn());
        return retval;
    }

    virtual BaseSemantics::SValuePtr number_(size_t nbits, uint64_t value) {
        SValuePtr retval = SValue::promote(BaseSemantics::RiscOperators::number_(nbits, value));
        if (compute_usedef && !omit_cur_insn)
            retval->defined_by(get_insn());
        return retval;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // New methods for constructing values, so we don't have to write so many SValue::promote calls in the RiscOperators
    // implementations.
protected:
    SValuePtr svalue_expr(const TreeNodePtr &expr, const InsnSet &defs=InsnSet()) {
        SValuePtr newval = SValue::promote(protoval->undefined_(expr->get_nbits()));
        newval->set_expression(expr);
        newval->set_defining_instructions(defs);
        return newval;
    }

    SValuePtr svalue_undefined(size_t nbits) {
        return SValue::promote(undefined_(nbits));
    }

    SValuePtr svalue_number(size_t nbits, uint64_t value) {
        return SValue::promote(number_(nbits, value));
    }

    SValuePtr svalue_boolean(bool b) {
        return SValue::promote(boolean_(b));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Configuration properties
protected:
    bool compute_usedef;                                // if true, add use-def info to each value
    bool omit_cur_insn;                                 // if true, do not include cur_insn as a definer
    bool compute_memwriters;                            // if true, add latest writer to each memory cell

public:
    /** Accessor for the compute_usedef property.  If compute_usedef is set, then RISC operators will update the set of
     *  defining instructions in their return values, computing the new set from the sets of definers for the RISC operands and
     *  possibly the current instruction (the current instruction is usually not a definer if it simply copies data verbatim).
     * @{ */
    void set_compute_usedef(bool b=true) { compute_usedef = b; }
    void clear_compute_usedef() { set_compute_usedef(false); }
    bool get_compute_usedef() const { return compute_usedef; }
    /** @} */

    /** Property: track latest writer to each memory location.
     *
     *  If true, then each @ref writeMemory operation will update the affected memory cells with latest-writer information if
     *  possible (depending on the type of memory state being used.
     *
     * @{ */
    void set_compute_memwriters(bool b = true) { compute_memwriters = b; }
    void clear_compute_memwriters() { compute_memwriters = false; }
    bool get_compute_memwriters() const { return compute_memwriters; }
    /** @} */

    // Used internally to control whether cur_insn should be omitted from the list of definers.
    bool getset_omit_cur_insn(bool b) { bool retval = omit_cur_insn; omit_cur_insn=b; return retval; }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first defined at this level of the class hierarchy
public:
    /** Substitute all occurrences of @p from with @p to in the current state.  For instance, in functions that use a frame
     *  pointer set up with "push ebp; mov ebp, esp", it is convenient to see stack offsets in terms of the function's stack
     *  frame rather than in terms of the original esp value.  This convenience comes from the fact that compilers tend to
     *  emit stack accessing code where the addresses are offsets from the function's stack frame.
     *
     *  For instance, after the "push ebp; mov ebp, esp" prologue, the machine state is:
     * @code
     *  registers:
     *    esp    = (add[32] esp_0[32] -4[32])
     *    ebp    = (add[32] esp_0[32] -4[32])
     *    eip    = 0x080480a3[32]
     *  memory:
     *    addr=(add[32] esp_0[32] -1[32]) value=(extract[8] 24[32] 32[32] ebp_0[32])
     *    addr=(add[32] esp_0[32] -2[32]) value=(extract[8] 16[32] 24[32] ebp_0[32])
     *    addr=(add[32] esp_0[32] -3[32]) value=(extract[8] 8[32] 16[32] ebp_0[32])
     *    addr=(add[32] esp_0[32] -4[32]) value=(extract[8] 0[32] 8[32] ebp_0[32])
     * @endcode
     *
     *  If we create a new variable called "stack_frame" where
     *
     * @code
     *  stack_frame = esp_0 - 4
     * @endcode
     *
     *  Solving for esp_0:
     *
     * @code
     *  esp_0 = stack_frame + 4
     * @endcode
     *
     * Then replacing the lhs (esp_0) with the rhs (stack_frame + 4) in the machine state causes the expressions to be
     * rewritten in terms of stack_frame instead of esp_0:
     * 
     * @code
     *  registers:
     *    esp    = stack_frame[32]
     *    ebp    = stack_frame[32]
     *    eip    = 0x080480a3[32]
     *  memory:
     *    addr=(add[32] stack_frame[32] 3[32]) value=(extract[8] 24[32] 32[32] ebp_0[32])
     *    addr=(add[32] stack_frame[32] 2[32]) value=(extract[8] 16[32] 24[32] ebp_0[32])
     *    addr=(add[32] stack_frame[32] 1[32]) value=(extract[8] 8[32] 16[32] ebp_0[32])
     *    addr=stack_frame[32] value=(extract[8] 0[32] 8[32] ebp_0[32])
     * @endcode
     *
     * Here's the source code for that substitution:
     *
     * @code
     *  SymbolicSemantics::RiscOperatorsPtr operators = ...;
     *  SymbolicSemantics::SValuePtr original_esp = ...; //probably read from the initial state
     *  BaseSemantics::SValuePtr stack_frame = operators->undefined_(32);
     *  stack_frame->set_comment("stack_frame"); //just so output looks nice
     *  SymbolicSemantics::SValuePtr rhs = SymbolicSemantics::SValue::promote(
     *      operators->add(stack_frame, operators->number_(32, 4))
     *  );
     *
     *  std::cerr <<"Prior to state:\n" <<*operators;
     *  operators->substitute(original_esp, rhs);
     *  std::cerr <<"Substituted state:\n" <<*operators;
     * @endcode
     */
    virtual void substitute(const SValuePtr &from, const SValuePtr &to);
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override methods from base class.  These are the RISC operators that are invoked by a Dispatcher.
public:
    virtual void interrupt(int majr, int minr) ROSE_OVERRIDE;
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
    virtual BaseSemantics::SValuePtr unsignedExtend(const BaseSemantics::SValuePtr &a_, size_t new_width) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr signExtend(const BaseSemantics::SValuePtr &a_, size_t new_width) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr add(const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr addWithCarries(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_,
                                                    const BaseSemantics::SValuePtr &c_,
                                                    BaseSemantics::SValuePtr &carry_out/*out*/);
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
    virtual BaseSemantics::SValuePtr readRegister(const RegisterDescriptor &reg) ROSE_OVERRIDE;
    virtual void writeRegister(const RegisterDescriptor &reg, const BaseSemantics::SValuePtr &a_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr readMemory(const RegisterDescriptor &segreg,
                                                const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt,
                                                const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;
    virtual void writeMemory(const RegisterDescriptor &segreg,
                             const BaseSemantics::SValuePtr &addr,
                             const BaseSemantics::SValuePtr &data,
                             const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
