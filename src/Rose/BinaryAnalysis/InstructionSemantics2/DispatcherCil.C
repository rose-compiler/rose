#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include "AsmUnparser_compat.h"
#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics.h>
#include <Rose/Diagnostics.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/DispatcherCil.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/Util.h>
#include "integerOps.h"
#include "stringify.h"
#include <boost/foreach.hpp>

#define M68K 0

using namespace Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functors that handle individual CIL instruction kinds
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Cil {

// An intermediate class that reduces the amount of typing in all that follows.  Its process() method does some up-front
// checking, dynamic casting, and pointer dereferencing and then calls the p() method that does the real work.
class P: public BaseSemantics::InsnProcessor {
public:
    typedef DispatcherCil *D;
    typedef BaseSemantics::RiscOperators *Ops;
    typedef SgAsmCilInstruction *I;
    typedef const SgAsmExpressionPtrList &A;
    virtual void p(D, Ops, I, A) = 0;

    virtual void process(const BaseSemantics::DispatcherPtr &dispatcher_, SgAsmInstruction *insn_) override {
        DispatcherCilPtr dispatcher = DispatcherCil::promote(dispatcher_);
        BaseSemantics::RiscOperatorsPtr operators = dispatcher->operators();
        SgAsmCilInstruction *insn = isSgAsmCilInstruction(insn_);
        ASSERT_not_null(insn);
        ASSERT_require(insn == operators->currentInstruction());

        // Update the PC to point to the fall-through instruction before we process the instruction, so that the semantics for
        // individual instructions (like branches) can override this choice by assigning a new value to PC.  However, we must
        // be careful of PC-relative addressing since the PC value during the addressing operations is the instruction address
        // plus 2.
        dispatcher->advanceInstructionPointer(insn);
        SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
        p(dispatcher.get(), operators.get(), insn, operands);
    }

    void assert_args(I insn, A args, size_t nargs) {
        if (args.size()!=nargs) {
            std::string mesg = "instruction must have " + StringUtility::plural(nargs, "arguments");
            throw BaseSemantics::Exception(mesg, insn);
        }
    }
};

struct IP_abcd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_add: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits() == args[1]->get_nBits());
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        d->decrementRegisters(args[1]);
        SValuePtr a0 = d->read(args[0], nBits);
        SValuePtr a1 = d->read(args[1], nBits);
        SValuePtr result = ops->add(a0, a1);
        d->write(args[1], result);
        d->incrementRegisters(args[0]);
        d->incrementRegisters(args[1]);

        SValuePtr sm = ops->extract(a0, nBits-1, nBits);
        SValuePtr dm = ops->extract(a1, nBits-1, nBits);
        SValuePtr rm = ops->extract(result, nBits-1, nBits);
        SValuePtr isNegative = rm;
        SValuePtr isZero = ops->equalToZero(result);
        SValuePtr v1 = ops->and_(ops->invert(dm), rm);
        SValuePtr v2 = ops->and_(ops->invert(sm), v1);
        SValuePtr isOverflow = ops->or_(ops->and_(sm, ops->and_(dm, ops->invert(rm))), v2);
        SValuePtr v3 = ops->and_(sm, ops->invert(rm));
        SValuePtr v4 = ops->or_(ops->and_(ops->invert(rm), dm), v3);
        SValuePtr isCarry = ops->or_(ops->and_(sm, dm), v4);


        ops->writeRegister(d->REG_CCR_C, isCarry);
        ops->writeRegister(d->REG_CCR_V, isOverflow);
        ops->writeRegister(d->REG_CCR_Z, isZero);
        ops->writeRegister(d->REG_CCR_N, isNegative);
        ops->writeRegister(d->REG_CCR_X, isCarry);
    }
};

struct IP_adda: P {
    void p(D d, Ops ops, I insn, A args) {
        // Documentation is unclear about what happens for instructions like: ADDA [a0++], a0
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits() <= 32);
        ASSERT_require(args[1]->get_nBits() == 32);
        d->decrementRegisters(args[0]);
        SValuePtr a0 = ops->signExtend(d->read(args[0], args[0]->get_nBits()), 32);
        d->incrementRegisters(args[0]);
        d->write(args[1], ops->add(a0, d->read(args[1], 32)));
    }
};

struct IP_addi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits() == args[1]->get_nBits());
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[1]);
        SValuePtr a0 = d->read(args[0], nBits);
        SValuePtr a1 = d->read(args[1], nBits);
        SValuePtr result = ops->add(a0, a1);
        d->write(args[1], result);
        d->incrementRegisters(args[1]);

        SValuePtr sm = ops->extract(a0, nBits-1, nBits);
        SValuePtr dm = ops->extract(a1, nBits-1, nBits);
        SValuePtr rm = ops->extract(result, nBits-1, nBits);
        SValuePtr isNegative = rm;
        SValuePtr isZero = ops->equalToZero(result);
        SValuePtr v1 = ops->and_(ops->invert(dm), rm);
        SValuePtr v2 = ops->and_(ops->invert(sm), v1);
        SValuePtr isOverflow = ops->or_(ops->and_(sm, ops->and_(dm, ops->invert(rm))), v2);
        SValuePtr v3 = ops->and_(sm, ops->invert(rm));
        SValuePtr v4 = ops->or_(ops->and_(ops->invert(rm), dm), v3);
        SValuePtr isCarry = ops->or_(ops->and_(sm, dm), v4);

        ops->writeRegister(d->REG_CCR_C, isCarry);
        ops->writeRegister(d->REG_CCR_V, isOverflow);
        ops->writeRegister(d->REG_CCR_Z, isZero);
        ops->writeRegister(d->REG_CCR_N, isNegative);
        ops->writeRegister(d->REG_CCR_X, isCarry);
    }
};

struct IP_addq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits() == args[1]->get_nBits());
        size_t nBits = args[0]->get_nBits();
        SValuePtr a0 = d->read(args[0], nBits);
        d->decrementRegisters(args[1]);
        SValuePtr a1 = d->read(args[1], nBits);
        SValuePtr result = ops->add(a0, a1);
        d->write(args[1], result);
        d->incrementRegisters(args[1]);

        SValuePtr sm = ops->extract(a0, nBits-1, nBits);
        SValuePtr dm = ops->extract(a1, nBits-1, nBits);
        SValuePtr rm = ops->extract(result, nBits-1, nBits);
        SValuePtr isNegative = rm;
        SValuePtr isZero = ops->equalToZero(result);
        SValuePtr v1 = ops->and_(ops->invert(dm), rm);
        SValuePtr v2 = ops->and_(ops->invert(sm), v1);
        SValuePtr isOverflow = ops->or_(ops->and_(sm, ops->and_(dm, ops->invert(rm))), v2);
        SValuePtr v3 = ops->and_(sm, ops->invert(rm));
        SValuePtr v4 = ops->or_(ops->and_(ops->invert(rm), dm), v3);
        SValuePtr isCarry = ops->or_(ops->and_(sm, dm), v4);

        ops->writeRegister(d->REG_CCR_C, isCarry);
        ops->writeRegister(d->REG_CCR_V, isOverflow);
        ops->writeRegister(d->REG_CCR_Z, isZero);
        ops->writeRegister(d->REG_CCR_N, isNegative);
        ops->writeRegister(d->REG_CCR_X, isCarry);
    }
};

struct IP_addx: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits()==args[1]->get_nBits());
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        d->decrementRegisters(args[1]);
        SValuePtr a0 = d->read(args[0], nBits);
        SValuePtr a1 = d->read(args[1], nBits);
        SValuePtr x = ops->readRegister(d->REG_CCR_X);
        SValuePtr result = ops->add(a0, ops->add(a1, ops->unsignedExtend(x, nBits)));
        d->write(args[1], result);
        d->incrementRegisters(args[0]);
        d->incrementRegisters(args[1]);

        SValuePtr sm = ops->extract(a0, nBits-1, nBits);
        SValuePtr dm = ops->extract(a1, nBits-1, nBits);
        SValuePtr rm = ops->extract(result, nBits-1, nBits);
        SValuePtr isNegative = rm;
        SValuePtr v1 = ops->and_(ops->invert(dm), rm);
        SValuePtr v2 = ops->and_(ops->invert(sm), v1);
        SValuePtr isOverflow = ops->or_(ops->and_(sm, ops->and_(dm, ops->invert(rm))), v2);
        SValuePtr v3 = ops->and_(sm, ops->invert(rm));
        SValuePtr v4 = ops->or_(ops->and_(ops->invert(rm), dm), v3);
        SValuePtr isCarry = ops->or_(ops->and_(sm, dm), v4);

        // Note that the Z bit behaves differently for this instruction than normal.  If the result is non-zero then the Z bit
        // is cleared, otherwise it is unchanged.
        SValuePtr no = ops->boolean_(false);
        SValuePtr ccr_z = ops->readRegister(d->REG_CCR_Z);
        SValuePtr isZero = ops->ite(ops->equalToZero(result), ccr_z, no);

        ops->writeRegister(d->REG_CCR_C, isCarry);
        ops->writeRegister(d->REG_CCR_V, isOverflow);
        ops->writeRegister(d->REG_CCR_Z, isZero);
        ops->writeRegister(d->REG_CCR_N, isNegative);
        ops->writeRegister(d->REG_CCR_X, isCarry);
    }
};

struct IP_and: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits() == args[1]->get_nBits());
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        d->decrementRegisters(args[1]);
        SValuePtr arg1 = d->read(args[1], nBits);
        SValuePtr result = ops->and_(d->read(args[0], nBits), arg1);
        d->write(args[1], result);
        d->incrementRegisters(args[0]);
        d->incrementRegisters(args[1]);

        ops->writeRegister(d->REG_CCR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_V, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(result));
        ops->writeRegister(d->REG_CCR_N, ops->extract(result, nBits-1, nBits));
    }
};

struct IP_andi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits() == args[1]->get_nBits());
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        d->decrementRegisters(args[1]);
        SValuePtr arg1 = d->read(args[1], nBits);
        SValuePtr result = ops->and_(d->read(args[0], nBits), arg1);
        d->write(args[1], result);
        d->incrementRegisters(args[0]);
        d->incrementRegisters(args[1]);

        ops->writeRegister(d->REG_CCR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_V, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(result));
        ops->writeRegister(d->REG_CCR_N, ops->extract(result, nBits-1, nBits));
    }
};

struct IP_asl: P {
    void p(D d, Ops ops, I insn, A args) {
        SgAsmExpression *dst = NULL;
        SValuePtr count;
        if (1==args.size()) {
            assert_args(insn, args, 1);
            ASSERT_require(args[0]->get_nBits()==16);
            count = ops->number_(8, 1);
            dst = args[0];
        } else {
            assert_args(insn, args, 2);
            count = d->read(args[0], args[0]->get_nBits());
            dst = args[1];
        }

        size_t nBits = dst->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr value = d->read(dst, nBits);
        SValuePtr result = ops->shiftLeft(value, count);
        d->write(dst, result);
        d->incrementRegisters(args[0]);

        // CCR_C bit is the final bit shifted off the left (or zero if the shift count is zero).
        SValuePtr isCarry = ops->extract(ops->shiftLeft(ops->concat(value, ops->number_(1, 0)), count), nBits, nBits+1);

        // CCR_X bit is like CCR_C except unmodified if the shift count is zero.
        SValuePtr ccr_x = ops->readRegister(d->REG_CCR_X);
        SValuePtr xBit = ops->ite(ops->equalToZero(count), ccr_x, isCarry);

        // CCR_V bit is set if the if the most significant bit is changed at any time during the shift operation.
        // The CCR_V bit is always clear for a coldfire processor
        SValuePtr isOverflow = ops->undefined_(1);      // FIXME[Robb P. Matzke 2014-07-28]: not implemented

        ops->writeRegister(d->REG_CCR_C, isCarry);
        ops->writeRegister(d->REG_CCR_V, isOverflow);
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(result));
        ops->writeRegister(d->REG_CCR_N, ops->extract(result, nBits-1, nBits));
        ops->writeRegister(d->REG_CCR_X, xBit);
    }
};

struct IP_asr: P {
    void p(D d, Ops ops, I insn, A args) {
        SgAsmExpression *dst = NULL;
        SValuePtr count;
        if (1==args.size()) {
            assert_args(insn, args, 1);
            ASSERT_require(args[0]->get_nBits()==16);
            count = ops->number_(8, 1);
            dst = args[0];
        } else {
            assert_args(insn, args, 2);
            count = d->read(args[0], args[0]->get_nBits());
            dst = args[1];
        }

        size_t nBits = dst->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr value = d->read(dst, nBits);
        SValuePtr result = ops->shiftRightArithmetic(value, count);
        d->write(dst, result);
        d->incrementRegisters(args[0]);

        // CCR_C bit is the final bit shifted off the right (or zero if the shift count is zero).
        SValuePtr isCarry = ops->extract(ops->shiftRightArithmetic(ops->concat(ops->number_(1, 0), value), count), 0, 1);

        // CCR_X bit is like CCR_C except unmodified if the shift count is zero.
        SValuePtr ccr_x = ops->readRegister(d->REG_CCR_X);
        SValuePtr xBit = ops->ite(ops->equalToZero(count), ccr_x, isCarry);

        ops->writeRegister(d->REG_CCR_C, isCarry);
        ops->writeRegister(d->REG_CCR_V, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(result));
        ops->writeRegister(d->REG_CCR_N, ops->extract(result, nBits-1, nBits));
        ops->writeRegister(d->REG_CCR_X, xBit);
    }
};

struct IP_bra: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ASSERT_require(args[0]->get_nBits()==32);
        ops->writeRegister(d->REG_PC, d->read(args[0], 32));
    }
};

struct IP_bsr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ASSERT_require(args[0]->get_nBits()==32);
        SValuePtr negFour = ops->number_(32, -4);
        SValuePtr newSp = ops->add(ops->readRegister(d->REG_A[7]), negFour);
        ops->writeRegister(d->REG_A[7], newSp);
        ops->writeRegister(d->REG_PC, d->read(args[0], 32));
        SValuePtr dstAddr = ops->number_(32, insn->get_address() + insn->get_size());
        ops->writeMemory(RegisterDescriptor(), newSp, dstAddr, ops->boolean_(true));
    }
};

struct IP_bhi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ASSERT_require(args[0]->get_nBits()==32);
        SValuePtr pc = ops->readRegister(d->REG_PC);
        SValuePtr arg0 = d->read(args[0], 32);
        SValuePtr newPc = ops->ite(d->condition(insn->get_kind(), ops), arg0, pc);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_bls: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ASSERT_require(args[0]->get_nBits()==32);
        SValuePtr pc = ops->readRegister(d->REG_PC);
        SValuePtr arg0 = d->read(args[0], 32);
        SValuePtr newPc = ops->ite(d->condition(insn->get_kind(), ops), arg0, pc);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_bcc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ASSERT_require(args[0]->get_nBits()==32);
        SValuePtr pc = ops->readRegister(d->REG_PC);
        SValuePtr arg0 = d->read(args[0], 32);
        SValuePtr newPc = ops->ite(d->condition(insn->get_kind(), ops), arg0, pc);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_bcs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ASSERT_require(args[0]->get_nBits()==32);
        SValuePtr pc = ops->readRegister(d->REG_PC);
        SValuePtr arg0 = d->read(args[0], 32);
        SValuePtr newPc = ops->ite(d->condition(insn->get_kind(), ops), arg0, pc);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_bne: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ASSERT_require(args[0]->get_nBits()==32);
        SValuePtr pc = ops->readRegister(d->REG_PC);
        SValuePtr arg0 = d->read(args[0], 32);
        SValuePtr newPc = ops->ite(d->condition(insn->get_kind(), ops), arg0, pc);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_beq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ASSERT_require(args[0]->get_nBits()==32);
        SValuePtr pc = ops->readRegister(d->REG_PC);
        SValuePtr arg0 = d->read(args[0], 32);
        SValuePtr newPc = ops->ite(d->condition(insn->get_kind(), ops), arg0, pc);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_bvc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ASSERT_require(args[0]->get_nBits()==32);
        SValuePtr pc = ops->readRegister(d->REG_PC);
        SValuePtr arg0 = d->read(args[0], 32);
        SValuePtr newPc = ops->ite(d->condition(insn->get_kind(), ops), arg0, pc);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_bvs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ASSERT_require(args[0]->get_nBits()==32);
        SValuePtr pc = ops->readRegister(d->REG_PC);
        SValuePtr arg0 = d->read(args[0], 32);
        SValuePtr newPc = ops->ite(d->condition(insn->get_kind(), ops), arg0, pc);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_bpl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ASSERT_require(args[0]->get_nBits()==32);
        SValuePtr pc = ops->readRegister(d->REG_PC);
        SValuePtr arg0 = d->read(args[0], 32);
        SValuePtr newPc = ops->ite(d->condition(insn->get_kind(), ops), arg0, pc);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_bmi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ASSERT_require(args[0]->get_nBits()==32);
        SValuePtr pc = ops->readRegister(d->REG_PC);
        SValuePtr arg0 = d->read(args[0], 32);
        SValuePtr newPc = ops->ite(d->condition(insn->get_kind(), ops), arg0, pc);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_bge: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ASSERT_require(args[0]->get_nBits()==32);
        SValuePtr pc = ops->readRegister(d->REG_PC);
        SValuePtr arg0 = d->read(args[0], 32);
        SValuePtr newPc = ops->ite(d->condition(insn->get_kind(), ops), arg0, pc);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_blt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ASSERT_require(args[0]->get_nBits()==32);
        SValuePtr pc = ops->readRegister(d->REG_PC);
        SValuePtr arg0 = d->read(args[0], 32);
        SValuePtr newPc = ops->ite(d->condition(insn->get_kind(), ops), arg0, pc);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_bgt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ASSERT_require(args[0]->get_nBits()==32);
        SValuePtr pc = ops->readRegister(d->REG_PC);
        SValuePtr arg0 = d->read(args[0], 32);
        SValuePtr newPc = ops->ite(d->condition(insn->get_kind(), ops), arg0, pc);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_ble: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ASSERT_require(args[0]->get_nBits()==32);
        SValuePtr pc = ops->readRegister(d->REG_PC);
        SValuePtr arg0 = d->read(args[0], 32);
        SValuePtr newPc = ops->ite(d->condition(insn->get_kind(), ops), arg0, pc);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_bchg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[1]->get_nBits()==8 || args[1]->get_nBits()==32);
        size_t nBits = args[1]->get_nBits();
        size_t logNBits = nBits==8 ? 3 : 5;
        d->decrementRegisters(args[1]);
        SValuePtr a0 = d->read(args[0], args[0]->get_nBits());
        SValuePtr a1 = d->read(args[1], nBits);
        SValuePtr bitNumber = ops->unsignedExtend(a0, logNBits); // bit number modulo destination size
        SValuePtr mask = ops->shiftLeft(ops->number_(nBits, 1), bitNumber);
        SValuePtr result = ops->xor_(mask, a1);
        d->write(args[1], result);
        d->incrementRegisters(args[1]);

        SValuePtr bit = ops->extract(ops->shiftRight(ops->and_(mask, a1), bitNumber), 0, 1);
        ops->writeRegister(d->REG_CCR_Z, ops->invert(bit)); // set if bit was originally clear
    }
};

