#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherAarch32.h>

#include <Rose/As.h>
#include <Rose/AST/Traversal.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/InstructionEnumsAarch32.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/Utility.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

#include <SgAsmAarch32Instruction.h>
#include <SgAsmBinaryPostupdate.h>
#include <SgAsmBinaryPreupdate.h>
#include <SgAsmDirectRegisterExpression.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmMemoryReferenceExpression.h>
#include <SgAsmOperandList.h>
#include <SgAsmRegisterNames.h>
#include <SgAsmType.h>

#include <Cxx_GrammarDowncast.h>
#include <sageContainer.h>

using namespace Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functors that handle individual A32/T32 instruction kinds
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Aarch32 {

// Base class for processing each instruction kind.
class P: public BaseSemantics::InsnProcessor {
public:
    using D = DispatcherAarch32*;
    using Ops = BaseSemantics::RiscOperators*;
    using I = SgAsmAarch32Instruction*;
    using A = const SgAsmExpressionPtrList&;
    using V = const BaseSemantics::SValue::Ptr&;

    virtual void p(D, Ops, I, A, V) = 0;

    virtual void process(const BaseSemantics::Dispatcher::Ptr &dispatcher_, SgAsmInstruction *insn_) override {
        DispatcherAarch32Ptr dispatcher = DispatcherAarch32::promote(dispatcher_);
        BaseSemantics::RiscOperators::Ptr operators = dispatcher->operators();
        SgAsmAarch32Instruction *insn = isSgAsmAarch32Instruction(insn_);
        ASSERT_not_null(insn);
        ASSERT_require(insn == operators->currentInstruction());
        dispatcher->advanceInstructionPointer(insn);    // branch instructions will reassign

        operators->comment("thumb mode?");
        dispatcher->setThumbMode(insn);

        SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
        SValue::Ptr enabled = dispatcher->conditionHolds(insn->get_condition());

        operators->comment("operand pre-updates");
        for (size_t i = 0; i < operands.size(); ++i)
            dispatcher->preUpdate(operands[i], enabled);

        operators->comment("executing instruction core");
        p(dispatcher.get(), operators.get(), insn, operands, enabled);

        operators->comment("operand post-updates");
        for (size_t i = 0; i < operands.size(); ++i)
            dispatcher->postUpdate(operands[i], enabled);
    }

    void assert_args(I insn, A args, size_t nargs) {
        ASSERT_not_null(insn);
        Address alignment = insn->get_size();
        if (alignDown(insn->get_address(), alignment) != insn->get_address()) {
            std::string mesg = "instruction is not " + boost::lexical_cast<std::string>(alignment) + "-byte aligned";
            throw BaseSemantics::Exception(mesg, insn);
        }
        if (args.size() != nargs) {
            std::string mesg = "instruction must have " + StringUtility::plural(nargs, "arguments");
            throw BaseSemantics::Exception(mesg, insn);
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NOTE: Almost all "auto" typed variables below are Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::SValue::Ptr.
//
// NOTE: As with all binary analysis that has the potential to use a symbolic domain, we cannot have more than one value
// constructed in any given function argument list. This is for testing purposes. It's because the order that function
// arguments are evaluated in C++ is unspecified, and therefore we cannot have more than one argument that has side
// effects. With the symbolic domain (and possibly others) the a side effect of creating a variable is that the counter used to
// create unique names is incremented. We get around this by using lots of otherwise unecessary local variables.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Add with carry
struct IP_adc: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr c = ops->readRegister(d->REG_PSTATE_C);

        auto sumAndFlags = d->addWithCarry(a, b, c);
        SValue::Ptr result = std::get<0>(sumAndFlags);
        SValue::Ptr nzcv = std::get<1>(sumAndFlags);

        if (d->isIpRegister(args[0])) {
            if (insn->get_updatesFlags()) {
                d->aluExceptionReturn(enabled, result);
            } else {
                d->aluWritePc(enabled, result);
            }
        } else {
            d->maybeWrite(enabled, args[0], result);
            if (insn->get_updatesFlags())
                d->maybeWriteRegister(enabled, d->REG_PSTATE_NZCV, nzcv);
        }
    }
};

// Add
struct IP_add: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);

        auto sumAndFlags = d->addWithCarry(a, b, ops->boolean_(false));
        SValue::Ptr result = std::get<0>(sumAndFlags);
        SValue::Ptr nzcv = std::get<1>(sumAndFlags);

        if (d->isIpRegister(args[0])) {
            if (insn->get_updatesFlags()) {
                d->aluExceptionReturn(enabled, result);
            } else {
                d->aluWritePc(enabled, result);
            }
        } else {
            d->maybeWrite(enabled, args[0], result);
            if (insn->get_updatesFlags())
                d->maybeWriteRegister(enabled, d->REG_PSTATE_NZCV, nzcv);
        }
    }
};

// Bitwise AND
struct IP_and: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);

        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr result = ops->and_(a, b);
#if 1 // FIXME[Robb Matzke 2021-02-05]
        SValue::Ptr carry = ops->undefined_(1);           // somehow computed from b
#endif

        if (d->isIpRegister(args[0])) {
            if (insn->get_updatesFlags()) {
                d->aluExceptionReturn(enabled, result);
            } else {
                d->aluWritePc(enabled, result);
            }
        } else {
            d->maybeWrite(enabled, args[0], result);
            if (insn->get_updatesFlags()) {
                d->maybeWriteRegister(enabled, d->REG_PSTATE_N, d->signBit(result));
                d->maybeWriteRegister(enabled, d->REG_PSTATE_Z, ops->equalToZero(result));
                d->maybeWriteRegister(enabled, d->REG_PSTATE_C, carry);
                // PSTATE_V is unchanged
            }
        }
    }
};

// Branch
struct IP_b: P {
    void p(D d, Ops, I insn, A args, V enabled) {
        assert_args(insn, args, 1);
        SValue::Ptr targetVa = d->read(args[0], 32);

        // Docs say BranchWritePC(PC + imm32) where PC is the address of the current 4-byte instruction plus 8. But apparently
        // the Capstone decoder has already done the addition.
        d->branchWritePc(enabled, targetVa, DispatcherAarch32::BranchType::DIR);
    }
};

// Bitwise bit clear
struct IP_bic: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr notB = ops->invert(b);
        SValue::Ptr result = ops->and_(a, notB);
#if 1 // FIXME[Robb Matzke 2021-02-05]
        SValue::Ptr carry = ops->undefined_(1);           // somehow computed from b
#endif

        if (d->isIpRegister(args[0])) {
            if (insn->get_updatesFlags()) {
                d->aluExceptionReturn(enabled, result);
            } else {
                d->aluWritePc(enabled, result);
            }
        } else {
            d->maybeWrite(enabled, args[0], result);
            if (insn->get_updatesFlags()) {
                d->maybeWriteRegister(enabled, d->REG_PSTATE_N, d->signBit(result));
                d->maybeWriteRegister(enabled, d->REG_PSTATE_Z, ops->equalToZero(result));
                d->maybeWriteRegister(enabled, d->REG_PSTATE_C, carry);
                // PSTATE_V is unchanged
            }
        }
    }
};

// Bit field clear
struct IP_bfc: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        ASSERT_require(isSgAsmIntegerValueExpression(args[1]));
        size_t lsb = isSgAsmIntegerValueExpression(args[1])->get_absoluteValue();
        ASSERT_require(isSgAsmIntegerValueExpression(args[2]));
        size_t width = isSgAsmIntegerValueExpression(args[2])->get_absoluteValue();
        SValue::Ptr newBits = ops->number_(width, 0);
        SValue::Ptr oldBits = d->read(args[0], 32);
        SValue::Ptr result;
        if (lsb > 0)
            result = ops->extract(oldBits, 0, lsb);
        result = result ? ops->concatHiLo(newBits, result) : newBits;
        if (lsb + width < 32)
            result = ops->concatHiLo(ops->extract(oldBits, lsb+width, 32), result);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Bit field insert
struct IP_bfi: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr a = d->read(args[1], 32);
        ASSERT_require(isSgAsmIntegerValueExpression(args[2]));
        size_t lsb = isSgAsmIntegerValueExpression(args[2])->get_absoluteValue();
        ASSERT_require(isSgAsmIntegerValueExpression(args[3]));
        size_t width = isSgAsmIntegerValueExpression(args[3])->get_absoluteValue();
        SValue::Ptr newBits = ops->extract(a, lsb, lsb+width);
        SValue::Ptr oldBits = d->read(args[0], 32);
        SValue::Ptr result;
        if (lsb > 0)
            result = ops->extract(oldBits, 0, lsb);
        result = result ? ops->concatHiLo(newBits, result) : newBits;
        if (lsb + width < 32)
            result = ops->concatHiLo(ops->extract(oldBits, lsb+width, 32), result);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Branch and link
struct IP_bl: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 1);
        SValue::Ptr label = d->read(args[0], 32);
        SValue::Ptr ifA32 = ops->and_(enabled, d->isA32Mode());
        SValue::Ptr ifT32 = ops->and_(enabled, d->isT32Mode());

        // Docs say LR = PC - 4, but also say "setting LR to the return address". In the docs, "PC" means the address of the
        // currently executing 4 byte instruction plus eight. I believe that the intended effect is that LR will point to the
        // beginning of the following instruction.
        SValue::Ptr fallThroughVa = ops->number_(32, insn->get_address() + insn->get_size());
        d->maybeWriteRegister(ifA32, d->REG_LR, fallThroughVa);

        // Docs say LR = PC<31:1>:1, but also say "setting LR to the return address". I'm not sure which is correct, especially since
        // the ARM documentation for the PC() functions says it's only called for A32 code.  I think the behavior is intended to be
        // that the value written to the LR register is one past the return address. I.e., it's the return address, which is normally
        // 2-byte aligned, but with the lsb set.
        SValue::Ptr fallThroughPlus1 = ops->number_(32, insn->get_address() + insn->get_size() + 1);
        d->maybeWriteRegister(ifT32, d->REG_LR, fallThroughPlus1);

        // The ARM documentation says that A32 addresses need to be realigned here, but I believe they must already be 4-byte aligned,
        // so I'm skipping this step.
        //
        // The ARM documentation says that the label argument needs to be added to the current PC value. However, the PC is already
        // known by the instruction, and furthermore, it appears that the Capstone decoder has already done the addition.
        SValue::Ptr targetVa = label;
        d->branchWritePc(enabled, targetVa, DispatcherAarch32::BranchType::DIRCALL);
    }
};

// Branch, link, and change from A32 to T32 or vice versa.
struct IP_blx: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 1);
        SValue::Ptr label = d->read(args[0], 32);
        SValue::Ptr ifA32 = ops->and_(enabled, d->isA32Mode());
        SValue::Ptr ifT32 = ops->and_(enabled, d->isT32Mode());

        // Docs say LR = PC - 4, but also say "setting LR to the return address". In the docs, "PC" means the address of the
        // currently executing 4 byte instruction plus eight. I believe that the intended effect is that LR will point to the
        // beginning of the following instruction.
        SValue::Ptr fallThroughVa = ops->number_(32, insn->get_address() + insn->get_size());
        d->maybeWriteRegister(ifA32, d->REG_LR, fallThroughVa);

        // Docs say LR = PC<31:1>:1, but also say "setting LR to the return address". I'm not sure which is correct, especially since
        // the ARM documentation for the PC() functions says it's only called for A32 code.  I think the behavior is intended to be
        // that the value written to the LR register is one past the return address. I.e., it's the return address, which is normally
        // 2-byte aligned, but with the lsb set.
        SValue::Ptr fallThroughPlus1 = ops->number_(32, insn->get_address() + insn->get_size() + 1);
        d->maybeWriteRegister(ifT32, d->REG_LR, fallThroughPlus1);

        // The ARM documentation says that A32 addresses need to be realigned here, but I believe they must already be 4-byte aligned,
        // so I'm skipping this step.
        //
        // The ARM documentation says that the label argument needs to be added to the current PC value. However, the PC is already
        // known by the instruction, and furthermore, it appears that the Capstone decoder has already done the addition.
        SValue::Ptr targetVa = label;
        d->setThumbMode(enabled, ops->invert(d->isT32Mode()));
        d->branchWritePc(enabled, targetVa, DispatcherAarch32::BranchType::DIRCALL);
    }
};

// Branch and exchange
struct IP_bx: P {
    void p(D d, Ops, I insn, A args, V enabled) {
        assert_args(insn, args, 1);
        SValue::Ptr result = d->read(args[0], 32);
        d->bxWritePc(enabled, result, DispatcherAarch32::BranchType::INDIR);
    }
};

// Count leading zeros
struct IP_clz: P {
    void p(D d, Ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr result = d->countLeadingZeroBits(a);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Compare negative
struct IP_cmn: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr a = d->read(args[0], 32);
        SValue::Ptr b = d->read(args[1], 32);

        auto sumAndFlags = d->addWithCarry(a, b, ops->boolean_(false));
        SValue::Ptr result = std::get<0>(sumAndFlags);
        SValue::Ptr nzcv = std::get<1>(sumAndFlags);

        d->maybeWriteRegister(enabled, d->REG_PSTATE_NZCV, nzcv);
    }
};

// Compare
struct IP_cmp: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr a = d->read(args[0]);
        SValue::Ptr b = d->read(args[1]);
        SValue::Ptr notB = ops->invert(b);
        SValue::Ptr nzcv = std::get<1>(d->addWithCarry(a, notB, ops->boolean_(true)));
        d->maybeWriteRegister(enabled, d->REG_PSTATE_NZCV, nzcv);
    }
};

// Change PE state
struct IP_cps: P {
    void p(D, Ops, I insn, A args, V) {
        assert_args(insn, args, 1);
        // CPS, CPSID, and CPSIE are treated as NOP if executed in USER mode.
    }
};

// Data synchronization barrier
struct IP_dsb: P {
    void p(D, Ops, I insn, A args, V) {
        assert_args(insn, args, 0);
    }
};

// Exclusive OR
struct IP_eor: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr result = ops->xor_(a, b);
#if 1 // FIXME[Robb Matzke 2021-02-05]
        SValue::Ptr carry = ops->undefined_(1);           // somehow computed from b
#endif
        if (d->isIpRegister(args[0])) {
            if (insn->get_updatesFlags()) {
                d->aluExceptionReturn(enabled, result);
            } else {
                d->aluWritePc(enabled, result);
            }
        } else {
            d->maybeWrite(enabled, args[0], result);
            if (insn->get_updatesFlags()) {
                d->maybeWriteRegister(enabled, d->REG_PSTATE_N, d->signBit(result));
                d->maybeWriteRegister(enabled, d->REG_PSTATE_Z, ops->equalToZero(result));
                d->maybeWriteRegister(enabled, d->REG_PSTATE_C, carry);
            }
        }
    }
};

// Hypervisor call
struct IP_hvc: P {
    void p(D d, Ops, I insn, A args, V enabled) {
        assert_args(insn, args, 1);
        SValue::Ptr value = d->read(args[0]);
        d->aarch32CallHypervisor(enabled, value);
    }
};

// Load data to system register
struct IP_ldc: P {
    void p(D d, Ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr value = d->read(args[2], 32);
        d->dbgdtrEl0(enabled, value);
    }
};

// Load multiple (increment after, full descending)
struct IP_ldm: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, std::max(args.size(), size_t(1)));

        // Get the memory address where the reads will start
        SValue::Ptr addr = d->read(args[0], 32);

        // Read registers if this instruction is enabled
        for (size_t i = 1; i < args.size(); ++i) {
            ASSERT_require(isSgAsmDirectRegisterExpression(args[i]));
            RegisterDescriptor reg = isSgAsmDirectRegisterExpression(args[i])->get_descriptor();
            SValue::Ptr dflt = ops->undefined_(32);
            SValue::Ptr value = ops->readMemory(RegisterDescriptor(), addr, dflt, ops->boolean_(true));
            if (reg == d->REG_PC) {
                d->loadWritePc(enabled, value);
            } else {
                d->maybeWriteRegister(enabled, reg, value);
            }
            addr = ops->add(addr, ops->number_(32, 4));
        }
    }
};

// Load multiple decrement after
struct IP_ldmda: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, std::max(args.size(), size_t(1)));

        // Get the memory address where the reads will start
        SValue::Ptr reg0 = d->read(args[0], 32);
        size_t nRegs = args.size() - 1;                 // number of registers to write
        SValue::Ptr nBytes = ops->number_(32, nRegs * 4 - 4);
        SValue::Ptr addr = ops->subtract(reg0, nBytes);   // starting address: addr = Rn - 4*N + 4

        // Read registers if this instruction is enabled
        for (size_t i = 1; i < args.size(); ++i) {
            ASSERT_require(isSgAsmDirectRegisterExpression(args[i]));
            RegisterDescriptor reg = isSgAsmDirectRegisterExpression(args[i])->get_descriptor();
            SValue::Ptr dflt = ops->undefined_(32);
            SValue::Ptr value = ops->readMemory(RegisterDescriptor(), addr, dflt, ops->boolean_(true));
            if (reg == d->REG_PC) {
                d->loadWritePc(enabled, value);
            } else {
                d->maybeWriteRegister(enabled, reg, value);
            }
            addr = ops->add(addr, ops->number_(32, 4));
        }
    }
};

// Load multiple decrement before
struct IP_ldmdb: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, std::max(args.size(), size_t(1)));

        // Get the memory address where the reads will start
        SValue::Ptr reg0 = d->read(args[0], 32);
        size_t nRegs = args.size() - 1;                 // number of registers to write
        SValue::Ptr nBytes = ops->number_(32, nRegs * 4);
        SValue::Ptr addr = ops->subtract(reg0, nBytes);   // starting address: addr = Rn - 4*N

        // Read registers if this instruction is enabled
        for (size_t i = 1; i < args.size(); ++i) {
            ASSERT_require(isSgAsmDirectRegisterExpression(args[i]));
            RegisterDescriptor reg = isSgAsmDirectRegisterExpression(args[i])->get_descriptor();
            SValue::Ptr dflt = ops->undefined_(32);
            SValue::Ptr value = ops->readMemory(RegisterDescriptor(), addr, dflt, ops->boolean_(true));
            if (reg == d->REG_PC) {
                d->loadWritePc(enabled, value);
            } else {
                d->maybeWriteRegister(enabled, reg, value);
            }
            addr = ops->add(addr, ops->number_(32, 4));
        }
    }
};

// Load multiple increment before
struct IP_ldmib: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, std::max(args.size(), size_t(1)));

        // Get the memory address where the reads will start
        SValue::Ptr reg0 = d->read(args[0], 32);
        SValue::Ptr addr = ops->add(reg0, ops->number_(32, 4)); // starting address: addr = Rn + 4

        // Read registers if this instruction is enabled
        for (size_t i = 1; i < args.size(); ++i) {
            ASSERT_require(isSgAsmDirectRegisterExpression(args[i]));
            RegisterDescriptor reg = isSgAsmDirectRegisterExpression(args[i])->get_descriptor();
            SValue::Ptr dflt = ops->undefined_(32);
            SValue::Ptr value = ops->readMemory(RegisterDescriptor(), addr, dflt, ops->boolean_(true));
            if (reg == d->REG_PC) {
                d->loadWritePc(enabled, value);
            } else {
                d->maybeWriteRegister(enabled, reg, value);
            }
            addr = ops->add(addr, ops->number_(32, 4));
        }
    }
};

// Load register
struct IP_ldr: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr readValue = d->read(args[1], 32);

        if (d->accessedRegister(args[0]) == d->REG_PC) {
            auto mre = isSgAsmMemoryReferenceExpression(args[1]);
            ASSERT_not_null(mre);
            SValue::Ptr addr = d->read(mre->get_address(), 32);
            SValue::Ptr isAligned = ops->equalToZero(ops->extract(addr, 0, 2));
            SValue::Ptr isNotAligned = ops->invert(isAligned);
            SValue::Ptr unknown = ops->undefined_(32);
            // if aligned, write to the PC, otherwise load the PC with an undefined value
            d->loadWritePc(ops->and_(enabled, isAligned), readValue);
            d->maybeWriteRegister(ops->and_(enabled, isNotAligned), d->REG_PC, unknown);
        } else {
            d->maybeWrite(enabled, args[0], readValue);
        }
    }
};

// Load register byte
struct IP_ldrb: P {
    void p(D d, Ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr readValue = d->read(args[1], 32);
        d->maybeWrite(enabled, args[0], readValue);
    }
};

// Load register byte unprivileged
struct IP_ldrbt: P {
    void p(D d, Ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr readValue = d->read(args[1], 32);
        d->maybeWrite(enabled, args[0], readValue);
    }
};

