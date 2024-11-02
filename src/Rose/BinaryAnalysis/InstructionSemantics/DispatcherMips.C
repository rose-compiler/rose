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

#include <SgAsmExpression.h>
#include <SgAsmBinaryAdd.h>
#include <SgAsmDirectRegisterExpression.h>
#include <SgAsmMemoryReferenceExpression.h>
#include <SgAsmIntegerValueExpression.h>
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
        SgAsmMipsInstruction* insn = isSgAsmMipsInstruction(insn_);
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

// Unconditional branch
// Branch and link (mips_bal, implemented by IP_b)
struct IP_b: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        const size_t nBits = d->architecture()->bitsPerWord();

        d->processDelaySlot(notnull(insn->get_delaySlot()));

        // Link instructions
        if (insn->get_kind() == mips_bal) {
            // Place the return address link (PC + 8) in GPR 31.
            SValue::Ptr link = ops->number_(nBits, insn->get_address());
            link = ops->add(link, ops->number_(nBits, 8));
            ops->writeRegister(d->REG_RA, link);
        }

        // Write new IP/PC
        SValue::Ptr target = d->read(args[0]);
        ops->writeRegister(d->instructionPointerRegister(), target);
    }
};

// Branch on equal
// Branch on not equal (mips_bne, implemented by IP_beq)
struct IP_beq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);

        d->processDelaySlot(notnull(insn->get_delaySlot()));

        // Calculate new IP value based on comparison of rs and rt registers
        SValue::Ptr compare;
        SValue::Ptr rs = d->read(args[0]);
        SValue::Ptr rt = d->read(args[1]);
        SValue::Ptr ipValue = ops->readRegister(d->instructionPointerRegister());

        switch (insn->get_kind()) {
          case mips_beq:
            compare = ops->isEqual(rs, rt);
            break;
          case mips_bne:
            compare = ops->isNotEqual(rs, rt);
            break;
          default:
            break;
        }

        // Write new IP/PC
        ipValue = ops->ite(compare, d->read(args[2]), ipValue);
        ops->writeRegister(d->instructionPointerRegister(), ipValue);
    }
};

// Branch on greater than or equal to zero (mips_bgez)
// Branch on greater than or equal to zero and link (mips_bgezal, implemented by IP_bgez)
// Branch on greater than zero (mips_bgtz, implemented by IP_bgez)
// Branch on less than or equal to zero (mips_blez, implemented by IP_bgez)
// Branch on less than zero (mips_bltz, implemented by IP_bgez)
// Branch on less than or equal to zero and link (mips_bltzal, implemented by IP_bgez)
struct IP_bgez: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nBits = d->architecture()->bitsPerWord();

        d->processDelaySlot(notnull(insn->get_delaySlot()));

        // Link instructions
        switch (insn->get_kind()) {
          case mips_bgezal:
          case mips_bltzal: {
            // Place the return address link (PC + 8) in GPR 31.
            SValue::Ptr link = ops->number_(nBits, insn->get_address());
            link = ops->add(link, ops->number_(nBits, 8));
            ops->writeRegister(d->REG_RA, link);
            break;
          }
          default:
            break;
        }

        // Calculate new IP value based on evaluation of rs register
        SValue::Ptr compare;
        SValue::Ptr rs = d->read(args[0]);
        SValue::Ptr ipValue = ops->readRegister(d->instructionPointerRegister());

        switch (insn->get_kind()) {
          case mips_bgez:
          case mips_bgezal:
            compare = ops->isSignedGreaterThanOrEqual(rs, ops->number_(nBits, 0));
            break;
          case mips_bgtz:
            compare = ops->isSignedGreaterThan(rs, ops->number_(nBits, 0));
            break;
          case mips_blez:
            compare = ops->isSignedLessThanOrEqual(rs, ops->number_(nBits, 0));
            break;
          case mips_bltz:
          case mips_bltzal:
            compare = ops->isSignedLessThan(rs, ops->number_(nBits, 0));
            break;
          default:
            break;
        }

        // Write new IP/PC
        ipValue = ops->ite(compare, d->read(args[1]), ipValue);
        ops->writeRegister(d->instructionPointerRegister(), ipValue);
    }
};

