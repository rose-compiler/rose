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


/* A policy for x86InstructionSemantics.
 *
 * This policy can be used to emulate the execution of a single basic block of instructions.  It is similar in nature to the
 * FindConstantsPolicy except much simpler, much faster, and much more memory-lean. */
namespace VirtualMachineSemantics {

extern uint64_t name_counter;

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

/** Represents one location in memory. Has an address data and size. Memory granularity is 32-bits. */
struct MemoryCell {
    ValueType<32> address;
    ValueType<32> data;
    size_t nbytes;

    template <size_t Len>
    MemoryCell(const ValueType<32> &address, const ValueType<Len> data, size_t nbytes)
        : address(address), data(data), nbytes(nbytes) {}

    /** Returns true if this memory value could possibly overlap with the @p other memory value.  In other words, returns fals
     *  only if this memory location cannot overlap with @p other memory location. */
    bool may_alias(const MemoryCell &other) const {
        const ValueType<32> &addr1 = this->address;
        const ValueType<32> &addr2 = other.address;
        if (addr1.name != addr2.name) return true;

        /* Same unknown values but inverses (any offset). */
        if (addr1.name && addr1.negate!=addr2.negate) return true;

        /* If they have the same base values (or are both constant) then check the offsets. The 32-bit casts are purportedly
         * necessary to wrap propertly, but I'm not sure this will work for addresses (LatticeElements) that have a length other
         * than 32 bits. [FIXME RPM 2009-02-03]. */
        uint32_t offsetDiff = (uint32_t)(addr1.offset - addr2.offset);
        if (offsetDiff < this->nbytes || offsetDiff > (uint32_t)(-other.nbytes))
            return true;
        return false;
    }

    /** Returns true if this memory address is the same as the @p other. Note that "same" is more strict than "overlap". */
    bool must_alias(const MemoryCell &other) const {
        if (!may_alias(other)) return false;
        return this->address == other.address;
    }
    
    friend bool operator==(const MemoryCell &a, const MemoryCell &b) {
        return a.address==b.address && a.data==b.data && a.nbytes==b.nbytes;
    }
    friend bool operator<(const MemoryCell &a, const MemoryCell &b) {
        return a.address < b.address;
    }
    friend std::ostream& operator<<(std::ostream &o, const MemoryCell &me) {
        o <<"size=" <<me.nbytes <<"; addr=" <<me.address <<"; value=" <<me.data;
        return o;
    }
    template <size_t Len>
    friend std::ostream& operator<<(std::ostream &o, const ValueType<Len> &e) {
        uint64_t sign_bit = (uint64_t)1 << (Len-1);  /* e.g., 80000000 */
        uint64_t val_mask = sign_bit - 1;            /* e.g., 7fffffff */
        uint64_t negative = Len>1 && (e.offset & sign_bit) ? (~e.offset & val_mask) + 1 : 0; /*magnitude of negative value*/

        if (e.name!=0) {
            /* This is a named value rather than a constant. */
            const char *sign = e.negate ? "-" : "";
            o <<sign <<"v" <<std::dec <<e.name;
            if (negative) {
                o <<"-0x" <<std::hex <<negative;
            } else if (e.offset) {
                o <<"+0x" <<std::hex <<e.offset;
            }
        } else {
            /* This is a constant */
            ROSE_ASSERT(!e.negate);
            o  <<"0x" <<std::hex <<e.offset;
            if (negative)
                o <<" (-0x" <<std::hex <<negative <<")";
        }
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

    friend bool operator==(const State &a, const State &b) {
        for (size_t i=0; i<n_gprs; ++i)
            if (a.gpr[i]!=b.gpr[i]) return false;
        for (size_t i=0; i<n_segregs; ++i)
            if (a.segreg[i]!=b.segreg[i]) return false;
        for (size_t i=0; i<n_flags; ++i)
            if (a.flag[i]!=b.flag[i]) return false;
        if (a.mem.size()!=b.mem.size()) return false;
        for (size_t i=0; i<a.mem.size(); i++) {
            if (a.mem[i].nbytes!=b.mem[i].nbytes ||
                a.mem[i].address!=b.mem[i].address ||
                a.mem[i].data!=b.mem[i].data)
                return false;
        }
        return true;
    }

    friend bool operator!=(const State &a, const State &b) {
        return !(a==b);
    }

    friend std::ostream& operator<<(std::ostream &o, const State &state) {
        std::string prefix = "    ";
        for (size_t i=0; i<n_gprs; ++i)
            o <<prefix << gprToString((X86GeneralPurposeRegister)i) << " = " << state.gpr[i] << std::endl;
        for (size_t i=0; i<n_segregs; ++i)
            o <<prefix << segregToString((X86SegmentRegister)i) << " = " << state.segreg[i] << std::endl;
        for (size_t i=0; i<n_flags; ++i)
            o <<prefix << flagToString((X86Flag)i) << " = " << state.flag[i] << std::endl;
        o <<prefix << "memory = ";
        if (state.mem.empty()) {
            o <<"{}\n";
        } else {
            o <<"{\n";
            for (Memory::const_iterator mi=state.mem.begin(); mi!=state.mem.end(); ++mi)
                o <<prefix <<"    " <<(*mi) <<"\n";
            o <<prefix << "}\n";
        }
        return o;
    }
};

/** A policy that is supplied to the semantic analysis constructor. */
class Policy {
private:
    SgAsmInstruction *cur_insn;         /**< Set by startInstruction(), cleared by finishInstruction() */
    ValueType<32> ip;                   /**< Initially cur_insn->get_address(), then updated during processInstruction() */
    State state;                        /**< Complete machine state updated by each processInstruction() */

public:
    Policy(): cur_insn(NULL) {}

