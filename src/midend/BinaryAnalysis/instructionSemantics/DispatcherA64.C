#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_A64

#include <sage3basic.h>
#include <DispatcherA64.h>

#include <BitOps.h>
#include <Diagnostics.h>

using namespace Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functors that handle individual A64 instruction kinds
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace A64 {

// Base class for processing each instruction kind.
class P: public BaseSemantics::InsnProcessor {
public:
    using D = DispatcherA64*;
    using Ops = BaseSemantics::RiscOperators*;
    using I = SgAsmA64Instruction*;
    using A = const SgAsmExpressionPtrList&;

    virtual void p(D, Ops, I, A) = 0;

    virtual void process(const BaseSemantics::DispatcherPtr &dispatcher_, SgAsmInstruction *insn_) ROSE_OVERRIDE {
        DispatcherA64Ptr dispatcher = DispatcherA64::promote(dispatcher_);
        BaseSemantics::RiscOperatorsPtr operators = dispatcher->get_operators();
        SgAsmA64Instruction *insn = isSgAsmA64Instruction(insn_);
        ASSERT_not_null(insn);
        ASSERT_require(insn == operators->currentInstruction());
        dispatcher->advanceInstructionPointer(insn);    // branch instructions will reassign
        SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
        for (size_t i = 0; i < operands.size(); ++i)
            dispatcher->preUpdate(operands[i]);
        p(dispatcher.get(), operators.get(), insn, operands);
        for (size_t i = 0; i < operands.size(); ++i)
            dispatcher->postUpdate(operands[i]);
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
        SValuePtr a = d->read(args[1]);
        SValuePtr b = ops->unsignedExtend(d->read(args[2]), a->get_width());
        SValuePtr result;

        if (auto vectorType = isSgAsmVectorType(args[0]->get_type())) {
            size_t elmtSize = vectorType->get_elmtType()->get_nBits();
            for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
                SValuePtr elmtA = ops->extract(a, i*elmtSize, (i+1)*elmtSize);
                SValuePtr elmtB = ops->extract(b, i*elmtSize, (i+1)*elmtSize);
                SValuePtr sum = ops->add(elmtA, elmtB);
                result = result ? ops->concat(result, sum) : sum;
            }
        } else {
            result = ops->add(a, b);
        }
        d->write(args[0], result);
    }
};

struct IP_adrp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr targetVa = d->read(args[1]);
        d->write(args[0], targetVa);
    }
};

struct IP_and: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValuePtr a = d->read(args[1]);
        SValuePtr b = ops->unsignedExtend(d->read(args[2]), a->get_width());
        SValuePtr result = ops->and_(a, b);
        d->write(args[0], result);
    }
};

struct IP_asr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValuePtr value = d->read(args[1]);
        SValuePtr amount = ops->unsignedExtend(d->read(args[2]), 32 == args[0]->get_nBits() ? 5 : 6);
        SValuePtr result = ops->shiftRightArithmetic(value, amount);
        d->write(args[0], result);
    }
};

struct IP_b: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr targetVa = d->read(args[0]);
        SValuePtr fallThroughVa = d->number_(targetVa->get_width(), insn->get_address() + insn->get_size());
        SValuePtr cond = d->conditionHolds(insn->get_condition());
        SValuePtr nextIp = ops->ite(cond, targetVa, fallThroughVa);
        ops->writeRegister(d->REG_PC, nextIp);
    }
};

struct IP_bl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr targetVa = d->read(args[0]);
        SValuePtr returnVa = ops->readRegister(d->REG_PC);
        ops->writeRegister(d->REG_LR, returnVa);
        ops->writeRegister(d->REG_PC, targetVa);
    }
};

struct IP_blr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr targetVa = d->read(args[0]);
        SValuePtr returnVa = ops->number_(targetVa->get_width(), insn->get_address() + insn->get_size());
        ops->writeRegister(d->REG_LR, returnVa);
        ops->writeRegister(d->REG_PC, targetVa);
    }
};

struct IP_br: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr targetVa = d->read(args[0]);
        ops->writeRegister(d->REG_PC, targetVa);
    }
};

struct IP_cbnz: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr value = d->read(args[0]);
        SValuePtr isZero = ops->equalToZero(value);
        SValuePtr targetVa = d->read(args[1]);
        SValuePtr fallThroughVa = d->number_(targetVa->get_width(), insn->get_address() + insn->get_size());
        SValuePtr nextIp = ops->ite(isZero, fallThroughVa, targetVa);
        ops->writeRegister(d->REG_PC, nextIp);
    }
};

struct IP_cbz: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr value = d->read(args[0]);
        SValuePtr isZero = ops->equalToZero(value);
        SValuePtr targetVa = d->read(args[1]);
        SValuePtr fallThroughVa = d->number_(targetVa->get_width(), insn->get_address() + insn->get_size());
        SValuePtr nextIp = ops->ite(isZero, targetVa, fallThroughVa);
        ops->writeRegister(d->REG_PC, nextIp);
    }
};

struct IP_ccmp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValuePtr cond = d->conditionHolds(insn->get_condition());
        SValuePtr flagsSpecified = d->read(args[2]);
        SValuePtr a = d->read(args[0]);
        SValuePtr b = ops->unsignedExtend(d->read(args[1]), a->get_width());
        SValuePtr carryOut;
        SValuePtr diff = ops->addWithCarries(a, ops->invert(b), ops->boolean_(true), carryOut);
        DispatcherA64::NZCV flagsComputed = d->computeNZCV(diff, carryOut);
        ops->writeRegister(d->REG_CPSR_N, ops->ite(cond, flagsComputed.n, ops->extract(flagsSpecified, 3, 4)));
        ops->writeRegister(d->REG_CPSR_Z, ops->ite(cond, flagsComputed.z, ops->extract(flagsSpecified, 2, 3)));
        ops->writeRegister(d->REG_CPSR_C, ops->ite(cond, flagsComputed.c, ops->extract(flagsSpecified, 1, 2)));
        ops->writeRegister(d->REG_CPSR_V, ops->ite(cond, flagsComputed.c, ops->extract(flagsSpecified, 0, 1)));
    }
};

