#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH64
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherAarch64.h>

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/Utility.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BitOps.h>
#include <Rose/Diagnostics.h>

#include <SgAsmAarch64Instruction.h>
#include <SgAsmBinaryLsl.h>
#include <SgAsmExpression.h>
#include <SgAsmOperandList.h>
#include <SgAsmRegisterReferenceExpression.h>
#include <SgAsmVectorType.h>

#include <Cxx_GrammarDowncast.h>
#include <sageContainer.h>

using namespace Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functors that handle individual A64 instruction kinds
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Aarch64 {

// Base class for processing each instruction kind.
class P: public BaseSemantics::InsnProcessor {
public:
    using D = DispatcherAarch64*;
    using Ops = BaseSemantics::RiscOperators*;
    using I = SgAsmAarch64Instruction*;
    using A = const SgAsmExpressionPtrList&;

    virtual void p(D, Ops, I, A) = 0;

    virtual void process(const BaseSemantics::Dispatcher::Ptr &dispatcher_, SgAsmInstruction *insn_) override {
        DispatcherAarch64Ptr dispatcher = DispatcherAarch64::promote(dispatcher_);
        BaseSemantics::RiscOperators::Ptr operators = dispatcher->operators();
        SgAsmAarch64Instruction *insn = isSgAsmAarch64Instruction(insn_);
        ASSERT_not_null(insn);
        ASSERT_require(insn == operators->currentInstruction());
        dispatcher->advanceInstructionPointer(insn);    // branch instructions will reassign
        SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();

        operators->comment("operand pre-updates");
        for (size_t i = 0; i < operands.size(); ++i)
            dispatcher->preUpdate(operands[i], operators->boolean_(true));

        operators->comment("executing instruction core");
        p(dispatcher.get(), operators.get(), insn, operands);

        operators->comment("operand post-updates");
        for (size_t i = 0; i < operands.size(); ++i)
            dispatcher->postUpdate(operands[i], operators->boolean_(true));
    }

    void assert_args(I insn, A args, size_t nargs) {
        if (args.size() != nargs) {
            std::string mesg = "instruction must have " + StringUtility::plural(nargs, "arguments");
            throw BaseSemantics::Exception(mesg, insn);
        }
    }
};

struct IP_add: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = ops->unsignedExtend(d->read(args[2]), a->nBits());
        SValue::Ptr result;

        if (auto vectorType = isSgAsmVectorType(args[0]->get_type())) {
            size_t elmtNBits = vectorType->get_elmtType()->get_nBits();
            for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
                SValue::Ptr elmtA = ops->extract(a, i*elmtNBits, (i+1)*elmtNBits);
                SValue::Ptr elmtB = ops->extract(b, i*elmtNBits, (i+1)*elmtNBits);
                SValue::Ptr sum = ops->add(elmtA, elmtB);
                result = result ? ops->concatLoHi(result, sum) : sum;
            }
        } else {
            SValue::Ptr carryIn = ops->boolean_(false);
            SValue::Ptr carryOut;
            result = ops->addWithCarries(a, b, carryIn, carryOut);
            if (insn->get_updatesFlags())
                d->updateNZCV(result, carryOut);
        }
        d->write(args[0], result);
    }
};

struct IP_addp: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.size() == 2) {
            SValue::Ptr a = d->read(args[1]);
            SValue::Ptr aLo = ops->extract(a, 0, args[0]->get_nBits());
            SValue::Ptr aHi = ops->extract(a, args[0]->get_nBits(), a->nBits());
            SValue::Ptr result = ops->add(aLo, aHi);
            d->write(args[0], result);
        } else {
            assert_args(insn, args, 3);
            SValue::Ptr a = d->read(args[1]);
            SValue::Ptr b = d->read(args[2]);
            SValue::Ptr combined = ops->concatHiLo(b, a);
            SgAsmVectorType *vectorType = isSgAsmVectorType(args[1]->get_type());
            ASSERT_not_null(vectorType);
            size_t elmtNBits = vectorType->get_elmtType()->get_nBits();
            SValue::Ptr result;
            for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
                SValue::Ptr aElmt = ops->extract(combined, 2*i*elmtNBits, 2*i*elmtNBits + elmtNBits);
                SValue::Ptr bElmt = ops->extract(combined, 2*i*elmtNBits + elmtNBits, 2*(i+1)*elmtNBits);
                SValue::Ptr resultElmt = ops->add(aElmt, bElmt);
                result = result ? ops->concatLoHi(result, resultElmt) : resultElmt;
            }
            d->write(args[0], result);
        }
    }
};

struct IP_adr: P {
    void p(D d, Ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr targetVa = d->read(args[1]);
        d->write(args[0], targetVa);
    }
};

struct IP_adrp: P {
    void p(D d, Ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr targetVa = d->read(args[1]);
        d->write(args[0], targetVa);
    }
};

struct IP_and: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = ops->unsignedExtend(d->read(args[2]), a->nBits());
        SValue::Ptr result = ops->and_(a, b);
        if (insn->get_updatesFlags()) {
            ops->writeRegister(d->REG_CPSR_N, ops->extract(result, result->nBits()-1, result->nBits()));
            ops->writeRegister(d->REG_CPSR_Z, ops->equalToZero(result));
            ops->writeRegister(d->REG_CPSR_C, ops->boolean_(false));
            ops->writeRegister(d->REG_CPSR_V, ops->boolean_(false));
        }
        d->write(args[0], result);
    }
};

struct IP_asr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr value = d->read(args[1]);
        SValue::Ptr amount = ops->unsignedExtend(d->read(args[2]), 32 == args[0]->get_nBits() ? 5 : 6);
        SValue::Ptr result = ops->shiftRightArithmetic(value, amount);
        d->write(args[0], result);
    }
};

struct IP_b: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValue::Ptr targetVa = d->read(args[0]);
        SValue::Ptr fallThroughVa = d->number_(targetVa->nBits(), insn->get_address() + insn->get_size());
        SValue::Ptr cond = d->conditionHolds(insn->get_condition());
        SValue::Ptr nextIp = ops->ite(cond, targetVa, fallThroughVa);
        ops->writeRegister(d->REG_PC, nextIp);
    }
};

struct IP_bfm: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        size_t immR = d->read(args[2])->toUnsigned().get();
        size_t immS = d->read(args[3])->toUnsigned().get();
        bool n = 64 == args[0]->get_nBits();
        d->bitfieldMove(ops, args[0], args[1], n, immR, immS);
    }
};

struct IP_bfxil: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        size_t lsb = d->read(args[2])->toUnsigned().get();
        size_t width = d->read(args[3])->toUnsigned().get();
        uint64_t immR = lsb;
        uint64_t immS = lsb + width - 1;
        bool n = 64 == args[0]->get_nBits();
        d->bitfieldMove(ops, args[0], args[1], n, immR, immS);
    }
};

struct IP_bic: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.size() == 2) {
            SgAsmVectorType *vectorType = isSgAsmVectorType(args[0]->get_type());
            ASSERT_not_null(vectorType);
            size_t elmtNBits = vectorType->get_elmtType()->get_nBits();
            SValue::Ptr a = d->read(args[0]);
            SValue::Ptr bInv = ops->invert(ops->unsignedExtend(d->read(args[1]), elmtNBits));
            SValue::Ptr result;
            for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
                SValue::Ptr elmtA = ops->extract(a, i*elmtNBits, (i+1)*elmtNBits);
                SValue::Ptr elmtResult = ops->and_(elmtA, bInv);
                result = result ? ops->concatLoHi(result, elmtResult) : elmtResult;
            }
            d->write(args[0], result);
        } else {
            assert_args(insn, args, 3);
            SValue::Ptr a = d->read(args[1]);
            SValue::Ptr b = d->read(args[2]);
            SValue::Ptr result = ops->and_(a, ops->invert(b));
            if (insn->get_updatesFlags()) {
                ops->writeRegister(d->REG_CPSR_N, ops->extract(result, result->nBits()-1, result->nBits()));
                ops->writeRegister(d->REG_CPSR_Z, ops->equalToZero(result));
                ops->writeRegister(d->REG_CPSR_C, ops->boolean_(false));
                ops->writeRegister(d->REG_CPSR_V, ops->boolean_(false));
            }
            d->write(args[0], result);
        }
    }
};

struct IP_bl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValue::Ptr targetVa = d->read(args[0]);
        SValue::Ptr returnVa = ops->readRegister(d->REG_PC);
        ops->writeRegister(d->REG_LR, returnVa);
        ops->writeRegister(d->REG_PC, targetVa);
    }
};

struct IP_blr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValue::Ptr targetVa = d->read(args[0]);
        SValue::Ptr returnVa = ops->number_(targetVa->nBits(), insn->get_address() + insn->get_size());
        ops->writeRegister(d->REG_LR, returnVa);
        ops->writeRegister(d->REG_PC, targetVa);
    }
};

struct IP_br: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValue::Ptr targetVa = d->read(args[0]);
        ops->writeRegister(d->REG_PC, targetVa);
    }
};

struct IP_brk: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t imm = d->read(args[0])->toUnsigned().get();
        ops->interrupt((int)Aarch64Exception::brk, imm);
    }
};

struct IP_cbnz: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = d->read(args[0]);
        SValue::Ptr isZero = ops->equalToZero(value);
        SValue::Ptr targetVa = d->read(args[1]);
        SValue::Ptr fallThroughVa = d->number_(targetVa->nBits(), insn->get_address() + insn->get_size());
        SValue::Ptr nextIp = ops->ite(isZero, fallThroughVa, targetVa);
        ops->writeRegister(d->REG_PC, nextIp);
    }
};

struct IP_cbz: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = d->read(args[0]);
        SValue::Ptr isZero = ops->equalToZero(value);
        SValue::Ptr targetVa = d->read(args[1]);
        SValue::Ptr fallThroughVa = d->number_(targetVa->nBits(), insn->get_address() + insn->get_size());
        SValue::Ptr nextIp = ops->ite(isZero, targetVa, fallThroughVa);
        ops->writeRegister(d->REG_PC, nextIp);
    }
};

struct IP_ccmn: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr cond = d->conditionHolds(insn->get_condition());
        SValue::Ptr flagsSpecified = d->read(args[2]);
        SValue::Ptr a = d->read(args[0]);
        SValue::Ptr b = ops->unsignedExtend(d->read(args[1]), a->nBits());
        SValue::Ptr carryOut;
        SValue::Ptr diff = ops->addWithCarries(a, b, ops->boolean_(false), carryOut);
        DispatcherAarch64::NZCV flagsComputed = d->computeNZCV(diff, carryOut);
        ops->writeRegister(d->REG_CPSR_N, ops->ite(cond, flagsComputed.n, ops->extract(flagsSpecified, 3, 4)));
        ops->writeRegister(d->REG_CPSR_Z, ops->ite(cond, flagsComputed.z, ops->extract(flagsSpecified, 2, 3)));
        ops->writeRegister(d->REG_CPSR_C, ops->ite(cond, flagsComputed.c, ops->extract(flagsSpecified, 1, 2)));
        ops->writeRegister(d->REG_CPSR_V, ops->ite(cond, flagsComputed.c, ops->extract(flagsSpecified, 0, 1)));
    }
};

