#ifndef Rose_VirtualMachineSemantics_H
#define Rose_VirtualMachineSemantics_H
#include <stdint.h>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#include "x86InstructionSemantics.h"

#include <map>
#include <vector>


/** A policy for x86InstructionSemantics.
 *
 *  This policy can be used to emulate the execution of a single basic block of instructions.  It is similar in nature to the
 *  FindConstantsPolicy except much simpler, much faster, and much more memory-lean.  The main classes are:
 * 
 *  <ul>
 *    <li>Policy: the policy class used to instantiate X86InstructionSemantic instances.</li>
 *    <li>State: represents the state of the virtual machine, its registers and memory.</li>
 *    <li>ValueType: the values stored in registers and memory and used for memory addresses.</li>
 *  </ul>
 *
 *  Each value is either a known value or an unknown value. An unknown value consists of a base name and offset and whether
 *  the value is negated. */
namespace VirtualMachineSemantics {

extern uint64_t name_counter;

typedef std::map<uint64_t/*oldname*/, uint64_t/*newname*/> RenameMap;

/** A value is either known or unknown. Unknown values have a base name (unique ID number), offset, and sign. */
template<size_t nBits>
struct ValueType {
    uint64_t name;                      /**< Zero for constants; non-zero ID number for everything else. */
    uint64_t offset;                    /**< The constant (if name==0) or an offset w.r.t. an unknown (named) base value. */
    bool negate;                        /**< Switch between name+offset and (-name)+offset; should be false for constants. */

    /** Construct a value that is unknown and unique. */
    ValueType(): name(++name_counter), offset(0), negate(false) {}

    /** Copy-construct a value, truncating or extending at msb the source value. */
    template <size_t Len>
    ValueType(const ValueType<Len> &other) {
        name = other.name;
        offset = other.offset & IntegerOps::GenMask<uint64_t, nBits>::value; /*make sure high-order bits are zeroed out*/
        negate = other.negate;
    }

    /** Construct a ValueType with a known value. */
    ValueType(uint64_t n)   /*implicit*/
        : name(0), offset(n), negate(false) {
        this->offset &= IntegerOps::GenMask<uint64_t, nBits>::value;
    }

    /** Low-level constructor used internally. */
    ValueType(uint64_t name, uint64_t offset, bool negate=false)
        : name(name), offset(offset), negate(negate) {
        this->offset &= IntegerOps::GenMask<uint64_t, nBits>::value;
    }

    /** Returns true if the value is known, false if the value only has a name. */
    bool is_known() const {
        return 0==name;
    }

    /** Returns the value if it is known. */
    uint64_t known_value() const {
        ROSE_ASSERT(is_known());
        return offset;
    }

    /** Print the value */
    void print(std::ostream &o) const;

    void rename(RenameMap&);

    friend bool operator==(const ValueType &a, const ValueType &b) {
        return a.name==b.name && (!a.name || a.negate==b.negate) && a.offset==b.offset;
    }

    friend bool operator!=(const ValueType &a, const ValueType &b) {
        return !(a==b);
    }

    friend bool operator<(const ValueType &a, const ValueType &b) {
        if (a.name<b.name) return true;
        if (b.name<a.name) return false;
        if (a.name && a.negate<b.negate) return true;
        if (a.name && b.negate<a.negate) return false;
        return a.offset < b.offset;
    }
};

template<size_t Len>
std::ostream& operator<<(std::ostream &o, const ValueType<Len> &e);

/** Represents one location in memory. Has an address data and size. Memory granularity is 32-bits. */
struct MemoryCell {
    ValueType<32> address;
    ValueType<32> data;
    size_t nbytes;
    bool clobbered;             /* Set to invalidate possible aliases during writeMemory() */

    template <size_t Len>
    MemoryCell(const ValueType<32> &address, const ValueType<Len> data, size_t nbytes)
        : address(address), data(data), nbytes(nbytes), clobbered(false) {}

    void rename(RenameMap&);

