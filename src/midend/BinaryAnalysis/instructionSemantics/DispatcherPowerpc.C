#include <sage3basic.h>
#include <BaseSemantics2.h>
#include <Diagnostics.h>
#include <DispatcherPowerpc.h>
#include <integerOps.h>
#include <SageBuilderAsm.h>

using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {

namespace UpdateCr {
    enum Flag { NO, YES };
}

namespace SaveLink {
    enum Flag { NO, YES };
}

/*******************************************************************************************************************************
 *                                      Support functions
 *******************************************************************************************************************************/


/*******************************************************************************************************************************
 *                                      Functors that handle individual PowerPC instructions kinds
 *******************************************************************************************************************************/

namespace Powerpc {

// An intermediate class that reduces the amount of typing in all that follows.  Its process() method does some up-front
// checking, dynamic casting, and pointer dereferencing and then calls the p() method that does the real work.
class P: public BaseSemantics::InsnProcessor {
public:
    typedef DispatcherPowerpc *D;
    typedef BaseSemantics::RiscOperators *Ops;
    typedef SgAsmPowerpcInstruction *I;
    typedef const SgAsmExpressionPtrList &A;
    virtual void p(D, Ops, I, A) = 0;

    virtual void process(const BaseSemantics::DispatcherPtr &dispatcher_, SgAsmInstruction *insn_) ROSE_OVERRIDE {
        DispatcherPowerpcPtr dispatcher = DispatcherPowerpc::promote(dispatcher_);
        BaseSemantics::RiscOperatorsPtr operators = dispatcher->get_operators();
        SgAsmPowerpcInstruction *insn = isSgAsmPowerpcInstruction(insn_);
        ASSERT_require(insn!=NULL && insn==operators->currentInstruction());
        dispatcher->advanceInstructionPointer(insn);
        SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
        p(dispatcher.get(), operators.get(), insn, operands);
    }

    void assert_args(I insn, A args, size_t nargs) {
        if (args.size()!=nargs) {
            std::string mesg = "instruction must have " + StringUtility::numberToString(nargs) + "argument" + (1==nargs?"":"s");
            throw BaseSemantics::Exception(mesg, insn);
        }
    }

    // Builds a mask counting bits using the non-ROSE PowerPC numbering.  If a < b then bits a through b (inclusive) are
    // set. If a > b then bits a through 31 or 63 (inclusive) and bits zero through b (inclusive) are set. See PowerPC instruction
    // manual section 3.3.12 "Fixed-Point Rotate and Shift Instructions" for details.  The nBits argument is necessary because
    // of PowerPC's stupid backward numbering instead of using power-of-two numbering.
    uint64_t buildMask(size_t mstart, size_t mstop, size_t nBits) {
        ASSERT_require(mstart < nBits);
        ASSERT_require(mstop < nBits);
        if (mstart <= mstop) {
            return IntegerOps::genMask<uint64_t>((nBits-1) - mstop, (nBits-1) - mstart);
        } else {
            return (IntegerOps::genMask<uint64_t>(0, (nBits-1) - mstart) |
                    IntegerOps::genMask<uint64_t>((nBits-1) - mstop, (nBits-1)));
        }
    }
};

// Fixed-point addition
struct IP_add: P {
    UpdateCr::Flag updateCr;
    explicit IP_add(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr result = ops->add(a, b);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point add carrying
struct IP_addc: P {
    UpdateCr::Flag updateCr;
    explicit IP_addc(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr carryOut, overflow;
        BaseSemantics::SValuePtr result = ops->addCarry(a, b, carryOut, overflow);
        d->write(args[0], result);
        ops->writeRegister(d->REG_XER_CA, carryOut);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point add carrying with overflow
struct IP_addco: P {
    UpdateCr::Flag updateCr;
    explicit IP_addco(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr carryOut, overflow;
        BaseSemantics::SValuePtr result = ops->addCarry(a, b, carryOut, overflow);
        d->write(args[0], result);
        ops->writeRegister(d->REG_XER_CA, carryOut);
        d->setXerOverflow(overflow);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point add extended
struct IP_adde: P {
    UpdateCr::Flag updateCr;
    explicit IP_adde(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr c = ops->readRegister(d->REG_XER_CA);
        BaseSemantics::SValuePtr carries;
        BaseSemantics::SValuePtr result = ops->addWithCarries(a, b, c, carries /*out*/);
        BaseSemantics::SValuePtr carryOut = ops->extract(carries, 31, 32);
        d->write(args[0], result);
        ops->writeRegister(d->REG_XER_CA, carryOut);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point add extended with overflow
struct IP_addeo: P {
    UpdateCr::Flag updateCr;
    explicit IP_addeo(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr c = ops->readRegister(d->REG_XER_CA);
        BaseSemantics::SValuePtr carries;
        BaseSemantics::SValuePtr result = ops->addWithCarries(a, b, c, carries /*out*/);
        BaseSemantics::SValuePtr carryOut = ops->extract(carries, d->addressWidth()-1, d->addressWidth());
        BaseSemantics::SValuePtr carryOut2 = ops->extract(carries, d->addressWidth()-2, d->addressWidth()-1);
        BaseSemantics::SValuePtr overflow = ops->xor_(carryOut, carryOut2);
        d->write(args[0], result);
        ops->writeRegister(d->REG_XER_CA, carryOut);
        d->setXerOverflow(overflow);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point addition
struct IP_addi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr result = ops->add(a, b);
        d->write(args[0], result);
    }
};

// Fixed-point add immediate carrying
struct IP_addic: P {
    UpdateCr::Flag updateCr;
    explicit IP_addic(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr carryOut, overflow;
        BaseSemantics::SValuePtr result = ops->addCarry(a, b, carryOut, overflow);
        d->write(args[0], result);
        ops->writeRegister(d->REG_XER_CA, carryOut);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point add immediate shifted
struct IP_addis: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], 16);
        BaseSemantics::SValuePtr result = ops->add(a, ops->signExtend(ops->concat(ops->number_(16, 0), b), d->addressWidth()));
        d->write(args[0], result);
    }
};

// Fixed-point add minus one extended
struct IP_addme: P {
    UpdateCr::Flag updateCr;
    explicit IP_addme(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = ops->invert(ops->number_(d->addressWidth(), 0)); // -1
        BaseSemantics::SValuePtr c = ops->readRegister(d->REG_XER_CA);
        BaseSemantics::SValuePtr carries;
        BaseSemantics::SValuePtr result = ops->addWithCarries(a, b, c, carries /*out*/);
        BaseSemantics::SValuePtr carryOut = ops->extract(carries, d->addressWidth()-1, d->addressWidth());
        d->write(args[0], result);
        ops->writeRegister(d->REG_XER_CA, carryOut);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point add minus one extended with overflow
struct IP_addmeo: P {
    UpdateCr::Flag updateCr;
    explicit IP_addmeo(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = ops->invert(ops->number_(d->addressWidth(), 0)); // -1
        BaseSemantics::SValuePtr c = ops->readRegister(d->REG_XER_CA);
        BaseSemantics::SValuePtr carries;
        BaseSemantics::SValuePtr result = ops->addWithCarries(a, b, c, carries /*out*/);
        BaseSemantics::SValuePtr carryOut = ops->extract(carries, d->addressWidth()-1, d->addressWidth());
        BaseSemantics::SValuePtr carryOut2 = ops->extract(carries, d->addressWidth()-2, d->addressWidth()-1);
        BaseSemantics::SValuePtr overflow = ops->xor_(carryOut, carryOut2);
        d->write(args[0], result);
        ops->writeRegister(d->REG_XER_CA, carryOut);
        d->setXerOverflow(overflow);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed point add, setting overflow
struct IP_addo: P {
    UpdateCr::Flag updateCr;
    explicit IP_addo(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr carryOut, overflow;
        BaseSemantics::SValuePtr result = ops->addCarry(a, b, carryOut, overflow);
        d->write(args[0], result);
        ops->writeRegister(d->REG_XER_CA, carryOut);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point add to zero extended
struct IP_addze: P {
    UpdateCr::Flag updateCr;
    explicit IP_addze(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = ops->unsignedExtend(ops->readRegister(d->REG_XER_CA), d->addressWidth());
        BaseSemantics::SValuePtr carryOut, overflow;
        BaseSemantics::SValuePtr result = ops->addCarry(a, b, carryOut, overflow);
        d->write(args[0], result);
        ops->writeRegister(d->REG_XER_CA, carryOut);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point add to zero extended
struct IP_addzeo: P {
    UpdateCr::Flag updateCr;
    explicit IP_addzeo(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = ops->unsignedExtend(ops->readRegister(d->REG_XER_CA), d->addressWidth());
        BaseSemantics::SValuePtr carryOut, overflow;
        BaseSemantics::SValuePtr result = ops->addCarry(a, b, carryOut, overflow);
        d->write(args[0], result);
        ops->writeRegister(d->REG_XER_CA, carryOut);
        d->setXerOverflow(overflow);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point logical AND
struct IP_and: P {
    UpdateCr::Flag updateCr;
    explicit IP_and(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr result = ops->and_(a, b);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point logical AND with complement
struct IP_andc: P {
    UpdateCr::Flag updateCr;
    explicit IP_andc(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr result = ops->and_(a, ops->invert(b));
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point logical AND immediate
struct IP_andi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr result = ops->and_(a, b);
        d->write(args[0], result);
        d->updateCr0(result);
    }
};


// Fixed-point logical AND immediate shifted
struct IP_andis: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], 16);
        BaseSemantics::SValuePtr bShifted = ops->unsignedExtend(ops->concat(ops->number_(16, 0), b), d->addressWidth());
        BaseSemantics::SValuePtr result = ops->and_(a, bShifted);
        d->write(args[0], result);
        d->updateCr0(result);
    }
};

// Branch instruction relative (optionally save link)
struct IP_b: P {
    SaveLink::Flag save_link;
    IP_b(SaveLink::Flag save_link): save_link(save_link) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (SaveLink::YES == save_link)
            ops->writeRegister(d->REG_LR, ops->number_(d->addressWidth(), insn->get_address() + 4));
        BaseSemantics::SValuePtr target = d->read(args[0], d->addressWidth()); // operand already is sum of insn addr and LI value
        ops->writeRegister(d->REG_IAR, target);
    }
};

// Branch absolute (optionally save link)
struct IP_ba: P {
    SaveLink::Flag save_link;
    IP_ba(SaveLink::Flag save_link): save_link(save_link) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (SaveLink::YES == save_link)
            ops->writeRegister(d->REG_LR, ops->number_(d->addressWidth(), insn->get_address() + 4));
        ops->writeRegister(d->REG_IAR, d->read(args[0], d->addressWidth()));
    }
};

// Branch conditional instruction relative (optionally save link)
struct IP_bc: P {
    SaveLink::Flag save_link;
    IP_bc(SaveLink::Flag save_link): save_link(save_link) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        if (SaveLink::YES == save_link)
            ops->writeRegister(d->REG_LR, ops->number_(d->addressWidth(), insn->get_address() + 4));
        SgAsmIntegerValueExpression *byteValue = isSgAsmIntegerValueExpression(args[0]);
        ASSERT_not_null(byteValue);
        uint8_t boConstant = byteValue->get_value();
        // bool bo_4 = boConstant & 0x1;
        bool bo_3 = boConstant & 0x2;
        bool bo_2 = boConstant & 0x4;
        bool bo_1 = boConstant & 0x8;
        bool bo_0 = boConstant & 0x10;
        if (!bo_2) {
            BaseSemantics::SValuePtr negOne = ops->number_(d->addressWidth(), -1);
            ops->writeRegister(d->REG_CTR, ops->add(ops->readRegister(d->REG_CTR), negOne));
        }
        BaseSemantics::SValuePtr ctr_ok;
        if (bo_2) {
            ctr_ok = ops->boolean_(true);
        } else if (bo_3) {
            ctr_ok = ops->equalToZero(ops->readRegister(d->REG_CTR));
        } else {
            ctr_ok = ops->invert(ops->equalToZero(ops->readRegister(d->REG_CTR)));
        }
        SgAsmRegisterReferenceExpression *bi = isSgAsmRegisterReferenceExpression(args[1]);
        ASSERT_require(bi && bi->get_descriptor().majorNumber() == powerpc_regclass_cr && bi->get_descriptor().nBits() == 1);
        BaseSemantics::SValuePtr cr_bi = bo_0 && bo_2 ? ops->boolean_(true) : ops->readRegister(bi->get_descriptor());
        BaseSemantics::SValuePtr cond_ok = bo_0 ? ops->boolean_(true) : bo_1 ? cr_bi : ops->invert(cr_bi);
        BaseSemantics::SValuePtr target = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr iar = ops->readRegister(d->REG_IAR);
        ops->writeRegister(d->REG_IAR, ops->ite(ops->and_(ctr_ok, cond_ok), target, iar));
    }
};

// Branch conditional to count register
struct IP_bcctr: P {
    SaveLink::Flag save_link;
    IP_bcctr(SaveLink::Flag save_link): save_link(save_link) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        if (SaveLink::YES == save_link)
            ops->writeRegister(d->REG_LR, ops->number_(d->addressWidth(), insn->get_address() + 4));
        SgAsmIntegerValueExpression *byteValue = isSgAsmIntegerValueExpression(args[0]);
        ASSERT_not_null(byteValue);
        uint8_t boConstant = byteValue->get_value();
        bool bo_1 = boConstant & 0x8;
        bool bo_0 = boConstant & 0x10;
        SgAsmRegisterReferenceExpression *bi = isSgAsmRegisterReferenceExpression(args[1]);
        ASSERT_require(bi && bi->get_descriptor().majorNumber() == powerpc_regclass_cr && bi->get_descriptor().nBits() == 1);
        BaseSemantics::SValuePtr cr_bi = ops->readRegister(bi->get_descriptor());
        BaseSemantics::SValuePtr cond_ok = bo_0 ? ops->boolean_(true) : bo_1 ? cr_bi : ops->invert(cr_bi);
        BaseSemantics::SValuePtr iar = ops->readRegister(d->REG_IAR);
        BaseSemantics::SValuePtr mask = ops->number_(d->addressWidth(), IntegerOps::genMask<uint64_t>(2, d->addressWidth()-1));
        ops->writeRegister(d->REG_IAR, ops->ite(cond_ok, ops->and_(ops->readRegister(d->REG_CTR), mask), iar));
    }
};

// Branch conditional (optionally save link)
struct IP_bclr: P {
    SaveLink::Flag save_link;
    IP_bclr(SaveLink::Flag save_link): save_link(save_link) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        if (SaveLink::YES == save_link)
            ops->writeRegister(d->REG_LR, ops->number_(d->addressWidth(), insn->get_address() + 4));
        SgAsmIntegerValueExpression *byteValue = isSgAsmIntegerValueExpression(args[0]);
        ASSERT_not_null(byteValue);
        uint8_t boConstant = byteValue->get_value();
        // bool bo_4 = boConstant & 0x1;
        bool bo_3 = boConstant & 0x2;
        bool bo_2 = boConstant & 0x4;
        bool bo_1 = boConstant & 0x8;
        bool bo_0 = boConstant & 0x10;
        if (!bo_2) {
            BaseSemantics::SValuePtr negOne = ops->number_(d->addressWidth(), -1);
            ops->writeRegister(d->REG_CTR, ops->add(ops->readRegister(d->REG_CTR), negOne));
        }
        BaseSemantics::SValuePtr ctr_ok;
        if (bo_2) {
            ctr_ok = ops->boolean_(true);
        } else if (bo_3) {
            ctr_ok = ops->equalToZero(ops->readRegister(d->REG_CTR));
        } else {
            ctr_ok = ops->invert(ops->equalToZero(ops->readRegister(d->REG_CTR)));
        }
        SgAsmRegisterReferenceExpression *bi = isSgAsmRegisterReferenceExpression(args[1]);
        ASSERT_require(bi && bi->get_descriptor().majorNumber() == powerpc_regclass_cr && bi->get_descriptor().nBits() == 1);
        BaseSemantics::SValuePtr cr_bi = bo_0 && bo_2 ? ops->boolean_(true) : ops->readRegister(bi->get_descriptor());
        BaseSemantics::SValuePtr cond_ok = bo_0 ? ops->boolean_(true) : bo_1 ? cr_bi : ops->invert(cr_bi);
        BaseSemantics::SValuePtr mask = ops->number_(d->addressWidth(), IntegerOps::genMask<uint64_t>(2, d->addressWidth()-1));
        BaseSemantics::SValuePtr target = ops->and_(ops->readRegister(d->REG_LR), mask);
        BaseSemantics::SValuePtr iar = ops->readRegister(d->REG_IAR);
        ops->writeRegister(d->REG_IAR, ops->ite(ops->and_(ctr_ok, cond_ok), target, iar));
    }
};

// Fixed-point compare
struct IP_cmp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        ASSERT_require(isSgAsmIntegerValueExpression(args[1]));
        size_t nBits = isSgAsmIntegerValueExpression(args[1])->get_absoluteValue() ? d->addressWidth() : 32;
        BaseSemantics::SValuePtr a = d->read(args[2], nBits);
        BaseSemantics::SValuePtr b = d->read(args[3], nBits);
        BaseSemantics::SValuePtr eq = ops->isEqual(a, b);
        BaseSemantics::SValuePtr lt = ops->isSignedLessThan(a, b);
        BaseSemantics::SValuePtr cmp = ops->ite(eq,
                                                ops->number_(3, 1),            // equal:        0b001
                                                ops->ite(lt,
                                                         ops->number_(3, 4),   // less than:    0b100
                                                         ops->number_(3, 2))); // greater than: 0b010
        BaseSemantics::SValuePtr cr = ops->concat(ops->readRegister(d->REG_XER_SO), cmp);
        d->write(args[0], cr);
    }
};

// Fixed-point compare immediate
struct IP_cmpi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        ASSERT_require(isSgAsmIntegerValueExpression(args[1]));
        bool doExtend = 0 == isSgAsmIntegerValueExpression(args[1])->get_absoluteValue();
        BaseSemantics::SValuePtr a = doExtend ?
                                     ops->signExtend(d->read(args[2], 32), d->addressWidth()) :
                                     d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[3], d->addressWidth());
        BaseSemantics::SValuePtr eq = ops->isEqual(a, b);
        BaseSemantics::SValuePtr lt = ops->isSignedLessThan(a, b);
        BaseSemantics::SValuePtr cmp = ops->ite(eq,
                                                ops->number_(3, 1),            // equal:        0b001
                                                ops->ite(lt,
                                                         ops->number_(3, 4),   // less than:    0b100
                                                         ops->number_(3, 2))); // greater than: 0b010
        BaseSemantics::SValuePtr cr = ops->concat(ops->readRegister(d->REG_XER_SO), cmp);
        d->write(args[0], cr);
    }
};

// Fixed-point compare logical
struct IP_cmpl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        ASSERT_require(isSgAsmIntegerValueExpression(args[1]));
        size_t nBits = isSgAsmIntegerValueExpression(args[1])->get_absoluteValue() ? d->addressWidth() : 32;
        BaseSemantics::SValuePtr a = d->read(args[2], nBits);
        BaseSemantics::SValuePtr b = d->read(args[3], nBits);
        BaseSemantics::SValuePtr eq = ops->isEqual(a, b);
        BaseSemantics::SValuePtr lt = ops->isUnsignedLessThan(a, b);
        BaseSemantics::SValuePtr cmp = ops->ite(eq,
                                                ops->number_(3, 1),            // equal:        0b001
                                                ops->ite(lt,
                                                         ops->number_(3, 4),   // less than:    0b100
                                                         ops->number_(3, 2))); // greater than: 0b010
        BaseSemantics::SValuePtr cr = ops->concat(ops->readRegister(d->REG_XER_SO), cmp);
        d->write(args[0], cr);
    }
};

// Fixed-point compare logical immediate
struct IP_cmpli: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        ASSERT_require(isSgAsmIntegerValueExpression(args[1]));
        bool doExtend = 0 == isSgAsmIntegerValueExpression(args[1])->get_absoluteValue();
        BaseSemantics::SValuePtr a = doExtend ?
                                     ops->unsignedExtend(d->read(args[2], 32), d->addressWidth()) :
                                     d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr b = ops->unsignedExtend(d->read(args[3], 32), d->addressWidth());
        BaseSemantics::SValuePtr eq = ops->isEqual(a, b);
        BaseSemantics::SValuePtr lt = ops->isUnsignedLessThan(a, b);
        BaseSemantics::SValuePtr cmp = ops->ite(eq,
                                                ops->number_(3, 1),            // equal:        0b001
                                                ops->ite(lt,
                                                         ops->number_(3, 4),   // less than:    0b100
                                                         ops->number_(3, 2))); // greater than: 0b010
        BaseSemantics::SValuePtr cr = ops->concat(ops->readRegister(d->REG_XER_SO), cmp);
        d->write(args[0], cr);
    }
};

// Count leading zeros doubleword
struct IP_cntlzd: P {
    UpdateCr::Flag updateCr;
    explicit IP_cntlzd(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr sixtyThree = ops->number_(64, 63);
        BaseSemantics::SValuePtr sixtyFour = ops->number_(64, 64);
        BaseSemantics::SValuePtr a = d->read(args[1], 64);
        BaseSemantics::SValuePtr n = ops->subtract(sixtyThree, ops->mostSignificantSetBit(a));
        BaseSemantics::SValuePtr result = ops->unsignedExtend(ops->ite(ops->equalToZero(a), sixtyFour, n), d->addressWidth());
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr) {
            d->updateCr0(result);
            ops->writeRegister(d->REG_CR0_LT, ops->boolean_(false));
        }
    }
};

// Count leading zeros word
struct IP_cntlzw: P {
    UpdateCr::Flag updateCr;
    explicit IP_cntlzw(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr thirtyOne = ops->number_(32, 31);
        BaseSemantics::SValuePtr thirtyTwo = ops->number_(32, 32);
        BaseSemantics::SValuePtr a = d->read(args[1], 32);
        BaseSemantics::SValuePtr n = ops->subtract(thirtyOne, ops->mostSignificantSetBit(a));
        BaseSemantics::SValuePtr result = ops->unsignedExtend(ops->ite(ops->equalToZero(a), thirtyTwo, n), d->addressWidth());
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr) {
            d->updateCr0(result);
            ops->writeRegister(d->REG_CR0_LT, ops->boolean_(false));
        }
    }
};