struct IP_ccmp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr cond = d->conditionHolds(insn->get_condition());
        SValue::Ptr flagsSpecified = d->read(args[2]);
        SValue::Ptr a = d->read(args[0]);
        SValue::Ptr b = ops->unsignedExtend(d->read(args[1]), a->nBits());
        SValue::Ptr carryOut;
        SValue::Ptr diff = ops->addWithCarries(a, ops->invert(b), ops->boolean_(true), carryOut);
        DispatcherAarch64::NZCV flagsComputed = d->computeNZCV(diff, carryOut);
        ops->writeRegister(d->REG_CPSR_N, ops->ite(cond, flagsComputed.n, ops->extract(flagsSpecified, 3, 4)));
        ops->writeRegister(d->REG_CPSR_Z, ops->ite(cond, flagsComputed.z, ops->extract(flagsSpecified, 2, 3)));
        ops->writeRegister(d->REG_CPSR_C, ops->ite(cond, flagsComputed.c, ops->extract(flagsSpecified, 1, 2)));
        ops->writeRegister(d->REG_CPSR_V, ops->ite(cond, flagsComputed.c, ops->extract(flagsSpecified, 0, 1)));
    }
};

struct IP_cls: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr src = d->read(args[0]);
        SValue::Ptr result;
        if (SgAsmVectorType *vectorType = isSgAsmVectorType(args[1]->get_type())) {
            size_t elmtNBits = vectorType->get_elmtType()->get_nBits();
            for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
                SValue::Ptr srcElmt = ops->extract(src, i*elmtNBits, (i+1)*elmtNBits);
                SValue::Ptr resultElmt = ops->countLeadingOnes(srcElmt);
                result = result ? ops->concatLoHi(result, resultElmt) : resultElmt;
            }
        } else {
            result = ops->countLeadingOnes(src);
        }
        d->write(args[0], result);
    }
};

struct IP_clz: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr src = d->read(args[0]);
        SValue::Ptr result;
        if (SgAsmVectorType *vectorType = isSgAsmVectorType(args[1]->get_type())) {
            size_t elmtNBits = vectorType->get_elmtType()->get_nBits();
            for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
                SValue::Ptr srcElmt = ops->extract(src, i*elmtNBits, (i+1)*elmtNBits);
                SValue::Ptr resultElmt = ops->countLeadingZeros(srcElmt);
                result = result ? ops->concatLoHi(result, resultElmt) : resultElmt;
            }
        } else {
            result = ops->countLeadingZeros(src);
        }
        d->write(args[0], result);
    }
};

struct IP_cmeq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr result;
        if (auto vectorType = isSgAsmVectorType(args[1]->get_type())) {
            size_t elmtNBits = vectorType->get_elmtType()->get_nBits();
            SValue::Ptr zeros = ops->number_(elmtNBits, 0);
            SValue::Ptr ones = ops->invert(zeros);
            for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
                SValue::Ptr aElmt = ops->extract(a, i*elmtNBits, (i+1)*elmtNBits);
                SValue::Ptr bElmt = ops->extract(b, i*elmtNBits, (i+1)*elmtNBits);
                SValue::Ptr resultElmt = ops->ite(ops->isEqual(aElmt, bElmt), ones, zeros);
                result = result ? ops->concatLoHi(result, resultElmt) : resultElmt;
            }
        } else {
            SValue::Ptr zeros = ops->number_(a->nBits(), 0);
            SValue::Ptr ones = ops->invert(zeros);
            result = ops->ite(ops->isEqual(a, b), ones, zeros);
        }
        d->write(args[0], result);
    }
};

struct IP_cmge: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr result;
        if (auto vectorType = isSgAsmVectorType(args[1]->get_type())) {
            size_t elmtNBits = vectorType->get_elmtType()->get_nBits();
            SValue::Ptr zeros = ops->number_(elmtNBits, 0);
            SValue::Ptr ones = ops->invert(zeros);
            for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
                SValue::Ptr aElmt = ops->extract(a, i*elmtNBits, (i+1)*elmtNBits);
                SValue::Ptr bElmt = ops->extract(b, i*elmtNBits, (i+1)*elmtNBits);
                SValue::Ptr resultElmt = ops->ite(ops->isSignedGreaterThanOrEqual(aElmt, bElmt), ones, zeros);
                result = result ? ops->concatLoHi(result, resultElmt) : resultElmt;
            }
        } else {
            SValue::Ptr zeros = ops->number_(a->nBits(), 0);
            SValue::Ptr ones = ops->invert(zeros);
            result = ops->ite(ops->isSignedGreaterThanOrEqual(a, b), ones, zeros);
        }
        d->write(args[0], result);
    }
};

struct IP_cmgt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr result;
        if (auto vectorType = isSgAsmVectorType(args[1]->get_type())) {
            size_t elmtNBits = vectorType->get_elmtType()->get_nBits();
            SValue::Ptr zeros = ops->number_(elmtNBits, 0);
            SValue::Ptr ones = ops->invert(zeros);
            for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
                SValue::Ptr aElmt = ops->extract(a, i*elmtNBits, (i+1)*elmtNBits);
                SValue::Ptr bElmt = ops->extract(b, i*elmtNBits, (i+1)*elmtNBits);
                SValue::Ptr resultElmt = ops->ite(ops->isSignedGreaterThan(aElmt, bElmt), ones, zeros);
                result = result ? ops->concatLoHi(result, resultElmt) : resultElmt;
            }
        } else {
            SValue::Ptr zeros = ops->number_(a->nBits(), 0);
            SValue::Ptr ones = ops->invert(zeros);
            result = ops->ite(ops->isSignedGreaterThan(a, b), ones, zeros);
        }
        d->write(args[0], result);
    }
};

struct IP_cmhi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr result;
        if (auto vectorType = isSgAsmVectorType(args[1]->get_type())) {
            size_t elmtNBits = vectorType->get_elmtType()->get_nBits();
            SValue::Ptr zeros = ops->number_(elmtNBits, 0);
            SValue::Ptr ones = ops->invert(zeros);
            for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
                SValue::Ptr aElmt = ops->extract(a, i*elmtNBits, (i+1)*elmtNBits);
                SValue::Ptr bElmt = ops->extract(b, i*elmtNBits, (i+1)*elmtNBits);
                SValue::Ptr resultElmt = ops->ite(ops->isUnsignedGreaterThan(aElmt, bElmt), ones, zeros);
                result = result ? ops->concatLoHi(result, resultElmt) : resultElmt;
            }
        } else {
            SValue::Ptr zeros = ops->number_(a->nBits(), 0);
            SValue::Ptr ones = ops->invert(zeros);
            result = ops->ite(ops->isUnsignedGreaterThan(a, b), ones, zeros);
        }
        d->write(args[0], result);
    }
};

struct IP_cmhs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr result;
        if (auto vectorType = isSgAsmVectorType(args[1]->get_type())) {
            size_t elmtNBits = vectorType->get_elmtType()->get_nBits();
            SValue::Ptr zeros = ops->number_(elmtNBits, 0);
            SValue::Ptr ones = ops->invert(zeros);
            for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
                SValue::Ptr aElmt = ops->extract(a, i*elmtNBits, (i+1)*elmtNBits);
                SValue::Ptr bElmt = ops->extract(b, i*elmtNBits, (i+1)*elmtNBits);
                SValue::Ptr resultElmt = ops->ite(ops->isUnsignedGreaterThanOrEqual(aElmt, bElmt), ones, zeros);
                result = result ? ops->concatLoHi(result, resultElmt) : resultElmt;
            }
        } else {
            SValue::Ptr zeros = ops->number_(a->nBits(), 0);
            SValue::Ptr ones = ops->invert(zeros);
            result = ops->ite(ops->isUnsignedGreaterThanOrEqual(a, b), ones, zeros);
        }
        d->write(args[0], result);
    }
};

struct IP_cmle: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr result;
        if (auto vectorType = isSgAsmVectorType(args[1]->get_type())) {
            size_t elmtNBits = vectorType->get_elmtType()->get_nBits();
            SValue::Ptr zeros = ops->number_(elmtNBits, 0);
            SValue::Ptr ones = ops->invert(zeros);
            for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
                SValue::Ptr aElmt = ops->extract(a, i*elmtNBits, (i+1)*elmtNBits);
                SValue::Ptr bElmt = ops->extract(b, i*elmtNBits, (i+1)*elmtNBits);
                SValue::Ptr resultElmt = ops->ite(ops->isSignedLessThanOrEqual(aElmt, bElmt), ones, zeros);
                result = result ? ops->concatLoHi(result, resultElmt) : resultElmt;
            }
        } else {
            SValue::Ptr zeros = ops->number_(a->nBits(), 0);
            SValue::Ptr ones = ops->invert(zeros);
            result = ops->ite(ops->isSignedLessThanOrEqual(a, b), ones, zeros);
        }
        d->write(args[0], result);
    }
};

struct IP_cmlt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr result;
        if (auto vectorType = isSgAsmVectorType(args[1]->get_type())) {
            size_t elmtNBits = vectorType->get_elmtType()->get_nBits();
            SValue::Ptr zeros = ops->number_(elmtNBits, 0);
            SValue::Ptr ones = ops->invert(zeros);
            for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
                SValue::Ptr aElmt = ops->extract(a, i*elmtNBits, (i+1)*elmtNBits);
                SValue::Ptr bElmt = ops->extract(b, i*elmtNBits, (i+1)*elmtNBits);
                SValue::Ptr resultElmt = ops->ite(ops->isSignedLessThan(aElmt, bElmt), ones, zeros);
                result = result ? ops->concatLoHi(result, resultElmt) : resultElmt;
            }
        } else {
            SValue::Ptr zeros = ops->number_(a->nBits(), 0);
            SValue::Ptr ones = ops->invert(zeros);
            result = ops->ite(ops->isSignedLessThan(a, b), ones, zeros);
        }
        d->write(args[0], result);
    }
};

struct IP_cmn: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr a = d->read(args[0]);
        SValue::Ptr b = ops->unsignedExtend(d->read(args[1]), a->nBits());
        SValue::Ptr carryOut;
        SValue::Ptr sum = ops->addWithCarries(a, b, ops->boolean_(false), carryOut);
        d->updateNZCV(sum, carryOut);
    }
};

struct IP_cmp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr rn = d->read(args[0]);
        SValue::Ptr notRm = ops->signExtend(ops->invert(d->read(args[1])), rn->nBits());
        SValue::Ptr carryIn = ops->boolean_(true);
        SValue::Ptr carryOut;
        SValue::Ptr diff = ops->addWithCarries(rn, notRm, carryIn, carryOut);
        d->updateNZCV(diff, carryOut);
    }
};