struct IP_cmn: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr a = d->read(args[0]);
        SValuePtr b = ops->unsignedExtend(d->read(args[1]), a->get_width());
        SValuePtr carryOut;
        SValuePtr sum = ops->addWithCarries(a, b, ops->boolean_(false), carryOut);
        d->updateNZCV(sum, carryOut);
    }
};

struct IP_cmp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr rn = d->read(args[0]);
        SValuePtr notRm = ops->signExtend(ops->invert(d->read(args[1])), rn->get_width());
        SValuePtr carryIn = ops->boolean_(true);
        SValuePtr carryOut;
        SValuePtr diff = ops->addWithCarries(rn, notRm, carryIn, carryOut);
        d->updateNZCV(diff, carryOut);
    }
};

struct IP_csel: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValuePtr cond = d->conditionHolds(insn->get_condition());
        SValuePtr ifTrue = d->read(args[1]);
        SValuePtr ifFalse = d->read(args[2]);
        SValuePtr result = ops->ite(cond, ifTrue, ifFalse);
        d->write(args[0], result);
    }
};

struct IP_cset: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr cond = d->conditionHolds(insn->get_condition());
        SValuePtr result = ops->unsignedExtend(cond, args[0]->get_nBits());
        d->write(args[0], result);
    }
};

struct IP_csetm: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr cond = d->conditionHolds(insn->get_condition());
        SValuePtr zeros = ops->number_(args[0]->get_nBits(), 0);
        SValuePtr ones = ops->invert(zeros);
        SValuePtr result = ops->ite(cond, ones, zeros);
        d->write(args[0], result);
    }
};

struct IP_dup: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr src = d->read(args[1]);
        ASSERT_require(args[0]->get_nBits() >= src->get_width());
        size_t nElmts = args[0]->get_nBits() / src->get_width();
        ASSERT_require(nElmts * src->get_width() == args[0]->get_nBits());
        SValuePtr toWrite;
        for (size_t i = 0; i < nElmts; ++i)
            toWrite = toWrite ? ops->concat(toWrite, src) : src;
        d->write(args[0], toWrite);
    }
};

struct IP_eor: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValuePtr a = d->read(args[1]);
        SValuePtr b = ops->unsignedExtend(d->read(args[2]), a->get_width());
        SValuePtr result = ops->xor_(a, b);
        d->write(args[0], result);
    }
};

struct IP_ins: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr result = d->read(args[1]);
        SValuePtr extended = ops->signExtend(result, args[0]->get_nBits());
        d->write(args[0], extended);
    }
};

struct IP_ldar: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr result = d->read(args[1]);
        d->write(args[0], result);
    }
};

struct IP_ldarb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr result = ops->unsignedExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], result);
    }
};

struct IP_ldarh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr result = ops->unsignedExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], result);
    }
};

struct IP_ldaxr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr result = d->read(args[1]);
        d->write(args[0], result);
    }
};

struct IP_ldaxrb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr result = ops->unsignedExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], result);
    }
};

struct IP_ldaxrh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr result = ops->unsignedExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], result);
    }
};

struct IP_ldp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        size_t regSize = args[0]->get_type()->get_nBits();
        ASSERT_require(args[1]->get_type()->get_nBits() == regSize);
        SValuePtr pair = d->read(args[2]);
        ASSERT_require(pair->get_width() == 2 * regSize);
        SValuePtr first = ops->extract(pair, 0, regSize);
        SValuePtr second = ops->extract(pair, regSize, pair->get_width());
        d->write(args[0], first);
        d->write(args[1], second);
    }
};

struct IP_ldr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr result = d->read(args[1]);
        SValuePtr extended = ops->signExtend(result, args[0]->get_nBits());
        d->write(args[0], extended);
    }
};

struct IP_ldrb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr byte = d->read(args[1]);
        SValuePtr extended = ops->unsignedExtend(byte, args[0]->get_nBits());
        d->write(args[0], extended);
    }
};

struct IP_ldrh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr half = d->read(args[1]);
        SValuePtr extended = ops->unsignedExtend(half, args[0]->get_nBits());
        d->write(args[0], extended);
    }
};

struct IP_ldrsb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr value = ops->signExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], value);
    }
};

struct IP_ldrsh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr value = ops->signExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], value);
    }
};

struct IP_ldrsw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr value = ops->signExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], value);
    }
};

struct IP_ldur: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr value = ops->unsignedExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], value);
    }
};

struct IP_ldurb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr value = ops->unsignedExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], value);
    }
};

struct IP_ldurh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr value = ops->unsignedExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], value);
    }
};

struct IP_ldursb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr value = ops->signExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], value);
    }
};

struct IP_ldursh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr value = ops->signExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], value);
    }
};

struct IP_ldursw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr value = ops->signExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], value);
    }
};

struct IP_ldxr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr result = d->read(args[1]);
        d->write(args[0], result);
    }
};

struct IP_ldxrb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr result = ops->unsignedExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], result);
    }
};

struct IP_ldxrh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr result = ops->unsignedExtend(d->read(args[1]), args[0]->get_nBits());
        d->write(args[0], result);
    }
};

struct IP_lsl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValuePtr a = d->read(args[1]);
        SValuePtr b = d->read(args[2]);
        // "The remainder obtained by dividing the second source register by the data size defines the number of bits by which
        // the first source register is left shifted."  We'll use masking instead of division since that seems like a much
        // simpler operation!
        size_t nBits = args[0]->get_nBits();
        ASSERT_require(32 == nBits || 64 == nBits);
        SValuePtr shiftAmount = ops->extract(b, 0, 32==nBits ? 5 : 6);
        SValuePtr shifted = ops->shiftLeft(a, shiftAmount);
        d->write(args[0], shifted);
    }
};