// Condition register AND
struct IP_crand: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr ba = d->read(args[1], 1);
        BaseSemantics::SValuePtr bb = d->read(args[2], 1);
        BaseSemantics::SValuePtr result = ops->and_(ba, bb);
        d->write(args[0], result);
    }
};

// Condition register AND with complement
struct IP_crandc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr ba = d->read(args[1], 1);
        BaseSemantics::SValuePtr bb = d->read(args[2], 1);
        BaseSemantics::SValuePtr result = ops->and_(ba, ops->invert(bb));
        d->write(args[0], result);
    }
};

// Condition register equivalent
struct IP_creqv: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr ba = d->read(args[1], 1);
        BaseSemantics::SValuePtr bb = d->read(args[2], 1);
        BaseSemantics::SValuePtr result = ops->invert(ops->xor_(ba, bb));
        d->write(args[0], result);
    }
};

// Condition register NAND
struct IP_crnand: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr ba = d->read(args[1], 1);
        BaseSemantics::SValuePtr bb = d->read(args[2], 1);
        BaseSemantics::SValuePtr result = ops->invert(ops->and_(ba, bb));
        d->write(args[0], result);
    }
};

// Condition register NOR
struct IP_crnor: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr ba = d->read(args[1], 1);
        BaseSemantics::SValuePtr bb = d->read(args[2], 1);
        BaseSemantics::SValuePtr result = ops->invert(ops->or_(ba, bb));
        d->write(args[0], result);
    }
};

// Condition register OR
struct IP_cror: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr ba = d->read(args[1], 1);
        BaseSemantics::SValuePtr bb = d->read(args[2], 1);
        BaseSemantics::SValuePtr result = ops->or_(ba, bb);
        d->write(args[0], result);
    }
};

// Condition register OR with complement
struct IP_crorc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr ba = d->read(args[1], 1);
        BaseSemantics::SValuePtr bb = d->read(args[2], 1);
        BaseSemantics::SValuePtr result = ops->or_(ba, ops->invert(bb));
        d->write(args[0], result);
    }
};

// Condition register XOR
struct IP_crxor: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr ba = d->read(args[1], 1);
        BaseSemantics::SValuePtr bb = d->read(args[2], 1);
        BaseSemantics::SValuePtr result = ops->xor_(ba, bb);
        d->write(args[0], result);
    }
};