// Load register dual
struct IP_ldrd: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr addr = d->effectiveAddress(args[2]);
        SValue::Ptr addrPlus4 = ops->add(addr, ops->number_(32, 4));
        SValue::Ptr isAligned = ops->equalToZero(ops->extract(addr, 0, 3));

        // Handle the 64-bit read, big- and little-endian
        SValue::Ptr isBigEndian = d->bigEndian();
        SValue::Ptr dflt64 = ops->undefined_(64);
        SValue::Ptr chunk64 = ops->readMemory(RegisterDescriptor(), addr, dflt64, ops->boolean_(true));
        SValue::Ptr valueLo64 = ops->ite(isBigEndian, ops->extract(chunk64, 32, 64), ops->extract(chunk64, 0, 32));
        SValue::Ptr valueHi64 = ops->ite(isBigEndian, ops->extract(chunk64, 0, 32), ops->extract(chunk64, 32, 64));

        // Handle the 32-bit reads
        SValue::Ptr dflt32lo = ops->undefined_(32);
        SValue::Ptr dflt32hi = ops->undefined_(32);
        SValue::Ptr valueLo32 = ops->readMemory(RegisterDescriptor(), addr, dflt32lo, ops->boolean_(true));
        SValue::Ptr valueHi32 = ops->readMemory(RegisterDescriptor(), addrPlus4, dflt32hi, ops->boolean_(true));

        // Choose the two 32-bit values from either the split 64-bit read or the two 32-bit reads
        SValue::Ptr valueLo = ops->ite(isAligned, valueLo64, valueLo32);
        SValue::Ptr valueHi = ops->ite(isAligned, valueHi64, valueHi32);

        // Write to the registers
        d->maybeWrite(enabled, args[0], valueLo);
        d->maybeWrite(enabled, args[1], valueHi);
    }
};

// Load register exclusive
struct IP_ldrex: P {
    void p(D d, Ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr readValue = d->read(args[1], 32);
        d->maybeWrite(enabled, args[0], readValue);
    }
};

// Load register halfword
struct IP_ldrh: P {
    void p(D d, Ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr readValue = d->read(args[1], 32);
        d->maybeWrite(enabled, args[0], readValue);
    }
};

// Load register halfword privileged
struct IP_ldrht: P {
    void p(D d, Ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr readValue = d->read(args[1], 32);
        d->maybeWrite(enabled, args[0], readValue);
    }
};

// Load register signed byte
struct IP_ldrsb: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr byte = d->read(args[1], 8);
        SValue::Ptr toWrite = ops->signExtend(byte, 32);
        d->maybeWrite(enabled, args[0], toWrite);
    }
};

// Load register signed byte unprivileged
struct IP_ldrsbt: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr byte = d->read(args[1], 8);
        SValue::Ptr toWrite = ops->signExtend(byte, 32);
        d->maybeWrite(enabled, args[0], toWrite);
    }
};

// Load register signed halfword
struct IP_ldrsh: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr byte = d->read(args[1], 16);
        SValue::Ptr toWrite = ops->signExtend(byte, 32);
        d->maybeWrite(enabled, args[0], toWrite);
    }
};

// Load register signed halfword unprivileged
struct IP_ldrsht: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr byte = d->read(args[1], 16);
        SValue::Ptr toWrite = ops->signExtend(byte, 32);
        d->maybeWrite(enabled, args[0], toWrite);
    }
};

// Load register unprivileged
struct IP_ldrt: P {
    void p(D d, Ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr readValue = d->read(args[1], 32);
        d->maybeWrite(enabled, args[0], readValue);
    }
};

// Move to system register from general purpose register or execute a system instruction.
struct IP_mcr: P {
    void p(D, Ops, I insn, A args, V) {
        assert_args(insn, args, 6);
        // System registers are modified by hardware over which we have no control, and we don't even know what this hardware
        // is. Therefore, all writes can be safely ignored since a read later could return anything anyway.
    }
};

// Move to system register from two general purpose registers
struct IP_mcrr: P {
    void p(D, Ops, I insn, A args, V) {
        assert_args(insn, args, 5);
        // System registers are modified by hardware over which we have no control, and we don't even know what this hardware
        // is. Therefore, all writes can be safely ignored since a read later could return anything anyway.
    }
};

// Multiply accumulate
struct IP_mla: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr c = d->read(args[3], 32);
        SValue::Ptr product = ops->unsignedExtend(ops->unsignedMultiply(a, b), 32);
        SValue::Ptr result = ops->add(product, c);
        d->maybeWrite(enabled, args[0], result);
        if (insn->get_updatesFlags()) {
            d->maybeWriteRegister(enabled, d->REG_PSTATE_N, d->signBit(result));
            d->maybeWriteRegister(enabled, d->REG_PSTATE_Z, ops->equalToZero(result));
        }
    }
};

// Multiply and subtract
struct IP_mls: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr c = d->read(args[3], 32);
        SValue::Ptr product = ops->unsignedMultiply(a, b);
        SValue::Ptr sum = ops->add(product, ops->unsignedExtend(c, 64));
        SValue::Ptr result = ops->extract(sum, 0, 32);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Move to register
struct IP_mov: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr result = d->read(args[1], 32);
#if 1 // FIXME[Robb Matzke 2021-02-05]
        SValue::Ptr carry = ops->undefined_(1);           // somehow computed from result
#endif
        if (d->isIpRegister(args[0])) {
            if (insn->get_updatesFlags()) {
                d->aluExceptionReturn(enabled, result);
            } else {
                d->aluWritePc(enabled, result);
            }
        } else {
            d->maybeWrite(enabled, args[0], result);
            if (insn->get_updatesFlags()) {
                d->maybeWriteRegister(enabled, d->REG_PSTATE_N, d->signBit(result));
                d->maybeWriteRegister(enabled, d->REG_PSTATE_Z, ops->equalToZero(result));
                d->maybeWriteRegister(enabled, d->REG_PSTATE_C, carry);
            }
        }
    }
};

// Move to top
struct IP_movt: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr valueHi = d->read(args[1], 16);
        SValue::Ptr valueLo = ops->extract(d->read(args[0]), 0, 16);
        SValue::Ptr value = ops->concatHiLo(valueHi, valueLo);
        d->maybeWrite(enabled, args[0], value);
    }
};

// Move immediate
struct IP_movw: P {
    void p(D d, Ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr result = d->read(args[1], 32);
        ASSERT_forbid(insn->get_updatesFlags());
        if (d->isIpRegister(args[0])) {
            d->aluWritePc(enabled, result);
        } else {
            d->maybeWrite(enabled, args[0], result);
        }
    }
};

// Move to general-purpose register from system register
struct IP_mrc: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 6);

        // System registers belong to coprocessors and we don't know what they are or how they behave. Therefore, every read
        // from a coprocessor must return a new variable.
        SValue::Ptr value = ops->undefined_(args[2]->get_type()->get_nBits());
        d->maybeWrite(enabled, args[2], value);
    }
};

// Move special register to general-purpose register
struct IP_mrs: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr a = d->read(args[1]);
        if (d->accessedRegister(args[1]) == d->REG_SPSR) {
            d->maybeWrite(enabled, args[0], a);
        } else {
            // ARM documentation: CPSR has same bit assignments as SPSR, but with the IT, J, SS, IL, and T bits masked out.
            // Furthermore, when accessed from user mode, the E, A, I, F, and bits<9:6> are unknown.
            //
            // But the ARM register documentation and the Capstone decoder both mention APSR instead.
            SValue::Ptr mask = ops->number_(32, 0b11111000000011110000001111011111);
            SValue::Ptr value = ops->and_(a, mask);
            SValue::Ptr v23_31 = d->part(value, 31, 23);
            SValue::Ptr v22 = ops->undefined_(1);
            SValue::Ptr v10_21 = d->part(value, 21, 10);
            SValue::Ptr v6_9 = d->undefined_(4);
            SValue::Ptr v5 = d->part(value, 5);
            SValue::Ptr v0_4 = d->undefined_(5);
            SValue::Ptr result = ops->concatHiLo(v23_31, v22);
            result = ops->concatHiLo(result, v10_21);
            result = ops->concatHiLo(result, v6_9);
            result = ops->concatHiLo(result, v5);
            result = ops->concatHiLo(result, v0_4);
            d->maybeWrite(enabled, args[0], result);
        }
    }
};

// Move to banked or special register from general-purpose register
struct IP_msr: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);

        uint32_t word = insn->get_rawBytes()[0] |
                        ((uint32_t)insn->get_rawBytes()[1] << 8) |
                        ((uint32_t)insn->get_rawBytes()[2] << 16) |
                        ((uint32_t)insn->get_rawBytes()[3] << 24);
        if (BitOps::bits(word, 12, 27) == 0b0011011000001111) {
            // MSR MASK==0 && R == 1 is constrainted unpredictable behavior. The choices are that this instruction
            // is either UNDEFINED or executes as NOP. ROSE will do the latter.
        } else {
            SValue::Ptr result = d->read(args[1], 32);
            if (auto rre = isSgAsmDirectRegisterExpression(args[0])) {
                RegisterDescriptor dest = rre->get_descriptor();
                result = ops->extract(result, dest.offset(), dest.offset() + dest.nBits());
            }
            d->maybeWrite(enabled, args[0], result);
        }
    }
};

// Multiply
struct IP_mul: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr product = ops->signedMultiply(a, b);
        SValue::Ptr result = ops->extract(product, 0, 32);
        d->maybeWrite(enabled, args[0], result);
        if (insn->get_updatesFlags()) {
            d->maybeWriteRegister(enabled, d->REG_PSTATE_N, d->signBit(result));
            d->maybeWriteRegister(enabled, d->REG_PSTATE_Z, ops->equalToZero(result));
        }
    }
};

// Bitwise NOT
struct IP_mvn: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr result = ops->invert(a);
#if 1 // FIXME[Robb Matzke 2021-02-05]
        SValue::Ptr carry = ops->undefined_(1);           // somehow computed from b
#endif
        if (d->isIpRegister(args[0])) {
            if (insn->get_updatesFlags()) {
                d->aluExceptionReturn(enabled, result);
            } else {
                d->aluWritePc(enabled, result);
            }
        } else {
            d->maybeWrite(enabled, args[0], result);
            if (insn->get_updatesFlags()) {
                d->maybeWriteRegister(enabled, d->REG_PSTATE_N, d->signBit(result));
                d->maybeWriteRegister(enabled, d->REG_PSTATE_Z, ops->equalToZero(result));
                d->maybeWriteRegister(enabled, d->REG_PSTATE_C, carry);
            }
        }
    }
};

// Bitwise OR
struct IP_orr: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr result = ops->or_(a, b);
#if 1 // FIXME[Robb Matzke 2021-02-05]
        SValue::Ptr carry = ops->undefined_(1);           // somehow computed from b
#endif
        if (d->isIpRegister(args[0])) {
            if (insn->get_updatesFlags()) {
                d->aluExceptionReturn(enabled, result);
            } else {
                d->aluWritePc(enabled, result);
            }
        } else {
            d->maybeWrite(enabled, args[0], result);
            if (insn->get_updatesFlags()) {
                d->maybeWriteRegister(enabled, d->REG_PSTATE_N, d->signBit(result));
                d->maybeWriteRegister(enabled, d->REG_PSTATE_Z, ops->equalToZero(result));
                d->maybeWriteRegister(enabled, d->REG_PSTATE_C, carry);
            }
        }
    }
};

// Pack halfword
struct IP_pkhbt: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->extract(a, 0, 16);
        SValue::Ptr bHi = ops->extract(b, 16, 32);
        SValue::Ptr result = ops->concatHiLo(bHi, aLo);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Pack halfword
struct IP_pkhtb: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aHi = ops->extract(a, 16, 32);
        SValue::Ptr bLo = ops->extract(b, 0, 16);
        SValue::Ptr result = ops->concatHiLo(aHi, bLo);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Preload data
struct IP_pld: P {
    void p(D, Ops, I insn, A args, V) {
        assert_args(insn, args, 1);
        // The effect of a PLD instruction is implementation defined. ROSE does nothing.
    }
};

// Pop multiple registers from stack
struct IP_pop: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, std::max(args.size(), size_t(1)));
        SValue::Ptr oldSp = ops->readRegister(d->REG_SP);
        SValue::Ptr addr = oldSp;
        SValue::Ptr four = ops->number_(32, 4);
        for (size_t i = 0; i < args.size(); ++i) {
            SValue::Ptr dflt = ops->undefined_(32);
            SValue::Ptr value = ops->readMemory(RegisterDescriptor(), addr, dflt, ops->boolean_(true));
            if (d->isIpRegister(args[i])) {
                SValue::Ptr validPc = value;
                SValue::Ptr invalidPc = ops->undefined_(32);
                SValue::Ptr addrAligned = ops->equalToZero(ops->extract(addr, 0, 2));
                SValue::Ptr newPc = ops->ite(addrAligned, validPc, invalidPc);
                d->loadWritePc(enabled, newPc);
            } else {
                d->maybeWrite(enabled, args[i], value);
            }
            addr = ops->add(addr, four);
        }

        // Update the stack pointer register. It's value is undefined if the stack pointer is one of the registers
        // that was popped.
        bool poppedSp = false;
        for (size_t i = 0; i < args.size(); ++i) {
            if (d->accessedRegister(args[i]) == d->REG_SP) {
                poppedSp = true;
                break;
            }
        }
        if (poppedSp) {
            d->maybeWriteRegister(enabled, d->REG_SP, ops->undefined_(32));
        } else {
            SValue::Ptr nBytesRead = ops->number_(32, 4 * args.size());
            SValue::Ptr newSp = ops->add(oldSp, nBytesRead);
            d->maybeWriteRegister(enabled, d->REG_SP, newSp);
        }
    }
};

// Push
struct IP_push: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, std::max(args.size(), size_t(1)));
        SValue::Ptr four = ops->number_(32, 4);
        SValue::Ptr nBytes = ops->number_(32, 4 * args.size());
        SValue::Ptr firstAddress = ops->subtract(ops->readRegister(d->REG_SP), nBytes);
        SValue::Ptr address = firstAddress;
        for (size_t i = 0; i < args.size(); ++i) {
            ASSERT_require(isSgAsmDirectRegisterExpression(args[i]));
            const RegisterDescriptor reg = isSgAsmDirectRegisterExpression(args[i])->get_descriptor();
            SValue::Ptr toWrite;
            if (reg == d->REG_SP && i != 0) {
                toWrite = ops->undefined_(32);
            } else if (reg == d->REG_PC) {
                toWrite = d->pcStoreValue();
            } else {
                toWrite = d->read(args[i]);
            }
            d->maybeWriteMemory(enabled, address, toWrite);
            address = ops->add(address, four);
        }
        d->maybeWriteRegister(enabled, d->REG_SP, firstAddress);
    }
};

// Saturating add
struct IP_qadd: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = ops->signExtend(d->read(args[1], 32), 33);
        SValue::Ptr b = ops->signExtend(d->read(args[2], 32), 33);
        SValue::Ptr sum = ops->add(a, b);
        SValue::Ptr result = d->signedSat(sum, 32);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Saturating add 16
struct IP_qadd16: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->signExtend(ops->extract(a, 0, 16), 17);
        SValue::Ptr aHi = ops->signExtend(ops->extract(a, 16, 32), 17);
        SValue::Ptr bLo = ops->signExtend(ops->extract(b, 0, 16), 17);
        SValue::Ptr bHi = ops->signExtend(ops->extract(b, 16, 32), 17);
        SValue::Ptr sum1 = d->signedSat(ops->add(aLo, bLo), 16);
        SValue::Ptr sum2 = d->signedSat(ops->add(aHi, bHi), 16);
        SValue::Ptr result = ops->concatHiLo(sum2, sum1);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Saturating add 8
struct IP_qadd8: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr result;
        for (unsigned i = 0; i < 4; ++i) {
            SValue::Ptr aPart = ops->signExtend(ops->extract(a, i*8, i*8+8), 9);
            SValue::Ptr bPart = ops->signExtend(ops->extract(b, i*8, i*8+8), 9);
            SValue::Ptr sum = ops->add(aPart, bPart);
            SValue::Ptr sumSat = d->signedSat(sum, 8);
            result = result ? ops->concatHiLo(sumSat, result) : sumSat;
        }
        d->maybeWrite(enabled, args[0], result);
    }
};

// Saturating add and subtract with exchange
struct IP_qasx: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->signExtend(ops->extract(a, 0, 16), 17);
        SValue::Ptr aHi = ops->signExtend(ops->extract(a, 16, 32), 17);
        SValue::Ptr bLo = ops->signExtend(ops->extract(b, 0, 16), 17);
        SValue::Ptr bHi = ops->signExtend(ops->extract(b, 16, 32), 17);
        SValue::Ptr diff = ops->subtract(aLo, bHi);
        SValue::Ptr sum = ops->add(aHi, bLo);
        SValue::Ptr diffSat = d->signedSat(diff, 16);
        SValue::Ptr sumSat = d->signedSat(sum, 16);
        SValue::Ptr result = ops->concatLoHi(diffSat, sumSat);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Saturating double and add
struct IP_qdadd: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = ops->signExtend(d->read(args[1], 32), 33);
        SValue::Ptr b = ops->signExtend(d->read(args[2], 32), 33);

        auto satAndOverflow = d->signedSatQ(ops->add(a, a), 32);
        SValue::Ptr dbl = std::get<0>(satAndOverflow);
        SValue::Ptr ov1 = std::get<1>(satAndOverflow);

        satAndOverflow = d->signedSatQ(ops->add(ops->unsignedExtend(dbl, 33), b), 32);
        SValue::Ptr result = std::get<0>(satAndOverflow);
        SValue::Ptr ov2 = std::get<1>(satAndOverflow);

        SValue::Ptr overflowed = ops->or_(ov1, ov2);
        SValue::Ptr yes = ops->boolean_(true);
        d->maybeWrite(enabled, args[0], result);
        d->maybeWriteRegister(ops->and_(enabled, overflowed), d->REG_PSTATE_Q, yes);
    }
};

// Saturating double and subtract
struct IP_qdsub: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = ops->signExtend(d->read(args[1], 32), 33);
        SValue::Ptr b = ops->signExtend(d->read(args[2], 32), 33);

        auto satAndOverflow = d->signedSatQ(ops->add(a, a), 32);
        SValue::Ptr dbl = std::get<0>(satAndOverflow);
        SValue::Ptr ov1 = std::get<1>(satAndOverflow);

        satAndOverflow = d->signedSatQ(ops->subtract(b, ops->unsignedExtend(dbl, 33)), 32);
        SValue::Ptr result = std::get<0>(satAndOverflow);
        SValue::Ptr ov2 = std::get<1>(satAndOverflow);

        SValue::Ptr overflowed = ops->or_(ov1, ov2);
        SValue::Ptr yes = ops->boolean_(true);
        d->maybeWrite(enabled, args[0], result);
        d->maybeWriteRegister(ops->and_(enabled, overflowed), d->REG_PSTATE_Q, yes);
    }
};

// Saturating subtract and add with exchange
struct IP_qsax: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->signExtend(ops->extract(a, 0, 16), 17);
        SValue::Ptr aHi = ops->signExtend(ops->extract(a, 16, 32), 17);
        SValue::Ptr bLo = ops->signExtend(ops->extract(b, 0, 16), 17);
        SValue::Ptr bHi = ops->signExtend(ops->extract(b, 16, 32), 17);
        SValue::Ptr sum = ops->add(aLo, bHi);
        SValue::Ptr diff = ops->subtract(aHi, bLo);
        SValue::Ptr diffSat = d->signedSat(diff, 16);
        SValue::Ptr sumSat = d->signedSat(sum, 16);
        SValue::Ptr result = ops->concatLoHi(diffSat, sumSat);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Saturating subtract
struct IP_qsub: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = ops->signExtend(d->read(args[1], 32), 33);
        SValue::Ptr b = ops->signExtend(d->read(args[2], 32), 33);
        SValue::Ptr diff = ops->subtract(a, b);

        auto satAndOverflow = d->signedSatQ(diff, 32);
        SValue::Ptr result = std::get<0>(satAndOverflow);
        SValue::Ptr sat = std::get<1>(satAndOverflow);

        d->maybeWrite(enabled, args[0], result);
        SValue::Ptr yes = ops->boolean_(true);
        d->maybeWriteRegister(ops->and_(enabled, sat), d->REG_PSTATE_Q, yes);
    }
};

// Saturating subtract 16
struct IP_qsub16: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->signExtend(ops->extract(a, 0, 16), 17);
        SValue::Ptr aHi = ops->signExtend(ops->extract(a, 16, 32), 17);
        SValue::Ptr bLo = ops->signExtend(ops->extract(b, 0, 16), 17);
        SValue::Ptr bHi = ops->signExtend(ops->extract(b, 16, 32), 17);
        SValue::Ptr diffLo = ops->subtract(aLo, bLo);
        SValue::Ptr diffHi = ops->subtract(aHi, bHi);
        SValue::Ptr diffLoSat = d->signedSat(diffLo, 16);
        SValue::Ptr diffHiSat = d->signedSat(diffHi, 16);
        SValue::Ptr result = ops->concatHiLo(diffHiSat, diffLoSat);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Saturating subtract 8
struct IP_qsub8: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr result;
        for (unsigned i = 0; i < 4; ++i) {
            SValue::Ptr aPart = ops->signExtend(ops->extract(a, i*8, i*8+8), 9);
            SValue::Ptr bPart = ops->signExtend(ops->extract(b, i*8, i*8+8), 9);
            SValue::Ptr diff = ops->subtract(aPart, bPart);
            SValue::Ptr diffSat = d->signedSat(diff, 8);
            result = result ? ops->concatHiLo(diffSat, result) : diffSat;
        }
        d->maybeWrite(enabled, args[0], result);
    }
};