struct IP_cinc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr cond = d->conditionHolds(insn->get_condition());
        SValue::Ptr src = d->read(args[1]);
        SValue::Ptr srcInc = ops->add(src, ops->number_(src->nBits(), 1));
        SValue::Ptr result = ops->ite(cond, srcInc, src);
        d->write(args[0], result);
    }
};

struct IP_cinv: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr cond = d->conditionHolds(insn->get_condition());
        SValue::Ptr src = d->read(args[1]);
        SValue::Ptr result = ops->ite(cond, ops->invert(src), src);
        d->write(args[0], result);
    }
};

struct IP_cneg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr cond = d->conditionHolds(insn->get_condition());
        SValue::Ptr src = d->read(args[1]);
        SValue::Ptr srcNeg = ops->negate(src);
        SValue::Ptr result = ops->ite(cond, srcNeg, src);
        d->write(args[0], result);
    }
};

struct IP_csel: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr cond = d->conditionHolds(insn->get_condition());
        SValue::Ptr ifTrue = d->read(args[1]);
        SValue::Ptr ifFalse = d->read(args[2]);
        SValue::Ptr result = ops->ite(cond, ifTrue, ifFalse);
        d->write(args[0], result);
    }
};

struct IP_cset: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValue::Ptr cond = d->conditionHolds(insn->get_condition());
        SValue::Ptr result = ops->unsignedExtend(cond, args[0]->get_nBits());
        d->write(args[0], result);
    }
};

struct IP_csetm: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValue::Ptr cond = d->conditionHolds(insn->get_condition());
        SValue::Ptr zeros = ops->number_(args[0]->get_nBits(), 0);
        SValue::Ptr ones = ops->invert(zeros);
        SValue::Ptr result = ops->ite(cond, ones, zeros);
        d->write(args[0], result);
    }
};

struct IP_csinc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr cond = d->conditionHolds(insn->get_condition());
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr binc = ops->add(b, ops->number_(b->nBits(), 1));
        SValue::Ptr result = ops->ite(cond, a, binc);
        d->write(args[0], result);
    }
};

struct IP_csinv: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr cond = d->conditionHolds(insn->get_condition());
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr result = ops->ite(cond, a, ops->invert(b));
        d->write(args[0], result);
    }
};

struct IP_csneg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr cond = d->conditionHolds(insn->get_condition());
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr result = ops->ite(cond, a, ops->negate(b));
        d->write(args[0], result);
    }
};

struct IP_dmb: P {
    void p(D, Ops, I insn, A args) {
        assert_args(insn, args, 1);
        // no operation required for semantics
    }
};

struct IP_dup: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr src = d->read(args[1]);
        ASSERT_require(args[0]->get_nBits() >= src->nBits());
        size_t nElmts = args[0]->get_nBits() / src->nBits();
        ASSERT_require(nElmts * src->nBits() == args[0]->get_nBits());
        SValue::Ptr toWrite;
        for (size_t i = 0; i < nElmts; ++i)
            toWrite = toWrite ? ops->concatLoHi(toWrite, src) : src;
        d->write(args[0], toWrite);
    }
};

struct IP_eon: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = ops->unsignedExtend(d->read(args[2]), a->nBits());
        SValue::Ptr result = ops->xor_(a, ops->invert(b));
        d->write(args[0], result);
    }
};

struct IP_eor: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = ops->unsignedExtend(d->read(args[2]), a->nBits());
        SValue::Ptr result = ops->xor_(a, b);
        d->write(args[0], result);
    }
};

struct IP_extr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        size_t lsb = d->read(args[3])->toUnsigned().get();
        SValue::Ptr src = ops->concatHiLo(a, b);
        SValue::Ptr result = ops->extract(src, lsb, lsb + args[0]->get_nBits());
        d->write(args[0], result);
    }
};

struct IP_hint: P {
    void p(D, Ops, I insn, A args) {
        assert_args(insn, args, 1);
    }
};

struct IP_ins: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr result = d->read(args[1]);
        SValue::Ptr extended = ops->signExtend(result, args[0]->get_nBits());
        d->write(args[0], extended);
    }
};

struct IP_ldar: P {
    void p(D d, Ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr result = d->read(args[1]);
        d->write(args[0], result);
    }
};

struct IP_ldarb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr result = ops->unsignedExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], result);
    }
};

struct IP_ldarh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr result = ops->unsignedExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], result);
    }
};

struct IP_ldaxr: P {
    void p(D d, Ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr result = d->read(args[1]);
        d->write(args[0], result);
    }
};

struct IP_ldaxrb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr result = ops->unsignedExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], result);
    }
};

struct IP_ldaxrh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr result = ops->unsignedExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], result);
    }
};

struct IP_ldp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        size_t regSize = args[0]->get_type()->get_nBits();
        ASSERT_require(args[1]->get_type()->get_nBits() == regSize);
        SValue::Ptr pair = d->read(args[2]);
        ASSERT_require(pair->nBits() == 2 * regSize);
        SValue::Ptr first = ops->extract(pair, 0, regSize);
        SValue::Ptr second = ops->extract(pair, regSize, pair->nBits());
        d->write(args[0], first);
        d->write(args[1], second);
    }
};

struct IP_ldpsw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr data = d->read(args[2]);
        SValue::Ptr val1 = ops->signExtend(ops->extract(data, 0, 32), 64);
        SValue::Ptr val2 = ops->signExtend(ops->extract(data, 32, 64), 64);
        d->write(args[0], val1);
        d->write(args[1], val2);
    }
};

struct IP_ldr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr result = d->read(args[1]);
        SValue::Ptr extended = ops->signExtend(result, args[0]->get_nBits());
        d->write(args[0], extended);
    }
};

struct IP_ldrb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr byte = d->read(args[1]);
        SValue::Ptr extended = ops->unsignedExtend(byte, args[0]->get_nBits());
        d->write(args[0], extended);
    }
};

struct IP_ldrh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr half = d->read(args[1]);
        SValue::Ptr extended = ops->unsignedExtend(half, args[0]->get_nBits());
        d->write(args[0], extended);
    }
};

struct IP_ldrsb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = ops->signExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], value);
    }
};

struct IP_ldrsh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = ops->signExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], value);
    }
};

struct IP_ldrsw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = ops->signExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], value);
    }
};

struct IP_ldur: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = ops->unsignedExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], value);
    }
};

struct IP_ldurb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = ops->unsignedExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], value);
    }
};

struct IP_ldurh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = ops->unsignedExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], value);
    }
};

struct IP_ldursb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = ops->signExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], value);
    }
};

struct IP_ldursh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = ops->signExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], value);
    }
};

struct IP_ldursw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = ops->signExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], value);
    }
};

struct IP_ldxr: P {
    void p(D d, Ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr result = d->read(args[1]);
        d->write(args[0], result);
    }
};

struct IP_ldxrb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr result = ops->unsignedExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], result);
    }
};

struct IP_ldxrh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr result = ops->unsignedExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], result);
    }
};

struct IP_lsl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        // "The remainder obtained by dividing the second source register by the data size defines the number of bits by which
        // the first source register is left shifted."  We'll use masking instead of division since that seems like a much
        // simpler operation!
        size_t nBits = args[0]->get_nBits();
        ASSERT_require(32 == nBits || 64 == nBits);
        SValue::Ptr shiftAmount = ops->extract(b, 0, 32==nBits ? 5 : 6);
        SValue::Ptr shifted = ops->shiftLeft(a, shiftAmount);
        d->write(args[0], shifted);
    }
};

struct IP_lsr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        // "The remainder obtained by dividing the second source register by the data size defines the number of bits by which
        // the first source register is right-shifted."  We'll use masking instead of division since that seems like a much
        // simpler operation!
        size_t nBits = args[0]->get_nBits();
        ASSERT_require(32 == nBits || 64 == nBits);
        SValue::Ptr shiftAmount = ops->extract(b, 0, 32==nBits ? 5 : 6);
        SValue::Ptr shifted = ops->shiftRight(a, shiftAmount);
        d->write(args[0], shifted);
    }
};

struct IP_madd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr c = d->read(args[3]);
        SValue::Ptr product = ops->unsignedExtend(ops->unsignedMultiply(a, b), c->nBits());
        SValue::Ptr sum = ops->add(product, c);
        d->write(args[0], sum);
    }
};

struct IP_mov: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr result = d->read(args[1]);
        SValue::Ptr extended = ops->signExtend(result, args[0]->get_nBits());
        d->write(args[0], extended);
    }
};

struct IP_movi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        auto vectorType = isSgAsmVectorType(args[0]->get_type());
        ASSERT_not_null(vectorType);
        size_t elmtNBits = vectorType->get_elmtType()->get_nBits();
        SValue::Ptr src = ops->unsignedExtend(d->read(args[1]), elmtNBits);
        SValue::Ptr result;
        for (size_t i = 0; i < vectorType->get_nElmts(); ++i)
            result = result ? ops->concatLoHi(result, src) : src;
        d->write(args[0], result);
    }
};

struct IP_movk: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);

        // The 2nd argument is the 16-bit constant to place into the destination, but it also has an optional left shift. We
        // need to use the left shift to decide where to place the 16 bits.
        SValue::Ptr newBits;
        size_t shiftAmount = 0;
        if (auto lsl = isSgAsmBinaryLsl(args[1])) {
            newBits = d->read(lsl->get_lhs(), 16);
            shiftAmount = d->read(lsl->get_rhs())->toUnsigned().get();
        } else {
            newBits = d->read(args[1], 16);
        }

        SValue::Ptr oldBits = d->read(args[0]);
        SValue::Ptr result;
        if (shiftAmount > 0) {
            result = ops->concatLoHi(ops->extract(oldBits, 0, shiftAmount), newBits);
        } else {
            result = newBits;
        }
        if (shiftAmount + 16 < oldBits->nBits())
            result = ops->concatLoHi(result, ops->extract(oldBits, shiftAmount+16, oldBits->nBits()));
        d->write(args[0], result);
    }
};

struct IP_movn: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr result = d->read(args[1]);
        SValue::Ptr extended = ops->unsignedExtend(result, args[0]->get_nBits());
        SValue::Ptr inverted = ops->invert(extended);
        d->write(args[0], inverted);
    }
};

struct IP_movz: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr result = d->read(args[1]);
        SValue::Ptr extended = ops->unsignedExtend(result, args[0]->get_nBits());
        d->write(args[0], extended);
    }
};

struct IP_msub: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr c = d->read(args[3]);
        SValue::Ptr product = ops->unsignedExtend(ops->unsignedMultiply(a, b), c->nBits());
        SValue::Ptr diff = ops->subtract(c, product);
        d->write(args[0], diff);
    }
};

