#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherMips.h>

#include <Rose/Affirm.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/InstructionEnumsMips.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Dispatcher.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Exception.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryState.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RiscOperators.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/State.h>
#include <Rose/StringUtility/Diagnostics.h>

#include <SgAsmMipsInstruction.h>
#include <SgAsmOperandList.h>

#include <Cxx_GrammarDowncast.h>

using namespace Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functors that handle individual MIPS instruction kinds
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Mips {

// An intermediate class that reduces the amount of typing in all that follows.  Its `process` method does some up-front checking,
// dynamic casting, and pointer dereferencing and then calls the `p` method that does the real work.
class P: public BaseSemantics::InsnProcessor {
public:
    using D = DispatcherMips*;
    using Ops = RiscOperators*;
    using I = SgAsmMipsInstruction*;
    using A = const SgAsmExpressionPtrList&;

    virtual void p(D, Ops, I, A) = 0;

    // Perform the semantics for the specified instruction.
    virtual void process(const Dispatcher::Ptr &dispatcher_, SgAsmInstruction *insn_) override {
        DispatcherMipsPtr dispatcher = DispatcherMips::promote(dispatcher_);
        RiscOperators::Ptr operators = dispatcher->operators();
        SgAsmMipsInstruction *insn = isSgAsmMipsInstruction(insn_);
        ASSERT_not_null(insn);
        ASSERT_require(insn == operators->currentInstruction());

        // Update the program counter/instruction pointer (PC) to point to the fall-through instruction before we process the
        // instruction, so that the semantics for individual instructions (like branches) can override this choice by assigning a
        // new value to PC.  However, we must be careful of PC-relative addressing and remember that we already incremented the PC.
        dispatcher->advanceInstructionPointer(insn);
        SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();

        operators->comment("executing instruction core");
        p(dispatcher.get(), operators.get(), insn, operands);
    }

    void assert_args(I insn, A args, size_t nargs) {
        if (args.size()!=nargs) {
            std::string mesg = "instruction must have " + StringUtility::plural(nargs, "arguments");
            throw BaseSemantics::Exception(mesg, insn);
        }
    }
};

// Add word
struct IP_add: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        const size_t nBits = d->architecture()->bitsPerWord();

        // Compute sum
        SValue::Ptr rs = d->read(args[1]);
        SValue::Ptr rt = d->read(args[2]);
        SValue::Ptr sum = ops->add(rs, rt);

        // Calculate overflow
        SValue::Ptr wideRs = ops->signExtend(d->read(args[1]), nBits + 1);
        SValue::Ptr wideRt = ops->signExtend(d->read(args[2]), nBits + 1);
        SValue::Ptr wideSum = ops->add(wideRs, wideRt);
        SValue::Ptr wideSum32 = ops->extract(wideSum, 32, 33); // bit #32
        SValue::Ptr wideSum31 = ops->extract(wideSum, 31, 32); // bit #31
        SValue::Ptr overflow = ops->isNotEqual(wideSum31, wideSum32);
        SValue::Ptr oldRd = d->read(args[0], nBits);
        SValue::Ptr result = ops->ite(overflow, oldRd, sum);

        // Side effects (do after everything else)
        d->write(args[0], result);
        ops->raiseInterrupt(mips_signal_exception, mips_integer_overflow, overflow);
    }
};

// Add immediate word
struct IP_addi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        const size_t nBits = d->architecture()->bitsPerWord();

        // Compute sum
        SValue::Ptr rs = d->read(args[1], nBits);       // 2-args version does unsigned extend or truncate
        SValue::Ptr imm = ops->signExtend(d->read(args[2]), nBits);
        SValue::Ptr sum = ops->add(rs, imm);

        // Calculate overflow
        SValue::Ptr wideRs = ops->signExtend(d->read(args[1]), nBits + 1);
        SValue::Ptr wideImm = ops->signExtend(d->read(args[2]), nBits + 1);
        SValue::Ptr wideSum = ops->add(wideRs, wideImm);
        SValue::Ptr wideSum32 = ops->extract(wideSum, 32, 33); // bit #32
        SValue::Ptr wideSum31 = ops->extract(wideSum, 31, 32); // bit #31
        SValue::Ptr overflow = ops->isNotEqual(wideSum31, wideSum32);
        SValue::Ptr oldRt = d->read(args[0], nBits);
        SValue::Ptr result = ops->ite(overflow, oldRt, sum);

        // Side effects (do after everything else)
        d->write(args[0], result);
        ops->raiseInterrupt(mips_signal_exception, mips_integer_overflow, overflow);
    }
};