// Return from exception, decrement after
struct IP_rfeda: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 1);
        SValue::Ptr a = d->read(args[0], 32);
        SValue::Ptr addrLo = ops->subtract(a, ops->number_(32, 8));
        SValue::Ptr addrHi = ops->subtract(a, ops->number_(32, 4));
        SValue::Ptr yes = ops->boolean_(true);
        SValue::Ptr newPcValue = ops->readMemory(RegisterDescriptor(), addrLo, ops->undefined_(32), yes);
        SValue::Ptr spsr = ops->readMemory(RegisterDescriptor(), addrHi, ops->undefined_(32), yes);
        d->aarch32ExceptionReturn(enabled, newPcValue, spsr);
    }
};

// Return from exception, decrement before
struct IP_rfedb: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 1);
        SValue::Ptr a = d->read(args[0], 32);
        SValue::Ptr addrLo = ops->subtract(a, ops->number_(32, 4));
        SValue::Ptr addrHi = a;
        SValue::Ptr yes = ops->boolean_(true);
        SValue::Ptr newPcValue = ops->readMemory(RegisterDescriptor(), addrLo, ops->undefined_(32), yes);
        SValue::Ptr spsr = ops->readMemory(RegisterDescriptor(), addrHi, ops->undefined_(32), yes);
        d->aarch32ExceptionReturn(enabled, newPcValue, spsr);
    }
};

// Return from exception, increment after
struct IP_rfeia: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 1);
        SValue::Ptr addrLo = d->read(args[0], 32);
        SValue::Ptr addrHi = ops->add(addrLo, ops->number_(32, 4));
        SValue::Ptr yes = ops->boolean_(true);
        SValue::Ptr newPcValue = ops->readMemory(RegisterDescriptor(), addrLo, ops->undefined_(32), yes);
        SValue::Ptr spsr = ops->readMemory(RegisterDescriptor(), addrHi, ops->undefined_(32), yes);
        d->aarch32ExceptionReturn(enabled, newPcValue, spsr);
    }
};

// Return from exception, increment before
struct IP_rfeib: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 1);
        SValue::Ptr a = d->read(args[0], 32);
        SValue::Ptr addrLo = ops->add(a, ops->number_(32, 4));
        SValue::Ptr addrHi = ops->add(a, ops->number_(32, 8));
        SValue::Ptr yes = ops->boolean_(true);
        SValue::Ptr newPcValue = ops->readMemory(RegisterDescriptor(), addrLo, ops->undefined_(32), yes);
        SValue::Ptr spsr = ops->readMemory(RegisterDescriptor(), addrHi, ops->undefined_(32), yes);
        d->aarch32ExceptionReturn(enabled, newPcValue, spsr);
    }
};

// Reverse subtract
struct IP_rsb: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr notA = ops->invert(a);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr c = ops->boolean_(true);

        auto sumAndFlags = d->addWithCarry(notA, b, c);
        SValue::Ptr result = std::get<0>(sumAndFlags);
        SValue::Ptr nzcv = std::get<1>(sumAndFlags);

        if (d->isIpRegister(args[0])) {
            if (insn->get_updatesFlags()) {
                d->aluExceptionReturn(enabled, result);
            } else {
                d->aluWritePc(enabled, result);
            }
        } else {
            d->maybeWrite(enabled, args[0], result);
            if (insn->get_updatesFlags())
                d->maybeWriteRegister(enabled, d->REG_PSTATE_NZCV, nzcv);
        }
    }
};

// Reverse subtract with carry
struct IP_rsc: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr notA = ops->invert(a);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr c = ops->readRegister(d->REG_PSTATE_C);

        auto sumAndFlags = d->addWithCarry(notA, b, c);
        SValue::Ptr result = std::get<0>(sumAndFlags);
        SValue::Ptr nzcv = std::get<1>(sumAndFlags);

        if (d->isIpRegister(args[0])) {
            if (insn->get_updatesFlags()) {
                d->aluExceptionReturn(enabled, result);
            } else {
                d->aluWritePc(enabled, result);
            }
        } else {
            d->maybeWrite(enabled, args[0], result);
            if (insn->get_updatesFlags())
                d->maybeWriteRegister(enabled, d->REG_PSTATE_NZCV, nzcv);
        }
    }
};

// Signed add 16
struct IP_sadd16: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->signExtend(ops->extract(a, 0, 16), 17);
        SValue::Ptr aHi = ops->signExtend(ops->extract(a, 16, 32), 17);
        SValue::Ptr bLo = ops->signExtend(ops->extract(b, 0, 16), 17);
        SValue::Ptr bHi = ops->signExtend(ops->extract(b, 16, 32), 17);
        SValue::Ptr sumLoWide = ops->add(aLo, bLo);
        SValue::Ptr sumHiWide = ops->add(aHi, bHi);

        SValue::Ptr sumLoNeg = d->signBit(sumLoWide);
        SValue::Ptr sumHiNeg = d->signBit(sumHiWide);
        SValue::Ptr zero = ops->number_(2, 0);
        SValue::Ptr three = ops->number_(2, 3);
        SValue::Ptr geLo = ops->ite(sumLoNeg, zero, three);
        SValue::Ptr geHi = ops->ite(sumHiNeg, zero, three);
        SValue::Ptr ge = ops->concatHiLo(geHi, geLo);

        SValue::Ptr sumLo = ops->extract(sumLoWide, 0, 16);
        SValue::Ptr sumHi = ops->extract(sumHiWide, 0, 16);
        SValue::Ptr result = ops->concatHiLo(sumHi, sumLo);

        d->maybeWrite(enabled, args[0], result);
        d->maybeWriteRegister(enabled, d->REG_PSTATE_GE, ge);
    }
};

// Signed add 8
struct IP_sadd8: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr result, ge;
        for (unsigned i = 0; i < 4; ++i) {
            SValue::Ptr aPart = ops->signExtend(ops->extract(a, i*8, i*8+8), 9);
            SValue::Ptr bPart = ops->signExtend(ops->extract(b, i*8, i*8+8), 9);
            SValue::Ptr sumWide = ops->add(aPart, bPart);
            SValue::Ptr nonNeg = ops->invert(d->signBit(sumWide));
            SValue::Ptr sum = ops->extract(sumWide, 0, 8);
            result = result ? ops->concatHiLo(sum, result) : sum;
            ge = ge ? ops->concatHiLo(nonNeg, ge) : nonNeg;
        }
        d->maybeWrite(enabled, args[0], result);
        d->maybeWriteRegister(enabled, d->REG_PSTATE_GE, ge);
    }
};

// Signed add and subtract with exchange
struct IP_sasx: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->signExtend(ops->extract(a, 0, 16), 17);
        SValue::Ptr aHi = ops->signExtend(ops->extract(a, 16, 32), 17);
        SValue::Ptr bLo = ops->signExtend(ops->extract(b, 0, 16), 17);
        SValue::Ptr bHi = ops->signExtend(ops->extract(b, 16, 32), 17);
        SValue::Ptr diffWide = ops->subtract(aLo, bHi);
        SValue::Ptr sumWide = ops->add(aHi, bLo);

        SValue::Ptr diffNeg = d->signBit(diffWide);
        SValue::Ptr sumNeg = d->signBit(sumWide);
        SValue::Ptr zero = ops->number_(2, 0);
        SValue::Ptr three = ops->number_(2, 3);
        SValue::Ptr geLo = ops->ite(diffNeg, zero, three);
        SValue::Ptr geHi = ops->ite(sumNeg, zero, three);
        SValue::Ptr ge = ops->concatHiLo(geHi, geLo);

        SValue::Ptr diff = ops->extract(diffWide, 0, 16);
        SValue::Ptr sum = ops->extract(sumWide, 0, 16);
        SValue::Ptr result = ops->concatHiLo(sum, diff);

        d->maybeWrite(enabled, args[0], result);
        d->maybeWriteRegister(enabled, d->REG_PSTATE_GE, ge);
    }
};

// Subtract with carry
struct IP_sbc: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr notB = ops->invert(b);
        SValue::Ptr c = ops->readRegister(d->REG_PSTATE_C);

        auto sumAndFlags = d->addWithCarry(a, notB, c);
        SValue::Ptr result = std::get<0>(sumAndFlags);
        SValue::Ptr nzcv = std::get<1>(sumAndFlags);

        if (d->isIpRegister(args[0])) {
            if (insn->get_updatesFlags()) {
                d->aluExceptionReturn(enabled, result);
            } else {
                d->aluWritePc(enabled, result);
            }
        } else {
            d->maybeWrite(enabled, args[0], result);
            if (insn->get_updatesFlags())
                d->maybeWriteRegister(enabled, d->REG_PSTATE_NZCV, nzcv);
        }
    }
};

// Signed bit field extract
struct IP_sbfx: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        ASSERT_require(isSgAsmIntegerValueExpression(args[2]));
        ASSERT_require(isSgAsmIntegerValueExpression(args[3]));
        SValue::Ptr a = d->read(args[1], 32);
        size_t lsb = isSgAsmIntegerValueExpression(args[2])->get_absoluteValue();
        size_t width = isSgAsmIntegerValueExpression(args[3])->get_absoluteValue();
        size_t msb = lsb + width - 1;
        if (msb <= 31) {
            SValue::Ptr aPart = ops->extract(a, lsb, lsb + width);
            SValue::Ptr result = ops->signExtend(aPart, 32);
            d->maybeWrite(enabled, args[0], result);
        } else {
            SValue::Ptr result = ops->undefined_(args[0]->get_type()->get_nBits());
            d->maybeWrite(enabled, args[0], result);
        }
    }
};

// Select bytes
struct IP_sel: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr ge = ops->readRegister(d->REG_PSTATE_GE);
        SValue::Ptr result;
        for (unsigned i = 0; i < 4; ++i) {
            SValue::Ptr aByte = ops->extract(a, i*8, i*8+8);
            SValue::Ptr bByte = ops->extract(b, i*8, i*8+8);
            SValue::Ptr geBit = ops->extract(ge, i, i+1);
            SValue::Ptr byte = ops->ite(geBit, aByte, bByte);
            result = result ? ops->concatHiLo(byte, result) : byte;
        }
        d->maybeWrite(enabled, args[0], result);
    }
};

// Signed halving add 16
struct IP_shadd16: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->signExtend(ops->extract(a, 0, 16), 17);
        SValue::Ptr aHi = ops->signExtend(ops->extract(a, 16, 32), 17);
        SValue::Ptr bLo = ops->signExtend(ops->extract(b, 0, 16), 17);
        SValue::Ptr bHi = ops->signExtend(ops->extract(b, 16, 32), 17);
        SValue::Ptr halfSumLo = ops->extract(ops->add(aLo, bLo), 1, 17);
        SValue::Ptr halfSumHi = ops->extract(ops->add(aHi, bHi), 1, 17);
        SValue::Ptr result = ops->concatHiLo(halfSumHi, halfSumLo);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Signed halving add 8
struct IP_shadd8: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr result;
        for (unsigned i = 0; i < 4; ++i) {
            SValue::Ptr aPart = ops->signExtend(ops->extract(a, i*8, i*8+8), 9);
            SValue::Ptr bPart = ops->signExtend(ops->extract(b, i*8, i*8+8), 9);
            SValue::Ptr sum = ops->add(aPart, bPart);
            SValue::Ptr sumHalf = ops->extract(sum, 1, 9);
            result = result ? ops->concatHiLo(sumHalf, result) : sumHalf;
        }
        d->maybeWrite(enabled, args[0], result);
    }
};

// Signed halving add and subtract with exchange
struct IP_shasx: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->signExtend(ops->extract(a, 0, 16), 17);
        SValue::Ptr aHi = ops->signExtend(ops->extract(a, 16, 32), 17);
        SValue::Ptr bLo = ops->signExtend(ops->extract(b, 0, 16), 17);
        SValue::Ptr bHi = ops->signExtend(ops->extract(b, 16, 32), 17);
        SValue::Ptr diff = ops->subtract(aLo, bHi);
        SValue::Ptr diffHalf = ops->extract(diff, 1, 17);
        SValue::Ptr sum = ops->add(aHi, bLo);
        SValue::Ptr sumHalf = ops->extract(sum, 1, 17);
        SValue::Ptr result = ops->concatHiLo(sumHalf, diffHalf);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Signed halving subtract and add with exchange
struct IP_shsax: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->signExtend(ops->extract(a, 0, 16), 17);
        SValue::Ptr aHi = ops->signExtend(ops->extract(a, 16, 32), 17);
        SValue::Ptr bLo = ops->signExtend(ops->extract(b, 0, 16), 17);
        SValue::Ptr bHi = ops->signExtend(ops->extract(b, 16, 32), 17);
        SValue::Ptr sum = ops->add(aLo, bHi);
        SValue::Ptr sumHalf = ops->extract(sum, 1, 17);
        SValue::Ptr diff = ops->subtract(aHi, bLo);
        SValue::Ptr diffHalf = ops->extract(diff, 1, 17);
        SValue::Ptr result = ops->concatHiLo(sumHalf, diffHalf);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Signed halving subtract 16
struct IP_shsub16: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->signExtend(ops->extract(a, 0, 16), 17);
        SValue::Ptr aHi = ops->signExtend(ops->extract(a, 16, 32), 17);
        SValue::Ptr bLo = ops->signExtend(ops->extract(b, 0, 16), 17);
        SValue::Ptr bHi = ops->signExtend(ops->extract(b, 16, 32), 17);
        SValue::Ptr diffLo = ops->subtract(aLo, bLo);
        SValue::Ptr diffHi = ops->subtract(aHi, bHi);
        SValue::Ptr diffLoHalf = ops->extract(diffLo, 1, 17);
        SValue::Ptr diffHiHalf = ops->extract(diffHi, 1, 17);
        SValue::Ptr result = ops->concatHiLo(diffHiHalf, diffLoHalf);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Signed halving subtract 8
struct IP_shsub8: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr result;
        for (unsigned i = 0; i < 4; ++i) {
            SValue::Ptr aPart = ops->signExtend(ops->extract(a, i*8, i*8+8), 9);
            SValue::Ptr bPart = ops->signExtend(ops->extract(b, i*8, i*8+8), 9);
            SValue::Ptr diff = ops->subtract(aPart, bPart);
            SValue::Ptr diffHalf = ops->extract(diff, 1, 9);
            result = result ? ops->concatHiLo(diffHalf, result) : diffHalf;
        }
        d->maybeWrite(enabled, args[0], result);
    }
};

// Signed multiply accumulate halfwords
struct IP_smlabb: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr a = ops->extract(d->read(args[1], 32), 0, 16);
        SValue::Ptr b = ops->extract(d->read(args[2], 32), 0, 16);
        SValue::Ptr c = d->read(args[3], 32);
        SValue::Ptr product = ops->signedMultiply(a, b);

        auto sumAndFlags = d->addWithCarry(product, c, ops->boolean_(false));
        SValue::Ptr result = std::get<0>(sumAndFlags);
        SValue::Ptr nzcv = std::get<1>(sumAndFlags);

        d->maybeWrite(enabled, args[0], result);
        SValue::Ptr overflowed = ops->extract(nzcv, 0, 1);
        SValue::Ptr yes = ops->boolean_(true);
        d->maybeWriteRegister(ops->and_(enabled, overflowed), d->REG_PSTATE_Q, yes);
    }
};

// Signed multiply accumulate halfwords
struct IP_smlabt: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr a = ops->extract(d->read(args[1], 32), 0, 16);
        SValue::Ptr b = ops->extract(d->read(args[2], 32), 16, 32);
        SValue::Ptr c = d->read(args[3], 32);
        SValue::Ptr product = ops->signedMultiply(a, b);

        auto sumAndFlags = d->addWithCarry(product, c, ops->boolean_(false));
        SValue::Ptr result = std::get<0>(sumAndFlags);
        SValue::Ptr nzcv = std::get<1>(sumAndFlags);

        d->maybeWrite(enabled, args[0], result);
        SValue::Ptr overflowed = ops->extract(nzcv, 0, 1);
        SValue::Ptr yes = ops->boolean_(true);
        d->maybeWriteRegister(ops->and_(enabled, overflowed), d->REG_PSTATE_Q, yes);
    }
};

// Signed multiply accumulate dual
struct IP_smlad: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->extract(a, 0, 16);
        SValue::Ptr aHi = ops->extract(a, 16, 32);
        SValue::Ptr bLo = ops->extract(b, 0, 16);
        SValue::Ptr bHi = ops->extract(b, 16, 32);
        SValue::Ptr productLo = ops->signExtend(ops->signedMultiply(aLo, bLo), 34);
        SValue::Ptr productHi = ops->signExtend(ops->signedMultiply(aHi, bHi), 34);
        SValue::Ptr sum1 = ops->add(productLo, productHi);
        SValue::Ptr sum2 = ops->add(sum1, ops->signExtend(a, 34));
        SValue::Ptr result = ops->extract(sum2, 0, 32);
        SValue::Ptr resultWide = ops->signExtend(result, 34);
        SValue::Ptr differ = ops->isEqual(sum2, resultWide);
        SValue::Ptr yes = ops->boolean_(true);
        d->maybeWrite(enabled, args[0], result);
        d->maybeWriteRegister(ops->and_(enabled, differ), d->REG_PSTATE_Q, yes);
    }
};

// Signed multiply accumulate dual
struct IP_smladx: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        // Documentation accomplishes the halfword swap in B by doing a rotate by 16. It's more efficient
        // for us to simply select the opposite sides of B.
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->extract(a, 0, 16);
        SValue::Ptr aHi = ops->extract(a, 16, 32);
        SValue::Ptr bLo = ops->extract(b, 16, 32);        // swapped, see comment above
        SValue::Ptr bHi = ops->extract(b, 0, 16);         // ditto
        SValue::Ptr productLo = ops->signExtend(ops->signedMultiply(aLo, bLo), 34);
        SValue::Ptr productHi = ops->signExtend(ops->signedMultiply(aHi, bHi), 34);
        SValue::Ptr sum1 = ops->add(productLo, productHi);
        SValue::Ptr sum2 = ops->add(sum1, ops->signExtend(a, 34));
        SValue::Ptr result = ops->extract(sum2, 0, 32);
        SValue::Ptr resultWide = ops->signExtend(result, 34);
        SValue::Ptr differ = ops->isEqual(sum2, resultWide);
        SValue::Ptr yes = ops->boolean_(true);
        d->maybeWrite(enabled, args[0], result);
        d->maybeWriteRegister(ops->and_(enabled, differ), d->REG_PSTATE_Q, yes);
    }
};

// Signed multiply accumulate long
struct IP_smlal: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr dLo = d->read(args[0], 32);
        SValue::Ptr dHi = d->read(args[1], 32);
        SValue::Ptr a = d->read(args[2], 32);
        SValue::Ptr b = d->read(args[3], 32);
        SValue::Ptr prod = ops->unsignedMultiply(a, b);
        SValue::Ptr accum = ops->concatHiLo(dHi, dLo);
        SValue::Ptr sum = ops->add(prod, accum);
        SValue::Ptr resultLo = ops->extract(sum, 0, 32);
        SValue::Ptr resultHi = ops->extract(sum, 32, 64);
        d->maybeWrite(enabled, args[0], resultLo);
        d->maybeWrite(enabled, args[1], resultHi);
        if (insn->get_updatesFlags()) {
            d->maybeWriteRegister(enabled, d->REG_PSTATE_N, d->signBit(sum));
            d->maybeWriteRegister(enabled, d->REG_PSTATE_Z, ops->equalToZero(sum));
        }
    }
};

// Signed multiply accumulate long
struct IP_smlalbb: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr dLo = d->read(args[0], 32);
        SValue::Ptr dHi = d->read(args[1], 32);
        SValue::Ptr a = d->read(args[2], 16);
        SValue::Ptr b = d->read(args[3], 16);
        SValue::Ptr prod = ops->signedMultiply(a, b);
        SValue::Ptr prod64 = ops->signExtend(prod, 64);
        SValue::Ptr addend = ops->concatHiLo(dHi, dLo);
        SValue::Ptr sum = ops->add(prod64, addend);
        SValue::Ptr resultLo = ops->extract(sum, 0, 32);
        SValue::Ptr resultHi = ops->extract(sum, 32, 64);
        d->maybeWrite(enabled, args[0], resultLo);
        d->maybeWrite(enabled, args[1], resultHi);
    }
};

// Signed multiply accumulate long
struct IP_smlalbt: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr dLo = d->read(args[0], 32);
        SValue::Ptr dHi = d->read(args[1], 32);
        SValue::Ptr a = d->read(args[2], 16);
        SValue::Ptr b = ops->extract(d->read(args[3], 32), 16, 32);
        SValue::Ptr prod = ops->signedMultiply(a, b);
        SValue::Ptr prod64 = ops->signExtend(prod, 64);
        SValue::Ptr addend = ops->concatHiLo(dHi, dLo);
        SValue::Ptr sum = ops->add(prod64, addend);
        SValue::Ptr resultLo = ops->extract(sum, 0, 32);
        SValue::Ptr resultHi = ops->extract(sum, 32, 64);
        d->maybeWrite(enabled, args[0], resultLo);
        d->maybeWrite(enabled, args[1], resultHi);
    }
};

