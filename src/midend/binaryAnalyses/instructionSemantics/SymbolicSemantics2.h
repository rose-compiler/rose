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



/******************************************************************************************************************
 *                                      Value type
 ******************************************************************************************************************/

/** Smart pointer to an SValue object.  SValue objects are reference counted and should not be explicitly deleted. */
typedef BaseSemantics::Pointer<class SValue> SValuePtr;

/** Type of values manipulated by the SymbolicSemantics domain.
 *
 *  Values of type type are used whenever a value needs to be stored, such as memory addresses, the values stored at those
 *  addresses, the values stored in registers, the operands for RISC operations, and the results of those operations.
 *
 *  An SValue points to an expression composed of the TreeNode types defined in InsnSemanticsExpr.h, also stores the set of
 *  instructions that were used in defining the value.  This provides a framework for some simple forms of def-use
 *  analysis. See get_defining_instructions() for details. */
class SValue: public BaseSemantics::SValue {
protected:
    /** The symbolic expression for this value.  Symbolic expressions are reference counted. */
    TreeNodePtr expr;

    /** Instructions defining this value.  Any instruction that saves the value to a register or memory location
     *  adds itself to the saved value. */
    InsnSet defs;

protected:
    // Protected constructors
    explicit SValue(size_t nbits): BaseSemantics::SValue(nbits) {
        expr = LeafNode::create_variable(nbits);
    }
    SValue(size_t nbits, uint64_t number): BaseSemantics::SValue(nbits) {
        expr = LeafNode::create_integer(nbits, number);
    }
    SValue(const TreeNodePtr &expr, const InsnSet &defs): BaseSemantics::SValue(expr->get_nbits()) {
        width = expr->get_nbits();
        this->expr = expr;
        this->defs = defs;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors...
public:
    /** Construct a prototypical value. Prototypical values are only used for their virtual constructors. */
    static SValuePtr instance() {
        return SValuePtr(new SValue(1));
    }

    /** Promote a base value to a SymbolicSemantics value.  The value @p v must have a SymbolicSemantics::SValue dynamic type. */
    static SValuePtr promote(const BaseSemantics::SValuePtr &v) {
        SValuePtr retval = BaseSemantics::dynamic_pointer_cast<SValue>(v);
        assert(retval!=NULL);
        return retval;
    }

    // Virtual allocating constructors inherited from the base class
    virtual BaseSemantics::SValuePtr undefined_(size_t nbits) const /*override*/ {
        return BaseSemantics::SValuePtr(new SValue(nbits));
    }
    virtual BaseSemantics::SValuePtr number_(size_t nbits, uint64_t value) const /*override*/ {
        return BaseSemantics::SValuePtr(new SValue(nbits, value));
    }
    virtual BaseSemantics::SValuePtr copy(size_t new_width=0) const /*override*/ {
        SValuePtr retval(new SValue(*this));
        if (new_width!=0 && new_width!=retval->get_width())
            retval->set_width(new_width);
        return retval;
    }

    /** Virtual allocating constructor. Constructs a new semantic value with full control over all aspects of the value. */
    virtual BaseSemantics::SValuePtr create(const TreeNodePtr &expr, const InsnSet &defs=InsnSet()) {
        return SValuePtr(new SValue(expr, defs));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override virtual methods...
public:
    virtual bool may_equal(const BaseSemantics::SValuePtr &other, SMTSolver *solver=NULL) const /*override*/;
    virtual bool must_equal(const BaseSemantics::SValuePtr &other, SMTSolver *solver=NULL) const /*override*/;

    // It's not possible to change the size of a symbolic expression in place. That would require that we recursively change
    // the size of the InsnSemanticsExpr, which might be shared with many unrelated values whose size we don't want to affect.
    virtual void set_width(size_t nbits) /*override*/ {
        assert(nbits==get_width());
    }

    virtual bool is_number() const /*override*/ {
        return expr->is_known();
    }

    virtual uint64_t get_number() const /*override*/ {
        LeafNodePtr leaf = expr->isLeafNode();
        assert(leaf!=NULL);
        return leaf->get_value();
    }

    virtual void print(std::ostream &output, BaseSemantics::PrintHelper *helper=NULL) const /*override*/;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Additional methods first declared in this class...
public:
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
    virtual size_t add_defining_instructions(const InsnSet &to_add) {
        size_t nadded = 0;
        for (InsnSet::const_iterator i=to_add.begin(); i!=to_add.end(); ++i) {
            std::pair<InsnSet::iterator, bool> inserted = defs.insert(*i);
            if (inserted.second)
                ++nadded;
        }
        return nadded;
    }
    virtual size_t add_defining_instructions(const SValuePtr &source) {
        return add_defining_instructions(source->get_defining_instructions());
    }
    virtual size_t add_defining_instructions(SgAsmInstruction *insn) {
        InsnSet tmp;
        if (insn)
            tmp.insert(insn);
        return add_defining_instructions(tmp);
    }
    /** @} */

    /** Set definint instructions.  This discards the old set of defining instructions and replaces it with the specified set.
     *  @{ */
    virtual void set_defining_instructions(const InsnSet &new_defs) {
        defs = new_defs;
    }
    virtual void set_defining_instructions(const SValuePtr &source) {
        set_defining_instructions(source->get_defining_instructions());
    }
    virtual void set_defining_instructions(SgAsmInstruction *insn) {
        InsnSet tmp;
        if (insn)
            tmp.insert(insn);
        return set_defining_instructions(tmp);
    }
    /** @} */
};



/******************************************************************************************************************
 *                                      Memory state
 ******************************************************************************************************************/

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
 *  A memory read operation scans the memory cell list and returns a McCarthy expression.  The read operates in two
 *  modes: a mode that returns a full McCarthy expression based on all memory cells in the cell list, or a mode
 *  that returns a pruned McCarthy expression consisting only of memory cells that may-alias the reading-from
 *  address.  The pruning mode is the default, but can be turned off by calling disable_read_pruning(). */
class MemoryState: public BaseSemantics::MemoryCellList {
protected:
    
    bool read_pruning;                      /**< Prune McCarthy expression for read operations. */

protected:
    // protected constructors, same as for the base class
    MemoryState(const BaseSemantics::MemoryCellPtr &protocell, const BaseSemantics::SValuePtr &protoval)
        : BaseSemantics::MemoryCellList(protocell, protoval), read_pruning(true) {}

public:
    /** Static allocating constructor.  This constructor uses BaseSemantics::MemoryCell as the cell type. */
    static  MemoryStatePtr instance(const BaseSemantics::SValuePtr &protoval) {
        BaseSemantics::MemoryCellPtr protocell = BaseSemantics::MemoryCell::instance(protoval, protoval);
        return MemoryStatePtr(new MemoryState(protocell, protoval));
    }

    /** Static allocating constructor. */
    static MemoryStatePtr instance(const BaseSemantics::MemoryCellPtr &protocell, const BaseSemantics::SValuePtr &protoval) {
        return MemoryStatePtr(new MemoryState(protocell, protoval));
    }

    virtual BaseSemantics::MemoryStatePtr create(const BaseSemantics::SValuePtr &protoval) const /*override*/ {
        return instance(protoval);
    }

    virtual BaseSemantics::MemoryStatePtr create(const BaseSemantics::MemoryCellPtr &protocell,
                                                 const BaseSemantics::SValuePtr &protoval) const /*override*/ {
        return instance(protocell, protoval);
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods that override the base class
public:
    /** Read a byte from memory.
     *
     *  In order to read a multi-byte value, use RiscOperators::readMemory(). */
    virtual BaseSemantics::SValuePtr readMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &dflt,
                                                size_t nbits, BaseSemantics::RiscOperators *ops) /*override*/;

    /** Write a byte to memory.
     *
     *  In order to write a multi-byte value, use RiscOperators::writeMemory(). */
    virtual void writeMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &value,
                             BaseSemantics::RiscOperators *ops) /*override*/;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first declared in this class
public:
    /** Enables or disables pruning of the McCarthy expression for read operations.
     * @{ */
    virtual bool get_read_pruning() const { return read_pruning; }
    virtual void enable_read_pruning(bool b=true) { read_pruning = b; }
    virtual void disable_read_pruning() { read_pruning = false; }
    /** @} */

    /** Build a McCarthy expression to select the specified address from a list of memory cells. */
    virtual SValuePtr mccarthy(const CellList &cells, const SValuePtr &address);
};



/******************************************************************************************************************
 *                                      RISC Operators
 ******************************************************************************************************************/

/** Smart pointer to a RiscOperators object.  RiscOperators objects are reference counted and should not be explicitly
 *  deleted. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** Defines RISC operators for the SymbolicSemantics domain.
 *
 *  These RISC operators depend on functionality introduced into the SValue class hierarchy at the SymbolicSemantics::SValue
 *  level. Therfore, the prototypical value supplied to the constructor or present in the supplied state object must have a
 *  dynamic type which is a SymbolicSemantics::SValue.
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
    // Protected constructors, same as those in the base class
protected:
    explicit RiscOperators(const BaseSemantics::SValuePtr &protoval, SMTSolver *solver=NULL)
        : BaseSemantics::RiscOperators(protoval, solver) {
        (void) SValue::promote(protoval); // make sure its dynamic type is a SymbolicSemantics::SValue
    }

    explicit RiscOperators(const BaseSemantics::StatePtr &state, SMTSolver *solver=NULL)
        : BaseSemantics::RiscOperators(state, solver) {
        (void) SValue::promote(state->get_protoval()); // values must have SymbolicSemantics::SValue dynamic type
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Public allocating constructors
public:
    /** Static allocating constructor.  Creats a new RiscOperators object and configures it to use semantic values and states
     * that are defaults for SymbolicSemantics. */
    static RiscOperatorsPtr instance(SMTSolver *solver=NULL) {
        BaseSemantics::SValuePtr protoval = SValue::instance();
        // FIXME: register state should probably be chosen based on an architecture [Robb Matzke 2013-03-01]
        BaseSemantics::RegisterStatePtr registers = BaseSemantics::RegisterStateX86::instance(protoval);
        BaseSemantics::MemoryStatePtr memory = MemoryState::instance(protoval);
        BaseSemantics::StatePtr state = BaseSemantics::State::instance(registers, memory);
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }

