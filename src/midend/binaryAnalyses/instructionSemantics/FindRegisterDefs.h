#ifndef Rose_FindRegisterDefs_H
#define Rose_FindRegisterDefs_H

#include "x86InstructionSemantics.h"
#include "integerOps.h"

/** A policy for x86InstructionSemantics.
 *
 *  This policy keeps track of which registers (or parts of registers) are initialized. */

namespace FindRegisterDefs {

/******************************************************************************************************************************
 *                                      ValueType
 ******************************************************************************************************************************/

template<size_t nBits>
struct ValueType {
    uint64_t defbits;           /** Bitmask whose bits are set if the value is defined. */

    /** Default constructor has all bits clear. */
    ValueType(): defbits(0) {}

    /** Set defbits to the specified value. */
    ValueType(uint64_t n) /*implicit*/ {
        defbits = n & IntegerOps::GenMask<uint64_t, nBits>::value;
    }

    /** Copy constructor. */
    template<size_t Len>
    ValueType(const ValueType<Len> &other) {
        defbits = other.defbits & IntegerOps::GenMask<uint64_t, nBits>::value; // make sure high-order bits are zeroed out
    }

    /** Returns a value with all bits defined. */
    static ValueType<nBits> defined() { return ValueType<nBits>((uint64_t)(-1)); }

    /** Returns a value with all bits undefined. */
    static ValueType<nBits> undefined() { return ValueType<nBits>(0); }

    /** Returns true if and only if all bits of the value are defined. */
    bool is_defined() const {
        return 0 == (~defbits & IntegerOps::GenMask<uint64_t, nBits>::value);
    }

    /** Returns true if and only if any of the bits of the value are not defined. */
    bool is_undefined() const {
        return !is_defined();
    }

    /** Returns the underlying bit vector.  Set bits indicate that a value has been assigned to that bit position, although
     *  this policy doesn't actually track the value itself. */
    uint64_t known_value() const {
        return defbits;
    }

    /** Print the value. */
    void print(std::ostream &o) const {
        if (is_defined()) {                     // all bits defined
            o <<"defined";
        } else if (0==defbits) {                // all bits undefined
            o <<"undefined";
        } else {                                // some defined, some undefined (show the mask)
            int nnibbles = (nBits+3)/4;
            char buf[128];
            snprintf(buf, sizeof buf, "0x%0*"PRIx64, nnibbles, defbits);
            o <<buf;
        }
    }
};

template<size_t nBits>
std::ostream& operator<<(std::ostream &o, const ValueType<nBits> &e)
{
    e.print(o);
    return o;
}

/******************************************************************************************************************************
 *                                      State
 ******************************************************************************************************************************/

struct State {
    static const size_t n_gprs = 8;             /**< Number of general purpose registers. */
    static const size_t n_segregs = 6;          /**< Number of segment registers. */
    static const size_t n_flags = 32;           /**< Number of flag registers. */

    ValueType<32> ip;
    ValueType<32> gpr[n_gprs];
    ValueType<16> segreg[n_segregs];
    ValueType<1> flag[n_flags];

    /** Set state back to initial value. */
    void clear() {
        *this = State();
    }

    /** Merge another state into this one.  The defined bits of the two states are OR'd together. */
    void merge(const State&);

    /** Returns true if and only if the registers of two states are equal in terms of which bits are defined. */
    bool equal_registers(const State&) const;

    /** Print the state in a human-friendly way.
     * @{ */
    void print(std::ostream&) const;
    friend std::ostream& operator<<(std::ostream &o, const State &state) {
        state.print(o);
        return o;
    }
    /** @} */
};

/******************************************************************************************************************************
 *                                      Policy
 ******************************************************************************************************************************/

class Policy {
private:
    SgAsmInstruction *cur_insn;
    State cur_state;                            /** Current state. Indicates which bits of registers have defined values. */
    mutable State rdundef;                      /** Which undefined bits have been read from registers. */
    size_t ninsns;

public:
    Policy(): cur_insn(NULL), ninsns(0) {}

    /** Returns the number of instructions processed. */
    size_t get_ninsns() const {
        return ninsns;
    }