// Fixed point divide doubleword
struct IP_divd: P {
    UpdateCr::Flag updateCr;
    explicit IP_divd(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr dividend = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr divisor = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr quotient = ops->signedDivide(dividend, divisor);

        // Undefined: 0x80...0 / -1
        // Undefined: x / 0
        BaseSemantics::SValuePtr zero = ops->number_(d->addressWidth(), 0);
        BaseSemantics::SValuePtr negOne = ops->invert(zero);
        BaseSemantics::SValuePtr minInt = ops->number_(d->addressWidth(), IntegerOps::genMask<uint64_t>(d->addressWidth()-1,
                                                                                                        d->addressWidth()-1));
        BaseSemantics::SValuePtr isUndefined = ops->or_(ops->and_(ops->isEqual(dividend, minInt), ops->isEqual(divisor, negOne)),
                                                        ops->isEqual(divisor, zero));

        BaseSemantics::SValuePtr result = ops->ite(isUndefined, ops->undefined_(d->addressWidth()), quotient);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed point divide doubleword with overflow
struct IP_divdo: P {
    UpdateCr::Flag updateCr;
    explicit IP_divdo(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr dividend = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr divisor = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr quotient = ops->signedDivide(dividend, divisor);

        // Undefined: 0x80...0 / -1
        // Undefined: x / 0
        BaseSemantics::SValuePtr zero = ops->number_(d->addressWidth(), 0);
        BaseSemantics::SValuePtr negOne = ops->invert(zero);
        BaseSemantics::SValuePtr minInt = ops->number_(d->addressWidth(), IntegerOps::genMask<uint64_t>(d->addressWidth()-1,
                                                                                                        d->addressWidth()-1));
        BaseSemantics::SValuePtr isUndefined = ops->or_(ops->and_(ops->isEqual(dividend, minInt), ops->isEqual(divisor, negOne)),
                                                        ops->isEqual(divisor, zero));

        BaseSemantics::SValuePtr result = ops->ite(isUndefined, ops->undefined_(d->addressWidth()), quotient);
        d->write(args[0], result);
        d->setXerOverflow(isUndefined);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed point divide doubleword unsigned
struct IP_divdu: P {
    UpdateCr::Flag updateCr;
    explicit IP_divdu(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr dividend = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr divisor = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr quotient = ops->unsignedDivide(dividend, divisor);
        BaseSemantics::SValuePtr isUndefined = ops->equalToZero(divisor);
        BaseSemantics::SValuePtr result = ops->ite(isUndefined, ops->undefined_(d->addressWidth()), quotient);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed point divide doubleword unsigned
struct IP_divduo: P {
    UpdateCr::Flag updateCr;
    explicit IP_divduo(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr dividend = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr divisor = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr quotient = ops->unsignedDivide(dividend, divisor);
        BaseSemantics::SValuePtr isUndefined = ops->equalToZero(divisor);
        BaseSemantics::SValuePtr result = ops->ite(isUndefined, ops->undefined_(d->addressWidth()), quotient);
        d->write(args[0], result);
        d->setXerOverflow(isUndefined);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point divide word
struct IP_divw: P {
    UpdateCr::Flag updateCr;
    explicit IP_divw(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr dividend = ops->signExtend(d->read(args[1], 32), d->addressWidth());
        BaseSemantics::SValuePtr divisor = ops->signExtend(d->read(args[2], 32), d->addressWidth());
        BaseSemantics::SValuePtr quotient = ops->signedDivide(dividend, divisor);
        
        // Undefined: 0x80...0 / -1
        // Undefined: x / 0
        BaseSemantics::SValuePtr zero = ops->number_(d->addressWidth(), 0);
        BaseSemantics::SValuePtr negOne = ops->invert(zero);
        BaseSemantics::SValuePtr minInt = ops->number_(d->addressWidth(), IntegerOps::genMask<uint64_t>(d->addressWidth()-1,
                                                                                                        d->addressWidth()-1));
        BaseSemantics::SValuePtr isUndefined = ops->or_(ops->and_(ops->isEqual(dividend, minInt), ops->isEqual(divisor, negOne)),
                                                        ops->isEqual(divisor, zero));

        if (d->addressWidth() == 64)
            quotient = ops->concat(ops->unsignedExtend(quotient, 32), ops->undefined_(32));
        BaseSemantics::SValuePtr result = ops->ite(isUndefined, ops->undefined_(d->addressWidth()), quotient);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point divide word with overflow
struct IP_divwo: P {
    UpdateCr::Flag updateCr;
    explicit IP_divwo(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr dividend = ops->signExtend(d->read(args[1], 32), d->addressWidth());
        BaseSemantics::SValuePtr divisor = ops->signExtend(d->read(args[2], 32), d->addressWidth());
        BaseSemantics::SValuePtr quotient = ops->signedDivide(dividend, divisor);
        
        // Undefined: 0x80...0 / -1
        // Undefined: x / 0
        BaseSemantics::SValuePtr zero = ops->number_(d->addressWidth(), 0);
        BaseSemantics::SValuePtr negOne = ops->invert(zero);
        BaseSemantics::SValuePtr minInt = ops->number_(d->addressWidth(), IntegerOps::genMask<uint64_t>(d->addressWidth()-1,
                                                                                                        d->addressWidth()-1));
        BaseSemantics::SValuePtr isUndefined = ops->or_(ops->and_(ops->isEqual(dividend, minInt), ops->isEqual(divisor, negOne)),
                                                        ops->isEqual(divisor, zero));

        if (d->addressWidth() == 64)
            quotient = ops->concat(ops->unsignedExtend(quotient, 32), ops->undefined_(32));
        BaseSemantics::SValuePtr result = ops->ite(isUndefined, ops->undefined_(d->addressWidth()), quotient);
        d->write(args[0], result);
        d->setXerOverflow(isUndefined);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point divide word unsigned
struct IP_divwu: P {
    UpdateCr::Flag updateCr;
    explicit IP_divwu(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr dividend = ops->unsignedExtend(d->read(args[1], 32), d->addressWidth());
        BaseSemantics::SValuePtr divisor = ops->unsignedExtend(d->read(args[2], 32), d->addressWidth());
        BaseSemantics::SValuePtr quotient = ops->unsignedDivide(dividend, divisor);
        BaseSemantics::SValuePtr isUndefined = ops->equalToZero(divisor);
        if (d->addressWidth() == 64)
            quotient = ops->concat(ops->unsignedExtend(quotient, 32), ops->undefined_(32));
        BaseSemantics::SValuePtr result = ops->ite(isUndefined, ops->undefined_(d->addressWidth()), quotient);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point divide word unsigned with overflow
struct IP_divwuo: P {
    UpdateCr::Flag updateCr;
    explicit IP_divwuo(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr dividend = ops->unsignedExtend(d->read(args[1], 32), d->addressWidth());
        BaseSemantics::SValuePtr divisor = ops->unsignedExtend(d->read(args[2], 32), d->addressWidth());
        BaseSemantics::SValuePtr quotient = ops->unsignedDivide(dividend, divisor);
        BaseSemantics::SValuePtr isUndefined = ops->equalToZero(divisor);
        if (d->addressWidth() == 64)
            quotient = ops->concat(ops->unsignedExtend(quotient, 32), ops->undefined_(32));
        BaseSemantics::SValuePtr result = ops->ite(isUndefined, ops->undefined_(d->addressWidth()), quotient);
        d->write(args[0], result);
        d->setXerOverflow(isUndefined);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Enforce in-order execution of I/O
struct IP_eieio: P {
    void p(D d, Ops ops, I insn, A args) {
        // no semantics necessary
    }
};

// Fixed-point bit-wise equal
struct IP_eqv: P {
    UpdateCr::Flag updateCr;
    explicit IP_eqv(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr result = ops->invert(ops->xor_(a, b));
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Extend sign byte
struct IP_extsb: P {
    UpdateCr::Flag updateCr;
    explicit IP_extsb(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr byte = d->read(args[1], 8);
        BaseSemantics::SValuePtr result = ops->signExtend(byte, d->addressWidth());
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Extend sign halfword
struct IP_extsh: P {
    UpdateCr::Flag updateCr;
    explicit IP_extsh(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr halfword = d->read(args[1], 16);
        BaseSemantics::SValuePtr result = ops->signExtend(halfword, d->addressWidth());
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Extend sign word
struct IP_extsw: P {
    UpdateCr::Flag updateCr;
    explicit IP_extsw(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr word = d->read(args[1], 32);
        BaseSemantics::SValuePtr result = ops->signExtend(word, 64);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Load byte and zero extend
struct IP_lbz: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->write(args[0], ops->unsignedExtend(d->read(args[1], 8), d->addressWidth()));
    }
};

// Load byte and zero extend with update
struct IP_lbzu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->write(args[0], ops->unsignedExtend(d->readAndUpdate(ops, args[1], 8), d->addressWidth()));
    }
};

// Load doubleword
struct IP_ld: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->write(args[0], d->read(args[1], 64));
    }
};

// Load doubleword with update
struct IP_ldu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->write(args[0], d->readAndUpdate(ops, args[1], 64));
    }
};

// Load doubleword with update indexed
struct IP_ldux: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->write(args[0], d->readAndUpdate(ops, args[1], 64));
    }
};

// Load doubleword indexed
struct IP_ldx: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->write(args[0], d->read(args[1], 64));
    }
};

// Load floating-point double
struct IP_lfd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr fp64 = ops->reinterpret(d->read(args[1], 64), SageBuilderAsm::buildIeee754Binary64());
        d->write(args[0], fp64);
    }
};

// Load floating-point single
struct IP_lfs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr fp32 = ops->reinterpret(d->read(args[1], 32), SageBuilderAsm::buildIeee754Binary32());
        BaseSemantics::SValuePtr fp64 = ops->fpConvert(fp32, SageBuilderAsm::buildIeee754Binary32(),
                                                       SageBuilderAsm::buildIeee754Binary64());
        d->write(args[0], fp64);
    }
};

// Load half-word algebraic
struct IP_lha: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->write(args[0], ops->signExtend(d->read(args[1], 16), d->addressWidth()));
    }
};

// Load halfword algebraic with update
struct IP_lhau: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->write(args[0], ops->signExtend(d->readAndUpdate(ops, args[1], 16), d->addressWidth()));
    }
};

// Load halfword byte-reverse indexed
struct IP_lhbrx: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr value = d->read(args[1], 16);
        BaseSemantics::SValuePtr swapped = ops->concat(ops->extract(value, 8, 16), ops->extract(value, 0, 8));
        d->write(args[0], ops->unsignedExtend(swapped, d->addressWidth()));
    }
};

// Load half-word and zero extend
struct IP_lhz: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->write(args[0], ops->unsignedExtend(d->read(args[1], 16), d->addressWidth()));
    }
};

// Load half-word and zero with update
struct IP_lhzu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->write(args[0], ops->unsignedExtend(d->readAndUpdate(ops, args[1], 16), d->addressWidth()));
    }
};

// Load multiple word
struct IP_lmw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr base = d->effectiveAddress(args[1], d->addressWidth());
        SgAsmRegisterReferenceExpression *rt = isSgAsmRegisterReferenceExpression(args[0]);
        ASSERT_require(rt && rt->get_descriptor().majorNumber() == powerpc_regclass_gpr);
        RegisterDescriptor reg = rt->get_descriptor();
        rose_addr_t offset = 0;
        for (unsigned minor = reg.minorNumber(); minor < 32; minor += 1, offset += 4) {
            BaseSemantics::SValuePtr addr = ops->add(base, ops->number_(d->addressWidth(), offset));
            BaseSemantics::SValuePtr dflt = ops->undefined_(32);
            BaseSemantics::SValuePtr value = ops->readMemory(RegisterDescriptor(), addr, dflt, ops->boolean_(true));
            reg.minorNumber(minor);
            ops->writeRegister(reg, ops->unsignedExtend(value, d->addressWidth()));
        }
    }
};

// Load string word immediate
struct IP_lswi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        const BaseSemantics::SValuePtr baseAddr = d->read(args[1], 32);
        size_t memOffset = 0;

        ASSERT_not_null(isSgAsmIntegerValueExpression(args[2]));
        size_t nBytesToTransfer = isSgAsmIntegerValueExpression(args[2])->get_absoluteValue();
        ASSERT_require(nBytesToTransfer > 0 && nBytesToTransfer <= 32);

        ASSERT_not_null(isSgAsmDirectRegisterExpression(args[0]));
        const RegisterDescriptor baseReg = isSgAsmDirectRegisterExpression(args[0])->get_descriptor();

        const size_t nRegistersAffected = (nBytesToTransfer + 3) / 4;
        for (size_t i = 0; i < nRegistersAffected; ++i) {
            const size_t nBytes = std::min(nBytesToTransfer, size_t(4));
            const BaseSemantics::SValuePtr dflt = ops->undefined_(8*nBytes);
            const BaseSemantics::SValuePtr ea = ops->add(baseAddr, ops->number_(32, memOffset));
            BaseSemantics::SValuePtr value = ops->readMemory(RegisterDescriptor(), ea, dflt, ops->boolean_(true)); // big endian
            if (nBytes < 4)
                value = ops->concat(ops->number_(8*(4-nBytes), 0), value);
            if (d->addressWidth() == 64)
                value = ops->concat(value, ops->number_(32, 0));
            const size_t regNum = (baseReg.minorNumber() + i) % 32;
            ops->writeRegister(RegisterDescriptor(baseReg.majorNumber(), regNum, 0, d->addressWidth()), value);

            memOffset += 4;
            nBytesToTransfer -= nBytes;
        }
    }
};

// Load string word indexed
struct IP_lswx: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr zeroByte = ops->number_(8, 0);
        BaseSemantics::SValuePtr baseAddr = d->read(args[1], 32);
        BaseSemantics::SValuePtr nBytes = ops->extract(ops->readRegister(d->REG_XER), 0, 7); // 7 bits wide
        ASSERT_require(isSgAsmDirectRegisterExpression(args[0]));
        RegisterDescriptor startReg = isSgAsmDirectRegisterExpression(args[0])->get_descriptor();
        for (size_t i = 0; i < 32; ++i) {
            size_t regNum = (startReg.minorNumber() + i) % 32;

            // Obtain the value to write into this register. The four low order bytes for the register come from three places:
            //   1. If the byte counter (memory offset from base address) is less than the total number of bytes to
            //      copy, then the value comes from reading a byte of memory.
            //   2. Else if any previous bytes for this register came from memory, then the value is zero.
            //   3. Else the value is the original byte from the register (i.e., no data transferred).
            BaseSemantics::SValuePtr isRegTouched = ops->isUnsignedLessThan(ops->number_(7, i*4), nBytes);
            BaseSemantics::SValuePtr result;
            if (d->addressWidth() > 32)
                result = ops->number_(d->addressWidth() - 32, 0);
            for (size_t j = 0; j < 4; ++j) {
                // Read a byte from memory
                size_t memOffset = i*4 + j;
                BaseSemantics::SValuePtr ea = ops->add(baseAddr, ops->number_(32, memOffset));
                BaseSemantics::SValuePtr memByte = ops->readMemory(RegisterDescriptor(), ea,
                                                                   ops->undefined_(8), ops->boolean_(true));

                // Read a byte from the register
                size_t regOffset = 32 - (j+1)*8; // stupid PPC bit numbering
                RegisterDescriptor curReg(startReg.majorNumber(), regNum, regOffset, 8);
                BaseSemantics::SValuePtr regByte = ops->readRegister(curReg);

                // Decide what value to write to the register
                BaseSemantics::SValuePtr doXfer = ops->isUnsignedLessThan(ops->number_(7, memOffset), nBytes);
                BaseSemantics::SValuePtr byte = ops->ite(isRegTouched, ops->ite(doXfer, memByte, zeroByte), regByte);

                // Compose value to be written to register, writing bytes from more significant to less significant
                // (what PowerPC stupidly calls "left to right" and describes as low index to high index).
                result = result ? ops->concat(byte, result) : byte;
            }
            ops->writeRegister(RegisterDescriptor(startReg.majorNumber(), regNum, 0, startReg.nBits()), result);
        }

        // If no bytes are transferred then the contents of register RT is undefined.
        BaseSemantics::SValuePtr undef = ops->undefined_(d->addressWidth());
        BaseSemantics::SValuePtr curValue = ops->readRegister(startReg);
        ops->writeRegister(startReg, ops->ite(ops->equalToZero(nBytes), undef, curValue));
    }
};

// Load word algebraic
struct IP_lwa: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->write(args[0], ops->signExtend(d->read(args[1], 32), d->addressWidth()));
    }
};

// Load word algebraic indexed
struct IP_lwax: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->write(args[0], ops->signExtend(d->read(args[1], 32), d->addressWidth()));
    }
};

// Load word algebraic with update indexed
struct IP_lwaux: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->write(args[0], ops->signExtend(d->readAndUpdate(ops, args[1], 32), d->addressWidth()));
    }
};

// Load word byte reverse indexed
struct IP_lwbrx: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr value = d->read(args[1], 32);
        BaseSemantics::SValuePtr swapped = ops->concat(ops->concat(ops->extract(value, 24, 32), ops->extract(value, 16, 24)),
                                                       ops->concat(ops->extract(value, 8, 16), ops->extract(value, 0, 8)));
        d->write(args[0], ops->unsignedExtend(swapped, d->addressWidth()));
    }
};

