#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_A64

#include <sage3basic.h>
#include <DispatcherA64.h>

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

struct IP_ins: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr result = d->read(args[1]);
        SValuePtr extended = ops->signExtend(result, args[0]->get_nBits());
        d->write(args[0], extended);
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

struct IP_mov: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr result = d->read(args[1]);
        SValuePtr extended = ops->signExtend(result, args[0]->get_nBits());
        d->write(args[0], extended);
    }
};

struct IP_movn: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValuePtr result = d->read(args[1]);
        SValuePtr extended = ops->signExtend(result, args[0]->get_nBits());
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
    iproc_set(ARM64_INS_DUP,    new A64::IP_dup);
    iproc_set(ARM64_INS_INS,    new A64::IP_ins);
    iproc_set(ARM64_INS_LDR,    new A64::IP_ldr);
    iproc_set(ARM64_INS_MOV,    new A64::IP_mov);
    iproc_set(ARM64_INS_MOVN,   new A64::IP_movn);
    iproc_set(ARM64_INS_MOVZ,   new A64::IP_movz);
    iproc_set(ARM64_INS_ORR,    new A64::IP_orr);
    iproc_set(ARM64_INS_UMOV,   new A64::IP_umov);
}

void
DispatcherA64::initializeRegisterDescriptors() {
    if (regdict) {
        REG_PC = findRegister("pc", 64);
        REG_SP = findRegister("sp", 64);
        REG_LR = findRegister("lr", 64);
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

} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics2::DispatcherA64);
#endif

#endif