    bool is_clobbered() const { return clobbered; }
    void clobber() { clobbered = true; }

    /** Returns true if this memory value could possibly overlap with the @p other memory value.  In other words, returns false
     *  only if this memory location cannot overlap with @p other memory location. Two addresses that are identical alias one
     *  another. */
    bool may_alias(const MemoryCell &other) const;

    /** Returns true if this memory address is the same as the @p other. Note that "same" is more strict than "overlap". */
    bool must_alias(const MemoryCell &other) const;
    
    friend bool operator==(const MemoryCell &a, const MemoryCell &b) {
        return a.address==b.address && a.data==b.data && a.nbytes==b.nbytes && a.clobbered==b.clobbered;
    }
    friend bool operator!=(const MemoryCell &a, const MemoryCell &b) {
        return !(a==b);
    }
    friend bool operator<(const MemoryCell &a, const MemoryCell &b) {
        return a.address < b.address;
    }
    friend std::ostream& operator<<(std::ostream &o, const MemoryCell &me) {
        o <<"size=" <<me.nbytes <<"; addr=" <<me.address <<"; value=" <<me.data;
        if (me.clobbered) o <<" (CLOBBERED)"; /*clobbered by a write to possibly aliased memory*/
        return o;
    }
};

typedef std::vector<MemoryCell> Memory;

/** Represents the entire state of the machine. However, the instruction pointer is not included in the state. */
struct State {
    static const size_t n_gprs = 8;
    static const size_t n_segregs = 6;
    static const size_t n_flags = 16;

    ValueType<32> gpr[n_gprs];
    ValueType<16> segreg[n_segregs];
    ValueType<1> flag[n_flags];
    Memory mem;

    /** Removes memory cells that are identical to the supplied original memory.  When a memory cell is read via readMemory(),
     *  a cell is created in the state and given the next available undefined (named) value. It is also created in the
     *  orig_mem. This function deletes any memory in this state that is identical to the orig_mem. This correctly handles
     *  cases like the following, which is a no-op but creates a new cell in memory.
     *  \code
     *    xchg ds:[0x1234], eax
     *    xchg ds:[0x1234], eax
     *  \code
     *
     *  Note that squelch() must be called on the original, non-normalized state because it compares this state with the
     *  non-normalized original memory.  For instance, to print the current value of the state sans original memory contents,
     *  one would do this:
     *  \code
     *    std::cout <<policy.get_state().squelch(policy.orig_mem).normalize();
     *    std::cout <<policy.get_state(true).normalize(); // equivalent to above
     *  \endcode
     * 
     *  The term "squelch" comes from electronics, and is a circuit that turns off the volume of a radio when the signal level
     *  falls below the "squelch threshold" so that the listener does not hear static.  In our case, the "static" are the
     *  memory cells that were created to hold explicit values but which are the same values as those that were originally
     *  implicit. */
    State squelch(const Memory &orig_mem) const;

    /** Print the state in a human-friendly way. */
    void print(std::ostream &o) const;

    /** Renames the unknown values of the state according to the supplied map, adjusting the map in the process.  This is used
     *  internally by normalization functions. */
    void rename(RenameMap&);

    /** Remap all named constants to lowest possible names.  This can be used before comparing states from two different
     *  policies. */
    State normalize() const;

    /** Tests registers of two states for equality. */
    bool equal_registers(const State&) const;
    
    /** Tests memory of two states for equality. Does not compare memory cells marked as clobbered. */
    bool equal_memory_without_clobbered(const State&) const;
    
    /** Tests memory of two states for equality. The clobbered memory cells are also compared. */
    bool equal_memory_with_clobbered(const State&) const;

    /** Tests equality of two states without looking at clobbered memory locations. */
    bool equal_without_clobbered(const State&) const;

    /** Tests equality of two states including the clobbered memory locations. */
    bool equal_with_clobbered(const State&) const;

    /** Discard stack memory below stack pointer */
    void discard_popped_memory();

#if 0 /*requires libgcrypt*/
    /** Returns the SHA1 hash of a state. */
    std::string SHA1() const;
#endif

