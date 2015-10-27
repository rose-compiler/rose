#ifndef Rose_SymbolicSemantics_H
#define Rose_SymbolicSemantics_H
#include <stdint.h>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#include "x86InstructionSemantics.h"
#include "BaseSemantics.h"
#include "SMTSolver.h"

#include <map>
#include <vector>

namespace rose {
namespace BinaryAnalysis {              // documented elsewhere
namespace InstructionSemantics {    // documented elsewhere

/** A fully symbolic semantic domain.
 *
 *  This semantic domain can be used to emulate the execution of a single basic block of instructions.  It is similar
 *  in nature to PartialSymbolicSemantics, but with a different type of ValueType: instead of values being a constant or
 *  variable with offset, values here are expression trees.
 *
 *  <ul>
 *    <li>ValueType: the values stored in registers and memory and used for memory addresses.</li>
 *    <li>MemoryCell: an address-expression/value-expression pair for memory.</li>
 *    <li>MemoryState: the collection of MemoryCells that form a complete memory state.</li>
 *    <li>RegisterState: the collection of registers that form a complete register state.</li>
 *    <li>State: represents the state of the virtual machine&mdash;its registers and memory.</li>
 *    <li>Policy: the policy class used to instantiate X86InstructionSemantic instances.</li>
 *  </ul>
 *
 *  If an SMT solver is supplied as a Policy constructor argument then that SMT solver will be used to answer various
 *  questions such as when two memory addresses can alias one another.  When an SMT solver is lacking, the questions
 *  will be answered by very naive comparison of the expression trees. */
namespace SymbolicSemantics {

typedef SymbolicExpr::Leaf LeafNode;
typedef SymbolicExpr::LeafPtr LeafNodePtr;
typedef SymbolicExpr::Interior InternalNode;
typedef SymbolicExpr::InteriorPtr InternalNodePtr;
typedef SymbolicExpr::Node TreeNode;
typedef SymbolicExpr::Ptr TreeNodePtr;
typedef std::set<SgAsmInstruction*> InsnSet;

/** Formatter for symbolic values. */
class Formatter: public BaseSemantics::Formatter {
public:
    Formatter(): show_defs(true) {
        expr_formatter.use_hexadecimal = false;             // preserve the old behavior in API1
    }
    SymbolicExpr::Formatter expr_formatter;
    bool show_defs;
};

/******************************************************************************************************************
 *                          ValueType
 ******************************************************************************************************************/

/** Symbolic expressions.
 *
 *  The ValueType is used whenever a value needs to be stored, such as memory addresses, the values stored at those
 *  addresses, the values stored in registers, the operands for RISC operations, and the results of those
 *  operations.
 *
 *  A ValueType has an intrinsic size in bits and points to an expression composed of the TreeNode types defined in
 *  BinarySymbolicExpr.h. ValueType cannot directly be a TreeNode because ValueType's bit size is a template
 *  argument while tree node sizes are stored as a data member.  Therefore, ValueType will always point to a
 *  TreeNode.  Most of the methods that are invoked on ValueType just call the same methods for TreeNode.
 *
 *  A ValueType also stores the set of instructions that were used in defining the value.  This provides a
 *  framework for some simple forms of def-use analysis. See get_defining_instructions() for details.
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
template<size_t nBits>
class ValueType {
protected:
    TreeNodePtr expr;

    /** Instructions defining this value.  Any instruction that saves the value to a register or memory location
     *  adds itself to the saved value. */
    InsnSet defs;

public:

    /** Construct a value that is unknown and unique. */
    ValueType(std::string comment="") {
        expr = LeafNode::createVariable(nBits, comment);
    }

    /** Copy constructor. */
    ValueType(const ValueType &other) {
        expr = other.expr;
        defs = other.defs;
    }

    /** Construct a ValueType with a known value. */
    explicit ValueType(uint64_t n, std::string comment="") {
        expr = LeafNode::createInteger(nBits, n, comment);
    }

    /** Construct a ValueType from a TreeNode. */
    explicit ValueType(const TreeNodePtr &node) {
        ASSERT_require(node->nBits()==nBits);
        expr = node;
    }

    virtual ~ValueType() {}

    /** Adds instructions to the list of defining instructions.  Adds the specified instruction and defining sets
     *  into this value and returns a reference to this value.  This is a convenience function used internally by
     *  the policy's X86InstructionSemantics callback methods. See also add_defining_instructions().
     * @{ */
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

    /** Print the value.
     * @{ */
    virtual void print(std::ostream &o) const {
        Formatter fmt;
        print(o, fmt);
    }
    virtual void print(std::ostream &o, BaseSemantics::Formatter &fmt_) const {
        Formatter fmt_dflt;
        Formatter *fmt_ptr = dynamic_cast<Formatter*>(&fmt_);
        if (!fmt_ptr)
            fmt_ptr = &fmt_dflt;
        if (fmt_ptr->show_defs) {
            o <<"defs={";
            size_t ndefs=0;
            for (InsnSet::const_iterator di=defs.begin(); di!=defs.end(); ++di, ++ndefs) {
                SgAsmInstruction *insn = *di;
                if (insn!=NULL)
                    o <<(ndefs>0?",":"") <<StringUtility::addrToString(insn->get_address());
            }
            o <<"} expr=";
        }
        expr->print(o, fmt_ptr->expr_formatter);
    }
    friend std::ostream& operator<<(std::ostream &o, const ValueType &e) {
        e.print(o);
        return o;
    }
    /** @} */

    /** Returns true if the value is a known constant. */
    virtual bool is_known() const {
        return expr->isNumber();
    }

    /** Returns the value of a known constant. Assumes this value is a known constant. */
    virtual uint64_t known_value() const {
        LeafNodePtr leaf = expr->isLeafNode();
        ASSERT_not_null(leaf);
        return leaf->toInt();
    }

    /** Returns the expression stored in this value.  Expressions are reference counted; the reference count of the
     *  returned expression is not incremented. */
    virtual const TreeNodePtr& get_expression() const {
        return expr;
    }

    /** Changes the expression stored in the value.
     * @{ */
    virtual void set_expression(const TreeNodePtr &new_expr) {
        expr = new_expr;
    }
    virtual void set_expression(const ValueType &source) {
        set_expression(source.get_expression());
    }
    /** @} */

    /** Returns the set of instructions that defined this value.  The return value is a flattened lattice
     *  represented as a set.  When analyzing this basic block starting with an initial default state:
     *
     *  @code
     *  1: mov eax, 2
     *  2: add eax, 1
     *  3: mov ebx, eax;
     *  4: mov ebx, 3
     *  @endcode
     *
     *  the defining set for EAX will be instructions {1, 2} and the defining set for EBX will be {4}.  Defining
     *  sets for other registers are the empty set. */
    virtual const InsnSet& get_defining_instructions() const {
        return defs;
    }

    /** Adds definitions to the list of defining instructions. Returns the number of items added that weren't
     *  already in the list of defining instructions.
     * @{ */
    virtual size_t add_defining_instructions(const InsnSet &to_add) {
        size_t nadded = 0;
        for (InsnSet::const_iterator i=to_add.begin(); i!=to_add.end(); ++i) {
            std::pair<InsnSet::iterator, bool> inserted = defs.insert(*i);
            if (inserted.second)
                ++nadded;
        }
        return nadded;
    }
    virtual size_t add_defining_instructions(const ValueType &source) {
        return add_defining_instructions(source.get_defining_instructions());
    }
    virtual size_t add_defining_instructions(SgAsmInstruction *insn) {
        InsnSet tmp;
        if (insn)
            tmp.insert(insn);
        return add_defining_instructions(tmp);
    }
    /** @} */