struct IP_bclr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[1]->get_nBits()==8 || args[1]->get_nBits()==32);
        size_t nBits = args[1]->get_nBits();
        size_t logNBits = nBits==8 ? 3 : 5;
        d->decrementRegisters(args[1]);
        SValuePtr a0 = d->read(args[0], args[0]->get_nBits());
        SValuePtr a1 = d->read(args[1], nBits);
        SValuePtr bitNumber = ops->unsignedExtend(a0, logNBits); // bit number modulo destination size
        SValuePtr mask = ops->shiftLeft(ops->number_(nBits, 1), bitNumber);
        SValuePtr result = ops->and_(ops->invert(mask), a1);
        d->write(args[1], result);
        d->incrementRegisters(args[1]);

        SValuePtr bit = ops->extract(ops->shiftRight(ops->and_(mask, a1), bitNumber), 0, 1);
        ops->writeRegister(d->REG_CCR_Z, ops->invert(bit)); // set if bit was originally clear
    }
};

struct IP_bfchg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bfclr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bfexts: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bfextu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bfins: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bfset: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bftst: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bkpt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bset: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[1]->get_nBits()==8 || args[1]->get_nBits()==32);
        size_t nBits = args[1]->get_nBits();
        size_t logNBits = nBits==8 ? 3 : 5;
        d->decrementRegisters(args[1]);
        SValuePtr a0 = d->read(args[0], args[0]->get_nBits());
        SValuePtr a1 = d->read(args[1], nBits);
        SValuePtr bitNumber = ops->unsignedExtend(a0, logNBits); // bit number modulo destination size
        SValuePtr mask = ops->shiftLeft(ops->number_(nBits, 1), bitNumber);
        SValuePtr result = ops->or_(mask, a1);
        d->write(args[1], result);
        d->incrementRegisters(args[1]);

        SValuePtr bit = ops->extract(ops->shiftRight(ops->and_(mask, a1), bitNumber), 0, 1);
        ops->writeRegister(d->REG_CCR_Z, ops->invert(bit)); // set if bit was originally clear
    }
};

struct IP_btst: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[1]->get_nBits()==8 || args[1]->get_nBits()==32);
        size_t nBits = args[1]->get_nBits();
        size_t logNBits = nBits==8 ? 3 : 5;
        d->decrementRegisters(args[1]);
        SValuePtr a0 = d->read(args[0], args[0]->get_nBits());
        SValuePtr a1 = d->read(args[1], nBits);
        SValuePtr bitNumber = ops->unsignedExtend(a0, logNBits); // bit number modulo destination size
        SValuePtr mask = ops->shiftLeft(ops->number_(nBits, 1), bitNumber);
        SValuePtr bit = ops->extract(ops->shiftRight(ops->and_(mask, a1), bitNumber), 0, 1);
        ops->writeRegister(d->REG_CCR_Z, ops->invert(bit)); // set if bit was originally clear
        d->incrementRegisters(args[1]);
    }
};

struct IP_callm: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_cas: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        ASSERT_require(args[0]->get_nBits() == args[1]->get_nBits());
        ASSERT_require(args[0]->get_nBits() == args[2]->get_nBits());
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[2]);
        SValuePtr dc = d->read(args[0], nBits);         // "compare" operand
        SValuePtr du = d->read(args[1], nBits);         // "update" operand
        SValuePtr ea = d->read(args[2], nBits);         // "address" operand
        SValuePtr isEqual = ops->equalToZero(ops->xor_(dc, ea));
        dc = ops->ite(isEqual, dc, ea);
        ea = ops->ite(isEqual, du, ea);
        SValuePtr result = ops->ite(isEqual, du, ea);   // not sure if this is what is meant by "result" in the documentation
        d->write(args[0], dc);
        d->write(args[2], ea);
        d->incrementRegisters(args[2]);

        SValuePtr sm = ops->extract(du, nBits-1, nBits);// documentation is not clear which operand is the "source"
        SValuePtr dm = ops->extract(ea, nBits-1, nBits);// documentation is not clear which operand is the "destination"
        SValuePtr rm = ops->extract(result, nBits-1, nBits);
        SValuePtr v1 = ops->and_(sm, rm);
        SValuePtr v2 = ops->or_(ops->and_(rm, ops->invert(dm)), v1);
        SValuePtr isCarry = ops->or_(ops->and_(sm, ops->invert(dm)), v2);
        SValuePtr v3 = ops->and_(sm, ops->and_(ops->invert(dm), rm));
        SValuePtr v4 = ops->and_(dm, ops->invert(rm));
        SValuePtr isOverflow = ops->or_(ops->and_(ops->invert(sm), v4), v3);
        
        ops->writeRegister(d->REG_CCR_C, isCarry);
        ops->writeRegister(d->REG_CCR_V, isOverflow);
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(result));
        ops->writeRegister(d->REG_CCR_N, ops->extract(result, nBits-1, nBits));
    }
};

struct IP_cas2: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 6);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_chk: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_chk2: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_clr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        d->write(args[0], ops->number_(nBits, 0));
        d->incrementRegisters(args[0]);

        ops->writeRegister(d->REG_CCR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_V, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_Z, ops->boolean_(true));
        ops->writeRegister(d->REG_CCR_N, ops->boolean_(false));
    }
};

struct IP_cmp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits()==args[1]->get_nBits());
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr a0 = d->read(args[0], nBits);
        SValuePtr a1 = d->read(args[1], nBits);
        d->incrementRegisters(args[0]);
        SValuePtr diff = ops->add(a1, ops->negate(a0));

        SValuePtr sm = ops->extract(a0, a0->nBits()-1, a0->nBits());
        SValuePtr dm = ops->extract(a1, a1->nBits()-1, a1->nBits());
        SValuePtr rm = ops->extract(diff, diff->nBits()-1, diff->nBits());

        SValuePtr isNegative = rm;
        SValuePtr isZero = ops->equalToZero(diff);
        SValuePtr v1 = ops->and_(sm, ops->and_(ops->invert(dm), rm));
        SValuePtr v2 = ops->and_(dm, ops->invert(rm));
        SValuePtr isOverflow = ops->or_(ops->and_(ops->invert(sm), v2), v1);
        SValuePtr v3 = ops->and_(sm, rm);
        SValuePtr v4 = ops->or_(ops->and_(rm, ops->invert(dm)), v3);
        SValuePtr isCarry = ops->or_(ops->and_(sm, ops->invert(dm)), v4);

        ops->writeRegister(d->REG_CCR_N, isNegative);
        ops->writeRegister(d->REG_CCR_Z, isZero);
        ops->writeRegister(d->REG_CCR_V, isOverflow);
        ops->writeRegister(d->REG_CCR_C, isCarry);
    }
};

struct IP_cmpa: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits() <= 32);
        ASSERT_require(args[1]->get_nBits() == 32);
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr a0 = ops->signExtend(d->read(args[0], nBits), 32);
        SValuePtr a1 = d->read(args[1], 32);
        d->incrementRegisters(args[0]);
        SValuePtr diff = ops->add(a1, ops->negate(a0));

        SValuePtr sm = ops->extract(a0, a0->nBits()-1, a0->nBits());
        SValuePtr dm = ops->extract(a1, a1->nBits()-1, a1->nBits());
        SValuePtr rm = ops->extract(diff, diff->nBits()-1, diff->nBits());

        SValuePtr isNegative = rm;
        SValuePtr isZero = ops->equalToZero(diff);
        SValuePtr v1 = ops->and_(sm, ops->and_(ops->invert(dm), rm));
        SValuePtr v2 = ops->and_(dm, ops->invert(rm));
        SValuePtr isOverflow = ops->or_(ops->and_(ops->invert(sm), v2), v1);
        SValuePtr v3 = ops->and_(sm, rm);
        SValuePtr v4 = ops->or_(ops->and_(rm, ops->invert(dm)), v3);
        SValuePtr isCarry = ops->or_(ops->and_(sm, ops->invert(dm)), v4);

        ops->writeRegister(d->REG_CCR_N, isNegative);
        ops->writeRegister(d->REG_CCR_Z, isZero);
        ops->writeRegister(d->REG_CCR_V, isOverflow);
        ops->writeRegister(d->REG_CCR_C, isCarry);
        assert_args(insn, args, 2);
    }
};

struct IP_cmpi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits()==args[1]->get_nBits());
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr a0 = d->read(args[0], nBits);
        SValuePtr a1 = d->read(args[1], nBits);
        d->incrementRegisters(args[0]);
        SValuePtr diff = ops->add(a1, ops->negate(a0));

        SValuePtr sm = ops->extract(a0, a0->nBits()-1, a0->nBits());
        SValuePtr dm = ops->extract(a1, a1->nBits()-1, a1->nBits());
        SValuePtr rm = ops->extract(diff, diff->nBits()-1, diff->nBits());

        SValuePtr isNegative = rm;
        SValuePtr isZero = ops->equalToZero(diff);
        SValuePtr v1 = ops->and_(sm, ops->and_(ops->invert(dm), rm));
        SValuePtr v2 = ops->and_(dm, ops->invert(rm));
        SValuePtr isOverflow = ops->or_(ops->and_(ops->invert(sm), v2), v1);
        SValuePtr v3 = ops->and_(sm, rm);
        SValuePtr v4 = ops->or_(ops->and_(rm, ops->invert(dm)), v3);
        SValuePtr isCarry = ops->or_(ops->and_(sm, ops->invert(dm)), v4);

        ops->writeRegister(d->REG_CCR_N, isNegative);
        ops->writeRegister(d->REG_CCR_Z, isZero);
        ops->writeRegister(d->REG_CCR_V, isOverflow);
        ops->writeRegister(d->REG_CCR_C, isCarry);
    }
};

struct IP_cmpm: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits()==args[1]->get_nBits());
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr a0 = d->read(args[0], nBits);
        SValuePtr a1 = d->read(args[1], nBits);
        d->incrementRegisters(args[0]);
        SValuePtr diff = ops->add(a1, ops->negate(a0));

        SValuePtr sm = ops->extract(a0, a0->nBits()-1, a0->nBits());
        SValuePtr dm = ops->extract(a1, a1->nBits()-1, a1->nBits());
        SValuePtr rm = ops->extract(diff, diff->nBits()-1, diff->nBits());

        SValuePtr isNegative = rm;
        SValuePtr isZero = ops->equalToZero(diff);
        SValuePtr v1 = ops->and_(sm, ops->and_(ops->invert(dm), rm));
        SValuePtr v2 = ops->and_(dm, ops->invert(rm));
        SValuePtr isOverflow = ops->or_(ops->and_(ops->invert(sm), v2), v1);
        SValuePtr v3 = ops->and_(sm, rm);
        SValuePtr v4 = ops->or_(ops->and_(rm, ops->invert(dm)), v3);
        SValuePtr isCarry = ops->or_(ops->and_(sm, ops->invert(dm)), v4);

        ops->writeRegister(d->REG_CCR_N, isNegative);
        ops->writeRegister(d->REG_CCR_Z, isZero);
        ops->writeRegister(d->REG_CCR_V, isOverflow);
        ops->writeRegister(d->REG_CCR_C, isCarry);
    }
};

struct IP_cmp2: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_cpushl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_cpushp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_cpusha: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbf: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbhi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbls: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbcc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbcs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbne: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbeq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbvc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbvs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbpl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbmi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbge: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dblt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbgt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dble: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_divs: P {
    void p(D d, Ops ops, I insn, A args) {
        SValuePtr quotient;
        if (2==args.size()) {
            assert_args(insn, args, 2);
            if (args[0]->get_nBits() == 16) {
                // DIVS.W <ea>, Dn
                // The word form divides a 32-bit argument (args[1]) by a 16-bit argument (args[0]) to get a 16-bit quotient
                // and a 16-bit remainder. The quotient is stored in the lower 16 bits of the result (args[1]) and the
                // remainder is stored in the higher 16 bits.
                ASSERT_require(args[1]->get_nBits()==32);
                d->decrementRegisters(args[0]);
                SValuePtr divisor = d->read(args[0], 16);
                SValuePtr dividend = d->read(args[1], 32);
                quotient = ops->extract(ops->signedDivide(dividend, divisor), 0, 16);
                SValuePtr remainder = ops->signedModulo(dividend, divisor);
                d->write(args[1], ops->concat(quotient, remainder));
                d->incrementRegisters(args[0]);
            } else if (args[0]->get_nBits() == 32) {
                // DIVS.L <ea>, Dn
                // The "first long form" divides a 32-bit argument (args[1]) by a 32-bit argument (args[0]) to get a 32-bit
                // quotient which is stored in the destination (args[1]). The remainder is discarded.
                ASSERT_require(args[1]->get_nBits()==32);
                d->decrementRegisters(args[0]);
                SValuePtr divisor = d->read(args[0], 32);
                SValuePtr dividend = d->read(args[1], 32);
                quotient = ops->signedDivide(dividend, divisor);
                d->write(args[1], quotient);
                d->incrementRegisters(args[0]);
            }
        } else {
            // DIVS.L <ea>, Dr, Dq
            // The dividend is 64-bits Dr:Dq and both a quotient and remainder are stored
            assert_args(insn, args, 3);
            ASSERT_require(args[0]->get_nBits()==32);
            ASSERT_require(args[1]->get_nBits()==32);
            ASSERT_require(args[2]->get_nBits()==32);
            d->decrementRegisters(args[0]);
            SValuePtr divisor = d->read(args[0], 32);
            SValuePtr arg1 = d->read(args[1], 32);
            SValuePtr dividend = ops->concat(d->read(args[2], 32), arg1);
            quotient = ops->extract(ops->signedDivide(dividend, divisor), 0, 32);
            SValuePtr remainder = ops->signedModulo(dividend, divisor);
            d->write(args[1], remainder);
            d->write(args[2], quotient);
            d->incrementRegisters(args[0]);
        }

        // FIXME[Robb P. Matzke 2014-07-28]: documentation says set on "division overflow" but apparently doesn't define it.
        SValuePtr isOverflow = ops->undefined_(1);

        ops->writeRegister(d->REG_CCR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_V, isOverflow);
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(quotient));
        ops->writeRegister(d->REG_CCR_N, ops->extract(quotient, quotient->nBits()-1, quotient->nBits()));
    }
};

struct IP_divsl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        ASSERT_require(args[0]->get_nBits()==32);       // divisor (in)
        ASSERT_require(args[1]->get_nBits()==32);       // remainder (out)
        ASSERT_require(args[2]->get_nBits()==32);       // dividend (in); quotient (out)
        d->decrementRegisters(args[0]);
        SValuePtr divisor = d->read(args[0], 32);
        SValuePtr dividend = d->read(args[2], 32);
        SValuePtr quotient = ops->signedDivide(dividend, divisor);
        SValuePtr remainder = ops->signedModulo(dividend, divisor);
        d->write(args[2], quotient);
        d->write(args[1], remainder);
        d->incrementRegisters(args[0]);

        // FIXME[Robb P. Matzke 2014-07-28]: documentation says set on "division overflow" but apparently doesn't define it.
        SValuePtr isOverflow = ops->undefined_(1);

        ops->writeRegister(d->REG_CCR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_V, isOverflow);
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(quotient));
        ops->writeRegister(d->REG_CCR_N, ops->extract(quotient, quotient->nBits()-1, quotient->nBits()));
    }
};

struct IP_divu: P {
    void p(D d, Ops ops, I insn, A args) {
        SValuePtr quotient;
        if (2==args.size()) {
            assert_args(insn, args, 2);
            if (args[0]->get_nBits() == 16) {
                // DIVU.W <ea>, Dn
                // The word form divides a 32-bit argument (args[1]) by a 16-bit argument(args[0]) to get a 16-bit quotient and
                // a 16-bit remainder. The quotient is stored in the lower 16 bits of the result (args[1]) and the remainder is
                // stored in the higher 16 bits.
                ASSERT_require(args[1]->get_nBits()==32);
                d->decrementRegisters(args[0]);
                SValuePtr divisor = d->read(args[0], 16);
                SValuePtr dividend = d->read(args[1], 32);
                quotient = ops->extract(ops->unsignedDivide(dividend, divisor), 0, 16);
                SValuePtr remainder = ops->unsignedModulo(dividend, divisor);
                d->write(args[1], ops->concat(quotient, remainder));
                d->incrementRegisters(args[0]);
            } else if (args[0]->get_nBits() == 32) {
                // DIVU.L <ea>, Dn
                // The "first long form" divides a 32-bit argument (args[1]) by a 32-bit argument (args[0]) to get a 32-bit
                // quotient which is stored in the destination (args[1]). The remainder is discarded.
                ASSERT_require(args[1]->get_nBits()==32);
                d->decrementRegisters(args[0]);
                SValuePtr divisor = d->read(args[0], 32);
                SValuePtr dividend = d->read(args[1], 32);
                quotient = ops->unsignedDivide(dividend, divisor);
                d->write(args[1], quotient);
                d->incrementRegisters(args[0]);
            }
        } else {
            // DIVU.L <ea>, Dr, Dq
            // The dividend is 64-bits Dr:Dq and both a quotient and remainder are stored
            assert_args(insn, args, 3);
            ASSERT_require(args[0]->get_nBits()==32);
            ASSERT_require(args[1]->get_nBits()==32);
            ASSERT_require(args[2]->get_nBits()==32);
            d->decrementRegisters(args[0]);
            SValuePtr divisor = d->read(args[0], 32);
            SValuePtr arg1 = d->read(args[1], 32);
            SValuePtr dividend = ops->concat(d->read(args[2], 32), arg1);
            quotient = ops->extract(ops->unsignedDivide(dividend, divisor), 0, 32);
            SValuePtr remainder = ops->unsignedModulo(dividend, divisor);
            d->write(args[1], remainder);
            d->write(args[2], quotient);
            d->incrementRegisters(args[0]);
        }

        // FIXME[Robb P. Matzke 2014-07-28]: documentation says set on "division overflow" but apparently doesn't define it.
        SValuePtr isOverflow = ops->undefined_(1);

        ops->writeRegister(d->REG_CCR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_V, isOverflow);
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(quotient));
        ops->writeRegister(d->REG_CCR_N, ops->extract(quotient, quotient->nBits()-1, quotient->nBits()));
    }
};