    friend std::ostream& operator<<(std::ostream&, const State&);
    friend bool operator==(const State &s1, const State &s2) { return s1.equal_with_clobbered(s2); }
    friend bool operator!=(const State &s1, const State &s2) { return !(s1==s2); }
};

/** A policy that is supplied to the semantic analysis constructor. */
class Policy {
private:
    SgAsmInstruction *cur_insn;         /**< Set by startInstruction(), cleared by finishInstruction() */
    ValueType<32> ip;                   /**< Initially cur_insn->get_address(), then updated during processInstruction() */
    State state;                        /**< Complete machine state updated by each processInstruction() */
    Memory orig_mem;                    /**< Within the state, every register has an initial explicit unknown (named) value.
                                         *   Giving every possible memory location an explicit unknown value is prohibitively
                                         *   expensive, so memory is given implicit unknown values. When we access a memory
                                         *   value from the state via readMemory(), and that memory location has never been
                                         *   read before, then we give it an explicit unknown value in the state and here in
                                         *   orig_mem.  This allows sequences like:
                                         *   \code
                                         *       mov eax, ss:[esp+4]
                                         *       mov ebx, ss:[esp+4]
                                         *   \endcode
                                         *
                                         *   to result in both eax and ebx having the same value. We also fall back on this
                                         *   orig_mem when comparing two states for equality. */
    bool p_discard_popped_memory;       /**< Property that determines how the stack behaves.  When set, any time the stack
                                         * pointer is adjusted, memory below the stack pointer and having the same address
                                         * name as the stack pointer is removed (the memory location becomes undefined). The
                                         * default is false, that is, no special treatment for the stack. */

public:
    Policy(): cur_insn(NULL), p_discard_popped_memory(false) {}

    /** Changes how the policy treats the stack.  See the p_discard_popped_memory property data member for details. */
    void set_discard_popped_memory(bool b) {
        p_discard_popped_memory = b;
    }

    /** Returns the current setting for the property that determines how the stack behaves. See the
     *  p_set_discard_popped_memory property data member for details. */
    bool get_discard_popped_memory() const {
        return p_discard_popped_memory;
    }

    /* Accessors */
    State get_state(bool do_squelch=false) const {
        return do_squelch ? state.squelch(orig_mem) : state;
    }
    const ValueType<32>& get_ip() const { return ip; }

    /** Print the normalized state of this policy, omitting memory locations that have initial values. */
    void print(std::ostream&) const;
    friend std::ostream& operator<<(std::ostream&, const Policy&);

    /** Sign extend from @p FromLen bits to @p ToLen bits. */
    template <size_t FromLen, size_t ToLen>
    ValueType<ToLen> signExtend(const ValueType<FromLen> &a) {
        if (a.name) return ValueType<ToLen>();
        return IntegerOps::signExtend<FromLen, ToLen>(a.offset);
    }

    /** Extracts certain bits from the specified value and shifts them to the low-order positions in the result.  The bits of
     *  the result include bits from BeginAt (inclusive) through EndAt (exclusive).  The lsb is number zero. */
    template <size_t BeginAt, size_t EndAt, size_t Len>
    ValueType<EndAt-BeginAt> extract(const ValueType<Len> &a) {
        if (0==BeginAt) return ValueType<EndAt-BeginAt>(a);
        if (a.name) return ValueType<EndAt-BeginAt>();
        return (a.offset >> BeginAt) & (IntegerOps::SHL1<uint64_t, EndAt-BeginAt>::value - 1);
    }

    /** Return a newly sized value by either truncating the most significant bits or by adding more most significant bits that
     *  are set to zeros. */
    template <size_t FromLen, size_t ToLen>
    ValueType<ToLen> extendByMSB(const ValueType<FromLen> &a) {
        return ValueType<ToLen>(a);
    }
    
    /** Returns true if the specified value exists in memory and is provably at or above the stack pointer.  The stack pointer
     *  need not have a known value. */
    bool on_stack(const ValueType<32> &value);

