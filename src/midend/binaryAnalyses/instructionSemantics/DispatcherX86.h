#ifndef ROSE_DispatcherX86_H
#define ROSE_DispatcherX86_H

#include "BaseSemantics2.h"

namespace BinaryAnalysis {
namespace InstructionSemantics {

typedef boost::shared_ptr<class DispatcherX86> DispatcherX86Ptr;

class DispatcherX86: public BaseSemantics::Dispatcher {
protected:
    explicit DispatcherX86(const BaseSemantics::RiscOperatorsPtr &ops): BaseSemantics::Dispatcher(ops) {
        iproc_init();
    }

    /** Loads the iproc table with instruction processing functors. This normally happens from the constructor. */
    void iproc_init();

public:
    /** Constructor. */
    static DispatcherX86Ptr instance(const BaseSemantics::RiscOperatorsPtr &ops) {
        return DispatcherX86Ptr(new DispatcherX86(ops));
    }

    /** Virtual constructor. */
    virtual BaseSemantics::DispatcherPtr create(const BaseSemantics::RiscOperatorsPtr &ops) const /*override*/ {
        return instance(ops);
    }

    /** Dynamic cast to a DispatcherX86Ptr with assertion. */
    static DispatcherX86Ptr promote(const BaseSemantics::DispatcherPtr &d) {
        DispatcherX86Ptr retval = boost::dynamic_pointer_cast<DispatcherX86>(d);
        assert(retval!=NULL);
        return retval;
    }

    virtual int iproc_key(SgAsmInstruction *insn_) const /*override*/ {
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(insn_);
        assert(insn!=NULL);
        return insn->get_kind();
    }

    /** Write the @p value to the location (memory or register) specified by the expression @p e. The number of bytes written
     *  depends on the width of @p value. */
    virtual void write(SgAsmExpression *e, const BaseSemantics::SValuePtr &value);

    // FIXME: Soon to be deprecated
    virtual void write8(SgAsmExpression *e, const BaseSemantics::SValuePtr &value) {
        assert(value->get_width()==8);
        write(e, value);
    }
    virtual void write16(SgAsmExpression *e, const BaseSemantics::SValuePtr &value) {
        assert(value->get_width()==16);
        write(e, value);
    }
    virtual void write32(SgAsmExpression *e, const BaseSemantics::SValuePtr &value) {
        assert(value->get_width()==32);
        write(e, value);
    }

    /** Read a value from the location (memory or register) specified by the expression @p e. */
    virtual BaseSemantics::SValuePtr read(SgAsmExpression *e, size_t nbits);

    // FIXME: Soon to be deprecated
    virtual BaseSemantics::SValuePtr read8(SgAsmExpression *e) { return read(e, 8); }
    virtual BaseSemantics::SValuePtr read16(SgAsmExpression *e) { return read(e, 16); }
    virtual BaseSemantics::SValuePtr read32(SgAsmExpression *e) { return read(e, 32); }
    
    virtual BaseSemantics::SValuePtr readEffectiveAddress(SgAsmExpression *expr);

    /** Read a multi-byte value from memory. */
    virtual BaseSemantics::SValuePtr readMemory(X86SegmentRegister segreg, const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr& cond, size_t nbits);

    /** Write a multi-byte value to memory. */
    void writeMemory(X86SegmentRegister segreg, const BaseSemantics::SValuePtr &addr,
                     const BaseSemantics::SValuePtr &value, const BaseSemantics::SValuePtr &cond);

    /** Set parity, sign, and zero flags appropriate for result value. */
    virtual void setFlagsForResult(const BaseSemantics::SValuePtr &result);

    /** Conditionally set parity, sign, and zero flags appropriate for result value. */
    virtual void setFlagsForResult(const BaseSemantics::SValuePtr &result, const BaseSemantics::SValuePtr &cond);

    /** Returns true if byte @p v has an even number of bits set; false for an odd number */
    virtual BaseSemantics::SValuePtr parity(const BaseSemantics::SValuePtr &v);