    /* Accessors */
    const State& get_state() const { return state; }
    State& get_state() { return state; }
    const ValueType<32>& get_ip() const { return ip; }
    

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
    bool on_stack(const ValueType<32> &value) {
        //std::cerr <<"VirtualMachineSemantics::on_stack(value=" <<value <<"):\n";
        const ValueType<32> sp_inverted = invert(state.gpr[x86_gpr_sp]);
        //std::cerr <<"  stack pointer = " <<state.gpr[x86_gpr_sp] <<"; inverted = " <<sp_inverted <<"\n";
        for (Memory::const_iterator mi=state.mem.begin(); mi!=state.mem.end(); ++mi) {
            //std::cerr <<"  mem entry " <<(*mi) <<":\n";
            if ((*mi).nbytes!=4 || !((*mi).data==value)) continue;
            const ValueType<32> &addr = (*mi).address;

            /* Is addr >= sp? */
            ValueType<32> carries = 0;
            ValueType<32> diff = addWithCarries(addr, sp_inverted, true_(), carries/*out*/);
            //std::cerr <<"    [" <<addr <<"] + [" <<sp_inverted <<"] = [" <<diff <<"] carry=" <<carries <<"\n";
            ValueType<1> sf = extract<31,32>(diff);
            ValueType<1> of = xor_(extract<31,32>(carries), extract<30,31>(carries));
            //std::cerr <<"    sf=" <<sf <<", of=" <<of <<"\n";
            //std::cerr <<"    on stack? "<<(sf==of ? "yes" : "no") <<"\n";
            if (sf==of) return true;
        }
        return false;
    }



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
    template <size_t Len>
    ValueType<Len> readMemory(X86SegmentRegister segreg, const ValueType<32> &addr, const ValueType<1> cond) {
        MemoryCell melmt(addr, ValueType<32>(0), Len/8);
        for (Memory::iterator mi=state.mem.begin(); mi!=state.mem.end(); ++mi) {
            if (melmt.must_alias(*mi))
                return (*mi).data;
        }
        return ValueType<Len>();
    }

    /** Writes a value to memory. */
    template <size_t Len>
    void writeMemory(X86SegmentRegister segreg, const ValueType<32> &addr, const ValueType<Len> &data, ValueType<1> cond) {
        MemoryCell melmt(addr, data, Len/8);
        Memory new_memory;
        for (Memory::iterator mi=state.mem.begin(); mi!=state.mem.end(); ++mi) {
            if (!melmt.may_alias(*mi))
                new_memory.push_back(*mi);
        }
        new_memory.push_back(melmt);
        std::sort(new_memory.begin(), new_memory.end());
        state.mem = new_memory;
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
        return IntegerOps::signExtend<Len1, 64>(a.offset) / IntegerOps::signExtend<Len2, 64>(b.offset);
    }

    /** Calculates modulo with signed values. */
    template <size_t Len1, size_t Len2>
    ValueType<Len2> signedModulo(const ValueType<Len1> &a, const ValueType<Len2> &b) {
        if (a.name || b.name) return ValueType<Len2>();
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