// Breakpoint
struct IP_break: P {
    void p(D, Ops ops, I insn, A args) {
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

// Execution hazard barrier
struct IP_ehb: P {
    void p(D, Ops, I insn, A args) {
        assert_args(insn, args, 0);
    }
};

// Jump
// Jump and link (mips_jal, implemented by IP_j)
// Jump and link exchange (mips_jalx, implemented by IP_j)
struct IP_j: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        const size_t nBits = d->architecture()->bitsPerWord();

        d->processDelaySlot(notnull(insn->get_delaySlot()));

        // Link instructions
        if (insn->get_kind() == mips_jal || insn->get_kind() == mips_jalx) {
            // Place the return address link (PC + 8) in GPR 31
            SValue::Ptr link = ops->number_(nBits, insn->get_address());
            link = ops->add(link, ops->number_(nBits, 8));
            ops->writeRegister(d->REG_RA, link);
        }

        if (insn->get_kind() == mips_jalx) {
            mlog[WARN] << "mips_jalx instruction needs to exchange ISAMode\n";
        }

        // Write target to IP/PC
        SValue::Ptr target = d->read(args[0]);
        ops->writeRegister(d->instructionPointerRegister(), target);
    }
};

// Jump and link register
// Jump and link register with hazard barrier (mips_jalr_hb, implemented by IP_jalr)
struct IP_jalr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        const size_t nBits = d->architecture()->bitsPerWord();

        if (insn->get_kind() == mips_jalr_hb) {
            mlog[WARN] << "ClearHazards() (need implementation) must be called before delay slot is executed\n";
        }

        d->processDelaySlot(notnull(insn->get_delaySlot()));

        // Place the return address link (PC + 8) in link register
        if (auto dre = isSgAsmDirectRegisterExpression(args[0])) {
            // For pre-release 6 rd can be zero, if so use $ra for return address link
            RegisterDescriptor reg = dre->get_descriptor();
            if (reg.majorNumber() == 0 && reg.minorNumber() == 0) {
                reg = d->REG_RA;
            }
            SValue::Ptr link = ops->number_(nBits, insn->get_address());
            link = ops->add(link, ops->number_(nBits, 8));
            ops->writeRegister(reg, link);
        }

        // R3 "hint" field must be zero for Release1

        // Write new IP/PC
        SValue::Ptr temp = d->read(args[1]);
        ops->writeRegister(d->instructionPointerRegister(), temp);
    }
};

// Jump register
// Jump register with hazard barrier (mips_jr_hb, implemented by IP_jr)
struct IP_jr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);

        mlog[WARN] << "both mips_jr and mips_jr_hb instructions need to set ISAMode\n";
        if (insn->get_kind() == mips_jr_hb) {
            mlog[WARN] << "ClearHazards() (need implementation) must be called before delay slot is executed\n";
        }

        d->processDelaySlot(notnull(insn->get_delaySlot()));

        // R3 "hint" field must be zero for Release1

        // Write new IP/PC from address in register
        SValue::Ptr temp = d->read(args[0]);
        ops->writeRegister(d->instructionPointerRegister(), temp);
    }
};

// Load byte
// Load byte EVA (mips_lbe, implemented by IP_lb)
struct IP_lb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr result = ops->signExtend(d->read(args[1]), nBits);
        d->write(args[0], result);
    }
};

// Load byte unsigned (mips_lbu)
// Load byte unsigned EVA (mips_lbue, implemented by IP_lbu)
struct IP_lbu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nBits = d->architecture()->bitsPerWord();
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

// Load halfword (mips_lh, implemented by IP_lb)
// Load halfword EVA (mips_lhe, implemented by IP_lb)

// Load halfword unsigned (mips_lhu, implemented by IP_lbu)
// Load halfword unsigned EVA (mips_lhue, implemented by IP_lbu)

// Load upper immediate (mips_lui)
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

// Load word (mips_lw, implemented by IP_lb)

// Load word to floating point
struct IP_lwc1: P {
    void p(D d, Ops, I insn, A args) {
        assert_args(insn, args, 2);
        // TODO: load into low word of FPR ft
        d->write(args[0], d->read(args[1]));
    }
};

// Load word EVA (mips_lwe, implemented by IP_lb)
// Load word unsigned (mips_lwu, implemented by IP_lbu)