// Load word and zero
struct IP_lwz: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->write(args[0], ops->unsignedExtend(d->read(args[1], 32), d->addressWidth()));
    }
};

// Load word and zero with update
struct IP_lwzu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->write(args[0], ops->unsignedExtend(d->readAndUpdate(ops, args[1], 32), d->addressWidth()));
    }
};

// Move condition register field
struct IP_mcrf: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr value = d->read(args[1], 4);
        d->write(args[0], value);
    }
};

// Move from condition register
struct IP_mfcr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        d->write(args[0], ops->unsignedExtend(ops->readRegister(d->REG_CR), d->addressWidth()));
    }
};

struct IP_mfspr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr src = d->read(args[1], args[1]->get_type()->get_nBits());// SPR in its native size
        d->write(args[0], ops->unsignedExtend(src, d->addressWidth()));
    }
};

// Copies the value from the second argument to the first argument.  This is used for a variety of instructions.
// FIXME[Robb Matzke 2019-08-08]: replace this with instruction-specific versions.
struct IP_move: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->write(args[0], d->read(args[1]));
    }
};

// Move to special purpose register
struct IP_mtspr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr src = d->read(args[1], args[0]->get_type()->get_nBits()); // truncate GPR to width of SPR
        d->write(args[0], src);
    }
};

// Multiply high doubleword
struct IP_mulhd: P {
    UpdateCr::Flag updateCr;
    explicit IP_mulhd(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], 64);
        BaseSemantics::SValuePtr b = d->read(args[2], 64);
        BaseSemantics::SValuePtr product = ops->signedMultiply(a, b);
        BaseSemantics::SValuePtr result = ops->extract(product, 64, 128);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Multiply high doubleword
struct IP_mulhdu: P {
    UpdateCr::Flag updateCr;
    explicit IP_mulhdu(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], 64);
        BaseSemantics::SValuePtr b = d->read(args[2], 64);
        BaseSemantics::SValuePtr product = ops->unsignedMultiply(a, b);
        BaseSemantics::SValuePtr result = ops->extract(product, 64, 128);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Multiply high word
struct IP_mulhw: P {
    UpdateCr::Flag updateCr;
    explicit IP_mulhw(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], 32);
        BaseSemantics::SValuePtr b = d->read(args[2], 32);
        BaseSemantics::SValuePtr product = ops->signedMultiply(a, b);
        BaseSemantics::SValuePtr result = ops->extract(product, 32, 64);
        if (d->addressWidth() == 64)
            result = ops->concat(result, ops->undefined_(d->addressWidth() - 32));
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Multiply high word unsigned
struct IP_mulhwu: P {
    UpdateCr::Flag updateCr;
    explicit IP_mulhwu(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], 32);
        BaseSemantics::SValuePtr b = d->read(args[2], 32);
        BaseSemantics::SValuePtr product = ops->unsignedMultiply(a, b);
        BaseSemantics::SValuePtr result = ops->extract(product, 32, 64);
        if (d->addressWidth() == 64)
            result = ops->concat(result, ops->undefined_(d->addressWidth() - 32));
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Multiply low doubleword
struct IP_mulld: P {
    UpdateCr::Flag updateCr;
    explicit IP_mulld(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], 32);
        BaseSemantics::SValuePtr b = d->read(args[2], 32);
        BaseSemantics::SValuePtr result = ops->extract(ops->signedMultiply(a, b), 0, 32);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Multiply low doubleword with overflow
struct IP_mulldo: P {
    UpdateCr::Flag updateCr;
    explicit IP_mulldo(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], 32);
        BaseSemantics::SValuePtr b = d->read(args[2], 32);
        BaseSemantics::SValuePtr product = ops->signedMultiply(a, b);
        BaseSemantics::SValuePtr result = ops->extract(product, 0, 32);
        d->write(args[0], result);
        d->setXerOverflow(ops->invert(ops->equalToZero(ops->extract(result, 32, 64))));
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Multiply low immediate
struct IP_mulli: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr result = ops->extract(ops->signedMultiply(a, b), 0, d->addressWidth());
        d->write(args[0], result);
    }
};

// Multiply low word
struct IP_mullw: P {
    UpdateCr::Flag updateCr;
    explicit IP_mullw(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], 32);
        BaseSemantics::SValuePtr b = d->read(args[2], 32);
        BaseSemantics::SValuePtr result = ops->extract(ops->signedMultiply(a, b), 0, d->addressWidth());
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Multiply low word with overflow
struct IP_mullwo: P {
    UpdateCr::Flag updateCr;
    explicit IP_mullwo(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], 32);
        BaseSemantics::SValuePtr b = d->read(args[2], 32);
        BaseSemantics::SValuePtr product = ops->signedMultiply(a, b);
        BaseSemantics::SValuePtr result = ops->extract(product, 0, d->addressWidth());
        d->write(args[0], result);
        d->setXerOverflow(ops->invert(ops->equalToZero(ops->extract(product, 32, 64))));
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Move to condition register fields
struct IP_mtcrf: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr mask = d->read(args[0], 8);
        BaseSemantics::SValuePtr newFields = d->read(args[1], 32);
        BaseSemantics::SValuePtr oldFields = ops->readRegister(d->REG_CR);
        BaseSemantics::SValuePtr result;
        for (size_t i = 0; i < 8; ++i) {
            BaseSemantics::SValuePtr newField = ops->extract(newFields, i*4, i*4+4);
            BaseSemantics::SValuePtr oldField = ops->extract(oldFields, i*4, i*4+4);
            BaseSemantics::SValuePtr selected = ops->extract(mask, i, i+1);
            BaseSemantics::SValuePtr field = ops->ite(selected, newField, oldField);
            result = result ? ops->concat(result, field) : field;
        }
        ASSERT_not_null(result);
        ASSERT_require(result->get_width() == 32);
        ops->writeRegister(d->REG_CR, result);
    }
};

