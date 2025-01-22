#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherCil.h>

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/Utility.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/Diagnostics.h>

#include <SgAsmCilInstruction.h>
#include <SgAsmFloatType.h>

#include <Cxx_GrammarDowncast.h>

#include <integerOps.h>                                 // rose
#include <stringify.h>                                  // rose

using namespace Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functors that handle individual CIL instruction kinds
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Cil {

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      DispatcherCil
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DispatcherCil::DispatcherCil() {}

DispatcherCil::DispatcherCil(const Architecture::Base::ConstPtr &arch)
    : BaseSemantics::Dispatcher(arch) {}

DispatcherCil::DispatcherCil(const Architecture::Base::ConstPtr &arch, const BaseSemantics::RiscOperators::Ptr &ops)
    : BaseSemantics::Dispatcher(arch, ops) {
    regcache_init();
    iproc_init();
    memory_init();
    initializeState(ops->currentState());
}

DispatcherCil::~DispatcherCil() {}

DispatcherCil::Ptr
DispatcherCil::instance(const Architecture::Base::ConstPtr &arch) {
    return Ptr(new DispatcherCil(arch));
}

DispatcherCil::Ptr
DispatcherCil::instance(const Architecture::Base::ConstPtr &arch, const BaseSemantics::RiscOperators::Ptr &ops) {
    return Ptr(new DispatcherCil(arch, ops));
}

BaseSemantics::Dispatcher::Ptr
DispatcherCil::create(const BaseSemantics::RiscOperators::Ptr &ops) const {
    return instance(architecture(), ops);
}

DispatcherCil::Ptr
DispatcherCil::promote(const BaseSemantics::Dispatcher::Ptr &d) {
    Ptr retval = as<DispatcherCil>(d);
    ASSERT_not_null(retval);
    return retval;
}

void
DispatcherCil::iproc_init() {}

void
DispatcherCil::regcache_init() {
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

void
DispatcherCil::memory_init() {
    if (BaseSemantics::State::Ptr state = currentState()) {
        if (BaseSemantics::MemoryState::Ptr memory = state->memoryState()) {
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

SValuePtr
DispatcherCil::condition(CilInstructionKind, RiscOperators*) {
    ASSERT_not_implemented("DQ (10/8/2021): Not clear what if any CIL support may be required here.");
}

int
DispatcherCil::iprocKey(SgAsmInstruction *insn_) const {
    SgAsmCilInstruction *insn = isSgAsmCilInstruction(insn_);
    ASSERT_not_null(insn);
    return insn->get_kind();
}

// Override Dispatcher::read so that if we read the PC register we get the address of the current instruction plus 2.  See
// the note in Cil::P::process()
BaseSemantics::SValuePtr
DispatcherCil::read(SgAsmExpression *e, size_t value_nbits, size_t addr_nbits/*=0*/) {
    ASSERT_not_null(e);
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

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::DispatcherCil);
#endif

#endif