// Add immediate unsigned word
struct IP_addiu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        const size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr rs = d->read(args[1]);
        SValue::Ptr imm = ops->signExtend(d->read(args[2]), nBits);
        SValue::Ptr result = ops->add(rs, imm);
        d->write(args[0], result);
    }
};

// Add unsigned word
struct IP_addu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr rs = d->read(args[1]);
        SValue::Ptr rt = d->read(args[2]);
        SValue::Ptr result = ops->add(rs, rt);
#if 0 // [Robb Matzke 2024-05-01] don't do this
        //                            side-effect-1     side-effect-2     (GCC)
        //                            side-effect-2     side-effect-1     (clang)
        SValue::Ptr result = ops->add(d->read(args[1]), d->read(args[2]));
#endif
        d->write(args[0], result);
    }
};

// Bitwise logical AND
struct IP_and: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr rs = d->read(args[1]);
        SValue::Ptr rt = d->read(args[2]);
        SValue::Ptr result = ops->and_(rs, rt);
        d->write(args[0], result);
    }
};

// Bitwise logical AND immediate
struct IP_andi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        const size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr rs = d->read(args[1]);
        SValue::Ptr imm = ops->unsignedExtend(d->read(args[2]), nBits);
        SValue::Ptr result = ops->and_(rs, imm);
        d->write(args[0], result);
    }
};

// Breakpoint
struct IP_break: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->raiseInterrupt(mips_signal_exception, mips_breakpoint, ops->boolean_(true));
    }
};

// Count leading ones in word
struct IP_clo: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr rs = d->read(args[1]);
        SValue::Ptr result = ops->countLeadingOnes(rs);
        d->write(args[0], result);
    }
};

// Count leading zeros in word
struct IP_clz: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr rs = d->read(args[1]);
        SValue::Ptr result = ops->countLeadingZeros(rs);
        d->write(args[0], result);
    }
};

// Divide word
struct IP_div: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr rs = d->read(args[0]);
        SValue::Ptr rt = d->read(args[1]);
        SValue::Ptr quotient = ops->signedDivide(rs, rt);
        SValue::Ptr remainder = ops->signedModulo(rs, rt);
        ops->writeRegister(d->REG_LO, quotient);
        ops->writeRegister(d->REG_HI, remainder);
    }
};

// Divide unsigned word
struct IP_divu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr rs = d->read(args[0]);
        SValue::Ptr rt = d->read(args[1]);
        SValue::Ptr quotient = ops->unsignedDivide(rs, rt);
        SValue::Ptr remainder = ops->unsignedModulo(rs, rt);
        ops->writeRegister(d->REG_LO, quotient);
        ops->writeRegister(d->REG_HI, remainder);
    }
};

// Load byte
struct IP_lb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr result = ops->signExtend(d->read(args[1]), nBits);
        d->write(args[0], result);
    }
};

// Load byte EVA
struct IP_lbe: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr result = ops->signExtend(d->read(args[1]), nBits);
        d->write(args[0], result);
    }
};

// Load byte unsigned
struct IP_lbu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        const size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr result = ops->unsignedExtend(d->read(args[1]), nBits);
        d->write(args[0], result);
    }
};

// Load byte unsigned EVA
struct IP_lbue: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        const size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr result = ops->unsignedExtend(d->read(args[1]), nBits);
        d->write(args[0], result);
    }
};

// Load doubleword (LD)
//TODO: implement

// Load doubleword to floating point (LDC1)
//TODO: implement

// Load doubleword to coprocessor 2 (LDC2)
//TODO: implement

// Load halfword
struct IP_lh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr result = ops->signExtend(d->read(args[1]), nBits);
        d->write(args[0], result);
    }
};

// Load halfword unsigned
struct IP_lhu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr result = ops->unsignedExtend(d->read(args[1]), nBits);
        d->write(args[0], result);
    }
};

