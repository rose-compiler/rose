#ifndef ROSE_X86INSTRUCTIONSEMANTICS_H
#define ROSE_X86INSTRUCTIONSEMANTICS_H

/* See tests/roseTests/binaryTests/SemanticVerification for ideas about how to test your work here. Note that the test is not
 * run automatically because it depends on setting up a slave machine who's architecture is what is being simulated by the
 * instruction semantics (not necessarily the same architecture that's running ROSE). */

#include "semanticsModule.h"
#include <cassert>
#include <cstdio>
#include <iostream>
#include "integerOps.h"
#include "AsmUnparser_compat.h" /* for unparseInstructionWithAddress() */

/* Returns the segment register corresponding to the specified register reference address expression. */
static inline X86SegmentRegister getSegregFromMemoryReference(SgAsmMemoryReferenceExpression* mr) {
    X86SegmentRegister segreg = x86_segreg_none;
    SgAsmx86RegisterReferenceExpression* seg = isSgAsmx86RegisterReferenceExpression(mr->get_segment());
    if (seg) {
        ROSE_ASSERT(seg->get_descriptor().get_major() == x86_regclass_segment);
        segreg = (X86SegmentRegister)(seg->get_descriptor().get_minor());
    } else {
        ROSE_ASSERT(!"Bad segment expr");
    }
    if (segreg == x86_segreg_none) segreg = x86_segreg_ds;
    return segreg;
}

namespace BinaryAnalysis {
    namespace InstructionSemantics {

/** Translation class.  Translates x86 instructions to RISC-like operations and invokes those operations in the supplied
 *  semantic policy (a template argument).  See the BinaryAnalysis::InstructionSemantics name space for details. Apologies for
 *  the lack of documentation for this class.  You can at least find some examples in the semantics.C file of the
 *  tests/roseTests/binaryTests directory, among others. */
template <typename Policy, template <size_t> class WordType>
struct X86InstructionSemantics {
#   ifdef Word
#       error "Having a macro called \"Word\" conflicts with x86InstructionSemantics.h"
#   else
#       define Word(Len) WordType<(Len)>
#   endif

    struct Exception {
        Exception(const std::string &mesg, SgAsmInstruction *insn): mesg(mesg), insn(insn) {}
        friend std::ostream& operator<<(std::ostream &o, const Exception &e) {
            o <<"instruction semantics: " <<e.mesg;
            if (e.insn) o <<" [" <<unparseInstructionWithAddress(e.insn) <<"]";
            return o;
        }
        std::string mesg;
        SgAsmInstruction *insn;
    };

    Policy& policy;
    SgAsmInstruction *current_instruction;
    Word(32) orig_eip; // set at the top of translate()

    /* Registers used explicitly by this class. */
    RegisterDescriptor REG_EAX, REG_EBX, REG_ECX, REG_EDX, REG_EDI, REG_EIP, REG_ESI, REG_ESP, REG_EBP;
    RegisterDescriptor REG_AX, REG_CX, REG_DX, REG_AL, REG_AH;
    RegisterDescriptor REG_EFLAGS, REG_AF, REG_CF, REG_DF, REG_OF, REG_PF, REG_SF, REG_ZF;

    X86InstructionSemantics(Policy& policy)
        : policy(policy), current_instruction(NULL), orig_eip(policy.readRegister<32>(policy.findRegister("eip"))) {
        REG_EAX = policy.findRegister("eax", 32);
        REG_EBX = policy.findRegister("ebx", 32);
        REG_ECX = policy.findRegister("ecx", 32);
        REG_EDX = policy.findRegister("edx", 32);
        REG_EDI = policy.findRegister("edi", 32);
        REG_EIP = policy.findRegister("eip", 32);
        REG_ESI = policy.findRegister("esi", 32);
        REG_ESP = policy.findRegister("esp", 32);
        REG_EBP = policy.findRegister("ebp", 32);

        REG_AX  = policy.findRegister("ax", 16);
        REG_CX  = policy.findRegister("cx", 16);
        REG_DX  = policy.findRegister("dx", 16);

        REG_AL  = policy.findRegister("al", 8);
        REG_AH  = policy.findRegister("ah", 8);

        REG_EFLAGS=policy.findRegister("eflags", 32);
        REG_AF  = policy.findRegister("af", 1);
        REG_CF  = policy.findRegister("cf", 1);
        REG_DF  = policy.findRegister("df", 1);
        REG_OF  = policy.findRegister("of", 1);
        REG_PF  = policy.findRegister("pf", 1);
        REG_SF  = policy.findRegister("sf", 1);
        REG_ZF  = policy.findRegister("zf", 1);
    }
    virtual ~X86InstructionSemantics() {}

    /** Beginning of a 'rep', 'repe', or 'repne' loop. The return value is the condition status, and is true if the loop body
     * should execute, false otherwise. */
    WordType<1> rep_enter() {
        return policy.invert(policy.equalToZero(readRegister<32>(REG_ECX)));
    }

    /** Decrement the counter for a 'rep', 'repe', or 'repne' loop and adjust the instruction pointer.  The instruction pointer
     *  is reset to the beginning of the instruction if the loop counter, cx register, is non-zero after decrementing and @p
     *  repeat is true. Otherwise the instruction pointer is not adjusted and the loop effectively exits.  If @p cond is false
     *  then this function has no effect on the state. */
    void rep_repeat(SgAsmx86Instruction *insn, WordType<1> repeat, WordType<1> cond) {
        WordType<32> new_cx = policy.add(readRegister<32>(REG_ECX),
                                         policy.ite(cond,
                                                    number<32>(-1),
                                                    number<32>(0)));
        writeRegister(REG_ECX, new_cx);
        repeat = policy.and_(repeat, policy.invert(policy.equalToZero(new_cx)));
        writeRegister(REG_EIP,
                      policy.ite(policy.and_(cond, repeat),
                                 orig_eip,    /* repeat */
                                 readRegister<32>(REG_EIP)));                    /* exit */
    }

    /** Return the value of the memory pointed to by the SI register. */
    template<size_t N>
    WordType<8*N> stringop_load_si(SgAsmx86Instruction *insn, WordType<1> cond) {
        return readMemory<8*N>((insn->get_segmentOverride() == x86_segreg_none ? x86_segreg_ds : insn->get_segmentOverride()),
                               readRegister<32>(REG_ESI),
                               cond);
    }

    /** Return the value of memory pointed to by the DI register. */
    template<size_t N>
    WordType<8*N> stringop_load_di(WordType<1> cond) {
        return readMemory<8*N>(x86_segreg_es, readRegister<32>(REG_EDI), cond);
    }

    /** Instruction semantics for stosN where N is 1 (b), 2 (w), or 4 (d). If @p cond is false then this instruction does not
     *  change any state. */
    template<size_t N>
    void stos_semantics(SgAsmx86Instruction *insn, WordType<1> cond) {
        const SgAsmExpressionPtrList& operands = insn->get_operandList()->get_operands();
        if (operands.size()!=0)
            throw Exception("instruction must have no operands", insn);
        if (insn->get_addressSize()!=x86_insnsize_32)
            throw Exception("address size must be 32 bits", insn);

        /* Fill memory pointed to by ES:[DI] with contents of AX. */
        policy.writeMemory(x86_segreg_es,
                           readRegister<32>(REG_EDI),
                           extract<0, 8*N>(readRegister<32>(REG_EAX)),
                           cond);

        /* Update DI */
        writeRegister(REG_EDI,
                      policy.ite(cond,
                                 policy.add(readRegister<32>(REG_EDI),
                                            policy.ite(readRegister<1>(REG_DF), number<32>(-N), number<32>(N))),
                                 readRegister<32>(REG_EDI)));
    }

    /** Instruction semantics for rep_stosN where N is 1 (b), 2 (w), or 4 (d). This method handles semantics for one iteration
     * of stosN. See https://siyobik.info/index.php?module=x86&id=279 */
    template<size_t N>
    void rep_stos_semantics(SgAsmx86Instruction *insn) {
        WordType<1> in_loop = rep_enter();
        stos_semantics<N>(insn, in_loop);
        rep_repeat(insn, policy.true_(), in_loop);
    }

    /** Instruction semantics for movsN where N is 1 (b), 2 (w), or 4 (d). If @p cond is false then this instruction does not
     * change any state. */
    template<size_t N>
    void movs_semantics(SgAsmx86Instruction *insn, WordType<1> cond) {
        const SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
        if (operands.size()!=0)
            throw Exception("instruction must have no operands", insn);
        if (insn->get_addressSize() != x86_insnsize_32)
            throw Exception("size not implemented", insn);

        policy.writeMemory(x86_segreg_es,
                           readRegister<32>(REG_EDI),
                           stringop_load_si<N>(insn, cond),
                           cond);
        writeRegister(REG_ESI,
                      policy.add(readRegister<32>(REG_ESI),
                                 policy.ite(cond,
                                            policy.ite(readRegister<1>(REG_DF),
                                                       number<32>(-(N)),
                                                       number<32>(N)),
                                            number<32>(0))));
        writeRegister(REG_EDI,
                      policy.add(readRegister<32>(REG_EDI),
                                 policy.ite(cond,
                                            policy.ite(readRegister<1>(REG_DF),
                                                       number<32>(-(N)),
                                                       number<32>(N)),
                                            number<32>(0))));
    }

    /** Instruction semantics for rep_movsN where N is 1 (b), 2 (w), or 4 (d). This method handles semantics for one iteration
     *  of the instruction. */
    template<size_t N>
    void rep_movs_semantics(SgAsmx86Instruction *insn) {
        WordType<1> in_loop = rep_enter();
        movs_semantics<N>(insn, in_loop);
        rep_repeat(insn, policy.true_(), in_loop);
    }

    /** Instruction semantics for cmpsN where N is 1 (b), 2 (w), or 4 (d).  If @p cond is false then this instruction does not
     * change any state. See Intel Instruction Set Reference 3-154 Vol 2a, March 2009 for opcodes 0xa6 and 0xa7 with no prefix. */
    template<size_t N>
    void cmps_semantics(SgAsmx86Instruction *insn, WordType<1> cond) {
        const SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
        if (operands.size()!=0)
            throw Exception("instruction must have no operands", insn);
        if (insn->get_addressSize() != x86_insnsize_32)
            throw Exception("size not implemented", insn);
        doAddOperation<8*N>(stringop_load_si<N>(insn, cond),
                            policy.invert(stringop_load_di<N>(cond)),
                            true,
                            policy.false_(),
                            cond);
        writeRegister(REG_ESI,
                      policy.ite(cond,
                                 policy.add(readRegister<32>(REG_ESI),
                                            policy.ite(readRegister<1>(REG_DF), number<32>(-N), number<32>(N))),
                                 readRegister<32>(REG_ESI)));
        writeRegister(REG_EDI,
                      policy.ite(cond,
                                 policy.add(readRegister<32>(REG_EDI),
                                            policy.ite(readRegister<1>(REG_DF), number<32>(-N), number<32>(N))),
                                 readRegister<32>(REG_EDI)));
    }

    /** Instruction semantics for one iteration of the repe_cmpsN instruction, where N is 1 (b), 2 (w), or 4 (d). */
    template<size_t N>
    void repe_cmps_semantics(SgAsmx86Instruction *insn) {
        WordType<1> in_loop = rep_enter();
        cmps_semantics<N>(insn, in_loop);
        WordType<1> repeat = readRegister<1>(REG_ZF);
        rep_repeat(insn, repeat, in_loop);
    }

    /** Instruction semantics for one iteration of the repne_cmpsN instruction, where N is 1 (b), 2 (w), or 4 (d). */
    template<size_t N>
    void repne_cmps_semantics(SgAsmx86Instruction *insn) {
        WordType<1> in_loop = rep_enter();
        cmps_semantics<N>(insn, in_loop);
        WordType<1> repeat = policy.invert(readRegister<1>(REG_ZF));
        rep_repeat(insn, repeat, in_loop);
    }

    /** Instruction semantics for scasN where N is 1 (b), 2 (w), or 4 (d). If @p cond is false then this instruction does not
     * change any state. */
    template<size_t N>
    void scas_semantics(SgAsmx86Instruction *insn, WordType<1> cond) {
        const SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
        if (operands.size()!=0)
            throw Exception("instruction must have no operands", insn);
        if (insn->get_addressSize() != x86_insnsize_32)
            throw Exception("size not implemented", insn);
        doAddOperation<8*N>(extract<0, 8*N>(readRegister<32>(REG_EAX)),
                            policy.invert(stringop_load_di<N>(cond)),
                            true,
                            policy.false_(),
                            cond);
        writeRegister(REG_EDI,
                      policy.ite(cond,
                                 policy.add(readRegister<32>(REG_EDI),
                                            policy.ite(readRegister<1>(REG_DF), number<32>(-N), number<32>(N))),
                                 readRegister<32>(REG_EDI)));
    }

    /** Instruction semantics for one iteration of repe_scasN where N is 1 (b), 2 (w), or 4 (d). */
    template<size_t N>
    void repe_scas_semantics(SgAsmx86Instruction *insn) {
        WordType<1> in_loop = rep_enter();
        scas_semantics<N>(insn, in_loop);
        WordType<1> repeat = readRegister<1>(REG_ZF);
        rep_repeat(insn, repeat, in_loop);
    }

    /** Instruction semantics for one iterator of repne_scasN where N is 1 (b), 2 (w), or 4 (d). */
    template<size_t N>
    void repne_scas_semantics(SgAsmx86Instruction *insn) {
        WordType<1> in_loop = rep_enter();
        scas_semantics<N>(insn, in_loop);
        WordType<1> repeat = policy.invert(readRegister<1>(REG_ZF));
        rep_repeat(insn, repeat, in_loop);
    }