struct IP_divul: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        ASSERT_require(args[0]->get_nBits()==32);       // divisor (in)
        ASSERT_require(args[1]->get_nBits()==32);       // remainder (out)
        ASSERT_require(args[2]->get_nBits()==32);       // dividend (in); quotient (out)
        d->decrementRegisters(args[0]);
        SValuePtr divisor = d->read(args[0], 32);
        SValuePtr dividend = d->read(args[2], 32);
        SValuePtr quotient = ops->unsignedDivide(dividend, divisor);
        SValuePtr remainder = ops->unsignedModulo(dividend, divisor);
        d->write(args[2], quotient);
        d->write(args[1], remainder);
        d->incrementRegisters(args[0]);

        // FIXME[Robb P. Matzke 2014-07-28]: documentation says set on "division overflow" but apparently doesn't define it.
        SValuePtr isOverflow = ops->undefined_(1);

        ops->writeRegister(d->REG_CCR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_V, isOverflow);
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(quotient));
        ops->writeRegister(d->REG_CCR_N, ops->extract(quotient, quotient->nBits()-1, quotient->nBits()));
    }
};

struct IP_eor: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits() == args[1]->get_nBits());
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[1]);
        SValuePtr arg1 = d->read(args[1], nBits);
        SValuePtr result = ops->xor_(d->read(args[0], nBits), arg1);
        d->write(args[1], result);
        d->incrementRegisters(args[1]);

        ops->writeRegister(d->REG_CCR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_V, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(result));
        ops->writeRegister(d->REG_CCR_N, ops->extract(result, nBits-1, nBits));
    }
};

struct IP_eori: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits() == args[1]->get_nBits());
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[1]);
        SValuePtr arg1 = d->read(args[1], nBits);
        SValuePtr result = ops->xor_(d->read(args[0], nBits), arg1);
        d->write(args[1], result);
        d->incrementRegisters(args[1]);

        ops->writeRegister(d->REG_CCR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_V, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(result));
        ops->writeRegister(d->REG_CCR_N, ops->extract(result, nBits-1, nBits));
    }
};

struct IP_exg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_ext: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t dstNBits = args[0]->get_nBits();
        size_t srcNBits = dstNBits / 2;
        SValuePtr a0 = ops->unsignedExtend(d->read(args[0], dstNBits), srcNBits);
        SValuePtr result = ops->signExtend(a0, dstNBits);
        d->write(args[0], result);
        
        ops->writeRegister(d->REG_CCR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_V, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(a0));         // result==0 iff a0==0
        ops->writeRegister(d->REG_CCR_N, ops->extract(a0, 7, 8));       // result<0  iff a0<0
    }
};

struct IP_extb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ASSERT_require(args[0]->get_nBits() >= 8);
        size_t dstNBits = args[0]->get_nBits();
        size_t srcNBits = dstNBits / 2;
        SValuePtr a0 = ops->unsignedExtend(d->read(args[0], dstNBits), srcNBits);
        SValuePtr result = ops->signExtend(a0, dstNBits);
        d->write(args[0], result);

        ops->writeRegister(d->REG_CCR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_V, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(a0));         // result==0 iff a0==0
        ops->writeRegister(d->REG_CCR_N, ops->extract(a0, 7, 8));       // result<0  iff a0<0
    }
};

struct IP_fabs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fsabs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fdabs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fadd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

#if M68K
struct IP_fp_add: P {
    CilInstructionKind kind;
    IP_fp_add(CilInstructionKind kind): kind(kind) {
        ASSERT_require(Cil_fdadd == kind || Cil_fsadd == kind);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(isSgAsmDirectRegisterExpression(args[1]));

        // The actual operation
        SgAsmFloatType *opType = isSgAsmFloatType(args[1]->get_type());
        ASSERT_not_null(opType);
        SValuePtr a = ops->convert(d->read(args[0], args[0]->get_nBits()), args[0]->get_type(), opType);
        SValuePtr b = ops->convert(d->read(args[1], args[1]->get_nBits()), args[1]->get_type(), opType);
        SValuePtr result = ops->fpAdd(b, a, opType);

        // Rounding
        SgAsmFloatType *roundingType = NULL;
        if (Cil_fssub == kind) {
            roundingType = SageBuilderAsm::buildIeee754Binary32();
        } else {
            roundingType = opType;
        }
        SValuePtr rounded = ops->convert(result, opType, roundingType);

        // Save result
        SValuePtr dst = ops->convert(rounded, roundingType, args[1]->get_type());
        if (kind != Cil_fcmp)
            d->write(args[1], dst);

        // Update status flags
        ops->writeRegister(d->REG_EXC_BSUN,  ops->boolean_(false));
        d->updateFpsrExcInan(a, opType, b, opType);
        d->updateFpsrExcIde(a, opType, b, opType);
        d->updateFpsrExcOvfl(result, opType, roundingType, args[1]->get_type());
        d->updateFpsrExcUnfl(result, opType, roundingType, args[1]->get_type());
        ops->writeRegister(d->REG_EXC_DZ, ops->boolean_(false));
        d->updateFpsrExcInex();

        SValuePtr aInf = ops->fpIsInfinity(a, opType);
        SValuePtr bInf = ops->fpIsInfinity(b, opType);
        SValuePtr aSign = ops->fpSign(a, opType);
        SValuePtr bSign = ops->fpSign(b, opType);
        SValuePtr bothInfinite = ops->and_(aInf, bInf);
        SValuePtr opositeSign = ops->xor_(aSign, bSign);
        ops->writeRegister(d->REG_EXC_OPERR, ops->and_(bothInfinite, opositeSign));

        d->adjustFpConditionCodes(dst, isSgAsmFloatType(args[1]->get_type()));
        d->accumulateFpExceptions();
    }
};
#endif

struct IP_fbeq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr target = d->read(args[0], args[0]->get_nBits());
        SValuePtr noChange = ops->readRegister(d->REG_PC);
        SValuePtr newPc = 
            ops->ite(ops->readRegister(d->REG_FPCC_Z),
                     target, noChange);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_fbne: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr target = d->read(args[0], args[0]->get_nBits());
        SValuePtr noChange = ops->readRegister(d->REG_PC);
        SValuePtr newPc = 
            ops->ite(ops->readRegister(d->REG_FPCC_Z),
                     noChange, target);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_fbgt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr target = d->read(args[0], args[0]->get_nBits());
        SValuePtr noChange = ops->readRegister(d->REG_PC);
        SValuePtr fpcc_n = ops->readRegister(d->REG_FPCC_N);
        SValuePtr v1 = ops->or_(ops->readRegister(d->REG_FPCC_Z), fpcc_n);
        SValuePtr newPc = ops->ite(ops->or_(ops->readRegister(d->REG_FPCC_NAN), v1),
                                   noChange, target);
        ops->writeRegister(d->REG_PC, newPc);
        ops->writeRegister(d->REG_EXC_BSUN, ops->readRegister(d->REG_FPCC_NAN));
        d->accumulateFpExceptions();
    }
};

struct IP_fbngt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr target = d->read(args[0], args[0]->get_nBits());
        SValuePtr noChange = ops->readRegister(d->REG_PC);
        SValuePtr fpcc_n = ops->readRegister(d->REG_FPCC_N);
        SValuePtr v1 = ops->or_(ops->readRegister(d->REG_FPCC_Z), fpcc_n);
        SValuePtr newPc = ops->ite(ops->or_(ops->readRegister(d->REG_FPCC_NAN), v1),
                                   target, noChange);
        ops->writeRegister(d->REG_PC, newPc);
        ops->writeRegister(d->REG_EXC_BSUN, ops->readRegister(d->REG_FPCC_NAN));
        d->accumulateFpExceptions();
    }
};

struct IP_fbge: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr target = d->read(args[0], args[0]->get_nBits());
        SValuePtr noChange = ops->readRegister(d->REG_PC);
        SValuePtr fpcc_n = ops->readRegister(d->REG_FPCC_N);
        SValuePtr v1 = ops->invert(ops->or_(ops->readRegister(d->REG_FPCC_NAN), fpcc_n));
        SValuePtr newPc = ops->ite(ops->or_(ops->readRegister(d->REG_FPCC_Z), v1),
                                   target, noChange);
        ops->writeRegister(d->REG_PC, newPc);
        ops->writeRegister(d->REG_EXC_BSUN, ops->readRegister(d->REG_FPCC_NAN));
        d->accumulateFpExceptions();
    }
};

struct IP_fbnge: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr target = d->read(args[0], args[0]->get_nBits());
        SValuePtr noChange = ops->readRegister(d->REG_PC);
        SValuePtr not_fpcc_z = ops->invert(ops->readRegister(d->REG_FPCC_Z));
        SValuePtr v1 = ops->and_(ops->readRegister(d->REG_FPCC_N), not_fpcc_z);
        SValuePtr newPc = ops->ite(ops->or_(ops->readRegister(d->REG_FPCC_NAN), v1),
                                   target, noChange);
        ops->writeRegister(d->REG_PC, newPc);
        ops->writeRegister(d->REG_EXC_BSUN, ops->readRegister(d->REG_FPCC_NAN));
        d->accumulateFpExceptions();
    }
};

struct IP_fblt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr target = d->read(args[0], args[0]->get_nBits());
        SValuePtr noChange = ops->readRegister(d->REG_PC);
        SValuePtr fpcc_z = ops->readRegister(d->REG_FPCC_Z);
        SValuePtr v1 = ops->invert(ops->or_(ops->readRegister(d->REG_FPCC_NAN), fpcc_z));
        SValuePtr newPc = ops->ite(ops->and_(ops->readRegister(d->REG_FPCC_N), v1),
                                   target, noChange);
        ops->writeRegister(d->REG_PC, newPc);
        ops->writeRegister(d->REG_EXC_BSUN, ops->readRegister(d->REG_FPCC_NAN));
        d->accumulateFpExceptions();
    }
};

struct IP_fbnlt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr target = d->read(args[0], args[0]->get_nBits());
        SValuePtr noChange = ops->readRegister(d->REG_PC);
        SValuePtr not_fpcc_n = ops->invert(ops->readRegister(d->REG_FPCC_N));
        SValuePtr v1 = ops->or_(ops->readRegister(d->REG_FPCC_Z), not_fpcc_n);
        SValuePtr newPc = ops->ite(ops->or_(ops->readRegister(d->REG_FPCC_NAN), v1),
                                   target, noChange);
        ops->writeRegister(d->REG_PC, newPc);
        ops->writeRegister(d->REG_EXC_BSUN, ops->readRegister(d->REG_FPCC_NAN));
        d->accumulateFpExceptions();
    }
};

struct IP_fble: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr target = d->read(args[0], args[0]->get_nBits());
        SValuePtr noChange = ops->readRegister(d->REG_PC);
        SValuePtr notNan = ops->invert(ops->readRegister(d->REG_FPCC_NAN));
        SValuePtr v1 = ops->and_(ops->readRegister(d->REG_FPCC_N), notNan);
        SValuePtr newPc = ops->ite(ops->or_(ops->readRegister(d->REG_FPCC_Z), v1),
                                   target, noChange);
        ops->writeRegister(d->REG_PC, newPc);
        ops->writeRegister(d->REG_EXC_BSUN, ops->readRegister(d->REG_FPCC_NAN));
        d->accumulateFpExceptions();
    }
};

struct IP_fbnle: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr target = d->read(args[0], args[0]->get_nBits());
        SValuePtr noChange = ops->readRegister(d->REG_PC);
        SValuePtr fpcc_z = ops->readRegister(d->REG_FPCC_Z);
        SValuePtr v1 = ops->invert(ops->or_(ops->readRegister(d->REG_FPCC_N), fpcc_z));
        SValuePtr newPc = ops->ite(ops->or_(ops->readRegister(d->REG_FPCC_NAN), v1),
                                   target, noChange);
        ops->writeRegister(d->REG_PC, newPc);
        ops->writeRegister(d->REG_EXC_BSUN, ops->readRegister(d->REG_FPCC_NAN));
        d->accumulateFpExceptions();
    }
};

struct IP_fbgl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr target = d->read(args[0], args[0]->get_nBits());
        SValuePtr noChange = ops->readRegister(d->REG_PC);
        SValuePtr fpcc_z = ops->readRegister(d->REG_FPCC_Z);
        SValuePtr newPc = ops->ite(ops->or_(ops->readRegister(d->REG_FPCC_NAN), fpcc_z),
                                   noChange, target);
        ops->writeRegister(d->REG_PC, newPc);
        ops->writeRegister(d->REG_EXC_BSUN, ops->readRegister(d->REG_FPCC_NAN));
        d->accumulateFpExceptions();
    }
};

struct IP_fbngl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr target = d->read(args[0], args[0]->get_nBits());
        SValuePtr noChange = ops->readRegister(d->REG_PC);
        SValuePtr fpcc_z = ops->readRegister(d->REG_FPCC_Z);
        SValuePtr newPc = ops->ite(ops->or_(ops->readRegister(d->REG_FPCC_NAN), fpcc_z),
                                   target, noChange);
        ops->writeRegister(d->REG_PC, newPc);
        ops->writeRegister(d->REG_EXC_BSUN, ops->readRegister(d->REG_FPCC_NAN));
        d->accumulateFpExceptions();
    }
};

struct IP_fbgle: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr target = d->read(args[0], args[0]->get_nBits());
        SValuePtr noChange = ops->readRegister(d->REG_PC);
        SValuePtr newPc = ops->ite(ops->readRegister(d->REG_FPCC_NAN),
                                   noChange, target);
        ops->writeRegister(d->REG_PC, newPc);
        ops->writeRegister(d->REG_EXC_BSUN, ops->readRegister(d->REG_FPCC_NAN));
        d->accumulateFpExceptions();
    }
};

struct IP_fbngle: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr target = d->read(args[0], args[0]->get_nBits());
        SValuePtr noChange = ops->readRegister(d->REG_PC);
        SValuePtr newPc = ops->ite(ops->readRegister(d->REG_FPCC_NAN),
                                   target, noChange);
        ops->writeRegister(d->REG_PC, newPc);
        ops->writeRegister(d->REG_EXC_BSUN, ops->readRegister(d->REG_FPCC_NAN));
        d->accumulateFpExceptions();
    }
};

struct IP_fbogt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr target = d->read(args[0], args[0]->get_nBits());
        SValuePtr noChange = ops->readRegister(d->REG_PC);
        SValuePtr fpcc_n = ops->readRegister(d->REG_FPCC_N);
        SValuePtr v1 = ops->or_(ops->readRegister(d->REG_FPCC_Z), fpcc_n);
        SValuePtr newPc = ops->ite(ops->or_(ops->readRegister(d->REG_FPCC_NAN), v1),
                                   noChange, target);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_fbule: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr target = d->read(args[0], args[0]->get_nBits());
        SValuePtr noChange = ops->readRegister(d->REG_PC);
        SValuePtr fpcc_n = ops->readRegister(d->REG_FPCC_N);
        SValuePtr v1 = ops->or_(ops->readRegister(d->REG_FPCC_Z), fpcc_n);
        SValuePtr newPc = ops->ite(ops->or_(ops->readRegister(d->REG_FPCC_NAN), v1),
                                   target, noChange);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_fboge: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr target = d->read(args[0], args[0]->get_nBits());
        SValuePtr noChange = ops->readRegister(d->REG_PC);
        SValuePtr fpcc_n = ops->readRegister(d->REG_FPCC_N);
        SValuePtr v1 = ops->invert(ops->or_(ops->readRegister(d->REG_FPCC_NAN), fpcc_n));
        SValuePtr newPc = ops->ite(ops->or_(ops->readRegister(d->REG_FPCC_Z), v1),
                                   target, noChange);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_fbult: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr target = d->read(args[0], args[0]->get_nBits());
        SValuePtr noChange = ops->readRegister(d->REG_PC);
        SValuePtr not_fpcc_z = ops->invert(ops->readRegister(d->REG_FPCC_Z));
        SValuePtr v1 = ops->and_(ops->readRegister(d->REG_FPCC_N), not_fpcc_z);
        SValuePtr newPc = ops->ite(ops->or_(ops->readRegister(d->REG_FPCC_NAN), v1),
                                   target, noChange);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_fbolt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr target = d->read(args[0], args[0]->get_nBits());
        SValuePtr noChange = ops->readRegister(d->REG_PC);
        SValuePtr fpcc_z = ops->readRegister(d->REG_FPCC_Z);
        SValuePtr v1 = ops->invert(ops->or_(ops->readRegister(d->REG_FPCC_NAN), fpcc_z));
        SValuePtr newPc = ops->ite(ops->and_(ops->readRegister(d->REG_FPCC_N), v1),
                                   target, noChange);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_fbuge: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr target = d->read(args[0], args[0]->get_nBits());
        SValuePtr noChange = ops->readRegister(d->REG_PC);
        SValuePtr not_fpcc_n = ops->invert(ops->readRegister(d->REG_FPCC_N));
        SValuePtr v1 = ops->or_(ops->readRegister(d->REG_FPCC_Z), not_fpcc_n);
        SValuePtr newPc = ops->ite(ops->or_(ops->readRegister(d->REG_FPCC_NAN), v1),
                                   target, noChange);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_fbole: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr target = d->read(args[0], args[0]->get_nBits());
        SValuePtr noChange = ops->readRegister(d->REG_PC);
        SValuePtr notNan = ops->invert(ops->readRegister(d->REG_FPCC_NAN));
        SValuePtr v1 = ops->and_(ops->readRegister(d->REG_FPCC_N), notNan);
        SValuePtr newPc = ops->ite(ops->or_(ops->readRegister(d->REG_FPCC_Z), v1),
                                   target, noChange);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_fbugt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr target = d->read(args[0], args[0]->get_nBits());
        SValuePtr noChange = ops->readRegister(d->REG_PC);
        SValuePtr fpcc_z = ops->readRegister(d->REG_FPCC_Z);
        SValuePtr v1 = ops->invert(ops->or_(ops->readRegister(d->REG_FPCC_N), fpcc_z));
        SValuePtr newPc = ops->ite(ops->or_(ops->readRegister(d->REG_FPCC_NAN), v1),
                                   target, noChange);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_fbogl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr target = d->read(args[0], args[0]->get_nBits());
        SValuePtr noChange = ops->readRegister(d->REG_PC);
        SValuePtr fpcc_z = ops->readRegister(d->REG_FPCC_Z);
        SValuePtr newPc = ops->ite(ops->or_(ops->readRegister(d->REG_FPCC_NAN), fpcc_z),
                                   noChange, target);
        ops->writeRegister(d->REG_PC, newPc);
    }
};

