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

namespace BinaryAnalysis {              // documented elsewhere
    namespace InstructionSemantics {    // documented elsewhere

        /** A fully symbolic semantic domain.
         *
         *  This semantic domain can be used to emulate the execution of a single basic block of instructions.  It is similar
         *  in nature to PartialSymbolicSemantics, but with a different type of ValueType: instead of values being a constant or
         *  variable with offset, values here are expression trees.
         *
         *  <ul>
         *    <li>Policy: the policy class used to instantiate X86InstructionSemantic instances.</li>
         *    <li>State: represents the state of the virtual machine, its registers and memory.</li>
         *    <li>ValueType: the values stored in registers and memory and used for memory addresses.</li>
         *  </ul>
         *
         *  If an SMT solver is supplied as a Policy constructor argument then that SMT solver will be used to answer various
         *  questions such as when two memory addresses can alias one another.  When an SMT solver is lacking, the questions
         *  will be answered by very naive comparison of the expression trees. */
        namespace SymbolicSemantics {

            typedef InsnSemanticsExpr::RenameMap RenameMap;
            typedef InsnSemanticsExpr::LeafNode LeafNode;
            typedef InsnSemanticsExpr::LeafNodePtr LeafNodePtr;
            typedef InsnSemanticsExpr::InternalNode InternalNode;
            typedef InsnSemanticsExpr::InternalNodePtr InternalNodePtr;
            typedef InsnSemanticsExpr::TreeNode TreeNode;
            typedef InsnSemanticsExpr::TreeNodePtr TreeNodePtr;
            typedef std::set<SgAsmInstruction*> InsnSet;

            /******************************************************************************************************************
             *                          ValueType
             ******************************************************************************************************************/

            /* ValueType cannot directly be a TreeNode because ValueType's bit size is a template argument while tree node
             * sizes are stored as a data member.  Therefore, ValueType will always point to a TreeNode.  Most of the methods
             * that are invoked on ValueType just call the same methods for TreeNode. */
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
                    expr = LeafNode::create_variable(nBits, comment);
                }

                ValueType(const ValueType &other) {
                    expr = other.expr;
                    defs = other.defs;
                }

                /** Construct a ValueType with a known value. */
                explicit ValueType(uint64_t n, std::string comment="") {
                    expr = LeafNode::create_integer(nBits, n, comment);
                }

                /** Construct a ValueType from a TreeNode. */
                explicit ValueType(const TreeNodePtr &node) {
                    assert(node->get_nbits()==nBits);
                    expr = node;
                }

                /** Adds instructions to the list of defining instructions.  Adds the specified instruction and defining sets
                 *  into this value and returns a reference to this value.  This is a convenience function used internally by
                 *  the policy's X86InstructionSemantics callback methods. See also add_defining_instructions().
                 * @{ */
                ValueType& defined_by(SgAsmInstruction *insn, const InsnSet &set1, const InsnSet &set2, const InsnSet &set3) {
                    add_defining_instructions(set3);
                    return defined_by(insn, set1, set2);
                }
                ValueType& defined_by(SgAsmInstruction *insn, const InsnSet &set1, const InsnSet &set2) {
                    add_defining_instructions(set2);
                    return defined_by(insn, set1);
                }
                ValueType& defined_by(SgAsmInstruction *insn, const InsnSet &set1) {
                    add_defining_instructions(set1);
                    return defined_by(insn);
                }
                ValueType& defined_by(SgAsmInstruction *insn) {
                    add_defining_instructions(insn);
                    return *this;
                }
                /** @} */

                /** Print the value. If a rename map is specified a named value will be renamed to have a shorter name.  See
                 *  the rename() method for details. */
                void print(std::ostream &o, RenameMap *rmap=NULL) const {
                    o <<"defs={";
                    size_t ndefs=0;
                    for (InsnSet::const_iterator di=defs.begin(); di!=defs.end(); ++di, ++ndefs) {
                        SgAsmInstruction *insn = *di;
                        if (insn!=NULL)
                            o <<(ndefs>0?",":"") <<StringUtility::addrToString(insn->get_address());
                    }
                    o <<"} expr=";
                    expr->print(o, rmap);
                }
                void print(std::ostream &o, BaseSemantics::SEMANTIC_NO_PRINT_HELPER *unused=NULL) const {
                    print(o, (RenameMap*)0);
                }
                friend std::ostream& operator<<(std::ostream &o, const ValueType &e) {
                    e.print(o, (RenameMap*)0);
                    return o;
                }

                /** Returns true if the value is a known constant. */
                bool is_known() const {
                    return expr->is_known();
                }

                /** Returns the value of a known constant. Assumes this value is a known constant. */
                uint64_t known_value() const {
                    LeafNodePtr leaf = expr->isLeafNode();
                    assert(leaf!=NULL);
                    return leaf->get_value();
                }

                /** Returns the expression stored in this value.  Expressions are reference counted; the reference count of the
                 *  returned expression is not incremented. */
                const TreeNodePtr& get_expression() const {
                    return expr;
                }