// Signed multiply accumulate long dual
struct IP_smlald: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr dLo = d->read(args[0], 32);
        SValue::Ptr dHi = d->read(args[1], 32);
        SValue::Ptr a = d->read(args[2], 32);
        SValue::Ptr b = d->read(args[3], 32);
        SValue::Ptr aLo = ops->extract(a, 0, 16);
        SValue::Ptr aHi = ops->extract(a, 16, 32);
        SValue::Ptr bLo = ops->extract(b, 0, 16);
        SValue::Ptr bHi = ops->extract(b, 16, 32);
        SValue::Ptr prod1 = ops->signExtend(ops->unsignedMultiply(aLo, bLo), 64);
        SValue::Ptr prod2 = ops->signExtend(ops->unsignedMultiply(aHi, bHi), 64);
        SValue::Ptr sum1 = ops->add(prod1, prod2);
        SValue::Ptr addend = ops->concatHiLo(dHi, dLo);
        SValue::Ptr sum2 = ops->add(sum1, addend);
        SValue::Ptr resultLo = ops->extract(sum2, 0, 32);
        SValue::Ptr resultHi = ops->extract(sum2, 32, 64);
        d->maybeWrite(enabled, args[0], resultLo);
        d->maybeWrite(enabled, args[1], resultHi);
    }
};

// Signed multiply accumulate long dual
struct IP_smlaldx: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr dLo = d->read(args[0], 32);
        SValue::Ptr dHi = d->read(args[1], 32);
        SValue::Ptr a = d->read(args[2], 32);
        SValue::Ptr b = d->read(args[3], 32);
        SValue::Ptr aLo = ops->extract(a, 0, 16);
        SValue::Ptr aHi = ops->extract(a, 16, 32);
        SValue::Ptr bLo = ops->extract(b, 16, 32);        // swapped wrt SMLALD
        SValue::Ptr bHi = ops->extract(b, 0, 16);         // ditto
        SValue::Ptr prod1 = ops->signExtend(ops->unsignedMultiply(aLo, bLo), 64);
        SValue::Ptr prod2 = ops->signExtend(ops->unsignedMultiply(aHi, bHi), 64);
        SValue::Ptr sum1 = ops->add(prod1, prod2);
        SValue::Ptr addend = ops->concatHiLo(dHi, dLo);
        SValue::Ptr sum2 = ops->add(sum1, addend);
        SValue::Ptr resultLo = ops->extract(sum2, 0, 32);
        SValue::Ptr resultHi = ops->extract(sum2, 32, 64);
        d->maybeWrite(enabled, args[0], resultLo);
        d->maybeWrite(enabled, args[1], resultHi);
    }
};

// Signed multiply accumulate long
struct IP_smlaltb: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr dLo = d->read(args[0], 32);
        SValue::Ptr dHi = d->read(args[1], 32);
        SValue::Ptr a = ops->extract(d->read(args[2], 32), 16, 32);
        SValue::Ptr b = d->read(args[3], 16);
        SValue::Ptr prod = ops->signedMultiply(a, b);
        SValue::Ptr prod64 = ops->signExtend(prod, 64);
        SValue::Ptr addend = ops->concatHiLo(dHi, dLo);
        SValue::Ptr sum = ops->add(prod64, addend);
        SValue::Ptr resultLo = ops->extract(sum, 0, 32);
        SValue::Ptr resultHi = ops->extract(sum, 32, 64);
        d->maybeWrite(enabled, args[0], resultLo);
        d->maybeWrite(enabled, args[1], resultHi);
    }
};

// Signed multiply accumulate long
struct IP_smlaltt: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr dLo = d->read(args[0], 32);
        SValue::Ptr dHi = d->read(args[1], 32);
        SValue::Ptr a = ops->extract(d->read(args[2], 32), 16, 32);
        SValue::Ptr b = ops->extract(d->read(args[3], 32), 16, 32);
        SValue::Ptr prod = ops->signedMultiply(a, b);
        SValue::Ptr prod64 = ops->signExtend(prod, 64);
        SValue::Ptr addend = ops->concatHiLo(dHi, dLo);
        SValue::Ptr sum = ops->add(prod64, addend);
        SValue::Ptr resultLo = ops->extract(sum, 0, 32);
        SValue::Ptr resultHi = ops->extract(sum, 32, 64);
        d->maybeWrite(enabled, args[0], resultLo);
        d->maybeWrite(enabled, args[1], resultHi);
    }
};

// Signed multiply accumulate halfwords
struct IP_smlatb: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr a = ops->extract(d->read(args[1], 32), 16, 32);
        SValue::Ptr b = ops->extract(d->read(args[2], 32), 0, 16);
        SValue::Ptr c = d->read(args[3], 32);
        SValue::Ptr product = ops->signedMultiply(a, b);

        auto sumAndFlags = d->addWithCarry(product, c, ops->boolean_(false));
        SValue::Ptr result = std::get<0>(sumAndFlags);
        SValue::Ptr nzcv = std::get<1>(sumAndFlags);

        d->maybeWrite(enabled, args[0], result);
        SValue::Ptr overflowed = ops->extract(nzcv, 0, 1);
        SValue::Ptr yes = ops->boolean_(true);
        d->maybeWriteRegister(ops->and_(enabled, overflowed), d->REG_PSTATE_Q, yes);
    }
};

// Signed multiply accumulate halfwords
struct IP_smlatt: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr a = ops->extract(d->read(args[1], 32), 16, 32);
        SValue::Ptr b = ops->extract(d->read(args[2], 32), 16, 32);
        SValue::Ptr c = d->read(args[3], 32);
        SValue::Ptr product = ops->signedMultiply(a, b);

        auto sumAndFlags = d->addWithCarry(product, c, ops->boolean_(false));
        SValue::Ptr result = std::get<0>(sumAndFlags);
        SValue::Ptr nzcv = std::get<1>(sumAndFlags);

        d->maybeWrite(enabled, args[0], result);
        SValue::Ptr overflowed = ops->extract(nzcv, 0, 1);
        SValue::Ptr yes = ops->boolean_(true);
        d->maybeWriteRegister(ops->and_(enabled, overflowed), d->REG_PSTATE_Q, yes);
    }
};

// Signed multiply accumulate word by halfword
struct IP_smlawb: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr c = d->read(args[2], 32);
        SValue::Ptr bLo = ops->extract(b, 0, 16);
        SValue::Ptr product = ops->unsignedMultiply(a, bLo); // 48 bits
        SValue::Ptr productWide = ops->signExtend(product, 49);
        SValue::Ptr aShifted = ops->concatHiLo(a, ops->number_(16, 0));
        SValue::Ptr aShiftedWide = ops->signExtend(aShifted, 49);
        SValue::Ptr sum = ops->add(productWide, aShiftedWide);
        SValue::Ptr result = ops->extract(sum, 16, 48);
        SValue::Ptr toCompare = ops->extract(ops->shiftRightArithmetic(sum, ops->number_(5, 16)), 0, 32);
        SValue::Ptr differ = ops->isNotEqual(result, toCompare); // strange way to check, but it's what's documented
        SValue::Ptr yes = ops->boolean_(true);

        d->maybeWrite(enabled, args[0], result);
        d->maybeWriteRegister(ops->and_(enabled, differ), d->REG_PSTATE_Q, yes);
    }
};

// Signed multiply accumulate word by halfword
struct IP_smlawt: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr c = d->read(args[2], 32);
        SValue::Ptr bHi = ops->extract(b, 16, 32);
        SValue::Ptr product = ops->unsignedMultiply(a, bHi); // 48 bits
        SValue::Ptr productWide = ops->signExtend(product, 49);
        SValue::Ptr aShifted = ops->concatHiLo(a, ops->number_(16, 0));
        SValue::Ptr aShiftedWide = ops->signExtend(aShifted, 49);
        SValue::Ptr sum = ops->add(productWide, aShiftedWide);
        SValue::Ptr result = ops->extract(sum, 16, 48);
        SValue::Ptr toCompare = ops->extract(ops->shiftRightArithmetic(sum, ops->number_(5, 16)), 0, 32);
        SValue::Ptr differ = ops->isNotEqual(result, toCompare); // strange way to check, but it's what's documented
        SValue::Ptr yes = ops->boolean_(true);

        d->maybeWrite(enabled, args[0], result);
        d->maybeWriteRegister(ops->and_(enabled, differ), d->REG_PSTATE_Q, yes);
    }
};

// Signed Multiply subtract dual
struct IP_smlsd: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr c = d->read(args[2], 32);
        SValue::Ptr aLo = ops->extract(a, 0, 16);
        SValue::Ptr aHi = ops->extract(a, 16, 32);
        SValue::Ptr bLo = ops->extract(b, 0, 16);
        SValue::Ptr bHi = ops->extract(b, 16, 32);
        SValue::Ptr prodLo = ops->unsignedMultiply(aLo, bLo);
        SValue::Ptr prodHi = ops->unsignedMultiply(aHi, bHi);
        SValue::Ptr prodLoWide = ops->signExtend(prodLo, 34);
        SValue::Ptr prodHiWide = ops->signExtend(prodHi, 34);
        SValue::Ptr diffWide = ops->subtract(prodLoWide, prodHiWide);
        SValue::Ptr cWide = ops->signExtend(c, 34);
        SValue::Ptr sum = ops->add(diffWide, cWide);
        SValue::Ptr result = ops->extract(sum, 0, 32);

        // Strange way to do overflow, but we're sticking with it so this whole function better matches the documentation.
        SValue::Ptr resultWide = ops->signExtend(result, 34);
        SValue::Ptr overflow = ops->isNotEqual(resultWide, sum);
        SValue::Ptr yes = ops->boolean_(true);

        d->maybeWrite(enabled, args[0], result);
        d->maybeWriteRegister(ops->and_(enabled, overflow), d->REG_PSTATE_Q, yes);
    }
};

// Signed Multiply subtract dual
struct IP_smlsdx: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr c = d->read(args[2], 32);
        SValue::Ptr aLo = ops->extract(a, 0, 16);
        SValue::Ptr aHi = ops->extract(a, 16, 32);
        SValue::Ptr bLo = ops->extract(b, 16, 32);        // swapped for smlsdx wrt smlsd
        SValue::Ptr bHi = ops->extract(b, 0, 16);         // ditto
        SValue::Ptr prodLo = ops->unsignedMultiply(aLo, bLo);
        SValue::Ptr prodHi = ops->unsignedMultiply(aHi, bHi);
        SValue::Ptr prodLoWide = ops->signExtend(prodLo, 34);
        SValue::Ptr prodHiWide = ops->signExtend(prodHi, 34);
        SValue::Ptr diffWide = ops->subtract(prodLoWide, prodHiWide);
        SValue::Ptr cWide = ops->signExtend(c, 34);
        SValue::Ptr sum = ops->add(diffWide, cWide);
        SValue::Ptr result = ops->extract(sum, 0, 32);

        // Strange way to do overflow, but we're sticking with it so this whole function better matches the documentation.
        SValue::Ptr resultWide = ops->signExtend(result, 34);
        SValue::Ptr overflow = ops->isNotEqual(resultWide, sum);
        SValue::Ptr yes = ops->boolean_(true);

        d->maybeWrite(enabled, args[0], result);
        d->maybeWriteRegister(ops->and_(enabled, overflow), d->REG_PSTATE_Q, yes);
    }
};

// Signed most significant word multiply accumulate
struct IP_smmla: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr c = d->read(args[3], 32);
        SValue::Ptr product = ops->unsignedMultiply(a, b);
        SValue::Ptr addend = ops->concatHiLo(c, ops->number_(32, 0));
        SValue::Ptr sum = ops->add(product, addend);
        SValue::Ptr result = ops->extract(sum, 32, 64);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Signed most significant word multiply accumulate
struct IP_smmlar: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr c = d->read(args[3], 32);
        SValue::Ptr product = ops->unsignedMultiply(a, b);
        SValue::Ptr addend1 = ops->concatHiLo(c, ops->number_(32, 0));
        SValue::Ptr sum1 = ops->add(product, addend1);
        SValue::Ptr addend2 = ops->unsignedExtend(ops->number_(32, 0x80000000), 64);
        SValue::Ptr sum2 = ops->add(sum1, addend2);
        SValue::Ptr result = ops->extract(sum2, 32, 64);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Signed most significant word multiply subtract
struct IP_smmls: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr c = d->read(args[3], 32);
        SValue::Ptr product = ops->unsignedMultiply(a, b);
        SValue::Ptr addend = ops->concatHiLo(c, ops->number_(32, 0));
        SValue::Ptr sum = ops->add(product, addend);
        SValue::Ptr result = ops->extract(sum, 32, 64);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Signed most significant word multiply subtract
struct IP_smmlsr: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr c = d->read(args[3], 32);
        SValue::Ptr product = ops->unsignedMultiply(a, b);
        SValue::Ptr addend1 = ops->concatHiLo(c, ops->number_(32, 0));
        SValue::Ptr sum1 = ops->add(product, addend1);
        SValue::Ptr addend2 = ops->unsignedExtend(ops->number_(32, 0x80000000), 64);
        SValue::Ptr sum2 = ops->add(sum1, addend2);
        SValue::Ptr result = ops->extract(sum2, 32, 64);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Signed multiply halfwords
struct IP_smulbb: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = ops->extract(d->read(args[1], 32), 0, 16);
        SValue::Ptr b = ops->extract(d->read(args[2], 32), 0, 16);
        SValue::Ptr result = ops->unsignedMultiply(a, b);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Signed multiply halfwords
struct IP_smulbt: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = ops->extract(d->read(args[1], 32), 0, 16);
        SValue::Ptr b = ops->extract(d->read(args[2], 32), 16, 32);
        SValue::Ptr result = ops->unsignedMultiply(a, b);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Signed multiply long
struct IP_smull: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr a = d->read(args[2], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr prod = ops->unsignedMultiply(a, b);
        SValue::Ptr resultLo = ops->extract(prod, 0, 32);
        SValue::Ptr resultHi = ops->extract(prod, 32, 64);
        d->maybeWrite(enabled, args[0], resultLo);
        d->maybeWrite(enabled, args[1], resultHi);
        if (insn->get_updatesFlags()) {
            d->maybeWriteRegister(enabled, d->REG_PSTATE_N, d->signBit(prod));
            d->maybeWriteRegister(enabled, d->REG_PSTATE_Z, ops->equalToZero(prod));
        }
    }
};

// Signed multiply halfwords
struct IP_smultb: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = ops->extract(d->read(args[1], 32), 16, 32);
        SValue::Ptr b = ops->extract(d->read(args[2], 32), 0, 16);
        SValue::Ptr result = ops->unsignedMultiply(a, b);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Signed multiply halfwords
struct IP_smultt: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = ops->extract(d->read(args[1], 32), 16, 32);
        SValue::Ptr b = ops->extract(d->read(args[2], 32), 16, 32);
        SValue::Ptr result = ops->unsignedMultiply(a, b);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Signed multiply word by halfword
struct IP_smulwb: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 16);
        SValue::Ptr prod = ops->unsignedMultiply(a, b);
        SValue::Ptr result = ops->extract(prod, 16, 48);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Signed multiply word by halfword
struct IP_smulwt: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = ops->extract(d->read(args[2], 32), 16, 32);
        SValue::Ptr prod = ops->unsignedMultiply(a, b);
        SValue::Ptr result = ops->extract(prod, 16, 48);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Signed satureate
struct IP_ssat: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        ASSERT_require(isSgAsmIntegerValueExpression(args[1]));
        size_t nBits = isSgAsmIntegerValueExpression(args[1])->get_absoluteValue();
        SValue::Ptr value = d->read(args[2], 32);

        auto satAndOverflow = d->signedSatQ(value, nBits);
        SValue::Ptr sat = std::get<0>(satAndOverflow);
        SValue::Ptr overflowed = std::get<1>(satAndOverflow);

        SValue::Ptr result = ops->signExtend(sat, 32);
        d->maybeWrite(enabled, args[0], result);
        SValue::Ptr yes = ops->boolean_(true);
        d->maybeWriteRegister(ops->and_(enabled, overflowed), d->REG_PSTATE_Q, yes);
    }
};

// Signed subtract and add with exchange
struct IP_ssax: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->signExtend(ops->extract(a, 0, 16), 17);
        SValue::Ptr aHi = ops->signExtend(ops->extract(a, 16, 32), 17);
        SValue::Ptr bLo = ops->signExtend(ops->extract(b, 0, 16), 17);
        SValue::Ptr bHi = ops->signExtend(ops->extract(b, 16, 32), 17);
        SValue::Ptr sumWide = ops->add(aLo, bHi);
        SValue::Ptr diffWide = ops->subtract(aHi, bLo);
        SValue::Ptr sumIsNeg = d->signBit(sumWide);
        SValue::Ptr diffIsNeg = d->signBit(diffWide);
        SValue::Ptr zero = d->number_(2, 0);
        SValue::Ptr three = d->number_(2, 3);
        SValue::Ptr geLo = ops->ite(sumIsNeg, zero, three);
        SValue::Ptr geHi = ops->ite(diffIsNeg, zero, three);
        SValue::Ptr ge = ops->concatHiLo(geHi, geLo);
        SValue::Ptr sum = ops->extract(sumWide, 0, 16);
        SValue::Ptr diff = ops->extract(diffWide, 0, 16);
        SValue::Ptr result = ops->concatHiLo(diff, sum);
        d->maybeWrite(enabled, args[0], result);
        d->maybeWriteRegister(enabled, d->REG_PSTATE_GE, ge);
    }
};

// Signed subtract 16
struct IP_ssub16: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->signExtend(ops->extract(a, 0, 16), 17);
        SValue::Ptr aHi = ops->signExtend(ops->extract(a, 16, 32), 17);
        SValue::Ptr bLo = ops->signExtend(ops->extract(b, 0, 16), 17);
        SValue::Ptr bHi = ops->signExtend(ops->extract(b, 16, 32), 17);
        SValue::Ptr diffLoWide = ops->subtract(aLo, bLo);
        SValue::Ptr diffHiWide = ops->subtract(aHi, bHi);

        SValue::Ptr diffLoNeg = d->signBit(diffLoWide);
        SValue::Ptr diffHiNeg = d->signBit(diffHiWide);
        SValue::Ptr zero = ops->number_(2, 0);
        SValue::Ptr three = ops->number_(2, 3);
        SValue::Ptr geLo = ops->ite(diffLoNeg, zero, three);
        SValue::Ptr geHi = ops->ite(diffHiNeg, zero, three);
        SValue::Ptr ge = ops->concatHiLo(geHi, geLo);

        SValue::Ptr diffLo = ops->extract(diffLoWide, 0, 16);
        SValue::Ptr diffHi = ops->extract(diffHiWide, 0, 16);
        SValue::Ptr result = ops->concatHiLo(diffHi, diffLo);

        d->maybeWrite(enabled, args[0], result);
        d->maybeWriteRegister(enabled, d->REG_PSTATE_GE, ge);
    }
};

// Signed subtract 8
struct IP_ssub8: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[1], 32);
        SValue::Ptr result, ge;
        for (unsigned i = 0; i < 4; ++i) {
            SValue::Ptr aPart = ops->signExtend(ops->extract(a, i*8, i*8+8), 9);
            SValue::Ptr bPart = ops->signExtend(ops->extract(b, i*8, i*8+8), 9);
            SValue::Ptr diffWide = ops->subtract(aPart, bPart);
            SValue::Ptr isNonNeg = ops->invert(d->signBit(diffWide));
            SValue::Ptr diff = ops->extract(diffWide, 0, 8);
            result = result ? ops->concatHiLo(diff, result) : diff;
            ge = ge ? ops->concatHiLo(isNonNeg, ge) : isNonNeg;
        }
        d->maybeWrite(enabled, args[0], result);
        d->maybeWriteRegister(enabled, d->REG_PSTATE_GE, ge);
    }
};

// Store data to system register
struct IP_stc: P {
    void p(D d, Ops, I insn, A args, V) {
        assert_args(insn, args, 3);
        d->write(args[2], d->dbgdtrEl0());
    }
};

// Store multiple (increment after, empty ascending)
struct IP_stm: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, std::max(args.size(), size_t(1)));
        RegisterDescriptor addrReg = d->accessedRegister(args[0]);

        // Get the memory address where the writes will start
        SValue::Ptr addr = d->read(args[0], 32);

        // Write registers if this instruction is enabled
        for (size_t i = 1; i < args.size(); ++i) {
            ASSERT_require(isSgAsmDirectRegisterExpression(args[i]));
            RegisterDescriptor reg = isSgAsmDirectRegisterExpression(args[i])->get_descriptor();
            SValue::Ptr toWrite;
            if (reg == addrReg && i != 1) {
                toWrite = ops->undefined_(32);
            } else if (d->REG_PC == reg) {
                toWrite = d->pcStoreValue();
            } else {
                toWrite = d->read(args[i], 32);
            }
            d->maybeWriteMemory(enabled, addr, toWrite);
            addr = ops->add(addr, ops->number_(32, 4));
        }
    }
};