struct IP_lsr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValuePtr a = d->read(args[1]);
        SValuePtr b = d->read(args[2]);
        // "The remainder obtained by dividing the second source register by the data size defines the number of bits by which
        // the first source register is right-shifted."  We'll use masking instead of division since that seems like a much
        // simpler operation!
        size_t nBits = args[0]->get_nBits();
        ASSERT_require(32 == nBits || 64 == nBits);
        SValuePtr shiftAmount = ops->extract(b, 0, 32==nBits ? 5 : 6);
        SValuePtr shifted = ops->shiftRight(a, shiftAmount);
        d->write(args[0], shifted);
    }
};

struct IP_mov: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr result = d->read(args[1]);
        SValuePtr extended = ops->signExtend(result, args[0]->get_nBits());
        d->write(args[0], extended);
    }
};

struct IP_movk: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);

        // The 2nd argument is the 16-bit constant to place into the destination, but it also has an optional left shift. We
        // need to use the left shift to decide where to place the 16 bits.
        SValuePtr newBits;
        size_t shiftAmount = 0;
        if (auto lsl = isSgAsmBinaryLsl(args[1])) {
            newBits = d->read(lsl->get_lhs(), 16);
            shiftAmount = d->read(lsl->get_rhs())->get_number();
        } else {
            newBits = d->read(args[1], 16);
        }

        SValuePtr oldBits = d->read(args[0]);
        SValuePtr result;
        if (shiftAmount > 0) {
            result = ops->concat(ops->extract(oldBits, 0, shiftAmount), newBits);
        } else {
            result = newBits;
        }
        if (shiftAmount + 16 < oldBits->get_width())
            result = ops->concat(result, ops->extract(oldBits, shiftAmount+16, oldBits->get_width()));
        d->write(args[0], result);
    }
};

struct IP_movn: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr result = d->read(args[1]);
        SValuePtr extended = ops->unsignedExtend(result, args[0]->get_nBits());
        SValuePtr inverted = ops->invert(extended);
        d->write(args[0], inverted);
    }
};

struct IP_movz: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr result = d->read(args[1]);
        SValuePtr extended = ops->unsignedExtend(result, args[0]->get_nBits());
        d->write(args[0], extended);
    }
};

struct IP_msub: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        SValuePtr a = d->read(args[1]);
        SValuePtr b = d->read(args[2]);
        SValuePtr c = d->read(args[3]);
        SValuePtr product = ops->unsignedExtend(ops->unsignedMultiply(a, b), c->get_width());
        SValuePtr diff = ops->subtract(c, product);
        d->write(args[0], diff);
    }
};

struct IP_mul: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValuePtr a = d->read(args[1]);
        SValuePtr b = d->read(args[2]);
        SValuePtr result;

        if (auto vectorType = isSgAsmVectorType(args[1]->get_type())) {
            if (isSgAsmVectorType(args[2])) {
                // Vector * vector, element-wise
                size_t elmtSize = vectorType->get_elmtType()->get_nBits();
                for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
                    SValuePtr elmtA = ops->extract(a, i*elmtSize, (i+1)*elmtSize);
                    SValuePtr elmtB = ops->extract(b, i*elmtSize, (i+1)*elmtSize);
                    SValuePtr elmtResult = ops->unsignedExtend(ops->unsignedMultiply(elmtA, elmtB), elmtSize);
                    result = result ? ops->concat(result, elmtResult) : elmtResult;
                }
            } else {
                // Vector * scalar, element-wise
                size_t elmtSize = vectorType->get_elmtType()->get_nBits();
                for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
                    SValuePtr elmtA = ops->extract(a, i*elmtSize, (i+1)*elmtSize);
                    SValuePtr elmtResult = ops->unsignedExtend(ops->unsignedMultiply(elmtA, b), elmtSize);
                    result = result ? ops->concat(result, elmtResult) : elmtResult;
                }
            }
        } else {
            // Scalar * scalar
            result = ops->unsignedExtend(ops->unsignedMultiply(a, b), args[0]->get_nBits());
        }
        d->write(args[0], result);
    }
};

struct IP_neg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr value = d->read(args[1]);
        SValuePtr result;
        if (auto vectorType = isSgAsmVectorType(args[1]->get_type())) {
            size_t elmtSize = vectorType->get_elmtType()->get_nBits();
            for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
                SValuePtr elmt = ops->extract(value, i*elmtSize, (i+1)*elmtSize);
                SValuePtr elmtResult = ops->negate(elmt);
                result = result ? ops->concat(result, elmtResult) : elmtResult;
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
        SValuePtr value = d->read(args[1]);
        SValuePtr zero = ops->number_(value->get_width(), 0);
        SValuePtr carryIn = ops->boolean_(true);
        SValuePtr carryOut;
        SValuePtr result = ops->addWithCarries(zero, ops->invert(value), carryIn, carryOut);
        d->updateNZCV(result, carryOut);
        d->write(args[0], result);
    }
};

struct IP_ngc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr a = d->read(args[1]);                 // the value to negate
        SValuePtr b = ops->unsignedExtend(ops->readRegister(d->REG_CPSR_C), a->get_width());
        SValuePtr carryIn = ops->boolean_(true);
        SValuePtr carryOut;
        SValuePtr result = ops->addWithCarries(b, ops->invert(a), carryIn, carryOut);
        d->write(args[0], result);
    }
};

struct IP_ngcs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr a = d->read(args[1]);                 // the value to negate
        SValuePtr b = ops->unsignedExtend(ops->readRegister(d->REG_CPSR_C), a->get_width());
        SValuePtr carryIn = ops->boolean_(true);
        SValuePtr carryOut;
        SValuePtr result = ops->addWithCarries(b, ops->invert(a), carryIn, carryOut);
        d->updateNZCV(result, carryOut);
        d->write(args[0], result);
    }
};

struct IP_nop: P {
    void p(D d, Ops ops, I insn, A args) {
    }
};

struct IP_not: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr value = d->read(args[1]);
        SValuePtr result = ops->invert(value);
        d->write(args[0], result);
    }
};

struct IP_orn: P {
    void p(D d, Ops ops, I insn, A args) {
        SValuePtr a = d->read(args[1]);
        SValuePtr b = ops->unsignedExtend(d->read(args[2]), a->get_width());
        SValuePtr result = ops->or_(a, ops->invert(b));
        d->write(args[0], result);
    }
};