struct IP_fbueq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbor: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbun: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbf: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbsf: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbst: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbseq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbsne: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fdiv: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fsdiv: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fddiv: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fint: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fintrz: P {
    void p(D d, Ops ops, I insn, A args) {
        SgAsmExpression *srcArg = args[0];
        SgAsmExpression *dstArg = NULL;
        if (args.size() == 2) {
            assert_args(insn, args, 2);
            dstArg = args[1];
        } else {
            assert_args(insn, args, 1);
            dstArg = args[0];
        }

        // The actual operation
        SgAsmFloatType *opType = isSgAsmFloatType(dstArg->get_type());
        ASSERT_not_null(opType);
        SValuePtr a = ops->convert(d->read(srcArg, srcArg->get_nBits()), srcArg->get_type(), opType);
        SValuePtr result = ops->fpRoundTowardZero(a, opType);

        // Save result
        d->write(dstArg, result);

        // Update status flags
        ops->writeRegister(d->REG_EXC_BSUN, ops->boolean_(false));
        ops->writeRegister(d->REG_EXC_INAN, ops->fpIsNan(result, opType));
        ops->writeRegister(d->REG_EXC_IDE, ops->fpIsDenormalized(result, opType));
        ops->writeRegister(d->REG_EXC_OPERR, ops->boolean_(false));
        ops->writeRegister(d->REG_EXC_OVFL, ops->boolean_(false));
        ops->writeRegister(d->REG_EXC_UNFL, ops->boolean_(false));
        ops->writeRegister(d->REG_EXC_DZ, ops->boolean_(false));
        d->updateFpsrExcInex();
        
        d->adjustFpConditionCodes(result, opType);
        d->accumulateFpExceptions();
    }
};

#if M68K
struct IP_fp_move: P {
    CilInstructionKind kind;
    IP_fp_move(CilInstructionKind kind): kind(kind) {
        ASSERT_require(Cil_fmove == kind || Cil_fdmove == kind || Cil_fsmove == kind);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_not_null(args[0]->get_type());
        ASSERT_not_null(args[1]->get_type());

        SValuePtr src = d->read(args[0], args[0]->get_nBits());

        // The "format" type from the CIL documentation. For register-to-memory or memory-to-register instructions this is
        // the type of the memory operand. For register-to-register instructions this is either the "D" or "S" type.
        SgAsmType *format = NULL;
        if (isSgAsmMemoryReferenceExpression(args[0])) {
            format = args[0]->get_type();
        } else if (isSgAsmMemoryReferenceExpression(args[1])) {
            format = args[1]->get_type();
        } else if (Cil_fsmove == kind) {
            format = SageBuilderAsm::buildIeee754Binary32();
        } else {
            format = args[1]->get_type();
        }

        // Convert the source type using the format a.k.a., "rounding mode" of the instruction
        SValuePtr intermediate = ops->convert(src, args[0]->get_type(), format);

        // Convert the intermediate value to the destination type and save it
        SValuePtr dst = ops->convert(intermediate, format, args[1]->get_type());
        d->write(args[1], dst);

        // Update status flags
        ops->writeRegister(d->REG_EXC_BSUN, ops->boolean_(false));
        if (SgAsmFloatType *formatFp = isSgAsmFloatType(format)) {
            ops->writeRegister(d->REG_EXC_INAN, ops->fpIsNan(intermediate, formatFp));
            ops->writeRegister(d->REG_EXC_OPERR, ops->boolean_(false));
        } else {
            ops->writeRegister(d->REG_EXC_INAN, ops->boolean_(false));
            // FIXME[Robb Matzke 2020-01-03]: Unclear documentation says "Set if source operand is x or if destination size
            // is exceeded after conversion and rounding; cleared otherwise."  What is "x" and by "size" do they mean the
            // width of the data, magnitude of the floating point value, or what?
            ops->writeRegister(d->REG_EXC_OPERR, ops->undefined_(1));
        }
        d->updateFpsrExcIde(src, args[0]->get_type(), dst, args[1]->get_type());
        d->updateFpsrExcOvfl(intermediate, format, format, args[1]->get_type());
        d->updateFpsrExcUnfl(intermediate, format, format, args[1]->get_type());
        ops->writeRegister(d->REG_EXC_DZ, ops->boolean_(false));
        d->updateFpsrExcInex();
        
        d->accumulateFpExceptions();
    }
};

struct IP_fmovem: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fmul: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fp_mul: P {
    CilInstructionKind kind;
    IP_fp_mul(CilInstructionKind kind): kind(kind) {
        ASSERT_require(Cil_fdmul == kind || Cil_fsmul == kind);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(isSgAsmDirectRegisterExpression(args[1]));

        // The actual operation
        SgAsmFloatType *opType = isSgAsmFloatType(args[1]->get_type());
        ASSERT_not_null(opType);
        SValuePtr a = ops->convert(d->read(args[0], args[0]->get_nBits()), args[0]->get_type(), opType);
        SValuePtr b = ops->convert(d->read(args[1], args[1]->get_nBits()), args[1]->get_type(), opType);
        SValuePtr result = ops->fpMultiply(b, a, opType);

        // Rounding
        SgAsmFloatType *roundingType = NULL;
        if (Cil_fssub == kind) {
            roundingType = SageBuilderAsm::buildIeee754Binary32();
        } else {
            roundingType = opType;
        }
        SValuePtr rounded = ops->convert(result, opType, roundingType);

        // Save result
        SValuePtr dst = ops->convert(rounded, roundingType, args[1]->get_type());
        d->write(args[1], dst);

        // Update status flags
        ops->writeRegister(d->REG_EXC_BSUN,  ops->boolean_(false));
        d->updateFpsrExcInan(a, opType, b, opType);
        d->updateFpsrExcIde(a, opType, b, opType);
        d->updateFpsrExcOvfl(result, opType, roundingType, args[1]->get_type());
        d->updateFpsrExcUnfl(result, opType, roundingType, args[1]->get_type());
        ops->writeRegister(d->REG_EXC_DZ, ops->boolean_(false));
        d->updateFpsrExcInex();

        // ColdFire documentation says "set for 0 x x; cleared otherwise" but I think they mean "set for zero times infinity or
        // infinity times zero; cleared otherwise". [Robb Matzke 2020-01-03]
        SValuePtr aIsZero = ops->fpIsZero(a, opType);
        SValuePtr aIsInf = ops->fpIsInfinity(a, opType);
        SValuePtr bIsZero = ops->fpIsZero(b, opType);
        SValuePtr bIsInf = ops->fpIsInfinity(b, opType);
        SValuePtr zeroInf = ops->and_(aIsZero, bIsInf);
        SValuePtr infZero = ops->and_(aIsInf, bIsZero);
        ops->writeRegister(d->REG_EXC_OPERR, ops->or_(zeroInf, infZero));

        d->adjustFpConditionCodes(dst, isSgAsmFloatType(args[1]->get_type()));
        d->accumulateFpExceptions();
    }
};

struct IP_fneg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fsneg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fdneg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fnop: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fsqrt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fssqrt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fdsqrt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fsub: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};
#endif

#if M68K
struct IP_fp_sub: P {
    CilInstructionKind kind;
    IP_fp_sub(CilInstructionKind kind): kind(kind) {
        ASSERT_require(Cil_fdsub == kind || Cil_fssub == kind || Cil_fcmp == kind);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(isSgAsmDirectRegisterExpression(args[1]));

        // The actual operation
        SgAsmFloatType *opType = isSgAsmFloatType(args[1]->get_type());
        ASSERT_not_null(opType);
        SValuePtr a = ops->convert(d->read(args[0], args[0]->get_nBits()), args[0]->get_type(), opType);
        SValuePtr b = ops->convert(d->read(args[1], args[1]->get_nBits()), args[1]->get_type(), opType);
        SValuePtr result = ops->fpSubtract(b, a, opType);

        // Rounding
        SgAsmFloatType *roundingType = NULL;
        if (Cil_fssub == kind) {
            roundingType = SageBuilderAsm::buildIeee754Binary32();
        } else {
            roundingType = opType;
        }
        SValuePtr rounded = ops->convert(result, opType, roundingType);

        // Save result
        SValuePtr dst = ops->convert(rounded, roundingType, args[1]->get_type());
        if (kind != Cil_fcmp)
            d->write(args[1], dst);

        // Update status flags
        ops->writeRegister(d->REG_EXC_BSUN,  ops->boolean_(false));
        d->updateFpsrExcInan(a, opType, b, opType);
        d->updateFpsrExcIde(a, opType, b, opType);
        d->updateFpsrExcOvfl(result, opType, roundingType, args[1]->get_type());
        d->updateFpsrExcUnfl(result, opType, roundingType, args[1]->get_type());
        ops->writeRegister(d->REG_EXC_DZ, ops->boolean_(false));
        d->updateFpsrExcInex();
        
        SValuePtr aInf = ops->fpIsInfinity(a, opType);
        SValuePtr bInf = ops->fpIsInfinity(b, opType);
        SValuePtr aSign = ops->fpSign(a, opType);
        SValuePtr bSign = ops->fpSign(b, opType);
        SValuePtr bothInfinite = ops->and_(aInf, bInf);
        SValuePtr sameSign = ops->invert(ops->xor_(aSign, bSign));
        ops->writeRegister(d->REG_EXC_OPERR, ops->and_(bothInfinite, sameSign));

        d->adjustFpConditionCodes(dst, isSgAsmFloatType(args[1]->get_type()));
        d->accumulateFpExceptions();
    }
};
#endif

struct IP_ftst: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_illegal: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_jmp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SValuePtr addr = d->read(args[0], 32);
        ops->writeRegister(d->REG_PC, addr);
    }
};

struct IP_jsr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ASSERT_require(args[0]->get_nBits()==32);
        SValuePtr negFour = ops->number_(32, -4);
        SValuePtr newSp = ops->add(ops->readRegister(d->REG_A[7]), negFour);
        ops->writeRegister(d->REG_A[7], newSp);
        ops->writeRegister(d->REG_PC, d->read(args[0], 32));
        SValuePtr dstAddr = ops->number_(32, insn->get_address() + insn->get_size());
        ops->writeMemory(RegisterDescriptor(), newSp, dstAddr, ops->boolean_(true));
    }
};

struct IP_lea: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits()==32);
        ASSERT_require(args[0]->get_nBits()==args[1]->get_nBits());
        ASSERT_require(isSgAsmMemoryReferenceExpression(args[0]));
        d->decrementRegisters(args[0]);
        SValuePtr addr = d->read(isSgAsmMemoryReferenceExpression(args[0])->get_address(), 32);
        d->incrementRegisters(args[0]);
        d->write(args[1], addr);
    }
};

struct IP_link: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require2(args[0]->get_nBits()==32, "arg-0 is always 32 bits regardless of offset width");
        // push 32-bit args[0] onto the stack
        SValuePtr negFour = ops->number_(32, -4);
        SValuePtr newSp = ops->add(ops->readRegister(d->REG_A[7]), negFour);
        ops->writeRegister(d->REG_A[7], newSp);
        SValuePtr yes = ops->boolean_(true);
        ops->writeMemory(RegisterDescriptor(), newSp, d->read(args[0], 32), yes);
        // move stack pointer to args[0]
        d->write(args[0], newSp, 32);
        // increment stack pointer by args[1] (usually a negative number)
        newSp = ops->add(newSp, ops->signExtend(d->read(args[1], args[1]->get_nBits()), 32));
        ops->writeRegister(d->REG_A[7], newSp);
    }
};

struct IP_lsl: P {
    void p(D d, Ops ops, I insn, A args) {
        SgAsmExpression *dst = NULL;
        SValuePtr count;
        if (1==args.size()) {
            assert_args(insn, args, 1);
            ASSERT_require(args[0]->get_nBits()==16);
            count = ops->number_(8, 1);
            dst = args[0];
        } else {
            assert_args(insn, args, 2);
            count = d->read(args[0], args[0]->get_nBits());
            dst = args[1];
        }

        size_t nBits = dst->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr value = d->read(dst, nBits);
        SValuePtr result = ops->shiftLeft(value, count);
        d->write(dst, result);
        d->incrementRegisters(args[0]);

        // CCR_C bit is the final bit shifted off the left (or zero if the shift count is zero).
        SValuePtr isCarry = ops->extract(ops->shiftLeft(ops->concat(value, ops->number_(1, 0)), count), nBits, nBits+1);

        // CCR_X bit is like CCR_C except unmodified if the shift count is zero.
        SValuePtr ccr_x = ops->readRegister(d->REG_CCR_X);
        SValuePtr xBit = ops->ite(ops->equalToZero(count), ccr_x, isCarry);

        ops->writeRegister(d->REG_CCR_C, isCarry);
        ops->writeRegister(d->REG_CCR_V, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(result));
        ops->writeRegister(d->REG_CCR_N, ops->extract(result, nBits-1, nBits));
        ops->writeRegister(d->REG_CCR_X, xBit);
    }
};

struct IP_lsr: P {
    void p(D d, Ops ops, I insn, A args) {
        SgAsmExpression *dst = NULL;
        SValuePtr count;
        if (1==args.size()) {
            assert_args(insn, args, 1);
            ASSERT_require(args[0]->get_nBits()==16);
            count = ops->number_(8, 1);
            dst = args[0];
        } else {
            assert_args(insn, args, 2);
            count = d->read(args[0], args[0]->get_nBits());
            dst = args[1];
        }

        size_t nBits = dst->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr value = d->read(dst, nBits);
        SValuePtr result = ops->shiftRight(value, count);
        d->write(dst, result);
        d->incrementRegisters(args[0]);

        // CCR_C bit is the final bit shifted off the right (or zero if the shift count is zero).
        SValuePtr isCarry = ops->extract(ops->shiftRight(ops->concat(ops->number_(1, 0), value), count), 0, 1);

        // CCR_X bit is like CCR_C except unmodified if the shift count is zero.
        SValuePtr ccr_x = ops->readRegister(d->REG_CCR_X);
        SValuePtr xBit = ops->ite(ops->equalToZero(count), ccr_x, isCarry);

        ops->writeRegister(d->REG_CCR_C, isCarry);
        ops->writeRegister(d->REG_CCR_V, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(result));
        ops->writeRegister(d->REG_CCR_N, ops->extract(result, nBits-1, nBits));
        ops->writeRegister(d->REG_CCR_X, xBit);
    }
};