// Store multiple decrement after
struct IP_stmda: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, std::max(args.size(), size_t(1)));
        RegisterDescriptor addrReg = d->accessedRegister(args[0]);

        // Get the memory address where the writes will start
        SValue::Ptr reg0 = d->read(args[0], 32);
        size_t nRegs = args.size() - 1;                 // number of registers to write
        SValue::Ptr nBytes = ops->number_(32, nRegs * 4 - 4);
        SValue::Ptr addr = ops->subtract(reg0, nBytes);   // starting address: addr = Rn - 4*N + 4

        // Write registers if this instruction is enabled
        for (size_t i = 1; i < args.size(); ++i) {
            ASSERT_require(isSgAsmDirectRegisterExpression(args[i]));
            RegisterDescriptor reg = isSgAsmDirectRegisterExpression(args[i])->get_descriptor();
            SValue::Ptr toWrite;
            if (reg == addrReg && i != 1) {
                toWrite = ops->undefined_(32);
            } else if (d->REG_PC == reg) {
                toWrite = d->pcStoreValue();
            } else {
                toWrite = d->read(args[i], 32);
            }
            d->maybeWriteMemory(enabled, addr, toWrite);
            addr = ops->add(addr, ops->number_(32, 4));
        }
    }
};

// Store multiple decrement before
struct IP_stmdb: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, std::max(args.size(), size_t(1)));
        RegisterDescriptor addrReg = d->accessedRegister(args[0]);

        // Get the memory address where the writes will start
        SValue::Ptr reg0 = d->read(args[0], 32);
        size_t nRegs = args.size() - 1;                 // number of registers to write
        SValue::Ptr nBytes = ops->number_(32, nRegs * 4);
        SValue::Ptr addr = ops->subtract(reg0, nBytes);   // starting address: addr = Rn - 4*N

        // Write registers if this instruction is enabled
        for (size_t i = 1; i < args.size(); ++i) {
            ASSERT_require(isSgAsmDirectRegisterExpression(args[i]));
            RegisterDescriptor reg = isSgAsmDirectRegisterExpression(args[i])->get_descriptor();
            SValue::Ptr toWrite;
            if (reg == addrReg && i != 1) {
                toWrite = ops->undefined_(32);
            } else if (d->REG_PC == reg) {
                toWrite = d->pcStoreValue();
            } else {
                toWrite = d->read(args[i], 32);
            }
            d->maybeWriteMemory(enabled, addr, toWrite);
            addr = ops->add(addr, ops->number_(32, 4));
        }
    }
};

// Store multiple increment before
struct IP_stmib: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, std::max(args.size(), size_t(1)));
        RegisterDescriptor addrReg = d->accessedRegister(args[0]);

        // Get the memory address where the writes will start
        SValue::Ptr reg0 = d->read(args[0], 32);
        SValue::Ptr addr = ops->add(reg0, ops->number_(32, 4));

        // Write registers if this instruction is enabled
        for (size_t i = 1; i < args.size(); ++i) {
            ASSERT_require(isSgAsmDirectRegisterExpression(args[i]));
            RegisterDescriptor reg = isSgAsmDirectRegisterExpression(args[i])->get_descriptor();
            SValue::Ptr toWrite;
            if (reg == addrReg && i != 1) {
                toWrite = ops->undefined_(32);
            } else if (d->REG_PC == reg) {
                toWrite = d->pcStoreValue();
            } else {
                toWrite = d->read(args[i], 32);
            }
            d->maybeWriteMemory(enabled, addr, toWrite);
            addr = ops->add(addr, ops->number_(32, 4));
        }
    }
};

// Write register to memory
struct IP_str: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr lhs = d->read(args[0], 32);
        SValue::Ptr toWrite;
        if (d->isIpRegister(args[0])) {
            SValue::Ptr pcPlus8 = d->pcStoreValue();
            toWrite = ops->ite(d->isA32Mode(), pcPlus8, lhs);
        } else {
            toWrite = lhs;
        }
        d->maybeWrite(enabled, args[1], toWrite);
    }
};

// Write register to memory byte
struct IP_strb: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr lhs = d->read(args[0], 32);
        SValue::Ptr toWrite = ops->extract(lhs, 0, 8);
        d->maybeWrite(enabled, args[1], toWrite);
    }
};

// Write register to memory byte unprivileged
struct IP_strbt: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr lhs = d->read(args[0], 32);
        SValue::Ptr toWrite = ops->extract(lhs, 0, 8);
        d->maybeWrite(enabled, args[1], toWrite);
    }
};

// Store register dual
struct IP_strd: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[0], 32);
        SValue::Ptr b = d->read(args[1], 32);
        SValue::Ptr addr = d->effectiveAddress(args[2]);
        SValue::Ptr addrPlus4 = ops->add(addr, ops->number_(32, 4));
        SValue::Ptr isAligned = ops->equalToZero(ops->extract(addr, 0, 3));
        SValue::Ptr isNotAligned = ops->negate(isAligned);
        SValue::Ptr isBigEndian = d->bigEndian();
        SValue::Ptr toWriteAlignedBe = ops->concatHiLo(a, b);
        SValue::Ptr toWriteAlignedLe = ops->concatLoHi(a, b);
        SValue::Ptr toWriteAligned = ops->ite(isBigEndian, toWriteAlignedBe, toWriteAlignedLe);
        d->maybeWriteMemory(ops->and_(enabled, isAligned), addr, toWriteAligned);
        d->maybeWriteMemory(ops->and_(enabled, isNotAligned), addr, a);
        d->maybeWriteMemory(ops->and_(enabled, isNotAligned), addrPlus4, b);
    }
};

// Write register to memory half word
struct IP_strh: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr lhs = d->read(args[0], 32);
        SValue::Ptr toWrite = ops->extract(lhs, 0, 16);
        d->maybeWrite(enabled, args[1], toWrite);
    }
};

// Write register to memory half word unprivileged
struct IP_strht: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr lhs = d->read(args[0], 32);
        SValue::Ptr toWrite = ops->extract(lhs, 0, 16);
        d->maybeWrite(enabled, args[1], toWrite);
    }
};

// Write register to memory unprivileged
struct IP_strt: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr lhs = d->read(args[0], 32);
        SValue::Ptr toWrite;
        if (d->isIpRegister(args[0])) {
            SValue::Ptr pcPlus8 = d->pcStoreValue();
            toWrite = ops->ite(d->isA32Mode(), pcPlus8, lhs);
        } else {
            toWrite = lhs;
        }
        d->maybeWrite(enabled, args[1], toWrite);
    }
};

// Subtract
struct IP_sub: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr notB = ops->invert(b);
        SValue::Ptr c = ops->boolean_(true);

        auto sumAndFlags = d->addWithCarry(a, notB, c);
        SValue::Ptr result = std::get<0>(sumAndFlags);
        SValue::Ptr nzcv = std::get<1>(sumAndFlags);

        if (d->isIpRegister(args[0])) {
            if (insn->get_updatesFlags()) {
                d->aluExceptionReturn(enabled, result);
            } else {
                d->aluWritePc(enabled, result);
            }
        } else {
            d->maybeWrite(enabled, args[0], result);
            if (insn->get_updatesFlags())
                d->maybeWriteRegister(enabled, d->REG_PSTATE_NZCV, nzcv);
        }
    }
};

// Supervisor call
struct IP_svc: P {
    void p(D, Ops ops, I insn, A args, V) {
        assert_args(insn, args, 1);
        ASSERT_require(isSgAsmIntegerValueExpression(args[0]));
        uint32_t val = isSgAsmIntegerValueExpression(args[0])->get_absoluteValue(32);
        ops->interrupt(aarch32_exception_svc, val);
    }
};

// Signed extend and add byte
struct IP_sxtab: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr bLo = ops->signExtend(ops->extract(b, 0, 8), 32);
        SValue::Ptr result = ops->add(a, bLo);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Signed extend and add byte 16
struct IP_sxtab16: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->extract(a, 0, 16);
        SValue::Ptr aHi = ops->extract(a, 16, 32);
        SValue::Ptr bLo = ops->signExtend(ops->extract(b, 0, 8), 16);
        SValue::Ptr bHi = ops->signExtend(ops->extract(b, 16, 24), 16);
        SValue::Ptr sumLo = ops->add(aLo, bLo);
        SValue::Ptr sumHi = ops->add(aHi, bHi);
        SValue::Ptr result = ops->concatHiLo(sumHi, sumLo);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Signed extend and add halfword
struct IP_sxtah: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = ops->signExtend(d->read(args[2], 16), 32);
        SValue::Ptr result = ops->add(a, b);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Test equivalence
struct IP_teq: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr a = d->read(args[0], 32);
        SValue::Ptr b = d->read(args[1], 32);
        SValue::Ptr result = ops->xor_(a, b);
#if 1 // FIXME[Robb Matzke 2021-02-05]
        SValue::Ptr carry = ops->undefined_(1);           // somehow computed from b
#endif
        d->maybeWriteRegister(enabled, d->REG_PSTATE_N, d->signBit(result));
        d->maybeWriteRegister(enabled, d->REG_PSTATE_Z, ops->equalToZero(result));
        d->maybeWriteRegister(enabled, d->REG_PSTATE_C, carry);
    }
};

// Test
struct IP_tst: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr a = d->read(args[0], 32);
        SValue::Ptr b = d->read(args[1], 32);
        SValue::Ptr result = ops->and_(a, b);
#if 1 // FIXME[Robb Matzke 2021-02-05]
        SValue::Ptr carry = ops->undefined_(1);           // somehow computed from b
#endif
        d->maybeWriteRegister(enabled, d->REG_PSTATE_N, d->signBit(result));
        d->maybeWriteRegister(enabled, d->REG_PSTATE_Z, ops->equalToZero(result));
        d->maybeWriteRegister(enabled, d->REG_PSTATE_C, carry);
    }
};

// Unsigned add 16
struct IP_uadd16: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->unsignedExtend(ops->extract(a, 0, 16), 17);
        SValue::Ptr aHi = ops->unsignedExtend(ops->extract(a, 16, 32), 17);
        SValue::Ptr bLo = ops->unsignedExtend(ops->extract(b, 0, 16), 17);
        SValue::Ptr bHi = ops->unsignedExtend(ops->extract(b, 16, 32), 17);
        SValue::Ptr sumLoWide = ops->add(aLo, bLo);
        SValue::Ptr sumHiWide = ops->add(aHi, bHi);

        SValue::Ptr sumLoOverflow = d->signBit(sumLoWide);
        SValue::Ptr sumHiOverflow = d->signBit(sumHiWide);
        SValue::Ptr zero = ops->number_(2, 0);
        SValue::Ptr three = ops->number_(2, 3);
        SValue::Ptr geLo = ops->ite(sumLoOverflow, three, zero);
        SValue::Ptr geHi = ops->ite(sumHiOverflow, three, zero);
        SValue::Ptr ge = ops->concatHiLo(geHi, geLo);

        SValue::Ptr sumLo = ops->extract(sumLoWide, 0, 16);
        SValue::Ptr sumHi = ops->extract(sumHiWide, 0, 16);
        SValue::Ptr result = ops->concatHiLo(sumHi, sumLo);

        d->maybeWrite(enabled, args[0], result);
        d->maybeWriteRegister(enabled, d->REG_PSTATE_GE, ge);
    }
};

// Unsigned add 8
struct IP_uadd8: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr zero = ops->boolean_(false);
        SValue::Ptr one = ops->boolean_(true);
        SValue::Ptr result, ge;
        for (unsigned i = 0; i < 4; ++i) {
            SValue::Ptr aPart = ops->unsignedExtend(ops->extract(a, i*8, i*8+8), 9);
            SValue::Ptr bPart = ops->unsignedExtend(ops->extract(b, i*8, i*8+8), 9);
            SValue::Ptr sumWide = ops->add(aPart, bPart);
            SValue::Ptr overflow = d->signBit(sumWide);
            SValue::Ptr sum = ops->extract(sumWide, 0, 8);
            result = result ? ops->concat(sum, result) : sum;
            ge = ge ? ops->concat(overflow, ge) : overflow;
        }
        d->maybeWrite(enabled, args[0], result);
        d->maybeWriteRegister(enabled, d->REG_PSTATE_GE, ge);
    }
};

// Unsigned add and subtract with exchange
struct IP_uasx: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->unsignedExtend(ops->extract(a, 0, 16), 17);
        SValue::Ptr aHi = ops->unsignedExtend(ops->extract(a, 16, 32), 17);
        SValue::Ptr bLo = ops->unsignedExtend(ops->extract(b, 0, 16), 17);
        SValue::Ptr bHi = ops->unsignedExtend(ops->extract(b, 16, 32), 17);
        SValue::Ptr diffWide = ops->subtract(aLo, bHi);
        SValue::Ptr sumWide = ops->add(aHi, bLo);

        SValue::Ptr diffNonNegative = ops->invert(d->signBit(diffWide));
        SValue::Ptr sumOverflow = d->signBit(sumWide);
        SValue::Ptr zeroZero = ops->number_(2, 0);
        SValue::Ptr oneOne = ops->number_(2, 3);
        SValue::Ptr geLo = ops->ite(diffNonNegative, oneOne, zeroZero);
        SValue::Ptr geHi = ops->ite(sumOverflow, oneOne, zeroZero);
        SValue::Ptr ge = ops->concatHiLo(geHi, geLo);

        SValue::Ptr diff = ops->extract(diffWide, 0, 16);
        SValue::Ptr sum = ops->extract(sumWide, 0, 16);
        SValue::Ptr result = ops->concatHiLo(sum, diff);

        d->maybeWrite(enabled, args[0], result);
        d->maybeWriteRegister(enabled, d->REG_PSTATE_GE, ge);
    }
};

// Unsigend bit field extract
struct IP_ubfx: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr a = d->read(args[1], 32);
        ASSERT_require(isSgAsmIntegerValueExpression(args[2]));
        size_t lsb = isSgAsmIntegerValueExpression(args[2])->get_absoluteValue();
        ASSERT_require(isSgAsmIntegerValueExpression(args[3]));
        size_t width = isSgAsmIntegerValueExpression(args[3])->get_absoluteValue();
        size_t msb = lsb + width - 1;
        if (msb <= 31) {
            SValue::Ptr bits = ops->extract(a, lsb, lsb+width);
            SValue::Ptr result = ops->unsignedExtend(bits, 32);
            d->maybeWrite(enabled, args[0], result);
        } else {
            SValue::Ptr result = ops->undefined_(args[0]->get_type()->get_nBits());
            d->maybeWrite(enabled, args[0], result);
        }
    }
};

// Permanently undefined
struct IP_udf: P {
    void p(D, Ops ops, I insn, A args, V) {
        assert_args(insn, args, 1);
        ASSERT_require(isSgAsmIntegerValueExpression(args[0]));
        int imm = isSgAsmIntegerValueExpression(args[0])->get_absoluteValue();
        ops->interrupt(aarch32_exception_udf, imm);
    }
};

// Unsigned halving add 16
struct IP_uhadd16: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->unsignedExtend(ops->extract(a, 0, 16), 17);
        SValue::Ptr aHi = ops->unsignedExtend(ops->extract(a, 16, 32), 17);
        SValue::Ptr bLo = ops->unsignedExtend(ops->extract(b, 0, 16), 17);
        SValue::Ptr bHi = ops->unsignedExtend(ops->extract(b, 16, 32), 17);
        SValue::Ptr sumLo = ops->extract(ops->add(aLo, bLo), 1, 17);
        SValue::Ptr sumHi = ops->extract(ops->add(aHi, bHi), 1, 17);
        SValue::Ptr result = ops->concatHiLo(sumHi, sumLo);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Unsigned halving add 8
struct IP_uhadd8: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr result;
        for (unsigned i = 0; i < 4; ++i) {
            SValue::Ptr aPart = ops->unsignedExtend(ops->extract(a, i*8, i*8+8), 9);
            SValue::Ptr bPart = ops->unsignedExtend(ops->extract(b, i*8, i*8+8), 9);
            SValue::Ptr sum = ops->extract(ops->add(aPart, bPart), 1, 9);
            result = result ? ops->concatHiLo(sum, result) : sum;
        }
        d->maybeWrite(enabled, args[0], result);
    }
};

// Unsigned halving add and subtract with exchange
struct IP_uhasx: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->unsignedExtend(ops->extract(a, 0, 16), 17);
        SValue::Ptr aHi = ops->unsignedExtend(ops->extract(a, 16, 32), 17);
        SValue::Ptr bLo = ops->unsignedExtend(ops->extract(b, 0, 16), 17);
        SValue::Ptr bHi = ops->unsignedExtend(ops->extract(b, 16, 32), 17);
        SValue::Ptr diff = ops->extract(ops->subtract(aLo, bHi), 1, 17);
        SValue::Ptr sum = ops->extract(ops->add(aHi, bLo), 1, 17);
        SValue::Ptr result = ops->concatHiLo(sum, diff);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Unsigned halving add and subtract with exchange
struct IP_uhsax: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->unsignedExtend(ops->extract(a, 0, 16), 17);
        SValue::Ptr aHi = ops->unsignedExtend(ops->extract(a, 16, 32), 17);
        SValue::Ptr bLo = ops->unsignedExtend(ops->extract(b, 0, 16), 17);
        SValue::Ptr bHi = ops->unsignedExtend(ops->extract(b, 16, 32), 17);
        SValue::Ptr sum = ops->extract(ops->add(aLo, bHi), 1, 17);
        SValue::Ptr diff = ops->extract(ops->subtract(aHi, bLo), 1, 17);
        SValue::Ptr result = ops->concatHiLo(diff, sum);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Unsigned halving subtract 16
struct IP_uhsub16: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->unsignedExtend(ops->extract(a, 0, 16), 17);
        SValue::Ptr aHi = ops->unsignedExtend(ops->extract(a, 16, 32), 17);
        SValue::Ptr bLo = ops->unsignedExtend(ops->extract(b, 0, 16), 17);
        SValue::Ptr bHi = ops->unsignedExtend(ops->extract(b, 16, 32), 17);
        SValue::Ptr diffLo = ops->extract(ops->subtract(aLo, bLo), 1, 17);
        SValue::Ptr diffHi = ops->extract(ops->subtract(aHi, bHi), 1, 17);
        SValue::Ptr result = ops->concatHiLo(diffHi, diffLo);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Unsigned multiply accumulate accumulate long
struct IP_umaal: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr dLo = d->read(args[0], 32);
        SValue::Ptr dHi = d->read(args[1], 32);
        SValue::Ptr rN = d->read(args[2], 32);
        SValue::Ptr rM = d->read(args[3], 32);
        SValue::Ptr product = ops->unsignedMultiply(rN, rM);
        SValue::Ptr sum1 = ops->add(product, ops->unsignedExtend(dHi, 64));
        SValue::Ptr sum2 = ops->add(sum1, ops->unsignedExtend(dLo, 64));
        SValue::Ptr resultLo = ops->extract(sum2, 0, 32);
        SValue::Ptr resultHi = ops->extract(sum2, 32, 64);
        d->maybeWrite(enabled, args[1], resultHi);
        d->maybeWrite(enabled, args[0], resultLo);
    }
};

// Unsigned multiply accumulate long
struct IP_umlal: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr dLo = d->read(args[0], 32);
        SValue::Ptr dHi = d->read(args[1], 32);
        SValue::Ptr a = d->read(args[2], 32);
        SValue::Ptr b = d->read(args[3], 32);
        SValue::Ptr product = ops->unsignedMultiply(a, b);
        SValue::Ptr accum = ops->concatHiLo(dHi, dLo);
        SValue::Ptr sum = ops->add(product, accum);
        SValue::Ptr resultLo = ops->extract(sum, 0, 32);
        SValue::Ptr resultHi = ops->extract(sum, 32, 64);
        d->maybeWrite(enabled, args[0], resultLo);
        d->maybeWrite(enabled, args[1], resultHi);
        if (insn->get_updatesFlags()) {
            d->maybeWriteRegister(enabled, d->REG_PSTATE_N, d->signBit(sum));
            d->maybeWriteRegister(enabled, d->REG_PSTATE_Z, ops->equalToZero(sum));
        }
    }
};

// Unsigned multiply
struct IP_umull: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr a = d->read(args[2], 32);
        SValue::Ptr b = d->read(args[3], 32);
        SValue::Ptr prod = ops->unsignedMultiply(a, b);
        SValue::Ptr resultLo = ops->extract(prod, 0, 32);
        SValue::Ptr resultHi = ops->extract(prod, 32, 64);
        d->maybeWrite(enabled, args[0], resultLo);
        d->maybeWrite(enabled, args[1], resultHi);
        if (insn->get_updatesFlags()) {
            d->maybeWriteRegister(enabled, d->REG_PSTATE_N, d->signBit(prod));
            d->maybeWriteRegister(enabled, d->REG_PSTATE_Z, ops->equalToZero(prod));
        }
    }
};