    /** Static allocating constructor.  An SMT solver may be specified as the second argument for convenience. See
     *  set_solver() for details. */
    static RiscOperatorsPtr instance(const BaseSemantics::SValuePtr &protoval, SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(protoval, solver));
    }

    /** Static allocating constructor.  An SMT solver may be specified as the second argument for convenience. See set_solver()
     *  for details. */
    static RiscOperatorsPtr instance(const BaseSemantics::StatePtr &state, SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }

    /** Virtual allocating constructor. */
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   SMTSolver *solver=NULL) const /*override*/ {
        return instance(protoval, solver);
    }

    /** Virtual allocating constructor. */
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr &state,
                                                   SMTSolver *solver=NULL) const /*override*/ {
        return instance(state, solver);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first introduced by this class
public:
    /** Convenience function to create a new symbolic semantic value having the specified expression and definers. This makes
     *  the RISC operator implementations less verbose.  We need to promote (dynamic cast) the prototypical value to a
     *  SymbolicSemantics::SValue in order to get to the methods that were introduced at that level of the class hierarchy. */
    virtual SValuePtr svalue(const TreeNodePtr &expr, const InsnSet &defs=InsnSet()) {
        BaseSemantics::SValuePtr newval = SValue::promote(protoval)->create(expr, defs);
        return SValue::promote(newval);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override some non-virtual functions only to change their return type for the convenience of us not having to
    // constantly explicitly dynamic cast them to our own SValuePtr type.
public:
    SValuePtr number_(size_t nbits, uint64_t value) {
        return SValue::promote(protoval->number_(nbits, value));
    }
    SValuePtr true_() {
        return SValue::promote(protoval->true_());
    }
    SValuePtr false_() {
        return SValue::promote(protoval->false_());
    }
    SValuePtr undefined_(size_t nbits) {
        return SValue::promote(protoval->undefined_(nbits));
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override methods from base class.  These are the RISC operators that are invoked by a Dispatcher.
public:
    virtual void interrupt(uint8_t inum) /*override*/;
    virtual void sysenter() /*override*/;
    virtual BaseSemantics::SValuePtr and_(const BaseSemantics::SValuePtr &a_,
                                          const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr or_(const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr xor_(const BaseSemantics::SValuePtr &a_,
                                          const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr invert(const BaseSemantics::SValuePtr &a_) /*override*/;
    virtual BaseSemantics::SValuePtr extract(const BaseSemantics::SValuePtr &a_,
                                             size_t begin_bit, size_t end_bit) /*override*/;
    virtual BaseSemantics::SValuePtr concat(const BaseSemantics::SValuePtr &a_,
                                            const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr leastSignificantSetBit(const BaseSemantics::SValuePtr &a_) /*override*/;
    virtual BaseSemantics::SValuePtr mostSignificantSetBit(const BaseSemantics::SValuePtr &a_) /*override*/;
    virtual BaseSemantics::SValuePtr rotateLeft(const BaseSemantics::SValuePtr &a_,
                                                const BaseSemantics::SValuePtr &sa_) /*override*/;
    virtual BaseSemantics::SValuePtr rotateRight(const BaseSemantics::SValuePtr &a_,
                                                 const BaseSemantics::SValuePtr &sa_) /*override*/;
    virtual BaseSemantics::SValuePtr shiftLeft(const BaseSemantics::SValuePtr &a_,
                                               const BaseSemantics::SValuePtr &sa_) /*override*/;
    virtual BaseSemantics::SValuePtr shiftRight(const BaseSemantics::SValuePtr &a_,
                                                const BaseSemantics::SValuePtr &sa_) /*override*/;
    virtual BaseSemantics::SValuePtr shiftRightArithmetic(const BaseSemantics::SValuePtr &a_,
                                                          const BaseSemantics::SValuePtr &sa_) /*override*/;
    virtual BaseSemantics::SValuePtr equalToZero(const BaseSemantics::SValuePtr &a_) /*override*/;
    virtual BaseSemantics::SValuePtr ite(const BaseSemantics::SValuePtr &sel_,
                                         const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr unsignedExtend(const BaseSemantics::SValuePtr &a_, size_t new_width) /*override*/;
    virtual BaseSemantics::SValuePtr signExtend(const BaseSemantics::SValuePtr &a_, size_t new_width) /*override*/;
    virtual BaseSemantics::SValuePtr add(const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr addWithCarries(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_,
                                                    const BaseSemantics::SValuePtr &c_,
                                                    BaseSemantics::SValuePtr &carry_out/*out*/);
    virtual BaseSemantics::SValuePtr negate(const BaseSemantics::SValuePtr &a_) /*override*/;
    virtual BaseSemantics::SValuePtr signedDivide(const BaseSemantics::SValuePtr &a_,
                                                  const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr signedModulo(const BaseSemantics::SValuePtr &a_,
                                                  const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr signedMultiply(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr unsignedDivide(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr unsignedModulo(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr unsignedMultiply(const BaseSemantics::SValuePtr &a_,
                                                      const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr readMemory(X86SegmentRegister sg,
                                                const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &cond,
                                                size_t nbits) /*override*/;
    virtual void writeMemory(X86SegmentRegister sg,
                             const BaseSemantics::SValuePtr &addr,
                             const BaseSemantics::SValuePtr &data,
                             const BaseSemantics::SValuePtr &cond) /*override*/;
};

} /*namespace*/
} /*namespace*/
} /*namespace*/

#endif