struct IP_mul: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr result;

        if (auto vectorType = isSgAsmVectorType(args[1]->get_type())) {
            if (isSgAsmVectorType(args[2])) {
                // Vector * vector, element-wise
                size_t elmtNBits = vectorType->get_elmtType()->get_nBits();
                for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
                    SValue::Ptr elmtA = ops->extract(a, i*elmtNBits, (i+1)*elmtNBits);
                    SValue::Ptr elmtB = ops->extract(b, i*elmtNBits, (i+1)*elmtNBits);
                    SValue::Ptr elmtResult = ops->unsignedExtend(ops->unsignedMultiply(elmtA, elmtB), elmtNBits);
                    result = result ? ops->concatLoHi(result, elmtResult) : elmtResult;
                }
            } else {
                // Vector * scalar, element-wise
                size_t elmtNBits = vectorType->get_elmtType()->get_nBits();
                for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
                    SValue::Ptr elmtA = ops->extract(a, i*elmtNBits, (i+1)*elmtNBits);
                    SValue::Ptr elmtResult = ops->unsignedExtend(ops->unsignedMultiply(elmtA, b), elmtNBits);
                    result = result ? ops->concatLoHi(result, elmtResult) : elmtResult;
                }
            }
        } else {
            // Scalar * scalar
            result = ops->unsignedExtend(ops->unsignedMultiply(a, b), args[0]->get_nBits());
        }
        d->write(args[0], result);
    }
};

struct IP_mvn: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr src = d->read(args[1]);
        SValue::Ptr result = ops->invert(src);
        d->write(args[0], result);
    }
};

struct IP_neg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = d->read(args[1]);
        SValue::Ptr result;
        if (auto vectorType = isSgAsmVectorType(args[1]->get_type())) {
            size_t elmtNBits = vectorType->get_elmtType()->get_nBits();
            for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
                SValue::Ptr elmt = ops->extract(value, i*elmtNBits, (i+1)*elmtNBits);
                SValue::Ptr elmtResult = ops->negate(elmt);
                result = result ? ops->concatLoHi(result, elmtResult) : elmtResult;
            }
        } else {
            result = ops->negate(value);
        }
        d->write(args[0], result);
    }
};

struct IP_negs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = d->read(args[1]);
        SValue::Ptr zero = ops->number_(value->nBits(), 0);
        SValue::Ptr carryIn = ops->boolean_(true);
        SValue::Ptr carryOut;
        SValue::Ptr result = ops->addWithCarries(zero, ops->invert(value), carryIn, carryOut);
        d->updateNZCV(result, carryOut);
        d->write(args[0], result);
    }
};

struct IP_ngc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr a = d->read(args[1]);                 // the value to negate
        SValue::Ptr b = ops->unsignedExtend(ops->readRegister(d->REG_CPSR_C), a->nBits());
        SValue::Ptr carryIn = ops->boolean_(true);
        SValue::Ptr carryOut;
        SValue::Ptr result = ops->addWithCarries(b, ops->invert(a), carryIn, carryOut);
        d->write(args[0], result);
    }
};

struct IP_ngcs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr a = d->read(args[1]);                 // the value to negate
        SValue::Ptr b = ops->unsignedExtend(ops->readRegister(d->REG_CPSR_C), a->nBits());
        SValue::Ptr carryIn = ops->boolean_(true);
        SValue::Ptr carryOut;
        SValue::Ptr result = ops->addWithCarries(b, ops->invert(a), carryIn, carryOut);
        d->updateNZCV(result, carryOut);
        d->write(args[0], result);
    }
};

struct IP_nop: P {
    void p(D, Ops, I, A) {
    }
};

struct IP_not: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = d->read(args[1]);
        SValue::Ptr result = ops->invert(value);
        d->write(args[0], result);
    }
};

struct IP_orn: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = ops->unsignedExtend(d->read(args[2]), a->nBits());
        SValue::Ptr result = ops->or_(a, ops->invert(b));
        d->write(args[0], result);
    }
};

struct IP_orr: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.size() == 2) {
            SValue::Ptr a1 = d->read(args[0]);
            SValue::Ptr a2 = d->advSimdExpandImm(args[0]->get_type(), d->read(args[1]));
            SValue::Ptr result = ops->or_(a1, a2);
            d->write(args[0], result);
        } else {
            assert_args(insn, args, 3);
            SValue::Ptr a1 = d->read(args[1], args[0]->get_nBits());
            SValue::Ptr a2 = d->read(args[2], args[0]->get_nBits());
            SValue::Ptr result = ops->or_(a1, a2);
            d->write(args[0], result);
        }
    }
};

struct IP_rbit: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = d->read(args[1]);
        SValue::Ptr result;
        if (auto vectorType = isSgAsmVectorType(args[1]->get_type())) {
            size_t elmtNBits = vectorType->get_elmtType()->get_nBits();
            for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
                SValue::Ptr elmt = ops->extract(value, i*elmtNBits, (i+1)*elmtNBits);
                SValue::Ptr elmtResult = ops->reverseElmts(elmt, 1);
                result = result ? ops->concatHiLo(elmtResult, result) : elmtResult;
            }
        } else {
            result = ops->reverseElmts(value, 1);
        }
        d->write(args[0], result);
    }
};

struct IP_ret: P {
    void p(D d, Ops ops, I insn, A args) {
        SValue::Ptr targetVa;
        if (args.empty()) {
            targetVa = ops->readRegister(d->REG_LR);
        } else {
            assert_args(insn, args, 1);
            targetVa = d->read(args[0]);
        }
        ops->writeRegister(d->REG_PC, targetVa);
    }
};

struct IP_rev: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = d->read(args[1]);
        size_t nBytes = value->nBits() / 8;
        SValue::Ptr result;
        for (size_t i = 0; i < nBytes; ++i) {
            SValue::Ptr byte = ops->extract(value, i*8, (i+1)*8);
            result = result ? ops->concatLoHi(byte, result) : byte;
        }
        d->write(args[0], result);
    }
};

struct IP_rev16: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = d->read(args[1]);
        size_t dataSize = args[0]->get_nBits();         // bits
        size_t containerSize = 16;                      // bits
        size_t bytesPerContainer = containerSize / 8;
        size_t nContainers = dataSize / containerSize;
        SValue::Ptr result;
        for (size_t i = 0; i < nContainers; ++i) {
            for (size_t j = bytesPerContainer; j > 0; --j) {
                size_t lsb = i * containerSize + (j-1) * 8;
                SValue::Ptr byte = ops->extract(value, lsb, lsb+8);
                result = result ? ops->concatLoHi(result, byte) : byte;
            }
        }
        d->write(args[0], result);
    }
};

struct IP_ror: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr src = d->read(args[1]);
        SValue::Ptr amount = d->read(args[2]);
        SValue::Ptr result = ops->rotateRight(src, amount);
        d->write(args[0], result);
    }
};

struct IP_sbc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[0]);
        SValue::Ptr b = d->read(args[1]);
        SValue::Ptr carryIn = ops->readRegister(d->REG_CPSR_C);
        SValue::Ptr carryOut;
        SValue::Ptr result = ops->addWithCarries(a, ops->invert(b), carryIn, carryOut);
        if (insn->get_updatesFlags())
            d->updateNZCV(result, carryOut);
        d->write(args[0], result);
    }
};

struct IP_sbfiz: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        size_t lsb = d->read(args[2])->toUnsigned().get();
        size_t width = d->read(args[3])->toUnsigned().get();
        uint64_t immR = -lsb % args[0]->get_nBits();
        uint64_t immS = width - 1;
        bool n = 64 == args[0]->get_nBits();
        d->signedBitfieldMove(ops, args[0], args[1], n, immR, immS);
    }
};

struct IP_sbfm: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        size_t immR = d->read(args[2])->toUnsigned().get();
        size_t immS = d->read(args[3])->toUnsigned().get();
        bool n = 64 == args[0]->get_nBits();
        d->signedBitfieldMove(ops, args[0], args[1], n, immR, immS);
    }
};

struct IP_sbfx: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        size_t lsb = d->read(args[2])->toUnsigned().get();
        size_t width = d->read(args[3])->toUnsigned().get();
        uint64_t immR = lsb;
        uint64_t immS = lsb + width - 1;
        bool n = 64 == args[0]->get_nBits();
        d->signedBitfieldMove(ops, args[0], args[1], n, immR, immS);
    }
};

struct IP_sdiv: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr result = ops->signedDivide(a, b);
        d->write(args[0], result);
    }
};

struct IP_smaddl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        SValue::Ptr wn = d->read(args[1]);
        SValue::Ptr wm = d->read(args[2]);
        SValue::Ptr xa = d->read(args[3]);
        SValue::Ptr result = ops->add(xa, ops->signedMultiply(wn, wm));
        d->write(args[0], result);
    }
};

struct IP_smulh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr product = ops->signedMultiply(a, b);
        SValue::Ptr result = ops->extract(product, 64, 128);
        d->write(args[0], result);
    }
};

struct IP_smull: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr product = ops->signedMultiply(a, b);
        d->write(args[0], product);
    }
};

struct IP_stlr: P {
    void p(D d, Ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = d->read(args[0]);
        d->write(args[1], value);
    }
};

struct IP_stlrb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = ops->extract(d->read(args[0]), 0, 8);
        d->write(args[1], value);
    }
};

struct IP_stlrh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = ops->extract(d->read(args[0]), 0, 16);
        d->write(args[1], value);
    }
};

#if 0 // [Robb Matzke 2020-09-03]: not present in Capstone
struct IP_stlur: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = d->read(args[0]);
        d->write(args[1], value);
    }
};

struct IP_stlurb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = ops->extract(d->read(args[0]), 0, 8);
        d->write(args[1], value);
    }
};

struct IP_stlurh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = ops->extract(d->read(args[0]), 0, 16);
        d->write(args[1], value);
    }
};
#endif

struct IP_stlxr: P {
    void p(D d, Ops ops, I insn, A args) {
        // ROSE assumes that the PE always has exclusive access to all memory
        assert_args(insn, args, 3);
        SValue::Ptr value = d->read(args[1]);
        d->write(args[2], value);
        SValue::Ptr status = ops->number_(args[0]->get_nBits(), 0);
        d->write(args[0], status);
    }
};

struct IP_stlxrb: P {
    void p(D d, Ops ops, I insn, A args) {
        // ROSE assumes that the PE always has exclusive access to all memory.
        assert_args(insn, args, 3);
        // Note: ARM documentation doesn't explicitly truncate the "data = X[t]", but I think that's a mistake given the
        // corresponding "data = bits(8) UNKNOWN" from the same "if" and the following "Mem[address, 1, ...] = data".
        SValue::Ptr value = ops->unsignedExtend(d->read(args[1]), 8);
        d->write(args[2], value);
        SValue::Ptr status = ops->number_(args[0]->get_nBits(), 0);
        d->write(args[0], status);
    }
};