struct IP_orr: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.size() == 2) {
            SValuePtr a1 = d->read(args[0]);
            SValuePtr a2 = d->advSimdExpandImm(args[0]->get_type(), d->read(args[1]));
            SValuePtr result = ops->or_(a1, a2);
            d->write(args[0], result);
        } else {
            assert_args(insn, args, 3);
            SValuePtr a1 = d->read(args[1], args[0]->get_nBits());
            SValuePtr a2 = d->read(args[2], args[0]->get_nBits());
            SValuePtr result = ops->or_(a1, a2);
            d->write(args[0], result);
        }
    }
};

struct IP_ret: P {
    void p(D d, Ops ops, I insn, A args) {
        SValuePtr targetVa;
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
        SValuePtr value = d->read(args[1]);
        size_t nBytes = value->get_width() / 8;
        SValuePtr result;
        for (size_t i = 0; i < nBytes; ++i) {
            SValuePtr byte = ops->extract(value, i*8, (i+1)*8);
            result = result ? ops->concat(byte, result) : byte;
        }
        d->write(args[0], result);
    }
};

struct IP_rev16: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr value = d->read(args[1]);
        size_t dataSize = args[0]->get_nBits();         // bits
        size_t containerSize = 16;                      // bits
        size_t bytesPerContainer = containerSize / 8;
        size_t nContainers = dataSize / containerSize;
        SValuePtr result;
        for (size_t i = 0; i < nContainers; ++i) {
            for (size_t j = bytesPerContainer; j > 0; --j) {
                size_t lsb = i * containerSize + (j-1) * 8;
                SValuePtr byte = ops->extract(value, lsb, lsb+8);
                result = result ? ops->concat(result, byte) : byte;
            }
        }
        d->write(args[0], result);
    }
};

struct IP_sbfiz: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        size_t lsb = d->read(args[2])->get_number();
        size_t width = d->read(args[3])->get_number();
        uint64_t immR = -lsb % args[0]->get_nBits();
        uint64_t immS = width - 1;
        bool n = 64 == args[0]->get_nBits();
        d->signedBitfieldMove(ops, args[0], args[1], n, immR, immS);
    }
};

struct IP_sbfm: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        size_t immR = d->read(args[2])->get_number();
        size_t immS = d->read(args[3])->get_number();
        bool n = 64 == args[0]->get_nBits();
        d->signedBitfieldMove(ops, args[0], args[1], n, immR, immS);
    }
};

struct IP_sbfx: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        size_t lsb = d->read(args[2])->get_number();
        size_t width = d->read(args[3])->get_number();
        uint64_t immR = lsb;
        uint64_t immS = lsb + width - 1;
        bool n = 64 == args[0]->get_nBits();
        d->signedBitfieldMove(ops, args[0], args[1], n, immR, immS);
    }
};

struct IP_stp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValuePtr first = d->read(args[0]);
        SValuePtr second = d->read(args[1]);
        SValuePtr pair = ops->concat(first, second);
        d->write(args[2], pair);
    }
};

struct IP_str: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr src = d->read(args[0]);
        d->write(args[1], src);
    }
};

struct IP_strb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr value = d->read(args[0]);
        SValuePtr byte = ops->extract(value, 0, 8);
        d->write(args[1], byte);
    }
};

struct IP_strh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr value = d->read(args[0]);
        SValuePtr byte = ops->extract(value, 0, 16);
        d->write(args[1], byte);
    }
};

struct IP_stur: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr value = d->read(args[0]);
        d->write(args[1], value);
    }
};

struct IP_sturb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr value = d->read(args[0]);
        d->write(args[1], value);
    }
};

struct IP_sturh: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr value = d->read(args[0]);
        d->write(args[1], value);
    }
};

struct IP_stxr: P {
    void p(D d, Ops ops, I insn, A args) {
        // ROSE assumes that the PE always has exclusive access to all memory
        assert_args(insn, args, 3);
        SValuePtr value = d->read(args[1]);
        d->write(args[2], value);
        SValuePtr status = ops->number_(args[0]->get_nBits(), 0);
        d->write(args[0], status);
    }
};

struct IP_stxrb: P {
    void p(D d, Ops ops, I insn, A args) {
        // ROSE assumes that the PE always has exclusive access to all memory.
        assert_args(insn, args, 3);
        // Note: ARM documentation doesn't explicitly truncate the "data = X[t]", but I think that's a mistake given the
        // corresponding "data = bits(8) UNKNOWN" from the same "if" and the following "Mem[address, 1, ...] = data".
        SValuePtr value = ops->unsignedExtend(d->read(args[1]), 8);
        d->write(args[2], value);
        SValuePtr status = ops->number_(args[0]->get_nBits(), 0);
        d->write(args[0], status);
    }
};

struct IP_stxrh: P {
    void p(D d, Ops ops, I insn, A args) {
        // ROSE assumes that the PE always has exclusive access to all memory.
        assert_args(insn, args, 3);
        // Note: ARM documentation doesn't explicitly truncate the "data = X[t]", but I think that's a mistake given the
        // corresponding "data = bits(16) UNKNOWN" from the same "if" and the following "Mem[address, 2, ...] = data".
        SValuePtr value = ops->unsignedExtend(d->read(args[1]), 16);
        d->write(args[2], value);
        SValuePtr status = ops->number_(args[0]->get_nBits(), 0);
        d->write(args[0], status);
    }
};