// Fixed-point logical NAND
struct IP_nand: P {
    UpdateCr::Flag updateCr;
    explicit IP_nand(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr result = ops->invert(ops->and_(a, b));
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point negation
struct IP_neg: P {
    UpdateCr::Flag updateCr;
    explicit IP_neg(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr result = ops->negate(d->read(args[1], d->addressWidth()));
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point negation with overflow
struct IP_nego: P {
    UpdateCr::Flag updateCr;
    explicit IP_nego(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr a = ops->invert(d->read(args[1], d->addressWidth()));
        BaseSemantics::SValuePtr b = ops->number_(d->addressWidth(), 1);
        BaseSemantics::SValuePtr carryOut, overflow;
        BaseSemantics::SValuePtr result = ops->addCarry(a, b, carryOut, overflow);
        d->write(args[0], result);
        d->setXerOverflow(overflow);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point logical NOR
struct IP_nor: P {
    UpdateCr::Flag updateCr;
    explicit IP_nor(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr result = ops->invert(ops->or_(a, b));
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point logical OR
struct IP_or: P {
    UpdateCr::Flag updateCr;
    explicit IP_or(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr result = ops->or_(a, b);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point logical OR with complement
struct IP_orc: P {
    UpdateCr::Flag updateCr;
    explicit IP_orc(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr result = ops->or_(a, ops->invert(b));
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point logocal OR immediate
struct IP_ori: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr result = ops->or_(a, b);
        d->write(args[0], result);
    }
};

// Fixed-point logical OR shifted
struct IP_oris: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], 16);
        BaseSemantics::SValuePtr bShifted = ops->unsignedExtend(ops->concat(ops->number_(16, 0), b), d->addressWidth());
        BaseSemantics::SValuePtr result = ops->or_(a, bShifted);
        d->write(args[0], result);
    }
};

// Population count bytes
struct IP_popcntb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr zero = ops->number_(8, 0);
        BaseSemantics::SValuePtr one = ops->number_(8, 1);
        BaseSemantics::SValuePtr a = d->read(args[0], d->addressWidth());
        BaseSemantics::SValuePtr result;
        for (size_t byteIdx = 0; byteIdx < d->addressWidth()/8; ++byteIdx) {
            BaseSemantics::SValuePtr bytePopCnt = ops->number_(8, 0);
            for (size_t i=0; i<8; ++i) {
                size_t bitIdx = byteIdx * 8 + i;
                bytePopCnt = ops->add(bytePopCnt, ops->ite(ops->extract(a, bitIdx, bitIdx+1), one, zero));
            }
            result = result ? ops->concat(result, bytePopCnt) : bytePopCnt;
        }
        d->write(args[0], result);
    }
};

// Rotate left doubleword then clear left
struct IP_rldcl: P {
    UpdateCr::Flag updateCr;
    explicit IP_rldcl(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        BaseSemantics::SValuePtr src = d->read(args[1], 64);
        BaseSemantics::SValuePtr amount = d->read(args[2], 6); // not 64
        BaseSemantics::SValuePtr rotated = ops->rotateLeft(src, amount);
        size_t mb = d->read(args[3])->get_number();
        BaseSemantics::SValuePtr mask = ops->number_(64, buildMask(mb, 63, 64));
        BaseSemantics::SValuePtr result = ops->and_(rotated, mask);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Rotate left doubleword then clear right
struct IP_rldcr: P {
    UpdateCr::Flag updateCr;
    explicit IP_rldcr(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        BaseSemantics::SValuePtr src = d->read(args[1], 64);
        BaseSemantics::SValuePtr amount = d->read(args[2], 6); // not 64
        BaseSemantics::SValuePtr rotated = ops->rotateLeft(src, amount);
        size_t me = d->read(args[3])->get_number();
        BaseSemantics::SValuePtr mask = ops->number_(64, buildMask(0, me, 64));
        BaseSemantics::SValuePtr result = ops->and_(rotated, mask);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Rotate left doubleword immediate then clear
struct IP_rldic: P {
    UpdateCr::Flag updateCr;
    explicit IP_rldic(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        BaseSemantics::SValuePtr src = d->read(args[1], 64);
        BaseSemantics::SValuePtr amount = d->read(args[2], 6);
        BaseSemantics::SValuePtr rotated = ops->rotateLeft(src, amount);
        size_t mb = d->read(args[3], 6)->get_number();
        size_t me = 63 - amount->get_number();
        BaseSemantics::SValuePtr mask = ops->number_(64, buildMask(mb, me, 64));
        BaseSemantics::SValuePtr result = ops->and_(rotated, mask);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Rotate left doubleword immediate then clear left
struct IP_rldicl: P {
    UpdateCr::Flag updateCr;
    explicit IP_rldicl(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        BaseSemantics::SValuePtr src = d->read(args[1], 64);
        BaseSemantics::SValuePtr amount = d->read(args[2], 6);
        BaseSemantics::SValuePtr rotated = ops->rotateLeft(src, amount);
        size_t mb = d->read(args[3], 6)->get_number();
        BaseSemantics::SValuePtr mask = ops->number_(64, buildMask(mb, 63, 64));
        BaseSemantics::SValuePtr result = ops->and_(rotated, mask);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Rotate left doubleword immediate then clear right
struct IP_rldicr: P {
    UpdateCr::Flag updateCr;
    explicit IP_rldicr(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        BaseSemantics::SValuePtr src = d->read(args[1], 64);
        BaseSemantics::SValuePtr amount = d->read(args[2], 6);
        BaseSemantics::SValuePtr rotated = ops->rotateLeft(src, amount);
        size_t me = d->read(args[3], 6)->get_number();
        BaseSemantics::SValuePtr mask = ops->number_(64, buildMask(0, me, 64));
        BaseSemantics::SValuePtr result = ops->and_(rotated, mask);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Rotate left doubleword immediate then mask insert
struct IP_rldimi: P {
    UpdateCr::Flag updateCr;
    explicit IP_rldimi(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        BaseSemantics::SValuePtr src = d->read(args[1], 64);
        BaseSemantics::SValuePtr amount = d->read(args[2], 6);
        BaseSemantics::SValuePtr rotated = ops->rotateLeft(src, amount);
        size_t mb = d->read(args[3], 6)->get_number();
        size_t me = 63 - amount->get_number();
        BaseSemantics::SValuePtr mask = ops->number_(64, buildMask(mb, me, 64));
        BaseSemantics::SValuePtr origBits = d->read(args[0], 64);
        BaseSemantics::SValuePtr preserved = ops->and_(origBits, ops->invert(mask));
        BaseSemantics::SValuePtr affected = ops->and_(rotated, mask);
        BaseSemantics::SValuePtr result = ops->or_(preserved, affected);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Rotate left word immediate then mask insert
struct IP_rlwimi: P {
    UpdateCr::Flag updateCr;
    explicit IP_rlwimi(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 5);
        BaseSemantics::SValuePtr src = d->read(args[1], 32);
        BaseSemantics::SValuePtr amount = d->read(args[2], 5);
        BaseSemantics::SValuePtr rotated = ops->rotateLeft(src, amount);
        if (d->addressWidth() == 64)
            rotated = ops->concat(rotated, rotated);
        size_t maskBegin = d->read(args[3], 5)->get_number() + (d->addressWidth()-32);
        size_t maskEnd = d->read(args[4], 5)->get_number() + (d->addressWidth()-32);
        BaseSemantics::SValuePtr mask = ops->number_(d->addressWidth(), buildMask(maskBegin, maskEnd, d->addressWidth()));
        BaseSemantics::SValuePtr origBits = d->read(args[0], d->addressWidth());
        BaseSemantics::SValuePtr preserved = ops->and_(origBits, ops->invert(mask));
        BaseSemantics::SValuePtr affected = ops->and_(rotated, mask);
        BaseSemantics::SValuePtr result = ops->or_(preserved, affected);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Rotate left word immediate then AND with mask
struct IP_rlwinm: P {
    UpdateCr::Flag updateCr;
    explicit IP_rlwinm(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 5);
        BaseSemantics::SValuePtr src = d->read(args[1], 32);
        BaseSemantics::SValuePtr amount = d->read(args[2], 5);
        BaseSemantics::SValuePtr rotated = ops->rotateLeft(src, amount);
        if (d->addressWidth() == 64)
            rotated = ops->concat(rotated, rotated);
        size_t maskBegin = d->read(args[3], 5)->get_number() + (d->addressWidth()-32);
        size_t maskEnd = d->read(args[4], 5)->get_number() + (d->addressWidth()-32);
        BaseSemantics::SValuePtr mask = ops->number_(d->addressWidth(), buildMask(maskBegin, maskEnd, d->addressWidth()));
        BaseSemantics::SValuePtr result = ops->and_(rotated, mask);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Rotate left word then AND with mask
struct IP_rlwnm: P {
    UpdateCr::Flag updateCr;
    explicit IP_rlwnm(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 5);
        BaseSemantics::SValuePtr src = d->read(args[1], 32);
        BaseSemantics::SValuePtr amount = d->read(args[2], 5);
        BaseSemantics::SValuePtr rotated = ops->rotateLeft(src, amount);
        if (d->addressWidth() == 64)
            rotated = ops->concat(rotated, rotated);
        size_t maskBegin = d->read(args[3], 5)->get_number() + (d->addressWidth()-32);
        size_t maskEnd = d->read(args[4], 5)->get_number() + (d->addressWidth()-32);
        BaseSemantics::SValuePtr mask = ops->number_(d->addressWidth(), buildMask(maskBegin, maskEnd, d->addressWidth()));
        BaseSemantics::SValuePtr result = ops->and_(rotated, mask);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// System call
struct IP_sc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SgAsmIntegerValueExpression *bv = isSgAsmIntegerValueExpression(args[0]);
        ASSERT_not_null(bv);
        ops->interrupt(0, bv->get_value());
    }
};

// Shift left doubleword
struct IP_sld: P {
    UpdateCr::Flag updateCr;
    explicit IP_sld(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr src = d->read(args[1], 64);
        BaseSemantics::SValuePtr amount = d->read(args[2], 7); // 6 for shifting, 1 for invalidating the result
        BaseSemantics::SValuePtr shifted = ops->shiftLeft(src, ops->unsignedExtend(amount, 6));
        BaseSemantics::SValuePtr zero = ops->number_(64, 0);
        BaseSemantics::SValuePtr result = ops->ite(ops->extract(amount, 6, 7), zero, shifted);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Shift left word
struct IP_slw: P {
    UpdateCr::Flag updateCr;
    explicit IP_slw(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr src = d->read(args[1], 32);
        BaseSemantics::SValuePtr amount = d->read(args[2], 6); // 5 for shifting, 1 for invalidating the result
        BaseSemantics::SValuePtr shifted = ops->unsignedExtend(ops->shiftLeft(src, ops->unsignedExtend(amount, 5)),
                                                               d->addressWidth());
        BaseSemantics::SValuePtr zero = ops->number_(d->addressWidth(), 0);
        BaseSemantics::SValuePtr result = ops->ite(ops->extract(amount, 5, 6), zero, shifted);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Shift right algebraic doubleword
struct IP_srad: P {
    UpdateCr::Flag updateCr;
    explicit IP_srad(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);

        // Primary result = source >> amount
        BaseSemantics::SValuePtr src = d->read(args[1], 64);
        BaseSemantics::SValuePtr amount6 = d->read(args[2], 6);
        BaseSemantics::SValuePtr shifted = ops->shiftRightArithmetic(src, amount6);

        // Special case when amount >= 64
        BaseSemantics::SValuePtr isLargeShift = ops->extract(d->read(args[2], 7), 6, 7);
        BaseSemantics::SValuePtr srcIsNegative = ops->extract(src, 63, 64);
        BaseSemantics::SValuePtr zero = ops->number_(64, 0);
        BaseSemantics::SValuePtr negOne = ops->invert(zero);

        BaseSemantics::SValuePtr result = ops->ite(isLargeShift, ops->ite(srcIsNegative, negOne, zero), shifted);
        d->write(args[0], result);

        // Adjust the XER CA bit. If the shift amount is >= 64, then CA is the same as the src sign bit. Otherwise
        // CA is true if the src is negative or bits were shifted out of the LSB position
        BaseSemantics::SValuePtr shiftOutMask = ops->invert(ops->shiftLeft(ops->number_(64, -1), amount6));
        BaseSemantics::SValuePtr shiftOutBits = ops->and_(src, shiftOutMask);
        BaseSemantics::SValuePtr hasBitsShiftedOut = ops->invert(ops->equalToZero(shiftOutBits));
        BaseSemantics::SValuePtr ca = ops->or_(ops->and_(isLargeShift, srcIsNegative),
                                               ops->and_(ops->invert(isLargeShift),
                                                         ops->or_(srcIsNegative, hasBitsShiftedOut)));
        ops->writeRegister(d->REG_XER_CA, ca);

        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Shift right algebraic doubleword immediate
struct IP_sradi: P {
    UpdateCr::Flag updateCr;
    explicit IP_sradi(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);

        // Primary result = source >> amount
        BaseSemantics::SValuePtr src = d->read(args[1], 64);
        BaseSemantics::SValuePtr amount = d->read(args[2], 6);
        BaseSemantics::SValuePtr result = ops->shiftRightArithmetic(src, amount);
        d->write(args[0], result);

        // Adjust the XER CA bit. CA is set if the source is negative and any 1 bits are shifted out.
        BaseSemantics::SValuePtr isNegative = ops->extract(src, 63, 64);
        BaseSemantics::SValuePtr shiftOutMask = ops->invert(ops->shiftLeft(ops->number_(64, -1), amount));
        BaseSemantics::SValuePtr shiftOutBits = ops->and_(src, shiftOutMask);
        BaseSemantics::SValuePtr hasBitsShiftedOut = ops->invert(ops->equalToZero(shiftOutBits));
        BaseSemantics::SValuePtr ca = ops->and_(isNegative, hasBitsShiftedOut);
        ops->writeRegister(d->REG_XER_CA, ca);

        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Shift right algebraic word
struct IP_sraw: P {
    UpdateCr::Flag updateCr;
    explicit IP_sraw(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);

        // Primary result = source >> amount
        BaseSemantics::SValuePtr src = d->read(args[1], 32);
        BaseSemantics::SValuePtr amount5 = d->read(args[2], 5);
        BaseSemantics::SValuePtr shifted = ops->signExtend(ops->shiftRightArithmetic(src, amount5), d->addressWidth());

        // Special case when amount >= 32
        BaseSemantics::SValuePtr isLargeShift = ops->extract(d->read(args[2], 6), 5, 6);
        BaseSemantics::SValuePtr srcIsNegative = ops->extract(src, 31, 32);
        BaseSemantics::SValuePtr zero = ops->number_(d->addressWidth(), 0);
        BaseSemantics::SValuePtr negOne = ops->invert(zero);

        BaseSemantics::SValuePtr result = ops->ite(isLargeShift, ops->ite(srcIsNegative, negOne, zero), shifted);
        d->write(args[0], result);

        // Adjust the XER CA bit. If the shift amount is >= 32, then CA is the same as the src sign bit. Otherwise
        // CA is true if the src is negative or bits where shifted out of the LSB position.
        BaseSemantics::SValuePtr shiftOutMask = ops->invert(ops->shiftLeft(ops->number_(32, -1), amount5));
        BaseSemantics::SValuePtr shiftOutBits = ops->and_(src, shiftOutMask);
        BaseSemantics::SValuePtr hasBitsShiftedOut = ops->invert(ops->equalToZero(shiftOutBits));
        BaseSemantics::SValuePtr ca = ops->or_(ops->and_(isLargeShift, srcIsNegative),
                                               ops->and_(ops->invert(isLargeShift),
                                                         ops->or_(srcIsNegative, hasBitsShiftedOut)));
        ops->writeRegister(d->REG_XER_CA, ca);

        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Shift right algebraic word immediate
struct IP_srawi: P {
    UpdateCr::Flag updateCr;
    explicit IP_srawi(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);

        // Primary result = source << amount
        BaseSemantics::SValuePtr src = d->read(args[1], 32);
        BaseSemantics::SValuePtr amount = d->read(args[2], 5);
        BaseSemantics::SValuePtr result = ops->signExtend(ops->shiftRightArithmetic(src, amount), d->addressWidth());
        d->write(args[0], result);

        // XER CA bit is set if the 32-bit source is negative and any 1-bits are shifted out; otherwise cleared
        BaseSemantics::SValuePtr rsIsNegative = ops->extract(src, 31, 32);
        BaseSemantics::SValuePtr shiftOutMask = ops->invert(ops->shiftLeft(ops->number_(32, -1), amount));
        BaseSemantics::SValuePtr shiftOutBits = ops->and_(src, shiftOutMask);
        BaseSemantics::SValuePtr hasBitsShiftedOut = ops->invert(ops->equalToZero(shiftOutBits));
        BaseSemantics::SValuePtr ca = ops->or_(rsIsNegative, hasBitsShiftedOut);
        ops->writeRegister(d->REG_XER_CA, ca);

        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Shift right doubleword
struct IP_srd: P {
    UpdateCr::Flag updateCr;
    explicit IP_srd(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr src = d->read(args[1], 64);
        BaseSemantics::SValuePtr amount = d->read(args[2], 7); // 6 for shifting, 1 for invalidating the result
        BaseSemantics::SValuePtr shifted = ops->shiftRight(src, ops->unsignedExtend(amount, 6));
        BaseSemantics::SValuePtr zero = ops->number_(64, 0);
        BaseSemantics::SValuePtr result = ops->ite(ops->extract(amount, 6, 7), zero, shifted);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Shift right word
struct IP_srw: P {
    UpdateCr::Flag updateCr;
    explicit IP_srw(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr src = d->read(args[1], 32);
        BaseSemantics::SValuePtr amount = d->read(args[2], 6); // 5 for shifting, 1 for invalidating the result
        BaseSemantics::SValuePtr shifted = ops->unsignedExtend(ops->shiftRight(src, ops->unsignedExtend(amount, 5)),
                                                               d->addressWidth());
        BaseSemantics::SValuePtr zero = ops->number_(d->addressWidth(), 0);
        BaseSemantics::SValuePtr result = ops->ite(ops->extract(amount, 5, 6), zero, shifted);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Store byte
struct IP_stb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->write(args[1], d->read(args[0], 8));
    }
};

// Store byte with update
struct IP_stbu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->writeAndUpdate(ops, args[1], d->read(args[0], 8));
    }
};

// Store byte with update indexed
struct IP_stbux: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->writeAndUpdate(ops, args[1], d->read(args[0], 8));
    }
};

// Store doubleword
struct IP_std: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->write(args[1], d->read(args[0], 64));
    }
};

// Store doubleword with update
struct IP_stdu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->writeAndUpdate(ops, args[1], d->read(args[0], 64));
    }
};

// Store doubleword indexed
struct IP_stdx: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->write(args[1], d->read(args[0], 64));
    }
};

// Store doubleword with update indexed
struct IP_stdux: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->writeAndUpdate(ops, args[1], d->read(args[0], 64));
    }
};

// Store floating-point double
struct IP_stfd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->write(args[1], d->read(args[0], 64));
    }
};

// Store floating-point single
struct IP_stfs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SgAsmFloatType *srcType = SageBuilderAsm::buildIeee754Binary64();
        BaseSemantics::SValuePtr src = ops->reinterpret(d->read(args[0], 64), srcType);
        BaseSemantics::SValuePtr single = ops->fpConvert(src, srcType, SageBuilderAsm::buildIeee754Binary32());
        d->write(args[1], single);
    }
};

// Store halfword
struct IP_sth: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->write(args[1], d->read(args[0], 16));
    }
};

// Store halfword byte reversed indexed
struct IP_sthbrx: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr value = d->read(args[0], 16);
        BaseSemantics::SValuePtr swapped = ops->concat(ops->extract(value, 8, 16), ops->extract(value, 0, 8));
        d->write(args[1], swapped);
    }
};

// Store halfword with update
struct IP_sthu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->writeAndUpdate(ops, args[1], d->read(args[0], 16));
    }
};

// Store halfword with update indexed
struct IP_sthux: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->writeAndUpdate(ops, args[1], d->read(args[0], 16));
    }
};

// Store halfword indexed
struct IP_sthx: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->write(args[1], d->read(args[0], 16));
    }
};

// Store multiple word
struct IP_stmw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr base = d->effectiveAddress(args[1], 32);
        SgAsmRegisterReferenceExpression *rs = isSgAsmRegisterReferenceExpression(args[0]);
        ASSERT_require(rs && rs->get_descriptor().majorNumber() == powerpc_regclass_gpr);
        RegisterDescriptor reg = rs->get_descriptor();
        rose_addr_t offset = 0;
        for (unsigned minor=reg.minorNumber(); minor<32; minor+=1, offset+=4) {
            BaseSemantics::SValuePtr addr = ops->add(base, ops->number_(32, offset));
            reg.minorNumber(minor);
            BaseSemantics::SValuePtr value = ops->unsignedExtend(ops->readRegister(reg), 32);
            ops->writeMemory(RegisterDescriptor(), addr, value, ops->boolean_(true));
        }
    }
};

// Store string word immediate
struct IP_stswi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        const BaseSemantics::SValuePtr baseAddr = d->read(args[1], 32);
        size_t memOffset = 0;

        ASSERT_not_null(isSgAsmIntegerValueExpression(args[2]));
        size_t nBytesToTransfer = isSgAsmIntegerValueExpression(args[2])->get_absoluteValue();
        ASSERT_require(nBytesToTransfer > 0 && nBytesToTransfer <= 32);

        ASSERT_not_null(isSgAsmDirectRegisterExpression(args[0]));
        const RegisterDescriptor baseReg = isSgAsmDirectRegisterExpression(args[0])->get_descriptor();

        const size_t nRegistersAffected = (nBytesToTransfer + 3) / 4;
        for (size_t i = 0; i < nRegistersAffected; ++i) {
            const size_t nBytes = std::min(nBytesToTransfer, size_t(4));
            const size_t regNum = (baseReg.minorNumber() + i) % 32;
            BaseSemantics::SValuePtr value = ops->readRegister(RegisterDescriptor(baseReg.majorNumber(), regNum, 0, 32));
            if (nBytes < 4)
                value = ops->extract(value, 8*(4-nBytes), 32);

            const BaseSemantics::SValuePtr ea = ops->add(baseAddr, ops->number_(32, memOffset));
            ops->writeMemory(RegisterDescriptor(), ea, value, ops->boolean_(true)); // big endian

            memOffset += 4;
            nBytesToTransfer -= nBytes;
        }
    }
};