// Load word left
struct IP_lwl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nBits = d->architecture()->bitsPerWord();

        SValue::Ptr rt = d->read(args[0]);
        SValue::Ptr mem = d->read(args[1]);
        SValue::Ptr merged = d->mergeRight(rt, mem);

        // sign-extend and write result
        d->write(args[0], ops->signExtend(merged, nBits));
    }
};

// Load word right
struct IP_lwr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nBits = d->architecture()->bitsPerWord();

        SValue::Ptr rt = d->read(args[0]);
        SValue::Ptr mem = d->read(args[1]);
        SValue::Ptr merged = d->mergeRight(rt, mem);

        // sign-extend and write result
        d->write(args[0], ops->signExtend(merged, nBits));
    }
};

// Load word unsigned (mips_lwu, implemented by IP_lbu)

// Load word indexed to floating point
// TODO: Load doubleword indexed to floating point (reuse functions, is that what this is on?)
struct IP_lwxc1: P {
    void p(D d, Ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr word = d->read(args[1]);
        // TODO: loaded to low word in FPR fd
        // TODO: AddressError if EffectiveAddress[1..0] != 0
        d->write(args[0], word);
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

// Move word from coprocessor 0
struct IP_mfc0: P {
    void p(D d, Ops ops, I insn, A args) {
        // TODO: Is sel field an arg?
        //       In anycase, not finding instruction
        assert_args(insn, args, 3);
        size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr result = ops->signExtend(d->read(args[1]), nBits);
        d->write(args[0], result);
    }
};

// Move word from floating point
struct IP_mfc1: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr result = ops->signExtend(d->read(args[1]), nBits);
        d->write(args[0], result);
    }
};

// Move word from coprocessor 2
struct IP_mfc2: P {
    void p(D d, Ops ops, I insn, A args) {
        // TODO: Memory has to be read from co-proc-2 register args[1]
        //       Probably need to change disassembler
        assert_args(insn, args, 2);
        size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr result = ops->signExtend(d->read(args[1]), nBits);
        d->write(args[0], result);
    }
};

// Move word from hi register
// Move word to hi register (mips_mthi, implemented by IP_mfhi)
// Note: removed in release 6
struct IP_mfhi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_kind() == mips_mfhi) {
            d->write(args[0], ops->readRegister(d->REG_HI));
        }
        else if (insn->get_kind() == mips_mthi) {
            ops->writeRegister(d->REG_HI, d->read(args[0]));
        }
    }
};

// Move word from lo register
// Move word to lo register (mips_mtlo, implemented by IP_mflo)
// Note: removed in release 6
struct IP_mflo: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_kind() == mips_mflo) {
            d->write(args[0], ops->readRegister(d->REG_LO));
        }
        else if (insn->get_kind() == mips_mtlo) {
            ops->writeRegister(d->REG_LO, d->read(args[0]));
        }
    }
};

// Floating point move (single)
struct IP_mov_s: P {
    void p(D d, Ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr fs = d->read(args[1]);
        d->write(args[0], fs);
    }
};

// Floating point move (double)
struct IP_mov_d: P {
    void p(D d, Ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr fs = d->read(args[1]);
        d->write(args[0], fs);
    }
};

// Floating point move (paired-single)
// TODO: In paired-single format, both the halves of the pair are copied to fd
// Note: removed in release 6
struct IP_mov_ps: P {
    void p(D d, Ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr fs = d->read(args[1]);
        d->write(args[0], fs);
    }
};

// Move conditional on floating point false
// Move conditional on floating point true (mips_movt, implemented by IP_movf)
// Note: removed in release 6
struct IP_movf: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr rs = d->read(args[1]);
        SValue::Ptr cc = d->read(args[2]);
        SValue::Ptr oldRd = d->read(args[0]);

        // Predicate for the result is based on instruction kind
        SValue::Ptr result;
        if (insn->get_kind() == mips_movf) {
            result = ops->ite(ops->equalToZero(cc), rs, oldRd);
        }
        else if (insn->get_kind() == mips_movt) {
            result = ops->ite(ops->equalToZero(cc), oldRd, rs);
        }
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