struct IP_stlxrh: P {
    void p(D d, Ops ops, I insn, A args) {
        // ROSE assumes that the PE always has exclusive access to all memory.
        assert_args(insn, args, 3);
        // Note: ARM documentation doesn't explicitly truncate the "data = X[t]", but I think that's a mistake given the
        // corresponding "data = bits(16) UNKNOWN" from the same "if" and the following "Mem[address, 2, ...] = data".
        SValue::Ptr value = ops->unsignedExtend(d->read(args[1]), 16);
        d->write(args[2], value);
        SValue::Ptr status = ops->number_(args[0]->get_nBits(), 0);
        d->write(args[0], status);
    }
};

struct IP_stp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr first = d->read(args[0]);
        SValue::Ptr second = d->read(args[1]);
        SValue::Ptr pair = ops->concatLoHi(first, second);
        d->write(args[2], pair);
    }
};

struct IP_str: P {
    void p(D d, Ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr src = d->read(args[0]);
        d->write(args[1], src);
    }
};

struct IP_strb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = d->read(args[0]);
        SValue::Ptr byte = ops->extract(value, 0, 8);
        d->write(args[1], byte);
    }
};

struct IP_strh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = d->read(args[0]);
        SValue::Ptr byte = ops->extract(value, 0, 16);
        d->write(args[1], byte);
    }
};

struct IP_stur: P {
    void p(D d, Ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = d->read(args[0]);
        d->write(args[1], value);
    }
};

struct IP_sturb: P {
    void p(D d, Ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = d->read(args[0]);
        d->write(args[1], value);
    }
};

struct IP_sturh: P {
    void p(D d, Ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr value = d->read(args[0]);
        d->write(args[1], value);
    }
};

struct IP_stxr: P {
    void p(D d, Ops ops, I insn, A args) {
        // ROSE assumes that the PE always has exclusive access to all memory
        assert_args(insn, args, 3);
        SValue::Ptr value = d->read(args[1]);
        d->write(args[2], value);
        SValue::Ptr status = ops->number_(args[0]->get_nBits(), 0);
        d->write(args[0], status);
    }
};

struct IP_stxrb: P {
    void p(D d, Ops ops, I insn, A args) {
        // ROSE assumes that the PE always has exclusive access to all memory.
        assert_args(insn, args, 3);
        // Note: ARM documentation doesn't explicitly truncate the "data = X[t]", but I think that's a mistake given the
        // corresponding "data = bits(8) UNKNOWN" from the same "if" and the following "Mem[address, 1, ...] = data".
        SValue::Ptr value = ops->unsignedExtend(d->read(args[1]), 8);
        d->write(args[2], value);
        SValue::Ptr status = ops->number_(args[0]->get_nBits(), 0);
        d->write(args[0], status);
    }
};

struct IP_stxrh: P {
    void p(D d, Ops ops, I insn, A args) {
        // ROSE assumes that the PE always has exclusive access to all memory.
        assert_args(insn, args, 3);
        // Note: ARM documentation doesn't explicitly truncate the "data = X[t]", but I think that's a mistake given the
        // corresponding "data = bits(16) UNKNOWN" from the same "if" and the following "Mem[address, 2, ...] = data".
        SValue::Ptr value = ops->unsignedExtend(d->read(args[1]), 16);
        d->write(args[2], value);
        SValue::Ptr status = ops->number_(args[0]->get_nBits(), 0);
        d->write(args[0], status);
    }
};

struct IP_sub: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr minuend = d->read(args[1]);
        SValue::Ptr subtrahend = ops->signExtend(d->read(args[2]), minuend->nBits());
        SValue::Ptr result;

        if (auto vectorType = isSgAsmVectorType(args[0]->get_type())) {
            size_t elmtNBits = vectorType->get_elmtType()->get_nBits();
            for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
                SValue::Ptr elmtMinuend = ops->extract(minuend, i*elmtNBits, (i+1)*elmtNBits);
                SValue::Ptr elmtSubtrahend = ops->extract(subtrahend, i*elmtNBits, (i+1)*elmtNBits);
                SValue::Ptr difference = ops->subtract(elmtMinuend, elmtSubtrahend);
                result = result ? ops->concatLoHi(result, difference) : difference;
            }
            ASSERT_forbid(insn->get_updatesFlags());
        } else {
            SValue::Ptr carryIn = ops->boolean_(true);
            SValue::Ptr carryOut;
            result = ops->addWithCarries(minuend, ops->invert(subtrahend), carryIn, carryOut);
            if (insn->get_updatesFlags())
                d->updateNZCV(result, carryOut);
        }
        d->write(args[0], result);
    }
};

struct IP_subs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr minuend = d->read(args[1]);
        SValue::Ptr subtrahend = ops->signExtend(d->read(args[2]), minuend->nBits());
        SValue::Ptr carryIn = ops->boolean_(true);
        SValue::Ptr carryOut;
        SValue::Ptr result = ops->addWithCarries(minuend, ops->invert(subtrahend), carryIn, carryOut);
        d->updateNZCV(result, carryOut);
        d->write(args[0], result);
    }
};

struct IP_sxtb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr source = d->read(args[1]);
        SValue::Ptr byte = ops->extract(source, 0, 8);
        SValue::Ptr result = ops->signExtend(byte, args[0]->get_nBits());
        d->write(args[0], result);
    }
};

struct IP_sxth: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr source = d->read(args[1]);
        SValue::Ptr half = ops->extract(source, 0, 16);
        SValue::Ptr result = ops->signExtend(half, args[0]->get_nBits());
        d->write(args[0], result);
    }
};

struct IP_sxtw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr source = d->read(args[1]);
        SValue::Ptr word = ops->extract(source, 0, 32);
        SValue::Ptr result = ops->signExtend(word, args[0]->get_nBits());
        d->write(args[0], result);
    }
};

struct IP_tbnz: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr index = d->read(args[1]);
        SValue::Ptr value = d->read(args[0]);
        SValue::Ptr bit = ops->extract(value, index->toUnsigned().get(), index->toUnsigned().get()+1);
        SValue::Ptr targetVa = d->read(args[2]);
        SValue::Ptr fallThroughVa = ops->number_(targetVa->nBits(), insn->get_address() + insn->get_size());
        SValue::Ptr nextIp = ops->ite(bit, targetVa, fallThroughVa);
        ops->writeRegister(d->REG_PC, nextIp);
    }
};

struct IP_tbz: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr index = d->read(args[1]);
        SValue::Ptr value = d->read(args[0]);
        SValue::Ptr bit = ops->extract(value, index->toUnsigned().get(), index->toUnsigned().get()+1);
        SValue::Ptr targetVa = d->read(args[2]);
        SValue::Ptr fallThroughVa = ops->number_(targetVa->nBits(), insn->get_address() + insn->get_size());
        SValue::Ptr nextIp = ops->ite(bit, fallThroughVa, targetVa);
        ops->writeRegister(d->REG_PC, nextIp);
    }
};

struct IP_tst: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr a = d->read(args[0]);
        SValue::Ptr b = ops->unsignedExtend(d->read(args[1]), a->nBits());
        SValue::Ptr result = ops->and_(a, b);
        ops->writeRegister(d->REG_CPSR_N, ops->extract(result, result->nBits()-1, result->nBits()));
        ops->writeRegister(d->REG_CPSR_Z, ops->equalToZero(result));
        ops->writeRegister(d->REG_CPSR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CPSR_V, ops->boolean_(false));
    }
};

struct IP_ubfiz: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        size_t lsb = d->read(args[2])->toUnsigned().get();
        size_t width = d->read(args[3])->toUnsigned().get();
        uint64_t immR = -lsb % args[0]->get_nBits();
        uint64_t immS = width - 1;
        bool n = 64 == args[0]->get_nBits();
        d->unsignedBitfieldMove(ops, args[0], args[1], n, immR, immS);
    }
};

struct IP_ubfm: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        size_t immR = d->read(args[2])->toUnsigned().get();
        size_t immS = d->read(args[3])->toUnsigned().get();
        bool n = 64 == args[0]->get_nBits();
        d->unsignedBitfieldMove(ops, args[0], args[1], n, immR, immS);
    }
};

struct IP_ubfx: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        size_t lsb = d->read(args[2])->toUnsigned().get();
        size_t width = d->read(args[3])->toUnsigned().get();
        uint64_t immR = lsb;
        uint64_t immS = lsb + width - 1;
        bool n = 64 == args[0]->get_nBits();
        d->unsignedBitfieldMove(ops, args[0], args[1], n, immR, immS);
    }
};

struct IP_udiv: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr result = ops->unsignedDivide(a, b);
        d->write(args[0], result);
    }
};

struct IP_umaddl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        SValue::Ptr wn = d->read(args[1]);
        SValue::Ptr wm = d->read(args[2]);
        SValue::Ptr xa = d->read(args[3]);
        SValue::Ptr result = ops->add(xa, ops->unsignedMultiply(wn, wm));
        d->write(args[0], result);
    }
};

struct IP_umov: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr result = d->read(args[1]);
        SValue::Ptr extended = ops->unsignedExtend(result, args[0]->get_nBits());
        d->write(args[0], extended);
    }
};

struct IP_umsubl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        SValue::Ptr factor1 = d->read(args[1]);
        SValue::Ptr factor2 = d->read(args[2]);
        SValue::Ptr product = ops->unsignedMultiply(factor1, factor2);
        SValue::Ptr minuend = d->read(args[3]);
        SValue::Ptr result = ops->subtract(minuend, product);
        d->write(args[0], result);
    }
};

struct IP_umulh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr product = ops->unsignedMultiply(a, b);
        SValue::Ptr result = ops->extract(product, 64, 128);
        d->write(args[0], result);
    }
};

struct IP_umull: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr b = d->read(args[2]);
        SValue::Ptr product = ops->unsignedMultiply(a, b);
        d->write(args[0], product);
    }
};

struct IP_uxtb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr byte = ops->extract(a, 0, 8);
        SValue::Ptr result = ops->unsignedExtend(byte, args[0]->get_nBits());
        d->write(args[0], result);
    }
};

struct IP_uxth: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr a = d->read(args[1]);
        SValue::Ptr word = ops->extract(a, 0, 16);
        SValue::Ptr result = ops->unsignedExtend(word, args[0]->get_nBits());
        d->write(args[0], result);
    }
};

struct IP_xtn: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        auto vectorType = isSgAsmVectorType(args[1]->get_type());
        ASSERT_not_null(vectorType);
        size_t elmtNBits = vectorType->get_elmtType()->get_nBits();
        SValue::Ptr src = d->read(args[1]);
        SValue::Ptr result;
        for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
            SValue::Ptr elmt = ops->extract(src, i*elmtNBits, i*elmtNBits + elmtNBits/2);
            result = result ? ops->concat(result, elmt) : elmt;
        }
        result = ops->concatLoHi(result, ops->number_(vectorType->get_nBits()/2, 0));
        d->write(args[0], result);
    }
};

