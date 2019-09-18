#include "sage3basic.h"

#include "AsmUnparser_compat.h"
#include "BaseSemantics2.h"
#include "Diagnostics.h"
#include "DispatcherM68k.h"
#include "integerOps.h"
#include "stringify.h"
#include <boost/foreach.hpp>

using namespace Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functors that handle individual M68k instruction kinds
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace M68k {

// An intermediate class that reduces the amount of typing in all that follows.  Its process() method does some up-front
// checking, dynamic casting, and pointer dereferencing and then calls the p() method that does the real work.
class P: public BaseSemantics::InsnProcessor {
public:
    typedef DispatcherM68k *D;
    typedef BaseSemantics::RiscOperators *Ops;
    typedef SgAsmM68kInstruction *I;
    typedef const SgAsmExpressionPtrList &A;
    virtual void p(D, Ops, I, A) = 0;

    virtual void process(const BaseSemantics::DispatcherPtr &dispatcher_, SgAsmInstruction *insn_) ROSE_OVERRIDE {
        DispatcherM68kPtr dispatcher = DispatcherM68k::promote(dispatcher_);
        BaseSemantics::RiscOperatorsPtr operators = dispatcher->get_operators();
        SgAsmM68kInstruction *insn = isSgAsmM68kInstruction(insn_);
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

        SValuePtr sm = ops->extract(a0, a0->get_width()-1, a0->get_width());
        SValuePtr dm = ops->extract(a1, a1->get_width()-1, a1->get_width());
        SValuePtr rm = ops->extract(diff, diff->get_width()-1, diff->get_width());

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

        SValuePtr sm = ops->extract(a0, a0->get_width()-1, a0->get_width());
        SValuePtr dm = ops->extract(a1, a1->get_width()-1, a1->get_width());
        SValuePtr rm = ops->extract(diff, diff->get_width()-1, diff->get_width());

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

        SValuePtr sm = ops->extract(a0, a0->get_width()-1, a0->get_width());
        SValuePtr dm = ops->extract(a1, a1->get_width()-1, a1->get_width());
        SValuePtr rm = ops->extract(diff, diff->get_width()-1, diff->get_width());

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

        SValuePtr sm = ops->extract(a0, a0->get_width()-1, a0->get_width());
        SValuePtr dm = ops->extract(a1, a1->get_width()-1, a1->get_width());
        SValuePtr rm = ops->extract(diff, diff->get_width()-1, diff->get_width());

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
        ops->writeRegister(d->REG_CCR_N, ops->extract(quotient, quotient->get_width()-1, quotient->get_width()));
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
        ops->writeRegister(d->REG_CCR_N, ops->extract(quotient, quotient->get_width()-1, quotient->get_width()));
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
        ops->writeRegister(d->REG_CCR_N, ops->extract(quotient, quotient->get_width()-1, quotient->get_width()));
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
        ops->writeRegister(d->REG_CCR_N, ops->extract(quotient, quotient->get_width()-1, quotient->get_width()));
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

struct IP_fp_add: P {
    M68kInstructionKind kind;
    IP_fp_add(M68kInstructionKind kind): kind(kind) {
        ASSERT_require(m68k_fdadd == kind || m68k_fsadd == kind);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(isSgAsmDirectRegisterExpression(args[1]));
        SgAsmFloatType *srcType = isSgAsmFloatType(args[0]->get_type());
        SgAsmFloatType *dstType = isSgAsmFloatType(args[1]->get_type());
        SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(args[0]);
        SValuePtr a;
        if (rre && rre->get_descriptor().majorNumber() == m68k_regclass_fpr) {
            // F{D,S}ADD.D FPx, FPy
            a = ops->fpConvert(d->read(args[0], args[0]->get_nBits()), srcType, dstType);
        } else {
            // F{D,S}ADD.{B,W,L,S,D} ea, FPy
            if (srcType) {
                a = ops->fpConvert(d->read(args[0], args[0]->get_nBits()), srcType, dstType);
            } else {
                a = ops->fpFromInteger(d->read(args[0], args[0]->get_nBits()), dstType);
            }
        }
        SValuePtr b = d->read(args[1], args[1]->get_nBits());
        SValuePtr result = ops->fpAdd(a, b, dstType);
        d->write(args[1], result);
        d->adjustFpConditionCodes(result, dstType);

        ops->writeRegister(d->REG_EXC_BSUN,  ops->boolean_(false));
        ops->writeRegister(d->REG_EXC_INAN,  ops->fpIsNan(result, dstType));
        ops->writeRegister(d->REG_EXC_IDE,   ops->fpIsDenormalized(result, dstType));
        SValuePtr v1 = ops->fpSign(b, dstType);
        SValuePtr v2 = ops->xor_(ops->fpSign(a, dstType), v1);
        SValuePtr v3 = ops->and_(ops->fpIsInfinity(b, dstType), v2);
        ops->writeRegister(d->REG_EXC_OPERR,            // if a and b are opposite signed infinities
                           ops->and_(ops->fpIsInfinity(a, dstType), v3));
        ops->writeRegister(d->REG_EXC_OVFL,  ops->undefined_(1));// FIXME[Robb P. Matzke 2015-08-04]
        ops->writeRegister(d->REG_EXC_UNFL,  ops->undefined_(1));// FIXME[Robb P. Matzke 2015-08-04]
        ops->writeRegister(d->REG_EXC_DZ,    ops->boolean_(false));
        ops->writeRegister(d->REG_EXC_INEX,  ops->undefined_(1));// FIXME[Robb P. Matzke 2015-08-03]
        d->accumulateFpExceptions();
    }
};

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
        assert_args(insn, args, 2);
        SgAsmFloatType *dstType = isSgAsmFloatType(args[1]->get_type());
        ASSERT_not_null(dstType);
        SValuePtr a;
        if (SgAsmFloatType *srcType = isSgAsmFloatType(args[0]->get_type())) {
            a = ops->fpConvert(d->read(args[0], args[0]->get_nBits()), srcType, dstType);
        } else {
            a = ops->fpFromInteger(d->read(args[0], args[0]->get_nBits()), dstType);
        }
        SValuePtr result = ops->fpRoundTowardZero(a, dstType);
        d->write(args[1], result);
        d->adjustFpConditionCodes(result, dstType);
        ops->writeRegister(d->REG_EXC_BSUN,  ops->boolean_(false));
        ops->writeRegister(d->REG_EXC_INAN,  ops->fpIsNan(result, dstType));
        ops->writeRegister(d->REG_EXC_IDE,   ops->fpIsDenormalized(result, dstType));
        ops->writeRegister(d->REG_EXC_OPERR, ops->boolean_(false));
        ops->writeRegister(d->REG_EXC_OVFL,  ops->boolean_(false));
        ops->writeRegister(d->REG_EXC_UNFL,  ops->boolean_(false));
        ops->writeRegister(d->REG_EXC_DZ,    ops->boolean_(false));
        ops->writeRegister(d->REG_EXC_INEX,  ops->undefined_(1));// FIXME[Robb P. Matzke 2015-08-03]
        d->accumulateFpExceptions();
    }
};

struct IP_fp_move: P {
    M68kInstructionKind kind;
    IP_fp_move(M68kInstructionKind kind): kind(kind) {
        ASSERT_require(m68k_fmove == kind || m68k_fdmove == kind || m68k_fsmove == kind);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SgAsmFloatType *srcType = isSgAsmFloatType(args[0]->get_type());
        size_t srcNBits = args[0]->get_nBits();         // null if src is not a floating-point value
        SgAsmFloatType *dstType = isSgAsmFloatType(args[1]->get_type());
        SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(args[0]);
        SValuePtr result, src;
        if (rre && rre->get_descriptor().majorNumber() == m68k_regclass_fpr) {
            if (dstType) {
                // F{D,S}MOVE.{D,S} FPx, FPy
                // FMOVE.{D,S} FPx, ea
                src = d->read(args[0], srcNBits);
                result = ops->fpConvert(src, srcType, dstType);
            } else {
                // FMOVE.{B,W,L} FPx, ea
                ASSERT_not_null(srcType);
                SValuePtr dflt = ops->undefined_(args[1]->get_nBits());
                result = ops->fpToInteger(d->read(args[0], srcNBits), srcType, dflt);
            }
        } else {
            ASSERT_require2(isSgAsmDirectRegisterExpression(args[1]), insn->toString());
            if (!dstType)
                dstType = SageBuilderAsm::buildIeee754Binary64();
            if (srcType) {
                // F{D,S}MOVE.{D,S} ea, FPy
                // FMOVE.{D,S} ea, FPy
                src = d->read(args[0], srcNBits);
                result = ops->fpConvert(src, srcType, dstType);
            } else {
                // F{D,S}MOVE.{B,W,L} ea, FPy
                // FMOVE.{B,W,L} ea, FPy
                result = ops->fpFromInteger(d->read(args[0], srcNBits), dstType);
            }
        }

        d->write(args[1], result);

        if (dstType) {                                  // destination is D or S
            ops->writeRegister(d->REG_EXC_BSUN,  ops->boolean_(false));
            ops->writeRegister(d->REG_EXC_INAN,  ops->fpIsNan(result, dstType));
            ops->writeRegister(d->REG_EXC_IDE,   ops->fpIsDenormalized(result, dstType));
            ops->writeRegister(d->REG_EXC_OPERR, ops->boolean_(false));
            ops->writeRegister(d->REG_EXC_OVFL,  ops->boolean_(false));
            ops->writeRegister(d->REG_EXC_UNFL,  ops->boolean_(false));
            ops->writeRegister(d->REG_EXC_DZ,    ops->boolean_(false));
            ops->writeRegister(d->REG_EXC_INEX,  ops->undefined_(1)); // FIXME[Robb P. Matzke 2015-08-03]
        } else if (src) {                                             // destination is B, W, or L
            ops->writeRegister(d->REG_EXC_BSUN,  ops->boolean_(false));
            ops->writeRegister(d->REG_EXC_INAN,  ops->fpIsNan(src, dstType));
            ops->writeRegister(d->REG_EXC_IDE,   ops->fpIsDenormalized(src, dstType));
            ops->writeRegister(d->REG_EXC_OPERR, ops->boolean_(false));
            ops->writeRegister(d->REG_EXC_OVFL,  ops->undefined_(1)); // FIXME[Robb P. Matzke 2015-08-05]
            ops->writeRegister(d->REG_EXC_UNFL,  ops->undefined_(1)); // FIXME[Robb P. Matzke 2015-08-05]
            ops->writeRegister(d->REG_EXC_DZ,    ops->boolean_(false));
            ops->writeRegister(d->REG_EXC_INEX,  ops->undefined_(1)); // FIXME[Robb P. Matzke 2015-08-03]
        }

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
    M68kInstructionKind kind;
    IP_fp_mul(M68kInstructionKind kind): kind(kind) {
        ASSERT_require(m68k_fdmul == kind || m68k_fsmul == kind);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(isSgAsmDirectRegisterExpression(args[1]));
        SgAsmFloatType *srcType = isSgAsmFloatType(args[0]->get_type());
        SgAsmFloatType *dstType = isSgAsmFloatType(args[1]->get_type());
        ASSERT_not_null(dstType);
        SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(args[0]);
        SValuePtr a;
        if (rre && rre->get_descriptor().majorNumber() == m68k_regclass_fpr) {
            // F{D,S}MUL.{D,S} FPx, FPy
            a = ops->fpConvert(d->read(args[0], args[0]->get_nBits()), srcType, dstType);
        } else {
            // F{D,S}MUL.{B,W,L,D,S} ea, FPy
            if (srcType) {
                a = ops->fpConvert(d->read(args[0], args[0]->get_nBits()), srcType, dstType);
            } else {
                a = ops->fpFromInteger(d->read(args[0], args[0]->get_nBits()), dstType);
            }
        }
        SValuePtr b = d->read(args[1], args[1]->get_nBits());
        SValuePtr result = ops->fpMultiply(a, b, dstType);
        d->write(args[1], result);
        d->adjustFpConditionCodes(result, dstType);

        // Temporary exponent (without bias) wide enough to prevent overflow
        ASSERT_require(dstType->exponentBits().size() < 64); // leave room for a sign bit
        SValuePtr maxExponent = ops->number_(64, IntegerOps::genMask<uint64_t>(dstType->exponentBits().size()-1));
        SValuePtr minExponent = ops->invert(maxExponent); //  -(2^exponentSize)
        SValuePtr v1 = ops->signExtend(ops->fpEffectiveExponent(b, dstType), 64);
        SValuePtr wideExponent = ops->add(ops->signExtend(ops->fpEffectiveExponent(a, dstType), 64), v1);

        ops->writeRegister(d->REG_EXC_BSUN,  ops->boolean_(false));
        ops->writeRegister(d->REG_EXC_INAN,  ops->fpIsNan(result, dstType));
        ops->writeRegister(d->REG_EXC_IDE,   ops->fpIsDenormalized(result, dstType));
        SValuePtr v2 = ops->fpIsZero(b, dstType);
        SValuePtr v3 = ops->and_(ops->fpIsInfinity(a, dstType), v2);
        SValuePtr v4 = ops->fpIsInfinity(b, dstType);
        ops->writeRegister(d->REG_EXC_OPERR, ops->or_(ops->and_(ops->fpIsZero(a, dstType), v4), v3));
        ops->writeRegister(d->REG_EXC_OVFL,  ops->isSignedGreaterThanOrEqual(wideExponent, maxExponent));
        ops->writeRegister(d->REG_EXC_UNFL,  ops->isSignedLessThanOrEqual(wideExponent, minExponent));
        ops->writeRegister(d->REG_EXC_DZ,    ops->boolean_(false));
        ops->writeRegister(d->REG_EXC_INEX,  ops->undefined_(1));// FIXME[Robb P. Matzke 2015-08-03]
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

struct IP_fp_sub: P {
    M68kInstructionKind kind;
    IP_fp_sub(M68kInstructionKind kind): kind(kind) {
        ASSERT_require(m68k_fdsub == kind || m68k_fssub == kind || m68k_fcmp == kind);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(isSgAsmDirectRegisterExpression(args[1]));
        SgAsmFloatType *srcType = isSgAsmFloatType(args[0]->get_type());
        SgAsmFloatType *dstType = isSgAsmFloatType(args[1]->get_type());
        SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(args[0]);
        SValuePtr a;
        if (rre && rre->get_descriptor().majorNumber() == m68k_regclass_fpr) {
            // F{D,S}SUB.D FPx, FPy
            a = ops->fpConvert(d->read(args[0], args[0]->get_nBits()), srcType, dstType);
        } else {
            // F{D,S}SUB.{B,W,L,S,D} ea, FPy
            if (srcType) {
                a = ops->fpConvert(d->read(args[0], args[0]->get_nBits()), srcType, dstType);
            } else {
                a = ops->fpFromInteger(d->read(args[0], args[0]->get_nBits()), dstType);
            }
        }
        SValuePtr b = d->read(args[1], args[1]->get_nBits());
        SValuePtr result = ops->fpSubtract(b, a, dstType);
        if (kind != m68k_fcmp)
            d->write(args[1], result);
        d->adjustFpConditionCodes(result, dstType);

        ops->writeRegister(d->REG_EXC_BSUN,  ops->boolean_(false));
        ops->writeRegister(d->REG_EXC_INAN,  ops->fpIsNan(result, dstType));
        ops->writeRegister(d->REG_EXC_IDE,   ops->fpIsDenormalized(result, dstType));
        SValuePtr v1 = ops->fpSign(b, dstType);
        SValuePtr v2 = ops->invert(ops->xor_(ops->fpSign(a, dstType), v1));
        SValuePtr v3 = ops->and_(ops->fpIsInfinity(b, dstType), v2);
        ops->writeRegister(d->REG_EXC_OPERR,            // if a and b are like signed infinities
                           ops->and_(ops->fpIsInfinity(a, dstType), v3));
        ops->writeRegister(d->REG_EXC_OVFL,  ops->undefined_(1));// FIXME[Robb P. Matzke 2015-08-04]
        ops->writeRegister(d->REG_EXC_UNFL,  ops->undefined_(1));// FIXME[Robb P. Matzke 2015-08-04]
        ops->writeRegister(d->REG_EXC_DZ,    ops->boolean_(false));
        ops->writeRegister(d->REG_EXC_INEX,  ops->undefined_(1));// FIXME[Robb P. Matzke 2015-08-03]
        d->accumulateFpExceptions();
    }
};

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
                                           d->get_register_dictionary()->get_architecture_name(),
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
        ASSERT_require(product->get_width()==32 || product->get_width()==64);

        // MAC operational bits. I cannot find documentation that describes these bits adequately. In particular, does a 1 mean
        // signed or unsigned for the MACSR_SU bit; does a 1 mean fractional or integer mode for the MACSR_FI bit?
        SValuePtr isSigned = ops->readRegister(d->REG_MACSR_SU);
        SValuePtr isFrac = ops->readRegister(d->REG_MACSR_FI);
        
        // Load the accumulator
        SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(args[3]);
        ASSERT_not_null2(rre, "fourth operand must be a MAC accumulator register");
        RegisterDescriptor macAccReg = rre->get_descriptor();
        ASSERT_require2(macAccReg.majorNumber()==m68k_regclass_mac, "fourth operand must be a MAC accumulator register");
        ASSERT_require2(macAccReg.nBits()==32, "MAC accumulator register must be 32 bits");
        RegisterDescriptor macExtReg;
        switch (macAccReg.minorNumber()) {
            case m68k_mac_acc0: macExtReg = d->REG_MACEXT0; break;
            case m68k_mac_acc1: macExtReg = d->REG_MACEXT1; break;
            case m68k_mac_acc2: macExtReg = d->REG_MACEXT2; break;
            case m68k_mac_acc3: macExtReg = d->REG_MACEXT3; break;
            default:
                ASSERT_not_reachable("invalid mac accumulator register: " +
                                     stringifyBinaryAnalysisM68kMacRegister(macAccReg.minorNumber()));
        }
        SValuePtr macAcc = ops->readRegister(macAccReg);
        SValuePtr macExt = ops->readRegister(macExtReg);
        ASSERT_require(macAcc->get_width()==32);
        ASSERT_require(macExt->get_width()==16);

        // When the accumulator is operating in integer mode, the lower 40 bits of the shifted product is added to the
        // low-order 40 bits of the 48-bit (macExt|macAcc) concatenation.  The upper 8 bits of the concatenation are not
        // affected (at least according to the diagram on 1-10 of "ColdFire Family Programmer's Reference Manual, Rev 3").
        SValuePtr productInt = ops->unsignedExtend(product, 40);
        SValuePtr accInt = ops->concat(macAcc, macExt);
        SValuePtr v2 = ops->extract(accInt, 40, 48);
        SValuePtr newAccInt = ops->concat(ops->add(ops->unsignedExtend(accInt, 40), productInt), v2);
        ASSERT_require(newAccInt->get_width()==48);

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
        ASSERT_require(newAccFrac->get_width()==48);

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
            ASSERT_require(toMove->get_width()==32);
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

        size_t resultNBits = result->get_width();
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

        size_t resultNBits = result->get_width();
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
//                                      DispatcherM68k
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
DispatcherM68k::iproc_init() {
    iproc_set(m68k_abcd,        new M68k::IP_abcd);
    iproc_set(m68k_add,         new M68k::IP_add);
    iproc_set(m68k_adda,        new M68k::IP_adda);
    iproc_set(m68k_addi,        new M68k::IP_addi);
    iproc_set(m68k_addq,        new M68k::IP_addq);
    iproc_set(m68k_addx,        new M68k::IP_addx);
    iproc_set(m68k_and,         new M68k::IP_and);
    iproc_set(m68k_andi,        new M68k::IP_andi);
    iproc_set(m68k_asl,         new M68k::IP_asl);
    iproc_set(m68k_asr,         new M68k::IP_asr);
    iproc_set(m68k_bra,         new M68k::IP_bra);
    iproc_set(m68k_bsr,         new M68k::IP_bsr);
    iproc_set(m68k_bhi,         new M68k::IP_bhi);
    iproc_set(m68k_bls,         new M68k::IP_bls);
    iproc_set(m68k_bcc,         new M68k::IP_bcc);
    iproc_set(m68k_bcs,         new M68k::IP_bcs);
    iproc_set(m68k_bne,         new M68k::IP_bne);
    iproc_set(m68k_beq,         new M68k::IP_beq);
    iproc_set(m68k_bvc,         new M68k::IP_bvc);
    iproc_set(m68k_bvs,         new M68k::IP_bvs);
    iproc_set(m68k_bpl,         new M68k::IP_bpl);
    iproc_set(m68k_bmi,         new M68k::IP_bmi);
    iproc_set(m68k_bge,         new M68k::IP_bge);
    iproc_set(m68k_blt,         new M68k::IP_blt);
    iproc_set(m68k_bgt,         new M68k::IP_bgt);
    iproc_set(m68k_ble,         new M68k::IP_ble);
    iproc_set(m68k_bchg,        new M68k::IP_bchg);
    iproc_set(m68k_bclr,        new M68k::IP_bclr);
    iproc_set(m68k_bfchg,       new M68k::IP_bfchg);
    iproc_set(m68k_bfclr,       new M68k::IP_bfclr);
    iproc_set(m68k_bfexts,      new M68k::IP_bfexts);
    iproc_set(m68k_bfextu,      new M68k::IP_bfextu);
    iproc_set(m68k_bfins,       new M68k::IP_bfins);
    iproc_set(m68k_bfset,       new M68k::IP_bfset);
    iproc_set(m68k_bftst,       new M68k::IP_bftst);
    iproc_set(m68k_bkpt,        new M68k::IP_bkpt);
    iproc_set(m68k_bset,        new M68k::IP_bset);
    iproc_set(m68k_btst,        new M68k::IP_btst);
    iproc_set(m68k_callm,       new M68k::IP_callm);
    iproc_set(m68k_cas,         new M68k::IP_cas);
    iproc_set(m68k_cas2,        new M68k::IP_cas2);
    iproc_set(m68k_chk,         new M68k::IP_chk);
    iproc_set(m68k_chk2,        new M68k::IP_chk2);
    iproc_set(m68k_clr,         new M68k::IP_clr);
    iproc_set(m68k_cmp,         new M68k::IP_cmp);
    iproc_set(m68k_cmpa,        new M68k::IP_cmpa);
    iproc_set(m68k_cmpi,        new M68k::IP_cmpi);
    iproc_set(m68k_cmpm,        new M68k::IP_cmpm);
    iproc_set(m68k_cmp2,        new M68k::IP_cmp2);
    iproc_set(m68k_cpushl,      new M68k::IP_cpushl);
    iproc_set(m68k_cpushp,      new M68k::IP_cpushp);
    iproc_set(m68k_cpusha,      new M68k::IP_cpusha);
    iproc_set(m68k_dbt,         new M68k::IP_dbt);
    iproc_set(m68k_dbf,         new M68k::IP_dbf);
    iproc_set(m68k_dbhi,        new M68k::IP_dbhi);
    iproc_set(m68k_dbls,        new M68k::IP_dbls);
    iproc_set(m68k_dbcc,        new M68k::IP_dbcc);
    iproc_set(m68k_dbcs,        new M68k::IP_dbcs);
    iproc_set(m68k_dbne,        new M68k::IP_dbne);
    iproc_set(m68k_dbeq,        new M68k::IP_dbeq);
    iproc_set(m68k_dbvc,        new M68k::IP_dbvc);
    iproc_set(m68k_dbvs,        new M68k::IP_dbvs);
    iproc_set(m68k_dbpl,        new M68k::IP_dbpl);
    iproc_set(m68k_dbmi,        new M68k::IP_dbmi);
    iproc_set(m68k_dbge,        new M68k::IP_dbge);
    iproc_set(m68k_dblt,        new M68k::IP_dblt);
    iproc_set(m68k_dbgt,        new M68k::IP_dbgt);
    iproc_set(m68k_dble,        new M68k::IP_dble);
    iproc_set(m68k_divs,        new M68k::IP_divs);
    iproc_set(m68k_divsl,       new M68k::IP_divsl);
    iproc_set(m68k_divu,        new M68k::IP_divu);
    iproc_set(m68k_divul,       new M68k::IP_divul);
    iproc_set(m68k_eor,         new M68k::IP_eor);
    iproc_set(m68k_eori,        new M68k::IP_eori);
    iproc_set(m68k_exg,         new M68k::IP_exg);
    iproc_set(m68k_ext,         new M68k::IP_ext);
    iproc_set(m68k_extb,        new M68k::IP_extb);
    iproc_set(m68k_fabs,        new M68k::IP_fabs);
    iproc_set(m68k_fsabs,       new M68k::IP_fsabs);
    iproc_set(m68k_fdabs,       new M68k::IP_fdabs);
    iproc_set(m68k_fadd,        new M68k::IP_fadd);
    iproc_set(m68k_fsadd,       new M68k::IP_fp_add(m68k_fsadd));
    iproc_set(m68k_fdadd,       new M68k::IP_fp_add(m68k_fdadd));
    iproc_set(m68k_fbeq,        new M68k::IP_fbeq);
    iproc_set(m68k_fbne,        new M68k::IP_fbne);
    iproc_set(m68k_fbgt,        new M68k::IP_fbgt);
    iproc_set(m68k_fbngt,       new M68k::IP_fbngt);
    iproc_set(m68k_fbge,        new M68k::IP_fbge);
    iproc_set(m68k_fbnge,       new M68k::IP_fbnge);
    iproc_set(m68k_fblt,        new M68k::IP_fblt);
    iproc_set(m68k_fbnlt,       new M68k::IP_fbnlt);
    iproc_set(m68k_fble,        new M68k::IP_fble);
    iproc_set(m68k_fbnle,       new M68k::IP_fbnle);
    iproc_set(m68k_fbgl,        new M68k::IP_fbgl);
    iproc_set(m68k_fbngl,       new M68k::IP_fbngl);
    iproc_set(m68k_fbgle,       new M68k::IP_fbgle);
    iproc_set(m68k_fbngle,      new M68k::IP_fbngle);
    iproc_set(m68k_fbogt,       new M68k::IP_fbogt);
    iproc_set(m68k_fbule,       new M68k::IP_fbule);
    iproc_set(m68k_fboge,       new M68k::IP_fboge);
    iproc_set(m68k_fbult,       new M68k::IP_fbult);
    iproc_set(m68k_fbolt,       new M68k::IP_fbolt);
    iproc_set(m68k_fbuge,       new M68k::IP_fbuge);
    iproc_set(m68k_fbole,       new M68k::IP_fbole);
    iproc_set(m68k_fbugt,       new M68k::IP_fbugt);
    iproc_set(m68k_fbogl,       new M68k::IP_fbogl);
    iproc_set(m68k_fbueq,       new M68k::IP_fbueq);
    iproc_set(m68k_fbor,        new M68k::IP_fbor);
    iproc_set(m68k_fbun,        new M68k::IP_fbun);
    iproc_set(m68k_fbf,         new M68k::IP_fbf);
    iproc_set(m68k_fbt,         new M68k::IP_fbt);
    iproc_set(m68k_fbsf,        new M68k::IP_fbsf);
    iproc_set(m68k_fbst,        new M68k::IP_fbst);
    iproc_set(m68k_fbseq,       new M68k::IP_fbseq);
    iproc_set(m68k_fbsne,       new M68k::IP_fbsne);
    iproc_set(m68k_fcmp,        new M68k::IP_fp_sub(m68k_fcmp));
    iproc_set(m68k_fdiv,        new M68k::IP_fdiv);
    iproc_set(m68k_fsdiv,       new M68k::IP_fsdiv);
    iproc_set(m68k_fddiv,       new M68k::IP_fddiv);
    iproc_set(m68k_fint,        new M68k::IP_fint);
    iproc_set(m68k_fintrz,      new M68k::IP_fintrz);
    iproc_set(m68k_fmove,       new M68k::IP_fp_move(m68k_fmove));
    iproc_set(m68k_fsmove,      new M68k::IP_fp_move(m68k_fsmove));
    iproc_set(m68k_fdmove,      new M68k::IP_fp_move(m68k_fdmove));
    iproc_set(m68k_fmovem,      new M68k::IP_fmovem);
    iproc_set(m68k_fmul,        new M68k::IP_fmul);
    iproc_set(m68k_fsmul,       new M68k::IP_fp_mul(m68k_fsmul));
    iproc_set(m68k_fdmul,       new M68k::IP_fp_mul(m68k_fdmul));
    iproc_set(m68k_fneg,        new M68k::IP_fneg);
    iproc_set(m68k_fsneg,       new M68k::IP_fsneg);
    iproc_set(m68k_fdneg,       new M68k::IP_fdneg);
    iproc_set(m68k_fnop,        new M68k::IP_fnop);
    iproc_set(m68k_fsqrt,       new M68k::IP_fsqrt);
    iproc_set(m68k_fssqrt,      new M68k::IP_fssqrt);
    iproc_set(m68k_fdsqrt,      new M68k::IP_fdsqrt);
    iproc_set(m68k_fsub,        new M68k::IP_fsub);
    iproc_set(m68k_fssub,       new M68k::IP_fp_sub(m68k_fssub));
    iproc_set(m68k_fdsub,       new M68k::IP_fp_sub(m68k_fdsub));
    iproc_set(m68k_ftst,        new M68k::IP_ftst);
    iproc_set(m68k_illegal,     new M68k::IP_illegal);
    iproc_set(m68k_jmp,         new M68k::IP_jmp);
    iproc_set(m68k_jsr,         new M68k::IP_jsr);
    iproc_set(m68k_lea,         new M68k::IP_lea);
    iproc_set(m68k_link,        new M68k::IP_link);
    iproc_set(m68k_lsl,         new M68k::IP_lsl);
    iproc_set(m68k_lsr,         new M68k::IP_lsr);
    iproc_set(m68k_mac,         new M68k::IP_mac);
    iproc_set(m68k_mov3q,       new M68k::IP_mov3q);
    iproc_set(m68k_movclr,      new M68k::IP_movclr);
    iproc_set(m68k_move,        new M68k::IP_move);
    iproc_set(m68k_move16,      new M68k::IP_move16);
    iproc_set(m68k_movea,       new M68k::IP_movea);
    iproc_set(m68k_movec,       new M68k::IP_move_no_ccr);
    iproc_set(m68k_movem,       new M68k::IP_movem);
    iproc_set(m68k_movep,       new M68k::IP_movep);
    iproc_set(m68k_moveq,       new M68k::IP_moveq);
    iproc_set(m68k_move_acc,    new M68k::IP_move_acc);
    iproc_set(m68k_move_accext, new M68k::IP_move_accext);
    iproc_set(m68k_move_ccr,    new M68k::IP_move_ccr);
    iproc_set(m68k_move_macsr,  new M68k::IP_move_macsr);
    iproc_set(m68k_move_mask,   new M68k::IP_move_mask);
    iproc_set(m68k_move_sr,     new M68k::IP_move_no_ccr);
    iproc_set(m68k_msac,        new M68k::IP_msac);
    iproc_set(m68k_muls,        new M68k::IP_muls);
    iproc_set(m68k_mulu,        new M68k::IP_mulu);
    iproc_set(m68k_mvs,         new M68k::IP_mvs);
    iproc_set(m68k_mvz,         new M68k::IP_mvz);
    iproc_set(m68k_nbcd,        new M68k::IP_nbcd);
    iproc_set(m68k_neg,         new M68k::IP_neg);
    iproc_set(m68k_negx,        new M68k::IP_negx);
    iproc_set(m68k_nop,         new M68k::IP_nop);
    iproc_set(m68k_not,         new M68k::IP_not);
    iproc_set(m68k_or,          new M68k::IP_or);
    iproc_set(m68k_ori,         new M68k::IP_ori);
    iproc_set(m68k_pack,        new M68k::IP_pack);
    iproc_set(m68k_pea,         new M68k::IP_pea);
    iproc_set(m68k_rol,         new M68k::IP_rol);
    iproc_set(m68k_ror,         new M68k::IP_ror);
    iproc_set(m68k_roxl,        new M68k::IP_roxl);
    iproc_set(m68k_roxr,        new M68k::IP_roxr);
    iproc_set(m68k_rtd,         new M68k::IP_rtd);
    iproc_set(m68k_rtm,         new M68k::IP_rtm);
    iproc_set(m68k_rtr,         new M68k::IP_rtr);
    iproc_set(m68k_rts,         new M68k::IP_rts);
    iproc_set(m68k_sbcd,        new M68k::IP_sbcd);
    iproc_set(m68k_st,          new M68k::IP_st);
    iproc_set(m68k_sf,          new M68k::IP_sf);
    iproc_set(m68k_shi,         new M68k::IP_shi);
    iproc_set(m68k_sls,         new M68k::IP_sls);
    iproc_set(m68k_scc,         new M68k::IP_scc);
    iproc_set(m68k_scs,         new M68k::IP_scs);
    iproc_set(m68k_sne,         new M68k::IP_sne);
    iproc_set(m68k_seq,         new M68k::IP_seq);
    iproc_set(m68k_svc,         new M68k::IP_svc);
    iproc_set(m68k_svs,         new M68k::IP_svs);
    iproc_set(m68k_spl,         new M68k::IP_spl);
    iproc_set(m68k_smi,         new M68k::IP_smi);
    iproc_set(m68k_sge,         new M68k::IP_sge);
    iproc_set(m68k_slt,         new M68k::IP_slt);
    iproc_set(m68k_sgt,         new M68k::IP_sgt);
    iproc_set(m68k_sle,         new M68k::IP_sle);
    iproc_set(m68k_sub,         new M68k::IP_sub);
    iproc_set(m68k_suba,        new M68k::IP_suba);
    iproc_set(m68k_subi,        new M68k::IP_subi);
    iproc_set(m68k_subq,        new M68k::IP_subq);
    iproc_set(m68k_subx,        new M68k::IP_subx);
    iproc_set(m68k_swap,        new M68k::IP_swap);
    iproc_set(m68k_tas,         new M68k::IP_tas);
    iproc_set(m68k_trap,        new M68k::IP_trap);
    iproc_set(m68k_trapt,       new M68k::IP_trapt);
    iproc_set(m68k_trapf,       new M68k::IP_trapf);
    iproc_set(m68k_traphi,      new M68k::IP_traphi);
    iproc_set(m68k_trapls,      new M68k::IP_trapls);
    iproc_set(m68k_trapcc,      new M68k::IP_trapcc);
    iproc_set(m68k_trapcs,      new M68k::IP_trapcs);
    iproc_set(m68k_trapne,      new M68k::IP_trapne);
    iproc_set(m68k_trapeq,      new M68k::IP_trapeq);
    iproc_set(m68k_trapvc,      new M68k::IP_trapvc);
    iproc_set(m68k_trapvs,      new M68k::IP_trapvs);
    iproc_set(m68k_trappl,      new M68k::IP_trappl);
    iproc_set(m68k_trapmi,      new M68k::IP_trapmi);
    iproc_set(m68k_trapge,      new M68k::IP_trapge);
    iproc_set(m68k_traplt,      new M68k::IP_traplt);
    iproc_set(m68k_trapgt,      new M68k::IP_trapgt);
    iproc_set(m68k_traple,      new M68k::IP_traple);
    iproc_set(m68k_trapv,       new M68k::IP_trapv);
    iproc_set(m68k_tst,         new M68k::IP_tst);
    iproc_set(m68k_unlk,        new M68k::IP_unlk);
    iproc_set(m68k_unpk,        new M68k::IP_unpk);
}

void
DispatcherM68k::regcache_init() {
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
DispatcherM68k::memory_init() {
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
DispatcherM68k::instructionPointerRegister() const {
    return REG_PC;
}

RegisterDescriptor
DispatcherM68k::stackPointerRegister() const {
    return REG_A[7];
}

RegisterDescriptor
DispatcherM68k::callReturnRegister() const {
    return RegisterDescriptor();
}

void
DispatcherM68k::set_register_dictionary(const RegisterDictionary *regdict) {
    BaseSemantics::Dispatcher::set_register_dictionary(regdict);
    regcache_init();
}

SValuePtr
DispatcherM68k::condition(M68kInstructionKind kind, RiscOperators *ops) {
    switch (kind) {
        case m68k_bhi:
        case m68k_shi: {
            SValuePtr v1 = ops->invert(ops->readRegister(REG_CCR_Z));
            return ops->and_(ops->invert(ops->readRegister(REG_CCR_C)), v1);
        }
        case m68k_bls:
        case m68k_sls: {
            SValuePtr v1 = ops->readRegister(REG_CCR_Z);
            return ops->or_(ops->readRegister(REG_CCR_C), v1);
        }
        case m68k_bcc:
        case m68k_scc:
            // "M680000 Family Programmer's Reference Manual" lacks the invert.
            return ops->invert(ops->readRegister(REG_CCR_C));
        case m68k_bcs:
        case m68k_scs:
            return ops->readRegister(REG_CCR_C);
        case m68k_bne:
        case m68k_sne:
            // "M680000 Family Programmer's Reference Manual" lacks the invert.
            return ops->invert(ops->readRegister(REG_CCR_Z));
        case m68k_beq:
        case m68k_seq:
            return ops->readRegister(REG_CCR_Z);
        case m68k_bvc:
        case m68k_svc:
            // "M680000 Family Programmer's Reference Manual" lacks the invert.
            return ops->invert(ops->readRegister(REG_CCR_V));
        case m68k_bvs:
        case m68k_svs:
            return ops->readRegister(REG_CCR_V);
        case m68k_bpl:
        case m68k_spl:
            // "M680000 Family Programmer's Reference Manual" lacks the invert.
            return ops->invert(ops->readRegister(REG_CCR_N));
        case m68k_bmi:
        case m68k_smi:
            return ops->readRegister(REG_CCR_N);
        case m68k_bge:
        case m68k_sge: {
            SValuePtr n = ops->readRegister(REG_CCR_N);
            SValuePtr v = ops->readRegister(REG_CCR_V);
            SValuePtr notV = ops->invert(v);
            SValuePtr x = ops->and_(ops->invert(n), notV);
            return ops->or_(ops->and_(n, v), x);
        }
        case m68k_blt:
        case m68k_slt: {
            SValuePtr n = ops->readRegister(REG_CCR_N);
            SValuePtr v = ops->readRegister(REG_CCR_V);
            SValuePtr x = ops->and_(ops->invert(n), v);
            return ops->or_(ops->and_(n, ops->invert(v)), x);
        }
        case m68k_bgt:
        case m68k_sgt: {
            SValuePtr n = ops->readRegister(REG_CCR_N);
            SValuePtr v = ops->readRegister(REG_CCR_V);
            SValuePtr nz = ops->invert(ops->readRegister(REG_CCR_Z));
            SValuePtr notV = ops->invert(v);
            SValuePtr x = ops->and_(nz, ops->and_(ops->invert(n), notV));
            return ops->or_(ops->and_(nz, ops->and_(n, v)), x);
        }
        case m68k_ble:
        case m68k_sle: {
            SValuePtr n = ops->readRegister(REG_CCR_N);
            SValuePtr v = ops->readRegister(REG_CCR_V);
            SValuePtr z = ops->readRegister(REG_CCR_Z);
            SValuePtr x = ops->and_(ops->invert(n), v);
            return ops->or_(z, ops->or_(ops->and_(n, ops->invert(v)), x));
        }
        default:
            ASSERT_not_reachable("instruction is not conditional: " +
                                 stringifyBinaryAnalysisM68kInstructionKind(kind));
    }
}

// Override Dispatcher::read so that if we read the PC register we get the address of the current instruction plus 2.  See
// the note in M68k::P::process()
BaseSemantics::SValuePtr
DispatcherM68k::read(SgAsmExpression *e, size_t value_nbits, size_t addr_nbits/*=0*/) {
    ASSERT_not_null(e);
    SValuePtr retval;
    if (SgAsmDirectRegisterExpression *re = isSgAsmDirectRegisterExpression(e)) {
        static const RegisterDescriptor REG_PC(m68k_regclass_spr, m68k_spr_pc, 0, 32);
        if (re->get_descriptor() == REG_PC) {
            SgAsmInstruction *insn = currentInstruction();
            ASSERT_not_null(insn);
            return operators->number_(32, insn->get_address() + 2);
        }
    }
    return Dispatcher::read(e, value_nbits, addr_nbits);
}

void
DispatcherM68k::accumulateFpExceptions() {
    SValuePtr exc_ovfl = operators->readRegister(REG_EXC_OVFL);
    operators->writeRegister(REG_AEXC_OVFL, operators->or_(operators->readRegister(REG_AEXC_OVFL), exc_ovfl));
    SValuePtr exc_dz = operators->readRegister(REG_EXC_DZ);
    operators->writeRegister(REG_AEXC_DZ, operators->or_(operators->readRegister(REG_AEXC_DZ), exc_dz));
    SValuePtr exc_inex = operators->readRegister(REG_EXC_INEX);
    operators->writeRegister(REG_AEXC_INEX, operators->or_(operators->readRegister(REG_AEXC_INEX), exc_inex));
    SValuePtr exc_operr = operators->readRegister(REG_EXC_OPERR);
    SValuePtr v1 = operators->or_(operators->readRegister(REG_EXC_INAN), exc_operr);
    SValuePtr v2 = operators->or_(operators->readRegister(REG_EXC_BSUN), v1);
    operators->writeRegister(REG_AEXC_IOP, operators->or_(operators->readRegister(REG_AEXC_IOP), v2));
    SValuePtr v3 = operators->or_(operators->readRegister(REG_EXC_UNFL), exc_inex);
    operators->writeRegister(REG_AEXC_UNFL, operators->or_(operators->readRegister(REG_AEXC_UNFL), v3));
}

void
DispatcherM68k::adjustFpConditionCodes(const SValuePtr &result, SgAsmFloatType *fpType) {
    operators->writeRegister(REG_FPCC_NAN, operators->fpIsNan(result, fpType));
    operators->writeRegister(REG_FPCC_I, operators->fpIsInfinity(result, fpType));
    operators->writeRegister(REG_FPCC_Z, operators->fpIsZero(result, fpType));
    operators->writeRegister(REG_FPCC_N, operators->fpSign(result, fpType));
}

} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics2::DispatcherM68k);
#endif