                /** Changes the expression stored in the value.
                 * @{ */
                void set_expression(const TreeNodePtr &new_expr) {
                    expr = new_expr;
                }
                void set_expression(const ValueType &source) {
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
                const InsnSet& get_defining_instructions() const {
                    return defs;
                }

                /** Adds definitions to the list of defining instructions. Returns the number of items added that weren't
                 *  already in the list of defining instructions.
                 * @{ */
                size_t add_defining_instructions(const InsnSet &to_add) {
                    size_t nadded = 0;
                    for (InsnSet::const_iterator i=to_add.begin(); i!=to_add.end(); ++i) {
                        std::pair<InsnSet::iterator, bool> inserted = defs.insert(*i);
                        if (inserted.second)
                            ++nadded;
                    }
                    return nadded;
                }
                size_t add_defining_instructions(const ValueType &source) {
                    return add_defining_instructions(source.get_defining_instructions());
                }
                size_t add_defining_instructions(SgAsmInstruction *insn) {
                    InsnSet tmp;
                    if (insn)
                        tmp.insert(insn);
                    return add_defining_instructions(tmp);
                }
                /** @} */

                /** Set definint instructions.  This discards the old set of defining instructions and replaces it with the
                 *  specified set.
                 * @{ */
                void set_defining_instructions(const InsnSet &new_defs) {
                    defs = new_defs;
                }
                void set_defining_instructions(const ValueType &source) {
                    set_defining_instructions(source.get_defining_instructions());
                }
                void set_defining_instructions(SgAsmInstruction *insn) {
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

            /** Memory cell with symbolic address and data.  The ValueType template argument should be a subclass of
             *  SymbolicSemantics::ValueType. */
            template<template<size_t> class ValueType=SymbolicSemantics::ValueType>
            class MemoryCell: public BaseSemantics::MemoryCell<ValueType> {
            public:

                /** Constructor that sets the defining instruction.  This is just like the base class, except we also add an
                 *  optional defining instruction. */
                template <size_t Len>
                MemoryCell(const ValueType<32> &address, const ValueType<Len> &data, size_t nbytes, SgAsmInstruction *insn=NULL)
                    : BaseSemantics::MemoryCell<ValueType>(address, data, nbytes) {
                    this->get_data().add_defining_instructions(insn);
                }

                /** Returns true if this memory value could possibly overlap with the @p other memory value.  In other words,
                 *  returns false only if this memory location cannot overlap with @p other memory location. Two addresses that
                 *  are identical alias one another. The @p solver is optional but recommended (absence of a solver will result
                 *  in a naive definition).
                 * 
                 *  Address X and Y may alias each other if X+datasize(X)>=Y or X<Y+datasize(Y) where datasize(A) is the number
                 *  of bytes stored at address A. In other words, if the following expression is satisfiable, then the memory
                 *  cells might alias one another.
                 *
                 *  \code
                 *     ((X.addr + X.nbytes > Y.addr) && (X.addr < Y.addr + Y.nbytes)) ||
                 *     ((Y.addr + Y.nbytes > X.addr) && (Y.addr < X.addr + X.nbytes))
                 *  \code
                 *
                 *  Or, equivalently written in LISP style
                 *
                 *  \code
                 *     (and (or (> (+ X.addr X.nbytes) Y.addr) (< X.addr (+ Y.addr Y.nbytes)))
                 *          (or (> (+ Y.addr Y.nbytes) X.addr) (< Y.addr (+ X.addr X.nbytes))))
                 *  \endcode
                 */
                bool may_alias(const MemoryCell &other, SMTSolver *solver) const {
                    bool retval = must_alias(other, solver); /*this might be faster to solve*/
                    if (retval)
                        return retval;
                    if (solver) {
                        TreeNodePtr x_addr   = this->address.get_expression();
                        TreeNodePtr x_nbytes = LeafNode::create_integer(32, this->nbytes);
                        TreeNodePtr y_addr   = other.address.get_expression();
                        TreeNodePtr y_nbytes = LeafNode::create_integer(32, other.nbytes);

                        TreeNodePtr x_end =
                            InternalNode::create(32, InsnSemanticsExpr::OP_ADD, x_addr, x_nbytes);
                        TreeNodePtr y_end =
                            InternalNode::create(32, InsnSemanticsExpr::OP_ADD, y_addr, y_nbytes);

                        TreeNodePtr and1 =
                            InternalNode::create(1, InsnSemanticsExpr::OP_AND,
                                                 InternalNode::create(1, InsnSemanticsExpr::OP_UGT, x_end, y_addr),
                                                 InternalNode::create(1, InsnSemanticsExpr::OP_ULT, x_addr, y_end));
                        TreeNodePtr and2 =
                            InternalNode::create(1, InsnSemanticsExpr::OP_AND,
                                                 InternalNode::create(1, InsnSemanticsExpr::OP_UGT, y_end, x_addr),
                                                 InternalNode::create(1, InsnSemanticsExpr::OP_ULT, y_addr, x_end));
                        TreeNodePtr assertion =
                            InternalNode::create(1, InsnSemanticsExpr::OP_OR, and1, and2);
                        retval = solver->satisfiable(assertion);
                    }
                    return retval;
                }

                /** Returns true if this memory address is the same as the @p other. Note that "same" is more strict than
                 *  "overlap".  The @p solver is optional but recommended (absence of a solver will result in a naive
                 *  definition). */
                bool must_alias(const MemoryCell &other, SMTSolver *solver) const {
                    return this->get_address().get_expression()->equal_to(other.get_address().get_expression(), solver);
                }
            };

            /** Represents the entire state of the machine. */
            template <template <size_t> class ValueType=SymbolicSemantics::ValueType>
            struct State: public BaseSemantics::StateX86<MemoryCell, ValueType> {
                /** Print info about how registers differ.  If a rename map is specified then named values will be renamed to
                 *  have a shorter name.  See the ValueType<>::rename() method for details. */
                void print_diff_registers(std::ostream &o, const State&, RenameMap *rmap=NULL) const;

                /** Tests registers of two states for equality. */
                bool equal_registers(const State&) const;

                /** Removes from memory those values at addresses below the current stack pointer. This is automatically called
                 *  after each instruction if the policy's p_discard_popped_memory property is set. */
                void discard_popped_memory() {
                    /*FIXME: not implemented yet. [RPM 2010-05-24]*/
                }
            };

            /** A policy that is supplied to the semantic analysis constructor. See documentation for the SymbolicSemantics
             *  namespace.  The RISC-like operations are documented in the
             *  BinaryAnalysis::InstructionSemantics::NullSemantics::Policy class. */
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
                                                     * the stack pointer is adjusted, memory below the stack pointer and having
                                                     * the same address name as the stack pointer is removed (the memory
                                                     * location becomes undefined). The default is false, that is, no special
                                                     * treatment for the stack. */
                size_t ninsns;                      /**< Total number of instructions processed. This is incremented by
                                                     * startInstruction(), which is the first thing called by
                                                     * X86InstructionSemantics::processInstruction(). */
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

                /** Returns the current instruction pointer. */
                const ValueType<32>& get_ip() const { return cur_state.ip; }

                /** Returns the original instruction pointer. See also get_orig_state(). */
                const ValueType<32>& get_orig_ip() const { return orig_state.ip; }

                /** Returns a copy of the state after removing memory that is not pertinent to an equal_states() comparison. */
                Memory memory_for_equality(const State<ValueType>&) const;

                /** Returns a copy of the current state after removing memory that is not pertinent to an equal_states()
                 *  comparison. */
                Memory memory_for_equality() const { return memory_for_equality(cur_state); }

                /** Compares two states for equality. The comarison looks at all register values and the memory locations that
                 *  are different than their original value (but excluding differences due to clobbering). It does not compare
                 *  memory that has only been read. */
                bool equal_states(const State<ValueType>&, const State<ValueType>&) const;

                /** Print the current state of this policy.  If a rename map is specified then named values will be renamed to
                 *  have a shorter name.  See the ValueType<>::rename() method for details. */
                void print(std::ostream &o, RenameMap *rmap=NULL) const {
                    cur_state.print(o, "", rmap);
                }
                friend std::ostream& operator<<(std::ostream &o, const Policy &p) {
                    p.print(o, NULL);
                    return o;
                }

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

                /** Print only the differences between two states.  If a rename map is specified then named values will be
                 *  renamed to have a shorter name.  See the ValueType<>::rename() method for details. */
                void print_diff(std::ostream&, const State<ValueType>&, const State<ValueType>&, RenameMap *rmap=NULL) const ;

                /** Print the difference between a state and the initial state.  If a rename map is specified then named values
                 *  will be renamed to have a shorter name.  See the ValueType<>::rename() method for details. */
                void print_diff(std::ostream &o, const State<ValueType> &state, RenameMap *rmap=NULL) const {
                    print_diff(o, orig_state, state, rmap);
                }

                /** Print the difference between the current state and the initial state.  If a rename map is specified then
                 *  named values will be renamed to have a shorter name.  See the ValueType<>::rename() method for details. */
                void print_diff(std::ostream &o, RenameMap *rmap=NULL) const {
                    print_diff(o, orig_state, cur_state, rmap);
                }

                /** Returns the SHA1 hash of the difference between the current state and the original state.  If libgcrypt is
                 *  not available then the return value will be an empty string. */
                std::string SHA1() const;

                /** Extend (or shrink) from @p FromLen bits to @p ToLen bits by adding or removing high-order bits from the
                 *  input. Added bits are always zeros. */
                template <size_t FromLen, size_t ToLen>
                ValueType<ToLen> unsignedExtend(const ValueType<FromLen> &a) const {
                    if (a.is_known())
                        return ValueType<ToLen>(IntegerOps::GenMask<uint64_t,ToLen>::value & a.known_value())
                            .defined_by(cur_insn, a.get_defining_instructions());
                    if (FromLen==ToLen) {
                        // no-op, so not defined by current insn
                        return ValueType<ToLen>(a.get_expression()).defined_by(NULL, a.get_defining_instructions());
                    }
                    if (FromLen>ToLen)
                        return ValueType<ToLen>(InternalNode::create(ToLen, InsnSemanticsExpr::OP_EXTRACT,
                                                                     LeafNode::create_integer(32, 0),
                                                                     LeafNode::create_integer(32, ToLen),
                                                                     a.get_expression()))
                            .defined_by(cur_insn, a.get_defining_instructions());
                    return ValueType<ToLen>(InternalNode::create(ToLen, InsnSemanticsExpr::OP_UEXTEND,
                                                                 LeafNode::create_integer(32, ToLen), a.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions());
                }

                /** Sign extend from @p FromLen bits to @p ToLen bits. */
                template <size_t FromLen, size_t ToLen>
                ValueType<ToLen> signedExtend(const ValueType<FromLen> &a) const {
                    if (a.is_known())
                        return ValueType<ToLen>(IntegerOps::signExtend<FromLen, ToLen>(a.known_value())).
                            defined_by(cur_insn, a.get_defining_instructions());
                    if (FromLen==ToLen) {
                        // no-op, so not defined by current insns
                        return ValueType<ToLen>(a.get_expression()).defined_by(NULL, a.get_defining_instructions());
                    }
                    if (FromLen > ToLen)
                        return ValueType<ToLen>(InternalNode::create(ToLen, InsnSemanticsExpr::OP_EXTRACT,
                                                                     LeafNode::create_integer(32, 0),
                                                                     LeafNode::create_integer(32, ToLen),
                                                                     a.get_expression()))
                            .defined_by(cur_insn, a.get_defining_instructions());
                    return ValueType<ToLen>(InternalNode::create(ToLen, InsnSemanticsExpr::OP_SEXTEND,
                                                                 LeafNode::create_integer(32, ToLen), a.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions());
                }

                /** Extracts certain bits from the specified value and shifts them to the low-order positions in the result.
                 *  The bits of the result include bits from BeginAt (inclusive) through EndAt (exclusive).  The lsb is
                 *  numbered zero. */
                template <size_t BeginAt, size_t EndAt, size_t Len>
                ValueType<EndAt-BeginAt> extract(const ValueType<Len> &a) const {
                    if (0==BeginAt)
                        return unsignedExtend<Len,EndAt-BeginAt>(a);
                    if (a.is_known())
                        return ValueType<EndAt-BeginAt>((a.known_value()>>BeginAt) & IntegerOps::genMask<uint64_t>(EndAt-BeginAt))
                            .defined_by(cur_insn, a.get_defining_instructions());
                    return ValueType<EndAt-BeginAt>(InternalNode::create(EndAt-BeginAt, InsnSemanticsExpr::OP_EXTRACT,
                                                                         LeafNode::create_integer(32, BeginAt),
                                                                         LeafNode::create_integer(32, EndAt),
                                                                         a.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions());
                }

                /** Reads a value from memory in a way that always returns the same value provided there are not intervening
                 *  writes that would clobber the value either directly or by aliasing.  Also, if appropriate, the value is
                 *  added to the original memory state (thus changing the value at that address from an implicit named value to
                 *  an explicit named value).
                 *
                 *  The @p dflt is the value to use if the memory address has not been assigned a value yet.
                 *
                 *  It is safe to call this function and supply the policy's original state as the state argument.
                 *
                 *  The documentation for MemoryCell has an example that demonstrates the desired behavior of mem_read() and
                 *  mem_write(). */
                template <size_t Len> ValueType<Len> mem_read(State<ValueType> &state, const ValueType<32> &addr,
                                                              const ValueType<Len> &dflt) const {
                    MemoryCell<ValueType> new_cell(addr, unsignedExtend<Len,32>(dflt), Len/8, NULL/*no defining instruction*/);
                    bool aliased = false; /*is new_cell aliased by any existing writes?*/

                    for (typename Memory::iterator mi=state.mem.begin(); mi!=state.mem.end(); ++mi) {
                        if (new_cell.must_alias(*mi, solver)) {
                            if ((*mi).is_clobbered()) {
                                (*mi).set_clobbered(false);
                                (*mi).set_data(new_cell.get_data());
                                return unsignedExtend<32, Len>(new_cell.get_data());
                            } else {
                                return unsignedExtend<32, Len>((*mi).get_data());
                            }
                        } else if ((*mi).is_written() && new_cell.may_alias(*mi, solver)) {
                            aliased = true;
                        }
                    }

                    if (!aliased && &state!=&orig_state) {
                        /* We didn't find the memory cell in the specified state and it's not aliased to any writes in that
                         * state.  Therefore use the value from the initial memory state (creating it if necessary). */
                        for (typename Memory::iterator mi=orig_state.mem.begin(); mi!=orig_state.mem.end(); ++mi) {
                            if (new_cell.must_alias(*mi, solver)) {
                                ROSE_ASSERT(!(*mi).is_clobbered());
                                ROSE_ASSERT(!(*mi).is_written());
                                state.mem.push_back(*mi);
                                return unsignedExtend<32, Len>((*mi).get_data());
                            }
                        }

                        orig_state.mem.push_back(new_cell);
                    }

                    /* Create the cell in the current state. */
                    state.mem.push_back(new_cell);
                    return unsignedExtend<32,Len>(new_cell.get_data()); // no defining instruction
                }

                /** See memory_reference_type(). */
                enum MemRefType { MRT_STACK_PTR, MRT_FRAME_PTR, MRT_OTHER_PTR };

                /** Determines if the specified address is related to the current stack or frame pointer. This is used by
                 *  mem_write() when we're operating under the assumption that memory written via stack pointer is different
                 *  than memory written via frame pointer, and that memory written by either pointer is different than all
                 *  other memory. */
                MemRefType memory_reference_type(const State<ValueType> &state, const ValueType<32> &addr) const {
#if 0 /*FIXME: not implemented yet [RPM 2010-05-24]*/
                    if (addr.name) {
                        if (addr.name==state.gpr[x86_gpr_sp].name) return MRT_STACK_PTR;
                        if (addr.name==state.gpr[x86_gpr_bp].name) return MRT_FRAME_PTR;
                        return MRT_OTHER_PTR;
                    }
                    if (addr==state.gpr[x86_gpr_sp]) return MRT_STACK_PTR;
                    if (addr==state.gpr[x86_gpr_bp]) return MRT_FRAME_PTR;
#endif
                    return MRT_OTHER_PTR;
                }

                /** Writes a value to memory. If the address written to is an alias for other addresses then the other
                 *  addresses will be clobbered. Subsequent reads from clobbered addresses will return new values. See also,
                 *  mem_read(). */
                template <size_t Len> void mem_write(State<ValueType> &state, const ValueType<32> &addr,
                                                     const ValueType<Len> &data) {
                    ROSE_ASSERT(&state!=&orig_state);
                    MemoryCell<ValueType> new_cell(addr, unsignedExtend<Len, 32>(data), Len/8, cur_insn);
                    new_cell.set_written();
                    bool saved = false; /* has new_cell been saved into memory? */

                    /* Is new memory reference through the stack pointer or frame pointer? */
                    MemRefType new_mrt = memory_reference_type(state, addr);

                    /* Overwrite and/or clobber existing memory locations. */
                    for (typename Memory::iterator mi=state.mem.begin(); mi!=state.mem.end(); ++mi) {
                        if (new_cell.must_alias(*mi, solver)) {
                            *mi = new_cell;
                            saved = true;
                        } else if (p_discard_popped_memory && new_mrt!=memory_reference_type(state, (*mi).get_address())) {
                            /* Assume that memory referenced through the stack pointer does not alias that which is referenced
                             * through the frame pointer, and neither of them alias memory that is referenced other ways. */
                        } else if (new_cell.may_alias(*mi, solver)) {
                            (*mi).set_clobbered();
                        } else {
                            /* memory cell *mi is not aliased to cell being written */
                        }
                    }
                    if (!saved)
                        state.mem.push_back(new_cell);
                }


                /*************************************************************************************************************
                 * Functions invoked by the X86InstructionSemantics class for every processed instructions
                 *************************************************************************************************************/

                /** See NullSemantics::Policy::startInstruction() */
                void startInstruction(SgAsmInstruction *insn) {
                    if (!cur_state.ip.is_known()) {
                        cur_state.ip = ValueType<32>(insn->get_address()); // semantics user should have probably initialized EIP
                    } else if (cur_state.ip.known_value()!=insn->get_address()) {
                        fprintf(stderr, "SymbolicSemantics::Policy::startInstruction: invalid EIP value for current instruction\n\
    startInstruction() is being called for an instruction with a concrete\n\
    address stored in the SgAsmx86Instruction object, but the current value of\n\
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
                        assert(cur_state.ip.known_value()==insn->get_address()); // redundant, used for error mesg
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
                    return ValueType<1>(1).defined_by(cur_insn);
                }

                /** See NullSemantics::Policy::false_() */
                ValueType<1> false_() const {
                    return ValueType<1>((uint64_t)0).defined_by(cur_insn);
                }

                /** See NullSemantics::Policy::undefined_() */
                ValueType<1> undefined_() const {
                    return ValueType<1>().defined_by(cur_insn);
                }

                /** See NullSemantics::Policy::number() */
                template <size_t Len>
                ValueType<Len> number(uint64_t n) const {
                    return ValueType<Len>(n).defined_by(cur_insn);
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
                    if (a.is_known()) {
                        if (b.is_known()) {
                            return ValueType<Len>(LeafNode::create_integer(Len, a.known_value()+b.known_value()))
                                .defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
                        } else if (0==a.known_value()) {
                            return b;
                        }
                    } else if (b.is_known() && 0==b.known_value()) {
                        return a;
                    }
                    return ValueType<Len>(InternalNode::create(Len, InsnSemanticsExpr::OP_ADD,
                                                               a.get_expression(),
                                                               b.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
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
                    if (a.is_known() && b.is_known())
                        return ValueType<Len>(a.known_value() & b.known_value())
                            .defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
                    return ValueType<Len>(InternalNode::create(Len, InsnSemanticsExpr::OP_BV_AND,
                                                               a.get_expression(), b.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
                }

                /** See NullSemantics::Policy::equalToZero() */
                template <size_t Len>
                ValueType<1> equalToZero(const ValueType<Len> &a) const {
                    if (a.is_known()) {
                        ValueType<1> retval = a.known_value() ? false_() : true_();
                        return retval.defined_by(cur_insn, a.get_defining_instructions());
                    }
                    return ValueType<1>(InternalNode::create(1, InsnSemanticsExpr::OP_ZEROP, a.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions());
                }

                /** See NullSemantics::Policy::invert() */
                template <size_t Len>
                ValueType<Len> invert(const ValueType<Len> &a) const {
                    if (a.is_known())
                        return ValueType<Len>(LeafNode::create_integer(Len, ~a.known_value()))
                            .defined_by(cur_insn, a.get_defining_instructions());
                    return ValueType<Len>(InternalNode::create(Len, InsnSemanticsExpr::OP_INVERT, a.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions());
                }

                /** See NullSemantics::Policy::concat() */
                template<size_t Len1, size_t Len2>
                ValueType<Len1+Len2> concat(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
                    if (a.is_known() && b.is_known())
                        return ValueType<Len1+Len2>(a.known_value() | (b.known_value() << Len1))
                            .defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
                    return ValueType<Len1+Len2>(InternalNode::create(Len1+Len2, InsnSemanticsExpr::OP_CONCAT,
                                                                     b.get_expression(), a.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
                }

                /** See NullSemantics::Policy::ite() */
                template <size_t Len>
                ValueType<Len> ite(const ValueType<1> &sel, const ValueType<Len> &ifTrue, const ValueType<Len> &ifFalse) const {
                    if (sel.is_known()) {
                        ValueType<Len> retval = sel.known_value() ? ifTrue : ifFalse;
                        return retval.defined_by(cur_insn, sel.get_defining_instructions());
                    }
                    if (solver) {
                        /* If the selection expression cannot be true, then return ifFalse */
                        TreeNodePtr assertion = InternalNode::create(1, InsnSemanticsExpr::OP_EQ,
                                                                     sel.get_expression(),
                                                                     LeafNode::create_integer(1, 1));
                        bool can_be_true = solver->satisfiable(assertion);
                        if (!can_be_true) {
                            ValueType<Len> retval = ifFalse;
                            return retval.defined_by(cur_insn, sel.get_defining_instructions());
                        }

                        /* If the selection expression cannot be false, then return ifTrue */
                        assertion = InternalNode::create(1, InsnSemanticsExpr::OP_EQ,
                                                         sel.get_expression(), LeafNode::create_integer(1, 0));
                        bool can_be_false = solver->satisfiable(assertion);
                        if (!can_be_false) {
                            ValueType<Len> retval = ifTrue;
                            return retval.defined_by(cur_insn, sel.get_defining_instructions());
                        }
                    }
                    return ValueType<Len>(InternalNode::create(Len, InsnSemanticsExpr::OP_ITE, sel.get_expression(),
                                                               ifTrue.get_expression(), ifFalse.get_expression()))
                        .defined_by(cur_insn, sel.get_defining_instructions(),
                                    ifTrue.get_defining_instructions(), ifFalse.get_defining_instructions());
                }

                /** See NullSemantics::Policy::leastSignificantSetBit() */
                template <size_t Len>
                ValueType<Len> leastSignificantSetBit(const ValueType<Len> &a) const {
                    if (a.is_known()) {
                        uint64_t n = a.known_value();
                        for (size_t i=0; i<Len; ++i) {
                            if (n & ((uint64_t)1 << i))
                                return number<Len>(i).defined_by(cur_insn, a.get_defining_instructions());
                        }
                        return number<Len>(0).defined_by(cur_insn, a.get_defining_instructions());
                    }
                    return ValueType<Len>(InternalNode::create(Len, InsnSemanticsExpr::OP_LSSB, a.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions());
                }

                /** See NullSemantics::Policy::mostSignificantSetBit() */
                template <size_t Len>
                ValueType<Len> mostSignificantSetBit(const ValueType<Len> &a) const {
                    if (a.is_known()) {
                        uint64_t n = a.known_value();
                        for (size_t i=Len; i>0; --i) {
                            if (n & ((uint64_t)1 << (i-1)))
                                return number<Len>(i-1).defined_by(cur_insn, a.get_defining_instructions());
                        }
                        return number<Len>(0);
                    }
                    return ValueType<Len>(InternalNode::create(Len, InsnSemanticsExpr::OP_MSSB, a.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions());
                }

                /** See NullSemantics::Policy::negate() */
                template <size_t Len>
                ValueType<Len> negate(const ValueType<Len> &a) const {
                    if (a.is_known())
                        return ValueType<Len>(-a.known_value()).defined_by(cur_insn, a.get_defining_instructions());
                    return ValueType<Len>(InternalNode::create(Len, InsnSemanticsExpr::OP_NEGATE, a.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions());
                }

                /** See NullSemantics::Policy::or_() */
                template <size_t Len>
                ValueType<Len> or_(const ValueType<Len> &a, const ValueType<Len> &b) const {
                    if (a.is_known() && b.is_known())
                        return ValueType<Len>(a.known_value() | b.known_value())
                            .defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
                    return ValueType<Len>(InternalNode::create(Len, InsnSemanticsExpr::OP_BV_OR,
                                                               a.get_expression(), b.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
                }

                /** See NullSemantics::Policy::rotateLeft() */
                template <size_t Len, size_t SALen>
                ValueType<Len> rotateLeft(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
                    if (a.is_known() && sa.is_known())
                        return ValueType<Len>(IntegerOps::rotateLeft<Len>(a.known_value(), sa.known_value()))
                            .defined_by(cur_insn, a.get_defining_instructions(), sa.get_defining_instructions());
                    return ValueType<Len>(InternalNode::create(Len, InsnSemanticsExpr::OP_ROL,
                                                               sa.get_expression(), a.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions(), sa.get_defining_instructions());
                }

                /** See NullSemantics::Policy::rotateRight() */
                template <size_t Len, size_t SALen>
                ValueType<Len> rotateRight(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
                    if (a.is_known() && sa.is_known())
                        return ValueType<Len>(IntegerOps::rotateRight<Len>(a.known_value(), sa.known_value()))
                            .defined_by(cur_insn, a.get_defining_instructions(), sa.get_defining_instructions());
                    return ValueType<Len>(InternalNode::create(Len, InsnSemanticsExpr::OP_ROR,
                                                               sa.get_expression(), a.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions(), sa.get_defining_instructions());
                }

                /** See NullSemantics::Policy::shiftLeft() */
                template <size_t Len, size_t SALen>
                ValueType<Len> shiftLeft(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
                    if (a.is_known() && sa.is_known())
                        return ValueType<Len>(IntegerOps::shiftLeft<Len>(a.known_value(), sa.known_value()))
                            .defined_by(cur_insn, a.get_defining_instructions(), sa.get_defining_instructions());
                    return ValueType<Len>(InternalNode::create(Len, InsnSemanticsExpr::OP_SHL0,
                                                               sa.get_expression(), a.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions(), sa.get_defining_instructions());
                }

                /** See NullSemantics::Policy::shiftRight() */
                template <size_t Len, size_t SALen>
                ValueType<Len> shiftRight(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
                    if (a.is_known() && sa.is_known())
                        return ValueType<Len>(IntegerOps::shiftRightLogical<Len>(a.known_value(), sa.known_value()))
                            .defined_by(cur_insn, a.get_defining_instructions(), sa.get_defining_instructions());
                    return ValueType<Len>(InternalNode::create(Len, InsnSemanticsExpr::OP_SHR0,
                                                               sa.get_expression(), a.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions(), sa.get_defining_instructions());
                }

                /** See NullSemantics::Policy::shiftRightArithmetic() */
                template <size_t Len, size_t SALen>
                ValueType<Len> shiftRightArithmetic(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
                    if (a.is_known() && sa.is_known())
                        return ValueType<Len>(IntegerOps::shiftRightArithmetic<Len>(a.known_value(), sa.known_value()))
                            .defined_by(cur_insn, a.get_defining_instructions(), sa.get_defining_instructions());
                    return ValueType<Len>(InternalNode::create(Len, InsnSemanticsExpr::OP_ASR,
                                                               sa.get_expression(), a.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions(), sa.get_defining_instructions());
                }

                /** See NullSemantics::Policy::signExtend() */
                template <size_t From, size_t To>
                ValueType<To> signExtend(const ValueType<From> &a) {
                    return signedExtend<From, To>(a);
                }

                /** See NullSemantics::Policy::signedDivide() */
                template <size_t Len1, size_t Len2>
                ValueType<Len1> signedDivide(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
                    if (a.is_known() && b.is_known() && 0!=b.known_value())
                        return ValueType<Len1>(IntegerOps::signExtend<Len1, 64>(a.known_value()) /
                                               IntegerOps::signExtend<Len2, 64>(b.known_value()))
                            .defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
                    return ValueType<Len1>(InternalNode::create(Len1, InsnSemanticsExpr::OP_SDIV,
                                                                a.get_expression(), b.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
                }

                /** See NullSemantics::Policy::signedModulo() */
                template <size_t Len1, size_t Len2>
                ValueType<Len2> signedModulo(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
                    if (a.is_known() && b.is_known() && 0!=b.known_value())
                        return ValueType<Len2>(IntegerOps::signExtend<Len1, 64>(a.known_value()) %
                                               IntegerOps::signExtend<Len2, 64>(b.known_value()))
                            .defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
                    return ValueType<Len2>(InternalNode::create(Len2, InsnSemanticsExpr::OP_SMOD,
                                                                a.get_expression(), b.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
                }

                /** See NullSemantics::Policy::signedMultiply() */
                template <size_t Len1, size_t Len2>
                ValueType<Len1+Len2> signedMultiply(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
                    if (a.is_known() && b.is_known())
                        return ValueType<Len1+Len2>(IntegerOps::signExtend<Len1, 64>(a.known_value()) *
                                                    IntegerOps::signExtend<Len2, 64>(b.known_value()))
                            .defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
                    return ValueType<Len1+Len2>(InternalNode::create(Len1+Len2, InsnSemanticsExpr::OP_SMUL,
                                                                     a.get_expression(), b.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
                }

                /** See NullSemantics::Policy::unsignedDivide() */
                template <size_t Len1, size_t Len2>
                ValueType<Len1> unsignedDivide(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
                    if (a.is_known() && b.is_known() && 0!=b.known_value())
                        return ValueType<Len1>(a.known_value() / b.known_value())
                            .defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
                    return ValueType<Len1>(InternalNode::create(Len1, InsnSemanticsExpr::OP_UDIV,
                                                                a.get_expression(), b.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
                }

                /** See NullSemantics::Policy::unsignedModulo() */
                template <size_t Len1, size_t Len2>
                ValueType<Len2> unsignedModulo(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
                    if (a.is_known() && b.is_known() && 0!=b.known_value())
                        return ValueType<Len2>(a.known_value() % b.known_value())
                            .defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
                    return ValueType<Len2>(InternalNode::create(Len2, InsnSemanticsExpr::OP_UMOD,
                                                                a.get_expression(), b.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
                }

                /** See NullSemantics::Policy::unsignedMultiply() */
                template <size_t Len1, size_t Len2>
                ValueType<Len1+Len2> unsignedMultiply(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
                    if (a.is_known() && b.is_known())
                        return ValueType<Len1+Len2>(a.known_value()*b.known_value())
                            .defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
                    return ValueType<Len1+Len2>(InternalNode::create(Len1+Len2, InsnSemanticsExpr::OP_UMUL,
                                                                     a.get_expression(), b.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
                }

                /** See NullSemantics::Policy::xor_() */
                template <size_t Len>
                ValueType<Len> xor_(const ValueType<Len> &a, const ValueType<Len> &b) const {
                    if (a.is_known() && b.is_known())
                        return ValueType<Len>(a.known_value() ^ b.known_value())
                            .defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
                    if (a.get_expression()->equal_to(b.get_expression(), solver))
                        return number<Len>(0).defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
                    return ValueType<Len>(InternalNode::create(Len, InsnSemanticsExpr::OP_BV_XOR,
                                                               a.get_expression(), b.get_expression()))
                        .defined_by(cur_insn, a.get_defining_instructions(), b.get_defining_instructions());
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
                            if (reg.get_minor()!=0 || reg.get_offset()>=cur_state.n_flags)
                                throw Exception("register not implemented in semantic policy");
                            if (reg.get_nbits()!=1)
                                throw Exception("semantic policy supports only single-bit flags");
                            return unsignedExtend<1, Len>(cur_state.flag[reg.get_offset()]);

                        case 8:
                            // Only general-purpose registers can be accessed at a byte granularity, and we can access only the
                            // low-order byte or the next higher byte.  For instance, "al" and "ah" registers.
                            if (reg.get_major()!=x86_regclass_gpr)
                                throw Exception("byte access only valid for general purpose registers");
                            if (reg.get_minor()>=cur_state.n_gprs)
                                throw Exception("register not implemented in semantic policy");
                            assert(reg.get_nbits()==8); // we had better be asking for a one-byte register (e.g., "ah", not "ax")
                            switch (reg.get_offset()) {
                                case 0:
                                    return extract<0, Len>(cur_state.gpr[reg.get_minor()]);
                                case 8:
                                    return extract<8, 8+Len>(cur_state.gpr[reg.get_minor()]);
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
                                    if (reg.get_minor()>=cur_state.n_segregs)
                                        throw Exception("register not implemented in semantic policy");
                                    return unsignedExtend<16, Len>(cur_state.segreg[reg.get_minor()]);
                                case x86_regclass_gpr:
                                    if (reg.get_minor()>=cur_state.n_gprs)
                                        throw Exception("register not implemented in semantic policy");
                                    return extract<0, Len>(cur_state.gpr[reg.get_minor()]);
                                case x86_regclass_flags:
                                    if (reg.get_minor()!=0 || cur_state.n_flags<16)
                                        throw Exception("register not implemented in semantic policy");
                                    return unsignedExtend<16, Len>(concat(cur_state.flag[0],
                                                                   concat(cur_state.flag[1],
                                                                   concat(cur_state.flag[2],
                                                                   concat(cur_state.flag[3],
                                                                   concat(cur_state.flag[4],
                                                                   concat(cur_state.flag[5],
                                                                   concat(cur_state.flag[6],
                                                                   concat(cur_state.flag[7],
                                                                   concat(cur_state.flag[8],
                                                                   concat(cur_state.flag[9],
                                                                   concat(cur_state.flag[10],
                                                                   concat(cur_state.flag[11],
                                                                   concat(cur_state.flag[12],
                                                                   concat(cur_state.flag[13],
                                                                   concat(cur_state.flag[14],
                                                                          cur_state.flag[15]))))))))))))))));
                                default:
                                    throw Exception("word access not valid for this register type");
                            }

                        case 32:
                            if (reg.get_offset()!=0)
                                throw Exception("policy does not support non-zero offsets for double word granularity"
                                                " register access");
                            switch (reg.get_major()) {
                                case x86_regclass_gpr:
                                    if (reg.get_minor()>=cur_state.n_gprs)
                                        throw Exception("register not implemented in semantic policy");
                                    return unsignedExtend<32, Len>(cur_state.gpr[reg.get_minor()]);
                                case x86_regclass_ip:
                                    if (reg.get_minor()!=0)
                                        throw Exception("register not implemented in semantic policy");
                                    return unsignedExtend<32, Len>(cur_state.ip);
                                case x86_regclass_segment:
                                    if (reg.get_minor()>=cur_state.n_segregs || reg.get_nbits()!=16)
                                        throw Exception("register not implemented in semantic policy");
                                    return unsignedExtend<16, Len>(cur_state.segreg[reg.get_minor()]);
                                case x86_regclass_flags: {
                                    if (reg.get_minor()!=0 || cur_state.n_flags<32)
                                        throw Exception("register not implemented in semantic policy");
                                    if (reg.get_nbits()!=32)
                                        throw Exception("register is not 32 bits");
                                    return unsignedExtend<32, Len>(concat(readRegister<16>("flags"), // no-op sign extension
                                                                   concat(cur_state.flag[16],
                                                                   concat(cur_state.flag[17],
                                                                   concat(cur_state.flag[18],
                                                                   concat(cur_state.flag[19],
                                                                   concat(cur_state.flag[20],
                                                                   concat(cur_state.flag[21],
                                                                   concat(cur_state.flag[22],
                                                                   concat(cur_state.flag[23],
                                                                   concat(cur_state.flag[24],
                                                                   concat(cur_state.flag[25],
                                                                   concat(cur_state.flag[26],
                                                                   concat(cur_state.flag[27],
                                                                   concat(cur_state.flag[28],
                                                                   concat(cur_state.flag[29],
                                                                   concat(cur_state.flag[30],
                                                                          cur_state.flag[31])))))))))))))))));
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
                            if (reg.get_minor()!=0 || reg.get_offset()>=cur_state.n_flags)
                                throw Exception("register not implemented in semantic policy");
                            if (reg.get_nbits()!=1)
                                throw Exception("semantic policy supports only single-bit flags");
                            cur_state.flag[reg.get_offset()] = unsignedExtend<Len, 1>(value);
                            cur_state.flag[reg.get_offset()].defined_by(cur_insn);
                            break;

                        case 8:
                            // Only general purpose registers can be accessed at byte granularity, and only for offsets 0 and 8.
                            if (reg.get_major()!=x86_regclass_gpr)
                                throw Exception("byte access only valid for general purpose registers.");
                            if (reg.get_minor()>=cur_state.n_gprs)
                                throw Exception("register not implemented in semantic policy");
                            assert(reg.get_nbits()==8); // we had better be asking for a one-byte register (e.g., "ah", not "ax")
                            switch (reg.get_offset()) {
                                case 0:
                                    cur_state.gpr[reg.get_minor()] =                                    // no-op extend
                                        concat(signExtend<Len, 8>(value), extract<8, 32>(cur_state.gpr[reg.get_minor()]));
                                    cur_state.gpr[reg.get_minor()].defined_by(cur_insn);
                                    break;
                                case 8:
                                    cur_state.gpr[reg.get_minor()] =
                                        concat(extract<0, 8>(cur_state.gpr[reg.get_minor()]),
                                               concat(unsignedExtend<Len, 8>(value),
                                                      extract<16, 32>(cur_state.gpr[reg.get_minor()])));
                                    cur_state.gpr[reg.get_minor()].defined_by(cur_insn);
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
                                    if (reg.get_minor()>=cur_state.n_segregs)
                                        throw Exception("register not implemented in semantic policy");
                                    cur_state.segreg[reg.get_minor()] = unsignedExtend<Len, 16>(value);
                                    cur_state.segreg[reg.get_minor()].defined_by(cur_insn);
                                    break;
                                case x86_regclass_gpr:
                                    if (reg.get_minor()>=cur_state.n_gprs)
                                        throw Exception("register not implemented in semantic policy");
                                    cur_state.gpr[reg.get_minor()] =
                                        concat(unsignedExtend<Len, 16>(value),
                                               extract<16, 32>(cur_state.gpr[reg.get_minor()]));
                                    cur_state.gpr[reg.get_minor()].defined_by(cur_insn);
                                    break;
                                case x86_regclass_flags:
                                    if (reg.get_minor()!=0 || cur_state.n_flags<16)
                                        throw Exception("register not implemented in semantic policy");
                                    cur_state.flag[0]  = extract<0,  1 >(value); cur_state.flag[0].defined_by(cur_insn);
                                    cur_state.flag[1]  = extract<1,  2 >(value); cur_state.flag[1].defined_by(cur_insn);
                                    cur_state.flag[2]  = extract<2,  3 >(value); cur_state.flag[2].defined_by(cur_insn);
                                    cur_state.flag[3]  = extract<3,  4 >(value); cur_state.flag[3].defined_by(cur_insn);
                                    cur_state.flag[4]  = extract<4,  5 >(value); cur_state.flag[4].defined_by(cur_insn);
                                    cur_state.flag[5]  = extract<5,  6 >(value); cur_state.flag[5].defined_by(cur_insn);
                                    cur_state.flag[6]  = extract<6,  7 >(value); cur_state.flag[6].defined_by(cur_insn);
                                    cur_state.flag[7]  = extract<7,  8 >(value); cur_state.flag[7].defined_by(cur_insn);
                                    cur_state.flag[8]  = extract<8,  9 >(value); cur_state.flag[8].defined_by(cur_insn);
                                    cur_state.flag[9]  = extract<9,  10>(value); cur_state.flag[9].defined_by(cur_insn);
                                    cur_state.flag[10] = extract<10, 11>(value); cur_state.flag[10].defined_by(cur_insn);
                                    cur_state.flag[11] = extract<11, 12>(value); cur_state.flag[11].defined_by(cur_insn);
                                    cur_state.flag[12] = extract<12, 13>(value); cur_state.flag[12].defined_by(cur_insn);
                                    cur_state.flag[13] = extract<13, 14>(value); cur_state.flag[13].defined_by(cur_insn);
                                    cur_state.flag[14] = extract<14, 15>(value); cur_state.flag[14].defined_by(cur_insn);
                                    cur_state.flag[15] = extract<15, 16>(value); cur_state.flag[15].defined_by(cur_insn);
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
                                    if (reg.get_minor()>=cur_state.n_gprs)
                                        throw Exception("register not implemented in semantic policy");
                                    cur_state.gpr[reg.get_minor()] = signExtend<Len, 32>(value);
                                    cur_state.gpr[reg.get_minor()].defined_by(cur_insn);
                                    break;
                                case x86_regclass_ip:
                                    if (reg.get_minor()!=0)
                                        throw Exception("register not implemented in semantic policy");
                                    cur_state.ip = unsignedExtend<Len, 32>(value);
                                    cur_state.ip.defined_by(cur_insn);
                                    break;
                                case x86_regclass_flags:
                                    if (reg.get_minor()!=0 || cur_state.n_flags<32)
                                        throw Exception("register not implemented in semantic policy");
                                    if (reg.get_nbits()!=32)
                                        throw Exception("register is not 32 bits");
                                    writeRegister<16>("flags", unsignedExtend<Len, 16>(value));
                                    cur_state.flag[16] = extract<16, 17>(value); cur_state.flag[16].defined_by(cur_insn);
                                    cur_state.flag[17] = extract<17, 18>(value); cur_state.flag[17].defined_by(cur_insn);
                                    cur_state.flag[18] = extract<18, 19>(value); cur_state.flag[18].defined_by(cur_insn);
                                    cur_state.flag[19] = extract<19, 20>(value); cur_state.flag[19].defined_by(cur_insn);
                                    cur_state.flag[20] = extract<20, 21>(value); cur_state.flag[20].defined_by(cur_insn);
                                    cur_state.flag[21] = extract<21, 22>(value); cur_state.flag[21].defined_by(cur_insn);
                                    cur_state.flag[22] = extract<22, 23>(value); cur_state.flag[22].defined_by(cur_insn);
                                    cur_state.flag[23] = extract<23, 24>(value); cur_state.flag[23].defined_by(cur_insn);
                                    cur_state.flag[24] = extract<24, 25>(value); cur_state.flag[24].defined_by(cur_insn);
                                    cur_state.flag[25] = extract<25, 26>(value); cur_state.flag[25].defined_by(cur_insn);
                                    cur_state.flag[26] = extract<26, 27>(value); cur_state.flag[26].defined_by(cur_insn);
                                    cur_state.flag[27] = extract<27, 28>(value); cur_state.flag[27].defined_by(cur_insn);
                                    cur_state.flag[28] = extract<28, 29>(value); cur_state.flag[28].defined_by(cur_insn);
                                    cur_state.flag[29] = extract<29, 30>(value); cur_state.flag[29].defined_by(cur_insn);
                                    cur_state.flag[30] = extract<30, 31>(value); cur_state.flag[30].defined_by(cur_insn);
                                    cur_state.flag[31] = extract<31, 32>(value); cur_state.flag[31].defined_by(cur_insn);
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
                    return mem_read<Len>(cur_state, addr, ValueType<Len>());
                }

                /** See NullSemantics::Policy::writeMemory() */
                template <size_t Len> void
                writeMemory(X86SegmentRegister segreg, const ValueType<32> &addr, const ValueType<Len> &data,
                            const ValueType<1> &cond) {
                    mem_write<Len>(cur_state, addr, data);
                }
            };
        } /*namespace*/
    } /*namespace*/
} /*namespace*/

#endif