struct IP_sub: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValuePtr minuend = d->read(args[1]);
        SValuePtr subtrahend = ops->signExtend(d->read(args[2]), minuend->get_width());
        SValuePtr result;

        if (auto vectorType = isSgAsmVectorType(args[0]->get_type())) {
            size_t elmtSize = vectorType->get_elmtType()->get_nBits();
            for (size_t i = 0; i < vectorType->get_nElmts(); ++i) {
                SValuePtr elmtMinuend = ops->extract(minuend, i*elmtSize, (i+1)*elmtSize);
                SValuePtr elmtSubtrahend = ops->extract(subtrahend, i*elmtSize, (i+1)*elmtSize);
                SValuePtr difference = ops->subtract(elmtMinuend, elmtSubtrahend);
                result = result ? ops->concat(result, difference) : difference;
            }
            ASSERT_forbid(insn->get_updatesFlags());
        } else {
            SValuePtr carryIn = ops->boolean_(true);
            SValuePtr carryOut;
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
        SValuePtr minuend = d->read(args[1]);
        SValuePtr subtrahend = ops->signExtend(d->read(args[2]), minuend->get_width());
        SValuePtr carryIn = ops->boolean_(true);
        SValuePtr carryOut;
        SValuePtr result = ops->addWithCarries(minuend, ops->invert(subtrahend), carryIn, carryOut);
        d->updateNZCV(result, carryOut);
        d->write(args[0], result);
    }
};

struct IP_sxtb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr source = d->read(args[1]);
        SValuePtr byte = ops->extract(source, 0, 8);
        SValuePtr result = ops->signExtend(byte, args[0]->get_nBits());
        d->write(args[0], result);
    }
};

struct IP_sxth: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr source = d->read(args[1]);
        SValuePtr half = ops->extract(source, 0, 16);
        SValuePtr result = ops->signExtend(half, args[0]->get_nBits());
        d->write(args[0], result);
    }
};

struct IP_sxtw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr source = d->read(args[1]);
        SValuePtr word = ops->extract(source, 0, 32);
        SValuePtr result = ops->signExtend(word, args[0]->get_nBits());
        d->write(args[0], result);
    }
};

struct IP_tbnz: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValuePtr index = d->read(args[1]);
        SValuePtr value = d->read(args[0]);
        SValuePtr bit = ops->extract(value, index->get_number(), index->get_number()+1);
        SValuePtr targetVa = d->read(args[2]);
        SValuePtr fallThroughVa = ops->number_(targetVa->get_width(), insn->get_address() + insn->get_size());
        SValuePtr nextIp = ops->ite(bit, targetVa, fallThroughVa);
        ops->writeRegister(d->REG_PC, nextIp);
    }
};

struct IP_tbz: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValuePtr index = d->read(args[1]);
        SValuePtr value = d->read(args[0]);
        SValuePtr bit = ops->extract(value, index->get_number(), index->get_number()+1);
        SValuePtr targetVa = d->read(args[2]);
        SValuePtr fallThroughVa = ops->number_(targetVa->get_width(), insn->get_address() + insn->get_size());
        SValuePtr nextIp = ops->ite(bit, fallThroughVa, targetVa);
        ops->writeRegister(d->REG_PC, nextIp);
    }
};

struct IP_tst: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr a = d->read(args[0]);
        SValuePtr b = ops->unsignedExtend(d->read(args[1]), a->get_width());
        SValuePtr result = ops->and_(a, b);
        ops->writeRegister(d->REG_CPSR_N, ops->extract(result, result->get_width()-1, result->get_width()));
        ops->writeRegister(d->REG_CPSR_Z, ops->equalToZero(result));
        ops->writeRegister(d->REG_CPSR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CPSR_V, ops->boolean_(false));
    }
};

struct IP_ubfiz: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        size_t lsb = d->read(args[2])->get_number();
        size_t width = d->read(args[3])->get_number();
        uint64_t immR = -lsb % args[0]->get_nBits();
        uint64_t immS = width - 1;
        bool n = 64 == args[0]->get_nBits();
        d->unsignedBitfieldMove(ops, args[0], args[1], n, immR, immS);
    }
};

struct IP_ubfm: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        size_t immR = d->read(args[2])->get_number();
        size_t immS = d->read(args[3])->get_number();
        bool n = 64 == args[0]->get_nBits();
        d->unsignedBitfieldMove(ops, args[0], args[1], n, immR, immS);
    }
};

struct IP_ubfx: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        size_t lsb = d->read(args[2])->get_number();
        size_t width = d->read(args[3])->get_number();
        uint64_t immR = lsb;
        uint64_t immS = lsb + width - 1;
        bool n = 64 == args[0]->get_nBits();
        d->unsignedBitfieldMove(ops, args[0], args[1], n, immR, immS);
    }
};

struct IP_udiv: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValuePtr a = d->read(args[1]);
        SValuePtr b = d->read(args[2]);
        SValuePtr result = ops->unsignedDivide(a, b);
        d->write(args[0], result);
    }
};

