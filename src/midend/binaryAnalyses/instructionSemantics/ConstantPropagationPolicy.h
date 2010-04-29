/** A policy for x86InstructionSemantics. This policy is for tracking the flow of constants through a single basic block. It
 *  is a simplified version of FindConstantsPolicy. */
#ifndef Rose_ConstantPropagationPolicy_H
#define Rose_ConstantPropagationPolicy_H
#include <stdint.h>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#include "x86InstructionSemantics.h"

#include <map>
#include <vector>



extern uint64_t CPValue_name_counter;

template<size_t nBits>
struct CPValue {
    CPValue(): name(++CPValue_name_counter), offset(0), negate(false) {}

    template <size_t Len>
    CPValue(const CPValue<Len> &other) {
        name = other.name;
        offset = other.offset & IntegerOps::GenMask<uint64_t, nBits>::value; /*make sure high-order bits are zeroed out*/
        negate = other.negate;
    }

    CPValue(uint64_t n)   /*implicit*/
        : name(0), offset(n), negate(false) {
        this->offset &= IntegerOps::GenMask<uint64_t, nBits>::value;
    }

    CPValue(uint64_t name, uint64_t offset, bool negate=false)
        : name(name), offset(offset), negate(negate) {
        this->offset &= IntegerOps::GenMask<uint64_t, nBits>::value;
    }

    friend bool operator==(const CPValue &a, const CPValue &b) {
        return a.name==b.name && (!a.name || a.negate==b.negate) && a.offset==b.offset;
    }

    friend bool operator<(const CPValue &a, const CPValue &b) {
        if (a.name<b.name) return true;
        if (b.name<a.name) return false;
        if (a.name && a.negate<b.negate) return true;
        if (a.name && b.negate<a.negate) return false;
        return a.offset < b.offset;
    }

    uint64_t name;                      /**< Zero for constants; non-zero ID number for everything else. */
    uint64_t offset;                    /**< The constant (if name==0) or an offset w.r.t. an unknown (named) base value. */
    bool negate;                        /**< Switch between name+offset and (-name)+offset; should be false for constants. */
};

struct CPMemoryValue {
    CPValue<32> address;
    CPValue<32> data;
    size_t nbytes;

    template <size_t Len>
    CPMemoryValue(const CPValue<32> &address, const CPValue<Len> data, size_t nbytes)
        : address(address), data(data), nbytes(nbytes) {}