// Unsigned saturating add and subtract with exchange
struct IP_uqasx: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->unsignedExtend(ops->extract(a, 0, 16), 17);
        SValue::Ptr aHi = ops->unsignedExtend(ops->extract(a, 16, 32), 17);
        SValue::Ptr bLo = ops->unsignedExtend(ops->extract(b, 0, 16), 17);
        SValue::Ptr bHi = ops->unsignedExtend(ops->extract(b, 16, 32), 17);
        SValue::Ptr diff = d->unsignedSat(ops->subtract(aLo, bHi), 16);
        SValue::Ptr sum = d->unsignedSat(ops->add(aHi, bLo), 16);
        SValue::Ptr result = ops->concatHiLo(sum, diff);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Unsigned saturating add and subtract with exchange
struct IP_uqsax: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->unsignedExtend(ops->extract(a, 0, 16), 17);
        SValue::Ptr aHi = ops->unsignedExtend(ops->extract(a, 16, 32), 17);
        SValue::Ptr bLo = ops->unsignedExtend(ops->extract(b, 0, 16), 17);
        SValue::Ptr bHi = ops->unsignedExtend(ops->extract(b, 16, 32), 17);
        SValue::Ptr sum = d->unsignedSat(ops->add(aLo, bHi), 16);
        SValue::Ptr diff = d->unsignedSat(ops->subtract(aHi, bLo), 16);
        SValue::Ptr result = ops->concatHiLo(diff, sum);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Unsigned saturating subtract 16
struct IP_uqsub16: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->unsignedExtend(ops->extract(a, 0, 16), 17);
        SValue::Ptr aHi = ops->unsignedExtend(ops->extract(a, 16, 32), 17);
        SValue::Ptr bLo = ops->unsignedExtend(ops->extract(b, 0, 16), 17);
        SValue::Ptr bHi = ops->unsignedExtend(ops->extract(b, 16, 32), 17);
        SValue::Ptr diffLo = d->unsignedSat(ops->subtract(aLo, bLo), 16);
        SValue::Ptr diffHi = d->unsignedSat(ops->subtract(aHi, bHi), 16);
        SValue::Ptr result = ops->concatHiLo(diffHi, diffLo);
        d->maybeWrite(enabled, args[0], result);
    }
};

// Unsigned saturating subtract 8
struct IP_uqsub8: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr result;
        for (unsigned int i = 0; i < 4; ++i) {
            SValue::Ptr aPart = ops->unsignedExtend(ops->extract(a, i*8, i*8+8), 9);
            SValue::Ptr bPart = ops->unsignedExtend(ops->extract(b, i*8, i*8+8), 9);
            SValue::Ptr diff = d->unsignedSat(ops->subtract(aPart, bPart), 8);
            result = result ? ops->concatHiLo(diff, result) : diff;
        }
        d->maybeWrite(enabled, args[0], result);
    }
};

// Unsigned sum of absolute differences and accumulate
struct IP_usada8: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 4);
        SValue::Ptr a = d->read(args[2], 32);
        SValue::Ptr b = d->read(args[3], 32);
        SValue::Ptr result;
        for (unsigned i = 0; i < 4; ++i) {
            SValue::Ptr aPart = ops->unsignedExtend(ops->extract(a, i*8, i*8+8), 9);
            SValue::Ptr bPart = ops->unsignedExtend(ops->extract(b, i*8, i*8+8), 9);
            SValue::Ptr diffWide = ops->subtract(aPart, bPart);
            SValue::Ptr resultPart = ops->extract(d->abs(diffWide), 0, 8);
            result = result ? ops->concatHiLo(resultPart, result) : resultPart;
        }
        d->maybeWrite(enabled, args[0], result);
    }
};

// Unsigned saturate
struct IP_usat: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        ASSERT_require(isSgAsmIntegerValueExpression(args[1]));
        size_t nBits = isSgAsmIntegerValueExpression(args[1])->get_absoluteValue();
        SValue::Ptr value = d->read(args[2], 32);

        auto satAndOverflow = d->unsignedSatQ(value, nBits);
        SValue::Ptr sat = std::get<0>(satAndOverflow);
        SValue::Ptr overflowed = std::get<1>(satAndOverflow);

        SValue::Ptr result = sat ? ops->unsignedExtend(sat, 32) : ops->number_(32, 0);
        d->maybeWrite(enabled, args[0], result);
        SValue::Ptr yes = ops->boolean_(true);
        d->maybeWriteRegister(ops->and_(enabled, overflowed), d->REG_PSTATE_Q, yes);
    }
};

// Unsigned add and subtract with exchange
struct IP_usax: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->unsignedExtend(ops->extract(a, 0, 16), 17);
        SValue::Ptr aHi = ops->unsignedExtend(ops->extract(a, 16, 32), 17);
        SValue::Ptr bLo = ops->unsignedExtend(ops->extract(b, 0, 16), 17);
        SValue::Ptr bHi = ops->unsignedExtend(ops->extract(b, 16, 32), 17);
        SValue::Ptr sumWide = ops->add(aLo, bHi);
        SValue::Ptr diffWide = ops->subtract(aHi, bLo);

        SValue::Ptr sumOverflow = d->signBit(sumWide);
        SValue::Ptr diffNonNegative = ops->invert(d->signBit(diffWide));
        SValue::Ptr zeroZero = ops->number_(2, 0);
        SValue::Ptr oneOne = ops->number_(2, 3);
        SValue::Ptr geLo = ops->ite(sumOverflow, oneOne, zeroZero);
        SValue::Ptr geHi = ops->ite(diffNonNegative, oneOne, zeroZero);
        SValue::Ptr ge = ops->concatHiLo(geHi, geLo);

        SValue::Ptr diff = ops->extract(diffWide, 0, 16);
        SValue::Ptr sum = ops->extract(sumWide, 0, 16);
        SValue::Ptr result = ops->concatHiLo(diff, sum);

        d->maybeWrite(enabled, args[0], result);
        d->maybeWriteRegister(enabled, d->REG_PSTATE_GE, ge);
    }
};

// Unsigned subtract 16
struct IP_usub16: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr aLo = ops->unsignedExtend(ops->extract(a, 0, 16), 17);
        SValue::Ptr aHi = ops->unsignedExtend(ops->extract(a, 16, 32), 17);
        SValue::Ptr bLo = ops->unsignedExtend(ops->extract(b, 0, 16), 17);
        SValue::Ptr bHi = ops->unsignedExtend(ops->extract(b, 16, 32), 17);
        SValue::Ptr diffLoWide = ops->subtract(aLo, bLo);
        SValue::Ptr diffHiWide = ops->subtract(aHi, bHi);
        SValue::Ptr diffLoNeg = d->signBit(diffLoWide);
        SValue::Ptr diffHiNeg = d->signBit(diffHiWide);
        SValue::Ptr zero = ops->number_(2, 0);
        SValue::Ptr three = ops->number_(2, 3);
        SValue::Ptr geLo = ops->ite(diffLoNeg, zero, three);
        SValue::Ptr geHi = ops->ite(diffHiNeg, zero, three);
        SValue::Ptr ge = ops->concatHiLo(geHi, geLo);
        SValue::Ptr diffLo = ops->extract(diffLoWide, 0, 16);
        SValue::Ptr diffHi = ops->extract(diffHiWide, 0, 16);
        SValue::Ptr result = ops->concatHiLo(diffHi, diffLo);
        d->maybeWrite(enabled, args[0], result);
        d->maybeWriteRegister(enabled, d->REG_PSTATE_GE, ge);
    }
};

// Unsigned subtract 8
struct IP_usub8: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[2], 32);
        SValue::Ptr result, ge;
        for (unsigned i = 0; i < 4; ++i) {
            SValue::Ptr aPart = ops->unsignedExtend(ops->extract(a, i*8, i*8+8), 9);
            SValue::Ptr bPart = ops->unsignedExtend(ops->extract(b, i*8, i*8+8), 9);
            SValue::Ptr diffWide = ops->subtract(aPart, bPart);
            SValue::Ptr isNonNeg = ops->invert(d->signBit(diffWide));
            SValue::Ptr diff = ops->extract(diffWide, 0, 8);
            result = result ? ops->concatHiLo(diff, result) : diff;
            ge = ge ? ops->concatHiLo(isNonNeg, ge) : isNonNeg;
        }
        d->maybeWrite(enabled, args[0], result);
        d->maybeWriteRegister(enabled, d->REG_PSTATE_GE, ge);
    }
};

// Unsigned extend and add byte 16
struct IP_uxtab16: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1], 32);
        SValue::Ptr b = d->read(args[1], 32);
        SValue::Ptr aLo = ops->extract(a, 0, 16);
        SValue::Ptr aHi = ops->extract(a, 16, 32);
        SValue::Ptr bLo = ops->unsignedExtend(ops->extract(b, 0, 8), 16);
        SValue::Ptr bHi = ops->unsignedExtend(ops->extract(b, 16, 24), 16);
        SValue::Ptr sumLo = ops->add(aLo, bLo);
        SValue::Ptr sumHi = ops->add(aHi, bHi);
        SValue::Ptr result = ops->concatHiLo(sumHi, sumLo);
        d->maybeWrite(enabled, args[0], result);
    }
};

// extract 8-bit value from register and zero extend to 32 bits
struct IP_uxtb: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr u8 = d->read(args[1], 8);
        SValue::Ptr u32 = ops->unsignedExtend(u8, 32);
        d->maybeWrite(enabled, args[0], u32);
    }
};

// Extract a 16-bit value from a register and zero extends it to 32 bits
struct IP_uxth: P {
    void p(D d, Ops ops, I insn, A args, V enabled) {
        assert_args(insn, args, 2);
        SValue::Ptr u16 = d->read(args[1], 16);
        SValue::Ptr u32 = ops->unsignedExtend(u16, 32);
        d->maybeWrite(enabled, args[0], u32);
    }
};

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DispatcherAarch32::DispatcherAarch32() {}

DispatcherAarch32::DispatcherAarch32(const Architecture::Base::ConstPtr &arch)
    : BaseSemantics::Dispatcher(arch) {}

DispatcherAarch32::DispatcherAarch32(const Architecture::Base::ConstPtr &arch, const BaseSemantics::RiscOperators::Ptr &ops)
    : BaseSemantics::Dispatcher(arch, ops) {
    initializeRegisterDescriptors();
    initializeInsnDispatchTable();
    initializeMemory();
    initializeState(ops->currentState());
}

DispatcherAarch32::~DispatcherAarch32() {}

DispatcherAarch32::Ptr
DispatcherAarch32::instance(const Architecture::Base::ConstPtr &arch) {
    return Ptr(new DispatcherAarch32(arch));
}

DispatcherAarch32::Ptr
DispatcherAarch32::instance(const Architecture::Base::ConstPtr &arch, const BaseSemantics::RiscOperators::Ptr &ops) {
    return Ptr(new DispatcherAarch32(arch, ops));
}

BaseSemantics::Dispatcher::Ptr
DispatcherAarch32::create(const BaseSemantics::RiscOperators::Ptr &ops) const {
    return instance(architecture(), ops);
}

DispatcherAarch32::Ptr
DispatcherAarch32::promote(const BaseSemantics::Dispatcher::Ptr &d) {
    Ptr retval = as<DispatcherAarch32>(d);
    ASSERT_not_null(retval);
    return retval;
}

void
DispatcherAarch32::initializeInsnDispatchTable() {
    iprocSet(ARM_INS_ADC,      new Aarch32::IP_adc);
    iprocSet(ARM_INS_ADD,      new Aarch32::IP_add);
    iprocSet(ARM_INS_AND,      new Aarch32::IP_and);
    iprocSet(ARM_INS_ASR,      new Aarch32::IP_mov);   // ASR is a special case of MOV
    iprocSet(ARM_INS_B,        new Aarch32::IP_b);
    iprocSet(ARM_INS_BFC,      new Aarch32::IP_bfc);
    iprocSet(ARM_INS_BFI,      new Aarch32::IP_bfi);
    iprocSet(ARM_INS_BIC,      new Aarch32::IP_bic);
    iprocSet(ARM_INS_BL,       new Aarch32::IP_bl);
    iprocSet(ARM_INS_BLX,      new Aarch32::IP_blx);
    iprocSet(ARM_INS_BX,       new Aarch32::IP_bx);
    iprocSet(ARM_INS_CLZ,      new Aarch32::IP_clz);
    iprocSet(ARM_INS_CMN,      new Aarch32::IP_cmn);
    iprocSet(ARM_INS_CMP,      new Aarch32::IP_cmp);
    iprocSet(ARM_INS_CPS,      new Aarch32::IP_cps);
    iprocSet(ARM_INS_DSB,      new Aarch32::IP_dsb);
    iprocSet(ARM_INS_EOR,      new Aarch32::IP_eor);
    iprocSet(ARM_INS_HVC,      new Aarch32::IP_hvc);
    iprocSet(ARM_INS_LDC,      new Aarch32::IP_ldc);
    iprocSet(ARM_INS_LDM,      new Aarch32::IP_ldm);
    iprocSet(ARM_INS_LDMDA,    new Aarch32::IP_ldmda);
    iprocSet(ARM_INS_LDMDB,    new Aarch32::IP_ldmdb);
    iprocSet(ARM_INS_LDMIB,    new Aarch32::IP_ldmib);
    iprocSet(ARM_INS_LDR,      new Aarch32::IP_ldr);
    iprocSet(ARM_INS_LDRB,     new Aarch32::IP_ldrb);
    iprocSet(ARM_INS_LDRBT,    new Aarch32::IP_ldrbt);
    iprocSet(ARM_INS_LDREX,    new Aarch32::IP_ldrex);
    iprocSet(ARM_INS_LDRD,     new Aarch32::IP_ldrd);
    iprocSet(ARM_INS_LDRH,     new Aarch32::IP_ldrh);
    iprocSet(ARM_INS_LDRHT,    new Aarch32::IP_ldrht);
    iprocSet(ARM_INS_LDRSB,    new Aarch32::IP_ldrsb);
    iprocSet(ARM_INS_LDRSBT,   new Aarch32::IP_ldrsbt);
    iprocSet(ARM_INS_LDRSH,    new Aarch32::IP_ldrsh);
    iprocSet(ARM_INS_LDRSHT,   new Aarch32::IP_ldrsht);
    iprocSet(ARM_INS_LDRT,     new Aarch32::IP_ldrt);
    iprocSet(ARM_INS_LSL,      new Aarch32::IP_mov);    // LSL is a special case of MOV
    iprocSet(ARM_INS_LSR,      new Aarch32::IP_mov);    // LSR is a special case of MOV
    iprocSet(ARM_INS_MCR,      new Aarch32::IP_mcr);
    iprocSet(ARM_INS_MCRR,     new Aarch32::IP_mcrr);
    iprocSet(ARM_INS_MLA,      new Aarch32::IP_mla);
    iprocSet(ARM_INS_MLS,      new Aarch32::IP_mls);
    iprocSet(ARM_INS_MOV,      new Aarch32::IP_mov);
    iprocSet(ARM_INS_MOVT,     new Aarch32::IP_movt);
    iprocSet(ARM_INS_MOVW,     new Aarch32::IP_movw);
    iprocSet(ARM_INS_MRC,      new Aarch32::IP_mrc);
    iprocSet(ARM_INS_MRS,      new Aarch32::IP_mrs);
    iprocSet(ARM_INS_MSR,      new Aarch32::IP_msr);
    iprocSet(ARM_INS_MUL,      new Aarch32::IP_mul);
    iprocSet(ARM_INS_MVN,      new Aarch32::IP_mvn);
    iprocSet(ARM_INS_ORR,      new Aarch32::IP_orr);
    iprocSet(ARM_INS_PKHBT,    new Aarch32::IP_pkhbt);
    iprocSet(ARM_INS_PKHTB,    new Aarch32::IP_pkhtb);
    iprocSet(ARM_INS_PLD,      new Aarch32::IP_pld);
    iprocSet(ARM_INS_POP,      new Aarch32::IP_pop);
    iprocSet(ARM_INS_PUSH,     new Aarch32::IP_push);
    iprocSet(ARM_INS_QADD,     new Aarch32::IP_qadd);
    iprocSet(ARM_INS_QADD16,   new Aarch32::IP_qadd16);
    iprocSet(ARM_INS_QADD8,    new Aarch32::IP_qadd8);
    iprocSet(ARM_INS_QASX,     new Aarch32::IP_qasx);
    iprocSet(ARM_INS_QDADD,    new Aarch32::IP_qdadd);
    iprocSet(ARM_INS_QDSUB,    new Aarch32::IP_qdsub);
    iprocSet(ARM_INS_QSAX,     new Aarch32::IP_qsax);
    iprocSet(ARM_INS_QSUB,     new Aarch32::IP_qsub);
    iprocSet(ARM_INS_QSUB16,   new Aarch32::IP_qsub16);
    iprocSet(ARM_INS_QSUB8,    new Aarch32::IP_qsub8);
    iprocSet(ARM_INS_RFEDA,    new Aarch32::IP_rfeda);
    iprocSet(ARM_INS_RFEDB,    new Aarch32::IP_rfedb);
    iprocSet(ARM_INS_RFEIA,    new Aarch32::IP_rfeia);
    iprocSet(ARM_INS_RFEIB,    new Aarch32::IP_rfeib);
    iprocSet(ARM_INS_ROR,      new Aarch32::IP_mov);    // ROR is a special case of MOV
    iprocSet(ARM_INS_RSB,      new Aarch32::IP_rsb);
    iprocSet(ARM_INS_RSC,      new Aarch32::IP_rsc);
    iprocSet(ARM_INS_SADD16,   new Aarch32::IP_sadd16);
    iprocSet(ARM_INS_SADD8,    new Aarch32::IP_sadd8);
    iprocSet(ARM_INS_SASX,     new Aarch32::IP_sasx);
    iprocSet(ARM_INS_SBC,      new Aarch32::IP_sbc);
    iprocSet(ARM_INS_SBFX,     new Aarch32::IP_sbfx);
    iprocSet(ARM_INS_SEL,      new Aarch32::IP_sel);
    iprocSet(ARM_INS_SHADD16,  new Aarch32::IP_shadd16);
    iprocSet(ARM_INS_SHADD8,   new Aarch32::IP_shadd8);
    iprocSet(ARM_INS_SHASX,    new Aarch32::IP_shasx);
    iprocSet(ARM_INS_SHSAX,    new Aarch32::IP_shsax);
    iprocSet(ARM_INS_SHSUB16,  new Aarch32::IP_shsub16);
    iprocSet(ARM_INS_SHSUB8,   new Aarch32::IP_shsub8);
    iprocSet(ARM_INS_SMLABB,   new Aarch32::IP_smlabb);
    iprocSet(ARM_INS_SMLABT,   new Aarch32::IP_smlabt);
    iprocSet(ARM_INS_SMLAD,    new Aarch32::IP_smlad);
    iprocSet(ARM_INS_SMLADX,   new Aarch32::IP_smladx);
    iprocSet(ARM_INS_SMLAL,    new Aarch32::IP_smlal);
    iprocSet(ARM_INS_SMLALBB,  new Aarch32::IP_smlalbb);
    iprocSet(ARM_INS_SMLALBT,  new Aarch32::IP_smlalbt);
    iprocSet(ARM_INS_SMLALD,   new Aarch32::IP_smlald);
    iprocSet(ARM_INS_SMLALDX,  new Aarch32::IP_smlaldx);
    iprocSet(ARM_INS_SMLALTB,  new Aarch32::IP_smlaltb);
    iprocSet(ARM_INS_SMLALTT,  new Aarch32::IP_smlaltt);
    iprocSet(ARM_INS_SMLATB,   new Aarch32::IP_smlatb);
    iprocSet(ARM_INS_SMLATT,   new Aarch32::IP_smlatt);
    iprocSet(ARM_INS_SMLAWB,   new Aarch32::IP_smlawb);
    iprocSet(ARM_INS_SMLAWT,   new Aarch32::IP_smlawt);
    iprocSet(ARM_INS_SMLSD,    new Aarch32::IP_smlsd);
    iprocSet(ARM_INS_SMLSDX,   new Aarch32::IP_smlsdx);
    iprocSet(ARM_INS_SMMLA,    new Aarch32::IP_smmla);
    iprocSet(ARM_INS_SMMLAR,   new Aarch32::IP_smmlar);
    iprocSet(ARM_INS_SMMLS,    new Aarch32::IP_smmls);
    iprocSet(ARM_INS_SMMLSR,   new Aarch32::IP_smmlsr);
    iprocSet(ARM_INS_SMULBB,   new Aarch32::IP_smulbb);
    iprocSet(ARM_INS_SMULBT,   new Aarch32::IP_smulbt);
    iprocSet(ARM_INS_SMULL,    new Aarch32::IP_smull);
    iprocSet(ARM_INS_SMULTB,   new Aarch32::IP_smultb);
    iprocSet(ARM_INS_SMULTT,   new Aarch32::IP_smultt);
    iprocSet(ARM_INS_SMULWB,   new Aarch32::IP_smulwb);
    iprocSet(ARM_INS_SMULWT,   new Aarch32::IP_smulwt);
    iprocSet(ARM_INS_SSAT,     new Aarch32::IP_ssat);
    iprocSet(ARM_INS_SSAX,     new Aarch32::IP_ssax);
    iprocSet(ARM_INS_SSUB16,   new Aarch32::IP_ssub16);
    iprocSet(ARM_INS_SSUB8,    new Aarch32::IP_ssub8);
    iprocSet(ARM_INS_STC,      new Aarch32::IP_stc);
    iprocSet(ARM_INS_STM,      new Aarch32::IP_stm);
    iprocSet(ARM_INS_STMDA,    new Aarch32::IP_stmda);
    iprocSet(ARM_INS_STMDB,    new Aarch32::IP_stmdb);
    iprocSet(ARM_INS_STMIB,    new Aarch32::IP_stmib);
    iprocSet(ARM_INS_STR,      new Aarch32::IP_str);
    iprocSet(ARM_INS_STRB,     new Aarch32::IP_strb);
    iprocSet(ARM_INS_STRBT,    new Aarch32::IP_strbt);
    iprocSet(ARM_INS_STRD,     new Aarch32::IP_strd);
    iprocSet(ARM_INS_STRH,     new Aarch32::IP_strh);
    iprocSet(ARM_INS_STRHT,    new Aarch32::IP_strht);
    iprocSet(ARM_INS_STRT,     new Aarch32::IP_strt);
    iprocSet(ARM_INS_SUB,      new Aarch32::IP_sub);
    iprocSet(ARM_INS_SVC,      new Aarch32::IP_svc);
    iprocSet(ARM_INS_SXTAB,    new Aarch32::IP_sxtab);
    iprocSet(ARM_INS_SXTAB16,  new Aarch32::IP_sxtab16);
    iprocSet(ARM_INS_SXTAH,    new Aarch32::IP_sxtah);
    iprocSet(ARM_INS_TEQ,      new Aarch32::IP_teq);
    iprocSet(ARM_INS_TST,      new Aarch32::IP_tst);
    iprocSet(ARM_INS_UADD16,   new Aarch32::IP_uadd16);
    iprocSet(ARM_INS_UADD8,    new Aarch32::IP_uadd8);
    iprocSet(ARM_INS_UASX,     new Aarch32::IP_uasx);
    iprocSet(ARM_INS_UBFX,     new Aarch32::IP_ubfx);
    iprocSet(ARM_INS_UDF,      new Aarch32::IP_udf);
    iprocSet(ARM_INS_UHADD16,  new Aarch32::IP_uhadd16);
    iprocSet(ARM_INS_UHADD8,   new Aarch32::IP_uhadd8);
    iprocSet(ARM_INS_UHASX,    new Aarch32::IP_uhasx);
    iprocSet(ARM_INS_UHSAX,    new Aarch32::IP_uhsax);
    iprocSet(ARM_INS_UHSUB16,  new Aarch32::IP_uhsub16);
    iprocSet(ARM_INS_UMAAL,    new Aarch32::IP_umaal);
    iprocSet(ARM_INS_UMLAL,    new Aarch32::IP_umlal);
    iprocSet(ARM_INS_UMULL,    new Aarch32::IP_umull);
    iprocSet(ARM_INS_UQASX,    new Aarch32::IP_uqasx);
    iprocSet(ARM_INS_UQSAX,    new Aarch32::IP_uqsax);
    iprocSet(ARM_INS_UQSUB16,  new Aarch32::IP_uqsub16);
    iprocSet(ARM_INS_UQSUB8,   new Aarch32::IP_uqsub8);
    iprocSet(ARM_INS_USADA8,   new Aarch32::IP_usada8);
    iprocSet(ARM_INS_USAT,     new Aarch32::IP_usat);
    iprocSet(ARM_INS_USAX,     new Aarch32::IP_usax);
    iprocSet(ARM_INS_USUB16,   new Aarch32::IP_usub16);
    iprocSet(ARM_INS_USUB8,    new Aarch32::IP_usub8);
    iprocSet(ARM_INS_UXTAB16,  new Aarch32::IP_uxtab16);
    iprocSet(ARM_INS_UXTB,     new Aarch32::IP_uxtb);
    iprocSet(ARM_INS_UXTH,     new Aarch32::IP_uxth);
}

