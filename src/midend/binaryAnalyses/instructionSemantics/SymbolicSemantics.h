#ifndef Rose_SymbolicSemantics_H
#define Rose_SymbolicSemantics_H
#include <stdint.h>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#include "x86InstructionSemantics.h"
#include "SMTSolver.h"

#include <map>
#include <vector>


/** A policy for x86InstructionSemantics.
 *
 *  This policy can be used to emulate the execution of a single basic block of instructions.  It is similar in nature to
 *  VirtualMachineSemantics, but with a different type of ValueType: instead of values being a constant or variable with
 *  offset, values here are expression trees.
 *
 *  <ul>
 *    <li>Policy: the policy class used to instantiate X86InstructionSemantic instances.</li>
 *    <li>State: represents the state of the virtual machine, its registers and memory.</li>
 *    <li>ValueType: the values stored in registers and memory and used for memory addresses.</li>
 *  </ul>
 *
 *  If an SMT solver is supplied as a Policy constructor argument then that SMT solver will be used to answer various
 *  questions such as when two memory addresses can alias one another.  When an SMT solver is lacking, the questions will be
 *  answered by very naive comparison of the expression trees. */
namespace SymbolicSemantics {

    typedef InsnSemanticsExpr::RenameMap RenameMap;
    typedef InsnSemanticsExpr::LeafNode LeafNode;
    typedef InsnSemanticsExpr::InternalNode InternalNode;
    typedef InsnSemanticsExpr::TreeNode TreeNode;

    /* ValueType cannot directly be a TreeNode because ValueType's bit size is a template argument while tree node sizes are
     * stored as a data member.  Therefore, ValueType will always point to a TreeNode.  Most of the methods that are invoked on
     * ValueType just call the same methods for TreeNode. */
    template<size_t nBits>
    struct ValueType {

        TreeNode *expr; /*reference counted*/

        /** Construct a value that is unknown and unique. */
        ValueType() {
            expr = LeafNode::create_variable(nBits);
            expr->inc_nrefs();
        }

        ValueType(const ValueType &other) {
            expr = other.expr;
            expr->inc_nrefs();
        }

        ValueType& operator=(const ValueType &other) {
            expr->dec_nrefs();
            expr->deleteDeeply();
            expr = other.expr;
            expr->inc_nrefs();
            return *this;
        }

        /** Construct a ValueType with a known value. */
        explicit ValueType(uint64_t n) {
            expr = LeafNode::create_integer(nBits, n);
            expr->inc_nrefs();
        }

        /** Construct a ValueType from a TreeNode. */
        explicit ValueType(TreeNode *node) {
            assert(node->get_nbits()==nBits);
            expr = node;
            expr->inc_nrefs();
        }

        ~ValueType() {
            expr->dec_nrefs();
            expr->deleteDeeply();
        }

        /** Print the value. If a rename map is specified a named value will be renamed to have a shorter name.  See the rename()
         *  method for details. */
        void print(std::ostream &o, RenameMap *rmap=NULL) const {
            expr->print(o, rmap);
        }

        /** Returns true if the value is a known constant. */
        bool is_known() const {
            return expr->is_known();
        }

        /** Returns the value of a known constant. Assumes this value is a known constant. */
        uint64_t known_value() const {
            LeafNode *leaf = dynamic_cast<LeafNode*>(expr);
            assert(leaf);
            return leaf->get_value();
        }
    };

    template<size_t Len>
    std::ostream& operator<<(std::ostream &o, const ValueType<Len> &e) {
        e.print(o, NULL);
        return o;
    }