// Move conditional on not zero
// Move conditional on zero (mips_movz, implemented by IP_movn)
// Note: removed in release 6
struct IP_movn: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr rt = d->read(args[2]);
        SValue::Ptr rs = d->read(args[1]);
        SValue::Ptr oldRd = d->read(args[0]);

        // Predicate for the result is based on instruction kind
        SValue::Ptr result;
        if (insn->get_kind() == mips_movz) {
            result = ops->ite(ops->equalToZero(rt), rs, oldRd);
        }
        else if (insn->get_kind() == mips_movn) {
            result = ops->ite(ops->equalToZero(rt), oldRd, rs);
        }
        d->write(args[0], result);
    }
};

// Move conditional on floating point true (mips_movt, implemented by IP_movf)
// Note: removed in release 6

// Move conditional on zero (mips_movz, implemented by IP_movn)
// Note: removed in release 6

// Floating point move conditional on zero
// TODO:
//struct IP_movz.fmt: P {

// Multiply and subtract word to hi, lo (mips_msub, implemented by IP_msub_su)
// Multiply and subtract unsigned word to hi, lo (mips_msubu, implemented by IP_msub_su)
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

// Move word to hi register (mips_mthi, implemented by IP_mfhi)
// Note: removed in release 6

// Move word to lo register (mips_mtlo, implemented by IP_mflo)
// Note: removed in release 6

// Multiply word to GPR
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
// Multiply unsigned word (mips_multu)
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
    void p(D, Ops, I insn, A args) {
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

// Read hardware register
struct IP_rdhwr: P {
    void p(D d, Ops /*ops*/, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr hwr = d->read(args[1]);
        d->write(args[0], hwr);
    }
};

// ROTR  -- Rotate word right (mips_rotr, implemented by IP_rotate)
// ROTRV -- Rotate word right variable (mips_rotrv, implemented by IP_rotate)
struct IP_rotate: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        const size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr low32rt = ops->extract(d->read(args[1]), 0, 32);

        // Rotate right
        SValue::Ptr result{};
        if (insn->get_kind() == mips_rotr) {
          SValue::Ptr sa = d->read(args[2]);
          result = ops->signExtend(ops->rotateRight(low32rt, sa), nBits);
        }
        else if (insn->get_kind() == mips_rotrv) {
          SValue::Ptr low5sa = ops->extract(d->read(args[2]), 0, 5);
          result = ops->signExtend(ops->rotateRight(low32rt, low5sa), nBits);
        }

        // Write rotate results
        d->write(args[0], result);
    }
};

// Store byte (mips_sb, implemented by IP_store)
// Store byte EVA (mips_sbe, implemented by IP_store)

struct IP_store: P {
    void p(D d, Ops, I insn, A args) {
        assert_args(insn, args, 2);

        // Calculate element size
        size_t nBits{0};
        switch (insn->get_kind()) {
            case mips_sb:
            case mips_sbe:
                nBits = 8;
                break;
            case mips_sdc1:
                nBits = 64;
                break;
            case mips_sh:
            case mips_she:
                nBits = 16;
                break;
            case mips_sw:
            case mips_swc1:
            case mips_swc2:
            case mips_swe:
            case mips_swxc1:
                nBits = 32;
                break;
            case mips_suxc1:
                nBits = 64;
                break;
            default:
                ASSERT_not_reachable("instruction not handled");
        }

        // Read and store result
        SValue::Ptr result = d->read(args[0], nBits);
        d->write(args[1], result);
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

// Store halfword (mips_sh, implemented by IP_store)
// Store halfword EVA (mips_she, implemented by IP_store)

// SLL -- Shift word left logical (mips_sll, implemented by IP_shift)
struct IP_shift: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        const size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr sa = d->read(args[2]);
        SValue::Ptr low32rt = ops->extract(d->read(args[1]), 0, 32);

        // Shift left (or right) logical
        SValue::Ptr result{};
        if (insn->get_kind() == mips_sll) {
          result = ops->signExtend(ops->shiftLeft(low32rt, sa), nBits);
        }
        else if (insn->get_kind() == mips_srl) {
          result = ops->signExtend(ops->shiftRight(low32rt, sa), nBits);
        }
        else if (insn->get_kind() == mips_sra) {
          result = ops->signExtend(ops->shiftRightArithmetic(low32rt, sa), nBits);
        }

        // Write shift results
        d->write(args[0], result);
    }
};