// Store string word indexed
struct IP_stswx: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr baseAddr = d->read(args[1], 32);
        BaseSemantics::SValuePtr nBytes = ops->extract(ops->readRegister(d->REG_XER), 0, 7); // 7 bits wide
        ASSERT_require(isSgAsmDirectRegisterExpression(args[0]));
        RegisterDescriptor startReg = isSgAsmDirectRegisterExpression(args[0])->get_descriptor();
        for (size_t i = 0; i < 32; ++i) {
            size_t regNum = (startReg.minorNumber() + i) % 32;

            // Obtain the value to write into memory. The value comes from two places:
            //   1. If the byte counter (memory offset from base address) is less than the total number of bytes to
            //      copy, then the value comes from reading the register.
            //   2. Else the value comes from reading the destination memory address (i.e., no data transfer)
            for (size_t j = 0; j < 4; ++j) {
                // Read a byte from memory
                size_t memOffset = i*4 + j;
                BaseSemantics::SValuePtr ea = ops->add(baseAddr, ops->number_(32, memOffset));
                BaseSemantics::SValuePtr memByte = ops->readMemory(RegisterDescriptor(), ea,
                                                                   ops->undefined_(8), ops->boolean_(true));

                // Read a byte from the register
                size_t regOffset = 32 - (j+1)*8; // stupid PPC bit numbering
                RegisterDescriptor curReg(startReg.majorNumber(), regNum, regOffset, 8);
                BaseSemantics::SValuePtr regByte = ops->readRegister(curReg);

                // Decide what value to write to the memory and do so.
                BaseSemantics::SValuePtr doXfer = ops->isUnsignedLessThan(ops->number_(7, memOffset), nBytes);
                BaseSemantics::SValuePtr byte = ops->ite(doXfer, regByte, memByte);
                ops->writeMemory(RegisterDescriptor(), ea, byte, ops->boolean_(true));
            }
        }
    }
};

// Store word
struct IP_stw: P {
    UpdateCr::Flag updateCr;
    explicit IP_stw(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr result = d->read(args[0], 32);
        d->write(args[1], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Store word byte-reversed indexed
struct IP_stwbrx: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr value = d->read(args[0], 32);
        BaseSemantics::SValuePtr swapped = ops->concat(ops->concat(ops->extract(value, 24, 32),
                                                                   ops->extract(value, 16, 24)),
                                                       ops->concat(ops->extract(value, 8, 16),
                                                                   ops->extract(value, 0, 8)));
        d->write(args[1], swapped);
    }
};

// Store word with update
struct IP_stwu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->writeAndUpdate(ops, args[1], d->read(args[0], 32));
    }
};

// Store word with update indexed
struct IP_stwux: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->writeAndUpdate(ops, args[1], d->read(args[0], 32));
    }
};