    /** Returns true if this memory value could possibly overlap with the @p other memory value.  In other words, returns fals
     *  only if this memory location cannot overlap with @p other memory location. */
    bool may_alias(const CPMemoryValue &other) const {
        const CPValue<32> &addr1 = this->address;
        const CPValue<32> &addr2 = other.address;
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
    bool must_alias(const CPMemoryValue &other) const {
        if (!may_alias(other)) return false;
        return this->address == other.address;
    }
    
    friend bool operator==(const CPMemoryValue &a, const CPMemoryValue &b) {
        return a.address==b.address && a.data==b.data && a.nbytes==b.nbytes;
    }
    friend bool operator<(const CPMemoryValue &a, const CPMemoryValue &b) {
        return a.address < b.address;
    }

    template <size_t Len>
    friend std::ostream& operator<<(std::ostream &o, const CPValue<Len> &e) {
        uint64_t sign_bit = (uint64_t)1 << (Len-1);  /* e.g., 80000000 */
        uint64_t val_mask = sign_bit - 1;            /* e.g., 7fffffff */
        uint64_t negative = (e.offset & sign_bit) ? (~e.offset & val_mask) + 1 : 0; /*magnitude of negative value*/

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

typedef std::vector<CPMemoryValue> CPMemory;

struct MachineState {
    CPValue<32> gpr[8];
    CPValue<16> segreg[6];
    CPValue<1> flag[16];
    std::vector<CPMemoryValue> mem;

    friend std::ostream& operator<<(std::ostream &o, const MachineState &state) {
        std::string prefix = "    ";
        for (size_t i = 0; i < 8; ++i)
            o <<prefix << gprToString((X86GeneralPurposeRegister)i) << " = " << state.gpr[i] << std::endl;
        for (size_t i = 0; i < 6; ++i)
            o <<prefix << segregToString((X86SegmentRegister)i) << " = " << state.segreg[i] << std::endl;
        for (size_t i = 0; i < 16; ++i)
            o <<prefix << flagToString((X86Flag)i) << " = " << state.flag[i] << std::endl;
        o <<prefix << "memory = ";
        if (state.mem.empty()) {
            o <<"{}\n";
        } else {
            o <<"{\n";
            for (size_t i = 0; i < state.mem.size(); ++i) {
                o <<prefix <<"    "
                  <<"size=" <<state.mem[i].nbytes
                  << "; addr=" <<state.mem[i].address
                  << "; value=" <<state.mem[i].data
                  <<"\n";
            }
            o <<prefix << "}\n";
        }
        return o;
    }
};

class ConstantPropagationPolicy {
public:
    SgAsmInstruction *cur_insn;
    CPValue<32> new_ip;
    MachineState state;

    ConstantPropagationPolicy(): cur_insn(NULL) {}

    /* Called at the beginning of X86InstructionSemantics::processInstruction() */
    void startInstruction(SgAsmInstruction *insn) {
        cur_insn = insn;
        new_ip = CPValue<32>(insn->get_address());
    }

    /* Called at the end of X86InstructionSemantics::processInstruction() */
    void finishInstruction(SgAsmInstruction*) {
        cur_insn = NULL;
    }

    /** Returns value of the specified 32-bit general purpose register. */
    CPValue<32> readGPR(X86GeneralPurposeRegister r) {
        return state.gpr[r];
    }
    
    /** Places a value in the specified 32-bit general purpose register. */
    void writeGPR(X86GeneralPurposeRegister r, const CPValue<32> &value) {
        state.gpr[r] = value;
    }
    
    /** Reads a value from the specified 16-bit segment register. */
    CPValue<16> readSegreg(X86SegmentRegister sr) {
        return state.segreg[sr];
    }
    
    /** Places a value in the specified 16-bit segment register. */
    void writeSegreg(X86SegmentRegister sr, const CPValue<16> &value) {
        state.segreg[sr] = value;
    }
    
    /** Returns the value of the instruction pointer as it would be during the execution of the instruction. In other words,
     *  it points to the first address past the end of the current instruction. */
    CPValue<32> readIP() {
        return new_ip;
    }
    
    /** Changes the value of the instruction pointer. */
    void writeIP(const CPValue<32> &value) {
        new_ip = value;
    }
    
    /** Returns the value of a specific control/status/system flag. */
    CPValue<1> readFlag(X86Flag f) {
        return state.flag[f];
    }
    
    /** Changes the value of the specified control/status/system flag. */
    void writeFlag(X86Flag f, const CPValue<1> &value) {
        state.flag[f] = value;
    }

    /** Reads a value from memory. */
    template <size_t Len>
    CPValue<Len> readMemory(X86SegmentRegister segreg, const CPValue<32> &addr, const CPValue<1> cond) {
        CPMemoryValue melmt(addr, CPValue<32>(0), Len/8);
        for (CPMemory::iterator mi=state.mem.begin(); mi!=state.mem.end(); ++mi) {
            if (melmt.must_alias(*mi))
                return (*mi).data;
        }
        return CPValue<Len>();
    }
    
    /** Writes a value to memory. */
    template <size_t Len>
    void writeMemory(X86SegmentRegister segreg, const CPValue<32> &addr, const CPValue<Len> &data, CPValue<1> cond) {
        CPMemoryValue melmt(addr, data, Len/8);
        CPMemory new_memory;
        for (CPMemory::iterator mi=state.mem.begin(); mi!=state.mem.end(); ++mi) {
            if (!melmt.may_alias(*mi))
                new_memory.push_back(*mi);
        }
        new_memory.push_back(melmt);
        std::sort(new_memory.begin(), new_memory.end());
        state.mem = new_memory;
    }
    
    /** True value */
    CPValue<1> true_() {
        return 1;
    }

    /** False value */
    CPValue<1> false_() {
        return 0;
    }
    
    /** Undfined Boolean */
    CPValue<1> undefined_() {
        return CPValue<1>();
    }

    /** Called only for CALL instructions before assigning new value to IP register. */
    CPValue<32> filterCallTarget(const CPValue<32> &a) {
        return a;
    }

    /** Called only for RET instructions before adjusting the IP register. */
    CPValue<32> filterReturnTarget(const CPValue<32> &a) {
        return a;
    }

    /** Called only for JMP instructions before adjusting the IP register. */
    CPValue<32> filterIndirectJumpTarget(const CPValue<32> &a) {
        return a;
    }

    /** Called only for the HLT instruction. */
    void hlt() {} // FIXME

    /** Called only for the RDTSC instruction. */
    CPValue<64> rdtsc() {
        return 0;
    }

    /** Called only for the INT instruction. */
    void interrupt(uint8_t num) {
        state = MachineState(); /*reset entire machine state*/
    }

    /** Used to build a known constant. */
    template <size_t Len>
    CPValue<Len> number(uint64_t n) {
        return n;
    }

    /** Sign extend from @p FromLen bits to @p ToLen bits. */
    template <size_t FromLen, size_t ToLen>
    CPValue<ToLen> signExtend(const CPValue<FromLen> &a) {
        if (a.name) return CPValue<ToLen>();
        return IntegerOps::signExtend<FromLen, ToLen>(a.offset);
    }

    /** Extracts certain bits from the specified value and shifts them to the low-order positions in the result.  The bits of
     *  the result include bits from BeginAt (inclusive) through EndAt (exclusive).  The lsb is number zero. */
    template <size_t BeginAt, size_t EndAt, size_t Len>
    CPValue<EndAt-BeginAt> extract(const CPValue<Len> &a) {
        if (0==BeginAt) return CPValue<EndAt-BeginAt>(a);
        if (a.name) return CPValue<EndAt-BeginAt>();
        return (a.offset >> BeginAt) & (IntegerOps::SHL1<uint64_t, EndAt-BeginAt>::value - 1);
    }

    /** Add two values of equal size and a carry flag. */
    template <size_t Len>
    CPValue<Len> add3(const CPValue<Len> &a, const CPValue<Len> &b, const CPValue<1> c) {
        if ((a.name?1:0) + (b.name?1:0) + (c.name?1:0) == 1) {
            /* One of the operands is an unknown value while the other two are known values. See add() for more details. */
            return CPValue<Len>(a.name+b.name+c.name, a.offset+b.offset+c.offset, a.negate||b.negate||c.negate);
        } else if (a.name==b.name && !c.name && a.negate!=b.negate) {
            /* A and B are known or have bases that cancel out, and C is known */
            return a.offset + b.offset + c.offset;
        } else {
            return CPValue<Len>();
        }
    }

    /** Bitwise XOR of three values. */
    template <size_t Len>
    CPValue<Len> xor3(const CPValue<Len> &a, const CPValue<Len> &b, const CPValue<Len> &c) {
        if (a==b) return c;
        if (b==c) return a;
        if (a==c) return b;
        if (a.name || b.name || c.name) return CPValue<Len>();
        return a.offset ^ b.offset ^ c.offset;
    }

    /** Return a newly sized value by either truncating the most significant bits or by adding more most significant bits that
     *  are set to zeros. */
    template <size_t FromLen, size_t ToLen>
    CPValue<ToLen> extendByMSB(const CPValue<FromLen> &a) {
        return CPValue<ToLen>(a);
    }
    
    /*************************************************************************************************************************
     * OPERATORS (alphabetically)
     *************************************************************************************************************************/

    /** Adds two values. */
    template <size_t Len>
    CPValue<Len> add(const CPValue<Len> &a, const CPValue<Len> &b) {
        if (a.name==b.name && (!a.name || a.negate!=b.negate)) {
            /* [V1+x] + [-V1+y] = [x+y]  or
             * [x] + [y] = [x+y] */
            return a.offset + b.offset;
        } else if (!a.name || !b.name) {
            /* [V1+x] + [y] = [V1+x+y]   or
             * [x] + [V2+y] = [V2+x+y]   or
             * [-V1+x] + [y] = [-V1+x+y] or
             * [x] + [-V2+y] = [-V2+x+y] */
            return CPValue<Len>(a.name+b.name, a.offset+b.offset, a.negate || b.negate);
        } else {
            return CPValue<Len>();
        }
    }

    /** Add two numbers and a carry bit, returning carry info for each bit position (shifted by one). In other words, the
     *  carry results are like the 1s (or tick marks) you would place above the first addend if you were a 2nd grader learning
     *  long addition, except shifted to the right by one slot. */
    template <size_t Len>
    CPValue<Len> addWithCarries(const CPValue<Len> &a, const CPValue<Len> &b, const CPValue<1> &carryIn,
                                CPValue<Len> &carries) {
        CPValue<Len+1> aa = extendByMSB<Len, Len+1>(a);
        CPValue<Len+1> bb = extendByMSB<Len, Len+1>(b);
        CPValue<Len+1> ans = add3(aa, bb, carryIn);
        carries = extract<1, Len+1>(xor3(aa, bb, ans));
        return extract<0, Len>(ans);
    }
    
    /** Computes bit-wise AND of two values. */
    template <size_t Len>
    CPValue<Len> and_(const CPValue<Len> &a, const CPValue<Len> &b) {
        if ((!a.name && 0==a.offset) || (!b.name && 0==b.offset)) return 0;
        if (a.name || b.name) return CPValue<Len>();
        return a.offset & b.offset;
    }

    /** Returns true_, false_, or undefined_ depending on whether argument is zero. */
    template <size_t Len>
    CPValue<1> equalToZero(const CPValue<Len> &a) {
        if (a.name) return undefined_();
        return a.offset ? false_() : true_();
    }

    /** One's complement */
    template <size_t Len>
    CPValue<Len> invert(const CPValue<Len> &a) {
        if (a.name) return CPValue<Len>(a.name, ~a.offset, !a.negate);
        return ~a.offset;
    }
    
    /** Concatenate the values of @p a and @p b so that the result has @p b in the high-order bits and @p a in the low order
     *  bits. */
    template<size_t Len1, size_t Len2>
    CPValue<Len1+Len2> concat(const CPValue<Len1> &a, const CPValue<Len2> &b) {
        if (a.name || b.name) return CPValue<Len1+Len2>();
        return a.offset | (b.offset << Len1);
    }

    /** Returns second or third arg depending on value of first arg. */
    template <size_t Len>
    CPValue<Len> ite(const CPValue<1> &sel, const CPValue<Len> &ifTrue, const CPValue<Len> &ifFalse) {
        if (ifTrue==ifFalse) return ifTrue;
        if (sel.name) return CPValue<Len>();
        return sel.offset ? ifTrue : ifFalse;
    }

    /** Returns position of least significant set bit; zero when no bits are set. */
    template <size_t Len>
    CPValue<Len> leastSignificantSetBit(const CPValue<Len> &a) {
        if (a.name) return CPValue<Len>();
        for (size_t i=0; i<Len; ++i) {
            if (a.offset & ((uint64_t)1 << i))
                return i;
        }
        return 0;
    }

    /** Returns position of most significant set bit; zero when no bits are set. */
    template <size_t Len>
    CPValue<Len> mostSignificantSetBit(const CPValue<Len> &a) {
        if (a.name) return CPValue<Len>();
        for (size_t i=Len; i>0; --i) {
            if (a.offset & ((uint64_t)1 << (i-1)))
                return i-1;
        }
        return 0;
    }

    /** Two's complement. */
    template <size_t Len>
    CPValue<Len> negate(const CPValue<Len> &a) {
        if (a.name) return CPValue<Len>(a.name, -a.offset, !a.negate);
        return -a.offset;
    }

    /** Computes bit-wise OR of two values. */
    template <size_t Len>
    CPValue<Len> or_(const CPValue<Len> &a, const CPValue<Len> &b) {
        if (a.name || b.name) return CPValue<Len>();
        return a.offset | b.offset;
    }

    /** Rotate bits to the left. */
    template <size_t Len, size_t SALen>
    CPValue<Len> rotateLeft(const CPValue<Len> &a, const CPValue<SALen> &sa) {
        if (a.name || sa.name) return CPValue<Len>();
        return IntegerOps::rotateLeft<Len>(a.offset, sa.offset);
    }
    
    /** Rotate bits to the right. */
    template <size_t Len, size_t SALen>
    CPValue<Len> rotateRight(const CPValue<Len> &a, const CPValue<SALen> &sa) {
        if (a.name || sa.name) return CPValue<Len>();
        return IntegerOps::rotateRight<Len>(a.offset, sa.offset);
    }
    
    /** Returns arg shifted left. */
    template <size_t Len, size_t SALen>
    CPValue<Len> shiftLeft(const CPValue<Len> &a, const CPValue<SALen> &sa) {
        if (a.name || sa.name) return CPValue<Len>();
        return IntegerOps::shiftLeft<Len>(a.offset, sa.offset);
    }
    
    /** Returns arg shifted right logically (no sign bit). */
    template <size_t Len, size_t SALen>
    CPValue<Len> shiftRight(const CPValue<Len> &a, const CPValue<SALen> &sa) {
        if (a.name || sa.name) return CPValue<Len>();
        return IntegerOps::shiftRightLogical<Len>(a.offset, sa.offset);
    }
    
    /** Returns arg shifted right arithmetically (with sign bit). */
    template <size_t Len, size_t SALen>
    CPValue<Len> shiftRightArithmetic(const CPValue<Len> &a, const CPValue<SALen> &sa) {
        if (a.name || sa.name) return CPValue<Len>();
        return IntegerOps::shiftRightArithmetic<Len>(a.offset, sa.offset);
    }

    /** Divides two signed values. */
    template <size_t Len1, size_t Len2>
    CPValue<Len1> signedDivide(const CPValue<Len1> &a, const CPValue<Len2> &b) {
        if (a.name || b.name) return CPValue<Len1>();
        return IntegerOps::signExtend<Len1, 64>(a.offset) / IntegerOps::signExtend<Len2, 64>(b.offset);
    }

    /** Calculates modulo with signed values. */
    template <size_t Len1, size_t Len2>
    CPValue<Len2> signedModulo(const CPValue<Len1> &a, const CPValue<Len2> &b) {
        if (a.name || b.name) return CPValue<Len2>();
        return IntegerOps::signExtend<Len1, 64>(a.offset) % IntegerOps::signExtend<Len2, 64>(b.offset);
    }

    /** Multiplies two signed values. */
    template <size_t Len1, size_t Len2>
    CPValue<Len1+Len2> signedMultiply(const CPValue<Len1> &a, const CPValue<Len2> &b) {
        if (a.name || b.name) return CPValue<Len1+Len2>();
        return IntegerOps::signExtend<Len1, 64>(a.offset) * IntegerOps::signExtend<Len2, 64>(b.offset);
    }

    /** Divides two unsigned values. */
    template <size_t Len1, size_t Len2>
    CPValue<Len1> unsignedDivide(const CPValue<Len1> &a, const CPValue<Len2> &b) {
        if (a.name || b.name) return CPValue<Len1>();
        if (0==b.offset) throw std::string("division by zero");
        return a.offset / b.offset;
    }

    /** Calculates modulo with unsigned values. */
    template <size_t Len1, size_t Len2>
    CPValue<Len2> unsignedModulo(const CPValue<Len1> &a, const CPValue<Len2> &b) {
        if (a.name || b.name) return CPValue<Len2>();
        return a.offset % b.offset;
    }

    /** Multiply two unsigned values. */
    template <size_t Len1, size_t Len2>
    CPValue<Len1+Len2> unsignedMultiply(const CPValue<Len1> &a, const CPValue<Len2> &b) {
        if (a.name || b.name) return CPValue<Len1+Len2>();
        return a.offset * b.offset;
    }

    /** Computes bit-wise XOR of two values. */
    template <size_t Len>
    CPValue<Len> xor_(const CPValue<Len> &a, const CPValue<Len> &b) {
        if (a==b) return 0;
        if (a.name || b.name) return CPValue<Len>();
        return a.offset ^ b.offset;
    }
    


};

    

#endif