    /** Helper for lods_semantics() to load one byte into the AL register. */
    void lods_semantics_regupdate(WordType<8> v) {
        writeRegister(REG_AL, v);
    }

    /** Helper for lods_semantics() to load one word into the AX register. */
    void lods_semantics_regupdate(WordType<16> v) {
        writeRegister(REG_AX, v);
    }

    /** Helper for lods_semantics() to load one doubleword into the EAX register. */
    void lods_semantics_regupdate(WordType<32> v) {
        writeRegister(REG_EAX, v);
    }
        
    /** Instruction semantics for lodsN where N is 1 (b), 2 (w), or 4 (d). */
    template<size_t N>
    void lods_semantics(SgAsmx86Instruction *insn) {
        const SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
        if (operands.size()!=0)
            throw Exception("instruction must have no operands", insn);
        if (insn->get_addressSize() != x86_insnsize_32)
            throw Exception("size not implemented", insn);
        lods_semantics_regupdate(stringop_load_si<N>(insn, policy.true_()));
        writeRegister(REG_ESI,
                      policy.add(readRegister<32>(REG_ESI),
                                 policy.ite(readRegister<1>(REG_DF), number<32>(-N), number<32>(N))));
    }

    /** Implements the SHR, SAR, SHL, SAL, SHRD, and SHLD instructions for various operand sizes.  The shift amount is always 8
     *  bits wide in the instruction, but the semantics mask off all but the low-order bits, keeping 5 bits in 32-bit mode and
     *  7 bits in 64-bit mode (indicated by the shiftSignificantBits template argument).  The semantics of SHL and SAL are
     *  identical (in fact, ROSE doesn't even define x86_sal). The @p source_bits argument contains the bits to be shifted into
     *  the result and is used only for SHRD and SHLD instructions. */
    template<size_t operandBits, size_t shiftSignificantBits>
    WordType<operandBits> shift_semantics(X86InstructionKind kind, const WordType<operandBits> &operand,
                                          const WordType<operandBits> &source_bits, const WordType<8> &total_shift) {
        assert(x86_shr==kind || x86_sar==kind || x86_shl==kind || x86_shld==kind || x86_shrd==kind);

        // The 8086 does not mask the shift count; processors starting with the 80286 (including virtual-8086 mode) do
        // mask.  The effect (other than timing) is the same either way.
        WordType<shiftSignificantBits> maskedShiftCount = extract<0, shiftSignificantBits>(total_shift);
        WordType<1> isZeroShiftCount = policy.equalToZero(maskedShiftCount);

        // isLargeShift is true if the (unmasked) amount by which to shift is greater than or equal to the size in
        // bits of the destination operand.
        assert(shiftSignificantBits<8);
        WordType<1> isLargeShift = policy.invert(policy.equalToZero(extract<shiftSignificantBits, 8>(total_shift)));

        // isOneBitShift is true if the (masked) amount by which to shift is equal to one.
        uintmax_t m = ((uintmax_t)1 << shiftSignificantBits) - 1;
        WordType<shiftSignificantBits> mask = number<shiftSignificantBits>(m); // -1 in modulo arithmetic
        WordType<1> isOneBitShift = policy.equalToZero(policy.add(maskedShiftCount, mask));

        // Do the actual shift, according to instruction kind.
        WordType<operandBits> retval = undefined_<operandBits>(); // not all policies define a default c'tor
        switch (kind) {
            case x86_shr:
                retval = policy.shiftRight(operand, maskedShiftCount);
                break;
            case x86_sar:
                retval = policy.shiftRightArithmetic(operand, maskedShiftCount);
                break;
            case x86_shl:
                retval = policy.shiftLeft(operand, maskedShiftCount);
                break;
            case x86_shrd:
                retval = policy.ite(isLargeShift,
                                    undefined_<operandBits>(),
                                    policy.or_(policy.shiftRight(operand, maskedShiftCount),
                                               policy.ite(isZeroShiftCount,
                                                          number<operandBits>(0),
                                                          policy.shiftLeft(source_bits, policy.negate(maskedShiftCount)))));
                break;
            case x86_shld:
                retval = policy.ite(isLargeShift,
                                    undefined_<operandBits>(),
                                    policy.or_(policy.shiftLeft(operand, maskedShiftCount),
                                               policy.ite(isZeroShiftCount,
                                                          number<operandBits>(0),
                                                          policy.shiftRight(source_bits, policy.negate(maskedShiftCount)))));
                break;
            default:
                abort();
        }

        // The AF flag is undefined if a shift occurs.  The documentation for SHL, SHR, and SAR are somewhat ambiguous about
        // this, but the documentation for SHLD and SHRD is more specific.  We assume that both sets of shift instructions
        // behave the same way.
        writeRegister(REG_AF, policy.ite(isZeroShiftCount, readRegister<1>(REG_AF), undefined_<1>()));

        // What is the last bit shifted off the operand?  If we're right shifting by N bits, then the original operand N-1 bit
        // is what should make it into the final CF; if we're left shifting by N bits then we need bit operandBits-N.
        WordType<shiftSignificantBits> bitPosition;
        if (x86_shr==kind || x86_sar==kind) {
            bitPosition = policy.add(maskedShiftCount, mask);
        } else {
            bitPosition = policy.add(number<shiftSignificantBits>(operandBits & m), // probably zero in modulo arithmetic
                                     policy.add(policy.invert(maskedShiftCount),
                                                number<shiftSignificantBits>(1)));
        }
        WordType<1> shifted_off = extract<0, 1>(policy.shiftRight(operand, bitPosition));

        // New carry flag value.  From the Intel manual, the CF flag is "undefined for SHL and SHR [and SAL] instructions where
        // the count is greater than or equal to the size (in bits) of the destination operand", and "if the count is 0, the
        // flags are not affected."  The manual is silent about the value of CF for large SAR shifts, so we use the original
        // sign bit, matching the pseudo-code in the manual.
        WordType<1> newCF = policy.ite(isZeroShiftCount,
                                        readRegister<1>(REG_CF), // preserve current value
                                        policy.ite(isLargeShift,
                                                   (x86_sar==kind ?
                                                    extract<operandBits-1, operandBits>(operand) : // original sign bit
                                                    undefined_<1>()),
                                                   shifted_off));
        writeRegister(REG_CF, newCF);

        // Ajust the overflow flag.  From the Intel manual for the SHL, SHR, and SAR instructions, "The OF flag is affected
        // only on 1-bit shifts.  For left shifts, the OF flag is set to 0 if the most-significant bit of the result is the
        // same as the CF flag (that is, the top two bits of the original operand were the same); otherwise, it is set to 1.
        // For the SAR instruction, the OF flag is cleared for all 1-bit shifts.  For the SHR instruction, the OF flag is set
        // to the most-significant bit of the original operand."  Later, it states that "the OF flag is affected only for 1-bit
        // shifts; otherwise it is undefined."  We're assuming that the statement "if the count is 0, then the flags are not
        // affected" takes precedence. For SHLD and SHRD it says, "for a 1-bit shift, the OF flag is set if a sign changed
        // occurred; otherwise it is cleared. For shifts greater than 1 bit, the OF flag is undefined."
        WordType<1> newOF = undefined_<1>();
        switch (kind) {
            case x86_shr:
                newOF = policy.ite(isOneBitShift,
                                   extract<operandBits-1, operandBits>(operand),
                                   policy.ite(isZeroShiftCount, 
                                              readRegister<1>(REG_OF),
                                              undefined_<1>()));
                break;
            case x86_sar:
                newOF = policy.ite(isOneBitShift,
                                   policy.false_(),
                                   policy.ite(isZeroShiftCount,
                                              readRegister<1>(REG_OF),
                                              undefined_<1>()));
                break;
            case x86_shl:
            case x86_shld:
            case x86_shrd:
                newOF = policy.ite(isOneBitShift,
                                   policy.xor_(newCF, extract<operandBits-1, operandBits>(retval)),
                                   policy.ite(isZeroShiftCount,
                                              readRegister<1>(REG_OF),
                                              undefined_<1>()));
                break;
            default: // to shut up compiler warnings even though we would have aborted by now.
                abort();
        }
        writeRegister(REG_OF, newOF);

        // Result flags SF, ZF, and PF are set according to the result, but are unchanged if the shift count is zero.
        setFlagsForResult<operandBits>(retval, policy.invert(isZeroShiftCount));
        return retval;
    }

    template <size_t Len>
    Word(Len) invertMaybe(const Word(Len)& w, bool inv) {
        if (inv) {
            return policy.invert(w);
        } else {
            return w;
        }
    }

    template <size_t Len>
    Word(Len) number(uintmax_t v) {
        return policy.template number<Len>(v);
    }

    template <size_t Len>
    Word(Len) undefined_() {
        return policy.template undefined_<Len>();
    }

    template <size_t From, size_t To, size_t Len>
        Word(To - From) extract(Word(Len) w) {
        return policy.template extract<From, To>(w);
    }

    template <size_t From, size_t To>
        Word(To) signExtend(Word(From) w) {
        return policy.template signExtend<From, To>(w);
    }

    /** Reads from a named register. Delegated to policy. */
    template<size_t Len>
    Word(Len) readRegister(const RegisterDescriptor &reg) {
        return policy.template readRegister<Len>(reg);
    }

    /** Writes to a named register. Delegated to policy. */
    template<size_t Len>
    void writeRegister(const RegisterDescriptor &reg, const Word(Len) &value) {
        policy.template writeRegister<Len>(reg, value);
    }

    template <size_t Len>
    Word(1) greaterOrEqualToTen(Word(Len) w) {
        Word(Len) carries = number<Len>(0);
        policy.addWithCarries(w, number<Len>(6), policy.false_(), carries);
        return extract<Len - 1, Len>(carries);
    }

    template <size_t Len/*bits*/>
    Word(Len) readMemory(X86SegmentRegister segreg, const Word(32)& addr, Word(1) cond) {
        return policy.template readMemory<Len>(segreg, addr, cond);
    }

    Word(32) readEffectiveAddress(SgAsmExpression* expr) {
        assert (isSgAsmMemoryReferenceExpression(expr));
        return read32(isSgAsmMemoryReferenceExpression(expr)->get_address());
    }

    /* Returns an eight-bit value desribed by an instruction operand. */
    Word(8) read8(SgAsmExpression* e) {
        switch (e->variantT()) {
            case V_SgAsmx86RegisterReferenceExpression: {
                SgAsmx86RegisterReferenceExpression* rre = isSgAsmx86RegisterReferenceExpression(e);
                return policy.readRegister<8>(rre->get_descriptor());
            }
            case V_SgAsmBinaryAdd: {
                return policy.add(read8(isSgAsmBinaryAdd(e)->get_lhs()), read8(isSgAsmBinaryAdd(e)->get_rhs()));
            }
            case V_SgAsmBinaryMultiply: {
                SgAsmByteValueExpression* rhs = isSgAsmByteValueExpression(isSgAsmBinaryMultiply(e)->get_rhs());
                if (!rhs)
                    throw Exception("byte value expression expected", current_instruction);
                SgAsmExpression* lhs = isSgAsmBinaryMultiply(e)->get_lhs();
                return extract<0, 8>(policy.unsignedMultiply(read8(lhs), read8(rhs)));
            }
            case V_SgAsmMemoryReferenceExpression: {
                return readMemory<8>(getSegregFromMemoryReference(isSgAsmMemoryReferenceExpression(e)),
                                     readEffectiveAddress(e), policy.true_());
            }
            case V_SgAsmByteValueExpression:
            case V_SgAsmWordValueExpression:
            case V_SgAsmDoubleWordValueExpression:
            case V_SgAsmQuadWordValueExpression: {
                uint64_t val = SageInterface::getAsmSignedConstant(isSgAsmValueExpression(e));
                return number<8>(val & 0xFFU);
            }
            default: {
                fprintf(stderr, "Bad variant %s in read8\n", e->class_name().c_str());
                abort();
            }
        }
    }

    /* Returns a 16-bit value described by an instruction operand. */
    Word(16) read16(SgAsmExpression* e) {
        switch (e->variantT()) {
            case V_SgAsmx86RegisterReferenceExpression: {
                SgAsmx86RegisterReferenceExpression* rre = isSgAsmx86RegisterReferenceExpression(e);
                return policy.readRegister<16>(rre->get_descriptor());
            }
            case V_SgAsmBinaryAdd: {
                return policy.add(read16(isSgAsmBinaryAdd(e)->get_lhs()), read16(isSgAsmBinaryAdd(e)->get_rhs()));
            }
            case V_SgAsmBinaryMultiply: {
                SgAsmByteValueExpression* rhs = isSgAsmByteValueExpression(isSgAsmBinaryMultiply(e)->get_rhs());
                if (!rhs)
                    throw Exception("byte value expression expected", current_instruction);
                SgAsmExpression* lhs = isSgAsmBinaryMultiply(e)->get_lhs();
                return extract<0, 16>(policy.unsignedMultiply(read16(lhs), read8(rhs)));
            }
            case V_SgAsmMemoryReferenceExpression: {
                return readMemory<16>(getSegregFromMemoryReference(isSgAsmMemoryReferenceExpression(e)),
                                      readEffectiveAddress(e), policy.true_());
            }
            case V_SgAsmByteValueExpression:
            case V_SgAsmWordValueExpression:
            case V_SgAsmDoubleWordValueExpression:
            case V_SgAsmQuadWordValueExpression: {
                uint64_t val = SageInterface::getAsmSignedConstant(isSgAsmValueExpression(e));
                return number<16>(val & 0xFFFFU);
            }
            default: {
                fprintf(stderr, "Bad variant %s in read16\n", e->class_name().c_str());
                abort();
            }
        }
    }