// Load upper immediate
struct IP_lui: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr zeros = ops->number_(16, 0);
        SValue::Ptr imm = ops->concatHiLo(d->read(args[1]), zeros);
        SValue::Ptr result = ops->signExtend(imm, nBits);
        d->write(args[0], result);
    }
};

// Load word
struct IP_lw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr result = ops->signExtend(d->read(args[1]), nBits);
        d->write(args[0], result);
    }
};

// Load word to floating point
struct IP_lwc1: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        // TODO: load into low word of FPR ft
        d->write(args[0], d->read(args[1]));
    }
};

// Load word EVA
struct IP_lwe: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr result = ops->signExtend(d->read(args[1]), nBits);
        d->write(args[0], result);
    }
};

// Load word unsigned
struct IP_lwu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr result = ops->unsignedExtend(d->read(args[1]), nBits);
        d->write(args[0], result);
    }
};

// Multiply and add word to hi, lo
// Note: removed in release 6
struct IP_madd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nBits = d->architecture()->bitsPerWord();

        // Multiply contents of rs by rt
        SValue::Ptr rs = d->read(args[0]);
        SValue::Ptr rt = d->read(args[1]);
        SValue::Ptr mult = ops->signedMultiply(rs, rt);

        // Add results to (HI,LO)
        SValue::Ptr hi = ops->readRegister(d->REG_HI);
        SValue::Ptr lo = ops->readRegister(d->REG_LO);
        SValue::Ptr hilo = ops->concatHiLo(hi, lo);
        SValue::Ptr sum = ops->add(hilo, mult);

        // Extract and write results
        hi = ops->signExtend(ops->extract(sum, 32, 64), nBits);
        lo = ops->signExtend(ops->extract(sum,  0, 32), nBits);
        ops->writeRegister(d->REG_HI, hi);
        ops->writeRegister(d->REG_LO, lo);
    }
};

// Multiply and add unsigned word to hi, lo
// Note: removed in release 6
struct IP_maddu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nBits = d->architecture()->bitsPerWord();

        // Multiply contents of rs by rt
        SValue::Ptr rs = d->read(args[0]);
        SValue::Ptr rt = d->read(args[1]);
        SValue::Ptr mult = ops->unsignedMultiply(rs, rt);

        // Add results to (HI,LO)
        SValue::Ptr hi = ops->readRegister(d->REG_HI);
        SValue::Ptr lo = ops->readRegister(d->REG_LO);
        SValue::Ptr hilo = ops->concatHiLo(hi, lo);
        SValue::Ptr sum = ops->add(hilo, mult);

        // Extract and write results
        hi = ops->signExtend(ops->extract(sum, 32, 64), nBits);
        lo = ops->signExtend(ops->extract(sum,  0, 32), nBits);
        ops->writeRegister(d->REG_HI, hi);
        ops->writeRegister(d->REG_LO, lo);
    }
};

// Move word from hi register
// Note: removed in release 6
struct IP_mfhi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        d->write(args[0], ops->readRegister(d->REG_HI));
    }
};

// Move word from lo register
// Note: removed in release 6
struct IP_mflo: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        d->write(args[0], ops->readRegister(d->REG_LO));
    }
};

// Floating point move (single)
struct IP_mov_s: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr fs = d->read(args[1]);
        d->write(args[0], fs);
    }
};

// Floating point move (double)
struct IP_mov_d: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr fs = d->read(args[1]);
        d->write(args[0], fs);
    }
};

// Floating point move (paired-single)
// Note: removed in release 6
struct IP_mov_ps: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr fs = d->read(args[1]);
        d->write(args[0], fs);
    }
};

// Move conditional on floating point false
// Note: removed in release 6
struct IP_movf: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr rs = d->read(args[1]);
        SValue::Ptr cc = d->read(args[2]);
        SValue::Ptr oldRd = d->read(args[0]);
        SValue::Ptr result = ops->ite(ops->equalToZero(cc), rs, oldRd);
        d->write(args[0], result);
    }
};

// Floating point move conditional on floating point false (single)
// Note: removed in release 6
struct IP_movf_s: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr fs = d->read(args[1]);
        SValue::Ptr cc = d->read(args[2]);
        SValue::Ptr oldFd = d->read(args[0]);
        SValue::Ptr result = ops->ite(ops->equalToZero(cc), fs, oldFd);
        d->write(args[0], result);
    }
};