    /** Removes from memory those values at addresses below the current stack pointer. This is automatically called after each
     *  instruction if the p_discard_popped_memory property is set. */
    void discard_popped_memory();

    /*************************************************************************************************************************
     * Functions invoked by the X86InstructionSemantics class for every processed instructions
     *************************************************************************************************************************/

    /* Called at the beginning of X86InstructionSemantics::processInstruction() */
    void startInstruction(SgAsmInstruction *insn) {
        cur_insn = insn;
        ip = ValueType<32>(insn->get_address());
    }

    /* Called at the end of X86InstructionSemantics::processInstruction() */
    void finishInstruction(SgAsmInstruction*) {
        if (p_discard_popped_memory)
            state.discard_popped_memory();
        cur_insn = NULL;
    }



    /*************************************************************************************************************************
     * Functions invoked by the X86InstructionSemantics class to construct values
     *************************************************************************************************************************/

    /** True value */
    ValueType<1> true_() {
        return 1;
    }

    /** False value */
    ValueType<1> false_() {
        return 0;
    }

    /** Undefined Boolean */
    ValueType<1> undefined_() {
        return ValueType<1>();
    }

    /** Used to build a known constant. */
    template <size_t Len>
    ValueType<Len> number(uint64_t n) {
        return n;
    }



    /*************************************************************************************************************************
     * Functions invoked by the X86InstructionSemantics class for individual instructions
     *************************************************************************************************************************/

    /** Called only for CALL instructions before assigning new value to IP register. */
    ValueType<32> filterCallTarget(const ValueType<32> &a) {
        return a;
    }

    /** Called only for RET instructions before adjusting the IP register. */
    ValueType<32> filterReturnTarget(const ValueType<32> &a) {
        return a;
    }

    /** Called only for JMP instructions before adjusting the IP register. */
    ValueType<32> filterIndirectJumpTarget(const ValueType<32> &a) {
        return a;
    }

    /** Called only for the HLT instruction. */
    void hlt() {} // FIXME

    /** Called only for the RDTSC instruction. */
    ValueType<64> rdtsc() {
        return 0;
    }

    /** Called only for the INT instruction. */
    void interrupt(uint8_t num) {
        state = State(); /*reset entire machine state*/
    }



    /*************************************************************************************************************************
     * Functions invoked by the X86InstructionSemantics class for data access operations
     *************************************************************************************************************************/

    /** Returns value of the specified 32-bit general purpose register. */
    ValueType<32> readGPR(X86GeneralPurposeRegister r) {
        return state.gpr[r];
    }

    /** Places a value in the specified 32-bit general purpose register. */
    void writeGPR(X86GeneralPurposeRegister r, const ValueType<32> &value) {
        state.gpr[r] = value;
    }

    /** Reads a value from the specified 16-bit segment register. */
    ValueType<16> readSegreg(X86SegmentRegister sr) {
        return state.segreg[sr];
    }

    /** Places a value in the specified 16-bit segment register. */
    void writeSegreg(X86SegmentRegister sr, const ValueType<16> &value) {
        state.segreg[sr] = value;
    }

    /** Returns the value of the instruction pointer as it would be during the execution of the instruction. In other words,
     *  it points to the first address past the end of the current instruction. */
    ValueType<32> readIP() {
        return ip;
    }

    /** Changes the value of the instruction pointer. */
    void writeIP(const ValueType<32> &value) {
        ip = value;
    }

    /** Returns the value of a specific control/status/system flag. */
    ValueType<1> readFlag(X86Flag f) {
        return state.flag[f];
    }

    /** Changes the value of the specified control/status/system flag. */
    void writeFlag(X86Flag f, const ValueType<1> &value) {
        state.flag[f] = value;
    }