    /* Returns a 32-bit value described by an instruction operand. */
    Word(32) read32(SgAsmExpression* e) {
        switch (e->variantT()) {
            case V_SgAsmx86RegisterReferenceExpression: {
                SgAsmx86RegisterReferenceExpression* rre = isSgAsmx86RegisterReferenceExpression(e);
                return policy.readRegister<32>(rre->get_descriptor());
            }
            case V_SgAsmBinaryAdd: {
                return policy.add(read32(isSgAsmBinaryAdd(e)->get_lhs()), read32(isSgAsmBinaryAdd(e)->get_rhs()));
            }
            case V_SgAsmBinaryMultiply: {
                SgAsmByteValueExpression* rhs = isSgAsmByteValueExpression(isSgAsmBinaryMultiply(e)->get_rhs());
                if (!rhs)
                    throw Exception("byte value expression expected", current_instruction);
                SgAsmExpression* lhs = isSgAsmBinaryMultiply(e)->get_lhs();
                return extract<0, 32>(policy.unsignedMultiply(read32(lhs), read8(rhs)));
            }
            case V_SgAsmMemoryReferenceExpression: {
                return readMemory<32>(getSegregFromMemoryReference(isSgAsmMemoryReferenceExpression(e)),
                                      readEffectiveAddress(e), policy.true_());
            }
            case V_SgAsmByteValueExpression:
            case V_SgAsmWordValueExpression:
            case V_SgAsmDoubleWordValueExpression:
            case V_SgAsmQuadWordValueExpression: {
                uint64_t val = SageInterface::getAsmSignedConstant(isSgAsmValueExpression(e));
                return number<32>(val & 0xFFFFFFFFU);
            }
            default: {
                fprintf(stderr, "Bad variant %s in read32\n", e->class_name().c_str());
                abort();
            }
        }
    }

    /* Writes the specified eight-bit value to the location specified by an instruction operand. */
    void write8(SgAsmExpression* e, const Word(8)& value) {
        switch (e->variantT()) {
            case V_SgAsmx86RegisterReferenceExpression: {
                SgAsmx86RegisterReferenceExpression* rre = isSgAsmx86RegisterReferenceExpression(e);
                policy.writeRegister(rre->get_descriptor(), value);
                break;
            }
            case V_SgAsmMemoryReferenceExpression: {
                policy.writeMemory(getSegregFromMemoryReference(isSgAsmMemoryReferenceExpression(e)),
                                   readEffectiveAddress(e), value, policy.true_());
                break;
            }
            default: {
                fprintf(stderr, "Bad variant %s in write8\n", e->class_name().c_str());
                abort();
            }
        }
    }

    /* Writes the specified 16-bit value to the location specified by an instruction operand. */
    void write16(SgAsmExpression* e, const Word(16)& value) {
        switch (e->variantT()) {
            case V_SgAsmx86RegisterReferenceExpression: {
                SgAsmx86RegisterReferenceExpression* rre = isSgAsmx86RegisterReferenceExpression(e);
                policy.writeRegister(rre->get_descriptor(), value);
                break;
            }
            case V_SgAsmMemoryReferenceExpression: {
                policy.writeMemory(getSegregFromMemoryReference(isSgAsmMemoryReferenceExpression(e)),
                                   readEffectiveAddress(e), value, policy.true_());
                break;
            }
            default: {
                fprintf(stderr, "Bad variant %s in write16\n", e->class_name().c_str());
                abort();
            }
        }
    }

    /* Writes the specified 32-bit value to the location specified by an instruction operand. */
    void write32(SgAsmExpression* e, const Word(32)& value) {
        switch (e->variantT()) {
            case V_SgAsmx86RegisterReferenceExpression: {
                SgAsmx86RegisterReferenceExpression* rre = isSgAsmx86RegisterReferenceExpression(e);
                policy.writeRegister(rre->get_descriptor(), value);
                break;
            }
            case V_SgAsmMemoryReferenceExpression: {
                policy.writeMemory(getSegregFromMemoryReference(isSgAsmMemoryReferenceExpression(e)),
                                   readEffectiveAddress(e), value, policy.true_());
                break;
            }
            default: {
                fprintf(stderr, "Bad variant %s in write32\n", e->class_name().c_str());
                abort();
            }
        }
    }

    /* Returns true if W has an even number of bits set; false for an odd number */
    Word(1) parity(Word(8) w) {
        Word(1) p01 = policy.xor_(extract<0, 1>(w), extract<1, 2>(w));
        Word(1) p23 = policy.xor_(extract<2, 3>(w), extract<3, 4>(w));
        Word(1) p45 = policy.xor_(extract<4, 5>(w), extract<5, 6>(w));
        Word(1) p67 = policy.xor_(extract<6, 7>(w), extract<7, 8>(w));
        Word(1) p0123 = policy.xor_(p01, p23);
        Word(1) p4567 = policy.xor_(p45, p67);
        return policy.invert(policy.xor_(p0123, p4567));
    }

    /* Sets flags: parity, sign, and zero */
    template <size_t Len>
    void setFlagsForResult(const Word(Len)& result) {
        writeRegister(REG_PF, parity(extract<0, 8>(result)));
        writeRegister(REG_SF, extract<Len - 1, Len>(result));
        writeRegister(REG_ZF, policy.equalToZero(result));
    }

    /* Sets flags conditionally. Sets parity, sign, and zero flags if COND is true. */
    template <size_t Len>
    void setFlagsForResult(const Word(Len)& result, Word(1) cond) {
        writeRegister(REG_PF, policy.ite(cond, parity(extract<0, 8>(result)), readRegister<1>(REG_PF)));
        writeRegister(REG_SF, policy.ite(cond, extract<Len - 1, Len>(result), readRegister<1>(REG_SF)));
        writeRegister(REG_ZF, policy.ite(cond, policy.equalToZero(result), readRegister<1>(REG_ZF)));
    }

    /* Adds A and B and adjusts condition flags. Can be used for subtraction if B is two's complement and invertCarries is set. */
    template <size_t Len>
    Word(Len) doAddOperation(const Word(Len)& a, const Word(Len)& b, bool invertCarries, Word(1) carryIn) {
        Word(Len) carries = number<Len>(0);
        Word(Len) result = policy.addWithCarries(a, b, invertMaybe(carryIn, invertCarries), carries/*out*/);
        setFlagsForResult<Len>(result);
        writeRegister(REG_AF, invertMaybe(extract<3, 4>(carries), invertCarries));
        writeRegister(REG_CF, invertMaybe(extract<Len - 1, Len>(carries), invertCarries));
        writeRegister(REG_OF, policy.xor_(extract<Len - 1, Len>(carries), extract<Len - 2, Len - 1>(carries)));
        return result;
    }

    /* Conditionally adds A and B and adjusts condition flags. Can be used for subtraction if B is two's complement and
     * invertCarries is set. Does nothing if COND is false. */ 
    template <size_t Len>
    Word(Len) doAddOperation(const Word(Len)& a, const Word(Len)& b, bool invertCarries, Word(1) carryIn, Word(1) cond) {
        Word(Len) carries = number<Len>(0);
        Word(Len) result = policy.addWithCarries(a, b, invertMaybe(carryIn, invertCarries), carries/*out*/);
        setFlagsForResult<Len>(result, cond);
        writeRegister(REG_AF,
                      policy.ite(cond,
                                 invertMaybe(extract<3, 4>(carries), invertCarries),
                                 readRegister<1>(REG_AF)));
        writeRegister(REG_CF,
                      policy.ite(cond,
                                 invertMaybe(extract<Len - 1, Len>(carries), invertCarries),
                                 readRegister<1>(REG_CF)));
        writeRegister(REG_OF,
                      policy.ite(cond,
                                 policy.xor_(extract<Len - 1, Len>(carries), extract<Len - 2, Len - 1>(carries)),
                                 readRegister<1>(REG_OF)));
        return result;
    }

    /* Does increment (decrement with DEC set), and adjusts condition flags. */
    template <size_t Len>
    Word(Len) doIncOperation(const Word(Len)& a, bool dec, bool setCarry) {
        Word(Len) carries = number<Len>(0);
        Word(Len) result = policy.addWithCarries(a, number<Len>(dec ? -1 : 1), policy.false_(), carries/*out*/);
        setFlagsForResult<Len>(result);
        writeRegister(REG_AF, invertMaybe(extract<3, 4>(carries), dec));
        writeRegister(REG_OF, policy.xor_(extract<Len - 1, Len>(carries), extract<Len - 2, Len - 1>(carries)));
        if (setCarry)
            writeRegister(REG_CF, invertMaybe(extract<Len - 1, Len>(carries), dec));
        return result;
    }