    /** Conditionally invert the bits of @p value.  The bits are inverted if @p maybe is true, otherwise @p value is returned. */
    virtual BaseSemantics::SValuePtr invertMaybe(const BaseSemantics::SValuePtr &value, bool maybe);

    /** Determines whether @p value is greater than or equal to ten. */
    virtual BaseSemantics::SValuePtr greaterOrEqualToTen(const BaseSemantics::SValuePtr &value);

    /** Return a Boolean for the specified flag combo for an instruction. */
    virtual BaseSemantics::SValuePtr flagsCombo(X86InstructionKind k);

    /** Enters a loop for a REP-, REPE-, or REPNE-prefixed instruction.  The return value is true if ECX is non-zero or the
     *  instruction doesn't have repeat prefix, and false otherwise. Use this in conjunction with repLeave(). */
    virtual BaseSemantics::SValuePtr repEnter(X86RepeatPrefix);

    /** Leave a loop for a REP-, REPE-, or REPNE-prefixed instruction.  The @p in_loop argument is the Boolean that indicates
     *  whether we just executed the instruction, and is usually the return value from the previous repEnter() call. If @p
     *  in_loop is false then this function is a no-op. Otherwise, the ECX register is decremented and, if it is non-zero and
     *  the repeat condition (true, equal, or not-equal) is satisified, then the EIP register is reset to the specified
     *  instruction address causing the instruction to be repeated. Use this in conjunction with repEnter(). */
    virtual void repLeave(X86RepeatPrefix, const BaseSemantics::SValuePtr &in_loop, rose_addr_t insn_va);

    /** Adds two values and adjusts flags.  This method can be used for subtraction if @p b is two's complement and @p
     *  invertCarries is set.  If @p cond is supplied, then the addition and flag adjustments are conditional.
     * @{ */
    virtual BaseSemantics::SValuePtr doAddOperation(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b,
                                                    bool invertCarries, const BaseSemantics::SValuePtr &carryIn);
    virtual BaseSemantics::SValuePtr doAddOperation(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b,
                                                    bool invertCarries, const BaseSemantics::SValuePtr &carryIn,
                                                    const BaseSemantics::SValuePtr &cond);
    /** @}*/

    /** Increments or decrements a value and adjusts flags.  If @p dec is set then the value is decremented instead of
     *  incremented. If @p setCarry is set then the CF flag is affected. */
    virtual BaseSemantics::SValuePtr doIncOperation(const BaseSemantics::SValuePtr &a, bool dec, bool setCarry);

    /** Implements the SHR, SAR, SHL, SAL, SHRD, and SHLD instructions for various operand sizes.  The shift amount is always 8
     *  bits wide in the instruction, but the semantics mask off all but the low-order bits, keeping 5 bits in 32-bit mode and
     *  7 bits in 64-bit mode (indicated by the @p shiftSignificantBits argument).  The semantics of SHL and SAL are
     *  identical (in fact, ROSE doesn't even define x86_sal). The @p source_bits argument contains the bits to be shifted into
     *  the result and is used only for SHRD and SHLD instructions. */
    virtual BaseSemantics::SValuePtr doShiftOperation(X86InstructionKind kind,
                                                      const BaseSemantics::SValuePtr &operand,
                                                      const BaseSemantics::SValuePtr &source_bits,
                                                      const BaseSemantics::SValuePtr &total_shift,
                                                      size_t shiftSignificantBits);
};



#if 0




/** Translation class.  Translates x86 instructions to RISC-like operations and invokes those operations in the supplied
 *  semantic policy (a template argument).  See the BinaryAnalysis::InstructionSemantics name space for details. Apologies for
 *  the lack of documentation for this class.  You can at least find some examples in the semantics.C file of the
 *  tests/roseTests/binaryTests directory, among others. */
template <typename Policy, template <size_t> class WordType>
struct X86InstructionSemantics {
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
#endif
        
    } /*namespace*/
} /*namespace*/
#endif