    /** Represents one location in memory. Has an address data and size in bytes.
     *
     *  When a state is created, every register and memory location will be given a unique named value. However, it's not
     *  practicle to store a named value for every possible memory address, yet we want the following example to work correctly:
     *  \code
     *  1: mov eax, ds:[edx]    // first read returns V1
     *  2: mov eax, ds:[edx]    // subsequent reads from same address also return V1
     *  3: mov ds:[ecx], eax    // write to unknown address clobbers all memory
     *  4: mov eax, ds:[edx]    // read from same address as above returns V2
     *  5: mov eax, ds:[edx]    // subsequent reads from same address also return V2
     *  \endcode
     *
     *  Furthermore, the read from ds:[edx] at #1 above, retroactively stores V1 in the original memory state. That way if we need
     *  to do additional analyses starting from the same initial state it will be available to use.
     *
     *  To summarize: every memory address is given a unique named value. These values are implicit until the memory location is
     *  actually read.
     *
     *  See also readMemory() and writeMemory(). */
    struct MemoryCell {
        ValueType<32> address;
        ValueType<32> data;
        size_t nbytes;
        bool clobbered;             /* Set to invalidate possible aliases during writeMemory() */
        bool written;               /* Set to true by writeMemory */

        template <size_t Len>
        MemoryCell(const ValueType<32> &address, const ValueType<Len> data, size_t nbytes)
            : address(address), data(data), nbytes(nbytes), clobbered(false), written(false) {}

        bool is_clobbered() const { return clobbered; }
        void set_clobbered() { clobbered = true; }
        bool is_written() const { return written; }
        void set_written() { written = true; }

        /** Returns true if this memory value could possibly overlap with the @p other memory value.  In other words, returns false
         *  only if this memory location cannot overlap with @p other memory location. Two addresses that are identical alias one
         *  another. The @p solver is optional but recommended (absence of a solver will result in a naive definition). */
        bool may_alias(const MemoryCell &other, SMTSolver *solver) const;

        /** Returns true if this memory address is the same as the @p other. Note that "same" is more strict than "overlap".
         *  The @p solver is optional but recommended (absence of a solver will result in a naive definition). */
        bool must_alias(const MemoryCell &other, SMTSolver *solver) const;

        /** Prints the value of a memory cell on a single line. If a rename map is specified then named values will be renamed to
         *  have a shorter name.  See the ValueType<>::rename() method for details. */
        void print(std::ostream &o, RenameMap *rmap=NULL) const;
    };

    typedef std::vector<MemoryCell> Memory;

    /** Represents the entire state of the machine. However, the instruction pointer is not included in the state. */
    struct State {
        static const size_t n_gprs = 8;             /**< Number of general-purpose registers in this state. */
        static const size_t n_segregs = 6;          /**< Number of segmentation registers in this state. */
        static const size_t n_flags = 16;           /**< Number of flag registers in this state. */

        ValueType<32> ip;                           /**< Instruction pointer. */
        ValueType<32> gpr[n_gprs];                  /**< General-purpose registers */
        ValueType<16> segreg[n_segregs];            /**< Segmentation registers. */
        ValueType<1> flag[n_flags];                 /**< Control/status flags (i.e., FLAG register). */
        Memory mem;                                 /**< Core memory. */

        /** Print the state in a human-friendly way.  If a rename map is specified then named values will be renamed to have a
         *  shorter name.  See the ValueType<>::rename() method for details. */
        void print(std::ostream &o, RenameMap *rmap=NULL) const;

        /** Print info about how registers differ.  If a rename map is specified then named values will be renamed to have a
         *  shorter name.  See the ValueType<>::rename() method for details. */
        void print_diff_registers(std::ostream &o, const State&, RenameMap *rmap=NULL) const;

        /** Tests registers of two states for equality. */
        bool equal_registers(const State&) const;

        /** Removes from memory those values at addresses below the current stack pointer. This is automatically called after each
         *  instruction if the policy's p_discard_popped_memory property is set. */
        void discard_popped_memory() {
            /*FIXME: not implemented yet. [RPM 2010-05-24]*/
        }

    };

    std::ostream& operator<<(std::ostream &o, const MemoryCell& mc);
    std::ostream& operator<<(std::ostream &o, const State& state);