struct IP_mac: P {
    void p(D d, Ops ops, I insn, A args) {
        // The 7-argument version is "multiply accumulate with load" and the 4-argument version is just "multiply accumulate".
        // The first 4 arguments are the same in both versions, namely the two registers (args[0] and args[1]) whose product is
        // shifted according to the scale factor (args[2]) and added to the accumulator (args[3]).  The 7-argument version has
        // the additional parallel operation of moving the source operand (args[4]) to the destination (args[6]) after
        // optionally (args[5]) masking it by the MAC MASK register.
        if (7==args.size()) {
            assert_args(insn, args, 7);
        } else {
            assert_args(insn, args, 4);
        }

        ASSERT_require(args[0]->get_nBits()==16 || args[0]->get_nBits()==32);
        ASSERT_require(args[1]->get_nBits()==16 || args[1]->get_nBits()==32);
        ASSERT_require(args[0]->get_nBits() == args[1]->get_nBits());
        size_t nBits = args[0]->get_nBits();

        if (d->REG_MACSR_SU.isEmpty() || d->REG_MACSR_FI.isEmpty() || d->REG_MACSR_N.isEmpty() ||
            d->REG_MACSR_Z.isEmpty()  || d->REG_MACSR_V.isEmpty()  || d->REG_MACSR_C.isEmpty() ||
            d->REG_MAC_MASK.isEmpty() || d->REG_MACEXT0.isEmpty()  || d->REG_MACEXT1.isEmpty() ||
            d->REG_MACEXT2.isEmpty()  || d->REG_MACEXT3.isEmpty()) {
            throw BaseSemantics::Exception("MAC registers are not available for " +
                                           d->registerDictionary()->get_architecture_name(),
                                           insn);
        }
        
        // Produce the product
        SValuePtr ry = d->read(args[0], nBits);
        SValuePtr rx = d->read(args[1], nBits);
        SValuePtr product = ops->unsignedMultiply(ry, rx);

        // Shift the product left or right if necessary
        // FIXME[Robb P. Matzke 2014-08-14]: this could be simplified since args[2] is always a constant
        size_t sfNBits = args[2]->get_nBits();
        SValuePtr sf = d->read(args[2], sfNBits);
        SValuePtr isSf1 = ops->equalToZero(ops->add(sf, ops->number_(sfNBits, -1)));
        SValuePtr isSf3 = ops->equalToZero(ops->add(sf, ops->number_(sfNBits, -3)));
        SValuePtr v1 = ops->ite(isSf3, ops->shiftRight(product, ops->number_(8, 1)), product);
        product = ops->ite(isSf1, ops->shiftLeft(product, ops->number_(8, 1)), v1);
        ASSERT_require(product->nBits()==32 || product->nBits()==64);

        // MAC operational bits. I cannot find documentation that describes these bits adequately. In particular, does a 1 mean
        // signed or unsigned for the MACSR_SU bit; does a 1 mean fractional or integer mode for the MACSR_FI bit?
        SValuePtr isSigned = ops->readRegister(d->REG_MACSR_SU);
        SValuePtr isFrac = ops->readRegister(d->REG_MACSR_FI);
        
        // Load the accumulator
        SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(args[3]);
        ASSERT_not_null2(rre, "fourth operand must be a MAC accumulator register");
        RegisterDescriptor macAccReg = rre->get_descriptor();
        ASSERT_require2(macAccReg.majorNumber()==Cil_regclass_mac, "fourth operand must be a MAC accumulator register");
        ASSERT_require2(macAccReg.nBits()==32, "MAC accumulator register must be 32 bits");
        RegisterDescriptor macExtReg;
        switch (macAccReg.minorNumber()) {
            case Cil_mac_acc0: macExtReg = d->REG_MACEXT0; break;
            case Cil_mac_acc1: macExtReg = d->REG_MACEXT1; break;
            case Cil_mac_acc2: macExtReg = d->REG_MACEXT2; break;
            case Cil_mac_acc3: macExtReg = d->REG_MACEXT3; break;
            default:
#if 0
             // DQ (10/8/2021): This M68k specific code is not wanted here.
                ASSERT_not_reachable("invalid mac accumulator register: " +
                                     stringifyBinaryAnalysisCilMacRegister(macAccReg.minorNumber()));
#else
             // DQ (10/8/2021): Not clear what if any CIL support may be required here.
                ROSE_ASSERT(false);
#endif
        }
        SValuePtr macAcc = ops->readRegister(macAccReg);
        SValuePtr macExt = ops->readRegister(macExtReg);
        ASSERT_require(macAcc->nBits()==32);
        ASSERT_require(macExt->nBits()==16);

        // When the accumulator is operating in integer mode, the lower 40 bits of the shifted product is added to the
        // low-order 40 bits of the 48-bit (macExt|macAcc) concatenation.  The upper 8 bits of the concatenation are not
        // affected (at least according to the diagram on 1-10 of "ColdFire Family Programmer's Reference Manual, Rev 3").
        SValuePtr productInt = ops->unsignedExtend(product, 40);
        SValuePtr accInt = ops->concat(macAcc, macExt);
        SValuePtr v2 = ops->extract(accInt, 40, 48);
        SValuePtr newAccInt = ops->concat(ops->add(ops->unsignedExtend(accInt, 40), productInt), v2);
        ASSERT_require(newAccInt->nBits()==48);

        // When the accumulator is operating in fractional mode, the upper 40 bits of the shifted product is extended to 48
        // bits (signed or unsigned depending on the MACSR register), and then added to the 48 bit accumulator. The 48 bit
        // accumulator is composed of (macExtHi|macAcc|macExtLo).
        SValuePtr v3 = ops->unsignedExtend(ops->extract(ops->unsignedExtend(product, 64), 24, 64), 48);
        SValuePtr productFrac = ops->ite(isSigned,
                                         ops->signExtend(ops->extract(ops->unsignedExtend(product, 64), 24, 64), 48),
                                         v3);
        SValuePtr v4 = ops->concat(macAcc, ops->extract(macExt, 8, 16));
        SValuePtr accFrac = ops->concat(ops->extract(macExt, 0, 8), v4);
        SValuePtr newAccFrac = ops->add(accFrac, productFrac);
        ASSERT_require(newAccFrac->nBits()==48);

        // The new macAcc and macExt values
        SValuePtr v5 = ops->unsignedExtend(newAccInt, 32);
        SValuePtr newMacAcc = ops->ite(isFrac, ops->extract(newAccFrac, 8, 40), v5);
        SValuePtr v6 = ops->extract(newAccInt, 32, 48);
        SValuePtr v7 = ops->extract(newAccFrac, 40, 48);
        SValuePtr newMacExt = ops->ite(isFrac, ops->concat(ops->unsignedExtend(newAccFrac, 8), v7), v6);

        if (7==args.size()) {
            // In parallel with multiply-accumulate above, load the <ea> argument (args[4]), optionally mask it with the MASK
            // register depending on whether args[5] is true or false.  Only the low-order 16 bits of the MASK register are
            // used and the upper 16 are assumed to be all set.
            ASSERT_require(args[4]->get_nBits()==32);
            ASSERT_require(args[6]->get_nBits()==32);
            d->decrementRegisters(args[4]);
            SValuePtr toMove = d->read(args[4], 32);
            d->incrementRegisters(args[4]);
            SValuePtr v8 = ops->number_(16, 0xffff);
            SValuePtr v9 = ops->and_(toMove, ops->concat(ops->unsignedExtend(ops->readRegister(d->REG_MAC_MASK), 16), v8));
            toMove = ops->ite(ops->equalToZero(d->read(args[5], args[5]->get_nBits())),
                              toMove,                       // don't use the mask
                              v9);
            ASSERT_require(toMove->nBits()==32);
            d->write(args[6], toMove);
        }
        
        // Write MAC results (load result was written above)
        ops->writeRegister(macAccReg, newMacAcc);
        ops->writeRegister(macExtReg, newMacExt);

        // Update MACSR indicator flags based on the accumulator's new value.
        SValuePtr acc48 = ops->ite(isFrac, newAccFrac, newAccInt);
        ops->writeRegister(d->REG_MACSR_N, ops->extract(acc48, 47, 48));
        ops->writeRegister(d->REG_MACSR_Z, ops->equalToZero(acc48));
        ops->writeRegister(d->REG_MACSR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_MACSR_V, ops->boolean_(false));// FIXME[Robb P. Matzke 2014-07-25]: no documentation found
    }
};

struct IP_mov3q: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require2(args[0]->get_nBits()==32, "source should have already been sign extended to 32 bits");
        ASSERT_require(args[1]->get_nBits()==32);
        SValuePtr result = d->read(args[0], 32);
        d->decrementRegisters(args[1]);
        d->write(args[1], result);
        d->incrementRegisters(args[1]);

        ops->writeRegister(d->REG_CCR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_V, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(result));
        ops->writeRegister(d->REG_CCR_N, ops->extract(result, 31, 32));
    }
};

struct IP_movclr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_move: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits()==args[1]->get_nBits());
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        d->decrementRegisters(args[1]);
        SValuePtr result = d->read(args[0], nBits);
        d->write(args[1], result);
        d->incrementRegisters(args[0]);
        d->incrementRegisters(args[1]);

        ops->writeRegister(d->REG_CCR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_V, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(result));
        ops->writeRegister(d->REG_CCR_N, ops->extract(result, nBits-1, nBits));
    }
};

struct IP_move_acc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_move_accext: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_move_ccr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t srcNBits = args[0]->get_nBits();
        size_t dstNBits = args[1]->get_nBits();
        if (8==srcNBits && 16==dstNBits) {
            // MOVE.W CCR, <ea>
            d->decrementRegisters(args[1]);
            SValuePtr src = ops->unsignedExtend(d->read(args[0], 8), 16);
            d->write(args[1], src);
            d->incrementRegisters(args[1]);
        } else {
            // MOVE.B <ea>, CCR  (truncates <ea> to eight bits)
            ASSERT_require(8==dstNBits);
            d->decrementRegisters(args[0]);
            SValuePtr src = ops->unsignedExtend(d->read(args[0], srcNBits), 8);
            d->write(args[1], src);
            d->incrementRegisters(args[0]);
        }
    }
};

struct IP_move_macsr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_move_mask: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

// same as IP_move, but does not update CCR
struct IP_move_no_ccr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits()==args[1]->get_nBits());
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        d->decrementRegisters(args[1]);
        SValuePtr result = d->read(args[0], nBits);
        d->write(args[1], result);
        d->incrementRegisters(args[0]);
        d->incrementRegisters(args[1]);
    }
};

struct IP_move16: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_movea: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits() <= 32);
        ASSERT_require(args[1]->get_nBits() == 32);
        d->decrementRegisters(args[0]);
        d->write(args[1], ops->signExtend(d->read(args[0], args[0]->get_nBits()), 32));
        d->incrementRegisters(args[0]);
    }
};

struct IP_movem: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);

        // Argument order indicates whether this is register-to-memory or memory-to-register
        SgAsmRegisterNames *regList = NULL;
        SgAsmMemoryReferenceExpression *mre = NULL;
        if (isSgAsmRegisterNames(args[0])) {
            regList = isSgAsmRegisterNames(args[0]);
            mre = isSgAsmMemoryReferenceExpression(args[1]);
        } else {
            regList = isSgAsmRegisterNames(args[1]);
            mre = isSgAsmMemoryReferenceExpression(args[0]);
        }
        ASSERT_not_null2(regList, "one of the arguments must be a register list");
        ASSERT_not_null2(mre, "one of the arguments must be a memory reference expression");
        ASSERT_require2(regList->get_nBits() == 32, "only 32-bit registers are supported");
        bool isRegToMem = (regList == args[0]);
        size_t nBits = mre->get_nBits();                // data is truncated or sign extended as necessary
        ASSERT_require(0 == nBits % 8);
        int32_t nTransfers = regList->get_registers().size();
        int32_t bytesPerTransfer = nBits / 8;

        // Get the memory address.  The register-to-memory mode might use a pre-decrementing register, and the
        // memory-to-register mode might use a post-incrementing register.  In either case we need to control when the
        // decrement or increment happens and how often.
        SValuePtr firstAddr;                                 // first memory address accessed
        RegisterDescriptor autoAdjust;                  // register that needs to be adjusted
        if (SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(mre->get_address())) {
            firstAddr = ops->readRegister(rre->get_descriptor());
            if (rre->get_adjustment() < 0) {
                ASSERT_require2(isRegToMem, "auto decrement is only valud for register-to-memory transfers");
                autoAdjust = rre->get_descriptor();
            } else if (rre->get_adjustment() > 0) {
                ASSERT_require2(!isRegToMem, "auto increment is only valid for memory-to-register transfers");
                autoAdjust = rre->get_descriptor();
            }
        }
        if (firstAddr==NULL)
            firstAddr = d->effectiveAddress(mre, 32);

        if (isRegToMem) {                               // registers-to-memory operation
            if (!autoAdjust.isEmpty()) {
                // Copying registers to memory and decrementing the address each time. Registers are copied from A7-A0, D7-D0
                // so that D0 is at the lowest (ending) address.  For M68020, M68030, M68040, and CPU32 the address register is
                // decremented before writing it to memory.
                ops->writeRegister(autoAdjust, ops->subtract(firstAddr, ops->number_(32, nTransfers*bytesPerTransfer)));
                const SgAsmRegisterReferenceExpressionPtrList &regs = isSgAsmRegisterNames(regList)->get_registers();
                for (size_t i=0; i<regs.size(); ++i) {
                    SValuePtr value = ops->unsignedExtend(d->read(regs[regs.size()-(i+1)], 32), nBits);
                    SValuePtr addr = ops->subtract(firstAddr, ops->number_(32, (i+1)*bytesPerTransfer));
                    ops->writeMemory(RegisterDescriptor(), addr, value, ops->boolean_(true));
                }
            } else {
                // Copying registers to memory at increasing addresses without incrementing the address register. Registers are
                // copied from D0-D7, A0-A7 so that D0 is at the lowest (starting) address.
                const SgAsmRegisterReferenceExpressionPtrList &regs = isSgAsmRegisterNames(regList)->get_registers();
                for (size_t i=0; i<regs.size(); ++i) {
                    SValuePtr value = ops->unsignedExtend(d->read(regs[i], 32), nBits);
                    SValuePtr addr = ops->add(firstAddr, ops->number_(32, i*bytesPerTransfer));
                    ops->writeMemory(RegisterDescriptor(), addr, value, ops->boolean_(true));
                }
            }
        } else {                                        // memory-to-registers operation
            if (!autoAdjust.isEmpty()) {
                // Copying memory to registers and incrementing the address each time.  Registers are copied from D0-D7, A0-A7
                // since D0 was stored at the lowest (starting) address.  The auto-adjusted register is clobbered after being
                // read from memory.
                const SgAsmRegisterReferenceExpressionPtrList &regs = isSgAsmRegisterNames(regList)->get_registers();
                for (size_t i=0; i<regs.size(); ++i) {
                    SValuePtr addr = ops->add(firstAddr, ops->number_(32, i*bytesPerTransfer));
                    SValuePtr yes = ops->boolean_(true);
                    SValuePtr value = ops->signExtend(ops->readMemory(RegisterDescriptor(), addr, ops->undefined_(32), yes),
                                                      32);
                    d->write(regs[i], value);
                }
                ops->writeRegister(autoAdjust, ops->add(firstAddr, ops->number_(32, nTransfers*bytesPerTransfer)));
            } else {
                // Copying memory to registers in increasing addresses from D0-D7, A0-A7 since D0 was stored at the lowest
                // (starting) address. The address register is not auto-incremented.
                const SgAsmRegisterReferenceExpressionPtrList &regs = isSgAsmRegisterNames(regList)->get_registers();
                for (size_t i=0; i<regs.size(); ++i) {
                    SValuePtr addr = ops->add(firstAddr, ops->number_(32, i*bytesPerTransfer));
                    SValuePtr yes = ops->boolean_(true);
                    SValuePtr value = ops->signExtend(ops->readMemory(RegisterDescriptor(), addr, ops->undefined_(32), yes),
                                                      32);
                    d->write(regs[i], value);
                }
            }
        }
    }
};

struct IP_movep: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_moveq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require2(args[0]->get_nBits()==32, "should have been sign extended to 32 bits by the disassembler");
        ASSERT_require(args[1]->get_nBits()==32);
        SValuePtr result = d->read(args[0], 32);
        d->write(args[1], result);

        ops->writeRegister(d->REG_CCR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_V, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(result));
        ops->writeRegister(d->REG_CCR_N, ops->extract(result, 31, 32));
    }
};

struct IP_msac: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_muls: P {
    void p(D d, Ops ops, I insn, A args) {
        SValuePtr result;
        SValuePtr isOverflow;
        if (2==args.size()) {
            // a0[32] x a1[32] -> a1[32]
            assert_args(insn, args, 2);
            ASSERT_require(args[0]->get_nBits()==32);
            ASSERT_require(args[1]->get_nBits()==32);
            d->decrementRegisters(args[0]);
            SValuePtr a0 = d->read(args[0], 32);
            SValuePtr a1 = d->read(args[1], 32);
            SValuePtr tmp = ops->signedMultiply(a0, a1);
            result = ops->unsignedExtend(tmp, 32);
            d->write(args[1], result);
            d->incrementRegisters(args[0]);

            // Overflow occurs when the high-order 32 bits of the 64-bit result are not equal to the high-order 32 bits of the
            // sign-extended 32-bit result.
            SValuePtr v1 = ops->extract(ops->signExtend(result, 64), 32, 64);
            isOverflow = ops->invert(ops->equalToZero(ops->xor_(ops->extract(tmp, 32, 64), v1)));
        } else if (3==args.size() && 16==args[0]->get_nBits()) {
            // a0[16] x a1[16] -> a2[32]  (a1 and a2 are different parts of the same physical register)
            assert_args(insn, args, 3);
            ASSERT_require(args[0]->get_nBits()==16);
            ASSERT_require(args[1]->get_nBits()==16);
            ASSERT_require(args[2]->get_nBits()==32);
            d->decrementRegisters(args[0]);
            SValuePtr a0 = d->read(args[0], 16);
            SValuePtr a1 = d->read(args[1], 16);
            result = ops->signedMultiply(a0, a1);
            d->write(args[2], result);
            d->incrementRegisters(args[0]);
            isOverflow = ops->boolean_(false);
        } else {
            // a0[32] x a1[32] -> a2[32]:a1[32]  (a2 are the high-order 32 bits of the 64-bit result)
            assert_args(insn, args, 3);
            ASSERT_require(args[0]->get_nBits()==32);
            ASSERT_require(args[1]->get_nBits()==32);
            ASSERT_require(args[2]->get_nBits()==32);
            d->decrementRegisters(args[0]);
            SValuePtr a0 = d->read(args[0], 32);
            SValuePtr a1 = d->read(args[1], 32);
            result = ops->signedMultiply(a0, a1);
            d->write(args[1], ops->unsignedExtend(result, 32));
            d->write(args[2], ops->extract(result, 32, 64));
            d->incrementRegisters(args[0]);
            isOverflow = ops->boolean_(false);
        }

        size_t resultNBits = result->nBits();
        ops->writeRegister(d->REG_CCR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_V, isOverflow);
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(result));
        ops->writeRegister(d->REG_CCR_N, ops->extract(result, resultNBits-1, resultNBits));
    }
};

struct IP_mulu: P {
    void p(D d, Ops ops, I insn, A args) {
        SValuePtr result;
        SValuePtr isOverflow;
        if (2==args.size()) {
            // a0[32] x a1[32] -> a1[32]
            assert_args(insn, args, 2);
            ASSERT_require(args[0]->get_nBits()==32);
            ASSERT_require(args[1]->get_nBits()==32);
            d->decrementRegisters(args[0]);
            SValuePtr a0 = d->read(args[0], 32);
            SValuePtr a1 = d->read(args[1], 32);
            SValuePtr tmp = ops->unsignedMultiply(a0, a1);
            result = ops->unsignedExtend(tmp, 32);
            d->write(args[1], result);
            d->incrementRegisters(args[0]);
            isOverflow = ops->invert(ops->equalToZero(ops->extract(tmp, 32, 64)));
        } else if (3==args.size() && 16==args[0]->get_nBits()) {
            // a0[16] x a1[16] -> a2[32]  (a1 and a2 are different parts of the same physical register)
            assert_args(insn, args, 3);
            ASSERT_require(args[0]->get_nBits()==16);
            ASSERT_require(args[1]->get_nBits()==16);
            ASSERT_require(args[2]->get_nBits()==32);
            d->decrementRegisters(args[0]);
            SValuePtr a0 = d->read(args[0], 16);
            SValuePtr a1 = d->read(args[1], 16);
            result = ops->unsignedMultiply(a0, a1);
            d->write(args[2], result);
            d->incrementRegisters(args[0]);
            isOverflow = ops->boolean_(false);
        } else {
            // a0[32] x a1[32] -> a2[32]:a1[32]  (a2 are the high-order 32 bits of the 64-bit result)
            assert_args(insn, args, 3);
            ASSERT_require(args[0]->get_nBits()==32);
            ASSERT_require(args[1]->get_nBits()==32);
            ASSERT_require(args[2]->get_nBits()==32);
            d->decrementRegisters(args[0]);
            SValuePtr a0 = d->read(args[0], 32);
            SValuePtr a1 = d->read(args[1], 32);
            result = ops->unsignedMultiply(a0, a1);
            d->write(args[1], ops->unsignedExtend(result, 32));
            d->write(args[2], ops->extract(result, 32, 64));
            d->incrementRegisters(args[0]);
            isOverflow = ops->boolean_(false);
        }

        size_t resultNBits = result->nBits();
        ops->writeRegister(d->REG_CCR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_V, isOverflow);
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(result));
        ops->writeRegister(d->REG_CCR_N, ops->extract(result, resultNBits-1, resultNBits));
    }
};