// Fixed point subtract
struct IP_subf: P {
    UpdateCr::Flag updateCr;
    explicit IP_subf(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr subtrahend = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr minuend = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr result = ops->subtract(minuend, subtrahend);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point subtract from carrying
struct IP_subfc: P {
    UpdateCr::Flag updateCr;
    explicit IP_subfc(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr subtrahend = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr minuend = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr carryOut, overflow;
        BaseSemantics::SValuePtr result = ops->subtractCarry(minuend, subtrahend, carryOut, overflow);
        d->write(args[0], result);
        ops->writeRegister(d->REG_XER_CA, carryOut);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point subtract from carrying with overflow
struct IP_subfco: P {
    UpdateCr::Flag updateCr;
    explicit IP_subfco(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr subtrahend = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr minuend = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr carryOut, overflow;
        BaseSemantics::SValuePtr result = ops->subtractCarry(minuend, subtrahend, carryOut, overflow);
        d->write(args[0], result);
        ops->writeRegister(d->REG_XER_CA, carryOut);
        d->setXerOverflow(overflow);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point subtract from extended
struct IP_subfe: P {
    UpdateCr::Flag updateCr;
    explicit IP_subfe(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr subtrahend = ops->invert(d->read(args[1], d->addressWidth())); // not negate
        BaseSemantics::SValuePtr minuend = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr c = ops->readRegister(d->REG_XER_CA);
        BaseSemantics::SValuePtr carries;
        BaseSemantics::SValuePtr result = ops->addWithCarries(minuend, subtrahend, c, carries /*out*/);
        BaseSemantics::SValuePtr carryOut = ops->extract(carries, d->addressWidth()-1, d->addressWidth());
        d->write(args[0], result);
        ops->writeRegister(d->REG_XER_CA, carryOut);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point subtract from extended with overflow
struct IP_subfeo: P {
    UpdateCr::Flag updateCr;
    explicit IP_subfeo(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr subtrahend = ops->invert(d->read(args[1], d->addressWidth())); // not negate
        BaseSemantics::SValuePtr minuend = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr c = ops->readRegister(d->REG_XER_CA);
        BaseSemantics::SValuePtr carries;
        BaseSemantics::SValuePtr result = ops->addWithCarries(minuend, subtrahend, c, carries /*out*/);
        BaseSemantics::SValuePtr carryOut = ops->extract(carries, d->addressWidth()-1, d->addressWidth());
        BaseSemantics::SValuePtr carryOut2 = ops->extract(carries, d->addressWidth()-2, d->addressWidth()-1);
        BaseSemantics::SValuePtr overflow = ops->xor_(carryOut, carryOut2);
        d->write(args[0], result);
        ops->writeRegister(d->REG_XER_CA, carryOut);
        d->setXerOverflow(overflow);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point subtract from immediate carrying
struct IP_subfic: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr subtrahend = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr minuend = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr carryOut, overflow;
        BaseSemantics::SValuePtr result = ops->subtractCarry(minuend, subtrahend, carryOut, overflow);
        d->write(args[0], result);
        ops->writeRegister(d->REG_XER_CA, carryOut);
    }
};

// Fixed point subtract from minus one extended
struct IP_subfme: P {
    UpdateCr::Flag updateCr;
    explicit IP_subfme(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr subtrahend = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr minuend = ops->invert(ops->number_(d->addressWidth(), 0)); // -1
        BaseSemantics::SValuePtr c = ops->readRegister(d->REG_XER_CA);
        BaseSemantics::SValuePtr carries;
        BaseSemantics::SValuePtr result = ops->addWithCarries(minuend, subtrahend, c, carries /*out*/);
        BaseSemantics::SValuePtr carryOut = ops->extract(carries, d->addressWidth()-1, d->addressWidth());
        d->write(args[0], result);
        ops->writeRegister(d->REG_XER_CA, carryOut);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed point subtract from minus one extended with overflow
struct IP_subfmeo: P {
    UpdateCr::Flag updateCr;
    explicit IP_subfmeo(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr subtrahend = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr minuend = ops->invert(ops->number_(d->addressWidth(), 0)); // -1
        BaseSemantics::SValuePtr c = ops->readRegister(d->REG_XER_CA);
        BaseSemantics::SValuePtr carries;
        BaseSemantics::SValuePtr result = ops->addWithCarries(minuend, subtrahend, c, carries /*out*/);
        BaseSemantics::SValuePtr carryOut = ops->extract(carries, d->addressWidth()-1, d->addressWidth());
        BaseSemantics::SValuePtr carryOut2 = ops->extract(carries, d->addressWidth()-2, d->addressWidth()-1);
        BaseSemantics::SValuePtr overflow = ops->xor_(carryOut, carryOut2);
        d->write(args[0], result);
        ops->writeRegister(d->REG_XER_CA, carryOut);
        d->setXerOverflow(overflow);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed point subtract with overflow
struct IP_subfo: P {
    UpdateCr::Flag updateCr;
    explicit IP_subfo(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr subtrahend = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr minuend = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr carryOut, overflow;
        BaseSemantics::SValuePtr result = ops->subtractCarry(minuend, subtrahend, carryOut, overflow);
        d->write(args[0], result);
        d->setXerOverflow(overflow);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point subtract from zero extended
struct IP_subfze: P {
    UpdateCr::Flag updateCr;
    explicit IP_subfze(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr a = ops->invert(d->read(args[1], d->addressWidth())); // not negate
        BaseSemantics::SValuePtr b = ops->unsignedExtend(ops->readRegister(d->REG_XER_CA), d->addressWidth());
        BaseSemantics::SValuePtr carryOut, overflow;
        BaseSemantics::SValuePtr result = ops->addCarry(a, b, carryOut, overflow);
        d->write(args[0], result);
        ops->writeRegister(d->REG_XER_CA, carryOut);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point subtract from zero extended with overflow
struct IP_subfzeo: P {
    UpdateCr::Flag updateCr;
    explicit IP_subfzeo(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        BaseSemantics::SValuePtr a = ops->invert(d->read(args[1], d->addressWidth())); // not negate
        BaseSemantics::SValuePtr b = ops->unsignedExtend(ops->readRegister(d->REG_XER_CA), d->addressWidth());
        BaseSemantics::SValuePtr carryOut, overflow;
        BaseSemantics::SValuePtr result = ops->addCarry(a, b, carryOut, overflow);
        d->write(args[0], result);
        ops->writeRegister(d->REG_XER_CA, carryOut);
        d->setXerOverflow(overflow);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Trap doubleword; Trap doubleword immediate
struct IP_td: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        ASSERT_require(isSgAsmIntegerValueExpression(args[0]));
        unsigned comparison = isSgAsmIntegerValueExpression(args[0])->get_absoluteValue();
        BaseSemantics::SValuePtr a = d->read(args[1], 64);
        BaseSemantics::SValuePtr b = d->read(args[2], 64);
        BaseSemantics::SValuePtr doTrap = ops->boolean_(false);

        doTrap = (comparison & 0x01) == 0 ? doTrap :
                 ops->or_(doTrap, ops->isSignedLessThan(a, b));
        doTrap = (comparison & 0x02) == 0 ? doTrap :
                 ops->or_(doTrap, ops->isSignedGreaterThan(a, b));
        doTrap = (comparison & 0x04) == 0 ? doTrap :
                 ops->or_(doTrap, ops->isEqual(a, b));
        doTrap = (comparison & 0x08) == 0 ? doTrap :
                 ops->or_(doTrap, ops->isUnsignedLessThan(a, b));
        doTrap = (comparison & 0x10) == 0 ? doTrap :
                 ops->or_(doTrap, ops->isUnsignedGreaterThan(a, b));

        BaseSemantics::SValuePtr target = ops->ite(doTrap, ops->number_(64, 0x700), ops->readRegister(d->REG_IAR));
        ops->writeRegister(d->REG_IAR, target);
    }
};

// Trap word; Trap word immediate
struct IP_tw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        ASSERT_require(isSgAsmIntegerValueExpression(args[0]));
        unsigned comparison = isSgAsmIntegerValueExpression(args[0])->get_absoluteValue();
        BaseSemantics::SValuePtr a = d->read(args[1], 32);
        BaseSemantics::SValuePtr b = d->read(args[2], 32);
        BaseSemantics::SValuePtr doTrap = ops->boolean_(false);

        doTrap = (comparison & 0x01) == 0 ? doTrap :
                 ops->or_(doTrap, ops->isSignedLessThan(a, b));
        doTrap = (comparison & 0x02) == 0 ? doTrap :
                 ops->or_(doTrap, ops->isSignedGreaterThan(a, b));
        doTrap = (comparison & 0x04) == 0 ? doTrap :
                 ops->or_(doTrap, ops->isEqual(a, b));
        doTrap = (comparison & 0x08) == 0 ? doTrap :
                 ops->or_(doTrap, ops->isUnsignedLessThan(a, b));
        doTrap = (comparison & 0x10) == 0 ? doTrap :
                 ops->or_(doTrap, ops->isUnsignedGreaterThan(a, b));

        BaseSemantics::SValuePtr target = ops->ite(doTrap, ops->number_(d->addressWidth(), 0x700), ops->readRegister(d->REG_IAR));
        ops->writeRegister(d->REG_IAR, target);
    }
};

// Fixed-point logical XOR
struct IP_xor: P {
    UpdateCr::Flag updateCr;
    explicit IP_xor(UpdateCr::Flag updateCr): updateCr(updateCr) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr result = ops->xor_(a, b);
        d->write(args[0], result);
        if (UpdateCr::YES == updateCr)
            d->updateCr0(result);
    }
};

// Fixed-point logical XOR immediate
struct IP_xori: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], d->addressWidth());
        BaseSemantics::SValuePtr result = ops->xor_(a, b);
        d->write(args[0], result);
    }
};

// Fixed-point logical XOR shifted
struct IP_xoris: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        BaseSemantics::SValuePtr a = d->read(args[1], d->addressWidth());
        BaseSemantics::SValuePtr b = d->read(args[2], 16);
        BaseSemantics::SValuePtr bShifted = ops->unsignedExtend(ops->concat(ops->number_(16, 0), b), d->addressWidth());
        BaseSemantics::SValuePtr result = ops->xor_(a, bShifted);
        d->write(args[0], result);
    }
};

} // namespace

/*******************************************************************************************************************************
 *                                      DispatcherPowerpc
 *******************************************************************************************************************************/

void
DispatcherPowerpc::iproc_init() {
    iproc_set(powerpc_addc,             new Powerpc::IP_addc(UpdateCr::NO));
    iproc_set(powerpc_addc_record,      new Powerpc::IP_addc(UpdateCr::YES));
    iproc_set(powerpc_addco,            new Powerpc::IP_addco(UpdateCr::NO));
    iproc_set(powerpc_addco_record,     new Powerpc::IP_addco(UpdateCr::YES));
    iproc_set(powerpc_adde,             new Powerpc::IP_adde(UpdateCr::NO));
    iproc_set(powerpc_adde_record,      new Powerpc::IP_adde(UpdateCr::YES));
    iproc_set(powerpc_addeo,            new Powerpc::IP_addeo(UpdateCr::NO));
    iproc_set(powerpc_addeo_record,     new Powerpc::IP_addeo(UpdateCr::YES));
    iproc_set(powerpc_addic,            new Powerpc::IP_addic(UpdateCr::NO));
    iproc_set(powerpc_addic_record,     new Powerpc::IP_addic(UpdateCr::YES));
    iproc_set(powerpc_addi,             new Powerpc::IP_addi);
    iproc_set(powerpc_addis,            new Powerpc::IP_addis);
    iproc_set(powerpc_addme,            new Powerpc::IP_addme(UpdateCr::NO));
    iproc_set(powerpc_addme_record,     new Powerpc::IP_addme(UpdateCr::YES));
    iproc_set(powerpc_addmeo,           new Powerpc::IP_addmeo(UpdateCr::NO));
    iproc_set(powerpc_addmeo_record,    new Powerpc::IP_addmeo(UpdateCr::YES));
    iproc_set(powerpc_add,              new Powerpc::IP_add(UpdateCr::NO));
    iproc_set(powerpc_add_record,       new Powerpc::IP_add(UpdateCr::YES));
    iproc_set(powerpc_addo,             new Powerpc::IP_addo(UpdateCr::NO));
    iproc_set(powerpc_addo_record,      new Powerpc::IP_addo(UpdateCr::YES));
    iproc_set(powerpc_addze,            new Powerpc::IP_addze(UpdateCr::NO));
    iproc_set(powerpc_addze_record,     new Powerpc::IP_addze(UpdateCr::YES));
    iproc_set(powerpc_addzeo,           new Powerpc::IP_addzeo(UpdateCr::NO));
    iproc_set(powerpc_addzeo_record,    new Powerpc::IP_addzeo(UpdateCr::YES));
    iproc_set(powerpc_andc,             new Powerpc::IP_andc(UpdateCr::NO));
    iproc_set(powerpc_andc_record,      new Powerpc::IP_andc(UpdateCr::YES));
    iproc_set(powerpc_andi_record,      new Powerpc::IP_andi);
    iproc_set(powerpc_andis_record,     new Powerpc::IP_andis);
    iproc_set(powerpc_and,              new Powerpc::IP_and(UpdateCr::NO));
    iproc_set(powerpc_and_record,       new Powerpc::IP_and(UpdateCr::YES));
    iproc_set(powerpc_ba,               new Powerpc::IP_ba(SaveLink::NO));
    iproc_set(powerpc_bca,              new Powerpc::IP_bc(SaveLink::NO));
    iproc_set(powerpc_bcctrl,           new Powerpc::IP_bcctr(SaveLink::YES));
    iproc_set(powerpc_bcctr,            new Powerpc::IP_bcctr(SaveLink::NO));
    iproc_set(powerpc_bcla,             new Powerpc::IP_bc(SaveLink::YES));
    iproc_set(powerpc_bcl,              new Powerpc::IP_bc(SaveLink::YES));
    iproc_set(powerpc_bclrl,            new Powerpc::IP_bclr(SaveLink::YES));
    iproc_set(powerpc_bclr,             new Powerpc::IP_bclr(SaveLink::NO));
    iproc_set(powerpc_bc,               new Powerpc::IP_bc(SaveLink::NO));
    iproc_set(powerpc_bla,              new Powerpc::IP_ba(SaveLink::YES));
    iproc_set(powerpc_bl,               new Powerpc::IP_b(SaveLink::YES));
    iproc_set(powerpc_b,                new Powerpc::IP_b(SaveLink::NO));
    iproc_set(powerpc_cmp,              new Powerpc::IP_cmp);
    iproc_set(powerpc_cmpi,             new Powerpc::IP_cmpi);
    iproc_set(powerpc_cmpl,             new Powerpc::IP_cmpl);
    iproc_set(powerpc_cmpli,            new Powerpc::IP_cmpli);
    iproc_set(powerpc_cntlzd,           new Powerpc::IP_cntlzd(UpdateCr::NO));
    iproc_set(powerpc_cntlzd_record,    new Powerpc::IP_cntlzd(UpdateCr::YES));
    iproc_set(powerpc_cntlzw,           new Powerpc::IP_cntlzw(UpdateCr::NO));
    iproc_set(powerpc_cntlzw_record,    new Powerpc::IP_cntlzw(UpdateCr::YES));
    iproc_set(powerpc_crandc,           new Powerpc::IP_crandc);
    iproc_set(powerpc_crand,            new Powerpc::IP_crand);
    iproc_set(powerpc_creqv,            new Powerpc::IP_creqv);
    iproc_set(powerpc_crnand,           new Powerpc::IP_crnand);
    iproc_set(powerpc_crnor,            new Powerpc::IP_crnor);
    iproc_set(powerpc_crorc,            new Powerpc::IP_crorc);
    iproc_set(powerpc_cror,             new Powerpc::IP_cror);
    iproc_set(powerpc_crxor,            new Powerpc::IP_crxor);
    iproc_set(powerpc_divd,             new Powerpc::IP_divd(UpdateCr::NO));
    iproc_set(powerpc_divd_record,      new Powerpc::IP_divd(UpdateCr::YES));
    iproc_set(powerpc_divdo,            new Powerpc::IP_divdo(UpdateCr::NO));
    iproc_set(powerpc_divdo_record,     new Powerpc::IP_divdo(UpdateCr::YES));
    iproc_set(powerpc_divdu,            new Powerpc::IP_divdu(UpdateCr::NO));
    iproc_set(powerpc_divdu_record,     new Powerpc::IP_divdu(UpdateCr::YES));
    iproc_set(powerpc_divduo,           new Powerpc::IP_divduo(UpdateCr::NO));
    iproc_set(powerpc_divduo_record,    new Powerpc::IP_divduo(UpdateCr::YES));
    iproc_set(powerpc_divw,             new Powerpc::IP_divw(UpdateCr::NO));
    iproc_set(powerpc_divw_record,      new Powerpc::IP_divw(UpdateCr::YES));
    iproc_set(powerpc_divwo,            new Powerpc::IP_divwo(UpdateCr::NO));
    iproc_set(powerpc_divwo_record,     new Powerpc::IP_divwo(UpdateCr::YES));
    iproc_set(powerpc_divwu,            new Powerpc::IP_divwu(UpdateCr::NO));
    iproc_set(powerpc_divwu_record,     new Powerpc::IP_divwu(UpdateCr::YES));
    iproc_set(powerpc_divwuo,           new Powerpc::IP_divwuo(UpdateCr::NO));
    iproc_set(powerpc_divwuo_record,    new Powerpc::IP_divwuo(UpdateCr::YES));
    iproc_set(powerpc_eieio,            new Powerpc::IP_eieio);
    iproc_set(powerpc_eqv,              new Powerpc::IP_eqv(UpdateCr::NO));
    iproc_set(powerpc_eqv_record,       new Powerpc::IP_eqv(UpdateCr::YES));
    iproc_set(powerpc_extsb,            new Powerpc::IP_extsb(UpdateCr::NO));
    iproc_set(powerpc_extsb_record,     new Powerpc::IP_extsb(UpdateCr::YES));
    iproc_set(powerpc_extsh,            new Powerpc::IP_extsh(UpdateCr::NO));
    iproc_set(powerpc_extsh_record,     new Powerpc::IP_extsh(UpdateCr::YES));
    iproc_set(powerpc_extsw,            new Powerpc::IP_extsw(UpdateCr::NO));
    iproc_set(powerpc_extsw_record,     new Powerpc::IP_extsw(UpdateCr::YES));
    iproc_set(powerpc_fmr,              new Powerpc::IP_move);
    iproc_set(powerpc_lbz,              new Powerpc::IP_lbz);
    iproc_set(powerpc_lbzu,             new Powerpc::IP_lbzu);
    iproc_set(powerpc_lbzux,            new Powerpc::IP_lbzu);
    iproc_set(powerpc_lbzx,             new Powerpc::IP_lbz);
    iproc_set(powerpc_ld,               new Powerpc::IP_ld);
    iproc_set(powerpc_ldu,              new Powerpc::IP_ldu);
    iproc_set(powerpc_ldux,             new Powerpc::IP_ldux);
    iproc_set(powerpc_ldx,              new Powerpc::IP_ldx);
    iproc_set(powerpc_lfd,              new Powerpc::IP_lfd);
    iproc_set(powerpc_lfs,              new Powerpc::IP_lfs);
    iproc_set(powerpc_lha,              new Powerpc::IP_lha);
    iproc_set(powerpc_lhau,             new Powerpc::IP_lhau);
    iproc_set(powerpc_lhaux,            new Powerpc::IP_lhau);
    iproc_set(powerpc_lhax,             new Powerpc::IP_lha);
    iproc_set(powerpc_lhbrx,            new Powerpc::IP_lhbrx);
    iproc_set(powerpc_lhz,              new Powerpc::IP_lhz);
    iproc_set(powerpc_lhzu,             new Powerpc::IP_lhzu);
    iproc_set(powerpc_lhzux,            new Powerpc::IP_lhzu);
    iproc_set(powerpc_lhzx,             new Powerpc::IP_lhz);
    iproc_set(powerpc_lmw,              new Powerpc::IP_lmw);
    iproc_set(powerpc_lswi,             new Powerpc::IP_lswi);
    iproc_set(powerpc_lswx,             new Powerpc::IP_lswx);
    iproc_set(powerpc_lwa,              new Powerpc::IP_lwa);
    iproc_set(powerpc_lwax,             new Powerpc::IP_lwax);
    iproc_set(powerpc_lwarx,            new Powerpc::IP_move);
    iproc_set(powerpc_lwaux,            new Powerpc::IP_lwaux);
    iproc_set(powerpc_lwbrx,            new Powerpc::IP_lwbrx);
    iproc_set(powerpc_lwz,              new Powerpc::IP_lwz);
    iproc_set(powerpc_lwzu,             new Powerpc::IP_lwzu);
    iproc_set(powerpc_lwzux,            new Powerpc::IP_lwzu);
    iproc_set(powerpc_lwzx,             new Powerpc::IP_lwz);
    iproc_set(powerpc_mcrf,             new Powerpc::IP_mcrf);
    iproc_set(powerpc_mfcr,             new Powerpc::IP_mfcr);
    iproc_set(powerpc_mfspr,            new Powerpc::IP_mfspr);
    iproc_set(powerpc_mtcrf,            new Powerpc::IP_mtcrf);
    iproc_set(powerpc_mtspr,            new Powerpc::IP_mtspr);
    iproc_set(powerpc_mulhd,            new Powerpc::IP_mulhd(UpdateCr::NO));
    iproc_set(powerpc_mulhd_record,     new Powerpc::IP_mulhd(UpdateCr::YES));
    iproc_set(powerpc_mulhdu,           new Powerpc::IP_mulhdu(UpdateCr::NO));
    iproc_set(powerpc_mulhdu_record,    new Powerpc::IP_mulhdu(UpdateCr::YES));
    iproc_set(powerpc_mulhw,            new Powerpc::IP_mulhw(UpdateCr::NO));
    iproc_set(powerpc_mulhw_record,     new Powerpc::IP_mulhw(UpdateCr::YES));
    iproc_set(powerpc_mulhwu,           new Powerpc::IP_mulhwu(UpdateCr::NO));
    iproc_set(powerpc_mulhwu_record,    new Powerpc::IP_mulhwu(UpdateCr::YES));
    iproc_set(powerpc_mulld,            new Powerpc::IP_mulld(UpdateCr::NO));
    iproc_set(powerpc_mulld_record,     new Powerpc::IP_mulld(UpdateCr::YES));
    iproc_set(powerpc_mulldo,           new Powerpc::IP_mulldo(UpdateCr::NO));
    iproc_set(powerpc_mulldo_record,    new Powerpc::IP_mulldo(UpdateCr::YES));
    iproc_set(powerpc_mulli,            new Powerpc::IP_mulli);
    iproc_set(powerpc_mullw,            new Powerpc::IP_mullw(UpdateCr::NO));
    iproc_set(powerpc_mullw_record,     new Powerpc::IP_mullw(UpdateCr::YES));
    iproc_set(powerpc_mullwo,           new Powerpc::IP_mullwo(UpdateCr::NO));
    iproc_set(powerpc_mullwo_record,    new Powerpc::IP_mullwo(UpdateCr::YES));
    iproc_set(powerpc_nand,             new Powerpc::IP_nand(UpdateCr::NO));
    iproc_set(powerpc_nand_record,      new Powerpc::IP_nand(UpdateCr::YES));
    iproc_set(powerpc_neg,              new Powerpc::IP_neg(UpdateCr::NO));
    iproc_set(powerpc_neg_record,       new Powerpc::IP_neg(UpdateCr::YES));
    iproc_set(powerpc_nego,             new Powerpc::IP_nego(UpdateCr::NO));
    iproc_set(powerpc_nego_record,      new Powerpc::IP_nego(UpdateCr::YES));
    iproc_set(powerpc_nor,              new Powerpc::IP_nor(UpdateCr::NO));
    iproc_set(powerpc_nor_record,       new Powerpc::IP_nor(UpdateCr::YES));
    iproc_set(powerpc_orc,              new Powerpc::IP_orc(UpdateCr::NO));
    iproc_set(powerpc_orc_record,       new Powerpc::IP_orc(UpdateCr::YES));
    iproc_set(powerpc_ori,              new Powerpc::IP_ori);
    iproc_set(powerpc_oris,             new Powerpc::IP_oris);
    iproc_set(powerpc_or,               new Powerpc::IP_or(UpdateCr::NO));
    iproc_set(powerpc_or_record,        new Powerpc::IP_or(UpdateCr::YES));
    iproc_set(powerpc_popcntb,          new Powerpc::IP_popcntb);
    iproc_set(powerpc_rldcl,            new Powerpc::IP_rldcl(UpdateCr::NO));
    iproc_set(powerpc_rldcl_record,     new Powerpc::IP_rldcl(UpdateCr::YES));
    iproc_set(powerpc_rldcr,            new Powerpc::IP_rldcr(UpdateCr::NO));
    iproc_set(powerpc_rldcr_record,     new Powerpc::IP_rldcr(UpdateCr::YES));
    iproc_set(powerpc_rldic,            new Powerpc::IP_rldic(UpdateCr::NO));
    iproc_set(powerpc_rldic_record,     new Powerpc::IP_rldic(UpdateCr::YES));
    iproc_set(powerpc_rldicl,           new Powerpc::IP_rldicl(UpdateCr::NO));
    iproc_set(powerpc_rldicl_record,    new Powerpc::IP_rldicl(UpdateCr::YES));
    iproc_set(powerpc_rldicr,           new Powerpc::IP_rldicr(UpdateCr::NO));
    iproc_set(powerpc_rldicr_record,    new Powerpc::IP_rldicr(UpdateCr::YES));
    iproc_set(powerpc_rldimi,           new Powerpc::IP_rldimi(UpdateCr::NO));
    iproc_set(powerpc_rldimi_record,    new Powerpc::IP_rldimi(UpdateCr::YES));
    iproc_set(powerpc_rlwimi,           new Powerpc::IP_rlwimi(UpdateCr::NO));
    iproc_set(powerpc_rlwimi_record,    new Powerpc::IP_rlwimi(UpdateCr::YES));
    iproc_set(powerpc_rlwinm,           new Powerpc::IP_rlwinm(UpdateCr::NO));
    iproc_set(powerpc_rlwinm_record,    new Powerpc::IP_rlwinm(UpdateCr::YES));
    iproc_set(powerpc_rlwnm,            new Powerpc::IP_rlwnm(UpdateCr::NO));
    iproc_set(powerpc_rlwnm_record,     new Powerpc::IP_rlwnm(UpdateCr::YES));
    iproc_set(powerpc_sc,               new Powerpc::IP_sc);
    iproc_set(powerpc_sld,              new Powerpc::IP_sld(UpdateCr::NO));
    iproc_set(powerpc_sld_record,       new Powerpc::IP_sld(UpdateCr::YES));
    iproc_set(powerpc_slw,              new Powerpc::IP_slw(UpdateCr::NO));
    iproc_set(powerpc_slw_record,       new Powerpc::IP_slw(UpdateCr::YES));
    iproc_set(powerpc_srad,             new Powerpc::IP_srad(UpdateCr::NO));
    iproc_set(powerpc_srad_record,      new Powerpc::IP_srad(UpdateCr::YES));
    iproc_set(powerpc_sradi,            new Powerpc::IP_sradi(UpdateCr::NO));
    iproc_set(powerpc_sradi_record,     new Powerpc::IP_sradi(UpdateCr::YES));
    iproc_set(powerpc_sraw,             new Powerpc::IP_sraw(UpdateCr::NO));
    iproc_set(powerpc_sraw_record,      new Powerpc::IP_sraw(UpdateCr::NO));
    iproc_set(powerpc_srawi,            new Powerpc::IP_srawi(UpdateCr::NO));
    iproc_set(powerpc_srawi_record,     new Powerpc::IP_srawi(UpdateCr::YES));
    iproc_set(powerpc_srd,              new Powerpc::IP_srd(UpdateCr::NO));
    iproc_set(powerpc_srd_record,       new Powerpc::IP_srd(UpdateCr::YES));
    iproc_set(powerpc_srw,              new Powerpc::IP_srw(UpdateCr::NO));
    iproc_set(powerpc_srw_record,       new Powerpc::IP_srw(UpdateCr::YES));
    iproc_set(powerpc_stb,              new Powerpc::IP_stb);
    iproc_set(powerpc_stbu,             new Powerpc::IP_stbu);
    iproc_set(powerpc_stbux,            new Powerpc::IP_stbux);
    iproc_set(powerpc_stbx,             new Powerpc::IP_stb);
    iproc_set(powerpc_std,              new Powerpc::IP_std);
    iproc_set(powerpc_stdu,             new Powerpc::IP_stdu);
    iproc_set(powerpc_stdux,            new Powerpc::IP_stdux);
    iproc_set(powerpc_stdx,             new Powerpc::IP_stdx);
    iproc_set(powerpc_sth,              new Powerpc::IP_sth);
    iproc_set(powerpc_sthbrx,           new Powerpc::IP_sthbrx);
    iproc_set(powerpc_sthu,             new Powerpc::IP_sthu);
    iproc_set(powerpc_sthux,            new Powerpc::IP_sthux);
    iproc_set(powerpc_sthx,             new Powerpc::IP_sthx);
    iproc_set(powerpc_stfd,             new Powerpc::IP_stfd);
    iproc_set(powerpc_stfs,             new Powerpc::IP_stfs);
    iproc_set(powerpc_stswi,            new Powerpc::IP_stswi);
    iproc_set(powerpc_stswx,            new Powerpc::IP_stswx);
    iproc_set(powerpc_stmw,             new Powerpc::IP_stmw);
    iproc_set(powerpc_stwcx_record,     new Powerpc::IP_stw(UpdateCr::YES));
    iproc_set(powerpc_stw,              new Powerpc::IP_stw(UpdateCr::NO));
    iproc_set(powerpc_stwbrx,           new Powerpc::IP_stwbrx);
    iproc_set(powerpc_stwu,             new Powerpc::IP_stwu);
    iproc_set(powerpc_stwux,            new Powerpc::IP_stwux);
    iproc_set(powerpc_stwx,             new Powerpc::IP_stw(UpdateCr::NO));
    iproc_set(powerpc_subf,             new Powerpc::IP_subf(UpdateCr::NO));
    iproc_set(powerpc_subf_record,      new Powerpc::IP_subf(UpdateCr::YES));
    iproc_set(powerpc_subfc,            new Powerpc::IP_subfc(UpdateCr::NO));
    iproc_set(powerpc_subfc_record,     new Powerpc::IP_subfc(UpdateCr::YES));
    iproc_set(powerpc_subfco,           new Powerpc::IP_subfco(UpdateCr::NO));
    iproc_set(powerpc_subfco_record,    new Powerpc::IP_subfco(UpdateCr::YES));
    iproc_set(powerpc_subfe,            new Powerpc::IP_subfe(UpdateCr::NO));
    iproc_set(powerpc_subfe_record,     new Powerpc::IP_subfe(UpdateCr::YES));
    iproc_set(powerpc_subfeo,           new Powerpc::IP_subfeo(UpdateCr::NO));
    iproc_set(powerpc_subfeo_record,    new Powerpc::IP_subfeo(UpdateCr::YES));
    iproc_set(powerpc_subfic,           new Powerpc::IP_subfic);
    iproc_set(powerpc_subfme,           new Powerpc::IP_subfme(UpdateCr::NO));
    iproc_set(powerpc_subfme_record,    new Powerpc::IP_subfme(UpdateCr::YES));
    iproc_set(powerpc_subfmeo,          new Powerpc::IP_subfmeo(UpdateCr::NO));
    iproc_set(powerpc_subfmeo_record,   new Powerpc::IP_subfmeo(UpdateCr::YES));
    iproc_set(powerpc_subfo,            new Powerpc::IP_subfo(UpdateCr::NO));
    iproc_set(powerpc_subfo_record,     new Powerpc::IP_subfo(UpdateCr::YES));
    iproc_set(powerpc_subfze,           new Powerpc::IP_subfze(UpdateCr::NO));
    iproc_set(powerpc_subfze_record,    new Powerpc::IP_subfze(UpdateCr::YES));
    iproc_set(powerpc_subfzeo,          new Powerpc::IP_subfzeo(UpdateCr::NO));
    iproc_set(powerpc_subfzeo_record,   new Powerpc::IP_subfzeo(UpdateCr::YES));
    iproc_set(powerpc_td,               new Powerpc::IP_td);
    iproc_set(powerpc_tdi,              new Powerpc::IP_td);
    iproc_set(powerpc_tw,               new Powerpc::IP_tw);
    iproc_set(powerpc_twi,              new Powerpc::IP_tw);
    iproc_set(powerpc_xori,             new Powerpc::IP_xori);
    iproc_set(powerpc_xoris,            new Powerpc::IP_xoris);
    iproc_set(powerpc_xor,              new Powerpc::IP_xor(UpdateCr::NO));
    iproc_set(powerpc_xor_record,       new Powerpc::IP_xor(UpdateCr::YES));
}

void
DispatcherPowerpc::regcache_init() {
    if (regdict) {
        switch (addressWidth()) {
            case 32:
                REG_IAR = findRegister("iar", 32);      // instruction address register (instruction pointer)
                REG_LR  = findRegister("lr", 32);       // link register
                REG_XER = findRegister("xer", 32);      // fixed-point exception register
                REG_CTR = findRegister("ctr", 32);      // count register
                break;
            case 64:
                REG_IAR = findRegister("iar", 64);      // instruction address register (instruction pointer)
                REG_LR  = findRegister("lr", 64);       // link register
                REG_XER = findRegister("xer", 64);      // fixed-point exception register
                REG_CTR = findRegister("ctr", 64);      // count register
                break;
            default:
                ASSERT_not_reachable("invalid address width");
        }
        REG_XER_CA = findRegister("xer_ca", 1);         // carry
        REG_XER_SO = findRegister("xer_so", 1);         // summary overflow
        REG_XER_OV = findRegister("xer_ov", 1);         // summary overflow
        REG_CR  = findRegister("cr", 32);               // condition register
        REG_CR0 = findRegister("cr0", 4);               // CR Field 0, result of fixed-point instruction; set by updateCr()
        REG_CR0_LT = findRegister("cr0.lt", 1);         // LT field of CR0 field of CR register
    }
}

void
DispatcherPowerpc::memory_init() {
    if (BaseSemantics::StatePtr state = currentState()) {
        if (BaseSemantics::MemoryStatePtr memory = state->memoryState()) {
            switch (memory->get_byteOrder()) {
                case ByteOrder::ORDER_LSB:
                    mlog[WARN] <<"PowerPC memory state is using little-endian byte order\n";
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

void
DispatcherPowerpc::set_register_dictionary(const RegisterDictionary *regdict) {
    BaseSemantics::Dispatcher::set_register_dictionary(regdict);
    regcache_init();
}

RegisterDescriptor
DispatcherPowerpc::instructionPointerRegister() const {
    return REG_IAR;
}

RegisterDescriptor
DispatcherPowerpc::stackPointerRegister() const {
    return findRegister("r1");
}

RegisterDescriptor
DispatcherPowerpc::callReturnRegister() const {
    return REG_LR;
}

void
DispatcherPowerpc::setXerOverflow(const BaseSemantics::SValuePtr &overflow) {
    ASSERT_not_null(overflow);
    ASSERT_require(overflow->get_width() == 1);
    operators->writeRegister(REG_XER_OV, overflow);
    operators->writeRegister(REG_XER_SO, operators->ite(overflow, overflow, operators->readRegister(REG_XER_SO)));
}

void
DispatcherPowerpc::updateCr0(const BaseSemantics::SValuePtr &result) {
    ASSERT_not_null(result);
    size_t nBits = result->get_width();

    // Three-bit constants
    BaseSemantics::SValuePtr one = operators->number_(3, 1);
    BaseSemantics::SValuePtr two = operators->number_(3, 2);
    BaseSemantics::SValuePtr four = operators->number_(3, 4);

    // High three bits of CR0 are set when result is less than zero, greater than zero, or equal to zero
    BaseSemantics::SValuePtr signBit = operators->extract(result, nBits-1, nBits);
    BaseSemantics::SValuePtr highThree = operators->ite(operators->equalToZero(result), one, operators->ite(signBit, four, two));

    // Low bit is the summary overflow copied from the XER's SO field
    BaseSemantics::SValuePtr so = operators->readRegister(REG_XER_SO);
    operators->writeRegister(REG_CR0, operators->concat(so, highThree));
}

BaseSemantics::SValuePtr
DispatcherPowerpc::readAndUpdate(BaseSemantics::RiscOperators *ops, SgAsmExpression *e, size_t valueNBits) {
    // Check the expression form
    SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(e);
    SgAsmBinaryAdd *sum = mre ? isSgAsmBinaryAdd(mre->get_address()) : NULL;
    SgAsmDirectRegisterExpression *dre = sum ? isSgAsmDirectRegisterExpression(sum->get_lhs()) : NULL;
    ASSERT_not_null(dre);

    // Calculate effective address and update address base register
    BaseSemantics::SValuePtr ea = read(sum);
    this->write(dre, ops->unsignedExtend(ea, dre->get_descriptor().nBits()));

    // Read and return the memory contents
    BaseSemantics::SValuePtr dflt = undefined_(valueNBits);
    return ops->readMemory(segmentRegister(mre), ea, dflt, ops->boolean_(true));
}

void
DispatcherPowerpc::writeAndUpdate(BaseSemantics::RiscOperators *ops, SgAsmExpression *destination,
                                  const BaseSemantics::SValuePtr &value) {
    // Check the expression form
    SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(destination);
    SgAsmBinaryAdd *sum = mre ? isSgAsmBinaryAdd(mre->get_address()) : NULL;
    SgAsmDirectRegisterExpression *dre = sum ? isSgAsmDirectRegisterExpression(sum->get_lhs()) : NULL;
    ASSERT_not_null(dre);

    // Calculate effective address and update address base register
    BaseSemantics::SValuePtr ea = read(sum);
    this->write(dre, ops->unsignedExtend(ea, dre->get_descriptor().nBits()));

    // Write to memory
    ops->writeMemory(segmentRegister(mre), ea, value, ops->boolean_(true));
}

} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics2::DispatcherPowerpc);
#endif