// Floating point move conditional on floating point false (double)
// Note: removed in release 6
struct IP_movf_d: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr fs = d->read(args[1]);
        SValue::Ptr cc = d->read(args[2]);
        SValue::Ptr oldFd = d->read(args[0]);
        SValue::Ptr result = ops->ite(ops->equalToZero(cc), fs, oldFd);
        d->write(args[0], result);
    }
};

// Floating point move conditional on floating point false (paired-single)
// Note: removed in release 6
struct IP_movf_ps: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr fs = d->read(args[1]);
        SValue::Ptr cc = d->read(args[2]);
        SValue::Ptr oldFd = d->read(args[0]);
        SValue::Ptr result = ops->ite(ops->equalToZero(cc), fs, oldFd);
        d->write(args[0], result);
    }
};

// Move conditional on zero
// TODO:
//struct IP_movz: P {

// Floating point move conditional on zero
// TODO:
//struct IP_movz.fmt: P {

// Multiply and subtract word to hi, lo (mips_msub)
// See IP_msub_su below

// Multiply and subtract unsigned word to hi, lo (mips_msubu)
// See IP_msub_su below

// Multiply and subtract word (signed and unsigned) to hi, lo
// Note: removed in release 6
struct IP_msub_su: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr rs = d->read(args[0]);
        SValue::Ptr rt = d->read(args[1]);

        // Multiply (signed or unsigned) contents of rs by rt
        SValue::Ptr mult{};
        if (insn->get_kind() == mips_msub) {
          mult = ops->signedMultiply(rs, rt);
        }
        else if (insn->get_kind() == mips_msubu) {
          mult = ops->unsignedMultiply(rs, rt);
        }

        // Subtract results from (HI,LO)
        SValue::Ptr hi = ops->readRegister(d->REG_HI);
        SValue::Ptr lo = ops->readRegister(d->REG_LO);
        SValue::Ptr hilo = ops->concatHiLo(hi, lo);
        SValue::Ptr sub = ops->subtract(hilo, mult);

        // Extract and write results
        SValue::Ptr hiBits = ops->signExtend(ops->extract(sub, 32, 64), nBits);
        SValue::Ptr loBits = ops->signExtend(ops->extract(sub,  0, 32), nBits);
        ops->writeRegister(d->REG_HI, hiBits);
        ops->writeRegister(d->REG_LO, loBits);
    }
};

// Multiply word to GPR
// Note: removed in release 6
struct IP_mul: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        size_t nBits = d->architecture()->bitsPerWord();

        // Multiply (signed or unsigned) contents of rs by rt
        SValue::Ptr rs = d->read(args[1]);
        SValue::Ptr rt = d->read(args[2]);
        SValue::Ptr smul = ops->signedMultiply(rs, rt);

        // Extract and write results
        SValue::Ptr loBits = ops->signExtend(ops->extract(smul, 0, 32), nBits);
        d->write(args[0], loBits);
    }
};

// Multiply word (mips_mult)
// See IP_mult_su below

// Multiply unsigned word (mips_multu)
// See IP_mult_su below

// Multiply word (signed and unsigned)
// Note: removed in release 6
struct IP_mult_su: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr rs = d->read(args[0]);
        SValue::Ptr rt = d->read(args[1]);

        // Multiply (signed or unsigned) contents of rs by rt
        SValue::Ptr mult{};
        if (insn->get_kind() == mips_mult) {
          mult = ops->signedMultiply(rs, rt);
        }
        else if (insn->get_kind() == mips_multu) {
          mult = ops->unsignedMultiply(rs, rt);
        }

        // Extract and write results
        SValue::Ptr hiBits = ops->signExtend(ops->extract(mult, 32, 64), nBits);
        SValue::Ptr loBits = ops->signExtend(ops->extract(mult,  0, 32), nBits);
        ops->writeRegister(d->REG_HI, hiBits);
        ops->writeRegister(d->REG_LO, loBits);
    }
};

// No operation
struct IP_nop: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
    }
};

// Not OR
struct IP_nor: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr rs = d->read(args[1]);
        SValue::Ptr rt = d->read(args[2]);
        SValue::Ptr result = ops->invert(ops->or_(rs, rt));
        d->write(args[0], result);
    }
};