struct IP_xtn2: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        auto vectorType = isSgAsmVectorType(args[1]->get_type());
        ASSERT_not_null(vectorType);
        size_t elmtNBits = vectorType->get_elmtType()->get_nBits();
        SValue::Ptr src = d->read(args[1]);
        SValue::Ptr result;
        for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
            SValue::Ptr elmt = ops->extract(src, i*elmtNBits, i*elmtNBits + elmtNBits/2);
            result = result ? ops->concat(result, elmt) : elmt;
        }
        result = ops->concatHiLo(result, ops->number_(vectorType->get_nBits()/2, 0));
        d->write(args[0], result);
    }
};

struct IP_yield: P {
    void p(D, Ops, I insn, A args) {
        assert_args(insn, args, 0);
    }
};

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DispatcherAarch64::DispatcherAarch64() {}

DispatcherAarch64::DispatcherAarch64(const Architecture::Base::ConstPtr &arch)
    : BaseSemantics::Dispatcher(arch) {}

DispatcherAarch64::DispatcherAarch64(const Architecture::Base::ConstPtr &arch, const BaseSemantics::RiscOperators::Ptr &ops)
    : BaseSemantics::Dispatcher(arch, ops) {
    initializeRegisterDescriptors();
    initializeInsnDispatchTable();
    initializeMemory();
    initializeState(ops->currentState());
}

DispatcherAarch64::~DispatcherAarch64() {}

DispatcherAarch64::Ptr
DispatcherAarch64::instance(const Architecture::Base::ConstPtr &arch) {
    return Ptr(new DispatcherAarch64(arch));
}

DispatcherAarch64::Ptr
DispatcherAarch64::instance(const Architecture::Base::ConstPtr &arch, const BaseSemantics::RiscOperators::Ptr &ops) {
    return DispatcherAarch64Ptr(new DispatcherAarch64(arch, ops));
}

BaseSemantics::Dispatcher::Ptr
DispatcherAarch64::create(const BaseSemantics::RiscOperators::Ptr &ops) const {
    return instance(architecture(), ops);
}

DispatcherAarch64::Ptr
DispatcherAarch64::promote(const BaseSemantics::Dispatcher::Ptr &d) {
    Ptr retval = as<DispatcherAarch64>(d);
    ASSERT_not_null(retval);
    return retval;
}

void
DispatcherAarch64::initializeInsnDispatchTable() {
    iprocSet(ARM64_INS_ADD,    new Aarch64::IP_add);
    iprocSet(ARM64_INS_ADDP,   new Aarch64::IP_addp);
    //iprocSet(ARM64_INS_ADDS,   new Aarch64::IP_adds); -- see ARM64_INS_ADD
    iprocSet(ARM64_INS_ADR,    new Aarch64::IP_adr);
    iprocSet(ARM64_INS_ADRP,   new Aarch64::IP_adrp);
    iprocSet(ARM64_INS_AND,    new Aarch64::IP_and);
    iprocSet(ARM64_INS_ASR,    new Aarch64::IP_asr);
    //iprocSet(ARM64_INS_ASRV,   new Aarch64::IP_asrv); -- see ARM64_INS_ASR
    iprocSet(ARM64_INS_B,      new Aarch64::IP_b);
    iprocSet(ARM64_INS_BFM,    new Aarch64::IP_bfm);
    iprocSet(ARM64_INS_BFXIL,  new Aarch64::IP_bfxil);
    iprocSet(ARM64_INS_BIC,    new Aarch64::IP_bic);
    iprocSet(ARM64_INS_BL,     new Aarch64::IP_bl);
    iprocSet(ARM64_INS_BLR,    new Aarch64::IP_blr);
    iprocSet(ARM64_INS_BR,     new Aarch64::IP_br);
    iprocSet(ARM64_INS_BRK,    new Aarch64::IP_brk);
    iprocSet(ARM64_INS_CBNZ,   new Aarch64::IP_cbnz);
    iprocSet(ARM64_INS_CBZ,    new Aarch64::IP_cbz);
    iprocSet(ARM64_INS_CCMN,   new Aarch64::IP_ccmn);
    iprocSet(ARM64_INS_CCMP,   new Aarch64::IP_ccmp);
    iprocSet(ARM64_INS_CINC,   new Aarch64::IP_cinc);
    iprocSet(ARM64_INS_CINV,   new Aarch64::IP_cinv);
    iprocSet(ARM64_INS_CLS,    new Aarch64::IP_cls);
    iprocSet(ARM64_INS_CLZ,    new Aarch64::IP_clz);
    iprocSet(ARM64_INS_CMEQ,   new Aarch64::IP_cmeq);
    iprocSet(ARM64_INS_CMGE,   new Aarch64::IP_cmge);
    iprocSet(ARM64_INS_CMGT,   new Aarch64::IP_cmgt);
    iprocSet(ARM64_INS_CMHI,   new Aarch64::IP_cmhi);
    iprocSet(ARM64_INS_CMHS,   new Aarch64::IP_cmhs);
    iprocSet(ARM64_INS_CMLE,   new Aarch64::IP_cmle);
    iprocSet(ARM64_INS_CMLT,   new Aarch64::IP_cmlt);
    iprocSet(ARM64_INS_CMN,    new Aarch64::IP_cmn);
    iprocSet(ARM64_INS_CMP,    new Aarch64::IP_cmp);
    iprocSet(ARM64_INS_CNEG,   new Aarch64::IP_cneg);
    iprocSet(ARM64_INS_CSEL,   new Aarch64::IP_csel);
    iprocSet(ARM64_INS_CSET,   new Aarch64::IP_cset);
    iprocSet(ARM64_INS_CSETM,  new Aarch64::IP_csetm);
    iprocSet(ARM64_INS_CSINC,  new Aarch64::IP_csinc);
    iprocSet(ARM64_INS_CSINV,  new Aarch64::IP_csinv);
    iprocSet(ARM64_INS_CSNEG,  new Aarch64::IP_csneg);
    iprocSet(ARM64_INS_DMB,    new Aarch64::IP_dmb);
    iprocSet(ARM64_INS_DUP,    new Aarch64::IP_dup);
    iprocSet(ARM64_INS_EON,    new Aarch64::IP_eon);
    iprocSet(ARM64_INS_EOR,    new Aarch64::IP_eor);
    iprocSet(ARM64_INS_EXTR,   new Aarch64::IP_extr);
    iprocSet(ARM64_INS_HINT,   new Aarch64::IP_hint);
    iprocSet(ARM64_INS_INS,    new Aarch64::IP_ins);
    iprocSet(ARM64_INS_LDAR,   new Aarch64::IP_ldar);
    iprocSet(ARM64_INS_LDARB,  new Aarch64::IP_ldarb);
    iprocSet(ARM64_INS_LDARH,  new Aarch64::IP_ldarh);
    iprocSet(ARM64_INS_LDAXR,  new Aarch64::IP_ldaxr);
    iprocSet(ARM64_INS_LDAXRB, new Aarch64::IP_ldaxrb);
    iprocSet(ARM64_INS_LDAXRH, new Aarch64::IP_ldaxrh);
    iprocSet(ARM64_INS_LDP,    new Aarch64::IP_ldp);
    iprocSet(ARM64_INS_LDPSW,  new Aarch64::IP_ldpsw);
    iprocSet(ARM64_INS_LDR,    new Aarch64::IP_ldr);
    iprocSet(ARM64_INS_LDRB,   new Aarch64::IP_ldrb);
    iprocSet(ARM64_INS_LDRH,   new Aarch64::IP_ldrh);
    iprocSet(ARM64_INS_LDRSB,  new Aarch64::IP_ldrsb);
    iprocSet(ARM64_INS_LDRSH,  new Aarch64::IP_ldrsh);
    iprocSet(ARM64_INS_LDRSW,  new Aarch64::IP_ldrsw);
    iprocSet(ARM64_INS_LDUR,   new Aarch64::IP_ldur);
    iprocSet(ARM64_INS_LDURB,  new Aarch64::IP_ldurb);
    iprocSet(ARM64_INS_LDURH,  new Aarch64::IP_ldurh);
    iprocSet(ARM64_INS_LDURSB, new Aarch64::IP_ldursb);
    iprocSet(ARM64_INS_LDURSH, new Aarch64::IP_ldursh);
    iprocSet(ARM64_INS_LDURSW, new Aarch64::IP_ldursw);
    iprocSet(ARM64_INS_LDXR,   new Aarch64::IP_ldxr);
    iprocSet(ARM64_INS_LDXRB,  new Aarch64::IP_ldxrb);
    iprocSet(ARM64_INS_LDXRH,  new Aarch64::IP_ldxrh);
    iprocSet(ARM64_INS_LSL,    new Aarch64::IP_lsl);
    iprocSet(ARM64_INS_LSR,    new Aarch64::IP_lsr);
    iprocSet(ARM64_INS_MADD,   new Aarch64::IP_madd);
    iprocSet(ARM64_INS_MOV,    new Aarch64::IP_mov);
    iprocSet(ARM64_INS_MOVI,   new Aarch64::IP_movi);
    iprocSet(ARM64_INS_MOVK,   new Aarch64::IP_movk);
    iprocSet(ARM64_INS_MOVN,   new Aarch64::IP_movn);
    iprocSet(ARM64_INS_MOVZ,   new Aarch64::IP_movz);
    iprocSet(ARM64_INS_MSUB,   new Aarch64::IP_msub);
    iprocSet(ARM64_INS_MUL,    new Aarch64::IP_mul);
    iprocSet(ARM64_INS_MVN,    new Aarch64::IP_mvn);
    iprocSet(ARM64_INS_NEG,    new Aarch64::IP_neg);
    iprocSet(ARM64_INS_NEGS,   new Aarch64::IP_negs);
    iprocSet(ARM64_INS_NGC,    new Aarch64::IP_ngc);
    iprocSet(ARM64_INS_NGCS,   new Aarch64::IP_ngcs);
    iprocSet(ARM64_INS_NOP,    new Aarch64::IP_nop);
    iprocSet(ARM64_INS_NOT,    new Aarch64::IP_not);
    iprocSet(ARM64_INS_ORN,    new Aarch64::IP_orn);
    iprocSet(ARM64_INS_ORR,    new Aarch64::IP_orr);
    iprocSet(ARM64_INS_RBIT,   new Aarch64::IP_rbit);
    iprocSet(ARM64_INS_RET,    new Aarch64::IP_ret);
    iprocSet(ARM64_INS_REV,    new Aarch64::IP_rev);
    iprocSet(ARM64_INS_REV16,  new Aarch64::IP_rev16);
    iprocSet(ARM64_INS_ROR,    new Aarch64::IP_ror);
    //iprocSet(ARM64_INS_RORV,   new Aarch64::IP_rorv); -- see AMD64_INS_ROR
    iprocSet(ARM64_INS_SBC,    new Aarch64::IP_sbc);
    iprocSet(ARM64_INS_SBFIZ,  new Aarch64::IP_sbfiz);
    iprocSet(ARM64_INS_SBFM,   new Aarch64::IP_sbfm);
    iprocSet(ARM64_INS_SBFX,   new Aarch64::IP_sbfx);
    iprocSet(ARM64_INS_SDIV,   new Aarch64::IP_sdiv);
    iprocSet(ARM64_INS_SMADDL, new Aarch64::IP_smaddl);
    iprocSet(ARM64_INS_SMULH,  new Aarch64::IP_smulh);
    iprocSet(ARM64_INS_SMULL,  new Aarch64::IP_smull);
    iprocSet(ARM64_INS_STLR,   new Aarch64::IP_stlr);
    iprocSet(ARM64_INS_STLRB,  new Aarch64::IP_stlrb);
    iprocSet(ARM64_INS_STLRH,  new Aarch64::IP_stlrh);
#if 0 // [Robb Matzke 2020-09-03]: not present in capstone
    iprocSet(ARM64_INS_STLUR,  new Aarch64::IP_stlur);
    iprocSet(ARM64_INS_STLURB, new Aarch64::IP_stlurb);
    iprocSet(ARM64_INS_STLURH, new Aarch64::IP_stlurh);
#endif
    iprocSet(ARM64_INS_STLXR,  new Aarch64::IP_stlxr);
    iprocSet(ARM64_INS_STLXRB, new Aarch64::IP_stlxrb);
    iprocSet(ARM64_INS_STLXRH, new Aarch64::IP_stlxrh);
    iprocSet(ARM64_INS_STP,    new Aarch64::IP_stp);
    iprocSet(ARM64_INS_STR,    new Aarch64::IP_str);
    iprocSet(ARM64_INS_STRB,   new Aarch64::IP_strb);
    iprocSet(ARM64_INS_STRH,   new Aarch64::IP_strh);
    iprocSet(ARM64_INS_STUR,   new Aarch64::IP_stur);
    iprocSet(ARM64_INS_STURB,  new Aarch64::IP_sturb);
    iprocSet(ARM64_INS_STURH,  new Aarch64::IP_sturh);
    iprocSet(ARM64_INS_STXR,   new Aarch64::IP_stxr);
    iprocSet(ARM64_INS_STXRB,  new Aarch64::IP_stxrb);
    iprocSet(ARM64_INS_STXRH,  new Aarch64::IP_stxrh);
    iprocSet(ARM64_INS_SUB,    new Aarch64::IP_sub);
    //iprocSet(ARM64_INS_SUBS,   new Aarch64::IP_subs); -- see ARM64_INS_SUB
    iprocSet(ARM64_INS_SXTB,   new Aarch64::IP_sxtb);
    iprocSet(ARM64_INS_SXTH,   new Aarch64::IP_sxth);
    iprocSet(ARM64_INS_SXTW,   new Aarch64::IP_sxtw);
    iprocSet(ARM64_INS_TBNZ,   new Aarch64::IP_tbnz);
    iprocSet(ARM64_INS_TBZ,    new Aarch64::IP_tbz);
    iprocSet(ARM64_INS_TST,    new Aarch64::IP_tst);
    iprocSet(ARM64_INS_UBFIZ,  new Aarch64::IP_ubfiz);
    iprocSet(ARM64_INS_UBFM,   new Aarch64::IP_ubfm);
    iprocSet(ARM64_INS_UBFX,   new Aarch64::IP_ubfx);
    iprocSet(ARM64_INS_UDIV,   new Aarch64::IP_udiv);
    iprocSet(ARM64_INS_UMADDL, new Aarch64::IP_umaddl);
    iprocSet(ARM64_INS_UMOV,   new Aarch64::IP_umov);
    iprocSet(ARM64_INS_UMSUBL, new Aarch64::IP_umsubl);
    iprocSet(ARM64_INS_UMULH,  new Aarch64::IP_umulh);
    iprocSet(ARM64_INS_UMULL,  new Aarch64::IP_umull);
    iprocSet(ARM64_INS_UXTB,   new Aarch64::IP_uxtb);
    iprocSet(ARM64_INS_UXTH,   new Aarch64::IP_uxth);
    iprocSet(ARM64_INS_XTN,    new Aarch64::IP_xtn);
    iprocSet(ARM64_INS_XTN2,   new Aarch64::IP_xtn2);
    iprocSet(ARM64_INS_YIELD,  new Aarch64::IP_yield);
}