    /** A policy that is supplied to the semantic analysis constructor. See documentation for the SymbolicSemantics namespace. */
    class Policy {
    private:
        SgAsmInstruction *cur_insn;         /**< Set by startInstruction(), cleared by finishInstruction() */
        mutable State orig_state;           /**< Original machine state, initialized by constructor and mem_write. This data
                                             *   member is mutable because a mem_read() operation, although conceptually const,
                                             *   may cache the value that was read so that subsquent reads from the same address
                                             *   will return the same value. This member is initialized by the first call to
                                             *   startInstruction() (as called by X86InstructionSemantics::processInstruction())
                                             *   which allows the user to initialize the original conditions using the same
                                             *   interface that's used to process instructions.  In other words, if one wants the
                                             *   stack pointer to contain a specific original value, then one may initialize the
                                             *   stack pointer by calling writeGPR() before processing the first instruction. */
        mutable State cur_state;            /**< Current machine state updated by each processInstruction().  The instruction
                                             *   pointer is updated before we process each instruction. This data member is
                                             *   mutable because a mem_read() operation, although conceptually const, may cache
                                             *   the value that was read so that subsequent reads from the same address will
                                             *   return the same value. */
        bool p_discard_popped_memory;       /**< Property that determines how the stack behaves.  When set, any time the stack
                                             * pointer is adjusted, memory below the stack pointer and having the same address
                                             * name as the stack pointer is removed (the memory location becomes undefined). The
                                             * default is false, that is, no special treatment for the stack. */
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
        const State& get_state() const { return cur_state; }
        State& get_state() { return cur_state; }

        /** Returns the original state.  The original state is initialized to be equal to the current state twice: once by the
         *  constructor, and then again when the first instruction is processed. */
        const State& get_orig_state() const { return orig_state; }
        State& get_orig_state() { return orig_state; }

        /** Returns the current instruction pointer. */
        const ValueType<32>& get_ip() const { return cur_state.ip; }

        /** Returns the original instruction pointer. See also get_orig_state(). */
        const ValueType<32>& get_orig_ip() const { return orig_state.ip; }

        /** Returns a copy of the state after removing memory that is not pertinent to an equal_states() comparison. */
        Memory memory_for_equality(const State&) const;

        /** Returns a copy of the current state after removing memory that is not pertinent to an equal_states() comparison. */
        Memory memory_for_equality() const { return memory_for_equality(cur_state); }

        /** Compares two states for equality. The comarison looks at all register values and the memory locations that are
         *  different than their original value (but excluding differences due to clobbering). It does not compare memory that has
         *  only been read. */
        bool equal_states(const State&, const State&) const;

        /** Print the current state of this policy.  If a rename map is specified then named values will be renamed to have a
         *  shorter name.  See the ValueType<>::rename() method for details. */
        void print(std::ostream&, RenameMap *rmap=NULL) const;
        friend std::ostream& operator<<(std::ostream &o, const Policy &p) {
            p.print(o, NULL);
            return o;
        }

        /** Returns true if the specified value exists in memory and is provably at or above the stack pointer.  The stack pointer
         *  need not have a known value. */
        bool on_stack(const ValueType<32> &value) const;

        /** Changes how the policy treats the stack.  See the p_discard_popped_memory property data member for details. */
        void set_discard_popped_memory(bool b) {
            p_discard_popped_memory = b;
        }

        /** Returns the current setting for the property that determines how the stack behaves. See the
         *  p_set_discard_popped_memory property data member for details. */
        bool get_discard_popped_memory() const {
            return p_discard_popped_memory;
        }

        /** Print only the differences between two states.  If a rename map is specified then named values will be renamed to
         *  have a shorter name.  See the ValueType<>::rename() method for details. */
        void print_diff(std::ostream&, const State&, const State&, RenameMap *rmap=NULL) const ;

        /** Print the difference between a state and the initial state.  If a rename map is specified then named values will be
         *  renamed to have a shorter name.  See the ValueType<>::rename() method for details. */
        void print_diff(std::ostream &o, const State &state, RenameMap *rmap=NULL) const {
            print_diff(o, orig_state, state, rmap);
        }

        /** Print the difference between the current state and the initial state.  If a rename map is specified then named values
         *  will be renamed to have a shorter name.  See the ValueType<>::rename() method for details. */
        void print_diff(std::ostream &o, RenameMap *rmap=NULL) const {
            print_diff(o, orig_state, cur_state, rmap);
        }