    /** Reads a value from memory. */
    template <size_t Len> ValueType<Len>
    readMemory(X86SegmentRegister segreg, const ValueType<32> &addr, const ValueType<1> cond) {
        MemoryCell new_cell(addr, ValueType<32>(), Len/8);

        /* Read memory from current memory state if possible. */
        for (Memory::iterator mi=state.mem.begin(); mi!=state.mem.end(); ++mi) {
            if (new_cell.must_alias(*mi)) {
                /* If memory cell is present but invalid then we must have seen a previous writeMemory() to a possible
                 * alias. We should mark the memory as no longer clobbered and return a new undefined value.  Otherwise return
                 * the existing value. */
                if ((*mi).is_clobbered()) *mi = new_cell;
                return (*mi).data;
            }
        }

        /* If original memory has a value then use that, saving it in our state also. */
        for (Memory::iterator mi=orig_mem.begin(); mi!=orig_mem.end(); ++mi) {
            if (new_cell.must_alias(*mi)) {
                ROSE_ASSERT(!(*mi).is_clobbered()); /*original memory is always valid*/
                state.mem.push_back(*mi);
                return (*mi).data;
            }
        }
        
        /* Create a new value and add it to both the original memory and the current state. */
        state.mem.push_back(new_cell);
        orig_mem.push_back(new_cell);
        return new_cell.data;
    }

    /** Writes a value to memory. */
    template <size_t Len> void
    writeMemory(X86SegmentRegister segreg, const ValueType<32> &addr, const ValueType<Len> &data, ValueType<1> cond) {
        MemoryCell new_cell(addr, data, Len/8);
        bool saved = false; /* has new_cell been saved into memory? */
        for (Memory::iterator mi=state.mem.begin(); mi!=state.mem.end(); ++mi) {
            if (new_cell.must_alias(*mi)) {
                *mi = new_cell;
                saved = true;
            } else if (p_discard_popped_memory &&
                       state.gpr[x86_gpr_sp]!=state.gpr[x86_gpr_bp] &&
                       state.gpr[x86_gpr_sp].name!=state.gpr[x86_gpr_bp].name &&
                       ((addr.name==state.gpr[x86_gpr_sp].name && (*mi).address.name==state.gpr[x86_gpr_bp].name) ||
                        (addr.name==state.gpr[x86_gpr_bp].name && (*mi).address.name==state.gpr[x86_gpr_sp].name))) {
                /* assume that mem referenced via stack pointer does not alias mem referenced with frame pointer. This isn't
                 * safe generally, but is usually the case for well-behaved programs. */
            } else if (new_cell.may_alias(*mi)) {
                (*mi).clobber();
            } else {
                /* memory cell *mi is not aliased to cell being written */
            }
        }
        if (!saved) {
            state.mem.push_back(new_cell);
            std::sort(state.mem.begin(), state.mem.end());
        }
    }



    /*************************************************************************************************************************
     * Functions invoked by the X86InstructionSemantics class for arithmetic operations
     *************************************************************************************************************************/

    /** Adds two values. */
    template <size_t Len>
    ValueType<Len> add(const ValueType<Len> &a, const ValueType<Len> &b) {
        if (a.name==b.name && (!a.name || a.negate!=b.negate)) {
            /* [V1+x] + [-V1+y] = [x+y]  or
             * [x] + [y] = [x+y] */
            return a.offset + b.offset;
        } else if (!a.name || !b.name) {
            /* [V1+x] + [y] = [V1+x+y]   or
             * [x] + [V2+y] = [V2+x+y]   or
             * [-V1+x] + [y] = [-V1+x+y] or
             * [x] + [-V2+y] = [-V2+x+y] */
            return ValueType<Len>(a.name+b.name, a.offset+b.offset, a.negate || b.negate);
        } else {
            return ValueType<Len>();
        }
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
                                  ValueType<Len> &carry_out) {
        int n_unknown = (a.name?1:0) + (b.name?1:0) + (c.name?1:0);
        if (n_unknown <= 1) {
            /* At most, one of the operands is an unknown value. See add() for more details. */
            uint64_t sum = a.name + b.name + c.name;
            carry_out = 0==n_unknown ? ValueType<Len>((a.offset ^ b.offset ^ sum)>>1) : ValueType<Len>();
            return ValueType<Len>(sum, a.offset+b.offset+c.offset, a.negate||b.negate||c.negate);
        } else if (a.name==b.name && !c.name && a.negate!=b.negate) {
            /* A and B are known or have bases that cancel out, and C is known */
            uint64_t sum = a.offset + b.offset + c.offset;
            carry_out = ValueType<Len>((a.offset ^ b.offset ^ sum)>>1);
            return ValueType<Len>(sum);
        } else {
            carry_out = ValueType<Len>();
            return ValueType<Len>();
        }
    }