void
DispatcherAarch32::initializeRegisterDescriptors() {
    REG_PC = findRegister("pc", 32);
    REG_SP = findRegister("sp", 32);
    REG_LR = findRegister("lr", 32);

    REG_CPSR = findRegister("cpsr", 32);
    REG_PSTATE_N = findRegister("cpsr_n", 1);
    REG_PSTATE_Z = findRegister("cpsr_z", 1);
    REG_PSTATE_C = findRegister("cpsr_c", 1);
    REG_PSTATE_V = findRegister("cpsr_v", 1);
    REG_PSTATE_T = findRegister("cpsr_t", 1);
    REG_PSTATE_E = findRegister("cpsr_e", 1);
    REG_PSTATE_Q = findRegister("cpsr_q", 1);
    REG_PSTATE_NZCV = findRegister("cpsr_nzcv", 4);
    REG_PSTATE_GE = findRegister("cpsr_ge", 4);

    REG_SPSR = findRegister("spsr", 32);
    REG_DTRTX = findRegister("dtrtx", 32);

    REG_UNKNOWN = findRegister("unknown", 32);
}

void
DispatcherAarch32::initializeMemory() {
    if (BaseSemantics::State::Ptr state = currentState()) {
        if (BaseSemantics::MemoryState::Ptr memory = state->memoryState()) {
            switch (memory->get_byteOrder()) {
                case ByteOrder::ORDER_LSB:
                    break;
                case ByteOrder::ORDER_MSB:
                    mlog[WARN] <<"AArch32 memory state is using big-endian byte order\n";
                    break;
                case ByteOrder::ORDER_UNSPECIFIED:
                    memory->set_byteOrder(ByteOrder::ORDER_LSB);
                    break;
            }
        }
    }
}

RegisterDescriptor
DispatcherAarch32::instructionPointerRegister() const {
    return REG_PC;
}

RegisterDescriptor
DispatcherAarch32::stackPointerRegister() const {
    return REG_SP;
}

RegisterDescriptor
DispatcherAarch32::callReturnRegister() const {
    return REG_LR;
}

int
DispatcherAarch32::iprocKey(SgAsmInstruction *insn_) const {
    auto insn = isSgAsmAarch32Instruction(insn_);
    ASSERT_not_null(insn);
    return insn->get_kind();
}

SValue::Ptr
DispatcherAarch32::read(SgAsmExpression *e, size_t value_nbits/*=0*/, size_t addr_nbits/*=0*/) {
    // Some registers are special
    if (auto rre = isSgAsmDirectRegisterExpression(e)) {
        SValue::Ptr retval;
        if (rre->get_descriptor() == REG_UNKNOWN) {
            // Reading from the special "unknown" register will return a new unknown value every time it's read.
            retval = operators()->undefined_(REG_UNKNOWN.nBits());
        } else if (rre->get_descriptor() == REG_PC) {
            // Reading from the PC register (r15) adds either 4 or 8 depending on the current instruction set.
            SgAsmInstruction *insn = AST::Traversal::findParentTyped<SgAsmInstruction>(e);
            retval = readIpRegister(insn);
        }

        if (retval) {
            if (value_nbits != 0 && value_nbits != REG_UNKNOWN.nBits())
                retval = operators()->unsignedExtend(retval, value_nbits);
            return retval;
        }
    }

    // Some arguments access discontiguous parts of a single register. ROSE can't represent that as a single
    // RegisterDescriptor, so it gets packed into a SgAsmRegisterNames node instead. We'll read all the parts and fill in the
    // unaccessed parts with zeros.
    if (auto regList = isSgAsmRegisterNames(e)) {
        ASSERT_require(!regList->get_registers().empty());
        std::vector<RegisterDescriptor> regs;
        for (SgAsmRegisterReferenceExpression *reg: regList->get_registers()) {
            ASSERT_not_null(reg);
            regs.push_back(reg->get_descriptor());
        }
        std::sort(regs.begin(), regs.end(), [](RegisterDescriptor a, RegisterDescriptor b) {
                return a.offset() < b.offset();
            });
        for (size_t i = 1; i < regs.size(); ++i) {
            ASSERT_require(regs[i].majorNumber() == regs[0].majorNumber());
            ASSERT_require(regs[i].minorNumber() == regs[0].minorNumber());
            ASSERT_require(regs[i].offset() >= regs[i-1].offset() + regs[i-1].nBits());
        }
        SValue::Ptr retval;
        for (RegisterDescriptor reg: regs) {
            SValue::Ptr value = operators()->readRegister(reg);
            if (!retval) {
                if (reg.offset() > 0) {
                    retval = operators()->number_(reg.offset(), 0);
                    retval = operators()->concatHiLo(value, retval);
                } else {
                    retval = value;
                }
            } else if (reg.offset() > retval->nBits()) {
                SValue::Ptr zeros = operators()->number_(reg.offset() - retval->nBits(), 0);
                retval = operators()->concatHiLo(zeros, retval);
                retval = operators()->concatHiLo(value, retval);
            } else {
                retval = operators()->concatHiLo(value, retval);
            }
        }
        size_t needWidth = value_nbits == 0 ? 32 : value_nbits;
        retval = operators()->unsignedExtend(retval, needWidth);
        return retval;
    }

    return Dispatcher::read(e, value_nbits, addr_nbits);
}

void
DispatcherAarch32::write(SgAsmExpression *e, const SValue::Ptr &value, size_t addr_nbits/*=0*/) {
    // Writes to the special "unknown" register are ignored.
    if (auto rre = isSgAsmDirectRegisterExpression(e)) {
        if (rre->get_descriptor() == REG_UNKNOWN)
            return;
    }

    // Writes to a register list write just those parts
    if (auto regList = isSgAsmRegisterNames(e)) {
        for (size_t i = 0; i < regList->get_registers().size(); ++i) {
            ASSERT_require(isSgAsmDirectRegisterExpression(regList->get_registers()[i]));
            RegisterDescriptor reg = isSgAsmDirectRegisterExpression(regList->get_registers()[i])->get_descriptor();
            ASSERT_require(reg.offset() + reg.nBits() <= value->nBits());
            SValue::Ptr part = operators()->extract(value, reg.offset(), reg.offset() + reg.nBits());
            operators()->writeRegister(reg, part);
        }
        return;
    }

    return Dispatcher::write(e, value, addr_nbits);
}

SValue::Ptr
DispatcherAarch32::conditionHolds(Aarch32InstructionCondition cond) {
    // FIXME[Robb Matzke 2021-01-27]: Surely not all of these can occur in the instruction condition field which is only three
    // bits wide!
    switch (cond) {
        case Aarch32InstructionCondition::ARM_CC_INVALID:
            ASSERT_not_reachable("[Robb Matzke 2021-01-27]");
        case Aarch32InstructionCondition::ARM_CC_EQ:    // equal (z set)
            return operators()->readRegister(REG_PSTATE_Z);

        case Aarch32InstructionCondition::ARM_CC_NE:    // not equal: not equal, or unordered (z clear)
            return operators()->invert(operators()->readRegister(REG_PSTATE_Z));

        case Aarch32InstructionCondition::ARM_CC_HS:    // unsigned higher or same: >, ==, or unordered (c set)
            return operators()->readRegister(REG_PSTATE_C);

        case Aarch32InstructionCondition::ARM_CC_LO:    // unsigned lower or same: less than (c clear)
            return operators()->invert(operators()->readRegister(REG_PSTATE_C));

        case Aarch32InstructionCondition::ARM_CC_MI:    // minus, negative: less than (n set)
            return operators()->readRegister(REG_PSTATE_N);

        case Aarch32InstructionCondition::ARM_CC_PL:    // plus, positive or zero: >, ==, or unordered (n clear)
            return operators()->invert(operators()->readRegister(REG_PSTATE_N));

        case Aarch32InstructionCondition::ARM_CC_VS:    // overflow: unordered (v set)
            return operators()->readRegister(REG_PSTATE_V);

        case Aarch32InstructionCondition::ARM_CC_VC:    // no overflow: ordered (v clear)
            return operators()->invert(operators()->readRegister(REG_PSTATE_V));

        case Aarch32InstructionCondition::ARM_CC_HI: {  // unsigned higher: greater than, or unordered (c set and z clear)
            SValue::Ptr cset = operators()->readRegister(REG_PSTATE_C);
            SValue::Ptr zclear = operators()->invert(operators()->readRegister(REG_PSTATE_Z));
            return operators()->and_(cset, zclear);
        }

        case Aarch32InstructionCondition::ARM_CC_LS: {  // unsigned lower or same: less than or equal (c clear or z set)
            SValue::Ptr cclear = operators()->invert(operators()->readRegister(REG_PSTATE_C));
            SValue::Ptr zset = operators()->readRegister(REG_PSTATE_Z);
            return operators()->or_(cclear, zset);
        }

        case Aarch32InstructionCondition::ARM_CC_GE: {  // greater than or equal: greater than or equal (n == v)
            SValue::Ptr n = operators()->readRegister(REG_PSTATE_N);
            SValue::Ptr v = operators()->readRegister(REG_PSTATE_V);
            return operators()->invert(operators()->xor_(n, v));
        }

        case Aarch32InstructionCondition::ARM_CC_LT: {  // less than: less than, or unordered (n != v)
            SValue::Ptr n = operators()->readRegister(REG_PSTATE_N);
            SValue::Ptr v = operators()->readRegister(REG_PSTATE_V);
            return operators()->xor_(n, v);
        }

        case Aarch32InstructionCondition::ARM_CC_GT: {  // signed greater than: greater than (z clear, n and v the same)
            // The ARM documentation says "z clear, n and v the same". I'm interpreting the comma as an "and". I found other
            // user documentation that says "Z = 0 & N = V".
            SValue::Ptr zclear = operators()->invert(operators()->readRegister(REG_PSTATE_Z));
            SValue::Ptr n = operators()->readRegister(REG_PSTATE_N);
            SValue::Ptr v = operators()->readRegister(REG_PSTATE_V);
            SValue::Ptr nvsame = operators()->invert(operators()->xor_(n, v));
            return operators()->and_(zclear, nvsame);
        }

        case Aarch32InstructionCondition::ARM_CC_LE: {  // signed less than or equal: <, ==, or unorderd (z set, n and v differ)
            // The ARM documentation I found says "z set, n and v differ", which is not the inverse of ARM_CC_GT's description.
            // I found user documentation that says "Z=1 or N=!V" (I'm parsing "N=!V" as "N != V".
            SValue::Ptr zset = operators()->readRegister(REG_PSTATE_Z);
            SValue::Ptr n = operators()->readRegister(REG_PSTATE_N);
            SValue::Ptr v = operators()->readRegister(REG_PSTATE_V);
            SValue::Ptr nvdiffer = operators()->xor_(n, v);
            return operators()->or_(zset, nvdiffer);
        }

        case Aarch32InstructionCondition::ARM_CC_AL:      // always (unconditional): always (unconditional)
            return operators()->boolean_(true);
    }
    ASSERT_not_reachable("invalid condition");
}

void
DispatcherAarch32::maybeWriteRegister(const SValue::Ptr &enabled, RegisterDescriptor reg, const SValue::Ptr &value) {
    ASSERT_not_null(enabled);
    ASSERT_require(enabled->nBits() == 1);
    ASSERT_require(reg.isValid());
    ASSERT_not_null(value);
    ASSERT_require(value->nBits() == reg.nBits());

    operators()->isNoopRead(true);
    SValue::Ptr oldValue = operators()->readRegister(reg);
    operators()->isNoopRead(false);
    SValue::Ptr toWrite = operators()->ite(enabled, value, oldValue);
    operators()->writeRegister(reg, toWrite);
}

void
DispatcherAarch32::maybeWriteMemory(const SValue::Ptr &enabled, const SValue::Ptr &address, const SValue::Ptr &value) {
    ASSERT_not_null(enabled);
    ASSERT_require(enabled->nBits() == 1);
    ASSERT_not_null(address);
    ASSERT_not_null(value);

    SValue::Ptr dflt = operators()->undefined_(value->nBits());
    operators()->isNoopRead(true);
    SValue::Ptr oldValue = operators()->readMemory(RegisterDescriptor(), address, dflt, operators()->boolean_(true));
    operators()->isNoopRead(false);
    SValue::Ptr newValue = operators()->ite(enabled, value, oldValue);
    operators()->writeMemory(RegisterDescriptor(), address, newValue, operators()->boolean_(true));
}

void
DispatcherAarch32::maybeWrite(const SValue::Ptr &enabled, SgAsmExpression *destination, const SValue::Ptr &value) {
    ASSERT_not_null(enabled);
    ASSERT_require(enabled->nBits() == 1);
    ASSERT_not_null(destination);

    operators()->isNoopRead(true);
    SValue::Ptr oldValue = read(destination, value->nBits());
    operators()->isNoopRead(false);
    SValue::Ptr toWrite = operators()->ite(enabled, value, oldValue);
    write(destination, toWrite);
}

bool
DispatcherAarch32::isIpRegister(SgAsmExpression *expr) {
    ASSERT_not_null(expr);
    return isSgAsmDirectRegisterExpression(expr) && isSgAsmDirectRegisterExpression(expr)->get_descriptor() == REG_PC;
}

SValue::Ptr
DispatcherAarch32::readIpRegister(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);

    // According to ARM documentation, reading the r15 (pc) register also adds 8 when in A32 mode, or 4 in T32 mode. We may
    // have already adjusted the register to point to the next instruction, so this function will calculate it anew from the
    // current instruction.
    SValue::Ptr t32 = isT32Mode();
    SValue::Ptr four = operators()->number_(32, 4);
    SValue::Ptr eight = operators()->number_(32, 8);
    SValue::Ptr offset = operators()->ite(t32, four, eight);
    SValue::Ptr base = operators()->number_(32, insn->get_address());
    SValue::Ptr retval = operators()->add(base, offset);
    return retval;
}

SValue::Ptr
DispatcherAarch32::isT32Mode() {
    return operators()->readRegister(REG_PSTATE_T);
}

SValue::Ptr
DispatcherAarch32::isA32Mode() {
    return operators()->invert(isT32Mode());
}

void
DispatcherAarch32::setThumbMode(const SValue::Ptr &b) {
    ASSERT_not_null(b);
    ASSERT_require(b->nBits() == 1);
    operators()->writeRegister(REG_PSTATE_T, b);
}

void
DispatcherAarch32::setThumbMode(const SValue::Ptr &enabled, const SValue::Ptr &b) {
    maybeWriteRegister(enabled, REG_PSTATE_T, b);
}

void
DispatcherAarch32::setThumbMode(SgAsmAarch32Instruction *insn) {
    operators()->writeRegister(REG_PSTATE_T, operators()->boolean_(2 == insn->get_size()));
}

SValue::Ptr
DispatcherAarch32::signBit(const SValue::Ptr &value) {
    ASSERT_not_null(value);
    return part(value, value->nBits()-1);
}

RegisterDescriptor
DispatcherAarch32::accessedRegister(SgAsmExpression *expr) {
    if (isSgAsmDirectRegisterExpression(expr)) {
        return isSgAsmDirectRegisterExpression(expr)->get_descriptor();
    } else if (auto op = isSgAsmBinaryPreupdate(expr)) {
        return accessedRegister(op->get_lhs());
    } else if (auto op = isSgAsmBinaryPostupdate(expr)) {
        return accessedRegister(op->get_lhs());
    } else if (auto op = isSgAsmRegisterNames(expr)) {
        // Assumes that all the registers have the same major and minor numbers, and that a 32 bit register is being accessed.
        ASSERT_require(!op->get_registers().empty());
        std::vector<RegisterDescriptor> regs;
        for (SgAsmRegisterReferenceExpression *reg: op->get_registers()) {
            ASSERT_not_null(reg);
            regs.push_back(reg->get_descriptor());
        }
        std::sort(regs.begin(), regs.end(), [](RegisterDescriptor a, RegisterDescriptor b) {
                return a.offset() < b.offset();
            });
        for (size_t i = 1; i < regs.size(); ++i) {
            ASSERT_require(regs[i].majorNumber() == regs[0].majorNumber());
            ASSERT_require(regs[i].minorNumber() == regs[0].minorNumber());
            ASSERT_require(regs[i].offset() >= regs[i-1].offset() + regs[i-1].nBits());
        }
        ASSERT_require(regs.back().offset() + regs.back().nBits() <= 32);
        return RegisterDescriptor(regs[0].majorNumber(), regs[0].minorNumber(), 0, 32);
    }
    ASSERT_not_reachable("no register accessed");
}