    /** Sets the number of instructions processed.  This is the same counter incremented at the beginning of each instruction
     *  and returned by get_ninsns(). */
    void set_ninsns(size_t n) {
        ninsns = n;
    }

    /** Returns the current instruction.  Returns the null pointer if no instruction is being processed. */
    SgAsmInstruction *get_insn() const {
        return cur_insn;
    }

    /** Returns the current state.
     * @{ */
    const State& get_state() const { return cur_state; }
    State& get_state() { return cur_state; }
    /** @} */

    /** Returns the read-undefined state.
     * @{ */
    const State& get_rdundef_state() const { return rdundef; }
    State& get_rdundef_state() { return rdundef; }
    /** @} */

    /** Returns the current instruction pointer. */
    const ValueType<32>& get_ip() const { return cur_state.ip; }

    /** Merge another policy into this one.  The defined bits of the two policies are OR'd together. */
    void merge(const Policy&);

    /** Returns true if and only if the states of two policies are equal. */
    bool equal_states(const Policy&) const;

    /** Print the current state of this policy.
     * @{ */
    void print(std::ostream&) const;
    friend std::ostream& operator<<(std::ostream &o, const Policy &p) {
        p.print(o);
        return o;
    }
    /** @} */

    /** Sign extend from @p FromLen bits to @p ToLen bits. */
    template<size_t FromLen, size_t ToLen>
    ValueType<ToLen> signExtend(const ValueType<FromLen> &a) const {
        return IntegerOps::signExtend<FromLen, ToLen>(a.defbits);
    }

    /** Extracts certain bits from the specified value and shifts them to the low-order positions in the result.  The bits of
     *  the result include bits from BeginAt (inclusive) through EndAt (exclusive).  The lsb is number zero. */
    template <size_t BeginAt, size_t EndAt, size_t Len>
    ValueType<EndAt-BeginAt> extract(const ValueType<Len> &a) const {
        if (0==BeginAt) return ValueType<EndAt-BeginAt>(a);
        return (a.defbits >> BeginAt) & IntegerOps::GenMask<uint64_t, EndAt-BeginAt>::value;
    }

    /** Return a newly sized value by either truncating the most significant bits or by adding more most significant bits that
     *  are defined (set). */
    template <size_t FromLen, size_t ToLen>
    ValueType<ToLen> extendByMSB(const ValueType<FromLen> &a) const {
        uint64_t newbits = IntegerOps::GenMask<uint64_t, ToLen>::value & ~IntegerOps::GenMask<uint64_t, FromLen>::value;
        return a.defbits | newbits; // all new bits are defined (i.e., they would be set to zero if we were tracking values)
    }

    /*************************************************************************************************************************
     * Functions invoked by the X86InstructionSemantics class for every processed instruction or block
     *************************************************************************************************************************/

    /* Called at the beginning of X86InstructionSemantics::processInstruction() */
    void startInstruction(SgAsmInstruction *insn) {
        cur_state.ip = ValueType<32>::defined();
        cur_insn = insn;
    }

    /* Called at the end of X86InstructionSemantics::processInstruction() */
    void finishInstruction(SgAsmInstruction*) {
        cur_insn = NULL;
    }

    /* Called at the beginning of X86InstructionSemantics::processBlock() */
    void startBlock(rose_addr_t addr) {}

    /* Called at the end of X86InstructionSemantics::processBlock() */
    void finishBlock(rose_addr_t addr) {}

    /*************************************************************************************************************************
     * Functions invoked by the X86InstructionSemantics class to construct values
     *************************************************************************************************************************/

    /** True value. The bit is defined. */
    ValueType<1> true_() const {
        return ValueType<1>::defined();
    }

    /** False value. The bit is defined. */
    ValueType<1> false_() const {
        return ValueType<1>::defined();
    }

    /** Undefined Boolean */
    ValueType<1> undefined_() const {
        return ValueType<1>::undefined();
    }