        /** Returns the SHA1 hash of the difference between the current state and the original state.  If libgcrypt is not
         *  available then the return value will be an empty string. */
        std::string SHA1() const;

        /** Extend (or shrink) from @p FromLen bits to @p ToLen bits by adding or removing high-order bits from the input. Added
         *  bits are always zeros. */
        template <size_t FromLen, size_t ToLen>
        ValueType<ToLen> unsignedExtend(const ValueType<FromLen> &a) const {
            if (a.is_known())
                return ValueType<ToLen>(IntegerOps::GenMask<uint64_t,ToLen>::value & a.known_value());
            if (FromLen==ToLen)
                return ValueType<ToLen>(a.expr);
            if (FromLen>ToLen)
                return ValueType<ToLen>(new InternalNode(ToLen, InsnSemanticsExpr::OP_EXTRACT,
                                                         LeafNode::create_integer(32, 0),
                                                         LeafNode::create_integer(32, ToLen),
                                                         a.expr));
            return ValueType<ToLen>(new InternalNode(ToLen, InsnSemanticsExpr::OP_UEXTEND,
                                                     LeafNode::create_integer(32, ToLen), a.expr));
        }

        /** Sign extend from @p FromLen bits to @p ToLen bits. */
        template <size_t FromLen, size_t ToLen>
        ValueType<ToLen> signedExtend(const ValueType<FromLen> &a) const {
            if (a.is_known())
                return ValueType<ToLen>(IntegerOps::signExtend<FromLen, ToLen>(a.known_value()));
            if (FromLen==ToLen)
                return ValueType<ToLen>(a.expr);
            if (FromLen > ToLen)
                return ValueType<ToLen>(new InternalNode(ToLen, InsnSemanticsExpr::OP_EXTRACT,
                                                         LeafNode::create_integer(32, 0),
                                                         LeafNode::create_integer(32, ToLen),
                                                         a.expr));
            return ValueType<ToLen>(new InternalNode(ToLen, InsnSemanticsExpr::OP_SEXTEND,
                                                     LeafNode::create_integer(32, ToLen), a.expr));
        }

        /** Extracts certain bits from the specified value and shifts them to the low-order positions in the result.  The bits of
         *  the result include bits from BeginAt (inclusive) through EndAt (exclusive).  The lsb is numbered zero. */
        template <size_t BeginAt, size_t EndAt, size_t Len>
        ValueType<EndAt-BeginAt> extract(const ValueType<Len> &a) const {
            if (0==BeginAt)
                return unsignedExtend<Len,EndAt-BeginAt>(a);
            if (a.is_known())
                return ValueType<EndAt-BeginAt>(a.known_value());
            return ValueType<EndAt-BeginAt>(new InternalNode(EndAt-BeginAt, InsnSemanticsExpr::OP_EXTRACT,
                                                             LeafNode::create_integer(32, BeginAt),
                                                             LeafNode::create_integer(32, EndAt),
                                                             a.expr));
        }