// SLLV -- Shift word left logical variable (mips_sllv, implemented by IP_shiftv)
struct IP_shiftv: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        const size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr low5sa = ops->extract(d->read(args[2]), 0, 5);
        SValue::Ptr low32rt = ops->extract(d->read(args[1]), 0, 32);

        // Shift left (or right) logical variable
        SValue::Ptr result{};
        if (insn->get_kind() == mips_sllv) {
          result = ops->signExtend(ops->shiftLeft(low32rt, low5sa), nBits);
        }
        else if (insn->get_kind() == mips_srlv) {
          result = ops->signExtend(ops->shiftRight(low32rt, low5sa), nBits);
        }
        else if (insn->get_kind() == mips_srav) {
          result = ops->signExtend(ops->shiftRightArithmetic(low32rt, low5sa), nBits);
        }

        // Write shift results
        d->write(args[0], result);
    }
};

// Set on less than (mips_slt, implemented by IP_slt_su)
// Set on less than unsigned (mips_sltu, implemented by IP_slt_su)
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

// Set on less than immediate (mips_slti, implemented by IP_slti_su)
// Set on less than immediate unsigned (mips_sltiu, implemented by IP_slti_su))
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

// SRA  -- Shift word right arithmetic (mips_sra, implemented by IP_shift)
// SRAV -- Shift word right arithmetic variable (mips_srav, implemented by IP_shiftv)

// SRL  -- Shift word right logical (mips_srl, implemented by IP_shift)
// SRLV -- Shift word right logical variable (mips_srlv, implemented by IP_shiftv)

// Superscalar no operation (mips_ssnop, implemented by IP_nop)

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

// Store doubleword from floating point (mips_sdc1, implemented by IP_store)
// Store doubleword indexed unaligned from floating point (mips_suxc1, implemented by IP_store)

// Store word (mips_sw, implemented by IP_store)
// Store word from floating point (mips_swc1, implemented by IP_store)
// Store word from coprocessor 2 (mips_swc2, implemented by IP_store)
// Store word EVA (mips_swe, implemented by IP_store)
// Store word indexed from floating point (mips_swxc1, implemented by IP_store)

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
    iprocSet(mips_b,     new Mips::IP_b);
    iprocSet(mips_bal,   new Mips::IP_b);     // mips_bal shares common implementation mips_b
    iprocSet(mips_beq,   new Mips::IP_beq);
    iprocSet(mips_bgez,  new Mips::IP_bgez);
    iprocSet(mips_bgezal,new Mips::IP_bgez);  // mips_bgezal shares common implementation mips_bgez
    iprocSet(mips_bgtz,  new Mips::IP_bgez);  // mips_bgtz   shares common implementation mips_bgez
    iprocSet(mips_blez,  new Mips::IP_bgez);  // mips_blez   shares common implementation mips_bgez
    iprocSet(mips_bltz,  new Mips::IP_bgez);  // mips_bltz   shares common implementation mips_bgez
    iprocSet(mips_bltzal,new Mips::IP_bgez);  // mips_bltzal shares common implementation mips_bgez
    iprocSet(mips_bne,   new Mips::IP_beq);   // mips_bne    shares common implementation mips_beq
    iprocSet(mips_break, new Mips::IP_break);
    iprocSet(mips_clo,   new Mips::IP_clo);
    iprocSet(mips_clz,   new Mips::IP_clz);
    iprocSet(mips_div,   new Mips::IP_div);
    iprocSet(mips_divu,  new Mips::IP_divu);
    iprocSet(mips_ehb,   new Mips::IP_ehb);
    iprocSet(mips_j,     new Mips::IP_j);
    iprocSet(mips_jal,   new Mips::IP_j);   // mips_jal shares common implementation mips_j
    iprocSet(mips_jalr,  new Mips::IP_jalr);
    iprocSet(mips_jalr_hb,new Mips::IP_jalr); // mips_jalr_hb shares common implementation mips_jalr
    iprocSet(mips_jalx,  new Mips::IP_j);   // mips_jalx shares common implementation mips_j
    iprocSet(mips_jr,    new Mips::IP_jr);
    iprocSet(mips_jr_hb, new Mips::IP_jr);  // mips_jr_hb shares common implementation mips_jr
    iprocSet(mips_lb,    new Mips::IP_lb);
    iprocSet(mips_lbe,   new Mips::IP_lb);  // mips_lbe shares common implementation mips_lb
    iprocSet(mips_lbu,   new Mips::IP_lbu);
    iprocSet(mips_lbue,  new Mips::IP_lbu); // mips_lbue shares common implementation mips_lbu
    iprocSet(mips_lh,    new Mips::IP_lb);  // mips_lh   shares common implementation mips_lb
    iprocSet(mips_lhe,   new Mips::IP_lb);  // mips_lhe  shares common implementation mips_lb
    iprocSet(mips_lhu,   new Mips::IP_lbu); // mips_lhu  shares common implementation mips_lbu
    iprocSet(mips_lhue,  new Mips::IP_lbu); // mips_lhue shares common implementation mips_lbu
    iprocSet(mips_lui,   new Mips::IP_lui);
    iprocSet(mips_lw,    new Mips::IP_lb);  // mips_lw shares common implementation mips_lb
    iprocSet(mips_lwc1,  new Mips::IP_lwc1);
    iprocSet(mips_lwe,   new Mips::IP_lb);  // mips_lwe shares common implementation mips_lb
    iprocSet(mips_lwl,   new Mips::IP_lwl);
    iprocSet(mips_lwr,   new Mips::IP_lwr);