struct IP_mvs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits() <= 32);
        ASSERT_require(args[1]->get_nBits() == 32);
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr a0 = d->read(args[0], nBits);
        d->incrementRegisters(args[0]);
        SValuePtr result = ops->signExtend(a0, 32);
        d->write(args[1], result);

        ops->writeRegister(d->REG_CCR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_V, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(a0)); // same as equalToZero(result), but probably simpler
        ops->writeRegister(d->REG_CCR_N, ops->extract(a0, nBits-1, nBits)); // a0 sign bit is same as result sign bit
    }
};

struct IP_mvz: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits() <= 32);
        ASSERT_require(args[1]->get_nBits() == 32);
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr a0 = d->read(args[0], nBits);
        d->incrementRegisters(args[0]);
        SValuePtr result = ops->unsignedExtend(a0, 32);
        d->write(args[1], result);

        ops->writeRegister(d->REG_CCR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_V, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(a0)); // same as equalToZero(result), but probably simpler
        ops->writeRegister(d->REG_CCR_N, ops->boolean_(false));
    }
};

struct IP_nbcd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_neg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr a0 = d->read(args[0], nBits);
        SValuePtr result = ops->negate(a0);
        d->write(args[0], result);
        d->incrementRegisters(args[0]);

        SValuePtr dm = ops->extract(a0, nBits-1, nBits);
        SValuePtr rm = ops->extract(result, nBits-1, nBits);
        SValuePtr isOverflow = ops->and_(dm, rm);
        SValuePtr isCarry = ops->or_(dm, rm);

        ops->writeRegister(d->REG_CCR_C, isCarry);
        ops->writeRegister(d->REG_CCR_V, isOverflow);
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(result));
        ops->writeRegister(d->REG_CCR_N, rm);
        ops->writeRegister(d->REG_CCR_X, isCarry);
    }
};

struct IP_negx: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr a0 = d->read(args[0], nBits);
        SValuePtr x = ops->readRegister(d->REG_CCR_X);
        SValuePtr result = ops->negate(ops->add(a0, ops->unsignedExtend(x, nBits)));
        d->write(args[0], result);
        d->incrementRegisters(args[0]);

        SValuePtr dm = ops->extract(a0, nBits-1, nBits);
        SValuePtr rm = ops->extract(result, nBits-1, nBits);
        SValuePtr isNegative = rm;
        SValuePtr isOverflow = ops->and_(dm, rm);
        SValuePtr isCarry = ops->or_(dm, rm);

        // Note that the Z bit behaves differently for this instruction than normal.  If the result is non-zero then the Z bit
        // is cleared, otherwise it is unchanged.
        SValuePtr no = ops->boolean_(false);
        SValuePtr ccr_z = ops->readRegister(d->REG_CCR_Z);
        SValuePtr isZero = ops->ite(ops->equalToZero(result), ccr_z, no);

        ops->writeRegister(d->REG_CCR_C, isCarry);
        ops->writeRegister(d->REG_CCR_V, isOverflow);
        ops->writeRegister(d->REG_CCR_Z, isZero);
        ops->writeRegister(d->REG_CCR_N, isNegative);
        ops->writeRegister(d->REG_CCR_X, isCarry);
    }
};

struct IP_nop: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
    }
};

struct IP_not: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr a0 = d->read(args[0], nBits);
        SValuePtr result = ops->invert(a0);
        d->write(args[0], result);
        d->incrementRegisters(args[0]);

        ops->writeRegister(d->REG_CCR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_V, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(result));
        ops->writeRegister(d->REG_CCR_N, ops->extract(result, nBits-1, nBits));
    }
};

struct IP_or: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits() == args[1]->get_nBits());
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[1]);
        SValuePtr a0 = d->read(args[0], nBits);
        SValuePtr a1 = d->read(args[1], nBits);
        SValuePtr result = ops->or_(a0, a1);
        d->write(args[1], result);
        d->incrementRegisters(args[1]);

        ops->writeRegister(d->REG_CCR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_V, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(result));
        ops->writeRegister(d->REG_CCR_N, ops->extract(result, nBits-1, nBits));
    }
};

struct IP_ori: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits() == args[1]->get_nBits());
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[1]);
        SValuePtr a0 = d->read(args[0], nBits);
        SValuePtr a1 = d->read(args[1], nBits);
        SValuePtr result = ops->or_(a0, a1);
        d->write(args[1], result);
        d->incrementRegisters(args[1]);

        ops->writeRegister(d->REG_CCR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_V, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(result));
        ops->writeRegister(d->REG_CCR_N, ops->extract(result, nBits-1, nBits));
    }
};

struct IP_pack: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_pea: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ASSERT_require(args[0]->get_nBits()==32);
        ASSERT_require(isSgAsmMemoryReferenceExpression(args[0]));
        SValuePtr negFour = ops->number_(32, -4);
        SValuePtr newSp = ops->add(ops->readRegister(d->REG_A[7]), negFour);
        ops->writeRegister(d->REG_A[7], newSp);
        d->decrementRegisters(args[0]);
        SValuePtr addr = d->read(isSgAsmMemoryReferenceExpression(args[0])->get_address(), 32);
        d->incrementRegisters(args[0]);
        ops->writeMemory(RegisterDescriptor(), newSp, addr, ops->boolean_(true));
    }
};

struct IP_rems: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_remu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_rol: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_ror: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_roxl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_roxr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_rtd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_rtm: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_rtr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_rts: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        SValuePtr sp = ops->readRegister(d->REG_A[7]);
        SValuePtr yes = ops->boolean_(true);
        SValuePtr newPc = ops->readMemory(RegisterDescriptor(), sp, ops->undefined_(32), yes);
        ops->writeRegister(d->REG_PC, newPc);
        ops->writeRegister(d->REG_A[7], ops->add(sp, ops->number_(32, 4)));
    }
};

struct IP_sbcd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_st: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        d->decrementRegisters(args[0]);
        d->write(args[0], ops->invert(ops->number_(args[0]->get_nBits(), 0)));
        d->incrementRegisters(args[0]);
    }
};

struct IP_sf: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        d->decrementRegisters(args[0]);
        d->write(args[0], ops->number_(args[0]->get_nBits(), 0));
        d->incrementRegisters(args[0]);
    }
};

struct IP_shi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr cond = d->condition(insn->get_kind(), ops);
        SValuePtr zero = ops->number_(nBits, 0);
        d->write(args[0], ops->ite(cond, ops->invert(zero), zero));
        d->incrementRegisters(args[0]);
    }
};

struct IP_sls: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr cond = d->condition(insn->get_kind(), ops);
        SValuePtr zero = ops->number_(nBits, 0);
        d->write(args[0], ops->ite(cond, ops->invert(zero), zero));
        d->incrementRegisters(args[0]);
    }
};

struct IP_scc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr cond = d->condition(insn->get_kind(), ops);
        SValuePtr zero = ops->number_(nBits, 0);
        d->write(args[0], ops->ite(cond, ops->invert(zero), zero));
        d->incrementRegisters(args[0]);
    }
};

struct IP_scs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr cond = d->condition(insn->get_kind(), ops);
        SValuePtr zero = ops->number_(nBits, 0);
        d->write(args[0], ops->ite(cond, ops->invert(zero), zero));
        d->incrementRegisters(args[0]);
    }
};

struct IP_sne: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr cond = d->condition(insn->get_kind(), ops);
        SValuePtr zero = ops->number_(nBits, 0);
        d->write(args[0], ops->ite(cond, ops->invert(zero), zero));
        d->incrementRegisters(args[0]);
    }
};

struct IP_seq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr cond = d->condition(insn->get_kind(), ops);
        SValuePtr zero = ops->number_(nBits, 0);
        d->write(args[0], ops->ite(cond, ops->invert(zero), zero));
        d->incrementRegisters(args[0]);
    }
};

struct IP_svc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr cond = d->condition(insn->get_kind(), ops);
        SValuePtr zero = ops->number_(nBits, 0);
        d->write(args[0], ops->ite(cond, ops->invert(zero), zero));
        d->incrementRegisters(args[0]);
    }
};

struct IP_svs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr cond = d->condition(insn->get_kind(), ops);
        SValuePtr zero = ops->number_(nBits, 0);
        d->write(args[0], ops->ite(cond, ops->invert(zero), zero));
        d->incrementRegisters(args[0]);
    }
};

struct IP_spl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr cond = d->condition(insn->get_kind(), ops);
        SValuePtr zero = ops->number_(nBits, 0);
        d->write(args[0], ops->ite(cond, ops->invert(zero), zero));
        d->incrementRegisters(args[0]);
    }
};

struct IP_smi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr cond = d->condition(insn->get_kind(), ops);
        SValuePtr zero = ops->number_(nBits, 0);
        d->write(args[0], ops->ite(cond, ops->invert(zero), zero));
        d->incrementRegisters(args[0]);
    }
};

struct IP_sge: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr cond = d->condition(insn->get_kind(), ops);
        SValuePtr zero = ops->number_(nBits, 0);
        d->write(args[0], ops->ite(cond, ops->invert(zero), zero));
        d->incrementRegisters(args[0]);
    }
};

struct IP_slt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr cond = d->condition(insn->get_kind(), ops);
        SValuePtr zero = ops->number_(nBits, 0);
        d->write(args[0], ops->ite(cond, ops->invert(zero), zero));
        d->incrementRegisters(args[0]);
    }
};

struct IP_sgt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr cond = d->condition(insn->get_kind(), ops);
        SValuePtr zero = ops->number_(nBits, 0);
        d->write(args[0], ops->ite(cond, ops->invert(zero), zero));
        d->incrementRegisters(args[0]);
    }
};

struct IP_sle: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr cond = d->condition(insn->get_kind(), ops);
        SValuePtr zero = ops->number_(nBits, 0);
        d->write(args[0], ops->ite(cond, ops->invert(zero), zero));
        d->incrementRegisters(args[0]);
    }
};

struct IP_sub: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits()==args[1]->get_nBits());
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        d->decrementRegisters(args[1]);
        SValuePtr a0 = d->read(args[0], nBits);
        SValuePtr a1 = d->read(args[1], nBits);
        SValuePtr result = ops->add(a1, ops->negate(a0));
        d->write(args[1], result);
        d->incrementRegisters(args[0]);
        d->incrementRegisters(args[1]);

        SValuePtr sm = ops->extract(a0, nBits-1, nBits);
        SValuePtr dm = ops->extract(a1, nBits-1, nBits);
        SValuePtr rm = ops->extract(result, nBits-1, nBits);
        SValuePtr isNegative = rm;
        SValuePtr isZero = ops->equalToZero(result);
        SValuePtr v1 = ops->and_(sm, ops->and_(ops->invert(dm), rm));
        SValuePtr v2 = ops->and_(dm, ops->invert(rm));
        SValuePtr isOverflow = ops->or_(ops->and_(ops->invert(sm), v2), v1);
        SValuePtr v3 = ops->and_(sm, rm);
        SValuePtr v4 = ops->or_(ops->and_(rm, ops->invert(dm)), v3);
        SValuePtr isCarry = ops->or_(ops->and_(sm, ops->invert(dm)), v4);
        ops->writeRegister(d->REG_CCR_C, isCarry);
        ops->writeRegister(d->REG_CCR_V, isOverflow);
        ops->writeRegister(d->REG_CCR_Z, isZero);
        ops->writeRegister(d->REG_CCR_N, isNegative);
        ops->writeRegister(d->REG_CCR_X, isCarry);
    }
};

struct IP_suba: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits() <= 32);
        ASSERT_require(args[1]->get_nBits() == 32);
        d->decrementRegisters(args[0]);
        SValuePtr subtrahend = ops->signExtend(d->read(args[0], args[0]->get_nBits()), 32);
        SValuePtr minuend = d->read(args[1], 32);
        SValuePtr difference = ops->add(minuend, ops->negate(subtrahend));
        d->write(args[1], difference);
        d->incrementRegisters(args[0]);
    }
};

struct IP_subi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits()==args[1]->get_nBits());
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[1]);
        SValuePtr a0 = d->read(args[0], nBits);
        SValuePtr a1 = d->read(args[1], nBits);
        SValuePtr result = ops->add(a1, ops->negate(a0));
        d->write(args[1], result);
        d->incrementRegisters(args[1]);

        SValuePtr sm = ops->extract(a0, nBits-1, nBits);
        SValuePtr dm = ops->extract(a1, nBits-1, nBits);
        SValuePtr rm = ops->extract(result, nBits-1, nBits);
        SValuePtr isNegative = rm;
        SValuePtr isZero = ops->equalToZero(result);
        SValuePtr v1 = ops->and_(sm, ops->and_(ops->invert(dm), rm));
        SValuePtr v2 = ops->and_(dm, ops->invert(rm));
        SValuePtr isOverflow = ops->or_(ops->and_(ops->invert(sm), v2), v1);
        SValuePtr v3 = ops->and_(sm, rm);
        SValuePtr v4 = ops->or_(ops->and_(rm, ops->invert(dm)), v3);
        SValuePtr isCarry = ops->or_(ops->and_(sm, ops->invert(dm)), v4);
        ops->writeRegister(d->REG_CCR_C, isCarry);
        ops->writeRegister(d->REG_CCR_V, isOverflow);
        ops->writeRegister(d->REG_CCR_Z, isZero);
        ops->writeRegister(d->REG_CCR_N, isNegative);
        ops->writeRegister(d->REG_CCR_X, isCarry);
    }
};

struct IP_subq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits() == args[1]->get_nBits());
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[1]);
        SValuePtr a0 = d->read(args[0], nBits);
        SValuePtr a1 = d->read(args[1], nBits);
        SValuePtr result = ops->add(a1, ops->negate(a0));
        d->write(args[1], result);
        d->incrementRegisters(args[1]);

        SValuePtr sm = ops->extract(a0, nBits-1, nBits);
        SValuePtr dm = ops->extract(a1, nBits-1, nBits);
        SValuePtr rm = ops->extract(result, nBits-1, nBits);
        SValuePtr isNegative = rm;
        SValuePtr isZero = ops->equalToZero(result);
        SValuePtr v1 = ops->and_(sm, ops->and_(ops->invert(dm), rm));
        SValuePtr v2 = ops->and_(dm, ops->invert(rm));
        SValuePtr isOverflow = ops->or_(ops->and_(ops->invert(sm), v2), v1);
        SValuePtr v3 = ops->and_(sm, rm);
        SValuePtr v4 = ops->or_(ops->and_(rm, ops->invert(dm)), v3);
        SValuePtr isCarry = ops->or_(ops->and_(sm, ops->invert(dm)), v4);
        ops->writeRegister(d->REG_CCR_C, isCarry);
        ops->writeRegister(d->REG_CCR_V, isOverflow);
        ops->writeRegister(d->REG_CCR_Z, isZero);
        ops->writeRegister(d->REG_CCR_N, isNegative);
        ops->writeRegister(d->REG_CCR_X, isCarry);
    }
};

struct IP_subx: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(args[0]->get_nBits()==args[1]->get_nBits());
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        d->decrementRegisters(args[1]);
        SValuePtr a0 = d->read(args[0], nBits);
        SValuePtr a1 = d->read(args[1], nBits);
        SValuePtr x = ops->readRegister(d->REG_CCR_X);
        SValuePtr result = ops->add(a1, ops->negate(ops->add(a0, ops->unsignedExtend(x, nBits))));
        d->write(args[1], result);
        d->incrementRegisters(args[0]);
        d->incrementRegisters(args[1]);

        SValuePtr sm = ops->extract(a0, nBits-1, nBits);
        SValuePtr dm = ops->extract(a1, nBits-1, nBits);
        SValuePtr rm = ops->extract(result, nBits-1, nBits);
        SValuePtr isNegative = rm;
        SValuePtr v1 = ops->and_(sm, ops->and_(ops->invert(dm), rm));
        SValuePtr v2 = ops->and_(dm, ops->invert(rm));
        SValuePtr isOverflow = ops->or_(ops->and_(ops->invert(sm), v2), v1);
        SValuePtr v3 = ops->and_(sm, rm);
        SValuePtr v4 = ops->or_(ops->and_(rm, ops->invert(dm)), v3);
        SValuePtr isCarry = ops->or_(ops->and_(sm, ops->invert(dm)), v4);

        // Note that the Z bit behaves differently for this instruction than normal.  If the result is non-zero then the Z bit
        // is cleared, otherwise it is unchanged.
        SValuePtr no = ops->boolean_(false);
        SValuePtr ccr_z = ops->readRegister(d->REG_CCR_Z);
        SValuePtr isZero = ops->ite(ops->equalToZero(result), ccr_z, no);

        ops->writeRegister(d->REG_CCR_C, isCarry);
        ops->writeRegister(d->REG_CCR_V, isOverflow);
        ops->writeRegister(d->REG_CCR_Z, isZero);
        ops->writeRegister(d->REG_CCR_N, isNegative);
        ops->writeRegister(d->REG_CCR_X, isCarry);
    }
};

struct IP_swap: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ASSERT_require(args[0]->get_nBits()==32);
        SValuePtr a0 = d->read(args[0], 32);
        SValuePtr v1 = ops->unsignedExtend(a0, 16);
        SValuePtr result = ops->concat(ops->extract(a0, 16, 32), v1);
        d->write(args[0], result);

        ops->writeRegister(d->REG_CCR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_V, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(a0)); // result is zero iff a0 is zero
        ops->writeRegister(d->REG_CCR_N, ops->extract(a0, 15, 16)); // result is negative iff a0 bit 15 is set
    }
};

struct IP_tas: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