        /** Reads a value from memory in a way that always returns the same value provided there are not intervening writes that
         *  would clobber the value either directly or by aliasing.  Also, if appropriate, the value is added to the original
         *  memory state (thus changing the value at that address from an implicit named value to an explicit named value).
         *
         *  It is safe to call this function and supply the policy's original state as the state argument.
         *
         *  The documentation for MemoryCell has an example that demonstrates the desired behavior of mem_read() and
         *  mem_write(). */
        template <size_t Len> ValueType<Len> mem_read(State &state, const ValueType<32> &addr) const {
            MemoryCell new_cell(addr, ValueType<32>(), Len/8);
            bool aliased = false; /*is new_cell aliased by any existing writes?*/

            for (Memory::iterator mi=state.mem.begin(); mi!=state.mem.end(); ++mi) {
                if (new_cell.must_alias(*mi, solver)) {
                    if ((*mi).clobbered) {
                        (*mi).clobbered = false;
                        (*mi).data = new_cell.data;
                        return unsignedExtend<32, Len>(new_cell.data);
                    } else {
                        return unsignedExtend<32, Len>((*mi).data);
                    }
                } else if ((*mi).written && new_cell.may_alias(*mi, solver)) {
                    aliased = true;
                }
            }

            if (!aliased && &state!=&orig_state) {
                /* We didn't find the memory cell in the specified state and it's not aliased to any writes in that state.
                 * Therefore use the value from the initial memory state (creating it if necessary). */
                for (Memory::iterator mi=orig_state.mem.begin(); mi!=orig_state.mem.end(); ++mi) {
                    if (new_cell.must_alias(*mi, solver)) {
                        ROSE_ASSERT(!(*mi).clobbered);
                        ROSE_ASSERT(!(*mi).written);
                        state.mem.push_back(*mi);
                        return unsignedExtend<32, Len>((*mi).data);
                    }
                }

                orig_state.mem.push_back(new_cell);
            }

            /* Create the cell in the current state. */
            state.mem.push_back(new_cell);
            return unsignedExtend<32,Len>(new_cell.data);
        }

        /** See memory_reference_type(). */
        enum MemRefType { MRT_STACK_PTR, MRT_FRAME_PTR, MRT_OTHER_PTR };