struct IP_umov: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr result = d->read(args[1]);
        SValuePtr extended = ops->unsignedExtend(result, args[0]->get_nBits());
        d->write(args[0], extended);
    }
};

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
DispatcherA64::initializeInsnDispatchTable() {
    iproc_set(ARM64_INS_ADD,    new A64::IP_add);
    iproc_set(ARM64_INS_ADRP,   new A64::IP_adrp);
    iproc_set(ARM64_INS_AND,    new A64::IP_and);
    iproc_set(ARM64_INS_ASR,    new A64::IP_asr);
    //iproc_set(ARM64_INS_ASRV,   new A64::IP_asrv); -- see ARM64_INS_ASR
    iproc_set(ARM64_INS_B,      new A64::IP_b);
    iproc_set(ARM64_INS_BL,     new A64::IP_bl);
    iproc_set(ARM64_INS_BLR,    new A64::IP_blr);
    iproc_set(ARM64_INS_BR,     new A64::IP_br);
    iproc_set(ARM64_INS_CBNZ,   new A64::IP_cbnz);
    iproc_set(ARM64_INS_CBZ,    new A64::IP_cbz);
    iproc_set(ARM64_INS_CCMP,   new A64::IP_ccmp);
    iproc_set(ARM64_INS_CMN,    new A64::IP_cmn);
    iproc_set(ARM64_INS_CMP,    new A64::IP_cmp);
    iproc_set(ARM64_INS_CSEL,   new A64::IP_csel);
    iproc_set(ARM64_INS_CSET,   new A64::IP_cset);
    iproc_set(ARM64_INS_CSETM,  new A64::IP_csetm);
    iproc_set(ARM64_INS_DUP,    new A64::IP_dup);
    iproc_set(ARM64_INS_EOR,    new A64::IP_eor);
    iproc_set(ARM64_INS_INS,    new A64::IP_ins);
    iproc_set(ARM64_INS_LDAR,   new A64::IP_ldar);
    iproc_set(ARM64_INS_LDARB,  new A64::IP_ldarb);
    iproc_set(ARM64_INS_LDARH,  new A64::IP_ldarh);
    iproc_set(ARM64_INS_LDAXR,  new A64::IP_ldaxr);
    iproc_set(ARM64_INS_LDAXRB, new A64::IP_ldaxrb);
    iproc_set(ARM64_INS_LDAXRH, new A64::IP_ldaxrh);
    iproc_set(ARM64_INS_LDP,    new A64::IP_ldp);
    iproc_set(ARM64_INS_LDR,    new A64::IP_ldr);
    iproc_set(ARM64_INS_LDRB,   new A64::IP_ldrb);
    iproc_set(ARM64_INS_LDRH,   new A64::IP_ldrh);
    iproc_set(ARM64_INS_LDRSB,  new A64::IP_ldrsb);
    iproc_set(ARM64_INS_LDRSH,  new A64::IP_ldrsh);
    iproc_set(ARM64_INS_LDRSW,  new A64::IP_ldrsw);
    iproc_set(ARM64_INS_LDUR,   new A64::IP_ldur);
    iproc_set(ARM64_INS_LDURB,  new A64::IP_ldurb);
    iproc_set(ARM64_INS_LDURH,  new A64::IP_ldurh);
    iproc_set(ARM64_INS_LDURSB, new A64::IP_ldursb);
    iproc_set(ARM64_INS_LDURSH, new A64::IP_ldursh);
    iproc_set(ARM64_INS_LDURSW, new A64::IP_ldursw);
    iproc_set(ARM64_INS_LDXR,   new A64::IP_ldxr);
    iproc_set(ARM64_INS_LDXRB,  new A64::IP_ldxrb);
    iproc_set(ARM64_INS_LDXRH,  new A64::IP_ldxrh);
    iproc_set(ARM64_INS_LSL,    new A64::IP_lsl);
    iproc_set(ARM64_INS_LSR,    new A64::IP_lsr);
    iproc_set(ARM64_INS_MOV,    new A64::IP_mov);
    iproc_set(ARM64_INS_MOVK,   new A64::IP_movk);
    iproc_set(ARM64_INS_MOVN,   new A64::IP_movn);
    iproc_set(ARM64_INS_MOVZ,   new A64::IP_movz);
    iproc_set(ARM64_INS_MSUB,   new A64::IP_msub);
    iproc_set(ARM64_INS_MUL,    new A64::IP_mul);
    iproc_set(ARM64_INS_NEG,    new A64::IP_neg);
    iproc_set(ARM64_INS_NEGS,   new A64::IP_negs);
    iproc_set(ARM64_INS_NGC,    new A64::IP_ngc);
    iproc_set(ARM64_INS_NGCS,   new A64::IP_ngcs);
    iproc_set(ARM64_INS_NOP,    new A64::IP_nop);
    iproc_set(ARM64_INS_NOT,    new A64::IP_not);
    iproc_set(ARM64_INS_ORN,    new A64::IP_orn);
    iproc_set(ARM64_INS_ORR,    new A64::IP_orr);
    iproc_set(ARM64_INS_RET,    new A64::IP_ret);
    iproc_set(ARM64_INS_REV,    new A64::IP_rev);
    iproc_set(ARM64_INS_REV16,  new A64::IP_rev16);
    iproc_set(ARM64_INS_SBFIZ,  new A64::IP_sbfiz);
    iproc_set(ARM64_INS_SBFM,   new A64::IP_sbfm);
    iproc_set(ARM64_INS_SBFX,   new A64::IP_sbfx);
    iproc_set(ARM64_INS_STP,    new A64::IP_stp);
    iproc_set(ARM64_INS_STR,    new A64::IP_str);
    iproc_set(ARM64_INS_STRB,   new A64::IP_strb);
    iproc_set(ARM64_INS_STRH,   new A64::IP_strh);
    iproc_set(ARM64_INS_STUR,   new A64::IP_stur);
    iproc_set(ARM64_INS_STURB,  new A64::IP_sturb);
    iproc_set(ARM64_INS_STURH,  new A64::IP_sturh);
    iproc_set(ARM64_INS_STXR,   new A64::IP_stxr);
    iproc_set(ARM64_INS_STXRB,  new A64::IP_stxrb);
    iproc_set(ARM64_INS_STXRH,  new A64::IP_stxrh);
    iproc_set(ARM64_INS_SUB,    new A64::IP_sub);
    //iproc_set(ARM64_INS_SUBS,   new A64::IP_subs); -- see ARM64_INS_SUB
    iproc_set(ARM64_INS_SXTB,   new A64::IP_sxtb);
    iproc_set(ARM64_INS_SXTH,   new A64::IP_sxth);
    iproc_set(ARM64_INS_SXTW,   new A64::IP_sxtw);
    iproc_set(ARM64_INS_TBNZ,   new A64::IP_tbnz);
    iproc_set(ARM64_INS_TBZ,    new A64::IP_tbz);
    iproc_set(ARM64_INS_TST,    new A64::IP_tst);
    iproc_set(ARM64_INS_UBFIZ,  new A64::IP_ubfiz);
    iproc_set(ARM64_INS_UBFM,   new A64::IP_ubfm);
    iproc_set(ARM64_INS_UBFX,   new A64::IP_ubfx);
    iproc_set(ARM64_INS_UDIV,   new A64::IP_udiv);
    iproc_set(ARM64_INS_UMOV,   new A64::IP_umov);
}

void
DispatcherA64::initializeRegisterDescriptors() {
    if (regdict) {
        REG_PC = findRegister("pc", 64);
        REG_SP = findRegister("sp", 64);
        REG_LR = findRegister("lr", 64);
        REG_CPSR_N = findRegister("cpsr.n", 1);
        REG_CPSR_Z = findRegister("cpsr.z", 1);
        REG_CPSR_C = findRegister("cpsr.c", 1);
        REG_CPSR_V = findRegister("cpsr.v", 1);
    }
}