// Bitwise OR
struct IP_or: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr rs = d->read(args[1]);
        SValue::Ptr result = ops->or_(rs, d->read(args[2]));
        d->write(args[0], result);
    }
};

// Bitwise OR immediate
struct IP_ori: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        const size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr rs = d->read(args[1]);
        SValue::Ptr imm = ops->unsignedExtend(d->read(args[2]), nBits);
        SValue::Ptr result = ops->or_(rs, imm);
        d->write(args[0], result);
    }
};

// Store byte
struct IP_sb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr byte = d->read(args[0], 8);
        d->write(args[1], byte);
    }
};

// Store byte EVA
struct IP_sbe: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr byte = d->read(args[0], 8);
        d->write(args[1], byte);
    }
};

// Store doubleword
//TODO: SD instruction doesn't exists in Mips.C (because 64 bit instruction?)

// Sign-extend byte
struct IP_seb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        const size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr byte = ops->signExtend(d->read(args[1],8), nBits);
        d->write(args[0], byte);
    }
};

// Sign-extend halfword
struct IP_seh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        const size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr halfWord = ops->signExtend(d->read(args[1],16), nBits);
        d->write(args[0], halfWord);
    }
};

// Store halfword
struct IP_sh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr halfWord = d->read(args[0], 16);
        d->write(args[1], halfWord);
    }
};

// Store halfword EVA
struct IP_she: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr halfWord = d->read(args[0], 16);
        d->write(args[1], halfWord);
    }
};

// Set on less than (mips_slt)
// See IP_slt_su below

// Set on less than unsigned (mips_sltu)
// See IP_slt_su below

// Set on less than immediate (mips_slti)
// See IP_slti_su below

// Set on less than immediate unsigned (mips_sltiu)
// See IP_slti_su below

// Set on less than (signed and unsigned)
struct IP_slt_su: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        const size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr rs = d->read(args[1]);
        SValue::Ptr rt = d->read(args[2]);

        // Compare (signed or unsigned) contents of rs and rt
        SValue::Ptr compare{};
        if (insn->get_kind() == mips_slt) {
          compare = ops->isSignedLessThan(rs, rt);
        }
        else if (insn->get_kind() == mips_sltu) {
          compare = ops->isUnsignedLessThan(rs, rt);
        }

        // Write comparison result
        SValue::Ptr zeros = ops->number_(nBits-1, 0);
        SValue::Ptr result = ops->concatHiLo(zeros, compare);
        d->write(args[0], result);
    }
};

// Set on less than immediate (signed and unsigned)
struct IP_slti_su: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        const size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr rs = d->read(args[1]);
        SValue::Ptr imm = ops->signExtend(d->read(args[2]), nBits);

        // Compare (signed or unsigned) contents of rs and immediate
        SValue::Ptr compare{};
        if (insn->get_kind() == mips_slti) {
          compare = ops->isSignedLessThan(rs, imm);
        }
        else if (insn->get_kind() == mips_sltiu) {
          compare = ops->isUnsignedLessThan(rs, imm);
        }

        // Write comparison result
        SValue::Ptr zeros = ops->number_(nBits-1, 0);
        SValue::Ptr result = ops->concatHiLo(zeros, compare);
        d->write(args[0], result);
    }
};

// Superscalar no operation
//   - implemented by IP_nop above

// Subtract word
struct IP_sub: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        const size_t nBits = d->architecture()->bitsPerWord();

        // Compute subtraction
        SValue::Ptr rs = d->read(args[1]);
        SValue::Ptr rt = d->read(args[2]);
        SValue::Ptr sub = ops->subtract(rs, rt);

        // Calculate overflow
        SValue::Ptr wideRs = ops->signExtend(d->read(args[1]), nBits + 1);
        SValue::Ptr wideRt = ops->signExtend(d->read(args[2]), nBits + 1);
        SValue::Ptr wideSub = ops->subtract(wideRs, wideRt);
        SValue::Ptr wideSub32 = ops->extract(wideSub, 32, 33); // bit #32
        SValue::Ptr wideSub31 = ops->extract(wideSub, 31, 32); // bit #31
        SValue::Ptr overflow = ops->isNotEqual(wideSub31, wideSub32);
        SValue::Ptr oldRd = d->read(args[0], nBits);
        SValue::Ptr result = ops->ite(overflow, oldRd, sub);

        // Side effects (do after everything else)
        d->write(args[0], result);
        ops->raiseInterrupt(mips_signal_exception, mips_integer_overflow, overflow);
    }
};