bool
DispatcherAarch32::mustBeSet(const SValue::Ptr &bit) {
    return bit && bit->nBits() == 1 && bit->isTrue();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The following operations are similar in name and function to those described in the ARM documentation. I'm trying to keep
// the names and arguments as similar as possible in order to get the easiest interoperability between the description in the
// documentation and the implementation in ROSE. The ARM documentation spells these "AARCH32", but they're really AArch32
// functions.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Corresponds to ARM32's V<m:n> operator, which extracts bits n (inclusive) through m (inclusive) where m >= n and the width
// of V is at least m+1 bits in width.  We name this operation "part" instead of "extract" because we want the arguments to
// be in the same order as the ARM pseudo code, and ROSE uses "extract" already with the args in the opposite order.
SValue::Ptr
DispatcherAarch32::part(const SValue::Ptr &value, size_t maxBit, size_t minBit) {
    ASSERT_not_null(value);
    ASSERT_require(maxBit >= minBit);
    ASSERT_require(maxBit < value->nBits());
    return operators()->extract(value, minBit, maxBit+1);
}

// Corresponds to ARM32's V<n> operator, which returns the bit at position N.
SValue::Ptr
DispatcherAarch32::part(const SValue::Ptr &value, size_t n) {
    ASSERT_not_null(value);
    ASSERT_require(n < value->nBits());
    return operators()->extract(value, n, n+1);
}

// This is the ARM32 H:L operator that concatenates H and L where H are the high-order bits and L are the low-order bits. We name
// this operation "join" instead of "concate" because we want the order of operands to match the ARM32 pseudo code.
SValue::Ptr
DispatcherAarch32::join(const SValue::Ptr &hi, const SValue::Ptr &lo) {
    ASSERT_not_null(hi);
    ASSERT_not_null(lo);
    return operators()->concat(lo, hi);
}

// See ARM32 A32ExpandImm_C
DispatcherAarch32::TwoValues
DispatcherAarch32::a32ExpandImmC(const SValue::Ptr &/*imm12*/) {
#if 0 // [Robb Matzke 2021-02-05]
    ASSERT_not_null(imm12);
    auto carryIn = operators()->readRegister(REG_PSTATE_C);
    auto unrotatedValue = zeroExtend(part(imm12, 7, 0), 12);;
    return ShiftC(unrotatedValue, SrType::ROR, times2(part(imm12, 11, 8), carryIn));
#else
    ASSERT_not_implemented("[Robb Matzke 2021-02-05]");
#endif
}

// See ARM32 ZeroExtend
SValue::Ptr
DispatcherAarch32::zeroExtend(const SValue::Ptr &value, size_t n) {
    ASSERT_not_null(value);
    ASSERT_require(n >= value->nBits());
    if (value->nBits() == n) {
        return value;
    } else {
        return operators()->unsignedExtend(value, n);
    }
}

// See ARM32 Zeros
SValue::Ptr
DispatcherAarch32::makeZeros(size_t n) {
    ASSERT_require(n > 0);
    return operators()->number_(n, 0);
}

// See ARM32 Shift_C
DispatcherAarch32::TwoValues
DispatcherAarch32::shiftC(const SValue::Ptr &value, SrType srType, int amount, const SValue::Ptr &carryIn) {
    ASSERT_not_null(value);
    ASSERT_not_null(carryIn);
    ASSERT_require(carryIn->nBits() == 1);
    ASSERT_forbid(SrType::RRX == srType && amount != 1);
    if (0 == amount)
        return {value, carryIn};
    switch (srType) {
        case SrType::LSL: return lslC(value, amount);
        case SrType::LSR: return lsrC(value, amount);
        case SrType::ASR: return asrC(value, amount);
        case SrType::ROR: return rorC(value, amount);
        case SrType::RRX: return rrxC(value, carryIn);
    }
    ASSERT_not_reachable("invalid srType");
}

// See ARM32 LSL_C
DispatcherAarch32::TwoValues
DispatcherAarch32::lslC(const SValue::Ptr &x, size_t shift) {
    ASSERT_not_null(x);
    ASSERT_require(shift > 0);
    const size_t n = x->nBits();
    auto extendedX = join(x, makeZeros(shift));
    auto result = part(extendedX, n-1, 0);
    auto carryOut = part(extendedX, n);
    return {result, carryOut};
}

// See ARM32 LSR_C
DispatcherAarch32::TwoValues
DispatcherAarch32::lsrC(const SValue::Ptr &x, size_t shift) {
    ASSERT_not_null(x);
    ASSERT_require(shift > 0);
    const size_t n = x->nBits();
    auto extendedX = zeroExtend(x, shift + n);
    auto result = part(extendedX, shift + n - 1, shift);
    auto carryOut = part(extendedX, shift - 1);
    return {result, carryOut};
}

// See ARM32 ASR_C
DispatcherAarch32::TwoValues
DispatcherAarch32::asrC(const SValue::Ptr &x, size_t shift) {
    ASSERT_not_null(x);
    ASSERT_require(shift > 0);
    const size_t n = x->nBits();
    auto extendedX = signExtend(x, shift + n);
    auto result = part(extendedX, shift + n - 1, shift);
    auto carryOut = part(extendedX, shift - 1);
    return {result, carryOut};
}

// See ARM32 ROR_C
DispatcherAarch32::TwoValues
DispatcherAarch32::rorC(const SValue::Ptr &x, int shift) {
    ASSERT_not_null(x);
    ASSERT_require(shift != 0);
    const size_t n = x->nBits();
    const int m = shift % n;
    auto result = operators()->or_(lsr(x, m), lsl(x, n-m));
    auto carryOut = part(result, n-1);
    return {result, carryOut};
}

// See ARM32 LSR
SValue::Ptr
DispatcherAarch32::lsr(const SValue::Ptr &x, size_t shift) {
    ASSERT_not_null(x);
    if (0 == shift) {
        return x;
    } else {
        return std::get<0>(lsrC(x, shift));
    }
}

// See ARM32 LSL
SValue::Ptr
DispatcherAarch32::lsl(const SValue::Ptr &x, size_t shift) {
    ASSERT_not_null(x);
    if (0 == shift) {
        return x;
    } else {
        return std::get<0>(lslC(x, shift));
    }
}

// See ARM32 RRX_C
DispatcherAarch32::TwoValues
DispatcherAarch32::rrxC(const SValue::Ptr &x, const SValue::Ptr &carryIn) {
    ASSERT_not_null(x);
    ASSERT_not_null(carryIn);
    ASSERT_require(carryIn->nBits() == 1);
    const size_t n = x->nBits();
    auto result = join(carryIn, part(x, n-1, 1));
    auto carryOut = part(x, 0);
    return {result, carryOut};
}

// See ARM32 SignExtend
SValue::Ptr
DispatcherAarch32::signExtend(const SValue::Ptr &x, size_t n) {
    ASSERT_not_null(x);
    ASSERT_require(n >= x->nBits());
    return operators()->signExtend(x, n);
}

// See ARM32 ALUExceptionReturn
void
DispatcherAarch32::aluExceptionReturn(const SValue::Ptr &enabled, const SValue::Ptr &address) {
    // There's some additional checking for undefined behaviors before calling aarch32ExceptionReturn.
    aarch32ExceptionReturn(enabled, address, spsr());
}

// See ARM32 SPSR
SValue::Ptr
DispatcherAarch32::spsr() {
    // This is the simplified version since ROSE is only supporting user instructions. The real version looks at the PSTATE.M
    // value and then returns one of the seven SPSR register values.
    return operators()->readRegister(REG_SPSR);
}

// See ARM32 AArch32.ExceptionReturn
void
DispatcherAarch32::aarch32ExceptionReturn(const SValue::Ptr &enabled, const SValue::Ptr &/*address*/, const SValue::Ptr &/*spsr*/) {
#if 0 // [Robb Matzke 2021-02-18]
    syncrhonizeContext(enabled);
    setPstateFromPsr(enabled, spsr);
    clearExclusiveLocal(enabled, processorId());
    sendEventLocal(enabled);
#endif

#if 1 // I cannot find any documentation for PSTATE.IL field
    SValue::Ptr pstateIl = operators()->undefined_(1);
#endif
    SValue::Ptr pstateT = operators()->readRegister(REG_PSTATE_T);
    SValue::Ptr zeroBit = operators()->boolean_(false);
    SValue::Ptr oldPc = operators()->readRegister(REG_PC);
    SValue::Ptr newPc0 = operators()->ite(pstateIl, operators()->undefined_(1), zeroBit);
    SValue::Ptr subA = operators()->ite(pstateT, part(oldPc, 1), zeroBit);
    SValue::Ptr newPc1 = operators()->ite(pstateIl, operators()->undefined_(1), subA);
    SValue::Ptr newPc10 = operators()->concatHiLo(newPc1, newPc0);
    SValue::Ptr newPc = operators()->concatHiLo(part(oldPc, 31, 2), newPc10);
    branchTo(enabled, newPc, BranchType::RET);
}

// See ARM32 ALUWritePC
void
DispatcherAarch32::aluWritePc(const SValue::Ptr &enabled, const SValue::Ptr &address) {
    bxWritePc(operators()->and_(enabled, isA32Mode()), address, BranchType::INDIR);
    branchWritePc(operators()->and_(enabled, isT32Mode()), address, BranchType::INDIR);
}

// See ARM32 BXWritePC
void
DispatcherAarch32::bxWritePc(const SValue::Ptr &enabled, const SValue::Ptr &address, BranchType branchType) {
    ASSERT_not_null(enabled);
    ASSERT_not_null(address);
    ASSERT_require(enabled->nBits() == 1);
    ASSERT_require(address->nBits() == 32);
    setThumbMode(part(address, 0));

    // Align the instruction address by clearing the least bit (T32 and A32) and the second least bit (A32).
    SValue::Ptr zero = operators()->boolean_(false);
    SValue::Ptr bit1 = part(address, 1);
    bit1 = operators()->ite(isT32Mode(), bit1, zero);
    SValue::Ptr upperBits = part(address, 31, 2);
    SValue::Ptr alignedAddress = operators()->concatHiLo(upperBits, bit1);
    alignedAddress = operators()->concatHiLo(alignedAddress, zero);

    branchTo(enabled, alignedAddress, branchType);
}

// See AARM32 BranchWritePC
void
DispatcherAarch32::branchWritePc(const SValue::Ptr &enabled, const SValue::Ptr &address, BranchType branchType) {
    ASSERT_not_null(enabled);
    ASSERT_require(enabled->nBits() == 1);
    ASSERT_not_null(address);

    // Align the instruction address by clearing the least bit (T32 and A32) and the second least bit (A32).
    SValue::Ptr zero = operators()->boolean_(false);
    SValue::Ptr bit1 = isA32Mode() ? zero : part(address, 1);
    SValue::Ptr upperBits = part(address, 31, 2);
    SValue::Ptr alignedAddress = operators()->concatHiLo(upperBits, bit1);
    alignedAddress = operators()->concatHiLo(alignedAddress, zero);

    branchTo(enabled, alignedAddress, branchType);
}

void
DispatcherAarch32::branchTo(const SValue::Ptr &enabled, const SValue::Ptr &alignedAddress, BranchType) {
    // The BranchType argument is only a hint. It has no effect on semantics.
    ASSERT_not_null(enabled);
    ASSERT_require(enabled->nBits() == 1);
    ASSERT_not_null(alignedAddress);
    maybeWriteRegister(enabled, REG_PC, alignedAddress);
}

// See AARCH32 Align
SValue::Ptr
DispatcherAarch32::align(const SValue::Ptr &x, unsigned y) {
    SValue::Ptr alignment = operators()->number_(32, y);
    SValue::Ptr aligned = operators()->unsignedMultiply(operators()->unsignedDivide(x, alignment), alignment);
    return operators()->unsignedExtend(aligned, 32);
}

// See AARCH32 PC
SValue::Ptr
DispatcherAarch32::pc() {
    // AAarch32 documentation says: "This function returns the PC value. On architecture versions before Armv7, it is permitted
    // to instead return PC+4, provided it does so consistently. It is used only to describe A32 instructions, so it returns
    // the address of the current instruction plus 8 (normally) or 12 (when the alternative is permitted)." Instructions such
    // as "BL" and "BLX" seem to imply that the return value is the address of the current instruction plus eight, even though
    // A32 instructions are only four bytes wide. Since ROSE increments the PC register by 4 (instruction size) as the first
    // part of every A32 instruction, we need to add four more to the return value.
    SValue::Ptr nextInsn = operators()->readRegister(REG_PC);
    SValue::Ptr plusFour = operators()->add(nextInsn, operators()->number_(32, 4));
    return plusFour;
}

// See AARCH32 PCStoreValue
SValue::Ptr
DispatcherAarch32::pcStoreValue() {
    // Contrary to what "store" would normally imply in a function name, this function doesn't actually store anything.
    // Instead, it returns the current value of the PC register as returned by the PC functions. I.e., the address of the
    // current instruction plus eight.  Here's what the documentation says: "This function returns the PC value. On
    // architecture versions before Armv7, it is permitted to instead return PC+4, provided it does so consistently. It is used
    // only to describe A32 instructions, so it returns the address of hte current instruction plus 8 (normally) or 12 (when
    // the alternative is permitted).
    return pc();
}

// See AARCH32 LoadWritePC
void
DispatcherAarch32::loadWritePc(const SValue::Ptr &enabled, const SValue::Ptr &address) {
    bxWritePc(enabled, address, BranchType::INDIR);
}

// See AARCH32 AddWithCarry
DispatcherAarch32::TwoValues
DispatcherAarch32::addWithCarry(const SValue::Ptr &a, const SValue::Ptr &b, const SValue::Ptr &carryIn) {
    SValue::Ptr carries;
    SValue::Ptr sum = operators()->addWithCarries(a, b, carryIn, carries /*out*/);
    SValue::Ptr n = operators()->extract(sum, sum->nBits()-1, sum->nBits()); // negative?
    SValue::Ptr z = operators()->equalToZero(sum);        // zero?
    SValue::Ptr c = operators()->extract(carries, carries->nBits()-1, carries->nBits()); // carry?
    SValue::Ptr v = operators()->xor_(c, operators()->extract(carries, carries->nBits()-2, carries->nBits()-1));
    SValue::Ptr nzcv = operators()->concatHiLo(operators()->concatHiLo(n, z),
                                             operators()->concatHiLo(c, v));
    return {sum, nzcv};
}

// See AARCH32 DBGDTR_EL0 (writing overload)
void
DispatcherAarch32::dbgdtrEl0(const SValue::Ptr &/*enabled*/, const SValue::Ptr &/*value*/) {
    // The EDSCR.TXfull and DTRTX registers are manipulated by hardware outside our control. Therefore, we should get a new
    // variable every time we read from them, and our own writes to it are meaningless.
}


// See AARCH32 DBGDTR_EL0 (reading overload)
SValue::Ptr
DispatcherAarch32::dbgdtrEl0() {
    // The EDSCR.RXfull and DTRTX registers are set by hardware that is outside our control. Therefore, every read should return
    // a new variable.
    return operators()->undefined_(32);
}

// See AARCH32 BitEndian
SValue::Ptr
DispatcherAarch32::bigEndian() {
    return operators()->readRegister(REG_PSTATE_E);
}

// See AARCH32 SignedSat
SValue::Ptr
DispatcherAarch32::signedSat(const SValue::Ptr &input, size_t width) {
    // Since the ARM version assumes that input is an "integer" instead of a bit vector, it was able to make some
    // optimizations. But ROSE needs to assume that the input is a symbolic expression whose value is not known. Therefore, the
    // "input" argument in ROSE needs to be wider than the desired return value in order to determine if the value is out of
    // range. For safety, we require that the input is exactly one bit wider than the output.
    ASSERT_not_null(input);
    ASSERT_require(width > 0);
    ASSERT_require(width + 1 == input->nBits());
    return std::get<0>(signedSatQ(input, width));
}

// See AARCH32 SignedSatQ
DispatcherAarch32::TwoValues
DispatcherAarch32::signedSatQ(const SValue::Ptr &aInput, size_t n) {
    // This implementation is completely different than documented because the documented version assumes that all values are
    // known. But we have to operate under the premise that values are symbolic and unknown, which makes this more complicated.
    // The return value will be truncated
    ASSERT_not_null(aInput);
    ASSERT_require(n > 0);
    SValue::Ptr a = aInput->nBits() > n ? aInput : operators()->signExtend(aInput, n+1);
    ASSERT_require(a->nBits() > n);

    SValue::Ptr one = operators()->number_(a->nBits(), 1);                          // example using a.nBits=32 and N=16...
    SValue::Ptr base = operators()->shiftLeft(one, operators()->number_(n, n-1));   // e.g., 0x00008000, which is (2^15)
    SValue::Ptr maxValWide = operators()->subtract(base, one);                      // e.g., 0x00007fff, which is (2^15)-1
    SValue::Ptr minValWide = operators()->invert(maxValWide);                       // e.g., 0xffff8000, which is -(2^15)
    SValue::Ptr maxValNarrow = operators()->unsignedExtend(maxValWide, n);          // e.g.,     0x7fff
    SValue::Ptr minValNarrow = operators()->unsignedExtend(maxValWide, n);          // e.g.,     0x8000
    SValue::Ptr aNarrow = operators()->unsignedExtend(a, n);
    SValue::Ptr overflowNeg = operators()->isSignedLessThan(a, minValWide);         // overflow in the negative direction?
    SValue::Ptr overflowPos = operators()->isSignedGreaterThan(a, maxValWide);      // overflow in the positive direction?
    SValue::Ptr result = operators()->ite(overflowNeg, minValNarrow,
                                        operators()->ite(overflowPos, maxValNarrow, aNarrow));
    SValue::Ptr didOverflow = operators()->or_(overflowNeg, overflowPos);
    return { result, didOverflow };
}

// See AArch32 UnsignedSat
SValue::Ptr
DispatcherAarch32::unsignedSat(const SValue::Ptr &input, size_t width) {
    // Since the ARM version assumes that input is an "integer" instead of a bit vector, it was able to make some
    // optimizations. But ROSE needs to assume that the input is a symbolic expression whose value is not known. Therefore, the
    // "input" argument in ROSE needs to be wider than the desired return value in order to determine if the value is out of
    // range. For safety, we require that the input is exactly one bit wider than the output.
    //
    // Also, see the comment in unsignedSatQ. When width is zero, unsignedSatQ returns a null SValue to indicate that the
    // ARM implementation would return a zero-width unsigned value.
    ASSERT_not_null(input);
    ASSERT_require(width > 0);
    ASSERT_require(width + 1 == input->nBits());
    return std::get<0>(unsignedSatQ(input, width));
}

// See AARCH32 UnsignedSatQ
DispatcherAarch32::TwoValues
DispatcherAarch32::unsignedSatQ(const SValue::Ptr &a, size_t n) {
    ASSERT_not_null(a);
    if (n >= a->nBits()) {
        SValue::Ptr result = operators()->unsignedExtend(a, n);
        SValue::Ptr didOverflow = operators()->boolean_(false);
        return { result, didOverflow };
    } else if (0 == n) {
        // saturate A so it's in [0, 2^N-1]. Yes, the ARM code really does say 2^N-1 and not 2^(N+1)-1. The instructions that
        // use this say that N is in [0, 31], not [1,32]. The ARM code also says that the width of the return value is
        // N. Therefore it's possible for the ARM version to return a value that's zero bits wide.  ROSE can't handle
        // zero-width values, so we return null instead and the caller better handle it.
        //
        // The ARM version might be buggy, because its final return statement, is "return (result<N-1:0>, saturated)" which is
        // nonsense when N is zero.
        //
        // This information is from the 2021-03 version of the documentation at developer.arm.com.
        return { SValue::Ptr(), operators()->boolean_(true) };
    } else {
        SValue::Ptr minVal = operators()->number_(n, 0);
        SValue::Ptr maxVal = operators()->invert(minVal);
        SValue::Ptr maxValWide = operators()->unsignedExtend(maxVal, a->nBits());
        SValue::Ptr aTooSmall = signBit(a);
        SValue::Ptr aTooBig = operators()->isSignedGreaterThan(a, maxValWide);

        SValue::Ptr val = operators()->unsignedExtend(a, n);
        SValue::Ptr valOrMin = operators()->ite(aTooSmall, operators()->number_(n, 0), val);
        SValue::Ptr aSaturated = operators()->ite(aTooBig, maxVal, valOrMin);

        SValue::Ptr didOverflow = operators()->or_(aTooSmall, aTooBig);
        return { aSaturated, didOverflow };
    }
}

// See AARCH32 Abs
SValue::Ptr
DispatcherAarch32::abs(const SValue::Ptr &a) {
    ASSERT_not_null(a);
    ASSERT_require(a->nBits() > 1);
    SValue::Ptr isNeg = signBit(a);
    SValue::Ptr aNeg = operators()->negate(a);
    return operators()->ite(isNeg, aNeg, a);
}

// See AARCH32 CountLeadingZeroBits
SValue::Ptr
DispatcherAarch32::countLeadingZeroBits(const SValue::Ptr &a) {
    ASSERT_not_null(a);
    return operators()->countLeadingZeros(a);
}

// See AARCH32 AArch32.CallHypervisor
void
DispatcherAarch32::aarch32CallHypervisor(const SValue::Ptr &enabled, const SValue::Ptr &value) {
    SValue::Ptr iMajor = operators()->number_(32, aarch32_exception_hvc);
    operators()->interrupt(iMajor, value, enabled);
}

} // namespace
} // namespace
} // namespace

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::DispatcherAarch32);
#endif

#endif