    /** Used to build a known constant. */
    template <size_t Len>
    ValueType<Len> number(uint64_t n) const {
        return ValueType<Len>::defined();
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

    /** Called only for the CPUID instruction. */
    void cpuid() {} // FIXME

    /** Called only for the RDTSC instruction. */
    ValueType<64> rdtsc() {
        return ValueType<64>::defined();
    }

    /** Called only for the INT instruction. */
    void interrupt(uint8_t num) {
        cur_state = State(); /*reset entire machine state*/
    }

    /** Called only for the SYSENTER instruction. */
    void sysenter() {
        cur_state = State(); /*reset entire machine state*/
    }


    /*************************************************************************************************************************
     * Functions invoked by the X86InstructionSemantics class for data access operations
     *************************************************************************************************************************/

    /** Returns value of the specified 32-bit general purpose register. */
    ValueType<32> readGPR(X86GeneralPurposeRegister r) const {
        assert((unsigned)r<State::n_gprs);
        rdundef.gpr[r].defbits |= ~cur_state.gpr[r].defbits & IntegerOps::GenMask<uint64_t, 32>::value;
        return cur_state.gpr[r];
    }

    /** Places a value in the specified 32-bit general purpose register. */
    void writeGPR(X86GeneralPurposeRegister r, const ValueType<32> &value) {
        assert((unsigned)r<State::n_gprs);
        cur_state.gpr[r] = value;
    }

    /** Reads a value from the specified 16-bit segment register. */
    ValueType<16> readSegreg(X86SegmentRegister sr) const {
        assert((unsigned)sr<State::n_segregs);
        rdundef.segreg[sr].defbits |= ~cur_state.segreg[sr].defbits & IntegerOps::GenMask<uint64_t, 16>::value;
        return cur_state.segreg[sr];
    }

    /** Places a value in the specified 16-bit segment register. */
    void writeSegreg(X86SegmentRegister sr, const ValueType<16> &value) {
        assert((unsigned)sr<State::n_segregs);
        cur_state.segreg[sr] = value;
    }

    /** Returns the value of the instruction pointer as it would be during the execution of the instruction. In other words,
     *  it points to the first address past the end of the current instruction. */
    ValueType<32> readIP() const {
        rdundef.ip.defbits |= ~cur_state.ip.defbits & IntegerOps::GenMask<uint64_t, 32>::value;
        return cur_state.ip;
    }

    /** Changes the value of the instruction pointer. */
    void writeIP(const ValueType<32> &value) {
        cur_state.ip = value;
    }

    /** Returns the value of a specific control/status/system flag. */
    ValueType<1> readFlag(X86Flag f) const {
        assert((unsigned)f<=State::n_flags);
        rdundef.flag[f].defbits |= ~cur_state.flag[f].defbits & 0x1;
        return cur_state.flag[f];
    }

    /** Changes the value of the specified control/status/system flag. */
    void writeFlag(X86Flag f, const ValueType<1> &value) {
        assert((unsigned)f<=State::n_flags);
        cur_state.flag[f] = value;
    }

    /** Reads a value from memory.  For simplicity, this policy assumes that all of memory is defined. */
    template <size_t Len> ValueType<Len>
    readMemory(X86SegmentRegister segreg, const ValueType<32> &addr, const ValueType<1> cond) const {
        return ValueType<Len>::defined();
    }

    /** Writes a value to memory. For simplicity, this is a no-op. */
    template <size_t Len> void
    writeMemory(X86SegmentRegister segreg, const ValueType<32> &addr, const ValueType<Len> &data, ValueType<1> cond) {
    }

    /*************************************************************************************************************************
     * Functions invoked by the X86InstructionSemantics class for arithmetic operations
     *************************************************************************************************************************/

    /** Adds two values.  We know the bits of the result from the least significant bit up to the point where one of the
     *  addends has an undefined bit.  We don't know higher bits past that point because we don't know whether we've carried or
     *  not. */
    template <size_t Len>
    ValueType<Len> add(const ValueType<Len> &a, const ValueType<Len> &b) const {
        uint64_t defbits = 0;
        for (size_t i=0; i<Len && 0!=(a.defbits & ((uint64_t)1<<i)) && 0!=(b.defbits & ((uint64_t)1<<i)); ++i)
            defbits |= (uint64_t)1<<i;
        return defbits;
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
        if (0==c.defbits)
            return carry_out = ValueType<Len>::undefined();

        ValueType<Len> retval = add(a, b);
        carry_out = (retval.defbits << 1);
        return retval;
    }

    /** Computes bit-wise AND of two values. */
    template <size_t Len>
    ValueType<Len> and_(const ValueType<Len> &a, const ValueType<Len> &b) const {
        return a.defbits & b.defbits; // result defined where both a and b are defined
    }

    /** Returns true_, false_, or undefined_ depending on whether argument is zero. */
    template <size_t Len>
    ValueType<1> equalToZero(const ValueType<Len> &a) const {
        return a.is_defined() ? ValueType<1>::defined() : ValueType<1>::undefined();
    }

    /** One's complement. */
    template <size_t Len>
    ValueType<Len> invert(const ValueType<Len> &a) const {
        return a; // no change in defined bits
    }

    /** Concatenate the values of @p a and @p b so that the result has @p b in the high-order bits and @p a in the low order
     *  bits. */
    template<size_t Len1, size_t Len2>
    ValueType<Len1+Len2> concat(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
        return a.defbits | (b.defbits << Len1);
    }

    /** Returns second or third arg depending on value of first arg.  In terms of which bits are defined... the result contains
     *  either the bits from ifTrue or from ifFalse, but we don't know which (because this policy doesn't know the value of
     *  ifFalse). In any case, if a bit is defined in both ifTrue and ifFalse then we know it will be defined in the result. */
    template <size_t Len>
    ValueType<Len> ite(const ValueType<1> &sel, const ValueType<Len> &ifTrue, const ValueType<Len> &ifFalse) const {
        return ifTrue.defbits & ifFalse.defbits;
    }

    /** Returns position of least significant set bit; zero when no bits are set.  In terms of which bits are defined, all we
     *  know is that the result is defined if we know all the bits of @p a.  If not all bits of @p a are defined, then the
     *  result might be undefined and we need to be conservative. */
    template <size_t Len>
    ValueType<Len> leastSignificantSetBit(const ValueType<Len> &a) const {
        return a.is_defined() ? ValueType<Len>::defined() : ValueType<Len>::undefined();
    }

    /** Returns position of most significant set bit; zero when no bits are set. See leastSignificantSetBit(). */
    template <size_t Len>
    ValueType<Len> mostSignificantSetBit(const ValueType<Len> &a) const {
        return a.is_defined() ? ValueType<Len>::defined() : ValueType<Len>::undefined();
    }

    /** Two's complement. */
    template <size_t Len>
    ValueType<Len> negate(const ValueType<Len> &a) const {
        return add(invert(a), ValueType<Len>::defined());
    }

    /** Computes bit-wise OR of two values. In terms of which bits are defined, the result is defined only where both operands
     *  are defined. */
    template <size_t Len>
    ValueType<Len> or_(const ValueType<Len> &a, const ValueType<Len> &b) const {
        return a.defbits & b.defbits;
    }

    /** Rotate bits to the left.  In terms of which bits are defined, we know that all the result bits are defined if all the
     *  @p a operand bits are defined, even if the rotation distance is undefined.  If any of the @p a operand bits are
     *  undefined then we don't know where those undefined bits end up in the result and must conservatively mark all results
     *  bits as undefined. */
    template <size_t Len, size_t SALen>
    ValueType<Len> rotateLeft(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
        return a.is_defined() ? ValueType<Len>::defined() : ValueType<Len>::undefined();
    }

    /** Rotate bits to the right.  See rotateLeft()*/
    template <size_t Len, size_t SALen>
    ValueType<Len> rotateRight(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
        return a.is_defined() ? ValueType<Len>::defined() : ValueType<Len>::undefined();
    }

    /** Returns arg shifted left. Bits shifted into the result are defined to be zero, but we don't know how many bits
     *  were shifted in. All bits up to the first undefined bit of the operand (exclusive) will continue to be defined in the
     *  result, but all higher bits will become undefined. */
    template <size_t Len, size_t SALen>
    ValueType<Len> shiftLeft(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
        uint64_t result = 0;
        for (size_t i=0; i<Len && 0!=(a.defbits & ((uint64_t)1<<i)); ++i)
            result |= (uint64_t)1 << i;
        return result;
    }

    /** Returns arg shifted right logically (no sign bit).  Bits shifted into the high-order side of the result are defined
     *  (always zero), but we don't know how many bits were shifted in.  Therefore, the result will have high order bits defined
     *  down to the first operand bit that's undefined (exclusive), and all lower bits of the result will be undefined. */
    template <size_t Len, size_t SALen>
    ValueType<Len> shiftRight(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
        uint64_t result = 0;
        for (size_t i=Len+1; i>0 && 0!=(a.defbits & ((uint64_t)1<<(i-1))); --i)
            result |= (uint64_t)1 << (i-1);
        return result;
    }

    /** Returns arg shifted right arithmetically (with sign bit).  Identical to shiftRight() in terms of which bits are defined
     *  in the result. */
    template <size_t Len, size_t SALen>
    ValueType<Len> shiftRightArithmetic(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
        return shiftRight(a, sa);
    }

    /** Divides two signed values. The result is defined if and only if both operands are defined. */
    template <size_t Len1, size_t Len2>
    ValueType<Len1> signedDivide(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
        // FIXME: could be made more precise, like signedMultiply()
        return a.is_defined() && b.is_defined() ? ValueType<Len1>::defined() : ValueType<Len1>::undefined();
    }

    /** Calculates modulo with signed values.  The result is defined if and only if both operands are defined. */
    template <size_t Len1, size_t Len2>
    ValueType<Len2> signedModulo(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
        return a.is_defined() && b.is_defined() ? ValueType<Len2>::defined() : ValueType<Len2>::undefined();
    }

    /** Multiplies two signed values.  Only part of the result is defined (some low-order bits) if either of the multiplicands
     *  are not fully defined. */
    template <size_t Len1, size_t Len2>
    ValueType<Len1+Len2> signedMultiply(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
        size_t a_ndef = 0;
        if (a.is_defined()) {
            a_ndef = Len1+Len2;
        } else {
            while (a_ndef<Len1 && 0!=(a.defbits & ((uint64_t)1<<a_ndef)))
                ++a_ndef;
        }

        size_t b_ndef = 0;
        if (b.is_defined()) {
            b_ndef = Len1+Len2;
        } else {
            while (b_ndef<Len1 && 0!=(b.defbits & ((uint64_t)1<<b_ndef)))
                ++b_ndef;
        }

        uint64_t result_ndef = std::min(a_ndef, b_ndef);
        uint64_t result = ((uint64_t)1 << result_ndef) - 1;
        return result;
    }

    /** Divides two unsigned values. The result is defined if and only if both operands are defined. */
    template <size_t Len1, size_t Len2>
    ValueType<Len1> unsignedDivide(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
        // FIXME: could be made more precise, like unsignedMultiply()
        return a.is_defined() && b.is_defined() ? ValueType<Len1>::defined() : ValueType<Len1>::undefined();
    }

    /** Calculates modulo with unsigned values. The result is defined if and only if both operands are defined. */
    template <size_t Len1, size_t Len2>
    ValueType<Len2> unsignedModulo(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
        return a.is_defined() && b.is_defined() ? ValueType<Len2>::defined() : ValueType<Len2>::undefined();
    }

    /** Multiply two unsigned values. The result is defined if and only if both operands are defined. */
    template <size_t Len1, size_t Len2>
    ValueType<Len1+Len2> unsignedMultiply(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
        size_t a_ndef = 0;
        if (a.is_defined()) {
            a_ndef = Len1+Len2;
        } else {
            while (a_ndef<Len1 && 0!=(a.defbits & ((uint64_t)1<<a_ndef)))
                ++a_ndef;
        }

        size_t b_ndef = 0;
        if (b.is_defined()) {
            b_ndef = Len1+Len2;
        } else {
            while (b_ndef<Len1 && 0!=(b.defbits & ((uint64_t)1<<b_ndef)))
                ++b_ndef;
        }

        uint64_t result_ndef = std::min(a_ndef, b_ndef);
        uint64_t result = ((uint64_t)1 << result_ndef) - 1;
        return result;
    }

    /** Computes bit-wise XOR of two values.  The result bits are defined when both operand bits are defined. */
    template <size_t Len>
    ValueType<Len> xor_(const ValueType<Len> &a, const ValueType<Len> &b) const {
        return a.defbits & b.defbits;
    }
};

}; // namespace

#endif