// Subtract unsigned word
struct IP_subu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr rs = d->read(args[1]);
        SValue::Ptr rt = d->read(args[2]);
        SValue::Ptr result = ops->subtract(rs, rt);
        d->write(args[0], result);
    }
};

// Store word
struct IP_sw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr word = d->read(args[0], 32);
        d->write(args[1], word);
    }
};

// Store word from floating point
struct IP_swc1: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr word = d->read(args[0], 32);
        d->write(args[1], word);
    }
};

// Store word EVA
struct IP_swe: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr word = d->read(args[0], 32);
        d->write(args[1], word);
    }
};

// Exclusive OR
struct IP_xor: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr rs = d->read(args[1]);
        SValue::Ptr result = ops->xor_(rs, d->read(args[2]));
        d->write(args[0], result);
    }
};

// Exclusive OR immediate
struct IP_xori: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        const size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr rs = d->read(args[1]);
        SValue::Ptr imm = ops->unsignedExtend(d->read(args[2]), nBits);
        SValue::Ptr result = ops->xor_(rs, imm);
        d->write(args[0], result);
    }
};

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      DispatcherMips
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
DispatcherMips::initializeDispatchTable() {
    iprocSet(mips_add,   new Mips::IP_add);
    iprocSet(mips_addi,  new Mips::IP_addi);
    iprocSet(mips_addiu, new Mips::IP_addiu);
    iprocSet(mips_addu,  new Mips::IP_addu);
    iprocSet(mips_and,   new Mips::IP_and);
    iprocSet(mips_andi,  new Mips::IP_andi);
    iprocSet(mips_break, new Mips::IP_break);
    iprocSet(mips_clo,   new Mips::IP_clo);
    iprocSet(mips_clz,   new Mips::IP_clz);
    iprocSet(mips_div,   new Mips::IP_div);
    iprocSet(mips_divu,  new Mips::IP_divu);
    iprocSet(mips_lb,    new Mips::IP_lb);
    iprocSet(mips_lbe,   new Mips::IP_lbe);
    iprocSet(mips_lbu,   new Mips::IP_lbu);
    iprocSet(mips_lbue,  new Mips::IP_lbue);
    iprocSet(mips_lh,    new Mips::IP_lh);
    iprocSet(mips_lhu,   new Mips::IP_lhu);
    iprocSet(mips_lui,   new Mips::IP_lui);
    iprocSet(mips_lw,    new Mips::IP_lw);
    iprocSet(mips_lwc1,  new Mips::IP_lwc1);
    iprocSet(mips_lwe,   new Mips::IP_lwe);
//  iprocSet(mips_lwu,   new Mips::IP_lwu); // mips_lwu (Release 6) not implemented in Mips.C
    iprocSet(mips_madd,  new Mips::IP_madd);
    iprocSet(mips_maddu, new Mips::IP_maddu);
    iprocSet(mips_mfhi,  new Mips::IP_mfhi);
    iprocSet(mips_mflo,  new Mips::IP_mflo);
    iprocSet(mips_mov_s, new Mips::IP_mov_s);
    iprocSet(mips_mov_d, new Mips::IP_mov_d);
    iprocSet(mips_mov_ps,new Mips::IP_mov_ps);

    iprocSet(mips_movf,    new Mips::IP_movf);
    iprocSet(mips_movf_s,  new Mips::IP_movf_s);
    iprocSet(mips_movf_d,  new Mips::IP_movf_d);
    iprocSet(mips_movf_ps, new Mips::IP_movf_ps);

    iprocSet(mips_msub,  new Mips::IP_msub_su); // Note, both mips_msub and mips_msubu use IP_msub_su
    iprocSet(mips_msubu, new Mips::IP_msub_su);
    iprocSet(mips_mul,   new Mips::IP_mul);
    iprocSet(mips_mult,  new Mips::IP_mult_su); // Note, both mips_mult and mips_multu use IP_mult_su
    iprocSet(mips_multu, new Mips::IP_mult_su);
    iprocSet(mips_nop,   new Mips::IP_nop);
    iprocSet(mips_nor,   new Mips::IP_nor);
    iprocSet(mips_or,    new Mips::IP_or);
    iprocSet(mips_ori,   new Mips::IP_ori);
    iprocSet(mips_sb,    new Mips::IP_sb);
    iprocSet(mips_sbe,   new Mips::IP_sbe);
    iprocSet(mips_seb,   new Mips::IP_seb);
    iprocSet(mips_seh,   new Mips::IP_seh);
    iprocSet(mips_sh,    new Mips::IP_sh);
    iprocSet(mips_she,   new Mips::IP_she);
    iprocSet(mips_slt,   new Mips::IP_slt_su); // Note, both mips_slt and mips_sltu use IP_slt_su
    iprocSet(mips_sltu,  new Mips::IP_slt_su);
    iprocSet(mips_slti,  new Mips::IP_slti_su); // Note, both mips_slti and mips_sltiu use IP_slt_su
    iprocSet(mips_sltiu, new Mips::IP_slti_su);
    iprocSet(mips_ssnop, new Mips::IP_nop); // Note, mips_ssnop implemented by IP_nop
    iprocSet(mips_sub,   new Mips::IP_sub);
    iprocSet(mips_subu,  new Mips::IP_subu);
    iprocSet(mips_sw,    new Mips::IP_sw);
    iprocSet(mips_swc1,  new Mips::IP_swc1);
    iprocSet(mips_swe,   new Mips::IP_swe);
    iprocSet(mips_xor,   new Mips::IP_xor);
    iprocSet(mips_xori,  new Mips::IP_xori);
}