void
DispatcherAarch64::initializeRegisterDescriptors() {
    REG_PC = findRegister("pc", 64);
    REG_SP = findRegister("sp", 64);
    REG_LR = findRegister("lr", 64);
    REG_CPSR_N = findRegister("cpsr.n", 1);
    REG_CPSR_Z = findRegister("cpsr.z", 1);
    REG_CPSR_C = findRegister("cpsr.c", 1);
    REG_CPSR_V = findRegister("cpsr.v", 1);
}

void
DispatcherAarch64::initializeMemory() {
    if (BaseSemantics::State::Ptr state = currentState()) {
        if (BaseSemantics::MemoryState::Ptr memory = state->memoryState()) {
            switch (memory->get_byteOrder()) {
                case ByteOrder::ORDER_LSB:
                    break;
                case ByteOrder::ORDER_MSB:
                    mlog[WARN] <<"Aarch64 memory state is using big-endian byte order\n";
                    break;
                case ByteOrder::ORDER_UNSPECIFIED:
                    memory->set_byteOrder(ByteOrder::ORDER_LSB);
                    break;
            }
        }
    }
}

RegisterDescriptor
DispatcherAarch64::instructionPointerRegister() const {
    return REG_PC;
}

RegisterDescriptor
DispatcherAarch64::stackPointerRegister() const {
    return REG_SP;
}

RegisterDescriptor
DispatcherAarch64::callReturnRegister() const {
    return REG_LR;
}

int
DispatcherAarch64::iprocKey(SgAsmInstruction *insn_) const {
    auto insn = isSgAsmAarch64Instruction(insn_);
    ASSERT_not_null(insn);
    return insn->get_kind();
}

SValue::Ptr
DispatcherAarch64::read(SgAsmExpression *e, size_t value_nbits/*=0*/, size_t addr_nbits/*=0*/) {
    // Reading from general purpose register 31 always returns zero
    if (auto rre = isSgAsmRegisterReferenceExpression(e)) {
        RegisterDescriptor reg = rre->get_descriptor();
        if (reg.majorNumber() == aarch64_regclass_gpr && reg.minorNumber() == 31) {
            if (0 == value_nbits)
                value_nbits = reg.nBits();
            return operators()->number_(value_nbits, 0);
        }
    }
    return BaseSemantics::Dispatcher::read(e, value_nbits, addr_nbits);
}

void
DispatcherAarch64::write(SgAsmExpression *e, const SValue::Ptr &value, size_t addr_nbits/*=0*/) {
    // Writes to general purpose register 31 are always discarded
    if (auto rre = isSgAsmRegisterReferenceExpression(e)) {
        RegisterDescriptor reg = rre->get_descriptor();
        if (reg.majorNumber() == aarch64_regclass_gpr && reg.minorNumber() == 31)
            return;
    }
    BaseSemantics::Dispatcher::write(e, value, addr_nbits);
}

SValue::Ptr
DispatcherAarch64::advSimdExpandImm(SgAsmType *type, const SValue::Ptr &imm) {
    ASSERT_not_null(type);
    ASSERT_not_null(imm);
    auto vectorType = isSgAsmVectorType(type);
    ASSERT_not_null(vectorType);
    auto elmtType = vectorType->get_elmtType();
    ASSERT_not_null(elmtType);
    ASSERT_require(isSgAsmScalarType(elmtType));
    SValue::Ptr elmt = operators()->unsignedExtend(imm, elmtType->get_nBits());
    SValue::Ptr result;
    for (size_t i = 0; i < vectorType->get_nElmts(); ++i)
        result = result ? operators()->concat(result, elmt) : elmt;
    return result;
}

DispatcherAarch64::NZCV
DispatcherAarch64::computeNZCV(const SValue::Ptr &sum, const SValue::Ptr &carries) {
    ASSERT_not_null(sum);
    ASSERT_require(sum->nBits() > 1);
    ASSERT_not_null(carries);
    ASSERT_require(carries->nBits() == sum->nBits());

    SValue::Ptr isNeg = operators()->extract(sum, sum->nBits()-1, sum->nBits());
    SValue::Ptr isZero = operators()->equalToZero(sum);
    SValue::Ptr isCarry = operators()->extract(carries, carries->nBits()-1, carries->nBits());
    SValue::Ptr isOverflow = operators()->xor_(operators()->extract(carries, carries->nBits()-1, carries->nBits()),
                                             operators()->extract(carries, carries->nBits()-2, carries->nBits()-1));

    return NZCV(isNeg, isZero, isCarry, isOverflow);
}

void
DispatcherAarch64::updateNZCV(const SValue::Ptr &sum, const SValue::Ptr &carries) {
    NZCV nzcv = computeNZCV(sum, carries);

    operators()->writeRegister(REG_CPSR_N, nzcv.n);
    operators()->writeRegister(REG_CPSR_Z, nzcv.z);
    operators()->writeRegister(REG_CPSR_C, nzcv.c);
    operators()->writeRegister(REG_CPSR_V, nzcv.v);
}