    /* Virtual so that we can subclass X86InstructionSemantics and have an opportunity to override the translation of any
     * instruction. */
#if _MSC_VER
        // tps (02/01/2010) : fixme : Commented this out for Windows - there is a problem with the try:
        // error C2590: 'translate' : only a constructor can have a base/member initializer list
    virtual void translate(SgAsmx86Instruction* insn)  {
        }
#else
    virtual void translate(SgAsmx86Instruction* insn) try {
        orig_eip = readRegister<32>(REG_EIP);
        writeRegister(REG_EIP, policy.add(orig_eip, policy.number<32>(insn->get_size())));
        X86InstructionKind kind = insn->get_kind();
        const SgAsmExpressionPtrList& operands = insn->get_operandList()->get_operands();
        switch (kind) {

            case x86_mov: {
                if (operands.size()!=2)
                    throw Exception("instruction must have two operands", insn);
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: write8(operands[0], read8(operands[1])); break;
                    case 2: write16(operands[0], read16(operands[1])); break;
                    case 4: write32(operands[0], read32(operands[1])); break;
                    default: throw Exception("size not implemented", insn); break;
                }
                break;
            }

            case x86_xchg: {
                if (operands.size()!=2)
                    throw Exception("instruction must have two operands", insn);
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: {
                        Word(8) temp = read8(operands[1]);
                        write8(operands[1], read8(operands[0]));
                        write8(operands[0], temp);
                        break;
                    }
                    case 2: {
                        Word(16) temp = read16(operands[1]);
                        write16(operands[1], read16(operands[0]));
                        write16(operands[0], temp);
                        break;
                    }
                    case 4: {
                        Word(32) temp = read32(operands[1]);
                        write32(operands[1], read32(operands[0]));
                        write32(operands[0], temp);
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                        break;
                }
                break;
            }

            case x86_movzx: {
                if (operands.size()!=2)
                    throw Exception("instruction must have two operands", insn);
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 2: {
                        write16(operands[0], policy.concat(read8(operands[1]), number<8>(0)));
                        break;
                    }
                    case 4: {
                        switch (numBytesInAsmType(operands[1]->get_type())) {
                            case 1: write32(operands[0], policy.concat(read8(operands[1]), number<24>(0))); break;
                            case 2: write32(operands[0], policy.concat(read16(operands[1]), number<16>(0))); break;
                            default: throw Exception("size not implemented", insn);

                        }
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                        break;
                }
                break;
            }

            case x86_movsx: {
                if (operands.size()!=2)
                    throw Exception("instruction must have two operands", insn);
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 2: {
                        Word(8) op1 = read8(operands[1]);
                        Word(16) result = signExtend<8, 16>(op1);
                        write16(operands[0], result);
                        break;
                    }
                    case 4: {
                        switch (numBytesInAsmType(operands[1]->get_type())) {
                            case 1: {
                                Word(8) op1 = read8(operands[1]);
                                Word(32) result = signExtend<8, 32>(op1);
                                write32(operands[0], result);
                                break;
                            }
                            case 2: {
                                Word(16) op1 = read16(operands[1]);
                                Word(32) result = signExtend<16, 32>(op1);
                                write32(operands[0], result);
                                break;
                            }
                            default:
                                throw Exception("size not implemented", insn);
                        }
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                        break;
                }
                break;
            }

            case x86_cbw: {
                if (operands.size()!=0)
                    throw Exception("instruction must have no operands", insn);
                writeRegister(REG_AX, signExtend<8, 16>(readRegister<8>(REG_AL)));
                break;
            }

            case x86_cwde: {
                if (operands.size()!=0)
                    throw Exception("instruction must have no operands", insn);
                writeRegister(REG_EAX, signExtend<16, 32>(readRegister<16>(REG_AX))); 
                break;
            }

            case x86_cwd: {
                if (operands.size()!=0)
                    throw Exception("instruction must have no operands", insn);
                writeRegister(REG_DX, extract<16, 32>(signExtend<16, 32>(readRegister<16>(REG_AX))));
                break;
            }

            case x86_cdq: {
                if (operands.size()!=0)
                    throw Exception("instruction must have no operands", insn);
                writeRegister(REG_EDX, extract<32, 64>(signExtend<32, 64>(readRegister<32>(REG_AX))));
                break;
            }

            case x86_lea: {
                if (operands.size()!=2)
                    throw Exception("instruction must have two operands", insn);
                write32(operands[0], readEffectiveAddress(operands[1]));
                break;
            }

            case x86_and: {
                if (operands.size()!=2)
                    throw Exception("instruction must have two operands", insn);
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: {
                        Word(8) result = policy.and_(read8(operands[0]), read8(operands[1]));
                        setFlagsForResult<8>(result);
                        write8(operands[0], result);
                        break;
                    }
                    case 2: {
                        Word(16) result = policy.and_(read16(operands[0]), read16(operands[1]));
                        setFlagsForResult<16>(result);
                        write16(operands[0], result);
                        break;
                    }
                    case 4: {
                        Word(32) result = policy.and_(read32(operands[0]), read32(operands[1]));
                        setFlagsForResult<32>(result);
                        write32(operands[0], result);
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                        break;
                }
                writeRegister(REG_OF, policy.false_());
                writeRegister(REG_AF, undefined_<1>());
                writeRegister(REG_CF, policy.false_());
                break;
            }

            case x86_or: {
                if (operands.size()!=2)
                    throw Exception("instruction must have two operands", insn);
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: {
                        Word(8) result = policy.or_(read8(operands[0]), read8(operands[1]));
                        setFlagsForResult<8>(result);
                        write8(operands[0], result);
                        break;
                    }
                    case 2: {
                        Word(16) result = policy.or_(read16(operands[0]), read16(operands[1]));
                        setFlagsForResult<16>(result);
                        write16(operands[0], result);
                        break;
                    }
                    case 4: {
                        Word(32) result = policy.or_(read32(operands[0]), read32(operands[1]));
                        setFlagsForResult<32>(result);
                        write32(operands[0], result);
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                        break;
                }
                writeRegister(REG_OF, policy.false_());
                writeRegister(REG_AF, undefined_<1>());
                writeRegister(REG_CF, policy.false_());
                break;
            }

            case x86_test: {
                if (operands.size()!=2)
                    throw Exception("instruction must have two operands", insn);
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: {
                        Word(8) result = policy.and_(read8(operands[0]), read8(operands[1]));
                        setFlagsForResult<8>(result);
                        break;
                    }
                    case 2: {
                        Word(16) result = policy.and_(read16(operands[0]), read16(operands[1]));
                        setFlagsForResult<16>(result);
                        break;
                    }
                    case 4: {
                        Word(32) result = policy.and_(read32(operands[0]), read32(operands[1]));
                        setFlagsForResult<32>(result);
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                        break;
                }
                writeRegister(REG_OF, policy.false_());
                writeRegister(REG_AF, undefined_<1>());
                writeRegister(REG_CF, policy.false_());
                break;
            }

            case x86_xor: {
                if (operands.size()!=2)
                    throw Exception("instruction must have two operands", insn);
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: {
                        Word(8) result = policy.xor_(read8(operands[0]), read8(operands[1]));
                        setFlagsForResult<8>(result);
                        write8(operands[0], result);
                        break;
                    }
                    case 2: {
                        Word(16) result = policy.xor_(read16(operands[0]), read16(operands[1]));
                        setFlagsForResult<16>(result);
                        write16(operands[0], result);
                        break;
                    }
                    case 4: {
                        Word(32) result = policy.xor_(read32(operands[0]), read32(operands[1]));
                        setFlagsForResult<32>(result);
                        write32(operands[0], result);
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                        break;
                }
                writeRegister(REG_OF, policy.false_());
                writeRegister(REG_AF, undefined_<1>());
                writeRegister(REG_CF, policy.false_());
                break;
            }

            case x86_not: {
                if (operands.size()!=1)
                    throw Exception("instruction must have one operand", insn);
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: {
                        Word(8) result = policy.invert(read8(operands[0]));
                        write8(operands[0], result);
                        break;
                    }
                    case 2: {
                        Word(16) result = policy.invert(read16(operands[0]));
                        write16(operands[0], result);
                        break;
                    }
                    case 4: {
                        Word(32) result = policy.invert(read32(operands[0]));
                        write32(operands[0], result);
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                        break;
                }
                break;
            }

            case x86_xadd: {
                if (operands.size()!=2)
                    throw Exception("instruction must have two operands", insn);
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: {
                        Word(8) result = doAddOperation<8>(read8(operands[0]), read8(operands[1]), false, policy.false_());
                        write8(operands[1], read8(operands[0]));
                        write8(operands[0], result);
                        break;
                    }
                    case 2: {
                        Word(16) result = doAddOperation<16>(read16(operands[0]), read16(operands[1]), false, policy.false_());
                        write16(operands[1], read16(operands[0]));
                        write16(operands[0], result);
                        break;
                    }
                    case 4: {
                        Word(32) result = doAddOperation<32>(read32(operands[0]), read32(operands[1]), false, policy.false_());
                        write32(operands[1], read32(operands[0]));
                        write32(operands[0], result);
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                        break;
                }
                break;
            }

            case x86_add: {
                if (operands.size()!=2)
                    throw Exception("instruction must have two operands", insn);
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: {
                        Word(8) result = doAddOperation<8>(read8(operands[0]), read8(operands[1]), false, policy.false_());
                        write8(operands[0], result);
                        break;
                    }
                    case 2: {
                        Word(16) result = doAddOperation<16>(read16(operands[0]), read16(operands[1]), false, policy.false_());
                        write16(operands[0], result);
                        break;
                    }
                    case 4: {
                        Word(32) result = doAddOperation<32>(read32(operands[0]), read32(operands[1]), false, policy.false_());
                        write32(operands[0], result);
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                        break;
                }
                break;
            }

            case x86_adc: {
                if (operands.size()!=2)
                    throw Exception("instruction must have two operands", insn);
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: {
                        Word(8) result = doAddOperation<8>(read8(operands[0]), read8(operands[1]), false,
                                                           readRegister<1>(REG_CF));
                        write8(operands[0], result);
                        break;
                    }
                    case 2: {
                        Word(16) result = doAddOperation<16>(read16(operands[0]), read16(operands[1]), false,
                                                             readRegister<1>(REG_CF));
                        write16(operands[0], result);
                        break;
                    }
                    case 4: {
                        Word(32) result = doAddOperation<32>(read32(operands[0]), read32(operands[1]), false,
                                                             readRegister<1>(REG_CF));
                        write32(operands[0], result);
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                        break;
                }
                break;
            }

            case x86_sub: {
                if (operands.size()!=2)
                    throw Exception("instruction must have two operands", insn);
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: {
                        Word(8) result = doAddOperation<8>(read8(operands[0]), policy.invert(read8(operands[1])), true,
                                                           policy.false_());
                        write8(operands[0], result);
                        break;
                    }
                    case 2: {
                        Word(16) result = doAddOperation<16>(read16(operands[0]), policy.invert(read16(operands[1])), true,
                                                             policy.false_());
                        write16(operands[0], result);
                        break;
                    }
                    case 4: {
                        Word(32) result = doAddOperation<32>(read32(operands[0]), policy.invert(read32(operands[1])), true,
                                                             policy.false_());
                        write32(operands[0], result);
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                        break;
                }
                break;
            }

            case x86_sbb: {
                if (operands.size()!=2)
                    throw Exception("instruction must have two operands", insn);
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: {
                        Word(8) result = doAddOperation<8>(read8(operands[0]), policy.invert(read8(operands[1])), true,
                                                           readRegister<1>(REG_CF));
                        write8(operands[0], result);
                        break;
                    }
                    case 2: {
                        Word(16) result = doAddOperation<16>(read16(operands[0]), policy.invert(read16(operands[1])), true,
                                                             readRegister<1>(REG_CF));
                        write16(operands[0], result);
                        break;
                    }
                    case 4: {
                        Word(32) result = doAddOperation<32>(read32(operands[0]), policy.invert(read32(operands[1])), true,
                                                             readRegister<1>(REG_CF));
                        write32(operands[0], result);
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                        break;
                }
                break;
            }

            case x86_cmp: {
                if (operands.size()!=2)
                    throw Exception("instruction must have two operands", insn);
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: {
                        doAddOperation<8>(read8(operands[0]), policy.invert(read8(operands[1])), true, policy.false_());
                        break;
                    }
                    case 2: {
                        doAddOperation<16>(read16(operands[0]), policy.invert(read16(operands[1])), true, policy.false_());
                        break;
                    }
                    case 4: {
                        doAddOperation<32>(read32(operands[0]), policy.invert(read32(operands[1])), true, policy.false_());
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                        break;
                }
                break;
            }

            case x86_neg: {
                if (operands.size()!=1)
                    throw Exception("instruction must have one operand", insn);
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: {
                        Word(8) result = doAddOperation<8>(number<8>(0), policy.invert(read8(operands[0])), true,
                                                           policy.false_());
                        write8(operands[0], result);
                        break;
                    }
                    case 2: {
                        Word(16) result = doAddOperation<16>(number<16>(0), policy.invert(read16(operands[0])), true,
                                                             policy.false_());
                        write16(operands[0], result);
                        break;
                    }
                    case 4: {
                        Word(32) result = doAddOperation<32>(number<32>(0), policy.invert(read32(operands[0])), true,
                                                             policy.false_());
                        write32(operands[0], result);
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                        break;
                }
                break;
            }

            case x86_inc: {
                if (operands.size()!=1)
                    throw Exception("instruction must have one operand", insn);
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: {
                        Word(8) result = doIncOperation<8>(read8(operands[0]), false, false);
                        write8(operands[0], result);
                        break;
                    }
                    case 2: {
                        Word(16) result = doIncOperation<16>(read16(operands[0]), false, false);
                        write16(operands[0], result);
                        break;
                    }
                    case 4: {
                        Word(32) result = doIncOperation<32>(read32(operands[0]), false, false);
                        write32(operands[0], result);
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                        break;
                }
                break;
            }

            case x86_dec: {
                if (operands.size()!=1)
                    throw Exception("instruction must have one operand", insn);
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: {
                        Word(8) result = doIncOperation<8>(read8(operands[0]), true, false);
                        write8(operands[0], result);
                        break;
                    }
                    case 2: {
                        Word(16) result = doIncOperation<16>(read16(operands[0]), true, false);
                        write16(operands[0], result);
                        break;
                    }
                    case 4: {
                        Word(32) result = doIncOperation<32>(read32(operands[0]), true, false);
                        write32(operands[0], result);
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                        break;
                }
                break;
            }

            case x86_cmpxchg: {
                if (operands.size()!=2)
                    throw Exception("instruction must have two operands", insn);
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: {
                        Word(8) op0 = read8(operands[0]);
                        Word(8) oldAx = readRegister<8>(REG_AL);
                        doAddOperation<8>(oldAx, policy.invert(op0), true, policy.false_());
                        write8(operands[0], policy.ite(readRegister<1>(REG_ZF), read8(operands[1]), op0));
                        writeRegister(REG_AL, policy.ite(readRegister<1>(REG_ZF), oldAx, op0));
                        break;
                    }
                    case 2: {
                        Word(16) op0 = read16(operands[0]);
                        Word(16) oldAx = readRegister<16>(REG_AX);
                        doAddOperation<16>(oldAx, policy.invert(op0), true, policy.false_());
                        write16(operands[0], policy.ite(readRegister<1>(REG_ZF), read16(operands[1]), op0));
                        writeRegister(REG_AX, policy.ite(readRegister<1>(REG_ZF), oldAx, op0));
                        break;
                    }
                    case 4: {
                        Word(32) op0 = read32(operands[0]);
                        Word(32) oldAx = readRegister<32>(REG_EAX);
                        doAddOperation<32>(oldAx, policy.invert(op0), true, policy.false_());
                        write32(operands[0], policy.ite(readRegister<1>(REG_ZF), read32(operands[1]), op0));
                        writeRegister(REG_EAX, policy.ite(readRegister<1>(REG_ZF), oldAx, op0));
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                        break;
                }
                break;
            }

            case x86_shl:       // fall through
            case x86_sar:       // fall through
            case x86_shr: {
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: {
                        WordType<8> output = shift_semantics<8, 5>(kind, read8(operands[0]), undefined_<8>(),
                                                                   read8(operands[1]));
                        write8(operands[0], output);
                        break;
                    }
                    case 2: {
                        WordType<16> output = shift_semantics<16, 5>(kind, read16(operands[0]), undefined_<16>(),
                                                                     read8(operands[1]));
                        write16(operands[0], output);
                        break;
                    }
                    case 4: {
                        WordType<32> output = shift_semantics<32, 5>(kind, read32(operands[0]), undefined_<32>(),
                                                                     read8(operands[1]));
                        write32(operands[0], output);
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                }
                break;
            }

            case x86_rol: {
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: {
                        Word(8) op = read8(operands[0]);
                        Word(5) shiftCount = extract<0, 5>(read8(operands[1]));
                        Word(8) output = policy.rotateLeft(op, shiftCount);
                        writeRegister(REG_CF, policy.ite(policy.equalToZero(shiftCount),
                                                       readRegister<1>(REG_CF),
                                                       extract<0, 1>(output)));
                        writeRegister(REG_OF, policy.ite(policy.equalToZero(shiftCount),
                                                       readRegister<1>(REG_OF),
                                                       policy.xor_(extract<0, 1>(output), extract<7, 8>(output))));
                        write8(operands[0], output);
                        break;
                    }
                    case 2: {
                        Word(16) op = read16(operands[0]);
                        Word(5) shiftCount = extract<0, 5>(read8(operands[1]));
                        Word(16) output = policy.rotateLeft(op, shiftCount);
                        writeRegister(REG_CF, policy.ite(policy.equalToZero(shiftCount),
                                                       readRegister<1>(REG_CF),
                                                       extract<0, 1>(output)));
                        writeRegister(REG_OF, policy.ite(policy.equalToZero(shiftCount),
                                                       readRegister<1>(REG_OF),
                                                       policy.xor_(extract<0, 1>(output), extract<15, 16>(output))));
                        write16(operands[0], output);
                        break;
                    }
                    case 4: {
                        Word(32) op = read32(operands[0]);
                        Word(5) shiftCount = extract<0, 5>(read8(operands[1]));
                        Word(32) output = policy.rotateLeft(op, shiftCount);
                        writeRegister(REG_CF, policy.ite(policy.equalToZero(shiftCount),
                                                       readRegister<1>(REG_CF),
                                                       extract<0, 1>(output)));
                        writeRegister(REG_OF, policy.ite(policy.equalToZero(shiftCount),
                                                       readRegister<1>(REG_OF),
                                                       policy.xor_(extract<0, 1>(output), extract<31, 32>(output))));
                        write32(operands[0], output);
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                }
                break;
            }

            case x86_ror: {
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: {
                        Word(8) op = read8(operands[0]);
                        Word(5) shiftCount = extract<0, 5>(read8(operands[1]));
                        Word(8) output = policy.rotateRight(op, shiftCount);
                        writeRegister(REG_CF, policy.ite(policy.equalToZero(shiftCount),
                                                       readRegister<1>(REG_CF),
                                                       extract<7, 8>(output)));
                        writeRegister(REG_OF, policy.ite(policy.equalToZero(shiftCount),
                                                       readRegister<1>(REG_OF),
                                                       policy.xor_(extract<6, 7>(output), extract<7, 8>(output))));
                        write8(operands[0], output);
                        break;
                    }
                    case 2: {
                        Word(16) op = read16(operands[0]);
                        Word(5) shiftCount = extract<0, 5>(read8(operands[1]));
                        Word(16) output = policy.rotateRight(op, shiftCount);
                        writeRegister(REG_CF, policy.ite(policy.equalToZero(shiftCount),
                                                       readRegister<1>(REG_CF),
                                                       extract<15, 16>(output)));
                        writeRegister(REG_OF, policy.ite(policy.equalToZero(shiftCount),
                                                       readRegister<1>(REG_OF),
                                                       policy.xor_(extract<14, 15>(output), extract<15, 16>(output))));
                        write16(operands[0], output);
                        break;
                    }
                    case 4: {
                        Word(32) op = read32(operands[0]);
                        Word(5) shiftCount = extract<0, 5>(read8(operands[1]));
                        Word(32) output = policy.rotateRight(op, shiftCount);
                        writeRegister(REG_CF, policy.ite(policy.equalToZero(shiftCount),
                                                       readRegister<1>(REG_CF),
                                                       extract<31, 32>(output)));
                        writeRegister(REG_OF, policy.ite(policy.equalToZero(shiftCount),
                                                       readRegister<1>(REG_OF),
                                                       policy.xor_(extract<30, 31>(output), extract<31, 32>(output))));
                        write32(operands[0], output);
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                }
                break;
            }

            case x86_shld: {
                Word(5) shiftCount = extract<0, 5>(read8(operands[2]));
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 2: {
                        Word(16) op1 = read16(operands[0]);
                        Word(16) op2 = read16(operands[1]);
                        Word(16) output1 = policy.shiftLeft(op1, shiftCount);
                        Word(16) output2 = policy.ite(policy.equalToZero(shiftCount),
                                                      number<16>(0),
                                                      policy.shiftRight(op2, policy.negate(shiftCount)));
                        Word(16) output = policy.or_(output1, output2);
                        Word(1) newCf = policy.ite(policy.equalToZero(shiftCount),
                                                   readRegister<1>(REG_CF),
                                                   extract<15, 16>(policy.shiftLeft(op1, policy.add(shiftCount, number<5>(15)))));
                        writeRegister(REG_CF, newCf);
                        Word(1) newOf = policy.ite(policy.equalToZero(shiftCount),
                                                   readRegister<1>(REG_OF), 
                                                   policy.xor_(extract<15, 16>(output), newCf));
                        writeRegister(REG_OF, newOf);
                        write16(operands[0], output);
                        setFlagsForResult<16>(output);
                        writeRegister(REG_AF, policy.ite(policy.equalToZero(shiftCount),
                                                       readRegister<1>(REG_AF),
                                                       undefined_<1>()));
                        break;
                    }
                    case 4: {
                        Word(32) op1 = read32(operands[0]);
                        Word(32) op2 = read32(operands[1]);
                        Word(5) shiftCount = extract<0, 5>(read8(operands[2]));
                        Word(32) output1 = policy.shiftLeft(op1, shiftCount);
                        Word(32) output2 = policy.ite(policy.equalToZero(shiftCount),
                                                      number<32>(0),
                                                      policy.shiftRight(op2, policy.negate(shiftCount)));
                        Word(32) output = policy.or_(output1, output2);
                        Word(1) newCf = policy.ite(policy.equalToZero(shiftCount),
                                                   readRegister<1>(REG_CF),
                                                   extract<31, 32>(policy.shiftLeft(op1, policy.add(shiftCount, number<5>(31)))));
                        writeRegister(REG_CF, newCf);
                        Word(1) newOf = policy.ite(policy.equalToZero(shiftCount),
                                                   readRegister<1>(REG_OF), 
                                                   policy.xor_(extract<31, 32>(output), newCf));
                        writeRegister(REG_OF, newOf);
                        write32(operands[0], output);
                        setFlagsForResult<32>(output);
                        writeRegister(REG_AF, policy.ite(policy.equalToZero(shiftCount),
                                                       readRegister<1>(REG_AF),
                                                       undefined_<1>()));
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                }
                break;
            }

            case x86_shrd: {
                Word(5) shiftCount = extract<0, 5>(read8(operands[2]));
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 2: {
                        WordType<16> output = shift_semantics<16, 5>(kind, read16(operands[0]), read16(operands[1]),
                                                                    read8(operands[2]));
                        write16(operands[0], output);
                        break;
                    }
                    case 4: {
                        Word(32) op1 = read32(operands[0]);
                        Word(32) op2 = read32(operands[1]);
                        Word(32) output1 = policy.shiftRight(op1, shiftCount);
                        Word(32) output2 = policy.ite(policy.equalToZero(shiftCount),
                                                      number<32>(0),
                                                      policy.shiftLeft(op2, policy.negate(shiftCount)));
                        Word(32) output = policy.or_(output1, output2);
                        Word(1) newCf = policy.ite(policy.equalToZero(shiftCount),
                                                   readRegister<1>(REG_CF),
                                                   extract<0, 1>(policy.shiftRight(op1, policy.add(shiftCount, number<5>(31)))));
                        writeRegister(REG_CF, newCf);
                        Word(1) newOf = policy.ite(policy.equalToZero(shiftCount),
                                                   readRegister<1>(REG_OF), 
                                                   policy.xor_(extract<31, 32>(output),
                                                               extract<31, 32>(op1)));
                        writeRegister(REG_OF, newOf);
                        write32(operands[0], output);
                        setFlagsForResult<32>(output);
                        writeRegister(REG_AF, policy.ite(policy.equalToZero(shiftCount),
                                                       readRegister<1>(REG_AF),
                                                       undefined_<1>()));
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                }
                break;
            }

            case x86_bsf: {
                writeRegister(REG_OF, undefined_<1>());
                writeRegister(REG_SF, undefined_<1>());
                writeRegister(REG_AF, undefined_<1>());
                writeRegister(REG_PF, undefined_<1>());
                writeRegister(REG_CF, undefined_<1>());
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 2: {
                        Word(16) op = read16(operands[1]);
                        writeRegister(REG_ZF, policy.equalToZero(op));
                        Word(16) result = policy.ite(readRegister<1>(REG_ZF),
                                                     read16(operands[0]),
                                                     policy.leastSignificantSetBit(op));
                        write16(operands[0], result);
                        break;
                    }
                    case 4: {
                        Word(32) op = read32(operands[1]);
                        writeRegister(REG_ZF, policy.equalToZero(op));
                        Word(32) result = policy.ite(readRegister<1>(REG_ZF),
                                                     read32(operands[0]),
                                                     policy.leastSignificantSetBit(op));
                        write32(operands[0], result);
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                }
                break;
            }

            case x86_bsr: {
                writeRegister(REG_OF, undefined_<1>());
                writeRegister(REG_SF, undefined_<1>());
                writeRegister(REG_AF, undefined_<1>());
                writeRegister(REG_PF, undefined_<1>());
                writeRegister(REG_CF, undefined_<1>());
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 2: {
                        Word(16) op = read16(operands[1]);
                        writeRegister(REG_ZF, policy.equalToZero(op));
                        Word(16) result = policy.ite(readRegister<1>(REG_ZF),
                                                     read16(operands[0]),
                                                     policy.mostSignificantSetBit(op));
                        write16(operands[0], result);
                        break;
                    }
                    case 4: {
                        Word(32) op = read32(operands[1]);
                        writeRegister(REG_ZF, policy.equalToZero(op));
                        Word(32) result = policy.ite(readRegister<1>(REG_ZF),
                                                     read32(operands[0]),
                                                     policy.mostSignificantSetBit(op));
                        write32(operands[0], result);
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                }
                break;
            }

            case x86_bt: {              /* Bit test */
                if (operands.size()!=2)
                    throw Exception("instruction must have two operands", insn);
                
                /* All flags except CF are undefined */
                writeRegister(REG_OF, undefined_<1>());
                writeRegister(REG_SF, undefined_<1>());
                writeRegister(REG_ZF, undefined_<1>());
                writeRegister(REG_AF, undefined_<1>());
                writeRegister(REG_PF, undefined_<1>());
                
                if (isSgAsmMemoryReferenceExpression(operands[0]) && isSgAsmx86RegisterReferenceExpression(operands[1])) {
                    /* Special case allowing multi-word offsets into memory */
                    Word(32) addr = readEffectiveAddress(operands[0]);
                    int numBytes = numBytesInAsmType(operands[1]->get_type());
                    Word(32) bitnum = numBytes == 2 ? signExtend<16, 32>(read16(operands[1])) : read32(operands[1]);
                    Word(32) adjustedAddr = policy.add(addr, signExtend<29, 32>(extract<3, 32>(bitnum)));
                    Word(8) val = readMemory<8>(getSegregFromMemoryReference(isSgAsmMemoryReferenceExpression(operands[0])),
                                                adjustedAddr, policy.true_());
                    Word(1) bitval = extract<0, 1>(policy.rotateRight(val, extract<0, 3>(bitnum)));
                    writeRegister(REG_CF, bitval);
                } else {
                    /* Simple case */
                    switch (numBytesInAsmType(operands[0]->get_type())) {
                        case 2: {
                            Word(16) op0 = read16(operands[0]);
                            Word(4) bitnum = extract<0, 4>(read16(operands[1]));
                            Word(1) bitval = extract<0, 1>(policy.rotateRight(op0, bitnum));
                            writeRegister(REG_CF, bitval);
                            break;
                        }
                        case 4: {
                            Word(32) op0 = read32(operands[0]);
                            Word(5) bitnum = extract<0, 5>(read32(operands[1]));
                            Word(1) bitval = extract<0, 1>(policy.rotateRight(op0, bitnum));
                            writeRegister(REG_CF, bitval);
                            break;
                        }
                        default:
                            throw Exception("size not implemented", insn);
                    }
                }
                break;
            }
                
            case x86_btr: {             /* Bit test and reset */
                if (operands.size()!=2)
                    throw Exception("instruction must have two operands", insn);
                
                /* All flags except CF are undefined */
                writeRegister(REG_OF, undefined_<1>());
                writeRegister(REG_SF, undefined_<1>());
                writeRegister(REG_ZF, undefined_<1>());
                writeRegister(REG_AF, undefined_<1>());
                writeRegister(REG_PF, undefined_<1>());
                
                if (isSgAsmMemoryReferenceExpression(operands[0]) && isSgAsmx86RegisterReferenceExpression(operands[1])) {
                    /* Special case allowing multi-word offsets into memory */
                    Word(32) addr = readEffectiveAddress(operands[0]);
                    int numBytes = numBytesInAsmType(operands[1]->get_type());
                    Word(32) bitnum = numBytes == 2 ? signExtend<16, 32>(read16(operands[1])) : read32(operands[1]);
                    Word(32) adjustedAddr = policy.add(addr, signExtend<29, 32>(extract<3, 32>(bitnum)));
                    Word(8) val = readMemory<8>(getSegregFromMemoryReference(isSgAsmMemoryReferenceExpression(operands[0])),
                                                adjustedAddr, policy.true_());
                    Word(1) bitval = extract<0, 1>(policy.rotateRight(val, extract<0, 3>(bitnum)));
                    Word(8) result = policy.and_(val,
                                                 policy.invert(policy.rotateLeft(number<8>(1),
                                                                                 extract<0, 3>(bitnum))));
                    writeRegister(REG_CF, bitval);
                    policy.writeMemory(getSegregFromMemoryReference(isSgAsmMemoryReferenceExpression(operands[0])),
                                       adjustedAddr, result, policy.true_());
                } else {
                    /* Simple case */
                    switch (numBytesInAsmType(operands[0]->get_type())) {
                        case 2: {
                            Word(16) op0 = read16(operands[0]);
                            Word(4) bitnum = extract<0, 4>(read16(operands[1]));
                            Word(1) bitval = extract<0, 1>(policy.rotateRight(op0, bitnum));
                            Word(16) result = policy.and_(op0, policy.invert(policy.rotateLeft(number<16>(1), bitnum)));
                            writeRegister(REG_CF, bitval);
                            write16(operands[0], result);
                            break;
                        }
                        case 4: {
                            Word(32) op0 = read32(operands[0]);
                            Word(5) bitnum = extract<0, 5>(read32(operands[1]));
                            Word(1) bitval = extract<0, 1>(policy.rotateRight(op0, bitnum));
                            Word(32) result = policy.and_(op0, policy.invert(policy.rotateLeft(number<32>(1), bitnum)));
                            writeRegister(REG_CF, bitval);
                            write32(operands[0], result);
                            break;
                        }
                        default:
                            throw Exception("size not implemented", insn);
                    }
                }
                break;
            }

            case x86_bts: {             /* bit test and set */
                if (operands.size()!=2)
                    throw Exception("instruction must have two operands", insn);
                
                /* All flags except CF are undefined */
                writeRegister(REG_OF, undefined_<1>());
                writeRegister(REG_SF, undefined_<1>());
                writeRegister(REG_ZF, undefined_<1>());
                writeRegister(REG_AF, undefined_<1>());
                writeRegister(REG_PF, undefined_<1>());
                
                if (isSgAsmMemoryReferenceExpression(operands[0]) && isSgAsmx86RegisterReferenceExpression(operands[1])) {
                    /* Special case allowing multi-word offsets into memory */
                    Word(32) addr = readEffectiveAddress(operands[0]);
                    int numBytes = numBytesInAsmType(operands[1]->get_type());
                    Word(32) bitnum = numBytes == 2 ? signExtend<16, 32>(read16(operands[1])) : read32(operands[1]);
                    Word(32) adjustedAddr = policy.add(addr, signExtend<29, 32>(extract<3, 32>(bitnum)));
                    Word(8) val = readMemory<8>(getSegregFromMemoryReference(isSgAsmMemoryReferenceExpression(operands[0])),
                                                adjustedAddr, policy.true_());
                    Word(1) bitval = extract<0, 1>(policy.rotateRight(val, extract<0, 3>(bitnum)));
                    Word(8) result = policy.or_(val, policy.rotateLeft(number<8>(1), extract<0, 3>(bitnum)));
                    writeRegister(REG_CF, bitval);
                    policy.writeMemory(getSegregFromMemoryReference(isSgAsmMemoryReferenceExpression(operands[0])),
                                       adjustedAddr, result, policy.true_());
                } else {
                    /* Simple case */
                    switch (numBytesInAsmType(operands[0]->get_type())) {
                        case 2: {
                            Word(16) op0 = read16(operands[0]);
                            Word(4) bitnum = extract<0, 4>(read16(operands[1]));
                            Word(1) bitval = extract<0, 1>(policy.rotateRight(op0, bitnum));
                            Word(16) result = policy.or_(op0, policy.rotateLeft(number<16>(1), bitnum));
                            writeRegister(REG_CF, bitval);
                            write16(operands[0], result);
                            break;
                        }
                        case 4: {
                            Word(32) op0 = read32(operands[0]);
                            Word(5) bitnum = extract<0, 5>(read32(operands[1]));
                            Word(1) bitval = extract<0, 1>(policy.rotateRight(op0, bitnum));
                            Word(32) result = policy.or_(op0, policy.rotateLeft(number<32>(1), bitnum));
                            writeRegister(REG_CF, bitval);
                            write32(operands[0], result);
                            break;
                        }
                        default:
                            throw Exception("size not implemented", insn);
                    }
                }
                break;
            }

            case x86_imul: {
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: {
                        Word(8) op0 = readRegister<8>(REG_AL);
                        Word(8) op1 = read8(operands[0]);
                        Word(16) mulResult = policy.signedMultiply(op0, op1);
                        writeRegister(REG_AX, mulResult);
                        Word(1) carry = policy.invert(policy.or_(policy.equalToZero(policy.invert(extract<7, 16>(mulResult))),
                                                                 policy.equalToZero(extract<7, 16>(mulResult))));
                        writeRegister(REG_CF, carry);
                        writeRegister(REG_OF, carry);
                        break;
                    }
                    case 2: {
                        Word(16) op0 = operands.size() == 1 ?
                                       readRegister<16>(REG_AX) :
                                       read16(operands[operands.size() - 2]);
                        Word(16) op1 = read16(operands[operands.size() - 1]);
                        Word(32) mulResult = policy.signedMultiply(op0, op1);
                        if (operands.size() == 1) {
                            writeRegister(REG_AX, extract<0, 16>(mulResult));
                            writeRegister(REG_DX, extract<16, 32>(mulResult));
                        } else {
                            write16(operands[0], extract<0, 16>(mulResult));
                        }
                        Word(1) carry = policy.invert(policy.or_(policy.equalToZero(policy.invert(extract<7, 32>(mulResult))),
                                                                 policy.equalToZero(extract<7, 32>(mulResult))));
                        writeRegister(REG_CF, carry);
                        writeRegister(REG_OF, carry);
                        break;
                    }
                    case 4: {
                        Word(32) op0 = operands.size() == 1 ?
                                       readRegister<32>(REG_EAX) :
                                       read32(operands[operands.size() - 2]);
                        Word(32) op1 = read32(operands[operands.size() - 1]);
                        Word(64) mulResult = policy.signedMultiply(op0, op1);
                        if (operands.size() == 1) {
                            writeRegister(REG_EAX, extract<0, 32>(mulResult));
                            writeRegister(REG_EDX, extract<32, 64>(mulResult));
                        } else {
                            write32(operands[0], extract<0, 32>(mulResult));
                        }
                        Word(1) carry = policy.invert(policy.or_(policy.equalToZero(policy.invert(extract<7, 64>(mulResult))),
                                                                 policy.equalToZero(extract<7, 64>(mulResult))));
                        writeRegister(REG_CF, carry);
                        writeRegister(REG_OF, carry);
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                }
                writeRegister(REG_SF, undefined_<1>());
                writeRegister(REG_ZF, undefined_<1>());
                writeRegister(REG_AF, undefined_<1>());
                writeRegister(REG_PF, undefined_<1>());
                break;
            }

            case x86_mul: {
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: {
                        Word(8) op0 = readRegister<8>(REG_AL);
                        Word(8) op1 = read8(operands[0]);
                        Word(16) mulResult = policy.unsignedMultiply(op0, op1);
                        writeRegister(REG_AX, mulResult);
                        Word(1) carry = policy.invert(policy.equalToZero(extract<8, 16>(mulResult)));
                        writeRegister(REG_CF, carry);
                        writeRegister(REG_OF, carry);
                        break;
                    }
                    case 2: {
                        Word(16) op0 = readRegister<16>(REG_AX);
                        Word(16) op1 = read16(operands[0]);
                        Word(32) mulResult = policy.unsignedMultiply(op0, op1);
                        writeRegister(REG_AX, extract<0, 16>(mulResult));
                        writeRegister(REG_DX, extract<16, 32>(mulResult));
                        Word(1) carry = policy.invert(policy.equalToZero(extract<16, 32>(mulResult)));
                        writeRegister(REG_CF, carry);
                        writeRegister(REG_OF, carry);
                        break;
                    }
                    case 4: {
                        Word(32) op0 = readRegister<32>(REG_EAX);
                        Word(32) op1 = read32(operands[0]);
                        Word(64) mulResult = policy.unsignedMultiply(op0, op1);
                        writeRegister(REG_EAX, extract<0, 32>(mulResult));
                        writeRegister(REG_EDX, extract<32, 64>(mulResult));
                        Word(1) carry = policy.invert(policy.equalToZero(extract<32, 64>(mulResult)));
                        writeRegister(REG_CF, carry);
                        writeRegister(REG_OF, carry);
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                }
                writeRegister(REG_SF, undefined_<1>());
                writeRegister(REG_ZF, undefined_<1>());
                writeRegister(REG_AF, undefined_<1>());
                writeRegister(REG_PF, undefined_<1>());
                break;
            }

            case x86_idiv: {
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: {
                        Word(16) op0 = readRegister<16>(REG_AX);
                        Word(8) op1 = read8(operands[0]);
                        /* if op1 == 0, we should trap */
                        Word(16) divResult = policy.signedDivide(op0, op1);
                        Word(8) modResult = policy.signedModulo(op0, op1);
                        /* if result overflows, we should trap */
                        writeRegister(REG_AX, policy.concat(extract<0, 8>(divResult), modResult));
                        break;
                    }
                    case 2: {
                        Word(32) op0 = policy.concat(readRegister<16>(REG_AX), readRegister<16>(REG_DX));
                        Word(16) op1 = read16(operands[0]);
                        /* if op1 == 0, we should trap */
                        Word(32) divResult = policy.signedDivide(op0, op1);
                        Word(16) modResult = policy.signedModulo(op0, op1);
                        /* if result overflows, we should trap */
                        writeRegister(REG_AX, extract<0, 16>(divResult));
                        writeRegister(REG_DX, modResult);
                        break;
                    }
                    case 4: {
                        Word(64) op0 = policy.concat(readRegister<32>(REG_EAX), readRegister<32>(REG_EDX));
                        Word(32) op1 = read32(operands[0]);
                        /* if op1 == 0, we should trap */
                        Word(64) divResult = policy.signedDivide(op0, op1);
                        Word(32) modResult = policy.signedModulo(op0, op1);
                        /* if result overflows, we should trap */
                        writeRegister(REG_EAX, extract<0, 32>(divResult));
                        writeRegister(REG_EDX, modResult);
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                }
                writeRegister(REG_SF, undefined_<1>());
                writeRegister(REG_ZF, undefined_<1>());
                writeRegister(REG_AF, undefined_<1>());
                writeRegister(REG_PF, undefined_<1>());
                writeRegister(REG_CF, undefined_<1>());
                writeRegister(REG_OF, undefined_<1>());
                break;
            }

            case x86_div: {
                switch (numBytesInAsmType(operands[0]->get_type())) {
                    case 1: {
                        Word(16) op0 = readRegister<16>(REG_AX);
                        Word(8) op1 = read8(operands[0]);
                        /* if op1 == 0, we should trap */
                        Word(16) divResult = policy.unsignedDivide(op0, op1);
                        Word(8) modResult = policy.unsignedModulo(op0, op1);
                        /* if extract<8, 16> of divResult is non-zero (overflow), we should trap */
                        writeRegister(REG_AX, policy.concat(extract<0, 8>(divResult), modResult));
                        break;
                    }
                    case 2: {
                        Word(32) op0 = policy.concat(readRegister<16>(REG_AX), readRegister<16>(REG_DX));
                        Word(16) op1 = read16(operands[0]);
                        /* if op1 == 0, we should trap */
                        Word(32) divResult = policy.unsignedDivide(op0, op1);
                        Word(16) modResult = policy.unsignedModulo(op0, op1);
                        /* if extract<16, 32> of divResult is non-zero (overflow), we should trap */
                        writeRegister(REG_AX, extract<0, 16>(divResult));
                        writeRegister(REG_DX, modResult);
                        break;
                    }
                    case 4: {
                        Word(64) op0 = policy.concat(readRegister<32>(REG_EAX), readRegister<32>(REG_EDX));
                        Word(32) op1 = read32(operands[0]);
                        /* if op1 == 0, we should trap */
                        Word(64) divResult = policy.unsignedDivide(op0, op1);
                        Word(32) modResult = policy.unsignedModulo(op0, op1);
                        /* if extract<32, 64> of divResult is non-zero (overflow), we should trap */
                        writeRegister(REG_EAX, extract<0, 32>(divResult));
                        writeRegister(REG_EDX, modResult);
                        break;
                    }
                    default:
                        throw Exception("size not implemented", insn);
                }
                writeRegister(REG_SF, undefined_<1>());
                writeRegister(REG_ZF, undefined_<1>());
                writeRegister(REG_AF, undefined_<1>());
                writeRegister(REG_PF, undefined_<1>());
                writeRegister(REG_CF, undefined_<1>());
                writeRegister(REG_OF, undefined_<1>());
                break;
            }

            case x86_aaa: {
                if (operands.size()!=0)
                    throw Exception("instruction must have no operands", insn);
                Word(1) incAh = policy.or_(readRegister<1>(REG_AF),
                                           greaterOrEqualToTen(extract<0, 4>(readRegister<8>(REG_AL))));
                writeRegister(REG_AX, 
                              policy.concat(policy.add(policy.ite(incAh, number<4>(6), number<4>(0)),
                                                       extract<0, 4>(readRegister<8>(REG_AL))),
                                            policy.concat(number<4>(0),
                                                          policy.add(policy.ite(incAh, number<8>(1), number<8>(0)),
                                                                     readRegister<8>(REG_AH)))));
                writeRegister(REG_OF, undefined_<1>());
                writeRegister(REG_SF, undefined_<1>());
                writeRegister(REG_ZF, undefined_<1>());
                writeRegister(REG_PF, undefined_<1>());
                writeRegister(REG_AF, incAh);
                writeRegister(REG_CF, incAh);
                break;
            }

            case x86_aas: {
                if (operands.size()!=0)
                    throw Exception("instruction must have no operands", insn);
                Word(1) decAh = policy.or_(readRegister<1>(REG_AF),
                                           greaterOrEqualToTen(extract<0, 4>(readRegister<8>(REG_AL))));
                writeRegister(REG_AX, 
                              policy.concat(policy.add(policy.ite(decAh, number<4>(-6), number<4>(0)),
                                                       extract<0, 4>(readRegister<8>(REG_AL))),
                                            policy.concat(number<4>(0),
                                                          policy.add(policy.ite(decAh, number<8>(-1), number<8>(0)),
                                                                     readRegister<8>(REG_AH)))));
                writeRegister(REG_OF, undefined_<1>());
                writeRegister(REG_SF, undefined_<1>());
                writeRegister(REG_ZF, undefined_<1>());
                writeRegister(REG_PF, undefined_<1>());
                writeRegister(REG_AF, decAh);
                writeRegister(REG_CF, decAh);
                break;
            }

            case x86_aam: {
                if (operands.size()!=1)
                    throw Exception("instruction must have one operand", insn);
                Word(8) al = readRegister<8>(REG_AL);
                Word(8) divisor = read8(operands[0]);
                Word(8) newAh = policy.unsignedDivide(al, divisor);
                Word(8) newAl = policy.unsignedModulo(al, divisor);
                writeRegister(REG_AX, policy.concat(newAl, newAh));
                writeRegister(REG_OF, undefined_<1>());
                writeRegister(REG_AF, undefined_<1>());
                writeRegister(REG_CF, undefined_<1>());
                setFlagsForResult<8>(newAl);
                break;
            }

            case x86_aad: {
                if (operands.size()!=1)
                    throw Exception("instruction must have one operand", insn);
                Word(8) al = readRegister<8>(REG_AL);
                Word(8) ah = readRegister<8>(REG_AH);
                Word(8) divisor = read8(operands[0]);
                Word(8) newAl = policy.add(al, extract<0, 8>(policy.unsignedMultiply(ah, divisor)));
                writeRegister(REG_AX, policy.concat(newAl, number<8>(0)));
                writeRegister(REG_OF, undefined_<1>());
                writeRegister(REG_AF, undefined_<1>());
                writeRegister(REG_CF, undefined_<1>());
                setFlagsForResult<8>(newAl);
                break;
            }

            case x86_bswap: {
                if (operands.size()!=1)
                    throw Exception("instruction must have one operand", insn);
                Word(32) oldVal = read32(operands[0]);
                Word(32) newVal = policy.concat(extract<24, 32>(oldVal),
                                                policy.concat(extract<16, 24>(oldVal),
                                                              policy.concat(extract<8, 16>(oldVal),
                                                                            extract<0, 8>(oldVal))));
                write32(operands[0], newVal);
                break;
            }

            case x86_push: {
                if (operands.size()!=1)
                    throw Exception("instruction must have one operand", insn);
                if (insn->get_addressSize() != x86_insnsize_32 || insn->get_operandSize() != x86_insnsize_32)
                        throw Exception("size not implemented", insn);
                Word(32) oldSp = readRegister<32>(REG_ESP);
                Word(32) newSp = policy.add(oldSp, number<32>(-4));
                policy.writeMemory(x86_segreg_ss, newSp, read32(operands[0]), policy.true_());
                writeRegister(REG_ESP, newSp);
                break;
            }

            case x86_pushad: {
                if (operands.size()!=0)
                    throw Exception("instruction must have no operands", insn);
                if (insn->get_addressSize() != x86_insnsize_32)
                    throw Exception("size not implemented", insn);
                Word(32) oldSp = readRegister<32>(REG_ESP);
                Word(32) newSp = policy.add(oldSp, number<32>(-32));
                policy.writeMemory(x86_segreg_ss, newSp,
                                   readRegister<32>(REG_EDI), policy.true_());
                policy.writeMemory(x86_segreg_ss, policy.add(newSp, number<32>(4)),
                                   readRegister<32>(REG_ESI), policy.true_());
                policy.writeMemory(x86_segreg_ss, policy.add(newSp, number<32>(8)),
                                   readRegister<32>(REG_EBP), policy.true_());
                policy.writeMemory(x86_segreg_ss, policy.add(newSp, number<32>(12)),
                                   oldSp, policy.true_());
                policy.writeMemory(x86_segreg_ss, policy.add(newSp, number<32>(16)),
                                   readRegister<32>(REG_EBX), policy.true_());
                policy.writeMemory(x86_segreg_ss, policy.add(newSp, number<32>(20)),
                                   readRegister<32>(REG_EDX), policy.true_());
                policy.writeMemory(x86_segreg_ss, policy.add(newSp, number<32>(24)),
                                   readRegister<32>(REG_ECX), policy.true_());
                policy.writeMemory(x86_segreg_ss, policy.add(newSp, number<32>(28)),
                                   readRegister<32>(REG_EAX), policy.true_());
                writeRegister(REG_ESP, newSp);
                break;
            }

            case x86_pushfd: {
                if (operands.size()!=0)
                    throw Exception("instruction must have no operands", insn);
                if (insn->get_addressSize() != x86_insnsize_32)
                    throw Exception("size not implemented", insn);
                Word(32) oldSp = readRegister<32>(REG_ESP);
                Word(32) newSp = policy.add(oldSp, number<32>(-4));
                policy.writeMemory(x86_segreg_ss, newSp, readRegister<32>(REG_EFLAGS), policy.true_());
                writeRegister(REG_ESP, newSp);
                break;
            }

            case x86_pop: {
                if (operands.size()!=1)
                    throw Exception("instruction must have one operand", insn);
                if (insn->get_addressSize() != x86_insnsize_32 || insn->get_operandSize() != x86_insnsize_32)
                    throw Exception("size not implemented", insn);
                Word(32) oldSp = readRegister<32>(REG_ESP);
                Word(32) newSp = policy.add(oldSp, number<32>(4));
                writeRegister(REG_ESP, newSp);
                write32(operands[0], readMemory<32>(x86_segreg_ss, oldSp, policy.true_()));
                break;
            }

            case x86_popad: {
                if (operands.size()!=0)
                    throw Exception("instruction must have no operands", insn);
                if (insn->get_addressSize() != x86_insnsize_32)
                    throw Exception("size not implemented", insn);
                Word(32) oldSp = readRegister<32>(REG_ESP);
                Word(32) newSp = policy.add(oldSp, number<32>(32));
                writeRegister(REG_EDI, readMemory<32>(x86_segreg_ss, oldSp, policy.true_()));
                writeRegister(REG_ESI, readMemory<32>(x86_segreg_ss, policy.add(oldSp, number<32>(4)), policy.true_()));
                writeRegister(REG_EBP, readMemory<32>(x86_segreg_ss, policy.add(oldSp, number<32>(8)), policy.true_()));
                writeRegister(REG_EBX, readMemory<32>(x86_segreg_ss, policy.add(oldSp, number<32>(16)), policy.true_()));
                writeRegister(REG_EDX, readMemory<32>(x86_segreg_ss, policy.add(oldSp, number<32>(20)), policy.true_()));
                writeRegister(REG_ECX, readMemory<32>(x86_segreg_ss, policy.add(oldSp, number<32>(24)), policy.true_()));
                writeRegister(REG_EAX, readMemory<32>(x86_segreg_ss, policy.add(oldSp, number<32>(28)), policy.true_()));
                readMemory<32>(x86_segreg_ss, policy.add(oldSp, number<32>(12)), policy.true_());
                writeRegister(REG_ESP, newSp);
                break;
            }

            case x86_leave: {
                if (operands.size()!=0)
                    throw Exception("instruction must have no operands", insn);
                writeRegister(REG_ESP, readRegister<32>(REG_EBP));
                if (insn->get_addressSize() != x86_insnsize_32 || insn->get_operandSize() != x86_insnsize_32)
                    throw Exception("size not implemented", insn);
                Word(32) oldSp = readRegister<32>(REG_ESP);
                Word(32) newSp = policy.add(oldSp, number<32>(4));
                writeRegister(REG_EBP, readMemory<32>(x86_segreg_ss, oldSp, policy.true_()));
                writeRegister(REG_ESP, newSp);
                break;
            }

            case x86_call: {
                if (operands.size()!=1)
                    throw Exception("instruction must have one operand", insn);
                if (insn->get_addressSize() != x86_insnsize_32 || insn->get_operandSize() != x86_insnsize_32)
                    throw Exception("size not implemented", insn);
                Word(32) oldSp = readRegister<32>(REG_ESP);
                Word(32) newSp = policy.add(oldSp, number<32>(-4));
                policy.writeMemory(x86_segreg_ss, newSp, readRegister<32>(REG_EIP), policy.true_());
                writeRegister(REG_EIP, policy.filterCallTarget(read32(operands[0])));
                writeRegister(REG_ESP, newSp);
                break;
            }

            case x86_ret: {
                if (operands.size()>1)
                    throw Exception("instruction must have zero or one operand", insn);
                if (insn->get_addressSize() != x86_insnsize_32 || insn->get_operandSize() != x86_insnsize_32)
                    throw Exception("size not implemented", insn);
                Word(32) extraBytes = (operands.size() == 1 ? read32(operands[0]) : number<32>(0));
                Word(32) oldSp = readRegister<32>(REG_ESP);
                Word(32) newSp = policy.add(oldSp, policy.add(number<32>(4), extraBytes));
                writeRegister(REG_EIP, policy.filterReturnTarget(readMemory<32>(x86_segreg_ss, oldSp, policy.true_())));
                writeRegister(REG_ESP, newSp);
                break;
            }

            case x86_loop: {
                if (operands.size()!=1)
                    throw Exception("instruction must have one operand", insn);
                if (insn->get_addressSize() != x86_insnsize_32 || insn->get_operandSize() != x86_insnsize_32)
                    throw Exception("size not implemented", insn);
                Word(32) oldCx = readRegister<32>(REG_ECX);
                Word(32) newCx = policy.add(number<32>(-1), oldCx);
                writeRegister(REG_ECX, newCx);
                Word(1) doLoop = policy.invert(policy.equalToZero(newCx));
                writeRegister(REG_EIP, policy.ite(doLoop, read32(operands[0]), readRegister<32>(REG_EIP)));
                break;
            }
            case x86_loopz: {
                if (operands.size()!=1)
                    throw Exception("instruction must have one operand", insn);
                if (insn->get_addressSize() != x86_insnsize_32 || insn->get_operandSize() != x86_insnsize_32)
                    throw Exception("size not implemented", insn);
                Word(32) oldCx = readRegister<32>(REG_ECX);
                Word(32) newCx = policy.add(number<32>(-1), oldCx);
                writeRegister(REG_ECX, newCx);
                Word(1) doLoop = policy.and_(policy.invert(policy.equalToZero(newCx)), readRegister<1>(REG_ZF));
                writeRegister(REG_EIP, policy.ite(doLoop, read32(operands[0]), readRegister<32>(REG_EIP)));
                break;
            }
            case x86_loopnz: {
                if (operands.size()!=1)
                    throw Exception("instruction must have one operand", insn);
                if (insn->get_addressSize() != x86_insnsize_32 || insn->get_operandSize() != x86_insnsize_32)
                    throw Exception("size not implemented", insn);
                Word(32) oldCx = readRegister<32>(REG_ECX);
                Word(32) newCx = policy.add(number<32>(-1), oldCx);
                writeRegister(REG_ECX, newCx);
                Word(1) doLoop = policy.and_(policy.invert(policy.equalToZero(newCx)),
                                             policy.invert(readRegister<1>(REG_ZF)));
                writeRegister(REG_EIP, policy.ite(doLoop, read32(operands[0]), readRegister<32>(REG_EIP)));
                break;
            }

            case x86_jmp: {
                if (operands.size()!=1)
                    throw Exception("instruction must have one operand", insn);
                writeRegister(REG_EIP, policy.filterIndirectJumpTarget(read32(operands[0])));
                break;
            }


            /* Flag expressions that must be true for a conditional jump to occur. */
#           define FLAGCOMBO_ne    policy.invert(readRegister<1>(REG_ZF))
#           define FLAGCOMBO_e     readRegister<1>(REG_ZF)
#           define FLAGCOMBO_no    policy.invert(readRegister<1>(REG_OF))
#           define FLAGCOMBO_o     readRegister<1>(REG_OF)
#           define FLAGCOMBO_ns    policy.invert(readRegister<1>(REG_SF))
#           define FLAGCOMBO_s     readRegister<1>(REG_SF)
#           define FLAGCOMBO_po    policy.invert(readRegister<1>(REG_PF))
#           define FLAGCOMBO_pe    readRegister<1>(REG_PF)
#           define FLAGCOMBO_ae    policy.invert(readRegister<1>(REG_CF))
#           define FLAGCOMBO_b     readRegister<1>(REG_CF)
#           define FLAGCOMBO_be    policy.or_(FLAGCOMBO_b, FLAGCOMBO_e)
#           define FLAGCOMBO_a     policy.and_(FLAGCOMBO_ae, FLAGCOMBO_ne)
#           define FLAGCOMBO_l     policy.xor_(readRegister<1>(REG_SF), readRegister<1>(REG_OF))
#           define FLAGCOMBO_ge    policy.invert(policy.xor_(readRegister<1>(REG_SF), readRegister<1>(REG_OF)))
#           define FLAGCOMBO_le    policy.or_(FLAGCOMBO_e, FLAGCOMBO_l)
#           define FLAGCOMBO_g     policy.and_(FLAGCOMBO_ge, FLAGCOMBO_ne)
#           define FLAGCOMBO_cxz   policy.equalToZero(readRegister<16>(REG_CX))
#           define FLAGCOMBO_ecxz  policy.equalToZero(readRegister<32>(REG_ECX))

#           define JUMP(tag) {                                                                                                 \
                if (operands.size()!=1)                                                                                        \
                    throw Exception("instruction must have one operand", insn);                                                \
                writeRegister(REG_EIP, policy.ite(FLAGCOMBO_##tag,                                                        \
                                                read32(operands[0]),                                                           \
                                                readRegister<32>(REG_EIP)));                                                     \
            }
            case x86_jne:   JUMP(ne);   break;
            case x86_je:    JUMP(e);    break;
            case x86_jno:   JUMP(no);   break;
            case x86_jo:    JUMP(o);    break;
            case x86_jpo:   JUMP(po);   break;
            case x86_jpe:   JUMP(pe);   break;
            case x86_jns:   JUMP(ns);   break;
            case x86_js:    JUMP(s);    break;
            case x86_jae:   JUMP(ae);   break;
            case x86_jb:    JUMP(b);    break;
            case x86_jbe:   JUMP(be);   break;
            case x86_ja:    JUMP(a);    break;
            case x86_jle:   JUMP(le);   break;
            case x86_jg:    JUMP(g);    break;
            case x86_jge:   JUMP(ge);   break;
            case x86_jl:    JUMP(l);    break;
            case x86_jcxz:  JUMP(cxz);  break;
            case x86_jecxz: JUMP(ecxz); break;
#           undef JUMP

#           define SET(tag) {                                                                                                  \
                if (operands.size()!=1)                                                                                        \
                    throw Exception("instruction must have one operand", insn);                                                \
                write8(operands[0], policy.concat(FLAGCOMBO_##tag, number<7>(0)));                                             \
            }
            case x86_setne: SET(ne); break;
            case x86_sete:  SET(e);  break;
            case x86_setno: SET(no); break;
            case x86_seto:  SET(o);  break;
            case x86_setpo: SET(po); break;
            case x86_setpe: SET(pe); break;
            case x86_setns: SET(ns); break;
            case x86_sets:  SET(s);  break;
            case x86_setae: SET(ae); break;
            case x86_setb:  SET(b);  break;
            case x86_setbe: SET(be); break;
            case x86_seta:  SET(a);  break;
            case x86_setle: SET(le); break;
            case x86_setg:  SET(g);  break;
            case x86_setge: SET(ge); break;
            case x86_setl:  SET(l);  break;
#           undef SET
                
#           define CMOV(tag) {                                                                                                 \
                if (operands.size()!=2)                                                                                        \
                    throw Exception("instruction must have two operands", insn);                                               \
                switch (numBytesInAsmType(operands[0]->get_type())) {                                                          \
                    case 2: write16(operands[0], policy.ite(FLAGCOMBO_##tag, read16(operands[1]), read16(operands[0]))); break; \
                    case 4: write32(operands[0], policy.ite(FLAGCOMBO_##tag, read32(operands[1]), read32(operands[0]))); break; \
                    default: throw Exception("size not implemented", insn);                                                    \
                                                                                                                               \
                }                                                                                                              \
            }
            case x86_cmovne:    CMOV(ne);       break;
            case x86_cmove:     CMOV(e);        break;
            case x86_cmovno:    CMOV(no);       break;
            case x86_cmovo:     CMOV(o);        break;
            case x86_cmovpo:    CMOV(po);       break;
            case x86_cmovpe:    CMOV(pe);       break;
            case x86_cmovns:    CMOV(ns);       break;
            case x86_cmovs:     CMOV(s);        break;
            case x86_cmovae:    CMOV(ae);       break;
            case x86_cmovb:     CMOV(b);        break;
            case x86_cmovbe:    CMOV(be);       break;
            case x86_cmova:     CMOV(a);        break;
            case x86_cmovle:    CMOV(le);       break;
            case x86_cmovg:     CMOV(g);        break;
            case x86_cmovge:    CMOV(ge);       break;
            case x86_cmovl:     CMOV(l);        break;
#           undef CMOV

            /* The flag expressions are no longer needed */
#           undef FLAGCOMBO_ne
#           undef FLAGCOMBO_e
#           undef FLAGCOMBO_ns
#           undef FLAGCOMBO_s
#           undef FLAGCOMBO_ae
#           undef FLAGCOMBO_b
#           undef FLAGCOMBO_be
#           undef FLAGCOMBO_a
#           undef FLAGCOMBO_l
#           undef FLAGCOMBO_ge
#           undef FLAGCOMBO_le
#           undef FLAGCOMBO_g
#           undef FLAGCOMBO_cxz
#           undef FLAGCOMBO_ecxz

            case x86_cld: {
                if (operands.size()!=0)
                    throw Exception("instruction must have no operands", insn);
                writeRegister(REG_DF, policy.false_());
                break;
            }

            case x86_std: {
                if (operands.size()!=0)
                    throw Exception("instruction must have no operands", insn);
                writeRegister(REG_DF, policy.true_());
                break;
            }

            case x86_clc: {
                if (operands.size()!=0)
                    throw Exception("instruction must have no operands", insn);
                writeRegister(REG_CF, policy.false_());
                break;
            }

            case x86_stc: {
                if (operands.size()!=0)
                    throw Exception("instruction must have no operands", insn);
                writeRegister(REG_CF, policy.true_());
                break;
            }

            case x86_cmc: {
                if (operands.size()!=0)
                    throw Exception("instruction must have no operands", insn);
                writeRegister(REG_CF, policy.invert(readRegister<1>(REG_CF)));
                break;
            }

            case x86_nop:
                break;


            case x86_repne_scasb: repne_scas_semantics<1>(insn); break;
            case x86_repne_scasw: repne_scas_semantics<2>(insn); break;
            case x86_repne_scasd: repne_scas_semantics<4>(insn); break;
            case x86_repe_scasb:  repe_scas_semantics<1>(insn);  break;
            case x86_repe_scasw:  repe_scas_semantics<2>(insn);  break;
            case x86_repe_scasd:  repe_scas_semantics<4>(insn);  break;

            case x86_scasb: scas_semantics<1>(insn, policy.true_()); break;
            case x86_scasw: scas_semantics<2>(insn, policy.true_()); break;
            case x86_scasd: scas_semantics<4>(insn, policy.true_()); break;

            case x86_repne_cmpsb: repne_cmps_semantics<1>(insn); break;
            case x86_repne_cmpsw: repne_cmps_semantics<2>(insn); break;
            case x86_repne_cmpsd: repne_cmps_semantics<4>(insn); break;
            case x86_repe_cmpsb:  repe_cmps_semantics<1>(insn);  break;
            case x86_repe_cmpsw:  repe_cmps_semantics<2>(insn);  break;
            case x86_repe_cmpsd:  repe_cmps_semantics<4>(insn);  break;

            case x86_cmpsb: cmps_semantics<1>(insn, policy.true_()); break;
            case x86_cmpsw: cmps_semantics<2>(insn, policy.true_()); break;
            case x86_cmpsd:
                /* This mnemonic, CMPSD, refers to two instructions: opcode A7 compares registers SI and DI (16-, 32-, or
                 * 64-bits) and sets the status flags. Opcode "F2 0F C2 /r ib" takes three arguments (an MMX register, an MMX
                 * or 64-bit register, and an 8-bit immediate) and compares floating point values.  The instruction semantics
                 * layer doesn't handle floating point instructions yet and reports them as "Bad instruction". */
                if (0==operands.size()) {
                    cmps_semantics<4>(insn, policy.true_()); break;
                } else {
                    /* Floating point instructions are not handled yet. */
                    throw Exception("instruction not implemented", insn);
                }
                break;

            case x86_movsb: movs_semantics<1>(insn, policy.true_()); break;
            case x86_movsw: movs_semantics<2>(insn, policy.true_()); break;
            case x86_movsd: movs_semantics<4>(insn, policy.true_()); break;
                
            case x86_rep_movsb: rep_movs_semantics<1>(insn); break;
            case x86_rep_movsw: rep_movs_semantics<2>(insn); break;
            case x86_rep_movsd: rep_movs_semantics<4>(insn); break;

            case x86_stosb: stos_semantics<1>(insn, policy.true_()); break;
            case x86_stosw: stos_semantics<2>(insn, policy.true_()); break;
            case x86_stosd: stos_semantics<4>(insn, policy.true_()); break;

            case x86_rep_stosb: rep_stos_semantics<1>(insn); break;
            case x86_rep_stosw: rep_stos_semantics<2>(insn); break;
            case x86_rep_stosd: rep_stos_semantics<4>(insn); break;

            case x86_lodsb: lods_semantics<1>(insn); break;
            case x86_lodsw: lods_semantics<2>(insn); break;
            case x86_lodsd: lods_semantics<4>(insn);  break;

            case x86_hlt: {
                if (operands.size()!=0)
                    throw Exception("instruction must have no operands", insn);
                policy.hlt();
                writeRegister(REG_EIP, orig_eip);
                break;
            }

            case x86_cpuid: {
                if (operands.size()!=0)
                    throw Exception("instruction must have no operands", insn);
                policy.cpuid();
                break;
            }

            case x86_rdtsc: {
                if (operands.size()!=0)
                    throw Exception("instruction must have no operands", insn);
                Word(64) tsc = policy.rdtsc();
                writeRegister(REG_EAX, extract<0, 32>(tsc));
                writeRegister(REG_EDX, extract<32, 64>(tsc));
                break;
            }

            case x86_int: {
                if (operands.size()!=1)
                    throw Exception("instruction must have one operand", insn);
                SgAsmByteValueExpression* bv = isSgAsmByteValueExpression(operands[0]);
                if (!bv)
                    throw Exception("operand must be a byte value expression", insn);
                policy.interrupt(bv->get_value());
                break;
            }

                /* This is a dummy version that should be replaced later FIXME */
            case x86_fnstcw: {
                if (operands.size()!=1)
                    throw Exception("instruction must have one operand", insn);
                write16(operands[0], number<16>(0x37f));
                break;
            }

            case x86_fldcw: {
                if (operands.size()!=1)
                    throw Exception("instruction must have one operand", insn);
                read16(operands[0]); /* To catch access control violations */
                break;
            }

            case x86_sysenter: {
                if (operands.size()!=0)
                    throw Exception("instruction must have no operands", insn);
                policy.sysenter();
                break;
            }

            default: {
                throw Exception("instruction not implemented", insn);
                break;
            }
        }
    } catch(Exception e) {
        if (!e.insn)
            e.insn = insn;
        throw e;
    }
#endif

    void processInstruction(SgAsmx86Instruction* insn) {
        ROSE_ASSERT(insn);
        current_instruction = insn;
        policy.startInstruction(insn);
        translate(insn);
        policy.finishInstruction(insn);
    }

    void processBlock(const SgAsmStatementPtrList& stmts, size_t begin, size_t end) {
        if (begin == end) return;
        policy.startBlock(stmts[begin]->get_address());
        for (size_t i = begin; i < end; ++i) {
            processInstruction(isSgAsmx86Instruction(stmts[i]));
        }
        policy.finishBlock(stmts[begin]->get_address());
    }

    static bool isRepeatedStringOp(SgAsmStatement* s) {
        SgAsmx86Instruction* insn = isSgAsmx86Instruction(s);
        if (!insn) return false;
        switch (insn->get_kind()) {
            case x86_repe_cmpsb: return true;
            case x86_repe_cmpsd: return true;
            case x86_repe_cmpsq: return true;
            case x86_repe_cmpsw: return true;
            case x86_repe_scasb: return true;
            case x86_repe_scasd: return true;
            case x86_repe_scasq: return true;
            case x86_repe_scasw: return true;
            case x86_rep_insb: return true;
            case x86_rep_insd: return true;
            case x86_rep_insw: return true;
            case x86_rep_lodsb: return true;
            case x86_rep_lodsd: return true;
            case x86_rep_lodsq: return true;
            case x86_rep_lodsw: return true;
            case x86_rep_movsb: return true;
            case x86_rep_movsd: return true;
            case x86_rep_movsq: return true;
            case x86_rep_movsw: return true;
            case x86_repne_cmpsb: return true;
            case x86_repne_cmpsd: return true;
            case x86_repne_cmpsq: return true;
            case x86_repne_cmpsw: return true;
            case x86_repne_scasb: return true;
            case x86_repne_scasd: return true;
            case x86_repne_scasq: return true;
            case x86_repne_scasw: return true;
            case x86_rep_outsb: return true;
            case x86_rep_outsd: return true;
            case x86_rep_outsw: return true;
            case x86_rep_stosb: return true;
            case x86_rep_stosd: return true;
            case x86_rep_stosq: return true;
            case x86_rep_stosw: return true;
            default: return false;
        }
    }

    static bool isHltOrInt(SgAsmStatement* s) {
        SgAsmx86Instruction* insn = isSgAsmx86Instruction(s);
        if (!insn) return false;
        switch (insn->get_kind()) {
            case x86_hlt: return true;
            case x86_int: return true;
            default: return false;
        }
    }

    void processBlock(SgAsmBlock* b) {
        const SgAsmStatementPtrList& stmts = b->get_statementList();
        if (stmts.empty()) return;
        if (!isSgAsmInstruction(stmts[0])) return; /* A block containing functions or something */
        size_t i = 0;
        while (i < stmts.size()) {
            size_t oldI = i;
            /* Advance until either i points to a repeated string op or it is just after a hlt or int */
            while (i < stmts.size() && !isRepeatedStringOp(stmts[i]) && (i == oldI || !isHltOrInt(stmts[i - 1]))) ++i;
            processBlock(stmts, oldI, i);
            if (i >= stmts.size()) break;
            if (isRepeatedStringOp(stmts[i])) {
                processBlock(stmts, i, i + 1);
                ++i;
            }
            ROSE_ASSERT(i != oldI);
        }
    }

};

#undef Word
        
    } /*namespace*/
} /*namespace*/
#endif /* ROSE_X86INSTRUCTIONSEMANTICS_H */