// Note that these semantics are from the "ColdFire Family Programmer's Reference Manual, Rev 3" and differ slightly from the
// semantics described in "M68000 Family Programmer's Reference Manual".
struct IP_trap: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ops->writeRegister(d->REG_SR_S, ops->boolean_(true));
        SValuePtr origSsp = ops->readRegister(d->REG_SSP);

        SValuePtr ssp = ops->add(origSsp, ops->number_(32, -4));
        SValuePtr nextInsnAddr = ops->number_(32, insn->get_address() + insn->get_size());
        SValuePtr yes = ops->boolean_(true);
        ops->writeMemory(RegisterDescriptor(), ssp, nextInsnAddr, yes);

        ssp = ops->add(origSsp, ops->number_(32, -6));
        ops->writeMemory(RegisterDescriptor(), ssp, ops->readRegister(d->REG_SR), yes);

        ssp = ops->add(origSsp, ops->number_(32, -8));
        SValuePtr formatOffset = ops->undefined_(16);   // FIXME[Robb P. Matzke 2014-07-28]: where is this documented?
        ops->writeMemory(RegisterDescriptor(), ssp, formatOffset, yes);

        SValuePtr vectorNumber = d->read(args[0], args[0]->get_nBits());
        SValuePtr v1 = ops->unsignedExtend(ops->unsignedMultiply(ops->number_(32, 4), vectorNumber), 32);
        SValuePtr v2 = ops->add(ops->number_(32, 0x80), v1);
        SValuePtr vectorAddr = ops->add(ops->readRegister(d->REG_VBR), v2);
        ops->writeRegister(d->REG_PC, vectorAddr);

        ops->writeRegister(d->REG_SSP, ops->add(origSsp, ops->number_(32, -8)));
    }
};

struct IP_trapt: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapf: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            assert_args(insn, args, 0);
        } else {
            assert_args(insn, args, 1);
        }
    }
};

struct IP_traphi: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapls: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapcc: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapcs: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapne: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapeq: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapvc: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapvs: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trappl: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapmi: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapge: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_traplt: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapgt: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_traple: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapv: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_tst: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nBits = args[0]->get_nBits();
        d->decrementRegisters(args[0]);
        SValuePtr result = d->read(args[0], nBits);
        d->incrementRegisters(args[0]);

        ops->writeRegister(d->REG_CCR_C, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_V, ops->boolean_(false));
        ops->writeRegister(d->REG_CCR_Z, ops->equalToZero(result));
        ops->writeRegister(d->REG_CCR_N, ops->extract(result, nBits-1, nBits));
    }
};

struct IP_unlk: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ASSERT_require(args[0]->get_nBits() == 32);
        SValuePtr a0 = d->read(args[0], 32);
        ops->writeRegister(d->REG_A[7], a0);
        SValuePtr yes = ops->boolean_(true);
        d->write(args[0], ops->readMemory(RegisterDescriptor(), a0, ops->undefined_(32), yes));
        ops->writeRegister(d->REG_A[7], ops->add(a0, ops->number_(32, 4)));
    }
};

struct IP_unpk: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      DispatcherCil
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
DispatcherCil::iproc_init() {
#if M68K
    iprocSet(Cil_abcd,        new Cil::IP_abcd);
    iprocSet(Cil_add,         new Cil::IP_add);
    iprocSet(Cil_adda,        new Cil::IP_adda);
    iprocSet(Cil_addi,        new Cil::IP_addi);
    iprocSet(Cil_addq,        new Cil::IP_addq);
    iprocSet(Cil_addx,        new Cil::IP_addx);
    iprocSet(Cil_and,         new Cil::IP_and);
    iprocSet(Cil_andi,        new Cil::IP_andi);
    iprocSet(Cil_asl,         new Cil::IP_asl);
    iprocSet(Cil_asr,         new Cil::IP_asr);
    iprocSet(Cil_bra,         new Cil::IP_bra);
    iprocSet(Cil_bsr,         new Cil::IP_bsr);
    iprocSet(Cil_bhi,         new Cil::IP_bhi);
    iprocSet(Cil_bls,         new Cil::IP_bls);
    iprocSet(Cil_bcc,         new Cil::IP_bcc);
    iprocSet(Cil_bcs,         new Cil::IP_bcs);
    iprocSet(Cil_bne,         new Cil::IP_bne);
    iprocSet(Cil_beq,         new Cil::IP_beq);
    iprocSet(Cil_bvc,         new Cil::IP_bvc);
    iprocSet(Cil_bvs,         new Cil::IP_bvs);
    iprocSet(Cil_bpl,         new Cil::IP_bpl);
    iprocSet(Cil_bmi,         new Cil::IP_bmi);
    iprocSet(Cil_bge,         new Cil::IP_bge);
    iprocSet(Cil_blt,         new Cil::IP_blt);
    iprocSet(Cil_bgt,         new Cil::IP_bgt);
    iprocSet(Cil_ble,         new Cil::IP_ble);
    iprocSet(Cil_bchg,        new Cil::IP_bchg);
    iprocSet(Cil_bclr,        new Cil::IP_bclr);
    iprocSet(Cil_bfchg,       new Cil::IP_bfchg);
    iprocSet(Cil_bfclr,       new Cil::IP_bfclr);
    iprocSet(Cil_bfexts,      new Cil::IP_bfexts);
    iprocSet(Cil_bfextu,      new Cil::IP_bfextu);
    iprocSet(Cil_bfins,       new Cil::IP_bfins);
    iprocSet(Cil_bfset,       new Cil::IP_bfset);
    iprocSet(Cil_bftst,       new Cil::IP_bftst);
    iprocSet(Cil_bkpt,        new Cil::IP_bkpt);
    iprocSet(Cil_bset,        new Cil::IP_bset);
    iprocSet(Cil_btst,        new Cil::IP_btst);
    iprocSet(Cil_callm,       new Cil::IP_callm);
    iprocSet(Cil_cas,         new Cil::IP_cas);
    iprocSet(Cil_cas2,        new Cil::IP_cas2);
    iprocSet(Cil_chk,         new Cil::IP_chk);
    iprocSet(Cil_chk2,        new Cil::IP_chk2);
    iprocSet(Cil_clr,         new Cil::IP_clr);
    iprocSet(Cil_cmp,         new Cil::IP_cmp);
    iprocSet(Cil_cmpa,        new Cil::IP_cmpa);
    iprocSet(Cil_cmpi,        new Cil::IP_cmpi);
    iprocSet(Cil_cmpm,        new Cil::IP_cmpm);
    iprocSet(Cil_cmp2,        new Cil::IP_cmp2);
    iprocSet(Cil_cpushl,      new Cil::IP_cpushl);
    iprocSet(Cil_cpushp,      new Cil::IP_cpushp);
    iprocSet(Cil_cpusha,      new Cil::IP_cpusha);
    iprocSet(Cil_dbt,         new Cil::IP_dbt);
    iprocSet(Cil_dbf,         new Cil::IP_dbf);
    iprocSet(Cil_dbhi,        new Cil::IP_dbhi);
    iprocSet(Cil_dbls,        new Cil::IP_dbls);
    iprocSet(Cil_dbcc,        new Cil::IP_dbcc);
    iprocSet(Cil_dbcs,        new Cil::IP_dbcs);
    iprocSet(Cil_dbne,        new Cil::IP_dbne);
    iprocSet(Cil_dbeq,        new Cil::IP_dbeq);
    iprocSet(Cil_dbvc,        new Cil::IP_dbvc);
    iprocSet(Cil_dbvs,        new Cil::IP_dbvs);
    iprocSet(Cil_dbpl,        new Cil::IP_dbpl);
    iprocSet(Cil_dbmi,        new Cil::IP_dbmi);
    iprocSet(Cil_dbge,        new Cil::IP_dbge);
    iprocSet(Cil_dblt,        new Cil::IP_dblt);
    iprocSet(Cil_dbgt,        new Cil::IP_dbgt);
    iprocSet(Cil_dble,        new Cil::IP_dble);
    iprocSet(Cil_divs,        new Cil::IP_divs);
    iprocSet(Cil_divsl,       new Cil::IP_divsl);
    iprocSet(Cil_divu,        new Cil::IP_divu);
    iprocSet(Cil_divul,       new Cil::IP_divul);
    iprocSet(Cil_eor,         new Cil::IP_eor);
    iprocSet(Cil_eori,        new Cil::IP_eori);
    iprocSet(Cil_exg,         new Cil::IP_exg);
    iprocSet(Cil_ext,         new Cil::IP_ext);
    iprocSet(Cil_extb,        new Cil::IP_extb);
    iprocSet(Cil_fabs,        new Cil::IP_fabs);
    iprocSet(Cil_fsabs,       new Cil::IP_fsabs);
    iprocSet(Cil_fdabs,       new Cil::IP_fdabs);
    iprocSet(Cil_fadd,        new Cil::IP_fadd);
    iprocSet(Cil_fsadd,       new Cil::IP_fp_add(Cil_fsadd));
    iprocSet(Cil_fdadd,       new Cil::IP_fp_add(Cil_fdadd));
    iprocSet(Cil_fbeq,        new Cil::IP_fbeq);
    iprocSet(Cil_fbne,        new Cil::IP_fbne);
    iprocSet(Cil_fbgt,        new Cil::IP_fbgt);
    iprocSet(Cil_fbngt,       new Cil::IP_fbngt);
    iprocSet(Cil_fbge,        new Cil::IP_fbge);
    iprocSet(Cil_fbnge,       new Cil::IP_fbnge);
    iprocSet(Cil_fblt,        new Cil::IP_fblt);
    iprocSet(Cil_fbnlt,       new Cil::IP_fbnlt);
    iprocSet(Cil_fble,        new Cil::IP_fble);
    iprocSet(Cil_fbnle,       new Cil::IP_fbnle);
    iprocSet(Cil_fbgl,        new Cil::IP_fbgl);
    iprocSet(Cil_fbngl,       new Cil::IP_fbngl);
    iprocSet(Cil_fbgle,       new Cil::IP_fbgle);
    iprocSet(Cil_fbngle,      new Cil::IP_fbngle);
    iprocSet(Cil_fbogt,       new Cil::IP_fbogt);
    iprocSet(Cil_fbule,       new Cil::IP_fbule);
    iprocSet(Cil_fboge,       new Cil::IP_fboge);
    iprocSet(Cil_fbult,       new Cil::IP_fbult);
    iprocSet(Cil_fbolt,       new Cil::IP_fbolt);
    iprocSet(Cil_fbuge,       new Cil::IP_fbuge);
    iprocSet(Cil_fbole,       new Cil::IP_fbole);
    iprocSet(Cil_fbugt,       new Cil::IP_fbugt);
    iprocSet(Cil_fbogl,       new Cil::IP_fbogl);
    iprocSet(Cil_fbueq,       new Cil::IP_fbueq);
    iprocSet(Cil_fbor,        new Cil::IP_fbor);
    iprocSet(Cil_fbun,        new Cil::IP_fbun);
    iprocSet(Cil_fbf,         new Cil::IP_fbf);
    iprocSet(Cil_fbt,         new Cil::IP_fbt);
    iprocSet(Cil_fbsf,        new Cil::IP_fbsf);
    iprocSet(Cil_fbst,        new Cil::IP_fbst);
    iprocSet(Cil_fbseq,       new Cil::IP_fbseq);
    iprocSet(Cil_fbsne,       new Cil::IP_fbsne);
    iprocSet(Cil_fcmp,        new Cil::IP_fp_sub(Cil_fcmp));
    iprocSet(Cil_fdiv,        new Cil::IP_fdiv);
    iprocSet(Cil_fsdiv,       new Cil::IP_fsdiv);
    iprocSet(Cil_fddiv,       new Cil::IP_fddiv);
    iprocSet(Cil_fint,        new Cil::IP_fint);
    iprocSet(Cil_fintrz,      new Cil::IP_fintrz);
    iprocSet(Cil_fmove,       new Cil::IP_fp_move(Cil_fmove));
    iprocSet(Cil_fsmove,      new Cil::IP_fp_move(Cil_fsmove));
    iprocSet(Cil_fdmove,      new Cil::IP_fp_move(Cil_fdmove));
    iprocSet(Cil_fmovem,      new Cil::IP_fmovem);
    iprocSet(Cil_fmul,        new Cil::IP_fmul);
    iprocSet(Cil_fsmul,       new Cil::IP_fp_mul(Cil_fsmul));
    iprocSet(Cil_fdmul,       new Cil::IP_fp_mul(Cil_fdmul));
    iprocSet(Cil_fneg,        new Cil::IP_fneg);
    iprocSet(Cil_fsneg,       new Cil::IP_fsneg);
    iprocSet(Cil_fdneg,       new Cil::IP_fdneg);
    iprocSet(Cil_fnop,        new Cil::IP_fnop);
    iprocSet(Cil_fsqrt,       new Cil::IP_fsqrt);
    iprocSet(Cil_fssqrt,      new Cil::IP_fssqrt);
    iprocSet(Cil_fdsqrt,      new Cil::IP_fdsqrt);
    iprocSet(Cil_fsub,        new Cil::IP_fsub);
    iprocSet(Cil_fssub,       new Cil::IP_fp_sub(Cil_fssub));
    iprocSet(Cil_fdsub,       new Cil::IP_fp_sub(Cil_fdsub));
    iprocSet(Cil_ftst,        new Cil::IP_ftst);
    iprocSet(Cil_illegal,     new Cil::IP_illegal);
    iprocSet(Cil_jmp,         new Cil::IP_jmp);
    iprocSet(Cil_jsr,         new Cil::IP_jsr);
    iprocSet(Cil_lea,         new Cil::IP_lea);
    iprocSet(Cil_link,        new Cil::IP_link);
    iprocSet(Cil_lsl,         new Cil::IP_lsl);
    iprocSet(Cil_lsr,         new Cil::IP_lsr);
    iprocSet(Cil_mac,         new Cil::IP_mac);
    iprocSet(Cil_mov3q,       new Cil::IP_mov3q);
    iprocSet(Cil_movclr,      new Cil::IP_movclr);
    iprocSet(Cil_move,        new Cil::IP_move);
    iprocSet(Cil_move16,      new Cil::IP_move16);
    iprocSet(Cil_movea,       new Cil::IP_movea);
    iprocSet(Cil_movec,       new Cil::IP_move_no_ccr);
    iprocSet(Cil_movem,       new Cil::IP_movem);
    iprocSet(Cil_movep,       new Cil::IP_movep);
    iprocSet(Cil_moveq,       new Cil::IP_moveq);
    iprocSet(Cil_move_acc,    new Cil::IP_move_acc);
    iprocSet(Cil_move_accext, new Cil::IP_move_accext);
    iprocSet(Cil_move_ccr,    new Cil::IP_move_ccr);
    iprocSet(Cil_move_macsr,  new Cil::IP_move_macsr);
    iprocSet(Cil_move_mask,   new Cil::IP_move_mask);
    iprocSet(Cil_move_sr,     new Cil::IP_move_no_ccr);
    iprocSet(Cil_msac,        new Cil::IP_msac);
    iprocSet(Cil_muls,        new Cil::IP_muls);
    iprocSet(Cil_mulu,        new Cil::IP_mulu);
    iprocSet(Cil_mvs,         new Cil::IP_mvs);
    iprocSet(Cil_mvz,         new Cil::IP_mvz);
    iprocSet(Cil_nbcd,        new Cil::IP_nbcd);
    iprocSet(Cil_neg,         new Cil::IP_neg);
    iprocSet(Cil_negx,        new Cil::IP_negx);
    iprocSet(Cil_nop,         new Cil::IP_nop);
    iprocSet(Cil_not,         new Cil::IP_not);
    iprocSet(Cil_or,          new Cil::IP_or);
    iprocSet(Cil_ori,         new Cil::IP_ori);
    iprocSet(Cil_pack,        new Cil::IP_pack);
    iprocSet(Cil_pea,         new Cil::IP_pea);
    iprocSet(Cil_rol,         new Cil::IP_rol);
    iprocSet(Cil_ror,         new Cil::IP_ror);
    iprocSet(Cil_roxl,        new Cil::IP_roxl);
    iprocSet(Cil_roxr,        new Cil::IP_roxr);
    iprocSet(Cil_rtd,         new Cil::IP_rtd);
    iprocSet(Cil_rtm,         new Cil::IP_rtm);
    iprocSet(Cil_rtr,         new Cil::IP_rtr);
    iprocSet(Cil_rts,         new Cil::IP_rts);
    iprocSet(Cil_sbcd,        new Cil::IP_sbcd);
    iprocSet(Cil_st,          new Cil::IP_st);
    iprocSet(Cil_sf,          new Cil::IP_sf);
    iprocSet(Cil_shi,         new Cil::IP_shi);
    iprocSet(Cil_sls,         new Cil::IP_sls);
    iprocSet(Cil_scc,         new Cil::IP_scc);
    iprocSet(Cil_scs,         new Cil::IP_scs);
    iprocSet(Cil_sne,         new Cil::IP_sne);
    iprocSet(Cil_seq,         new Cil::IP_seq);
    iprocSet(Cil_svc,         new Cil::IP_svc);
    iprocSet(Cil_svs,         new Cil::IP_svs);
    iprocSet(Cil_spl,         new Cil::IP_spl);
    iprocSet(Cil_smi,         new Cil::IP_smi);
    iprocSet(Cil_sge,         new Cil::IP_sge);
    iprocSet(Cil_slt,         new Cil::IP_slt);
    iprocSet(Cil_sgt,         new Cil::IP_sgt);
    iprocSet(Cil_sle,         new Cil::IP_sle);
    iprocSet(Cil_sub,         new Cil::IP_sub);
    iprocSet(Cil_suba,        new Cil::IP_suba);
    iprocSet(Cil_subi,        new Cil::IP_subi);
    iprocSet(Cil_subq,        new Cil::IP_subq);
    iprocSet(Cil_subx,        new Cil::IP_subx);
    iprocSet(Cil_swap,        new Cil::IP_swap);
    iprocSet(Cil_tas,         new Cil::IP_tas);
    iprocSet(Cil_trap,        new Cil::IP_trap);
    iprocSet(Cil_trapt,       new Cil::IP_trapt);
    iprocSet(Cil_trapf,       new Cil::IP_trapf);
    iprocSet(Cil_traphi,      new Cil::IP_traphi);
    iprocSet(Cil_trapls,      new Cil::IP_trapls);
    iprocSet(Cil_trapcc,      new Cil::IP_trapcc);
    iprocSet(Cil_trapcs,      new Cil::IP_trapcs);
    iprocSet(Cil_trapne,      new Cil::IP_trapne);
    iprocSet(Cil_trapeq,      new Cil::IP_trapeq);
    iprocSet(Cil_trapvc,      new Cil::IP_trapvc);
    iprocSet(Cil_trapvs,      new Cil::IP_trapvs);
    iprocSet(Cil_trappl,      new Cil::IP_trappl);
    iprocSet(Cil_trapmi,      new Cil::IP_trapmi);
    iprocSet(Cil_trapge,      new Cil::IP_trapge);
    iprocSet(Cil_traplt,      new Cil::IP_traplt);
    iprocSet(Cil_trapgt,      new Cil::IP_trapgt);
    iprocSet(Cil_traple,      new Cil::IP_traple);
    iprocSet(Cil_trapv,       new Cil::IP_trapv);
    iprocSet(Cil_tst,         new Cil::IP_tst);
    iprocSet(Cil_unlk,        new Cil::IP_unlk);
    iprocSet(Cil_unpk,        new Cil::IP_unpk);
#endif
}