        /** Determines if the specified address is related to the current stack or frame pointer. This is used by mem_write() when
         *  we're operating under the assumption that memory written via stack pointer is different than memory written via frame
         *  pointer, and that memory written by either pointer is different than all other memory. */
        MemRefType memory_reference_type(const State &state, const ValueType<32> &addr) const {
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

        /** Writes a value to memory. If the address written to is an alias for other addresses then the other addresses will be
         *  clobbered. Subsequent reads from clobbered addresses will return new values. See also, mem_read(). */
        template <size_t Len> void mem_write(State &state, const ValueType<32> &addr, const ValueType<Len> &data) {
            ROSE_ASSERT(&state!=&orig_state);
            MemoryCell new_cell(addr, unsignedExtend<Len, 32>(data), Len/8);
            new_cell.set_written();
            bool saved = false; /* has new_cell been saved into memory? */

            /* Is new memory reference through the stack pointer or frame pointer? */
            MemRefType new_mrt = memory_reference_type(state, addr);

            /* Overwrite and/or clobber existing memory locations. */
            for (Memory::iterator mi=state.mem.begin(); mi!=state.mem.end(); ++mi) {
                if (new_cell.must_alias(*mi, solver)) {
                    *mi = new_cell;
                    saved = true;
                } else if (p_discard_popped_memory && new_mrt!=memory_reference_type(state, (*mi).address)) {
                    /* Assume that memory referenced through the stack pointer does not alias that which is referenced through the
                     * frame pointer, and neither of them alias memory that is referenced other ways. */
                } else if (new_cell.may_alias(*mi, solver)) {
                    (*mi).set_clobbered();
                } else {
                    /* memory cell *mi is not aliased to cell being written */
                }
            }
            if (!saved)
                state.mem.push_back(new_cell);
        }


        /*************************************************************************************************************************
         * Functions invoked by the X86InstructionSemantics class for every processed instructions
         *************************************************************************************************************************/

        /* Called at the beginning of X86InstructionSemantics::processInstruction() */
        void startInstruction(SgAsmInstruction *insn) {
            cur_state.ip = ValueType<32>(insn->get_address());
            if (0==ninsns++)
                orig_state = cur_state;
            cur_insn = insn;
        }

        /* Called at the end of X86InstructionSemantics::processInstruction() */
        void finishInstruction(SgAsmInstruction*) {
            if (p_discard_popped_memory)
                cur_state.discard_popped_memory();
            cur_insn = NULL;
        }



        /*************************************************************************************************************************
         * Functions invoked by the X86InstructionSemantics class to construct values
         *************************************************************************************************************************/

        /** True value */
        ValueType<1> true_() const {
            return ValueType<1>(1);
        }

        /** False value */
        ValueType<1> false_() const {
            return ValueType<1>((uint64_t)0);
        }

        /** Undefined Boolean */
        ValueType<1> undefined_() const {
            return ValueType<1>();
        }

        /** Used to build a known constant. */
        template <size_t Len>
        ValueType<Len> number(uint64_t n) const {
            return ValueType<Len>(n);
        }



        /*************************************************************************************************************************
         * Functions invoked by the X86InstructionSemantics class for individual instructions
         *************************************************************************************************************************/

        /** Called only for CALL instructions before assigning new value to IP register. */
        ValueType<32> filterCallTarget(const ValueType<32> &a) const {
            return a;
        }

        /** Called only for RET instructions before adjusting the IP register. */
        ValueType<32> filterReturnTarget(const ValueType<32> &a) const {
            return a;
        }

        /** Called only for JMP instructions before adjusting the IP register. */
        ValueType<32> filterIndirectJumpTarget(const ValueType<32> &a) const {
            return a;
        }

        /** Called only for the HLT instruction. */
        void hlt() {} // FIXME

        /** Called only for the RDTSC instruction. */
        ValueType<64> rdtsc() {
            return ValueType<64>((uint64_t)0);
        }

        /** Called only for the INT instruction. */
        void interrupt(uint8_t num) {
            cur_state = State(); /*reset entire machine state*/
        }



        /*************************************************************************************************************************
         * Functions invoked by the X86InstructionSemantics class for data access operations
         *************************************************************************************************************************/

        /** Returns value of the specified 32-bit general purpose register. */
        ValueType<32> readGPR(X86GeneralPurposeRegister r) const {
            return cur_state.gpr[r];
        }

        /** Places a value in the specified 32-bit general purpose register. */
        void writeGPR(X86GeneralPurposeRegister r, const ValueType<32> &value) {
            cur_state.gpr[r] = value;
        }

        /** Reads a value from the specified 16-bit segment register. */
        ValueType<16> readSegreg(X86SegmentRegister sr) const {
            return cur_state.segreg[sr];
        }

        /** Places a value in the specified 16-bit segment register. */
        void writeSegreg(X86SegmentRegister sr, const ValueType<16> &value) {
            cur_state.segreg[sr] = value;
        }

        /** Returns the value of the instruction pointer as it would be during the execution of the instruction. In other words,
         *  it points to the first address past the end of the current instruction. */
        ValueType<32> readIP() const {
            return cur_state.ip;
        }

        /** Changes the value of the instruction pointer. */
        void writeIP(const ValueType<32> &value) {
            cur_state.ip = value;
        }

        /** Returns the value of a specific control/status/system flag. */
        ValueType<1> readFlag(X86Flag f) const {
            return cur_state.flag[f];
        }

        /** Changes the value of the specified control/status/system flag. */
        void writeFlag(X86Flag f, const ValueType<1> &value) {
            cur_state.flag[f] = value;
        }

        /** Reads a value from memory. */
        template <size_t Len> ValueType<Len>
        readMemory(X86SegmentRegister segreg, const ValueType<32> &addr, const ValueType<1> cond) const {
            return mem_read<Len>(cur_state, addr);
        }

        /** Writes a value to memory. */
        template <size_t Len> void
        writeMemory(X86SegmentRegister segreg, const ValueType<32> &addr, const ValueType<Len> &data, ValueType<1> cond) {
            mem_write<Len>(cur_state, addr, data);
        }



        /*************************************************************************************************************************
         * Functions invoked by the X86InstructionSemantics class for arithmetic operations
         *************************************************************************************************************************/

        /** Adds two values. */
        template <size_t Len>
        ValueType<Len> add(const ValueType<Len> &a, const ValueType<Len> &b) const {
            if (a.is_known()) {
                if (b.is_known()) {
                    return ValueType<Len>(LeafNode::create_integer(Len, a.known_value()+b.known_value()));
                } else if (0==a.known_value()) {
                    return b;
                }
            } else if (b.is_known() && 0==b.known_value()) {
                return a;
            }
            return ValueType<Len>(new InternalNode(Len, InsnSemanticsExpr::OP_ADD, a.expr, b.expr));
        }

        /** Add two values of equal size and a carry bit.  Carry information is returned via carry_out argument.  The carry_out
         *  value is the tick marks that are written above the first addend when doing long arithmetic like a 2nd grader would do
         *  (of course, they'd probably be adding two base-10 numbers).  For instance, when adding 00110110 and 11100100:
         *
         *  \code
         *    '''..'..         <-- carry tick marks: '=carry .=no carry
         *     00110110
         *   + 11100100
         *   ----------
         *    100011010
         *  \endcode
         *
         *  The carry_out value is 11100100.
         */
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

        /** Computes bit-wise AND of two values. */
        template <size_t Len>
        ValueType<Len> and_(const ValueType<Len> &a, const ValueType<Len> &b) const {
            return ValueType<Len>(new InternalNode(Len, InsnSemanticsExpr::OP_BV_AND, a.expr, b.expr));
        }

        /** Returns true_, false_, or undefined_ depending on whether argument is zero. */
        template <size_t Len>
        ValueType<1> equalToZero(const ValueType<Len> &a) const {
            return ValueType<1>(new InternalNode(1, InsnSemanticsExpr::OP_ZEROP, a.expr));
        }

        /** One's complement */
        template <size_t Len>
        ValueType<Len> invert(const ValueType<Len> &a) const {
            if (a.is_known())
                return ValueType<Len>(LeafNode::create_integer(Len, ~a.known_value()));
            return ValueType<Len>(new InternalNode(Len, InsnSemanticsExpr::OP_INVERT, a.expr));
        }

        /** Concatenate the values of @p a and @p b so that the result has @p b in the high-order bits and @p a in the low order
         *  bits. */
        template<size_t Len1, size_t Len2>
        ValueType<Len1+Len2> concat(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
            return ValueType<Len1+Len2>(new InternalNode(Len1+Len2, InsnSemanticsExpr::OP_CONCAT, a.expr, b.expr));
        }

        /** Returns second or third arg depending on value of first arg. "ite" means "if-then-else". */
        template <size_t Len>
        ValueType<Len> ite(const ValueType<1> &sel, const ValueType<Len> &ifTrue, const ValueType<Len> &ifFalse) const {
            if (sel.is_known()) {
                return sel.known_value() ? ifTrue : ifFalse;
            }
            if (solver) {
                /* If the selection expression cannot be true, then return ifFalse */
                TreeNode *assertion = new InternalNode(1, InsnSemanticsExpr::OP_EQ,
                                                       sel.expr, LeafNode::create_integer(1, 1));
                bool can_be_true = solver->satisfiable(assertion);
                assertion->deleteDeeply(); assertion=NULL;
                if (!can_be_true) return ifFalse;

                /* If the selection expression cannot be false, then return ifTrue */
                assertion = new InternalNode(1, InsnSemanticsExpr::OP_EQ,
                                             sel.expr, LeafNode::create_integer(1, 0));
                bool can_be_false = solver->satisfiable(assertion);
                assertion->deleteDeeply(); assertion=NULL;
                if (!can_be_false) return ifTrue;
            }
            return ValueType<Len>(new InternalNode(Len, InsnSemanticsExpr::OP_ITE, sel.expr, ifTrue.expr, ifFalse.expr));
        }

        /** Returns position of least significant set bit; zero when no bits are set. */
        template <size_t Len>
        ValueType<Len> leastSignificantSetBit(const ValueType<Len> &a) const {
            return ValueType<Len>(new InternalNode(Len, InsnSemanticsExpr::OP_LSSB, a.expr));
        }

        /** Returns position of most significant set bit; zero when no bits are set. */
        template <size_t Len>
        ValueType<Len> mostSignificantSetBit(const ValueType<Len> &a) const {
            return ValueType<Len>(new InternalNode(Len, InsnSemanticsExpr::OP_MSSB, a.expr));
        }

        /** Two's complement. */
        template <size_t Len>
        ValueType<Len> negate(const ValueType<Len> &a) const {
            return ValueType<Len>(new InternalNode(Len, InsnSemanticsExpr::OP_NEGATE, a.expr));
        }

        /** Computes bit-wise OR of two values. */
        template <size_t Len>
        ValueType<Len> or_(const ValueType<Len> &a, const ValueType<Len> &b) const {
            return ValueType<Len>(new InternalNode(Len, InsnSemanticsExpr::OP_BV_OR, a.expr, b.expr));
        }

        /** Rotate bits to the left. */
        template <size_t Len, size_t SALen>
        ValueType<Len> rotateLeft(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
            return ValueType<Len>(new InternalNode(Len, InsnSemanticsExpr::OP_ROL, sa.expr, a.expr));
        }

        /** Rotate bits to the right. */
        template <size_t Len, size_t SALen>
        ValueType<Len> rotateRight(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
            return ValueType<Len>(new InternalNode(Len, InsnSemanticsExpr::OP_ROR, sa.expr, a.expr));
        }

        /** Returns arg shifted left. */
        template <size_t Len, size_t SALen>
        ValueType<Len> shiftLeft(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
            return ValueType<Len>(new InternalNode(Len, InsnSemanticsExpr::OP_SHL0, sa.expr, a.expr));
        }

        /** Returns arg shifted right logically (no sign bit). */
        template <size_t Len, size_t SALen>
        ValueType<Len> shiftRight(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
            return ValueType<Len>(new InternalNode(Len, InsnSemanticsExpr::OP_SHR0, sa.expr, a.expr));
        }

        /** Returns arg shifted right arithmetically (with sign bit). */
        template <size_t Len, size_t SALen>
        ValueType<Len> shiftRightArithmetic(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
            return ValueType<Len>(new InternalNode(Len, InsnSemanticsExpr::OP_ASR, sa.expr, a.expr));
        }

        /** Sign extends a value. */
        template <size_t From, size_t To>
        ValueType<To> signExtend(const ValueType<From> &a) {
            return signedExtend<From, To>(a);
        }

        /** Divides two signed values. */
        template <size_t Len1, size_t Len2>
        ValueType<Len1> signedDivide(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
            return ValueType<Len1>(new InternalNode(Len1, InsnSemanticsExpr::OP_SDIV, a.expr, b.expr));
        }

        /** Calculates modulo with signed values. */
        template <size_t Len1, size_t Len2>
        ValueType<Len2> signedModulo(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
            return ValueType<Len2>(new InternalNode(Len2, InsnSemanticsExpr::OP_SMOD, a.expr, b.expr));
        }

        /** Multiplies two signed values. */
        template <size_t Len1, size_t Len2>
        ValueType<Len1+Len2> signedMultiply(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
            return ValueType<Len1+Len2>(new InternalNode(Len1+Len2, InsnSemanticsExpr::OP_SMUL, a.expr, b.expr));
        }

        /** Divides two unsigned values. */
        template <size_t Len1, size_t Len2>
        ValueType<Len1> unsignedDivide(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
            return ValueType<Len1>(new InternalNode(Len1, InsnSemanticsExpr::OP_UDIV, a.expr, b.expr));
        }

        /** Calculates modulo with unsigned values. */
        template <size_t Len1, size_t Len2>
        ValueType<Len2> unsignedModulo(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
            return ValueType<Len2>(new InternalNode(Len2, InsnSemanticsExpr::OP_UMOD, a.expr, b.expr));
        }

        /** Multiply two unsigned values. */
        template <size_t Len1, size_t Len2>
        ValueType<Len1+Len2> unsignedMultiply(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
            return ValueType<Len1+Len2>(new InternalNode(Len1+Len2, InsnSemanticsExpr::OP_UMUL, a.expr, b.expr));
        }

        /** Computes bit-wise XOR of two values. */
        template <size_t Len>
        ValueType<Len> xor_(const ValueType<Len> &a, const ValueType<Len> &b) const {
            return ValueType<Len>(new InternalNode(Len, InsnSemanticsExpr::OP_BV_XOR, a.expr, b.expr));
        }
    };

}; /*namespace*/


#endif