void
DispatcherA64::initializeMemory() {
    if (BaseSemantics::StatePtr state = currentState()) {
        if (BaseSemantics::MemoryStatePtr memory = state->memoryState()) {
            switch (memory->get_byteOrder()) {
                case ByteOrder::ORDER_LSB:
                    break;
                case ByteOrder::ORDER_MSB:
                    mlog[WARN] <<"A64 memory state is using big-endian byte order\n";
                    break;
                case ByteOrder::ORDER_UNSPECIFIED:
                    memory->set_byteOrder(ByteOrder::ORDER_LSB);
                    break;
            }
        }
    }
}

RegisterDescriptor
DispatcherA64::instructionPointerRegister() const {
    return REG_PC;
}

RegisterDescriptor
DispatcherA64::stackPointerRegister() const {
    return REG_SP;
}

RegisterDescriptor
DispatcherA64::callReturnRegister() const {
    return REG_LR;
}

void
DispatcherA64::set_register_dictionary(const RegisterDictionary *regdict) {
    BaseSemantics::Dispatcher::set_register_dictionary(regdict);
    initializeRegisterDescriptors();
}

int
DispatcherA64::iproc_key(SgAsmInstruction *insn_) const {
    auto insn = isSgAsmA64Instruction(insn_);
    ASSERT_not_null(insn);
    return insn->get_kind();
}

SValuePtr
DispatcherA64::read(SgAsmExpression *e, size_t value_nbits/*=0*/, size_t addr_nbits/*=0*/) {
    // Reading from general purpose register 31 always returns zero
    if (auto rre = isSgAsmRegisterReferenceExpression(e)) {
        RegisterDescriptor reg = rre->get_descriptor();
        if (reg.majorNumber() == arm_regclass_gpr && reg.minorNumber() == 31) {
            if (0 == value_nbits)
                value_nbits = reg.nBits();
            return operators->number_(value_nbits, 0);
        }
    }
    return BaseSemantics::Dispatcher::read(e, value_nbits, addr_nbits);
}

void
DispatcherA64::write(SgAsmExpression *e, const SValuePtr &value, size_t addr_nbits/*=0*/) {
    // Writes to general purpose register 31 are always discarded
    if (auto rre = isSgAsmRegisterReferenceExpression(e)) {
        RegisterDescriptor reg = rre->get_descriptor();
        if (reg.majorNumber() == arm_regclass_gpr && reg.minorNumber() == 31)
            return;
    }
    BaseSemantics::Dispatcher::write(e, value, addr_nbits);
}

SValuePtr
DispatcherA64::advSimdExpandImm(SgAsmType *type, const SValuePtr &imm) {
    ASSERT_not_null(type);
    ASSERT_not_null(imm);
    auto vectorType = isSgAsmVectorType(type);
    ASSERT_not_null(vectorType);
    auto elmtType = vectorType->get_elmtType();
    ASSERT_not_null(elmtType);
    ASSERT_require(isSgAsmScalarType(elmtType));
    SValuePtr elmt = operators->unsignedExtend(imm, elmtType->get_nBits());
    SValuePtr result;
    for (size_t i = 0; i < vectorType->get_nElmts(); ++i)
        result = result ? operators->concat(result, elmt) : elmt;
    return result;
}

DispatcherA64::NZCV
DispatcherA64::computeNZCV(const SValuePtr &sum, const SValuePtr &carries) {
    ASSERT_not_null(sum);
    ASSERT_require(sum->get_width() > 1);
    ASSERT_not_null(carries);
    ASSERT_require(carries->get_width() == sum->get_width());

    SValuePtr isNeg = operators->extract(sum, sum->get_width()-1, sum->get_width());
    SValuePtr isZero = operators->equalToZero(sum);
    SValuePtr isCarry = operators->extract(carries, carries->get_width()-1, carries->get_width());
    SValuePtr isOverflow = operators->xor_(operators->extract(carries, carries->get_width()-1, carries->get_width()),
                                           operators->extract(carries, carries->get_width()-2, carries->get_width()-1));

    return NZCV(isNeg, isZero, isCarry, isOverflow);
}

void
DispatcherA64::updateNZCV(const SValuePtr &sum, const SValuePtr &carries) {
    NZCV nzcv = computeNZCV(sum, carries);

    operators->writeRegister(REG_CPSR_N, nzcv.n);
    operators->writeRegister(REG_CPSR_Z, nzcv.z);
    operators->writeRegister(REG_CPSR_C, nzcv.c);
    operators->writeRegister(REG_CPSR_V, nzcv.v);
}