//  iprocSet(mips_lwu,   new Mips::IP_lwu); // mips_lwu (Release 6) not implemented in Mips.C
    iprocSet(mips_madd,  new Mips::IP_madd);
    iprocSet(mips_maddu, new Mips::IP_maddu);
    iprocSet(mips_mfc0,  new Mips::IP_mfc0);
    iprocSet(mips_mfc1,  new Mips::IP_mfc1);
    iprocSet(mips_mfc2,  new Mips::IP_mfc2);
    iprocSet(mips_mfhi,  new Mips::IP_mfhi);
    iprocSet(mips_mflo,  new Mips::IP_mflo);
    iprocSet(mips_mov_s, new Mips::IP_mov_s);
    iprocSet(mips_mov_d, new Mips::IP_mov_d);
    iprocSet(mips_mov_ps,new Mips::IP_mov_ps);

    iprocSet(mips_movf,    new Mips::IP_movf);
    iprocSet(mips_movf_s,  new Mips::IP_movf_s);
    iprocSet(mips_movf_d,  new Mips::IP_movf_d);
    iprocSet(mips_movf_ps, new Mips::IP_movf_ps);

    iprocSet(mips_movn,    new Mips::IP_movn);
    iprocSet(mips_movt,    new Mips::IP_movf);  // mips_movt  shares common implementation IP_movf
    iprocSet(mips_movz,    new Mips::IP_movn);  // mips_movz  shares common implementation IP_movn

    iprocSet(mips_msub,  new Mips::IP_msub_su); // mips_msub  shares common implementation IP_msub_su
    iprocSet(mips_msubu, new Mips::IP_msub_su); // mips_msubu shares common implementation IP_msub_su

    iprocSet(mips_mthi,  new Mips::IP_mfhi);    // mips_mthi shares common implementation IP_mfhi
    iprocSet(mips_mtlo,  new Mips::IP_mflo);    // mips_mtlo shares common implementation IP_mflo

    iprocSet(mips_mul,   new Mips::IP_mul);
    iprocSet(mips_mult,  new Mips::IP_mult_su); // mips_mult  shares common implementation IP_mult_su
    iprocSet(mips_multu, new Mips::IP_mult_su); // mips_multu shares common implementation IP_mult_su
    iprocSet(mips_nop,   new Mips::IP_nop);
    iprocSet(mips_nor,   new Mips::IP_nor);
    iprocSet(mips_or,    new Mips::IP_or);
    iprocSet(mips_ori,   new Mips::IP_ori);
    iprocSet(mips_rdhwr, new Mips::IP_rdhwr);
    iprocSet(mips_rotr,  new Mips::IP_rotate);  // mips_rotr  shares common implementation IP_rotate
    iprocSet(mips_rotrv, new Mips::IP_rotate);  // mips_rotrv shares common implementation IP_rotate
    iprocSet(mips_sb,    new Mips::IP_store);   // mips_sb    shares common implementation IP_store
    iprocSet(mips_sbe,   new Mips::IP_store);   // mips_sbe   shares common implementation IP_store
    iprocSet(mips_sdc1,  new Mips::IP_store);   // mips_sdc1  shares common implementation IP_store
    iprocSet(mips_seb,   new Mips::IP_seb);
    iprocSet(mips_seh,   new Mips::IP_seh);
    iprocSet(mips_sh,    new Mips::IP_store);   // mips_sh    shares common implementation IP_store
    iprocSet(mips_she,   new Mips::IP_store);   // mips_she   shares common implementation IP_store
    iprocSet(mips_sll,   new Mips::IP_shift);   // mips_sll   shares common implementation IP_shift
    iprocSet(mips_sllv,  new Mips::IP_shiftv);  // mips_sllv  shares common implementation IP_shiftv
    iprocSet(mips_sra,   new Mips::IP_shift);   // mips_sra   shares common implementation IP_shift
    iprocSet(mips_srav,  new Mips::IP_shiftv);  // mips_srav  shares common implementation IP_shiftv
    iprocSet(mips_srl,   new Mips::IP_shift);   // mips_srl   shares common implementation IP_shift
    iprocSet(mips_srlv,  new Mips::IP_shiftv);  // mips_srlv  shares common implementation IP_shiftv
    iprocSet(mips_slt,   new Mips::IP_slt_su);  // mips_slt   shares common implementation IP_slt_su
    iprocSet(mips_sltu,  new Mips::IP_slt_su);  // mips_sltu  shares common implementation IP_slt_su
    iprocSet(mips_slti,  new Mips::IP_slti_su); // mips_slti  shares common implementation IP_slti_su
    iprocSet(mips_sltiu, new Mips::IP_slti_su); // mips_sltiu shares common implementation IP_slti_su
    iprocSet(mips_ssnop, new Mips::IP_nop);     // mips_ssnop shares common implementation IP_nop

    iprocSet(mips_sub,   new Mips::IP_sub);
    iprocSet(mips_subu,  new Mips::IP_subu);

    iprocSet(mips_suxc1, new Mips::IP_store);   // mips_suxc1 shares common implementation IP_store
    iprocSet(mips_sw,    new Mips::IP_store);   // mips_sw    shares common implementation IP_store
    iprocSet(mips_swc1,  new Mips::IP_store);   // mips_swc1  shares common implementation IP_store
    iprocSet(mips_swc2,  new Mips::IP_store);   // mips_swc2  shares common implementation IP_store
    iprocSet(mips_swe,   new Mips::IP_store);   // mips_swe   shares common implementation IP_store
    iprocSet(mips_swxc1, new Mips::IP_store);   // mips_swxc1 shares common implementation IP_store

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
      REG_RA(arch->registerDictionary()->findOrThrow("ra")),
      REG_ZERO(arch->registerDictionary()->findOrThrow("zero")) {
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

// Merge unaligned memory left
SValuePtr
DispatcherMips::mergeLeft(SValuePtr reg, SValuePtr mem) {
    size_t nBits = architecture()->bitsPerWord();
    RiscOperators::Ptr ops = operators();

    // Determine the effective address of memory access [GPR[base] + offset]
    auto mipsI = isSgAsmMipsInstruction(currentInstruction());
    ASSERT_not_null(mipsI);
    ASSERT_require(mipsI->nOperands() > 1);
    auto memRef = isSgAsmMemoryReferenceExpression(mipsI->operand(1));
    ASSERT_not_null(memRef);

    SValue::Ptr effAddr = effectiveAddress(memRef, nBits);

    // Calculate alignment offset (first two bits of memory address)
    SValue::Ptr take1, take2, take3;
    SValue::Ptr offset = ops->extract(effAddr, 0, 2); // bits 0..1

    // See Figure 5.2, Bytes Loaded by LWL Instruction
    if (ByteOrder::BE == architecture()->byteOrder()) {
        take1 = ops->isEqual(ops->number_(2,3), offset);
        take2 = ops->isEqual(ops->number_(2,2), offset);
        take3 = ops->isEqual(ops->number_(2,1), offset);
    }
    else {
        take1 = ops->isEqual(ops->number_(2,0), offset);
        take2 = ops->isEqual(ops->number_(2,1), offset);
        take3 = ops->isEqual(ops->number_(2,2), offset);
    }

    // default (take 4), result is memory contents
    SValue::Ptr result = mem;

    // Note: all register bytes are from the least significant half, if 64-bit architecture

    // take1, word will contain last byte from memory | last three (least) bytes from register
    SValue::Ptr src = ops->extract(mem, 24, 32); // word is 4 bytes
    SValue::Ptr dst = ops->extract(reg, nBits-24, nBits); // register is nBits
    SValue::Ptr word = ops->concatHiLo(src, dst);
    result = ops->ite(take1, word, result);

    // take2, word will contain last two bytes from memory | last two (least) bytes from register
    src = ops->extract(mem, 16, 32); // word is 4 bytes
    dst = ops->extract(reg, nBits-16, nBits); // register is nBits
    word = ops->concatHiLo(src, dst);
    result = ops->ite(take2, word, result);

    // take3, word will contain last three bytes from memory | last (least) byte from register
    src = ops->extract(mem, 8, 32); // word is 4 bytes
    dst = ops->extract(reg, nBits-8, nBits); // register is nBits
    word = ops->concatHiLo(src, dst);
    result = ops->ite(take3, word, result);

    return result;
}

// Merge unaligned memory right
SValuePtr
DispatcherMips::mergeRight(SValuePtr reg, SValuePtr mem) {
    size_t nBits = architecture()->bitsPerWord();
    RiscOperators::Ptr ops = operators();

    // Determine the effective address of memory access [GPR[base] + offset]
    auto mipsI = isSgAsmMipsInstruction(currentInstruction());
    ASSERT_not_null(mipsI);
    ASSERT_require(mipsI->nOperands() > 1);
    auto memRef = isSgAsmMemoryReferenceExpression(mipsI->operand(1));
    ASSERT_not_null(memRef);

    SValue::Ptr effAddr = effectiveAddress(memRef, nBits);

    // Calculate alignment offset (first two bits of memory address)
    SValue::Ptr take1, take2, take3;
    SValue::Ptr offset = ops->extract(effAddr, 0, 2); // bits 0..1

    // See Figure 5.8, Bytes Loaded by LWR Instruction
    if (ByteOrder::BE == architecture()->byteOrder()) {
        take1 = ops->isEqual(ops->number_(2,0), offset);
        take2 = ops->isEqual(ops->number_(2,1), offset);
        take3 = ops->isEqual(ops->number_(2,2), offset);
    }
    else {
        take1 = ops->isEqual(ops->number_(2,3), offset);
        take2 = ops->isEqual(ops->number_(2,2), offset);
        take3 = ops->isEqual(ops->number_(2,1), offset);
    }

    // default (take 4), result is memory contents
    SValue::Ptr result = mem;

    // Note: all register bytes are from the least significant half, if 64-bit arch

    // take1, word will contain first byte from memory | first three (least) bytes from register
    SValue::Ptr src = ops->extract(mem, 0, 8); // word is 4 bytes
    SValue::Ptr dst = ops->extract(reg, nBits-24, nBits); // register is nBits
    SValue::Ptr word = ops->concatHiLo(dst, src);
    result = ops->ite(take1, word, result);

    // take2, word will contain first two bytes from memory | first two (least) bytes from register
    src = ops->extract(mem, 0, 16); // word is 4 bytes
    dst = ops->extract(reg, nBits-16, nBits); // register is nBits
    word = ops->concatHiLo(dst, src);
    result = ops->ite(take2, word, result);

    // take3, word will contain first three bytes from memory | first byte (least) from register
    src = ops->extract(mem, 0, 24); // word is 4 bytes
    dst = ops->extract(reg, nBits-8, nBits); // register is nBits
    word = ops->concatHiLo(dst, src);
    result = ops->ite(take3, word, result);

    return result;
}

// Override Dispatcher::read so that if we read from the "zero" register we get zero.
SValue::Ptr
DispatcherMips::read(SgAsmExpression *e, const size_t valueNBits/*or zero*/, const size_t addrNBits/*or zero*/) {
    ASSERT_not_null(e);
    SValue::Ptr retval;
    if (SgAsmDirectRegisterExpression *re = isSgAsmDirectRegisterExpression(e)) {
        if (re->get_descriptor() == REG_ZERO)
            return operators()->number_(valueNBits ? valueNBits : REG_ZERO.nBits(), 0);
    }
    return Dispatcher::read(e, valueNBits, addrNBits);
}

} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::DispatcherMips);
#endif

#endif