DispatcherMips::~DispatcherMips() {}

DispatcherMips::DispatcherMips(const Architecture::Base::ConstPtr &arch)
    : Super(arch) {}

DispatcherMips::DispatcherMips(const Architecture::Base::ConstPtr &arch, const RiscOperators::Ptr &ops)
    : Super(arch, ops),
      REG_R0(arch->registerDictionary()->findOrThrow("r0")),
      REG_R1(arch->registerDictionary()->findOrThrow("r1")),
      REG_HI(arch->registerDictionary()->findOrThrow("hi")),
      REG_LO(arch->registerDictionary()->findOrThrow("lo")),
      REG_PC(arch->registerDictionary()->findOrThrow("pc")),
      REG_SP(arch->registerDictionary()->findOrThrow("sp")),
      REG_FP(arch->registerDictionary()->findOrThrow("fp")),
      REG_RA(arch->registerDictionary()->findOrThrow("ra")) {
    initializeDispatchTable();
    initializeMemoryState();
}

DispatcherMips::Ptr
DispatcherMips::instance(const Architecture::Base::ConstPtr &arch) {
    return Ptr(new DispatcherMips(arch));
}

DispatcherMips::Ptr
DispatcherMips::instance(const Architecture::Base::ConstPtr &arch, const RiscOperators::Ptr &ops) {
    return Ptr(new DispatcherMips(arch, ops));
}

BaseSemantics::Dispatcher::Ptr
DispatcherMips::create(const BaseSemantics::RiscOperators::Ptr &ops) const {
    return instance(architecture(), ops);
}

DispatcherMips::Ptr
DispatcherMips::promote(const BaseSemantics::Dispatcher::Ptr &d) {
    Ptr retval = boost::dynamic_pointer_cast<DispatcherMips>(d);
    ASSERT_not_null(retval);
    return retval;
}

int
DispatcherMips::iprocKey(SgAsmInstruction *insn_) const {
    SgAsmMipsInstruction *insn = isSgAsmMipsInstruction(insn_);
    ASSERT_not_null(insn);
    return insn->get_kind();
}

void
DispatcherMips::initializeMemoryState() {
    if (State::Ptr state = currentState()) {
        if (MemoryState::Ptr memory = state->memoryState()) {
            switch (memory->get_byteOrder()) {
                case ByteOrder::BE:
                case ByteOrder::EL:
                    break;
                case ByteOrder::ORDER_UNSPECIFIED:
                    memory->set_byteOrder(architecture()->byteOrder());
                    break;
            }
        }
    }
}

} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::DispatcherMips);
#endif

#endif