    /** Set defining instructions.  This discards the old set of defining instructions and replaces it with the
     *  specified set.
     * @{ */
    virtual void set_defining_instructions(const InsnSet &new_defs) {
        defs = new_defs;
    }
    virtual void set_defining_instructions(const ValueType &source) {
        set_defining_instructions(source.get_defining_instructions());
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
 *                          MemoryCell
 ******************************************************************************************************************/

/** Memory cell with symbolic address and data.
 *
 *  The ValueType template argument should be a subclass of SymbolicSemantics::ValueType. */
template<template<size_t> class ValueType=SymbolicSemantics::ValueType>
class MemoryCell {
private:
    ValueType<32> address_;                 /**< Memory address expression. */
    ValueType<8> value_;                    /**< Byte value expression stored at the address. */
    bool written;                           /**< Was the cell created by a write operation? */

public:
    MemoryCell(const ValueType<32> &address, const ValueType<8> &value, SgAsmInstruction *insn=NULL)
        : address_(address), value_(value) {
        value_.add_defining_instructions(insn);
    }

    /** Memory cell address expression.
     * @{ */
    ValueType<32> address() const { return address_; }
    void address(const ValueType<32> &a) { address_ = a; }
    /** @} */

    /** Memory cell value expression.
     * @{ */
    ValueType<8> value() const { return value_; }
    void value(const ValueType<8> &v) { value_ = v; }
    /** @} */

    /** Accessor for whether a cell has been written.  A cell that is written to with writeMemory() should be
     *  marked as such.  This is to make a distinction between cells that have sprung insto existence by virtue of
     *  reading from a previously unknown cell and cells that have been created as a result of a memoryWrite
     *  operation.
     * @{ */
    bool is_written() const { return written; }
    void set_written(bool b=true) { written=b; }
    void clear_written() { written=false; }
    /** @}*/

    /** Returns true if this memory address is the same as the @p other. Note that "same" is more strict than
     *  "overlap".  The @p solver is optional but recommended (absence of a solver will result in a naive
     *  definition). */
    bool must_alias(const ValueType<32> &addr, SMTSolver *solver) const {
        return this->address().get_expression()->mustEqual(addr.get_expression(), solver);
    }

    /** Returns true if address can refer to this memory cell. */
    bool may_alias(const ValueType<32> &addr, SMTSolver *solver) const {
        if (must_alias(addr, solver))
            return true;
        if (!solver)
            return false;
        TreeNodePtr x_addr = this->address().get_expression();
        TreeNodePtr y_addr = addr.get_expression();
        TreeNodePtr assertion = InternalNode::create(1, SymbolicExpr::OP_EQ, x_addr, y_addr);
        return SMTSolver::SAT_NO != solver->satisfiable(assertion);
    }

    /** Print a memory cell.
     *  @{ */
    void print(std::ostream &o) const {
        BaseSemantics::Formatter fmt;
        print(o, fmt);
    }
    void print(std::ostream &o, BaseSemantics::Formatter &fmt) const {
        o <<fmt.get_line_prefix() <<"address = { ";
        address().print(o, fmt);
        o <<" }\n";

        o <<fmt.get_line_prefix() <<"  value = { ";
        value().print(o, fmt);
        o <<" }\n";

        o <<fmt.get_line_prefix() <<"  flags = {";
        if (!written) o <<" rdonly";
        o <<" }\n";
    }

    friend std::ostream& operator<<(std::ostream &o, const MemoryCell &mc) {
        mc.print(o);
        return o;
    }
    /** @} */
};

/******************************************************************************************************************
 *                          MemoryState
 ******************************************************************************************************************/

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
template<template <size_t> class ValueType=SymbolicSemantics::ValueType>
class MemoryState {
public:
    typedef std::list<MemoryCell<ValueType> > CellList;
    CellList cell_list;
    bool read_pruning;                      /**< Prune McCarthy expression for read operations. */

    MemoryState(): read_pruning(true) {}

    /** Enables or disables pruning of the McCarthy expression for read operations.
     * @{ */
    bool get_read_pruning() const { return read_pruning; }
    void enable_read_pruning(bool b=true) { read_pruning = b; }
    void disable_read_pruning() { read_pruning = false; }
    /** @} */

    /** Clear all memory */
    void clear() { cell_list.clear(); }

    /** Write a value to memory. Returns the list of cells that were added. The number of cells added is the same
     *  as the number of bytes in the value being written. */
    template<size_t nBits>
    CellList write(const ValueType<32> &addr, const ValueType<nBits> &value, SMTSolver *solver) {
        ASSERT_require(8==nBits || 16==nBits || 32==nBits);
        CellList retval;
        for (size_t bytenum=0; bytenum<nBits/8; ++bytenum) {
            MemoryCell<ValueType> cell = write_byte(add(addr, bytenum), extract_byte(value, bytenum), solver);
            retval.push_back(cell);
        }
        return retval;
    }

    /** Read a byte from memory.  Returns the list of cells that compose the result.  The cell list can be
     *  converted to a value expression via cells_to_value() method. */
    CellList read_byte(const ValueType<32> &addr, bool *found_must_alias/*out*/, SMTSolver *solver) {
        CellList cells;
        *found_must_alias = false;
        if (read_pruning) {
            for (typename CellList::iterator cli=may_alias(addr, solver, cell_list.begin());
                 cli!=cell_list.end();
                 cli=may_alias(addr, solver, ++cli)) {
                cells.push_back(*cli);
                if (cli->must_alias(addr, solver)) {
                    *found_must_alias = true;
                    break;
                }
            }
        } else {
            for (typename CellList::iterator cli=cell_list.begin(); cli!=cell_list.end(); ++cli) {
                cells.push_back(*cli);
                if (cli->must_alias(addr, solver))
                    *found_must_alias = true;
            }
        }
        return cells;
    }

    /** Build a value from a list of memory cells. */
    ValueType<8> value_from_cells(const ValueType<32> &addr, const CellList &cells) {
        ASSERT_require(!cells.empty());
        if (1==cells.size())
            return cells.front().value();
        // FIXME: This makes no attempt to remove duplicate values
        TreeNodePtr expr = LeafNode::createMemory(addr.get_expression()->nBits(), 8);
        for (typename CellList::const_iterator ci=cells.begin(); ci!=cells.end(); ++ci) {
            expr = InternalNode::create(8, SymbolicExpr::OP_WRITE,
                                        expr, ci->address().get_expression(), ci->value().get_expression());
        }
        ValueType<8> retval(InternalNode::create(8, SymbolicExpr::OP_READ, expr, addr.get_expression()));
        for (typename CellList::const_iterator ci=cells.begin(); ci!=cells.end(); ++ci)
            retval.add_defining_instructions(ci->value().get_defining_instructions());
        return retval;
    }

    /** Write a single byte to memory. */
    MemoryCell<ValueType>& write_byte(const ValueType<32> &addr, const ValueType<8> &value, SMTSolver *solver) {
        typename CellList::iterator cli = must_alias(addr, solver, cell_list.begin());
        if (cli!=cell_list.end())
            cell_list.erase(cli);
        MemoryCell<ValueType> new_cell(addr, value);
        new_cell.set_written();
        cell_list.push_front(new_cell);
        return cell_list.front();
    }

    /** Extract one byte from a 16 or 32 bit value. Byte zero is the little-endian byte. */
    template<size_t nBits>
    ValueType<8> extract_byte(const ValueType<nBits> &a, size_t bytenum) {
        ValueType<8> retval;
        if (a.is_known()) {
            retval = ValueType<8>((a.known_value()>>(bytenum*8)) & IntegerOps::GenMask<uint64_t, 8>::value);
        } else {
            retval = ValueType<8>(InternalNode::create(8, SymbolicExpr::OP_EXTRACT,
                                                       LeafNode::createInteger(32, 8*bytenum),
                                                       LeafNode::createInteger(32, 8*bytenum+8),
                                                       a.get_expression()));
        }
        retval.defined_by(NULL, a.get_defining_instructions());
        return retval;
    }

    /** Add a constant to an address. */
    ValueType<32> add(const ValueType<32> &a, uint64_t n) {
        ValueType<32> retval;
        if (0==n)
            return a; // current instruction doesn't contribute anything to the value
        if (a.is_known()) {
            retval = ValueType<32>(a.known_value()+n);
        } else {
            retval = ValueType<32>(InternalNode::create(32, SymbolicExpr::OP_ADD,
                                                        a.get_expression(), LeafNode::createInteger(32, n)));
        }
        retval.set_defining_instructions(a.get_defining_instructions());
        return retval;
    }

    /** Returns the first memory cell that must be aliased by @p addr. */
    typename CellList::iterator must_alias(const ValueType<32> &addr, SMTSolver *solver,
                                           typename CellList::iterator begin) {
        for (typename CellList::iterator cli=begin; cli!=cell_list.end(); ++cli) {
            if (cli->must_alias(addr, solver))
                return cli;
        }
        return cell_list.end();
    }

    /** Returns the first memory cell that might be aliased by @p addr. */
    typename CellList::iterator may_alias(const ValueType<32> &addr, SMTSolver *solver,
                                          typename CellList::iterator begin) {
        for (typename CellList::iterator cli=begin; cli!=cell_list.end(); ++cli) {
            if (cli->may_alias(addr, solver))
                return cli;
        }
        return cell_list.end();
    }

    /** Print values of all memory.
     * @{ */
    void print(std::ostream &o) {
        BaseSemantics::Formatter fmt;
        print(o, fmt);
    }
    void print(std::ostream &o, BaseSemantics::Formatter &fmt) const {
        for (typename CellList::const_iterator cli=cell_list.begin(); cli!=cell_list.end(); ++cli)
            cli->print(o, fmt);
    }
    /** @} */
};

/******************************************************************************************************************
 *                          RegisterStateX86
 ******************************************************************************************************************/

/** X86 register state.
 *
 *  The set of registers and their values used by the instruction semantics. */
template <template <size_t> class ValueType=SymbolicSemantics::ValueType>
class RegisterStateX86: public BaseSemantics::RegisterStateX86<ValueType> {};

/******************************************************************************************************************
 *                          State
 ******************************************************************************************************************/

/** Entire machine state.
 *
 *  The state holds the set of registers, their values, and the list of all memory locations. */
template <template <size_t> class ValueType=SymbolicSemantics::ValueType>
class State {
public:
    typedef RegisterStateX86<ValueType> Registers;
    typedef MemoryState<ValueType> Memory;

    Registers registers;
    Memory memory;

    /** Print info about how registers differ.
     * @{ */
    void print_diff_registers(std::ostream &o, const State &state) const {
        BaseSemantics::Formatter fmt;
        print(o, state, fmt);
    }
    void print_diff_registers(std::ostream &o, const State&, BaseSemantics::Formatter &fmt) const;
    /** @} */

    /** Tests registers of two states for equality. */
    bool equal_registers(const State&) const;

    /** Removes from memory those values at addresses below the current stack pointer. This is automatically called
     *  after each instruction if the policy's p_discard_popped_memory property is set. */
    void discard_popped_memory() {
        /*FIXME: not implemented yet. [RPM 2010-05-24]*/
    }

    friend std::ostream& operator <<(std::ostream &o, const State &state) {
        state.print(o);
        return o;
    }

#if 1   /* These won't be needed once we can start inheriting from BaseSemantics::StateX86 again. [RPM 2012-07-03] */
    void clear() {
        registers.clear();
        memory.clear();
    }

    void zero_registers() {
        registers.zero();
    }

    void clear_memory() {
        memory.clear();
    }

    void print(std::ostream &o) const {
        BaseSemantics::Formatter fmt;
        print(o, fmt);
    }
    void print(std::ostream &o, BaseSemantics::Formatter &fmt) const {
        std::string prefix = fmt.get_line_prefix();
        BaseSemantics::Indent indent(fmt);
        o <<prefix <<"registers:\n";
        registers.print(o, fmt);
        o <<prefix <<"memory:\n";
        memory.print(o, fmt);
    }
#endif
};

/******************************************************************************************************************
 *                          Policy
 ******************************************************************************************************************/

/** A policy that is supplied to the semantic analysis constructor. See documentation for the SymbolicSemantics
 *  namespace.  The RISC-like operations are documented in the
 *  rose::BinaryAnalysis::InstructionSemantics::NullSemantics::Policy class. */
template <
    template <template <size_t> class ValueType> class State = SymbolicSemantics::State,
    template <size_t> class ValueType = SymbolicSemantics::ValueType
    >
class Policy: public BaseSemantics::Policy {
protected:
    typedef typename State<ValueType>::Memory Memory;

    SgAsmInstruction *cur_insn;         /**< Set by startInstruction(), cleared by finishInstruction() */
    mutable State<ValueType> orig_state;/**< Original machine state, initialized by constructor and mem_write. This
                                         *   data member is mutable because a mem_read() operation, although
                                         *   conceptually const, may cache the value that was read so that
                                         *   subsquent reads from the same address will return the same value. This
                                         *   member is initialized by the first call to startInstruction() (as
                                         *   called by X86InstructionSemantics::processInstruction()) which allows
                                         *   the user to initialize the original conditions using the same
                                         *   interface that's used to process instructions.  In other words, if one
                                         *   wants the stack pointer to contain a specific original value, then one
                                         *   may initialize the stack pointer by calling writeGPR() before
                                         *   processing the first instruction. */
    mutable State<ValueType> cur_state;/**< Current machine state updated by each processInstruction().  The
                                         *   instruction pointer is updated before we process each
                                         *   instruction. This data member is mutable because a mem_read()
                                         *   operation, although conceptually const, may cache the value that was
                                         *   read so that subsequent reads from the same address will return the
                                         *   same value. */
    bool p_discard_popped_memory;       /**< Property that determines how the stack behaves.  When set, any time
                                         *   the stack pointer is adjusted, memory below the stack pointer and having
                                         *   the same address name as the stack pointer is removed (the memory
                                         *   location becomes undefined). The default is false, that is, no special
                                         *   treatment for the stack. */
    size_t ninsns;                      /**< Total number of instructions processed. This is incremented by
                                         *   startInstruction(), which is the first thing called by
                                         *   X86InstructionSemantics::processInstruction(). */
    SMTSolver *solver;                  /**< The solver to use for Satisfiability Modulo Theory, or NULL. */

public:

    /** Constructs a new policy without an SMT solver. */
    Policy() {
        init();
        /* So that named values are identical in both; reinitialized by first call to startInstruction(). */
        orig_state = cur_state;
    }

    /** Constructs a new policy with an SMT solver. */
    Policy(SMTSolver *solver) {
        init();
        this->solver = solver;
        /* So that named values are identical in both; reinitialized by first call to startInstruction(). */
        orig_state = cur_state;
    }

    /** Initialize undefined policy. Used by constructors so initialization is in one location. */
    void init() {
        set_register_dictionary(RegisterDictionary::dictionary_pentium4());
        cur_insn = NULL;
        p_discard_popped_memory = false;
        ninsns = 0;
        solver = NULL;
    }

    /** Sets the satisfiability modulo theory (SMT) solver to use for certain operations. */
    void set_solver(SMTSolver *s) { solver = s; }

    /** Returns the solver that is currently being used. */
    SMTSolver *get_solver() const { return solver; }

    /** Returns the current state. */
    const State<ValueType>& get_state() const { return cur_state; }
    State<ValueType>& get_state() { return cur_state; }

    /** Returns the original state.  The original state is initialized to be equal to the current state twice: once
     *  by the constructor, and then again when the first instruction is processed. */
    const State<ValueType>& get_orig_state() const { return orig_state; }
    State<ValueType>& get_orig_state() { return orig_state; }

    /** Enables or disables pruning of the McCarthy expression for read operations on the current and initial
     *  memory states.  This property can also be set on each state individually, which is all this method does
     *  anyway.
     * @{ */
    void enable_read_pruning(bool b=true) {
        cur_state.memory.enable_read_pruning(b);
        orig_state.memory.enable_read_pruning(b);
    }
    void disable_read_pruning() { enable_read_pruning(false); }
    /** @} */

    /** Returns the current instruction pointer. */
    const ValueType<32>& get_ip() const { return cur_state.registers.ip; }

    /** Returns the original instruction pointer. See also get_orig_state(). */
    const ValueType<32>& get_orig_ip() const { return orig_state.registers.ip; }

    /** Returns a copy of the state after removing memory that is not pertinent to an equal_states() comparison. */
    Memory memory_for_equality(const State<ValueType>&) const;

    /** Returns a copy of the current state after removing memory that is not pertinent to an equal_states()
     *  comparison. */
    Memory memory_for_equality() const { return memory_for_equality(cur_state); }

    /** Compares two states for equality. The comarison looks at all register values and the memory locations that
     *  are different than their original value (but excluding differences due to clobbering). It does not compare
     *  memory that has only been read. */
    bool equal_states(const State<ValueType>&, const State<ValueType>&) const;

    /** Print the current state of this policy.  If a print helper is specified then it will be passed along to the
     *  SymbolicExpr::print() method.
     * @{ */
    void print(std::ostream &o) const {
        BaseSemantics::Formatter fmt;
        print(o, fmt);
    }
    void print(std::ostream &o, BaseSemantics::Formatter &fmt) const {
        std::string prefix = fmt.get_line_prefix();
        BaseSemantics::Indent indent(fmt);
        o <<prefix <<"registers:\n";
        cur_state.registers.print(o, fmt);
        o <<prefix <<"memory:\n";
        cur_state.memory.print(o, fmt);
        o <<prefix <<"init mem:\n";
        orig_state.memory.print(o, fmt);
    }
    friend std::ostream& operator<<(std::ostream &o, const Policy &p) {
        p.print(o);
        return o;
    }
    /** @} */

    /** Returns true if the specified value exists in memory and is provably at or above the stack pointer.  The
     *  stack pointer need not have a known value. */
    bool on_stack(const ValueType<32> &value) const;

    /** Changes how the policy treats the stack.  See the p_discard_popped_memory property data member for
     *  details. */
    void set_discard_popped_memory(bool b) {
        p_discard_popped_memory = b;
    }

    /** Returns the current setting for the property that determines how the stack behaves. See the
     *  p_set_discard_popped_memory property data member for details. */
    bool get_discard_popped_memory() const {
        return p_discard_popped_memory;
    }

    /** Print only the differences between two states.
     * @{ */
    void print_diff(std::ostream &o, const State<ValueType> &s1, const State<ValueType> &s2) const {
        BaseSemantics::Formatter fmt;
        print(o, s1, s2, fmt);
    }
    void print_diff(std::ostream&, const State<ValueType>&, const State<ValueType>&,
                    BaseSemantics::Formatter &fmt) const;
    /** @} */


    /** Print the difference between the current state and the initial state.
     * @{ */
    void print_diff(std::ostream &o) const {
        print_diff(o, orig_state, cur_state);
    }

    void print_diff(std::ostream &o, const BaseSemantics::Formatter &fmt = BaseSemantics::Formatter()) const {
        print_diff(o, orig_state, cur_state, fmt);
    }
    /** @} */

    /** Returns the SHA1 hash of the difference between the current state and the original state.  If libgcrypt is
     *  not available then the return value will be an empty string. */
    std::string SHA1() const;

    /** Extend (or shrink) from @p FromLen bits to @p ToLen bits by adding or removing high-order bits from the
     *  input. Added bits are always zeros. */
    template <size_t FromLen, size_t ToLen>
    ValueType<ToLen> unsignedExtend(const ValueType<FromLen> &a) const {
        ValueType<ToLen> retval;
        if (a.is_known()) {
            retval = ValueType<ToLen>(IntegerOps::GenMask<uint64_t,ToLen>::value & a.known_value());
            retval.defined_by(FromLen==ToLen?NULL:cur_insn, a.get_defining_instructions());
        } else if (FromLen==ToLen) {
            // no-op, so not defined by current insn
            retval = ValueType<ToLen>(a.get_expression());
            retval.defined_by(NULL, a.get_defining_instructions());
        } else if (FromLen>ToLen) {
            retval = ValueType<ToLen>(InternalNode::create(ToLen, SymbolicExpr::OP_EXTRACT,
                                                           LeafNode::createInteger(32, 0),
                                                           LeafNode::createInteger(32, ToLen),
                                                           a.get_expression()));
            retval.defined_by(cur_insn, a.get_defining_instructions());
        } else {
            retval = ValueType<ToLen>(InternalNode::create(ToLen, SymbolicExpr::OP_UEXTEND,
                                                           LeafNode::createInteger(32, ToLen),
                                                           a.get_expression()));
            retval.defined_by(cur_insn, a.get_defining_instructions());
        }
        return retval;
    }

    /** Sign extend from @p FromLen bits to @p ToLen bits. */
    template <size_t FromLen, size_t ToLen>
    ValueType<ToLen> signedExtend(const ValueType<FromLen> &a) const {
        ValueType<ToLen> retval;
        if (a.is_known()) {
            retval = ValueType<ToLen>(IntegerOps::signExtend<FromLen, ToLen>(a.known_value()));
            retval.defined_by(FromLen==ToLen?NULL:cur_insn, a.get_defining_instructions());
        } else if (FromLen==ToLen) {
            // no-op, so not defined by current insns
            retval = ValueType<ToLen>(a.get_expression());
            retval.defined_by(NULL, a.get_defining_instructions());
        } else if (FromLen > ToLen) {
            retval = ValueType<ToLen>(InternalNode::create(ToLen, SymbolicExpr::OP_EXTRACT,
                                                           LeafNode::createInteger(32, 0),
                                                           LeafNode::createInteger(32, ToLen),
                                                           a.get_expression()));
            retval.defined_by(cur_insn, a.get_defining_instructions());
        } else {
            retval = ValueType<ToLen>(InternalNode::create(ToLen, SymbolicExpr::OP_SEXTEND,
                                                           LeafNode::createInteger(32, ToLen),
                                                           a.get_expression()));
            retval.defined_by(cur_insn, a.get_defining_instructions());
        }
        return retval;
    }

    /** Extracts certain bits from the specified value and shifts them to the low-order positions in the result.
     *  The bits of the result include bits from BeginAt (inclusive) through EndAt (exclusive).  The lsb is
     *  numbered zero. */
    template <size_t BeginAt, size_t EndAt, size_t Len>
    ValueType<EndAt-BeginAt> extract(const ValueType<Len> &a) const {
        ValueType<EndAt-BeginAt> retval;
        if (0==BeginAt) {
            retval = unsignedExtend<Len,EndAt-BeginAt>(a);
        } else if (a.is_known()) {
            retval = ValueType<EndAt-BeginAt>((a.known_value()>>BeginAt) &
                                              IntegerOps::genMask<uint64_t>(EndAt-BeginAt));
            retval.defined_by(cur_insn, a.get_defining_instructions());
        } else {
            retval = ValueType<EndAt-BeginAt>(InternalNode::create(EndAt-BeginAt, SymbolicExpr::OP_EXTRACT,
                                                                   LeafNode::createInteger(32, BeginAt),
                                                                   LeafNode::createInteger(32, EndAt),
                                                                   a.get_expression()));
            retval.defined_by(cur_insn, a.get_defining_instructions());
        }
        return retval;
    }

    /** Reads a single-byte value from memory.
     *
     *  Reads from the specified memory state and updates the original state if appropriate.  Reading from a memory
     *  state might actually create new memory cells in the original state.  The @p dflt is the byte value to save to
     *  the original state when appropriate. */
    ValueType<8> mem_read_byte(State<ValueType> &state, const ValueType<32> &addr, const ValueType<8> &dflt) const {
        typedef typename State<ValueType>::Memory::CellList CellList;
        bool found_must_alias;
        CellList cells = state.memory.read_byte(addr, &found_must_alias, solver);
        if (!found_must_alias) {
            if (&state!=&orig_state) {
                /* We didn't find the exact (must-alias) memory cell in the specified state. See if we can find a
                 * value in the initial state, creating it there if necessary. */
                CellList cells_init = orig_state.memory.read_byte(addr, &found_must_alias, solver);
                cells.insert(cells.end(), cells_init.begin(), cells_init.end());
                if (!found_must_alias) {
                    MemoryCell<ValueType> &new_cell = orig_state.memory.write_byte(addr, dflt, solver);
                    new_cell.clear_written();
                    cells.push_back(new_cell);
                }
            }
        }
        return state.memory.value_from_cells(addr, cells);
    }

    /** Reads a multi-byte value from memory.
     *
     *  Reads a multi-byte, little-endian value from memory and updates the original state if appropriate.  Reading
     *  from a memory state might actually create new memory cells in the original state.  The @p dflt is the value
     *  to save to the original state when appropriate. */
    template <size_t nBits>
    ValueType<nBits> mem_read(State<ValueType> &state, const ValueType<32> &addr,
                              const ValueType<nBits> *dflt_ptr=NULL) const {
        ASSERT_require(8==nBits || 16==nBits || 32==nBits);
        typedef typename State<ValueType>::Memory::CellList CellList;
        ValueType<nBits> dflt = dflt_ptr ? *dflt_ptr : ValueType<nBits>();

        // Read bytes in little endian order.
        ValueType<nBits> retval, defs;
        std::vector<ValueType<8> > bytes; // little endian order
        for (size_t bytenum=0; bytenum<nBits/8; ++bytenum) {
            ValueType<8> dflt_byte = state.memory.extract_byte(dflt, bytenum);
            ValueType<8> byte = mem_read_byte(state, state.memory.add(addr, bytenum), dflt_byte);
            defs.defined_by(NULL, byte.get_defining_instructions());
            bytes.push_back(byte);
        }

        // Try to match the pattern of bytes:
        //    (extract 0  8 EXPR_0)
        //    (extract 8 16 EXPR_1)
        //    ...
        // where EXPR_i are all structurally identical.
        bool matched = false;
        if (bytes.size()>1) {
            matched = true; // and prove otherwise
            for (size_t bytenum=0; bytenum<bytes.size() && matched; ++bytenum) {
                InternalNodePtr extract = bytes[bytenum].get_expression()->isInteriorNode();
                if (!extract || SymbolicExpr::OP_EXTRACT!=extract->getOperator()) {
                    matched = false;
                    break;
                }
                LeafNodePtr arg0 = extract->child(0)->isLeafNode();
                LeafNodePtr arg1 = extract->child(1)->isLeafNode();
                if (!arg0 || !arg0->isNumber() || arg0->toInt()!=8*bytenum ||
                    !arg1 || !arg1->isNumber() || arg1->toInt()!=8*(bytenum+1)) {
                    matched = false;
                    break;
                }
                if (bytenum>0) {
                    TreeNodePtr e0 = bytes[0      ].get_expression()->isInteriorNode()->child(2);
                    TreeNodePtr ei = bytes[bytenum].get_expression()->isInteriorNode()->child(2);
                    matched = e0->isEquivalentTo(ei);
                }
            }
        }

        // If the bytes match the above pattern, then we can just return (the low order bits of) EXPR_0, otherwise
        // we have to construct a return value by extending and shifting the bytes and bitwise-OR them together.
        if (matched) {
            TreeNodePtr e0 = bytes[0].get_expression()->isInteriorNode()->child(2);
            if (e0->nBits()==nBits) {
                retval = ValueType<nBits>(e0);
            } else {
                ASSERT_require(e0->nBits()>nBits);
                retval = ValueType<nBits>(InternalNode::create(nBits, SymbolicExpr::OP_EXTRACT,
                                                               LeafNode::createInteger(32, 0),
                                                               LeafNode::createInteger(32, nBits),
                                                               e0));
            }
        } else {
            for (size_t bytenum=0; bytenum<bytes.size(); ++bytenum) { // little endian
                ValueType<8> byte = bytes[bytenum];

                // extend byte
                ValueType<nBits> word;
                if (byte.is_known()) {
                    word = ValueType<nBits>(byte.known_value());
                } else if (nBits==8) {
                    word = ValueType<nBits>(byte.get_expression());
                } else {
                    word = ValueType<nBits>(InternalNode::create(nBits, SymbolicExpr::OP_UEXTEND,
                                                                 LeafNode::createInteger(32, nBits),
                                                                 byte.get_expression()));
                }

                // left shift
                if (0!=bytenum) {
                    if (word.is_known()) {
                        word = ValueType<nBits>(word.known_value() << (bytenum*8));
                    } else {
                        word = ValueType<nBits>(InternalNode::create(nBits, SymbolicExpr::OP_SHR0,
                                                                     LeafNode::createInteger(32, bytenum*8),
                                                                     word.get_expression()));
                    }
                }

                // bit-wise OR into the return value
                if (0==bytenum) {
                    retval = word;
                } else if (retval.is_known() && word.is_known()) {
                    retval = ValueType<nBits>(retval.known_value() | word.known_value());
                } else {
                    retval = ValueType<nBits>(InternalNode::create(nBits, SymbolicExpr::OP_BV_OR,
                                                                   retval.get_expression(), word.get_expression()));
                }
            }
        }

        retval.defined_by(NULL, defs.get_defining_instructions());
        return retval;
    }

    /** Writes a value to memory. */
    template <size_t Len>
    void mem_write(State<ValueType> &state, const ValueType<32> &addr, const ValueType<Len> &data) {
        ASSERT_require(&state!=&orig_state);
        typedef typename State<ValueType>::Memory::CellList CellList;
        ValueType<Len> data_with_def(data);
        data_with_def.defined_by(cur_insn);
        state.memory.write(addr, data_with_def, solver);
    }


    /*************************************************************************************************************
     * Functions invoked by the X86InstructionSemantics class for every processed instructions
     *************************************************************************************************************/

    /** See NullSemantics::Policy::startInstruction() */
    void startInstruction(SgAsmInstruction *insn) {
        if (!cur_state.registers.ip.is_known()) {
            // semantics user should have probably initialized EIP
            cur_state.registers.ip = ValueType<32>(insn->get_address());
        } else if (cur_state.registers.ip.known_value()!=insn->get_address()) {
            fprintf(stderr, "SymbolicSemantics::Policy::startInstruction: invalid EIP value for current instruction\n\
startInstruction() is being called for an instruction with a concrete\n\
address stored in the SgAsmX86Instruction object, but the current value of\n\
this policy's EIP register does not match the instruction.  This might\n\
happen if you're processing instructions in an order that's different than\n\
the order the policy thinks they would be executed.  If this is truly your\n\
intent, then you need to set the policy's EIP register to the instruction\n\
address before you translate the instruction--and you might want to make\n\
sure the other state information is also appropriate for this instruction\n\
rather than use the final state from the previously translated\n\
instruction.  x86 \"REP\" instructions might be the culprit: ROSE\n\
instruction semantics treat them as a tiny loop, updating the policy's EIP\n\
depending on whether the loop is to be taken again, or not.\n");
            std::cerr <<"ip = " <<cur_state.registers.ip <<"\n";
            ASSERT_require(cur_state.registers.ip.known_value()==insn->get_address()); // redundant, used for error mesg
            abort(); // we must fail even when optimized
        }
        if (0==ninsns++)
            orig_state = cur_state;
        cur_insn = insn;
    }

    /** See NullSemantics::Policy::finishInstruction() */
    void finishInstruction(SgAsmInstruction*) {
        if (p_discard_popped_memory)
            cur_state.discard_popped_memory();
        cur_insn = NULL;
    }



    /*****************************************************************************************************************
     * Functions invoked by the X86InstructionSemantics class to construct values
     *****************************************************************************************************************/

    /** See NullSemantics::Policy::true_() */
    ValueType<1> true_() const {
        ValueType<1> retval(1);
        retval.defined_by(cur_insn);
        return retval;
    }

    /** See NullSemantics::Policy::false_() */
    ValueType<1> false_() const {
        ValueType<1> retval((uint64_t)0);
        retval.defined_by(cur_insn);
        return retval;
    }

    /** See NullSemantics::Policy::undefined_() */
    template <size_t Len>
    ValueType<Len> undefined_() const {
        ValueType<Len> retval;
        retval.defined_by(cur_insn);
        return retval;
    }

    /** See NullSemantics::Policy::unspecified_() */
    template <size_t Len>
    ValueType<Len> unspecified_() const {
        ValueType<Len> retval;
        retval.defined_by(cur_insn);
        return retval;
    }

    /** See NullSemantics::Policy::number() */
    template <size_t Len>
    ValueType<Len> number(uint64_t n) const {
        ValueType<Len> retval(n);
        retval.defined_by(cur_insn);
        return retval;
    }



    /*****************************************************************************************************************
     * Functions invoked by the X86InstructionSemantics class for individual instructions
     *****************************************************************************************************************/

    /** See NullSemantics::Policy::filterCallTarget() */
    ValueType<32> filterCallTarget(const ValueType<32> &a) const {
        return a;
    }

    /** See NullSemantics::Policy::filterReturnTarget() */
    ValueType<32> filterReturnTarget(const ValueType<32> &a) const {
        return a;
    }

    /** See NullSemantics::Policy::filterIndirectJumpTarget() */
    ValueType<32> filterIndirectJumpTarget(const ValueType<32> &a) const {
        return a;
    }

    /** See NullSemantics::Policy::hlt() */
    void hlt() {} // FIXME

    /** See NullSemantics::Policy::cpuid() */
    void cpuid() {} // FIXME

    /** See NullSemantics::Policy::rdtsc() */
    ValueType<64> rdtsc() {
        return ValueType<64>((uint64_t)0);
    }

    /** See NullSemantics::Policy::interrupt() */
    void interrupt(uint8_t num) {
        cur_state = State<ValueType>(); /*reset entire machine state*/
    }

    /** See NullSemantics::Policy::sysenter() */
    void sysenter() {
        cur_state = State<ValueType>(); /*reset entire machine state*/
    }



    /*****************************************************************************************************************
     * Functions invoked by the X86InstructionSemantics class for arithmetic operations
     *****************************************************************************************************************/

    /** See NullSemantics::Policy::add() */
    template <size_t Len>
    ValueType<Len> add(const ValueType<Len> &a, const ValueType<Len> &b) const {
        ValueType<Len> retval;
        if (a.is_known()) {
            if (b.is_known()) {
                retval = ValueType<Len>(LeafNode::createInteger(Len, a.known_value()+b.known_value()));
                retval.defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
                return retval;
            } else if (0==a.known_value()) {
                return b;
            }
        } else if (b.is_known() && 0==b.known_value()) {
            return a;
        }
        retval = ValueType<Len>(InternalNode::create(Len, SymbolicExpr::OP_ADD,
                                                     a.get_expression(),
                                                     b.get_expression()));
        retval.defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
        return retval;
    }

    /** See NullSemantics::Policy::addWithCarries() */
    template <size_t Len>
    ValueType<Len> addWithCarries(const ValueType<Len> &a, const ValueType<Len> &b, const ValueType<1> &c,
                                  ValueType<Len> &carry_out) const {
        ValueType<Len+1> aa = unsignedExtend<Len, Len+1>(a);
        ValueType<Len+1> bb = unsignedExtend<Len, Len+1>(b);
        ValueType<Len+1> cc = unsignedExtend<1,   Len+1>(c);
        ValueType<Len+1> sumco = add<Len+1>(aa, add<Len+1>(bb, cc));
        carry_out = extract<1, Len+1>(xor_<Len+1>(aa, xor_<Len+1>(bb, sumco)));
        return add<Len>(a, add<Len>(b, unsignedExtend<1, Len>(c)));
    }

    /** See NullSemantics::Policy::and_() */
    template <size_t Len>
    ValueType<Len> and_(const ValueType<Len> &a, const ValueType<Len> &b) const {
        ValueType<Len> retval;
        if (a.is_known() && b.is_known()) {
            retval = ValueType<Len>(a.known_value() & b.known_value());
        } else {
            retval = ValueType<Len>(InternalNode::create(Len, SymbolicExpr::OP_BV_AND,
                                                         a.get_expression(), b.get_expression()));
        }
        retval.defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
        return retval;
    }

    /** See NullSemantics::Policy::equalToZero() */
    template <size_t Len>
    ValueType<1> equalToZero(const ValueType<Len> &a) const {
        ValueType<1> retval;
        if (a.is_known()) {
            retval = a.known_value() ? false_() : true_();
        } else {
            retval = ValueType<1>(InternalNode::create(1, SymbolicExpr::OP_ZEROP, a.get_expression()));
        }
        retval.defined_by(cur_insn, a.get_defining_instructions());
        return retval;
    }

    /** See NullSemantics::Policy::invert() */
    template <size_t Len>
    ValueType<Len> invert(const ValueType<Len> &a) const {
        ValueType<Len> retval;
        if (a.is_known()) {
            retval = ValueType<Len>(LeafNode::createInteger(Len, ~a.known_value()));
        } else {
            retval = ValueType<Len>(InternalNode::create(Len, SymbolicExpr::OP_INVERT, a.get_expression()));
        }
        retval.defined_by(cur_insn, a.get_defining_instructions());
        return retval;
    }

    /** See NullSemantics::Policy::concat() */
    template<size_t Len1, size_t Len2>
    ValueType<Len1+Len2> concat(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
        ValueType<Len1+Len2> retval;
        if (a.is_known() && b.is_known()) {
            retval = ValueType<Len1+Len2>(a.known_value() | (b.known_value() << Len1));
        } else {
            retval = ValueType<Len1+Len2>(InternalNode::create(Len1+Len2, SymbolicExpr::OP_CONCAT,
                                                               b.get_expression(), a.get_expression()));
        }
        retval.defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
        return retval;
    }

    /** See NullSemantics::Policy::ite() */
    template <size_t Len>
    ValueType<Len> ite(const ValueType<1> &sel, const ValueType<Len> &ifTrue, const ValueType<Len> &ifFalse) const {
        ValueType<Len> retval;
        if (sel.is_known()) {
            retval = sel.known_value() ? ifTrue : ifFalse;
            retval.defined_by(cur_insn, sel.get_defining_instructions());
            return retval;
        }
        if (solver) {
            /* If the selection expression cannot be true, then return ifFalse */
            TreeNodePtr assertion = InternalNode::create(1, SymbolicExpr::OP_EQ,
                                                         sel.get_expression(),
                                                         LeafNode::createInteger(1, 1));
            bool can_be_true = SMTSolver::SAT_NO != solver->satisfiable(assertion);
            if (!can_be_true) {
                ValueType<Len> retval = ifFalse;
                retval.defined_by(cur_insn, sel.get_defining_instructions());
                return retval;
            }

            /* If the selection expression cannot be false, then return ifTrue */
            assertion = InternalNode::create(1, SymbolicExpr::OP_EQ,
                                             sel.get_expression(), LeafNode::createInteger(1, 0));
            bool can_be_false = SMTSolver::SAT_NO != solver->satisfiable(assertion);
            if (!can_be_false) {
                ValueType<Len> retval = ifTrue;
                retval.defined_by(cur_insn, sel.get_defining_instructions());
                return retval;
            }
        }
        retval = ValueType<Len>(InternalNode::create(Len, SymbolicExpr::OP_ITE, sel.get_expression(),
                                                     ifTrue.get_expression(), ifFalse.get_expression()));
        retval.defined_by(cur_insn, sel.get_defining_instructions(),
                          ifTrue.get_defining_instructions(), ifFalse.get_defining_instructions());
        return retval;
    }

    /** See NullSemantics::Policy::leastSignificantSetBit() */
    template <size_t Len>
    ValueType<Len> leastSignificantSetBit(const ValueType<Len> &a) const {
        ValueType<Len> retval;
        if (a.is_known()) {
            uint64_t n = a.known_value();
            for (size_t i=0; i<Len; ++i) {
                if (n & ((uint64_t)1 << i)) {
                    retval = number<Len>(i);
                    retval.defined_by(cur_insn, a.get_defining_instructions());
                    return retval;
                }
            }
            retval = number<Len>(0);
            retval.defined_by(cur_insn, a.get_defining_instructions());
            return retval;
        }
        retval = ValueType<Len>(InternalNode::create(Len, SymbolicExpr::OP_LSSB, a.get_expression()));
        retval.defined_by(cur_insn, a.get_defining_instructions());
        return retval;
    }

    /** See NullSemantics::Policy::mostSignificantSetBit() */
    template <size_t Len>
    ValueType<Len> mostSignificantSetBit(const ValueType<Len> &a) const {
        ValueType<Len> retval;
        if (a.is_known()) {
            uint64_t n = a.known_value();
            for (size_t i=Len; i>0; --i) {
                if (n & ((uint64_t)1 << (i-1))) {
                    retval = number<Len>(i-1);
                    retval.defined_by(cur_insn, a.get_defining_instructions());
                    return retval;
                }
            }
            retval = number<Len>(0);
            retval.defined_by(cur_insn, a.get_defining_instructions());
            return retval;
        }
        retval = ValueType<Len>(InternalNode::create(Len, SymbolicExpr::OP_MSSB, a.get_expression()));
        retval.defined_by(cur_insn, a.get_defining_instructions());
        return retval;
    }

    /** See NullSemantics::Policy::negate() */
    template <size_t Len>
    ValueType<Len> negate(const ValueType<Len> &a) const {
        ValueType<Len> retval;
        if (a.is_known()) {
            retval = ValueType<Len>(-a.known_value());
        } else {
            retval = ValueType<Len>(InternalNode::create(Len, SymbolicExpr::OP_NEGATE, a.get_expression()));
        }
        retval.defined_by(cur_insn, a.get_defining_instructions());
        return retval;
    }

    /** See NullSemantics::Policy::or_() */
    template <size_t Len>
    ValueType<Len> or_(const ValueType<Len> &a, const ValueType<Len> &b) const {
        ValueType<Len> retval;
        if (a.is_known() && b.is_known()) {
            retval = ValueType<Len>(a.known_value() | b.known_value());
        } else {
            retval = ValueType<Len>(InternalNode::create(Len, SymbolicExpr::OP_BV_OR,
                                                         a.get_expression(), b.get_expression()));
        }
        retval.defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
        return retval;
    }

    /** See NullSemantics::Policy::rotateLeft() */
    template <size_t Len, size_t SALen>
    ValueType<Len> rotateLeft(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
        ValueType<Len> retval;
        if (a.is_known() && sa.is_known()) {
            retval = ValueType<Len>(IntegerOps::rotateLeft<Len>(a.known_value(), sa.known_value()));
        } else {
            retval = ValueType<Len>(InternalNode::create(Len, SymbolicExpr::OP_ROL,
                                                         sa.get_expression(), a.get_expression()));
        }
        retval.defined_by(cur_insn, a.get_defining_instructions(), sa.get_defining_instructions());
        return retval;
    }

    /** See NullSemantics::Policy::rotateRight() */
    template <size_t Len, size_t SALen>
    ValueType<Len> rotateRight(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
        ValueType<Len> retval;
        if (a.is_known() && sa.is_known()) {
            retval = ValueType<Len>(IntegerOps::rotateRight<Len>(a.known_value(), sa.known_value()));
        } else {
            retval = ValueType<Len>(InternalNode::create(Len, SymbolicExpr::OP_ROR,
                                                         sa.get_expression(), a.get_expression()));
        }
        retval.defined_by(cur_insn, a.get_defining_instructions(), sa.get_defining_instructions());
        return retval;
    }

    /** See NullSemantics::Policy::shiftLeft() */
    template <size_t Len, size_t SALen>
    ValueType<Len> shiftLeft(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
        ValueType<Len> retval;
        if (a.is_known() && sa.is_known()) {
            retval = ValueType<Len>(IntegerOps::shiftLeft<Len>(a.known_value(), sa.known_value()));
        } else {
            retval = ValueType<Len>(InternalNode::create(Len, SymbolicExpr::OP_SHL0,
                                                         sa.get_expression(), a.get_expression()));
        }
        retval.defined_by(cur_insn, a.get_defining_instructions(), sa.get_defining_instructions());
        return retval;
    }

    /** See NullSemantics::Policy::shiftRight() */
    template <size_t Len, size_t SALen>
    ValueType<Len> shiftRight(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
        ValueType<Len> retval;
        if (a.is_known() && sa.is_known()) {
            retval = ValueType<Len>(IntegerOps::shiftRightLogical<Len>(a.known_value(), sa.known_value()));
        } else {
            retval = ValueType<Len>(InternalNode::create(Len, SymbolicExpr::OP_SHR0,
                                                         sa.get_expression(), a.get_expression()));
        }
        retval.defined_by(cur_insn, a.get_defining_instructions(), sa.get_defining_instructions());
        return retval;
    }

    /** See NullSemantics::Policy::shiftRightArithmetic() */
    template <size_t Len, size_t SALen>
    ValueType<Len> shiftRightArithmetic(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
        ValueType<Len> retval;
        if (a.is_known() && sa.is_known()) {
            retval = ValueType<Len>(IntegerOps::shiftRightArithmetic<Len>(a.known_value(), sa.known_value()));
        } else {
            retval = ValueType<Len>(InternalNode::create(Len, SymbolicExpr::OP_ASR,
                                                         sa.get_expression(), a.get_expression()));
        }
        retval.defined_by(cur_insn, a.get_defining_instructions(), sa.get_defining_instructions());
        return retval;
    }

    /** See NullSemantics::Policy::signExtend() */
    template <size_t From, size_t To>
    ValueType<To> signExtend(const ValueType<From> &a) {
        return signedExtend<From, To>(a);
    }

    /** See NullSemantics::Policy::signedDivide() */
    template <size_t Len1, size_t Len2>
    ValueType<Len1> signedDivide(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
        ValueType<Len1> retval;
        if (a.is_known() && b.is_known() && 0!=b.known_value()) {
            retval = ValueType<Len1>(IntegerOps::signExtend<Len1, 64>(a.known_value()) /
                                     IntegerOps::signExtend<Len2, 64>(b.known_value()));
        } else {
            retval = ValueType<Len1>(InternalNode::create(Len1, SymbolicExpr::OP_SDIV,
                                                          a.get_expression(), b.get_expression()));
        }
        retval.defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
        return retval;
    }

    /** See NullSemantics::Policy::signedModulo() */
    template <size_t Len1, size_t Len2>
    ValueType<Len2> signedModulo(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
        ValueType<Len2> retval;
        if (a.is_known() && b.is_known() && 0!=b.known_value()) {
            retval = ValueType<Len2>(IntegerOps::signExtend<Len1, 64>(a.known_value()) %
                                     IntegerOps::signExtend<Len2, 64>(b.known_value()));
        } else {
            retval = ValueType<Len2>(InternalNode::create(Len2, SymbolicExpr::OP_SMOD,
                                                          a.get_expression(), b.get_expression()));
        }
        retval.defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
        return retval;
    }

    /** See NullSemantics::Policy::signedMultiply() */
    template <size_t Len1, size_t Len2>
    ValueType<Len1+Len2> signedMultiply(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
        ValueType<Len1+Len2> retval;
        if (a.is_known() && b.is_known()) {
            retval = ValueType<Len1+Len2>(IntegerOps::signExtend<Len1, 64>(a.known_value()) *
                                          IntegerOps::signExtend<Len2, 64>(b.known_value()));
        } else {
            retval = ValueType<Len1+Len2>(InternalNode::create(Len1+Len2, SymbolicExpr::OP_SMUL,
                                                               a.get_expression(), b.get_expression()));
        }
        retval.defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
        return retval;
    }

    /** See NullSemantics::Policy::unsignedDivide() */
    template <size_t Len1, size_t Len2>
    ValueType<Len1> unsignedDivide(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
        ValueType<Len1> retval;
        if (a.is_known() && b.is_known() && 0!=b.known_value()) {
            retval = ValueType<Len1>(a.known_value() / b.known_value());
        } else {
            retval = ValueType<Len1>(InternalNode::create(Len1, SymbolicExpr::OP_UDIV,
                                                          a.get_expression(), b.get_expression()));
        }
        retval.defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
        return retval;
    }

    /** See NullSemantics::Policy::unsignedModulo() */
    template <size_t Len1, size_t Len2>
    ValueType<Len2> unsignedModulo(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
        ValueType<Len2> retval;
        if (a.is_known() && b.is_known() && 0!=b.known_value()) {
            retval = ValueType<Len2>(a.known_value() % b.known_value());
        } else {
            retval = ValueType<Len2>(InternalNode::create(Len2, SymbolicExpr::OP_UMOD,
                                                          a.get_expression(), b.get_expression()));
        }
        retval.defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
        return retval;
    }

    /** See NullSemantics::Policy::unsignedMultiply() */
    template <size_t Len1, size_t Len2>
    ValueType<Len1+Len2> unsignedMultiply(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
        ValueType<Len1+Len2> retval;
        if (a.is_known() && b.is_known()) {
            retval = ValueType<Len1+Len2>(a.known_value()*b.known_value());
        } else {
            retval = ValueType<Len1+Len2>(InternalNode::create(Len1+Len2, SymbolicExpr::OP_UMUL,
                                                               a.get_expression(), b.get_expression()));
        }
        retval.defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
        return retval;
    }

    /** See NullSemantics::Policy::xor_() */
    template <size_t Len>
    ValueType<Len> xor_(const ValueType<Len> &a, const ValueType<Len> &b) const {
        ValueType<Len> retval;
        if (a.is_known() && b.is_known()) {
            retval = ValueType<Len>(a.known_value() ^ b.known_value());
        } else if (a.get_expression()->mustEqual(b.get_expression(), solver)) {
            retval = number<Len>(0);
        } else {
            retval = ValueType<Len>(InternalNode::create(Len, SymbolicExpr::OP_BV_XOR,
                                                         a.get_expression(), b.get_expression()));
        }
        retval.defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
        return retval;
    }



    /*****************************************************************************************************************
     * Functions invoked by the X86InstructionSemantics class for data access operations
     *****************************************************************************************************************/


    /** See NullSemantics::Policy::readRegister() */
    template<size_t Len/*bits*/>
    ValueType<Len> readRegister(const char *regname) {
        return readRegister<Len>(findRegister(regname, Len));
    }

    /** See NullSemantics::Policy::writeRegister() */
    template<size_t Len/*bits*/>
    void writeRegister(const char *regname, const ValueType<Len> &value) {
        writeRegister<Len>(findRegister(regname, Len), value);
    }

    /** See NullSemantics::Policy::readRegister() */
    template<size_t Len>
    ValueType<Len> readRegister(const RegisterDescriptor &reg) {
        switch (Len) {
            case 1:
                // Only FLAGS/EFLAGS bits have a size of one.  Other registers cannot be accessed at this
                // granularity.
                if (reg.get_major()!=x86_regclass_flags)
                    throw Exception("bit access only valid for FLAGS/EFLAGS register");
                if (reg.get_minor()!=0 || reg.get_offset()>=cur_state.registers.n_flags)
                    throw Exception("register not implemented in semantic policy");
                if (reg.get_nbits()!=1)
                    throw Exception("semantic policy supports only single-bit flags");
                return unsignedExtend<1, Len>(cur_state.registers.flag[reg.get_offset()]);

            case 8:
                // Only general-purpose registers can be accessed at a byte granularity, and we can access only the
                // low-order byte or the next higher byte.  For instance, "al" and "ah" registers.
                if (reg.get_major()!=x86_regclass_gpr)
                    throw Exception("byte access only valid for general purpose registers");
                if (reg.get_minor()>=cur_state.registers.n_gprs)
                    throw Exception("register not implemented in semantic policy");
                ASSERT_require(reg.get_nbits()==8); // we had better be asking for a one-byte register (e.g., "ah", not "ax")
                switch (reg.get_offset()) {
                    case 0:
                        return extract<0, Len>(cur_state.registers.gpr[reg.get_minor()]);
                    case 8:
                        return extract<8, 8+Len>(cur_state.registers.gpr[reg.get_minor()]);
                    default:
                        throw Exception("invalid one-byte access offset");
                }

            case 16:
                if (reg.get_nbits()!=16)
                    throw Exception("invalid 2-byte register");
                if (reg.get_offset()!=0)
                    throw Exception("policy does not support non-zero offsets for word granularity register access");
                switch (reg.get_major()) {
                    case x86_regclass_segment:
                        if (reg.get_minor()>=cur_state.registers.n_segregs)
                            throw Exception("register not implemented in semantic policy");
                        return unsignedExtend<16, Len>(cur_state.registers.segreg[reg.get_minor()]);
                    case x86_regclass_gpr:
                        if (reg.get_minor()>=cur_state.registers.n_gprs)
                            throw Exception("register not implemented in semantic policy");
                        return extract<0, Len>(cur_state.registers.gpr[reg.get_minor()]);
                    case x86_regclass_flags:
                        if (reg.get_minor()!=0 || cur_state.registers.n_flags<16)
                            throw Exception("register not implemented in semantic policy");
                        return unsignedExtend<16, Len>(concat(cur_state.registers.flag[0],
                                                       concat(cur_state.registers.flag[1],
                                                       concat(cur_state.registers.flag[2],
                                                       concat(cur_state.registers.flag[3],
                                                       concat(cur_state.registers.flag[4],
                                                       concat(cur_state.registers.flag[5],
                                                       concat(cur_state.registers.flag[6],
                                                       concat(cur_state.registers.flag[7],
                                                       concat(cur_state.registers.flag[8],
                                                       concat(cur_state.registers.flag[9],
                                                       concat(cur_state.registers.flag[10],
                                                       concat(cur_state.registers.flag[11],
                                                       concat(cur_state.registers.flag[12],
                                                       concat(cur_state.registers.flag[13],
                                                       concat(cur_state.registers.flag[14],
                                                              cur_state.registers.flag[15]))))))))))))))));
                    default:
                        throw Exception("word access not valid for this register type");
                }

            case 32:
                if (reg.get_offset()!=0)
                    throw Exception("policy does not support non-zero offsets for double word granularity"
                                    " register access");
                switch (reg.get_major()) {
                    case x86_regclass_gpr:
                        if (reg.get_minor()>=cur_state.registers.n_gprs)
                            throw Exception("register not implemented in semantic policy");
                        return unsignedExtend<32, Len>(cur_state.registers.gpr[reg.get_minor()]);
                    case x86_regclass_ip:
                        if (reg.get_minor()!=0)
                            throw Exception("register not implemented in semantic policy");
                        return unsignedExtend<32, Len>(cur_state.registers.ip);
                    case x86_regclass_segment:
                        if (reg.get_minor()>=cur_state.registers.n_segregs || reg.get_nbits()!=16)
                            throw Exception("register not implemented in semantic policy");
                        return unsignedExtend<16, Len>(cur_state.registers.segreg[reg.get_minor()]);
                    case x86_regclass_flags: {
                        if (reg.get_minor()!=0 || cur_state.registers.n_flags<32)
                            throw Exception("register not implemented in semantic policy");
                        if (reg.get_nbits()!=32)
                            throw Exception("register is not 32 bits");
                        return unsignedExtend<32, Len>(concat(readRegister<16>("flags"), // no-op sign extension
                                                       concat(cur_state.registers.flag[16],
                                                       concat(cur_state.registers.flag[17],
                                                       concat(cur_state.registers.flag[18],
                                                       concat(cur_state.registers.flag[19],
                                                       concat(cur_state.registers.flag[20],
                                                       concat(cur_state.registers.flag[21],
                                                       concat(cur_state.registers.flag[22],
                                                       concat(cur_state.registers.flag[23],
                                                       concat(cur_state.registers.flag[24],
                                                       concat(cur_state.registers.flag[25],
                                                       concat(cur_state.registers.flag[26],
                                                       concat(cur_state.registers.flag[27],
                                                       concat(cur_state.registers.flag[28],
                                                       concat(cur_state.registers.flag[29],
                                                       concat(cur_state.registers.flag[30],
                                                              cur_state.registers.flag[31])))))))))))))))));
                    }
                    default:
                        throw Exception("double word access not valid for this register type");
                }

            default:
                throw Exception("invalid register access width");
        }
    }

    /** See NullSemantics::Policy::writeRegister() */
    template<size_t Len>
    void writeRegister(const RegisterDescriptor &reg, const ValueType<Len> &value) {
        switch (Len) {
            case 1:
                // Only FLAGS/EFLAGS bits have a size of one.  Other registers cannot be accessed at this
                // granularity.
                if (reg.get_major()!=x86_regclass_flags)
                    throw Exception("bit access only valid for FLAGS/EFLAGS register");
                if (reg.get_minor()!=0 || reg.get_offset()>=cur_state.registers.n_flags)
                    throw Exception("register not implemented in semantic policy");
                if (reg.get_nbits()!=1)
                    throw Exception("semantic policy supports only single-bit flags");
                cur_state.registers.flag[reg.get_offset()] = unsignedExtend<Len, 1>(value);
                cur_state.registers.flag[reg.get_offset()].defined_by(cur_insn);
                break;

            case 8:
                // Only general purpose registers can be accessed at byte granularity, and only for offsets 0 and 8.
                if (reg.get_major()!=x86_regclass_gpr)
                    throw Exception("byte access only valid for general purpose registers.");
                if (reg.get_minor()>=cur_state.registers.n_gprs)
                    throw Exception("register not implemented in semantic policy");
                ASSERT_require(reg.get_nbits()==8); // we had better be asking for a one-byte register (e.g., "ah", not "ax")
                switch (reg.get_offset()) {
                    case 0:
                        cur_state.registers.gpr[reg.get_minor()] =                                    // no-op extend
                            concat(signExtend<Len, 8>(value),
                                   extract<8, 32>(cur_state.registers.gpr[reg.get_minor()]));
                        cur_state.registers.gpr[reg.get_minor()].defined_by(cur_insn);
                        break;
                    case 8:
                        cur_state.registers.gpr[reg.get_minor()] =
                            concat(extract<0, 8>(cur_state.registers.gpr[reg.get_minor()]),
                                   concat(unsignedExtend<Len, 8>(value),
                                          extract<16, 32>(cur_state.registers.gpr[reg.get_minor()])));
                        cur_state.registers.gpr[reg.get_minor()].defined_by(cur_insn);
                        break;
                    default:
                        throw Exception("invalid byte access offset");
                }
                break;

            case 16:
                if (reg.get_nbits()!=16)
                    throw Exception("invalid 2-byte register");
                if (reg.get_offset()!=0)
                    throw Exception("policy does not support non-zero offsets for word granularity register access");
                switch (reg.get_major()) {
                    case x86_regclass_segment:
                        if (reg.get_minor()>=cur_state.registers.n_segregs)
                            throw Exception("register not implemented in semantic policy");
                        cur_state.registers.segreg[reg.get_minor()] = unsignedExtend<Len, 16>(value);
                        cur_state.registers.segreg[reg.get_minor()].defined_by(cur_insn);
                        break;
                    case x86_regclass_gpr:
                        if (reg.get_minor()>=cur_state.registers.n_gprs)
                            throw Exception("register not implemented in semantic policy");
                        cur_state.registers.gpr[reg.get_minor()] =
                            concat(unsignedExtend<Len, 16>(value),
                                   extract<16, 32>(cur_state.registers.gpr[reg.get_minor()]));
                        cur_state.registers.gpr[reg.get_minor()].defined_by(cur_insn);
                        break;
                    case x86_regclass_flags:
                        if (reg.get_minor()!=0 || cur_state.registers.n_flags<16)
                            throw Exception("register not implemented in semantic policy");
                        cur_state.registers.flag[0]  = extract<0,  1 >(value);
                        cur_state.registers.flag[0].defined_by(cur_insn);
                        cur_state.registers.flag[1]  = extract<1,  2 >(value);
                        cur_state.registers.flag[1].defined_by(cur_insn);
                        cur_state.registers.flag[2]  = extract<2,  3 >(value);
                        cur_state.registers.flag[2].defined_by(cur_insn);
                        cur_state.registers.flag[3]  = extract<3,  4 >(value);
                        cur_state.registers.flag[3].defined_by(cur_insn);
                        cur_state.registers.flag[4]  = extract<4,  5 >(value);
                        cur_state.registers.flag[4].defined_by(cur_insn);
                        cur_state.registers.flag[5]  = extract<5,  6 >(value);
                        cur_state.registers.flag[5].defined_by(cur_insn);
                        cur_state.registers.flag[6]  = extract<6,  7 >(value);
                        cur_state.registers.flag[6].defined_by(cur_insn);
                        cur_state.registers.flag[7]  = extract<7,  8 >(value);
                        cur_state.registers.flag[7].defined_by(cur_insn);
                        cur_state.registers.flag[8]  = extract<8,  9 >(value);
                        cur_state.registers.flag[8].defined_by(cur_insn);
                        cur_state.registers.flag[9]  = extract<9,  10>(value);
                        cur_state.registers.flag[9].defined_by(cur_insn);
                        cur_state.registers.flag[10] = extract<10, 11>(value);
                        cur_state.registers.flag[10].defined_by(cur_insn);
                        cur_state.registers.flag[11] = extract<11, 12>(value);
                        cur_state.registers.flag[11].defined_by(cur_insn);
                        cur_state.registers.flag[12] = extract<12, 13>(value);
                        cur_state.registers.flag[12].defined_by(cur_insn);
                        cur_state.registers.flag[13] = extract<13, 14>(value);
                        cur_state.registers.flag[13].defined_by(cur_insn);
                        cur_state.registers.flag[14] = extract<14, 15>(value);
                        cur_state.registers.flag[14].defined_by(cur_insn);
                        cur_state.registers.flag[15] = extract<15, 16>(value);
                        cur_state.registers.flag[15].defined_by(cur_insn);
                        break;
                    default:
                        throw Exception("word access not valid for this register type");
                }
                break;

            case 32:
                if (reg.get_offset()!=0)
                    throw Exception("policy does not support non-zero offsets for double word granularity"
                                    " register access");
                switch (reg.get_major()) {
                    case x86_regclass_gpr:
                        if (reg.get_minor()>=cur_state.registers.n_gprs)
                            throw Exception("register not implemented in semantic policy");
                        cur_state.registers.gpr[reg.get_minor()] = signExtend<Len, 32>(value);
                        cur_state.registers.gpr[reg.get_minor()].defined_by(cur_insn);
                        break;
                    case x86_regclass_ip:
                        if (reg.get_minor()!=0)
                            throw Exception("register not implemented in semantic policy");
                        cur_state.registers.ip = unsignedExtend<Len, 32>(value);
                        cur_state.registers.ip.defined_by(cur_insn);
                        break;
                    case x86_regclass_flags:
                        if (reg.get_minor()!=0 || cur_state.registers.n_flags<32)
                            throw Exception("register not implemented in semantic policy");
                        if (reg.get_nbits()!=32)
                            throw Exception("register is not 32 bits");
                        writeRegister<16>("flags", unsignedExtend<Len, 16>(value));
                        cur_state.registers.flag[16] = extract<16, 17>(value);
                        cur_state.registers.flag[16].defined_by(cur_insn);
                        cur_state.registers.flag[17] = extract<17, 18>(value);
                        cur_state.registers.flag[17].defined_by(cur_insn);
                        cur_state.registers.flag[18] = extract<18, 19>(value);
                        cur_state.registers.flag[18].defined_by(cur_insn);
                        cur_state.registers.flag[19] = extract<19, 20>(value);
                        cur_state.registers.flag[19].defined_by(cur_insn);
                        cur_state.registers.flag[20] = extract<20, 21>(value);
                        cur_state.registers.flag[20].defined_by(cur_insn);
                        cur_state.registers.flag[21] = extract<21, 22>(value);
                        cur_state.registers.flag[21].defined_by(cur_insn);
                        cur_state.registers.flag[22] = extract<22, 23>(value);
                        cur_state.registers.flag[22].defined_by(cur_insn);
                        cur_state.registers.flag[23] = extract<23, 24>(value);
                        cur_state.registers.flag[23].defined_by(cur_insn);
                        cur_state.registers.flag[24] = extract<24, 25>(value);
                        cur_state.registers.flag[24].defined_by(cur_insn);
                        cur_state.registers.flag[25] = extract<25, 26>(value);
                        cur_state.registers.flag[25].defined_by(cur_insn);
                        cur_state.registers.flag[26] = extract<26, 27>(value);
                        cur_state.registers.flag[26].defined_by(cur_insn);
                        cur_state.registers.flag[27] = extract<27, 28>(value);
                        cur_state.registers.flag[27].defined_by(cur_insn);
                        cur_state.registers.flag[28] = extract<28, 29>(value);
                        cur_state.registers.flag[28].defined_by(cur_insn);
                        cur_state.registers.flag[29] = extract<29, 30>(value);
                        cur_state.registers.flag[29].defined_by(cur_insn);
                        cur_state.registers.flag[30] = extract<30, 31>(value);
                        cur_state.registers.flag[30].defined_by(cur_insn);
                        cur_state.registers.flag[31] = extract<31, 32>(value);
                        cur_state.registers.flag[31].defined_by(cur_insn);
                        break;
                    default:
                        throw Exception("double word access not valid for this register type");
                }
                break;

            default:
                throw Exception("invalid register access width");
        }
    }

    /** See NullSemantics::Policy::readMemory() */
    template <size_t Len> ValueType<Len>
    readMemory(X86SegmentRegister segreg, const ValueType<32> &addr, const ValueType<1> &cond) const {
        return mem_read<Len>(cur_state, addr);
    }

    /** See NullSemantics::Policy::writeMemory() */
    template <size_t Len> void
    writeMemory(X86SegmentRegister segreg, const ValueType<32> &addr, const ValueType<Len> &data,
                const ValueType<1> &cond) {
        mem_write<Len>(cur_state, addr, data);
    }
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