void
DispatcherCil::regcache_init() {
    if (regdict) {
        for (int i=0; i<8; ++i) {
            REG_D[i] = findRegister("d"+StringUtility::numberToString(i), 32);
            REG_A[i] = findRegister("a"+StringUtility::numberToString(i), 32);
            REG_FP[i] = findRegister("fp"+StringUtility::numberToString(i));
            ASSERT_require2(REG_FP[i].nBits()==64 || REG_FP[i].nBits()==80, "invalid floating point register size");
        }
        REG_PC = findRegister("pc", 32);
        REG_CCR   = findRegister("ccr", 8);
        REG_CCR_C = findRegister("ccr_c", 1);
        REG_CCR_V = findRegister("ccr_v", 1);
        REG_CCR_Z = findRegister("ccr_z", 1);
        REG_CCR_N = findRegister("ccr_n", 1);
        REG_CCR_X = findRegister("ccr_x", 1);
        REG_SR_S  = findRegister("sr_s",  1);
        REG_SR    = findRegister("sr", 16);
        REG_VBR   = findRegister("vbr", 32);
        REG_SSP   = findRegister("ssp", 32);

        // TOO1 (8/11/2014): Renamed variable from "OPTIONAL" to "IS_OPTIONAL".
        //                   "OPTIONAL" is a predefined macro in Windows.
        // Multiply-accumulated registers.  These are optional.
        static const bool IS_OPTIONAL = true;
        REG_MACSR_SU = findRegister("macsr_su", 1, IS_OPTIONAL);
        REG_MACSR_FI = findRegister("macsr_fi", 1, IS_OPTIONAL);
        REG_MACSR_N  = findRegister("macsr_n",  1, IS_OPTIONAL);
        REG_MACSR_Z  = findRegister("macsr_z",  1, IS_OPTIONAL);
        REG_MACSR_V  = findRegister("macsr_v",  1, IS_OPTIONAL);
        REG_MACSR_C  = findRegister("macsr_c",  1, IS_OPTIONAL);
        REG_MAC_MASK = findRegister("mask",    32, IS_OPTIONAL);
        REG_MACEXT0  = findRegister("accext0", 16, IS_OPTIONAL);
        REG_MACEXT1  = findRegister("accext1", 16, IS_OPTIONAL);
        REG_MACEXT2  = findRegister("accext2", 16, IS_OPTIONAL);
        REG_MACEXT3  = findRegister("accext3", 16, IS_OPTIONAL);

        // Floating-point condition code bits
        REG_FPCC_NAN = findRegister("fpcc_nan", 1);     // result is not a number
        REG_FPCC_I   = findRegister("fpcc_i",   1);     // result is +/- infinity
        REG_FPCC_Z   = findRegister("fpcc_z",   1);     // result is +/- zero
        REG_FPCC_N   = findRegister("fpcc_n",   1);     // result is negative

        // Floating-point status register exception bits
        REG_EXC_BSUN  = findRegister("exc_bsun",  1);   // branch/set on unordered
        REG_EXC_OPERR = findRegister("exc_operr", 1);   // operand error
        REG_EXC_OVFL  = findRegister("exc_ovfl",  1);   // overflow
        REG_EXC_UNFL  = findRegister("exc_unfl",  1);   // underflow
        REG_EXC_DZ    = findRegister("exc_dz",    1);   // divide by zero
        REG_EXC_INAN  = findRegister("exc_snan",  1);   // is not-a-number
        REG_EXC_IDE   = findRegister("exc_inex1", 1);   // input is denormalized
        REG_EXC_INEX  = findRegister("exc_inex2", 1);   // inexact result

        // Floating-point status register accrued exception bits
        REG_AEXC_IOP  = findRegister("aexc_iop",  1);
        REG_AEXC_OVFL = findRegister("aexc_ovfl", 1);
        REG_AEXC_UNFL = findRegister("aexc_unfl", 1);
        REG_AEXC_DZ   = findRegister("aexc_dz",   1);
        REG_AEXC_INEX = findRegister("aexc_inex", 1);
    }
}

void
DispatcherCil::memory_init() {
    if (BaseSemantics::StatePtr state = currentState()) {
        if (BaseSemantics::MemoryStatePtr memory = state->memoryState()) {
            switch (memory->get_byteOrder()) {
                case ByteOrder::ORDER_LSB:
                    mlog[WARN] <<"m68k memory state is using little-endian byte order\n";
                    break;
                case ByteOrder::ORDER_MSB:
                    break;
                case ByteOrder::ORDER_UNSPECIFIED:
                    memory->set_byteOrder(ByteOrder::ORDER_MSB);
                    break;
            }
        }
    }
}

RegisterDescriptor
DispatcherCil::instructionPointerRegister() const {
    return REG_PC;
}

RegisterDescriptor
DispatcherCil::stackPointerRegister() const {
    return REG_A[7];
}

RegisterDescriptor
DispatcherCil::stackFrameRegister() const {
    return RegisterDescriptor();
}

RegisterDescriptor
DispatcherCil::callReturnRegister() const {
    return RegisterDescriptor();
}

void
DispatcherCil::set_register_dictionary(const RegisterDictionary *regdict) {
    BaseSemantics::Dispatcher::set_register_dictionary(regdict);
    regcache_init();
}

SValuePtr
DispatcherCil::condition(CilInstructionKind kind, RiscOperators *ops) {
    switch (kind) {
#if M68K
        case Cil_bhi:
        case Cil_shi: {
            SValuePtr v1 = ops->invert(ops->readRegister(REG_CCR_Z));
            return ops->and_(ops->invert(ops->readRegister(REG_CCR_C)), v1);
        }
        case Cil_bls:
        case Cil_sls: {
            SValuePtr v1 = ops->readRegister(REG_CCR_Z);
            return ops->or_(ops->readRegister(REG_CCR_C), v1);
        }
        case Cil_bcc:
        case Cil_scc:
            // "M680000 Family Programmer's Reference Manual" lacks the invert.
            return ops->invert(ops->readRegister(REG_CCR_C));
        case Cil_bcs:
        case Cil_scs:
            return ops->readRegister(REG_CCR_C);
        case Cil_bne:
        case Cil_sne:
            // "M680000 Family Programmer's Reference Manual" lacks the invert.
            return ops->invert(ops->readRegister(REG_CCR_Z));
        case Cil_beq:
        case Cil_seq:
            return ops->readRegister(REG_CCR_Z);
        case Cil_bvc:
        case Cil_svc:
            // "M680000 Family Programmer's Reference Manual" lacks the invert.
            return ops->invert(ops->readRegister(REG_CCR_V));
        case Cil_bvs:
        case Cil_svs:
            return ops->readRegister(REG_CCR_V);
        case Cil_bpl:
        case Cil_spl:
            // "M680000 Family Programmer's Reference Manual" lacks the invert.
            return ops->invert(ops->readRegister(REG_CCR_N));
        case Cil_bmi:
        case Cil_smi:
            return ops->readRegister(REG_CCR_N);
        case Cil_bge:
        case Cil_sge: {
            SValuePtr n = ops->readRegister(REG_CCR_N);
            SValuePtr v = ops->readRegister(REG_CCR_V);
            SValuePtr notV = ops->invert(v);
            SValuePtr x = ops->and_(ops->invert(n), notV);
            return ops->or_(ops->and_(n, v), x);
        }
        case Cil_blt:
        case Cil_slt: {
            SValuePtr n = ops->readRegister(REG_CCR_N);
            SValuePtr v = ops->readRegister(REG_CCR_V);
            SValuePtr x = ops->and_(ops->invert(n), v);
            return ops->or_(ops->and_(n, ops->invert(v)), x);
        }
        case Cil_bgt:
        case Cil_sgt: {
            SValuePtr n = ops->readRegister(REG_CCR_N);
            SValuePtr v = ops->readRegister(REG_CCR_V);
            SValuePtr nz = ops->invert(ops->readRegister(REG_CCR_Z));
            SValuePtr notV = ops->invert(v);
            SValuePtr x = ops->and_(nz, ops->and_(ops->invert(n), notV));
            return ops->or_(ops->and_(nz, ops->and_(n, v)), x);
        }
        case Cil_ble:
        case Cil_sle: {
            SValuePtr n = ops->readRegister(REG_CCR_N);
            SValuePtr v = ops->readRegister(REG_CCR_V);
            SValuePtr z = ops->readRegister(REG_CCR_Z);
            SValuePtr x = ops->and_(ops->invert(n), v);
            return ops->or_(z, ops->or_(ops->and_(n, ops->invert(v)), x));
        }
#endif
        default:
#if M68K
         // DQ (10/8/2021): This M68k specific code is not wanted here.
            ASSERT_not_reachable("instruction is not conditional: " +
                                 stringifyBinaryAnalysisCilInstructionKind(kind));
#else
         // DQ (10/8/2021): Not clear what if any CIL support may be required here.
            ROSE_ASSERT(false);
#endif
    }
    return nullptr; // fix warning: no return statement in function returning non-void
}

// Override Dispatcher::read so that if we read the PC register we get the address of the current instruction plus 2.  See
// the note in Cil::P::process()
BaseSemantics::SValuePtr
DispatcherCil::read(SgAsmExpression *e, size_t value_nbits, size_t addr_nbits/*=0*/) {
    ASSERT_not_null(e);
    SValuePtr retval;
    if (SgAsmDirectRegisterExpression *re = isSgAsmDirectRegisterExpression(e)) {
        static const RegisterDescriptor REG_PC(Cil_regclass_spr, Cil_spr_pc, 0, 32);
        if (re->get_descriptor() == REG_PC) {
            SgAsmInstruction *insn = currentInstruction();
            ASSERT_not_null(insn);
            return operators()->number_(32, insn->get_address() + 2);
        }
    }
    return Dispatcher::read(e, value_nbits, addr_nbits);
}


void
DispatcherCil::updateFpsrExcInan(const SValuePtr &a, SgAsmType *aType, const SValuePtr &b, SgAsmType *bType) {
    // Set FPSR EXC INAN bit if either argument is nan; cleared otherwise.
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    ASSERT_require(a->nBits() == aType->get_nBits());
    ASSERT_not_null(b);
    ASSERT_not_null(bType);
    ASSERT_require(b->nBits() == bType->get_nBits());


    SValuePtr aIsNan;
    if (SgAsmFloatType *aFpType = isSgAsmFloatType(aType)) {
        aIsNan = operators()->fpIsNan(a, aFpType);
    } else {
        aIsNan = operators()->boolean_(false);
    }

    SValuePtr bIsNan;
    if (SgAsmFloatType *bFpType = isSgAsmFloatType(bType)) {
        bIsNan = operators()->fpIsNan(b, bFpType);
    } else {
        bIsNan = operators()->boolean_(false);
    }

    SValuePtr isNaN = operators()->or_(aIsNan, bIsNan);
    operators()->writeRegister(REG_EXC_INAN, isNaN);
}

void
DispatcherCil::updateFpsrExcIde(const SValuePtr &a, SgAsmType *aType, const SValuePtr &b, SgAsmType *bType) {
    // Set FPSR EXC IDE bit if either argument is denormalized; cleared otherwise.
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    ASSERT_require(a->nBits() == aType->get_nBits());
    ASSERT_not_null(b);
    ASSERT_not_null(bType);
    ASSERT_require(b->nBits() == bType->get_nBits());

    SValuePtr aIsDenorm;
    if (SgAsmFloatType *aFpType = isSgAsmFloatType(aType)) {
        aIsDenorm = operators()->fpIsDenormalized(a, aFpType);
    } else {
        aIsDenorm = operators()->boolean_(false);
    }

    SValuePtr bIsDenorm;
    if (SgAsmFloatType *bFpType = isSgAsmFloatType(bType)) {
        bIsDenorm = operators()->fpIsDenormalized(b, bFpType);
    } else {
        bIsDenorm = operators()->boolean_(false);
    }

    SValuePtr isDenorm = operators()->or_(aIsDenorm, bIsDenorm);
    operators()->writeRegister(REG_EXC_IDE, isDenorm);
}

void
DispatcherCil::updateFpsrExcOvfl(const SValuePtr &value, SgAsmType *valueType, SgAsmType *rounding, SgAsmType *dstType) {
    ASSERT_not_null(value);
    ASSERT_require(value->nBits() == valueType->get_nBits());
    ASSERT_not_null(rounding);
    ASSERT_not_null(dstType);

    SgAsmFloatType *valueFpType = isSgAsmFloatType(valueType);
    SgAsmFloatType *roundingFpType = isSgAsmFloatType(rounding);
    SgAsmFloatType *dstFpType = isSgAsmFloatType(dstType);

    if (!valueFpType || !roundingFpType || !dstFpType) {
        operators()->writeRegister(REG_EXC_OVFL, operators()->boolean_(false));
        return;
    }

    size_t nBits = std::max(valueFpType->exponentBits().size(), roundingFpType->exponentBits().size());

    SValuePtr valueBias = operators()->number_(nBits, valueFpType->exponentBias());
    SValuePtr valueExp = operators()->extract(value, valueFpType->exponentBits().least(), valueFpType->exponentBits().greatest()+1);
    valueExp = operators()->subtract(operators()->unsignedExtend(valueExp, nBits), valueBias);

    SValuePtr roundingExp =
        operators()->number_(nBits,
                           IntegerOps::genMask<uint64_t>(roundingFpType->exponentBits().size()) - roundingFpType->exponentBias());

    SValuePtr isOverflow = operators()->isUnsignedGreaterThanOrEqual(valueExp, roundingExp);
    operators()->writeRegister(REG_EXC_OVFL, isOverflow);
}

void
DispatcherCil::updateFpsrExcUnfl(const SValuePtr &value, SgAsmType *valueType, SgAsmType *rounding, SgAsmType *dstType) {
    ASSERT_not_null(value);
    ASSERT_require(value->nBits() == valueType->get_nBits());
    ASSERT_not_null(rounding);
    ASSERT_not_null(dstType);

    SgAsmFloatType *valueFpType = isSgAsmFloatType(valueType);
    SgAsmFloatType *roundingFpType = isSgAsmFloatType(rounding);
    SgAsmFloatType *dstFpType = isSgAsmFloatType(dstType);

    if (!valueFpType || !roundingFpType || !dstFpType) {
        operators()->writeRegister(REG_EXC_UNFL, operators()->boolean_(false));
        return;
    }

    size_t nBits = std::max(valueFpType->exponentBits().size(), roundingFpType->exponentBits().size());

    SValuePtr valueBias = operators()->number_(nBits, valueFpType->exponentBias());
    SValuePtr valueExp = operators()->extract(value, valueFpType->exponentBits().least(), valueFpType->exponentBits().greatest()+1);
    valueExp = operators()->subtract(operators()->unsignedExtend(valueExp, nBits), valueBias);

    // Minimum exponent field is "1", therefore minimum exponent value is "1 - bias"
    SValuePtr one = operators()->number_(nBits, 1);
    SValuePtr roundingBias = operators()->number_(nBits, roundingFpType->exponentBias());
    SValuePtr roundingExp = operators()->subtract(one, roundingBias);
    
    SValuePtr isUnderflow = operators()->isSignedLessThanOrEqual(valueExp, roundingExp);
    operators()->writeRegister(REG_EXC_UNFL, isUnderflow);
}

void
DispatcherCil::updateFpsrExcInex() {
    // FIXME[Robb Matzke 2020-01-03]
    operators()->writeRegister(REG_EXC_INEX, operators()->undefined_(1));
}

void
DispatcherCil::accumulateFpExceptions() {
    SValuePtr exc_ovfl = operators()->readRegister(REG_EXC_OVFL);
    operators()->writeRegister(REG_AEXC_OVFL, operators()->or_(operators()->readRegister(REG_AEXC_OVFL), exc_ovfl));
    SValuePtr exc_dz = operators()->readRegister(REG_EXC_DZ);
    operators()->writeRegister(REG_AEXC_DZ, operators()->or_(operators()->readRegister(REG_AEXC_DZ), exc_dz));
    SValuePtr exc_inex = operators()->readRegister(REG_EXC_INEX);
    operators()->writeRegister(REG_AEXC_INEX, operators()->or_(operators()->readRegister(REG_AEXC_INEX), exc_inex));
    SValuePtr exc_operr = operators()->readRegister(REG_EXC_OPERR);
    SValuePtr v1 = operators()->or_(operators()->readRegister(REG_EXC_INAN), exc_operr);
    SValuePtr v2 = operators()->or_(operators()->readRegister(REG_EXC_BSUN), v1);
    operators()->writeRegister(REG_AEXC_IOP, operators()->or_(operators()->readRegister(REG_AEXC_IOP), v2));
    SValuePtr v3 = operators()->or_(operators()->readRegister(REG_EXC_UNFL), exc_inex);
    operators()->writeRegister(REG_AEXC_UNFL, operators()->or_(operators()->readRegister(REG_AEXC_UNFL), v3));
}

void
DispatcherCil::adjustFpConditionCodes(const SValuePtr &result, SgAsmFloatType *fpType) {
    operators()->writeRegister(REG_FPCC_NAN, operators()->fpIsNan(result, fpType));
    operators()->writeRegister(REG_FPCC_I, operators()->fpIsInfinity(result, fpType));
    operators()->writeRegister(REG_FPCC_Z, operators()->fpIsZero(result, fpType));
    operators()->writeRegister(REG_FPCC_N, operators()->fpSign(result, fpType));
}

} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics2::DispatcherCil);
#endif

#endif