    /** Computes bit-wise AND of two values. */
    template <size_t Len>
    ValueType<Len> and_(const ValueType<Len> &a, const ValueType<Len> &b) {
        if ((!a.name && 0==a.offset) || (!b.name && 0==b.offset)) return 0;
        if (a.name || b.name) return ValueType<Len>();
        return a.offset & b.offset;
    }

    /** Returns true_, false_, or undefined_ depending on whether argument is zero. */
    template <size_t Len>
    ValueType<1> equalToZero(const ValueType<Len> &a) {
        if (a.name) return undefined_();
        return a.offset ? false_() : true_();
    }

    /** One's complement */
    template <size_t Len>
    ValueType<Len> invert(const ValueType<Len> &a) {
        if (a.name) return ValueType<Len>(a.name, ~a.offset, !a.negate);
        return ~a.offset;
    }

    /** Concatenate the values of @p a and @p b so that the result has @p b in the high-order bits and @p a in the low order
     *  bits. */
    template<size_t Len1, size_t Len2>
    ValueType<Len1+Len2> concat(const ValueType<Len1> &a, const ValueType<Len2> &b) {
        if (a.name || b.name) return ValueType<Len1+Len2>();
        return a.offset | (b.offset << Len1);
    }

    /** Returns second or third arg depending on value of first arg. */
    template <size_t Len>
    ValueType<Len> ite(const ValueType<1> &sel, const ValueType<Len> &ifTrue, const ValueType<Len> &ifFalse) {
        if (ifTrue==ifFalse) return ifTrue;
        if (sel.name) return ValueType<Len>();
        return sel.offset ? ifTrue : ifFalse;
    }

    /** Returns position of least significant set bit; zero when no bits are set. */
    template <size_t Len>
    ValueType<Len> leastSignificantSetBit(const ValueType<Len> &a) {
        if (a.name) return ValueType<Len>();
        for (size_t i=0; i<Len; ++i) {
            if (a.offset & ((uint64_t)1 << i))
                return i;
        }
        return 0;
    }

    /** Returns position of most significant set bit; zero when no bits are set. */
    template <size_t Len>
    ValueType<Len> mostSignificantSetBit(const ValueType<Len> &a) {
        if (a.name) return ValueType<Len>();
        for (size_t i=Len; i>0; --i) {
            if (a.offset & ((uint64_t)1 << (i-1)))
                return i-1;
        }
        return 0;
    }

    /** Two's complement. */
    template <size_t Len>
    ValueType<Len> negate(const ValueType<Len> &a) {
        if (a.name) return ValueType<Len>(a.name, -a.offset, !a.negate);
        return -a.offset;
    }

    /** Computes bit-wise OR of two values. */
    template <size_t Len>
    ValueType<Len> or_(const ValueType<Len> &a, const ValueType<Len> &b) {
        if (a.name || b.name) return ValueType<Len>();
        return a.offset | b.offset;
    }

    /** Rotate bits to the left. */
    template <size_t Len, size_t SALen>
    ValueType<Len> rotateLeft(const ValueType<Len> &a, const ValueType<SALen> &sa) {
        if (a.name || sa.name) return ValueType<Len>();
        return IntegerOps::rotateLeft<Len>(a.offset, sa.offset);
    }