SValue::Ptr
DispatcherAarch64::conditionHolds(Aarch64InstructionCondition cond) {
    // WARNING: ARM documentation is inconsistent and sometimes wrong when it describes how these flags are set.
    switch (cond) {
        case Aarch64InstructionCondition::ARM64_CC_INVALID: // occurs for "B" instruction
            return operators()->boolean_(true);
        case Aarch64InstructionCondition::ARM64_CC_EQ:      // equal (z set)
            return operators()->readRegister(REG_CPSR_Z);
        case Aarch64InstructionCondition::ARM64_CC_NE:      // not equal: not equal, or unordered (z clear)
            return operators()->invert(operators()->readRegister(REG_CPSR_Z));
        case Aarch64InstructionCondition::ARM64_CC_HS:      // unsigned higher or same: >, ==, or unordered (c set)
            return operators()->readRegister(REG_CPSR_C);
        case Aarch64InstructionCondition::ARM64_CC_LO:      // unsigned lower or same: less than (c clear)
            return operators()->invert(operators()->readRegister(REG_CPSR_C));
        case Aarch64InstructionCondition::ARM64_CC_MI:      // minus, negative: less than (n set)
            return operators()->readRegister(REG_CPSR_N);
        case Aarch64InstructionCondition::ARM64_CC_PL:      // plus, positive or zero: >, ==, or unordered (n clear)
            return operators()->invert(operators()->readRegister(REG_CPSR_N));
        case Aarch64InstructionCondition::ARM64_CC_VS:      // overflow: unordered (v set)
            return operators()->readRegister(REG_CPSR_V);
        case Aarch64InstructionCondition::ARM64_CC_VC:      // no overflow: ordered (v clear)
            return operators()->invert(operators()->readRegister(REG_CPSR_V));
        case Aarch64InstructionCondition::ARM64_CC_HI: {    // unsigned higher: greater than, or unordered
            // WARNING: The ARM definition reads "c set and z clear", but see LS below.
            SValue::Ptr c = operators()->readRegister(REG_CPSR_C);
            SValue::Ptr z = operators()->readRegister(REG_CPSR_Z);
            return operators()->and_(c, operators()->invert(z));
        }
        case Aarch64InstructionCondition::ARM64_CC_LS: {    // unsigned lower or same: less than or equal
            // WARNING: The ARM definition, which reads "c clear and z set" is not the inverse of the description for HI which
            // reads "c set and z clear", although it should be since HI and LS are inverses. The inverse of HI would be
            // "c clear or z set".  I found other documentation that indeed says "c clear or z set", so I'm going with that.
            SValue::Ptr c = operators()->readRegister(REG_CPSR_C);
            SValue::Ptr z = operators()->readRegister(REG_CPSR_Z);
            return operators()->or_(operators()->invert(c), z);
        }
        case Aarch64InstructionCondition::ARM64_CC_GE: {    // greater than or equal: greater than or equal (n == v)
            SValue::Ptr n = operators()->readRegister(REG_CPSR_N);
            SValue::Ptr v = operators()->readRegister(REG_CPSR_V);
            return operators()->invert(operators()->xor_(n, v));
        }
        case Aarch64InstructionCondition::ARM64_CC_LT: {    // less than: less than, or unordered (n != v)
            SValue::Ptr n = operators()->readRegister(REG_CPSR_N);
            SValue::Ptr v = operators()->readRegister(REG_CPSR_V);
            return operators()->xor_(n, v);
        }
        case Aarch64InstructionCondition::ARM64_CC_GT: {    // signed greater than: greater than
            // WARNING: ARM documentation sometimes says "z clear, n and v the same", but see LE below. I found other user documentation that
            // says "Z = 0 & N = V", so I'm going with that.
            SValue::Ptr n = operators()->readRegister(REG_CPSR_N);
            SValue::Ptr v = operators()->readRegister(REG_CPSR_V);
            SValue::Ptr z = operators()->readRegister(REG_CPSR_Z);
            SValue::Ptr nEqV = operators()->invert(operators()->xor_(n, v));
            return operators()->and_(operators()->invert(z), nEqV);
        }
        case Aarch64InstructionCondition::ARM64_CC_LE: {    // signed less than or equal: <, ==, or unorderd
            // WARNING: ARM documentation reads "z set, n and v differ", which is not the inverse of the LE description that
            // reads "z clear, n and v the same" regardless of whether one treats the comma as "and" or "or". The correct
            // inverse of "z clear and n == v" is "z set or n != v". I found other user documentation that says "Z=1 or N=!V"
            // (I'm parsing "N=!V" as "N != V".
            SValue::Ptr n = operators()->readRegister(REG_CPSR_N);
            SValue::Ptr v = operators()->readRegister(REG_CPSR_V);
            SValue::Ptr z = operators()->readRegister(REG_CPSR_Z);
            SValue::Ptr nNeV = operators()->xor_(n, v);
            return operators()->or_(z, nNeV);
        }
        case Aarch64InstructionCondition::ARM64_CC_AL:      // always (unconditional): always (unconditional)
        case Aarch64InstructionCondition::ARM64_CC_NV:      // always (unconditional): always (unconditional)
            return operators()->boolean_(true);
    }
    ASSERT_not_reachable("invalid condition");
}

std::pair<uint64_t/* m bits */, uint64_t/*m bits*/>
DispatcherAarch64::decodeBitMasks(size_t m, bool immN, uint64_t immS/*6 bits*/, uint64_t immR/*6 bits*/, bool immediate) {
#if 0 // DEBUGGING [Robb Matzke 2020-07-24]
    std::cerr <<"ROBB: (bits(" <<m <<"), bits(" <<m <<")) DecodeBitMask(bit immN=" <<(immN?1:0)
              <<", bits(6) imms=" <<StringUtility::toBinary(immS, 6)
              <<", bits(6) immr=" <<StringUtility::toBinary(immR, 6)
              <<", immediate=" <<(immediate?1:0) <<")\n";
#endif
    // This code comes from the ARM documentation.  C++ doesn't have arbitrary size integers, so we use the largest we'll ever
    // need (uint64_t) and make sure all our arithmetic uses only the least significant M bits.
    ASSERT_always_require(m <= 64);
    immN &= 0x3f;
    immR &= 0x3f;

    // "Compute log2 of element size. 2^len must be in range [2, M]"
    uint64_t len = *BitOps::highestSetBit((immN ? 0x40 : 0) | (~immS & 0x3f)); // len = HighestBitSet(immN:NOT(imms));
    ASSERT_forbid(len < 1);                             // if len < 1 then UNDEFINED;
    ASSERT_require(m >= (1u << len));                   // asert M >= (1 << len);

    // "Determine S, R and S - R parameters"
    uint64_t levels = BitOps::lowMask<uint64_t>(len) & 0x3f; // bits(6) levels = ZeroExtend(Ones(len), 6);

    // "For logical immediates an all-ones value of S is reserved since it would generate a useless all-cones result (many
    // times)"
    ASSERT_always_forbid(immediate && (immS & levels) == levels); // if immediate && (imms AND levels) == levels then UNDEFINED;

    uint64_t s = immS & levels;                         // S = UInt(imms AND levels);
    uint64_t r = immR & levels;                         // R = Uint(immr AND levels);
    uint64_t diff = s - r;                              // diff = S - R; // 6-bit subtract with borrow

    // "From a software perspective, the remaining code is equivalent to"... and we use that commented out code instead
    // of the much more complicated code that's not commented out.
    uint64_t esize = 1u << len;                         // esize = 1 << len
    uint64_t d = BitOps::bits(diff, 0, len-1);          // d = UInt(diff<len-1:0>);
    uint64_t welem = BitOps::lowMask<uint64_t>(s + 1) & BitOps::lowMask<uint64_t>(esize); // welem = ZeroExtend(Ones(S+1), esize)
    uint64_t telem = BitOps::lowMask<uint64_t>(d + 1) & BitOps::lowMask<uint64_t>(esize); // telem = ZeroExtend(Ones(d+1), esize)
    uint64_t wmask = BitOps::replicate(BitOps::rotateRightLsb(welem, esize, r), esize); // bits(64) wmask = Replicate(ROR(welem, R))
    uint64_t tmask = BitOps::replicate(telem, esize); // bits(64) tmask = Replicate(telem)
#if 0 // DEBUGGING [Robb Matzke 2020-07-24]
    std::cerr <<"ROBB: returning wmask=" <<StringUtility::toBinary(wmask) <<", tmask=" <<StringUtility::toBinary(tmask) <<"\n";
#endif
    return std::make_pair(wmask, tmask);
}

void
DispatcherAarch64::bitfieldMove(RiscOperators *ops, SgAsmExpression *dstExpr, SgAsmExpression *srcExpr, bool n,
                            uint64_t immR, uint64_t immS) {
    ASSERT_not_null(dstExpr);
    ASSERT_not_null(srcExpr);
    ASSERT_require(dstExpr->get_nBits() == srcExpr->get_nBits());

    size_t dataSize = dstExpr->get_nBits();
    auto wtmasks = decodeBitMasks(dataSize, n, immS, immR, false);
    SValue::Ptr wmask = ops->number_(dataSize, wtmasks.first);
    SValue::Ptr tmask = ops->number_(dataSize, wtmasks.second);

    SValue::Ptr src = read(srcExpr);
    SValue::Ptr dst = read(dstExpr);

    // Perform bitfield move on low bits
    SValue::Ptr r = ops->number_(6, immR);
    SValue::Ptr botLhs = ops->and_(dst, ops->invert(wmask));
    SValue::Ptr botRhs = ops->and_(ops->rotateRight(src, r), wmask);
    SValue::Ptr bot = ops->or_(botLhs, botRhs);

    // Combine extension bits and result bits
    SValue::Ptr combinedLhs = ops->and_(dst, ops->invert(tmask));
    SValue::Ptr combinedRhs = ops->and_(bot, tmask);
    SValue::Ptr combined = ops->or_(combinedLhs, combinedRhs);
    write(dstExpr, combined);
}

void
DispatcherAarch64::unsignedBitfieldMove(RiscOperators *ops, SgAsmExpression *dstExpr, SgAsmExpression *srcExpr, bool n,
                                        uint64_t immR, uint64_t immS) {
    ASSERT_not_null(dstExpr);
    ASSERT_not_null(srcExpr);
    ASSERT_require(dstExpr->get_nBits() == srcExpr->get_nBits());

    size_t dataSize = dstExpr->get_nBits();
    auto wtmasks = decodeBitMasks(dataSize, n, immS, immR, false);
    SValue::Ptr wmask = ops->number_(dataSize, wtmasks.first);
    SValue::Ptr tmask = ops->number_(dataSize, wtmasks.second);

    SValue::Ptr src = read(srcExpr);

    // Perform bitfield move on the low bits
    SValue::Ptr r = ops->number_(6, immR);
    SValue::Ptr bot = ops->and_(ops->rotateRight(src, r), wmask);

    // Combine extension bits and result bits
    SValue::Ptr combined = ops->and_(bot, tmask);
    write(dstExpr, combined);
}

void
DispatcherAarch64::signedBitfieldMove(RiscOperators *ops, SgAsmExpression *dstExpr, SgAsmExpression *srcExpr, bool n,
                                      uint64_t immR, uint64_t immS) {
    ASSERT_not_null(dstExpr);
    ASSERT_not_null(srcExpr);
    ASSERT_require(dstExpr->get_nBits() == srcExpr->get_nBits());

    size_t dataSize = dstExpr->get_nBits();
    auto wtmasks = decodeBitMasks(dataSize, n, immS, immR, false);
    SValue::Ptr wmask = ops->number_(dataSize, wtmasks.first);
    SValue::Ptr tmask = ops->number_(dataSize, wtmasks.second);

    SValue::Ptr src = read(srcExpr);

    // Perform bitfield move on low bits
    SValue::Ptr r = ops->number_(6, immR);
    SValue::Ptr bot = ops->and_(ops->rotateRight(src, r), wmask);

    // Determine extension bits (sign, zero or dest register). ARM pseudo code is "bits(datasize) top = Replicate(src<S>);"
    SValue::Ptr signBit = ops->extract(src, immS, immS+1);
    SValue::Ptr zero = ops->number_(dataSize, 0);
    SValue::Ptr ones = ops->invert(zero);
    SValue::Ptr top = ops->ite(signBit, ones, zero);

    // Combine extensio bits and result bits
    SValue::Ptr lhs = ops->and_(top, ops->invert(tmask));
    SValue::Ptr rhs = ops->and_(bot, tmask);
    SValue::Ptr combined = ops->or_(lhs, rhs);
    write(dstExpr, combined);
}

} // namespace
} // namespace
} // namespace

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::DispatcherAarch64);
#endif

#endif