SValuePtr
DispatcherA64::conditionHolds(A64InstructionCondition cond) {
    switch (cond) {
        case A64InstructionCondition::ARM64_CC_INVALID: // occurs for "B" instruction
            return operators->boolean_(true);
        case A64InstructionCondition::ARM64_CC_EQ:      // equal
            return operators->readRegister(REG_CPSR_Z);
        case A64InstructionCondition::ARM64_CC_NE:      // not equal: not equal, or unordered
            return operators->invert(operators->readRegister(REG_CPSR_Z));
        case A64InstructionCondition::ARM64_CC_HS:      // unsigned higher or same: >, ==, or unordered
            return operators->readRegister(REG_CPSR_C);
        case A64InstructionCondition::ARM64_CC_LO:      // unsigned lower or same: less than
            return operators->invert(operators->readRegister(REG_CPSR_C));
        case A64InstructionCondition::ARM64_CC_MI:      // minus, negative: less than
            return operators->readRegister(REG_CPSR_N);
        case A64InstructionCondition::ARM64_CC_PL:      // plus, positive or zero: >, ==, or unordered
            return operators->invert(operators->readRegister(REG_CPSR_N));
        case A64InstructionCondition::ARM64_CC_VS:      // overflow: unordered
            return operators->readRegister(REG_CPSR_V);
        case A64InstructionCondition::ARM64_CC_VC:      // no overflow: ordered
            return operators->invert(operators->readRegister(REG_CPSR_V));
        case A64InstructionCondition::ARM64_CC_HI: {    // unsigned higher: greater than, or unordered
            SValuePtr c = operators->readRegister(REG_CPSR_C);
            SValuePtr z = operators->readRegister(REG_CPSR_Z);
            return operators->and_(c, operators->invert(z));
        }
        case A64InstructionCondition::ARM64_CC_LS: {    // unsigned lower or same: less than or equal
            SValuePtr c = operators->readRegister(REG_CPSR_C);
            SValuePtr z = operators->readRegister(REG_CPSR_Z);
            return operators->invert(operators->and_(c, operators->invert(z)));
        }
        case A64InstructionCondition::ARM64_CC_GE: {    // greater than or equal: greater than or equal
            SValuePtr n = operators->readRegister(REG_CPSR_N);
            SValuePtr v = operators->readRegister(REG_CPSR_V);
            return operators->invert(operators->xor_(n, v));
        }
        case A64InstructionCondition::ARM64_CC_LT: {    // less than: less than, or unordered
            SValuePtr n = operators->readRegister(REG_CPSR_N);
            SValuePtr v = operators->readRegister(REG_CPSR_V);
            return operators->xor_(n, v);
        }
        case A64InstructionCondition::ARM64_CC_GT: {    // signed greater than: greater than
            SValuePtr n = operators->readRegister(REG_CPSR_N);
            SValuePtr v = operators->readRegister(REG_CPSR_V);
            SValuePtr z = operators->readRegister(REG_CPSR_Z);
            SValuePtr nEqV = operators->invert(operators->xor_(n, v));
            return operators->and_(nEqV, z);
        }
        case A64InstructionCondition::ARM64_CC_LE: {    // signed less than or equal: <, ==, or unorderd
            SValuePtr n = operators->readRegister(REG_CPSR_N);
            SValuePtr v = operators->readRegister(REG_CPSR_V);
            SValuePtr z = operators->readRegister(REG_CPSR_Z);
            SValuePtr nEqV = operators->invert(operators->xor_(n, v));
            return operators->invert(operators->and_(nEqV, z));
        }
        case A64InstructionCondition::ARM64_CC_AL:      // always (unconditional): always (unconditional)
        case A64InstructionCondition::ARM64_CC_NV:      // always (unconditional): always (unconditional)
            return operators->boolean_(true);
    }
    ASSERT_not_reachable("invalid condition");
}

std::pair<uint64_t/* m bits */, uint64_t/*m bits*/>
DispatcherA64::decodeBitMasks(size_t m, bool immN, uint64_t immS/*6 bits*/, uint64_t immR/*6 bits*/, bool immediate) {
#if 0 // DEBUGGING [Robb Matzke 2020-07-24]
    std::cerr <<"ROBB: (bits(" <<m <<"), bits(" <<m <<")) DecodeBitMask(bit immN=" <<(immN?1:0)
              <<", bits(6) imms=" <<StringUtility::toBinary(immS, 6)
              <<", bits(6) immr=" <<StringUtility::toBinary(immR, 6)
              <<", immediate=" <<(immediate?1:0) <<")\n";
#endif
    // This code comes from the ARM documentation.  C++ doesn't have arbitrary size integers, so we use the largest we'll ever
    // need (uint64_t) and make sure all our arithmetic uses only the least significant M bits.
    ASSERT_require(m <= 64);
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
    ASSERT_forbid(immediate && (immS & levels) == levels); // if immediate && (imms AND levels) == levels then UNDEFINED;

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
DispatcherA64::unsignedBitfieldMove(RiscOperators *ops, SgAsmExpression *dstExpr, SgAsmExpression *srcExpr, bool n,
                                    uint64_t immR, uint64_t immS) {
    ASSERT_not_null(dstExpr);
    ASSERT_not_null(srcExpr);
    ASSERT_require(dstExpr->get_nBits() == srcExpr->get_nBits());

    size_t dataSize = dstExpr->get_nBits();
    auto wtmasks = decodeBitMasks(dataSize, n, immS, immR, false);
    SValuePtr wmask = ops->number_(dataSize, wtmasks.first);
    SValuePtr tmask = ops->number_(dataSize, wtmasks.second);

    SValuePtr src = read(srcExpr);

    // Perform bitfield move on the low bits
    SValuePtr r = ops->number_(6, immR);
    SValuePtr bot = ops->and_(ops->rotateRight(src, r), wmask);

    // Combine extension bits and result bits
    SValuePtr combined = ops->and_(bot, tmask);
    write(dstExpr, combined);
}

void
DispatcherA64::signedBitfieldMove(RiscOperators *ops, SgAsmExpression *dstExpr, SgAsmExpression *srcExpr, bool n,
                                    uint64_t immR, uint64_t immS) {
    ASSERT_not_null(dstExpr);
    ASSERT_not_null(srcExpr);
    ASSERT_require(dstExpr->get_nBits() == srcExpr->get_nBits());

    size_t dataSize = dstExpr->get_nBits();
    auto wtmasks = decodeBitMasks(dataSize, n, immS, immR, false);
    SValuePtr wmask = ops->number_(dataSize, wtmasks.first);
    SValuePtr tmask = ops->number_(dataSize, wtmasks.second);

    SValuePtr src = read(srcExpr);

    // Perform bitfield move on low bits
    SValuePtr r = ops->number_(6, immR);
    SValuePtr bot = ops->and_(ops->rotateRight(src, r), wmask);

    // Determine extension bits (sign, zero or dest register). ARM pseudo code is "bits(datasize) top = Replicate(src<S>);"
    SValuePtr signBit = ops->extract(src, immS, immS+1);
    SValuePtr zero = ops->number_(dataSize, 0);
    SValuePtr ones = ops->invert(zero);
    SValuePtr top = ops->ite(signBit, ones, zero);

    // Combine extensio bits and result bits
    SValuePtr lhs = ops->and_(top, ops->invert(tmask));
    SValuePtr rhs = ops->and_(bot, tmask);
    SValuePtr combined = ops->or_(lhs, rhs);
    write(dstExpr, combined);
}

} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics2::DispatcherA64);
#endif

#endif