    /** Rotate bits to the right. */
    template <size_t Len, size_t SALen>
    ValueType<Len> rotateRight(const ValueType<Len> &a, const ValueType<SALen> &sa) {
        if (a.name || sa.name) return ValueType<Len>();
        return IntegerOps::rotateRight<Len>(a.offset, sa.offset);
    }

    /** Returns arg shifted left. */
    template <size_t Len, size_t SALen>
    ValueType<Len> shiftLeft(const ValueType<Len> &a, const ValueType<SALen> &sa) {
        if (a.name || sa.name) return ValueType<Len>();
        return IntegerOps::shiftLeft<Len>(a.offset, sa.offset);
    }

    /** Returns arg shifted right logically (no sign bit). */
    template <size_t Len, size_t SALen>
    ValueType<Len> shiftRight(const ValueType<Len> &a, const ValueType<SALen> &sa) {
        if (a.name || sa.name) return ValueType<Len>();
        return IntegerOps::shiftRightLogical<Len>(a.offset, sa.offset);
    }

    /** Returns arg shifted right arithmetically (with sign bit). */
    template <size_t Len, size_t SALen>
    ValueType<Len> shiftRightArithmetic(const ValueType<Len> &a, const ValueType<SALen> &sa) {
        if (a.name || sa.name) return ValueType<Len>();
        return IntegerOps::shiftRightArithmetic<Len>(a.offset, sa.offset);
    }

    /** Divides two signed values. */
    template <size_t Len1, size_t Len2>
    ValueType<Len1> signedDivide(const ValueType<Len1> &a, const ValueType<Len2> &b) {
        if (a.name || b.name) return ValueType<Len1>();
        if (0==b.offset) throw std::string("division by zero");
        return IntegerOps::signExtend<Len1, 64>(a.offset) / IntegerOps::signExtend<Len2, 64>(b.offset);
    }

    /** Calculates modulo with signed values. */
    template <size_t Len1, size_t Len2>
    ValueType<Len2> signedModulo(const ValueType<Len1> &a, const ValueType<Len2> &b) {
        if (a.name || b.name) return ValueType<Len2>();
        if (0==b.offset) throw std::string("division by zero");
        return IntegerOps::signExtend<Len1, 64>(a.offset) % IntegerOps::signExtend<Len2, 64>(b.offset);
    }

    /** Multiplies two signed values. */
    template <size_t Len1, size_t Len2>
    ValueType<Len1+Len2> signedMultiply(const ValueType<Len1> &a, const ValueType<Len2> &b) {
        if (a.name || b.name) return ValueType<Len1+Len2>();
        return IntegerOps::signExtend<Len1, 64>(a.offset) * IntegerOps::signExtend<Len2, 64>(b.offset);
    }

    /** Divides two unsigned values. */
    template <size_t Len1, size_t Len2>
    ValueType<Len1> unsignedDivide(const ValueType<Len1> &a, const ValueType<Len2> &b) {
        if (a.name || b.name) return ValueType<Len1>();
        if (0==b.offset) throw std::string("division by zero");
        return a.offset / b.offset;
    }

    /** Calculates modulo with unsigned values. */
    template <size_t Len1, size_t Len2>
    ValueType<Len2> unsignedModulo(const ValueType<Len1> &a, const ValueType<Len2> &b) {
        if (a.name || b.name) return ValueType<Len2>();
        if (0==b.offset) throw std::string("division by zero");
        return a.offset % b.offset;
    }

    /** Multiply two unsigned values. */
    template <size_t Len1, size_t Len2>
    ValueType<Len1+Len2> unsignedMultiply(const ValueType<Len1> &a, const ValueType<Len2> &b) {
        if (a.name || b.name) return ValueType<Len1+Len2>();
        return a.offset * b.offset;
    }

    /** Computes bit-wise XOR of two values. */
    template <size_t Len>
    ValueType<Len> xor_(const ValueType<Len> &a, const ValueType<Len> &b) {
        if (a==b) return 0;
        if (a.name || b.name) return ValueType<Len>();
        return a.offset ^ b.offset;
    }
};
    
}; /*namespace*/


#endif
