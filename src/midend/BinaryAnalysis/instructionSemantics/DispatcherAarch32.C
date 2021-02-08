#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32
#include <sage3basic.h>
#include <DispatcherAarch32.h>

using namespace Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {

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

    virtual void p(D, Ops, I, A) = 0;

    virtual void process(const BaseSemantics::DispatcherPtr &dispatcher_, SgAsmInstruction *insn_) ROSE_OVERRIDE {
        DispatcherAarch32Ptr dispatcher = DispatcherAarch32::promote(dispatcher_);
        BaseSemantics::RiscOperatorsPtr operators = dispatcher->operators();
        SgAsmAarch32Instruction *insn = isSgAsmAarch32Instruction(insn_);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NOTE: Almost all "auto" typed variables below are Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct IP_and: P {
    void p(D d, Ops ops, I insn, A args) {
#if 0 // [Robb Matzke 2021-01-28]: work in progress, does not compile yet
        assert_args(insn, args, 3);
        auto cond = d->conditionHolds(insn->get_condition());
        auto a = d->read(args[1]);
        auto [b, carry] = d->a32ExpandImmC(d->read(args[2]));
        auto result = ops->and_(a, b);
        auto nop = d->read(args[0]);
        auto condResult = ops->ite(cond, result, nop);
        d->write(args[0], condResult);

        if (d->isIPRegister(args[0])) {
            if (insn->updateFlags()) {
                d->aluExceptionReturn(cond, result);
            } else {
                d->aluWritePc(cond, result);
            }
        } else if (insn->updateFlags()) {
            d->conditionalUpdate(cond, PSTATE_N, signBit(d, ops, result));
            d->conditionalUpdate(cond, PSTATE_Z, isZero(d, ops, result));
            d->conditionalUpdate(cond, PSTATE_C, carry);
            // PSTATE_V is unchanged
        }
#else
        ASSERT_not_implemented("[Robb Matzke 2021-01-28]");
#endif
    }
};


} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
DispatcherAarch32::initializeInsnDispatchTable() {
    iproc_set(ARM_INS_AND, new Aarch32::IP_and);
}

void
DispatcherAarch32::initializeRegisterDescriptors() {
    if (regdict) {
        REG_PC = findRegister("pc", 32);
        REG_SP = findRegister("sp", 32);
        REG_LR = findRegister("lr", 32);
    }
}

void
DispatcherAarch32::initializeMemory() {
    if (BaseSemantics::StatePtr state = currentState()) {
        if (BaseSemantics::MemoryStatePtr memory = state->memoryState()) {
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

void
DispatcherAarch32::set_register_dictionary(const RegisterDictionary *regdict) {
    BaseSemantics::Dispatcher::set_register_dictionary(regdict);
    initializeRegisterDescriptors();
}

int
DispatcherAarch32::iproc_key(SgAsmInstruction *insn_) const {
    auto insn = isSgAsmAarch32Instruction(insn_);
    ASSERT_not_null(insn);
    return insn->get_kind();
}

SValuePtr
DispatcherAarch32::conditionHolds(Aarch32InstructionCondition cond) {
#if 0 // [Robb Matzke 2021-01-28]
    // FIXME[Robb Matzke 2021-01-27]: Surely not all of these can occur in the instruction condition field which is only three
    // bits wide!
    switch (cond) {
        case Aarch32InstructionCondition::ARM_CC_INVALID:
            ASSERT_not_reachable("[Robb Matzke 2021-01-27]");
        case Aarch32InstructionCondition::ARM_CC_EQ:      // equal (z set)
            return operators()->readRegister(REG_PSTATE_Z);

        case Aarch32InstructionCondition::ARM_CC_NE:      // not equal: not equal, or unordered (z clear)
        case Aarch32InstructionCondition::ARM_CC_HS:      // unsigned higher or same: >, ==, or unordered (c set)
        case Aarch32InstructionCondition::ARM_CC_LO:      // unsigned lower or same: less than (c clear)
        case Aarch32InstructionCondition::ARM_CC_MI:      // minus, negative: less than (n set)
        case Aarch32InstructionCondition::ARM_CC_PL:      // plus, positive or zero: >, ==, or unordered (n clear)
        case Aarch32InstructionCondition::ARM_CC_VS:      // overflow: unordered (v set)
        case Aarch32InstructionCondition::ARM_CC_VC:      // no overflow: ordered (v clear)
        case Aarch32InstructionCondition::ARM_CC_HI:      // unsigned higher: greater than, or unordered
        case Aarch32InstructionCondition::ARM_CC_LS:      // unsigned lower or same: less than or equal
        case Aarch32InstructionCondition::ARM_CC_GE:      // greater than or equal: greater than or equal (n == v)
        case Aarch32InstructionCondition::ARM_CC_LT:      // less than: less than, or unordered (n != v)
        case Aarch32InstructionCondition::ARM_CC_GT:      // signed greater than: greater than
        case Aarch32InstructionCondition::ARM_CC_LE:      // signed less than or equal: <, ==, or unorderd
        case Aarch32InstructionCondition::ARM_CC_AL:      // always (unconditional): always (unconditional)
            // Arhg!!! Impedence mismatch between the Capstone documentation and the ARM documentation means we'll have to
            // reverse engineer the meanings when we first encounter them. Also, the values of these enumerators in Capstone
            // don't match the values in the 3-bit field of the encoded instructions, so we can't go by that either.
            ASSERT_not_implemented("[Robb Matzke 2021-01-27]");
    }
    ASSERT_not_reachable("invalid condition");
#else
    ASSERT_not_implemented("[Robb Matzke 2021-01-28]");
#endif
}

// Corresponds to ARM32's V<m:n> operator, which extracts bits n (inclusive) through m (inclusive) where m >= n and the width
// of V is at least m+1 bits in width.  We name this operation "part" instead of "extract" because we want the arguments to
// be in the same order as the ARM pseudo code, and ROSE uses "extract" already with the args in the opposite order.
SValuePtr
DispatcherAarch32::part(const SValuePtr &value, size_t maxBit, size_t minBit) {
#if 0 // [Robb Matzke 2021-01-29]
    ASSERT_not_null(value);
    ASSERT_require(maxBit >= minBit);
    ASSERT_require(maxBit < value->nBits());
    return operators()->extract(value, minBit, maxBit+1);
#else
    ASSERT_not_implemented("[Robb Matzke 2021-01-29]");
#endif
}

// Corresponds to ARM32's V<n> operator, which returns the bit at position N.
SValuePtr
DispatcherAarch32::part(const SValuePtr &value, size_t n) {
#if 0 // [Robb Matzke 2021-01-29]
    ASSERT_not_null(value);
    ASSERT_require(n < value->nBits());
    return operators()->extract(value, n, n+1);
#else
    ASSERT_not_implemented("[Robb Matzke 2021-01-29]");
#endif
}

// This is the ARM32 H:L operator that concatenates H and L where H are the high-order bits and L are the low-order bits. We name
// this operation "join" instead of "concate" because we want the order of operands to match the ARM32 pseudo code.
SValuePtr
DispatcherAarch32::join(const SValuePtr &hi, const SValuePtr &lo) {
    ASSERT_not_null(hi);
    ASSERT_not_null(lo);
    return operators()->concat(lo, hi);
}

// See ARM32 A32ExpandImm_C
std::tuple<SValuePtr /*value*/, SValuePtr /*carry*/>
DispatcherAarch32::a32ExpandImmC(const SValuePtr &imm12) {
#if 0 // [Robb Matzke 2021-01-28]
    ASSERT_not_null(expr);
    auto carryIn = read(PSTATE_C);
    auto unrotatedValue = zeroExtend(part(imm12, 7, 0), 12);;
    return ShiftC(unrotatedValue, srTypeRor, times2(part(imm12, 11, 8), carryIn));
#else
    ASSERT_not_implemented("[Robb Matzke 2021-01-28]");
#endif
}

// See ARM32 ZeroExtend
SValuePtr
DispatcherAarch32::zeroExtend(const SValuePtr &value, size_t n) {
#if 0 // [Robb Matzke 2021-01-28]
    ASSERT_not_null(value);
    ASSERT_require(n >= value->nBits());
    if (value->nBits() == n) {
        return value;
    } else {
        return operators_->unsignedExtend(value, n);
    }
#else
    ASSERT_not_implemented("[Robb Matzke 2021-01-28]");
#endif
}

// See ARM32 Zeros
SValuePtr
DispatcherAarch32::makeZeros(size_t n) {
    ASSERT_require(n > 0);
    return operators()->number_(n, 0);
}

// See ARM32 Shift_C
std::tuple<SValuePtr /*value*/, SValuePtr /*carry*/>
DispatcherAarch32::shiftC(const SValuePtr &value, SrType srType, int amount, const SValuePtr &carryIn) {
#if 0 // [Robb Matzke 2021-01-29]
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
#else
    ASSERT_not_implemented("[Robb Matzke 2021-01-29]");
#endif
}

// See ARM32 LSL_C
std::tuple<SValuePtr /*value*/, SValuePtr /*carry*/>
DispatcherAarch32::lslC(const SValuePtr &x, size_t shift) {
#if 0 // [Robb Matzke 2021-01-28]
    ASSERT_not_null(x);
    ASSERT_require(shift > 0);
    const size_t n = x->nBits();
    auto extendedX = join(x, makeZeros(shift));
    auto result = part(extendedX, n-1, 0);
    auto carryOut = part(extendedX, n);
    return {result, carryOut};
#else
    ASSERT_not_implemented("[Robb Matzke 2021-01-28]");
#endif
}

// See ARM32 LSR_C
std::tuple<SValuePtr /*value*/, SValuePtr /*carry*/>
DispatcherAarch32::lsrC(const SValuePtr &x, size_t shift) {
#if 0 // [Robb Matzke 2021-01-28]
    ASSERT_not_null(x);
    ASSERT_require(shift > 0);
    const size_t n = x->nBits();
    auto extendedX = zeroExtend(x, shift + n);
    auto result = part(extendedX, shift + n - 1, shift);
    auto carryOut = part(extendedX, shift - 1);
    return {result, carryOut};
#else
    ASSERT_not_implemented("[Robb Matzke 2021-01-28]");
#endif
}

// See ARM32 ASR_C
std::tuple<SValuePtr /*value*/, SValuePtr /*carry*/>
DispatcherAarch32::asrC(const SValuePtr &x, size_t shift) {
#if 0 // [Robb Matzke 2021-01-28]
    ASSERT_not_null(x);
    ASSERT_require(shift > 0);
    const size_t n = x->nBits();
    auto extendedX = signExtend(x, shift + n);
    auto result = part(extendedX, shift + n - 1, shift);
    auto carryOut = part(extendedX, shift - 1);
    return {result, carryOut};
#else
    ASSERT_not_implemented("[Robb Matzke 2021-01-28]");
#endif
}

// See ARM32 ROR_C
std::tuple<SValuePtr /*value*/, SValuePtr /*carry*/>
DispatcherAarch32::rorC(const SValuePtr &x, int shift) {
#if 0 // [Robb Matzke 2021-01-28]
    ASSERT_not_null(x);
    ASSERT_require(shift != 0);
    const size_t n = x->nBits();
    const int m = shift % n;
    auto result = operators()->or_(lsr(x, m), lsl(x, n-m));
    auto carryOut = part(result, n-1);
    return {result, carryOut};
#else
    ASSERT_not_implemented("[Robb Matzke 2021-01-28]");
#endif
}

// See ARM32 LSR
SValuePtr
DispatcherAarch32::lsr(const SValuePtr &x, size_t shift) {
    ASSERT_not_null(x);
    if (0 == shift) {
        return x;
    } else {
        return std::get<0>(lsrC(x, shift));
    }
}

// See ARM32 LSL
SValuePtr
DispatcherAarch32::lsl(const SValuePtr &x, size_t shift) {
    ASSERT_not_null(x);
    if (0 == shift) {
        return x;
    } else {
        return std::get<0>(lslC(x, shift));
    }
}

// See ARM32 RRX_C
std::tuple<SValuePtr /*value*/, SValuePtr /*carry*/>
DispatcherAarch32::rrxC(const SValuePtr &x, const SValuePtr &carryIn) {
#if 0 // [Robb Matzke 2021-01-28]
    ASSERT_not_null(x);
    ASSERT_not_null(carryIn);
    ASSERT_require(carryIn->nBits() == 1);
    const size_t n = x->nBits();
    auto result = join(carryIn, part(x, n-1, 1));
    auto carryOut = part(x, 0);
    return {result, carryOut};
#else
    ASSERT_not_implemented("[Robb Matzke 2021-01-28]");
#endif
}

// See ARM32 SignExtend
SValuePtr
DispatcherAarch32::signExtend(const SValuePtr &x, size_t n) {
#if 0 // [Robb Matzke 2021-01-29]
    ASSERT_not_null(x);
    ASSERT_require(n >= x->nBits());
    return operators()->signExtend(x, n);
#else
    ASSERT_not_implemented("[Robb Matzke 2021-01-29]");
#endif
}

} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics2::DispatcherAarch32);
#endif

#endif
