#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"
#include <Rose/BinaryAnalysis/DisassemblerCil.h>

#include <Rose/Diagnostics.h>

// DQ (11/1/2021): Added header files for le_to_host.
#include "ByteOrder.h"
#include "SageBuilderAsm.h"

#include "integerOps.h"
#include "stringify.h"
#include "SageBuilderAsm.h"
#include <Rose/BinaryAnalysis/InstructionSemantics2/DispatcherCil.h>
#include <Rose/BinaryAnalysis/Unparser/Cil.h>

#include <Sawyer/Assert.h>                              // FIXME[Robb P. Matzke 2014-06-19]: replace with "Diagnostics.h"

#if 1 /*DEBUGGING [Robb P. Matzke 2013-10-02]*/
#include "AsmUnparser_compat.h"
#endif

namespace Rose {
namespace BinaryAnalysis {

using namespace Diagnostics;
using namespace IntegerOps;
using namespace StringUtility;

typedef BitPattern<uint16_t> Pattern;

// Bit pattern for an opcode in the high-order 4 bits of the instruction word
static Pattern
OP(unsigned val) {
    return Pattern(12, 15, val, 0);
}

// Bit pattern for an effective address mode in the low-order 6 bits of the instruction word. Only the specified modes are
// matched.
static Pattern
EAM(unsigned eamodes, size_t lobit=0)
{
    ASSERT_require(eamodes!=0);
    Pattern retval;
    size_t hibit = lobit + 6 - 1; // inclusive
    for (size_t bitnum=0; eamodes!=0; ++bitnum) {
        unsigned bit = IntegerOps::shl1<unsigned>(bitnum);
        if (0 != (eamodes & bit)) {
            eamodes &= ~bit;
            CilEffectiveAddressMode eam = (CilEffectiveAddressMode)bit;
            switch (eam) {
                case Cil_eam_drd:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, reg, 0);
                    break;
                case Cil_eam_ard:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, 0x08|reg, 0);
                    break;
                case Cil_eam_ari:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, 0x10|reg, 0);
                    break;
                case Cil_eam_inc:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, 0x18|reg, 0);
                    break;
                case Cil_eam_dec:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, 0x20|reg, 0);
                    break;
                case Cil_eam_dsp:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, 0x28|reg, 0);
                    break;
                case Cil_eam_idx8:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, 0x30|reg, 0);
                    break;
                case Cil_eam_idxbd:
                    if (0 == (eamodes & Cil_eam_idx8)) {
                        for (size_t reg=0; reg<8; ++reg)
                            retval |= Pattern(lobit, hibit, 0x30|reg, 0);
                    }
                    break;
                case Cil_eam_mpost:
                    if (0 == (eamodes & Cil_eam_idx)) {
                        for (size_t reg=0; reg<8; ++reg)
                            retval |= Pattern(lobit, hibit, 0x30|reg, 0);
                    }
                    break;
                case Cil_eam_mpre:
                    if (0 == (eamodes & (Cil_eam_idx | Cil_eam_mpost))) {
                        for (size_t reg=0; reg<8; ++reg)
                            retval |= Pattern(lobit, hibit, 0x30|reg, 0);
                    }
                    break;
                case Cil_eam_pcdsp:
                    retval |= Pattern(lobit, hibit, 0x3a, 0);
                    break;
                case Cil_eam_pcidx8:
                    retval |= Pattern(lobit, hibit, 0x3b, 0);
                    break;
                case Cil_eam_pcidxbd:
                    if (0 == (eamodes & Cil_eam_pcidx8))
                        retval |= Pattern(lobit, hibit, 0x3b, 0);
                    break;
                case Cil_eam_pcmpost:
                    if (0 == (eamodes & Cil_eam_pcidx))
                        retval |= Pattern(lobit, hibit, 0x3b, 0);
                    break;
                case Cil_eam_pcmpre:
                    if (0 == (eamodes & (Cil_eam_pcidx | Cil_eam_pcmpost)))
                        retval |= Pattern(lobit, hibit, 0x3b, 0);
                    break;
                case Cil_eam_absw:
                    retval |= Pattern(lobit, hibit, 0x38, 0);
                    break;
                case Cil_eam_absl:
                    retval |= Pattern(lobit, hibit, 0x39, 0);
                    break;
                case Cil_eam_imm:
                    retval |= Pattern(lobit, hibit, 0x3c, 0);
                    break;
                default:
                    ASSERT_not_reachable("invalid effective address mode: " + toHex(eam));
            }
        }
    }
    return retval;
}

// Like EAM but matches REG:MODE rather than MODE:REG
static Pattern
EAM_BACKWARD(unsigned eamodes, size_t lobit=0)
{
    ASSERT_require(eamodes!=0);
    Pattern retval;
    size_t hibit = lobit + 6 - 1; // inclusive
    for (size_t bitnum=0; eamodes!=0; ++bitnum) {
        unsigned bit = IntegerOps::shl1<unsigned>(bitnum);
        if (0 != (eamodes & bit)) {
            eamodes &= ~bit;
            CilEffectiveAddressMode eam = (CilEffectiveAddressMode)bit;
            switch (eam) {
                case Cil_eam_drd:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, reg<<3, 0);
                    break;
                case Cil_eam_ard:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, (reg<<3)|1, 0);
                    break;
                case Cil_eam_ari:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, (reg<<3)|2, 0);
                    break;
                case Cil_eam_inc:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, (reg<<3)|3, 0);
                    break;
                case Cil_eam_dec:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, (reg<<3)|4, 0);
                    break;
                case Cil_eam_dsp:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, (reg<<3)|5, 0);
                    break;
                case Cil_eam_idx8:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, (reg<<3)|6, 0);
                    break;
                case Cil_eam_idxbd:
                    if (0 == (eamodes & Cil_eam_idx8)) {
                        for (size_t reg=0; reg<8; ++reg)
                            retval |= Pattern(lobit, hibit, (reg<<3)|6, 0);
                    }
                    break;
                case Cil_eam_mpost:
                    if (0 == (eamodes & Cil_eam_idx)) {
                        for (size_t reg=0; reg<8; ++reg)
                            retval |= Pattern(lobit, hibit, (reg<<3)|6, 0);
                    }
                    break;
                case Cil_eam_mpre:
                    if (0 == (eamodes & (Cil_eam_idx | Cil_eam_mpost))) {
                        for (size_t reg=0; reg<8; ++reg)
                            retval |= Pattern(lobit, hibit, (reg<<3)|6, 0);
                    }
                    break;
                case Cil_eam_pcdsp:
                    retval |= Pattern(lobit, hibit, 0x17, 0);
                    break;
                case Cil_eam_pcidx8:
                    retval |= Pattern(lobit, hibit, 0x1f, 0);
                    break;
                case Cil_eam_pcidxbd:
                    if (0 == (eamodes & Cil_eam_pcidx8))
                        retval |= Pattern(lobit, hibit, 0x1f, 0);
                    break;
                case Cil_eam_pcmpost:
                    if (0 == (eamodes & Cil_eam_pcidx))
                        retval |= Pattern(lobit, hibit, 0x1f, 0);
                    break;
                case Cil_eam_pcmpre:
                    if (0 == (eamodes & (Cil_eam_pcidx | Cil_eam_pcmpost)))
                        retval |= Pattern(lobit, hibit, 0x1f, 0);
                    break;
                case Cil_eam_absw:
                    retval |= Pattern(lobit, hibit, 0x07, 0);
                    break;
                case Cil_eam_absl:
                    retval |= Pattern(lobit, hibit, 0x0f, 0);
                    break;
                case Cil_eam_imm:
                    retval |= Pattern(lobit, hibit, 0x27, 0);
                    break;
                default:
                    ASSERT_not_reachable("invalid effective address mode: " + toHex(eam));
            }
        }
    }
    return retval;
}
    
template<size_t lo, size_t hi>
Pattern BITS(unsigned val)
{
    return Pattern(lo, hi, val, 0);
}

template<size_t wordnum>
Pattern INSN_WORD(const Pattern &sub_pattern)
{
    return sub_pattern.shift_left(16*wordnum);
}

template<size_t lo>
Pattern BIT(unsigned val)
{
    return Pattern(lo, lo, val, 0);
}

static CilDataFormat
integerFormat(unsigned fmtNumber)
{
    switch (fmtNumber) {
        case 0: return Cil_fmt_i8;
        case 1: return Cil_fmt_i16;
        case 2: return Cil_fmt_i32;
        case 3: return Cil_fmt_i16;
    }
    ASSERT_not_reachable("invalid integer format number: " + numberToString(fmtNumber));
}

// Floating point format as encoded in a floating-point instruction.  The CilDataFormat enums have the same value as the
// format field in the instruction.
static CilDataFormat
floatingFormat(DisassemblerCil::State &state, unsigned fmtNumber)
{
    switch (fmtNumber) {
        case Cil_fmt_i32:
        case Cil_fmt_f32:
        case Cil_fmt_f96:
        case Cil_fmt_p96:
        case Cil_fmt_i16:
        case Cil_fmt_f64:
        case Cil_fmt_i8 :
            return (CilDataFormat)fmtNumber;
        default:
            throw Disassembler::Exception("invalid floating point format code=" + StringUtility::numberToString(fmtNumber),
                                          state.insn_va);
    }
}

// Default format for floating-point operations. Floating point values are converted to this type internally before any
// operations are performed on them.
static CilDataFormat
floatingFormatForFamily(CilFamily family)
{
#if 0
 // DQ (10/9/2021): Eliminate this code for CIL.
    if (0 != (family & Cil_freescale)) {
        // FreeScale ColdFire processors operate on 64-bit double-precision real format ("D") values stored in 64-bit
        // floating-point data registers.
        return Cil_fmt_f64;
    }
    if (0 != (family & Cil_generation_3)) {
        // The M68040 family of processors operate on 96-bit extended-precision real format ("X") values stored in 80-bit
        // floating-point data registers (16 bits of an X-format value are always zero).
        return Cil_fmt_f96;
    }
#endif

    ASSERT_not_reachable("CIL family has no floating point hardware: " + addrToString(family));
}

// Convert a numeric format to a letter that's appended to instruction mnemonics.
static std::string
formatLetter(CilDataFormat fmt)
{
    switch (fmt) {
        case Cil_fmt_i32: return "l";
        case Cil_fmt_f32: return "s";
        case Cil_fmt_f96: return "x";
        case Cil_fmt_p96: return "p";
        case Cil_fmt_i16: return "w";
        case Cil_fmt_f64: return "d";
        case Cil_fmt_i8:  return "b";
        case Cil_fmt_unknown: ASSERT_not_reachable("Cil_fmt_unknown is not a valid data format");
    }
#if 0
 // DQ (10/9/2021): Eliminate this code for CIL.
    ASSERT_not_reachable("invalid CIL data format: " + stringifyBinaryAnalysisCilDataFormat(fmt));
#else
    ROSE_ASSERT(false);
#endif
}

static size_t
formatNBits(CilDataFormat fmt)
{
    switch (fmt) {
        case Cil_fmt_i32: return 32;
        case Cil_fmt_f32: return 32;
        case Cil_fmt_f96: return 96;
        case Cil_fmt_p96: return 96;
        case Cil_fmt_i16: return 16;
        case Cil_fmt_f64: return 64;
        case Cil_fmt_i8:  return 8;
        case Cil_fmt_unknown: ASSERT_not_reachable("Cil_fmt_unknown is not a valid data format");
    }
#if 0
 // DQ (10/9/2021): Eliminate this code for CIL.
    ASSERT_not_reachable("invalid CIL data format: " + stringifyBinaryAnalysisCilDataFormat(fmt));
#else
    ROSE_ASSERT(false);
#endif
    // fix warning: no return statement in function returning non-void
    return 0;
}

// see base class
bool
DisassemblerCil::canDisassemble(SgAsmGenericHeader *header) const
{
    SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();

#if 0
 // DQ (10/9/2021): Eliminate this code for Cil.
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_Cil_Family;
#else
    return false;
#endif
}

Unparser::BasePtr
DisassemblerCil::unparser() const {
    return Unparser::Cil::instance();
}

SgAsmType *
DisassemblerCil::makeType(State &state, CilDataFormat fmt) const
{
    switch (fmt) {
        case Cil_fmt_i32: return SageBuilderAsm::buildTypeU32();
        case Cil_fmt_f32: return SageBuilderAsm::buildIeee754Binary32();

     // DQ (10/9/2021): Eliminate this code for Cil.
     // case Cil_fmt_f96: return SageBuilderAsm::buildTypeCilFloat96();

        case Cil_fmt_p96: throw Exception("96-bit binary coded decimal not implemented", state.insn_va);
        case Cil_fmt_i16: return SageBuilderAsm::buildTypeU16();
        case Cil_fmt_f64: return SageBuilderAsm::buildIeee754Binary64();
        case Cil_fmt_i8:  return SageBuilderAsm::buildTypeU8();
        case Cil_fmt_unknown: ASSERT_not_reachable("Cil_fmt_unknown is not a valid data format");
    }
#if 0
 // DQ (10/9/2021): Eliminate this code for Cil.
    ASSERT_not_reachable("invalid CIL data format: " + stringifyBinaryAnalysisCilDataFormat(fmt));
#else
    ROSE_ASSERT(false);
    return NULL;
#endif
}

SgAsmRegisterReferenceExpression *
DisassemblerCil::makeDataRegister(State &state, unsigned regnum, CilDataFormat fmt, size_t bit_offset) const
{
    size_t nbits = formatNBits(fmt);
    ASSERT_require2(regnum <= 7, "register number " + numberToString(regnum));
    ASSERT_require2(8==nbits || 16==nbits || 32==nbits, "register is " + plural(nbits, "bits"));
    ASSERT_require2(0==bit_offset || 16==bit_offset, "register offset is " + plural(bit_offset, "bits"));
    ASSERT_require(bit_offset+nbits <= 32);
    RegisterDescriptor desc(Cil_regclass_data, regnum, bit_offset, nbits);
    SgAsmRegisterReferenceExpression *expr = new SgAsmDirectRegisterExpression(desc);
    expr->set_type(makeType(state, fmt));
    return expr;
}

SgAsmRegisterReferenceExpression *
DisassemblerCil::makeAddressRegister(State &state, unsigned regnum, CilDataFormat fmt, size_t bit_offset) const
{
    size_t nbits = formatNBits(fmt);
    ASSERT_require2(regnum <= 7, "register number " + numberToString(regnum));
    ASSERT_require2(0==bit_offset || 16==bit_offset, "register offset is " + plural(bit_offset, "bits"));
    ASSERT_require(bit_offset+nbits <= 32);
    if (16!=nbits && 32!=nbits) // in particular, 8-bit access is not possible
        throw Exception("invalid address register size: " + numberToString(nbits), state.insn_va);
    RegisterDescriptor desc(Cil_regclass_addr, regnum, bit_offset, nbits);
    SgAsmRegisterReferenceExpression *expr = new SgAsmDirectRegisterExpression(desc);
    expr->set_type(makeType(state, fmt));
    return expr;
}

SgAsmMemoryReferenceExpression *
DisassemblerCil::makeAddressRegisterPreDecrement(State &state, unsigned regnum, CilDataFormat fmt) const
{
    SgAsmType *type = makeType(state, fmt);
    SgAsmRegisterReferenceExpression *rre = makeAddressRegister(state, regnum, Cil_fmt_i32);
    size_t nbits = formatNBits(fmt);
    ASSERT_require(nbits % 8 == 0);
    rre->set_adjustment(-(nbits/8)); // pre decrement number of bytes
    return SageBuilderAsm::buildMemoryReferenceExpression(rre, NULL/*segment*/, type);
}

SgAsmMemoryReferenceExpression *
DisassemblerCil::makeAddressRegisterPostIncrement(State &state, unsigned regnum, CilDataFormat fmt) const
{
    SgAsmType *type = makeType(state, fmt);
    SgAsmRegisterReferenceExpression *rre = makeAddressRegister(state, regnum, Cil_fmt_i32);
    size_t nbits = formatNBits(fmt);
    ASSERT_require(nbits % 8 == 0);
    rre->set_adjustment(nbits/8); // post increment number of bytes
    return SageBuilderAsm::buildMemoryReferenceExpression(rre, NULL/*segment*/, type);
}
    

SgAsmRegisterReferenceExpression *
DisassemblerCil::makeDataAddressRegister(State &state, unsigned regnum, CilDataFormat fmt, size_t bit_offset) const
{
    ASSERT_require2(regnum <= 15, "register number is " + numberToString(regnum));
    return regnum <= 7 ? makeDataRegister(state, regnum, fmt, bit_offset) : makeAddressRegister(state, regnum-8, fmt, bit_offset);
}

SgAsmRegisterNames *
DisassemblerCil::makeRegistersFromMask(State &state, unsigned mask, CilDataFormat fmt, bool reverse) const
{
    SgAsmRegisterNames *reglist = new SgAsmRegisterNames;
    for (unsigned bitnum=0; bitnum<16; ++bitnum) {
        if (0 != (mask & IntegerOps::shl1<unsigned>(bitnum))) {
            SgAsmRegisterReferenceExpression *reg = makeDataAddressRegister(state, reverse?15-bitnum:bitnum, fmt, 0);
            reglist->get_registers().push_back(reg);
            reg->set_parent(reglist);
        }
    }
    reglist->set_mask(mask);
    reglist->set_type(makeType(state, fmt));
    return reglist;
}

SgAsmRegisterNames *
DisassemblerCil::makeFPRegistersFromMask(State &state, unsigned mask, CilDataFormat fmt, bool reverse) const
{
    SgAsmRegisterNames *reglist = new SgAsmRegisterNames;
    for (unsigned bitnum=0; bitnum<8; ++bitnum) {
        if (0 != (mask & IntegerOps::shl1<unsigned>(bitnum))) {
            SgAsmRegisterReferenceExpression *reg = makeFPRegister(state, reverse?8-bitnum:bitnum);
            reglist->get_registers().push_back(reg);
            reg->set_parent(reglist);
        }
    }
    reglist->set_mask(mask);
    reglist->set_type(makeType(state, fmt));
    return reglist;
}

SgAsmRegisterReferenceExpression *
DisassemblerCil::makeStatusRegister(State &state) const
{
    RegisterDescriptor desc(Cil_regclass_spr, Cil_spr_sr, 0, 16);
    SgAsmRegisterReferenceExpression *expr = new SgAsmDirectRegisterExpression(desc);
    expr->set_type(makeType(state, Cil_fmt_i16));
    return expr;
}

SgAsmRegisterReferenceExpression *
DisassemblerCil::makeConditionCodeRegister(State &state) const
{
    RegisterDescriptor desc(Cil_regclass_spr, Cil_spr_sr, 0, 8); // CCR is the low byte of the status register
    SgAsmRegisterReferenceExpression *expr = new SgAsmDirectRegisterExpression(desc);
    expr->set_type(makeType(state, Cil_fmt_i8));
    return expr;
}

SgAsmRegisterReferenceExpression *
DisassemblerCil::makeColdFireControlRegister(State &state, unsigned regnum) const {
    // From table 8.3: "Coldfire CPU Space Assignments" in "CFPRM ColdFire Family Programmer's Reference Manual"
    // for the privileged MOVEC instruction.
    RegisterDescriptor rd;
    switch (regnum) {
        // Memory management control registers
        case 0x002:                                     // cache control register (CACR)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_cacr, 0, 32);
            break;
        case 0x003:                                     // address space identifier register (ASID)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_asid, 0, 32);
            break;
        case 0x004:                                     // access control registers 0 (ACR0)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_acr0, 0, 32);
            break;
        case 0x005:                                     // access control registers 1 (ACR1)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_acr1, 0, 32);
            break;
        case 0x006:                                     // access control registers 2 (ACR2)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_acr2, 0, 32);
            break;
        case 0x007:                                     // access control registers 3 (ACR3)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_acr3, 0, 32);
            break;
        case 0x008:                                     // mmu base address register (MMUBAR)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_mmubar, 0, 32);
            break;

        // Processor miscellaneous registers
        case 0x801:                                     // vector base register (VBR)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_vbr, 0, 32);
            break;
        case 0x80f:                                     // program counter (PC)
            return makeProgramCounter(state);

        // Local memory and module control registers
        case 0xc00:                                     // ROM base address register 0 (ROMBAR0)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_rombar0, 0, 32);
            break;
        case 0xc01:                                     // ROM base address register 1 (ROMBAR1)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_rombar1, 0, 32);
            break;
        case 0xc04:                                     // RAM base address register 0 (RAMBAR0)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_rambar0, 0, 32);
            break;
        case 0xc05:                                     // RAM base address register 1( RAMBAR1)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_rambar1, 0, 32);
            break;
        case 0xc0c:                                     // multiprocessor control register (MPCR)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_mpcr, 0, 32);
            break;
        case 0xc0d:                                     // embedded DRAM base address register (EDRAMBAR)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_edrambar, 0, 32);
            break;
        case 0xc0e:                                     // secondary module base address register (SECMBAR)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_secmbar, 0, 32);
            break;
        case 0xc0f:                                     // primary module base address register (MBAR)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_mbar, 0, 32);
            break;

        // Local memory address permutation control registers
        case 0xd02:                                     // 32 msbs of RAM 0 permutation control register 1 (PCR1U0)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_0_pcr1, 32, 32);
            break;
        case 0xd03:                                     // 32 lsbs of RAM 0 permutation control register 1 (PCR1L0)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_0_pcr1, 0, 32);
            break;
        case 0xd04:                                     // 32 msbs of RAM 0 permutation control register 2 (PCR2U0)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_0_pcr2, 32, 32);
            break;
        case 0xd05:                                     // 32 lsbs of RAM 0 permutation control register 2 (PCR2L0)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_0_pcr2, 0, 32);
            break;
        case 0xd06:                                     // 32 msbs of RAM 0 permutation control register 3 (PCR3U0)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_0_pcr3, 32, 32);
            break;
        case 0xd07:                                     // 32 lsbs of RAM 0 permutation control register 3 (PCR3L0)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_0_pcr3, 0, 32);
            break;
        case 0xd0a:                                     // 32 msbs of RAM 1 permutation control register 1 (PCR1U1)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_1_pcr1, 32, 32);
            break;
        case 0xd0b:                                     // 32 lsbs of RAM 1 permutation control register 1 (PCR1L1)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_1_pcr1, 0, 32);
            break;
        case 0xd0c:                                     // 32 msbs of RAM 1 permutation control register 2 (PCR2U1)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_1_pcr2, 32, 32);
            break;
        case 0xd0d:                                     // 32 lsbs of RAM 1 permutation control register 2 (PCR2L1)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_1_pcr2, 0, 32);
            break;
        case 0xd0e:                                     // 32 msbs of RAM 1 permutation control register 3 (PCR3U1)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_1_pcr3, 32, 32);
            break;
        case 0xd0f:                                     // 32 lsbs of RAM 1 permutation control register 3 (PCR3L1)
            rd = RegisterDescriptor(Cil_regclass_sup, Cil_sup_1_pcr3, 0, 32);
            break;

        default:
            // "Not all control registers are implemented in every processor design. Attempted access to undefined or
            // unimplemented control register space produces undefined results." [Programmer's Reference Manual].
            throw Disassembler::Exception("invalid control register number: " + StringUtility::toHex2(regnum, 12, false, false),
                                          state.insn_va);
    }
    SgAsmRegisterReferenceExpression *expr = new SgAsmDirectRegisterExpression(rd);
    ASSERT_require(rd.nBits() == 32);
    expr->set_type(makeType(state, Cil_fmt_i32));
    return expr;
}

SgAsmRegisterReferenceExpression *
DisassemblerCil::makeProgramCounter(State &state) const
{
    RegisterDescriptor desc(Cil_regclass_spr, Cil_spr_pc, 0, 32);
    SgAsmRegisterReferenceExpression *expr = new SgAsmDirectRegisterExpression(desc);
    expr->set_type(makeType(state, Cil_fmt_i32));
    return expr;
}

SgAsmRegisterReferenceExpression *
DisassemblerCil::makeMacRegister(State &state, CilMacRegister minor) const
{
    RegisterDescriptor desc(Cil_regclass_mac, minor, 0, 32);
    SgAsmRegisterReferenceExpression *expr = new SgAsmDirectRegisterExpression(desc);
    expr->set_type(makeType(state, Cil_fmt_i32));
    return expr;
}

SgAsmRegisterReferenceExpression *
DisassemblerCil::makeMacAccumulatorRegister(State &state, unsigned accumIndex) const
{
    ASSERT_require(accumIndex < 4);
    RegisterDescriptor desc(Cil_regclass_mac, Cil_mac_acc0+accumIndex, 0, 32);
    SgAsmRegisterReferenceExpression *expr = new SgAsmDirectRegisterExpression(desc);
    expr->set_type(makeType(state, Cil_fmt_i32));
    return expr;
}

SgAsmRegisterReferenceExpression *
DisassemblerCil::makeFPRegister(State &state, unsigned regnum) const
{
    CilDataFormat fmt = floatingFormatForFamily(family);
    SgAsmType *type = NULL;
#if 0
 // DQ (10/9/2021): Eliminate this code for Cil.
    switch (fmt) {
        case Cil_fmt_f64:
            type = makeType(state, fmt);
            break;
        case Cil_fmt_f96:
            // The Motorola 68xxx 96-bit "extended real" type has 16 bits of zeros smack in the middle. The floating-point
            // registers don't store the zeros and are therefore only 80 bits wide.
            type = SageBuilderAsm::buildTypeCilFloat80();
            break;
        default:
            ASSERT_not_reachable("invalid default floating-point format: " + stringifyBinaryAnalysisCilDataFormat(fmt));
    }
#else
    ROSE_ASSERT(false);
#endif

    RegisterDescriptor desc(Cil_regclass_fpr, regnum, 0, type->get_nBits());
    SgAsmRegisterReferenceExpression *expr = new SgAsmDirectRegisterExpression(desc);
    expr->set_type(type);
    return expr;
}

SgAsmIntegerValueExpression *
DisassemblerCil::makeImmediateValue(State &state, CilDataFormat fmt, unsigned value) const
{
    return SageBuilderAsm::buildValueInteger(value, makeType(state, fmt));
}

SgAsmIntegerValueExpression *
DisassemblerCil::makeImmediateExtension(State &state, CilDataFormat fmt, size_t ext_word_number) const
{
    size_t nBits = formatNBits(fmt);
    ASSERT_require(nBits > 0);
    if (8==nBits)
        return SageBuilderAsm::buildValueInteger(instructionWord(state, ext_word_number+1) & 0xff, makeType(state, fmt));
        
    ASSERT_require(nBits % 16 == 0);
    size_t nWords = nBits / 16;
    Sawyer::Container::BitVector bv(nBits);
    for (size_t i=0; i<nWords; ++i) {                   // values are stored from high to low word; each word big endian
        uint16_t word = instructionWord(state, ext_word_number+i+1);// +1 for the opcode word itself
        size_t bitOffset = (nWords-(i+1))*16;
        bv.fromInteger(bv.baseSize(bitOffset, 16), word);
    }
    return SageBuilderAsm::buildValueInteger(bv, makeType(state, fmt));
}
            
// The modreg should be 6 bits: upper three bits are the mode, lower three bits are usually a register number.
SgAsmExpression *
DisassemblerCil::makeEffectiveAddress(State &state, unsigned modreg, CilDataFormat fmt, size_t ext_offset) const
{
    ASSERT_require2(0 == (modreg & ~0x3f), "modreg should be 6 bits wide; got " + addrToString(modreg));
    unsigned mode = (modreg >> 3) & 7;
    unsigned reg  = modreg & 7;
    return makeEffectiveAddress(state, mode, reg, fmt, ext_offset);
}

SgAsmExpression *
DisassemblerCil::makeEffectiveAddress(State &state, unsigned mode, unsigned reg, CilDataFormat fmt, size_t ext_offset) const
{
    ASSERT_require(mode < 8);
    ASSERT_require(reg < 8);

#if 0
 // DQ (10/9/2021): Eliminate this code for Cil.

    SgAsmType *type = makeType(state, fmt);

    if (0==mode) {
        // Cil_eam_drd: data register direct
        return makeDataRegister(state, reg, fmt);
    } else if (1==mode) {
        // Cil_eam_ard: address register direct
        return makeAddressRegister(state, reg, fmt);
    } else if (2==mode) {
        // Cil_eam_ari: address register indirect
        SgAsmRegisterReferenceExpression *rre = makeAddressRegister(state, reg, Cil_fmt_i32);
        return SageBuilderAsm::buildMemoryReferenceExpression(rre, NULL/*segment*/, type);
    } else if (3==mode) {
        // Cil_eam_inc: address register indirect with post increment
        return makeAddressRegisterPostIncrement(state, reg, fmt);
    } else if (4==mode) {
        // Cil_eam_dec: address register indirect with pre decrement
        return makeAddressRegisterPreDecrement(state, reg, fmt);
    } else if (5==mode) {
        // Cil_eam_dsp: address register indirect with displacement
        SgAsmRegisterReferenceExpression *rre = makeAddressRegister(state, reg, Cil_fmt_i32);
        uint64_t displacement_n = signExtend<16, 32>((uint64_t)instructionWord(state, ext_offset+1));
        SgAsmIntegerValueExpression *displacement = SageBuilderAsm::buildValueInteger(displacement_n, makeType(state, Cil_fmt_i32));
        SgAsmExpression *address = SageBuilderAsm::buildAddExpression(rre, displacement);
        address->set_type(makeType(state, Cil_fmt_i32));
        return SageBuilderAsm::buildMemoryReferenceExpression(address, NULL/*segment*/, type);
    } else if (6==mode) {
        if (family & (Cil_generation_2 | Cil_generation_3)) {
            // Cil_eam_idxbd: address register indirect with index (base displacement)
            throw Exception("FIXME[Robb P. Matzke 2014-07-14]: Cil_eam_idxbd not implemented yet", state.insn_va);
        } else {
            // Cil_eam_idx8: address register indirect with index (8-bit displacement)
            // Uses the brief extension word format (1 word having D/A, Register, W/L, Scale, and 8-bit displacement)
            if (0!=extract<8, 8>(instructionWord(state, ext_offset+1)))
                throw Exception("Cil_eam_idx8 mode requires that bit 8 of the extension word is clear", state.insn_va);
            if (extract<11, 11>(instructionWord(state, ext_offset+1))) {
                // 32-bit index
                SgAsmExpression *address = makeAddressRegister(state, reg, Cil_fmt_i32);
                uint32_t disp = signExtend<8, 32>((uint32_t)extract<0, 7>(instructionWord(state, ext_offset+1)));
                SgAsmIntegerValueExpression *dispExpr = SageBuilderAsm::buildValueInteger(disp, makeType(state, Cil_fmt_i32));
                address = SageBuilderAsm::buildAddExpression(address, dispExpr);
                address->set_type(makeType(state, Cil_fmt_i32));

                unsigned indexRegisterNumber = extract<12, 15>(instructionWord(state, ext_offset+1));
                SgAsmRegisterReferenceExpression *indexRRE = makeDataAddressRegister(state, indexRegisterNumber, Cil_fmt_i32);
                uint32_t scale = IntegerOps::shl1<uint32_t>(extract<9, 10>(instructionWord(state, ext_offset+1)));
                SgAsmIntegerValueExpression *scaleExpr = SageBuilderAsm::buildValueInteger(scale, makeType(state, Cil_fmt_i32));
                SgAsmExpression *product = SageBuilderAsm::buildMultiplyExpression(indexRRE, scaleExpr);
                product->set_type(makeType(state, Cil_fmt_i32));
                address = SageBuilderAsm::buildAddExpression(address, product);
                address->set_type(makeType(state, Cil_fmt_i32));
                return SageBuilderAsm::buildMemoryReferenceExpression(address, NULL /*segment*/, type);
            } else {
                throw Exception("FIXME[Robb P. Matzke 2014-07-14]: word index register not implemented yet", state.insn_va);
            }
        }
    } else if (7==mode && 0==reg) {
        // Cil_eam_absw: absolute short addressing mode
        uint64_t val = signExtend<16, 32>((uint64_t)instructionWord(state, ext_offset+1));
        SgAsmIntegerValueExpression *address = SageBuilderAsm::buildValueInteger(val, makeType(state, Cil_fmt_i32));
        return SageBuilderAsm::buildMemoryReferenceExpression(address, NULL/*segment*/, type);
    } else if (7==mode && 1==reg) {
        // Cil_eam_absl: absolute long addressing mode
        uint64_t val = shiftLeft<32>((uint64_t)instructionWord(state, ext_offset+1), 16) | (uint64_t)instructionWord(state, ext_offset+2);
        SgAsmIntegerValueExpression *address = SageBuilderAsm::buildValueInteger(val, makeType(state, Cil_fmt_i32));
        return SageBuilderAsm::buildMemoryReferenceExpression(address, NULL/*segment*/, type);
    } else if (7==mode && 2==reg) {
        // Cil_eam_pcdsp: program counter indirect with displacement
        SgAsmRegisterReferenceExpression *rre = makeProgramCounter(state);
        uint64_t displacement_n = signExtend<16, 32>((uint64_t)instructionWord(state, ext_offset+1));
        SgAsmIntegerValueExpression *displacement = SageBuilderAsm::buildValueInteger(displacement_n, makeType(state, Cil_fmt_i32));
        SgAsmExpression *address = SageBuilderAsm::buildAddExpression(rre, displacement);
        address->set_type(makeType(state, Cil_fmt_i32));
        return SageBuilderAsm::buildMemoryReferenceExpression(address, NULL/*segment*/, type);
    } else if (7==mode && 3==reg) {
        if (0==extract<8, 8>(instructionWord(state, ext_offset+1))) { // brief extension word format (8-bit displacement)
            unsigned indexRegisterNumber = extract<12, 15>(instructionWord(state, ext_offset+1));
            SgAsmRegisterReferenceExpression *indexRRE = extract<11, 11>(instructionWord(state, ext_offset+1)) ?
                                                         makeDataAddressRegister(state, indexRegisterNumber, Cil_fmt_i32, 0) :
                                                         makeDataAddressRegister(state, indexRegisterNumber, Cil_fmt_i16, 0);
            uint32_t scale = IntegerOps::shl1<uint32_t>(extract<9, 10>(instructionWord(state, ext_offset+1)));
            SgAsmIntegerValueExpression *scaleExpr = SageBuilderAsm::buildValueInteger(scale, makeType(state, Cil_fmt_i32));
            SgAsmExpression *product = SageBuilderAsm::buildMultiplyExpression(indexRRE, scaleExpr);
            product->set_type(makeType(state, Cil_fmt_i32));

            uint32_t disp = signExtend<8, 32>((uint32_t)extract<0, 7>(instructionWord(state, ext_offset+1)));
            SgAsmIntegerValueExpression *dispExpr = SageBuilderAsm::buildValueInteger(disp, makeType(state, Cil_fmt_i32));

            SgAsmExpression *address = makeProgramCounter(state);
            address = SageBuilderAsm::buildAddExpression(address, dispExpr);
            address->set_type(makeType(state, Cil_fmt_i32));
            address = SageBuilderAsm::buildAddExpression(address, product);
            address->set_type(makeType(state, Cil_fmt_i32));
            return SageBuilderAsm::buildMemoryReferenceExpression(address, NULL /*segment*/, type);
        } else {
            // Cil_eam_pcidxbd: program counter indirect with index (base displacement)
            // Cil_eam_pcmpost: program counter memory indirect postindexed
            // Cil_eam_pcmpre: program counter memory indirect preindexed
            throw Exception("FIXME[Robb P. Matzke 2014-06-19]: cannot distinguish between Cil_eam_{pcidxbd,pcmpost,pcmpre}",
                            state.insn_va);
        }
    } else if (7==mode && 4==reg) {
        // Cil_eam_imm: immediate data (1, 2, 4, or 6 extension words)
        return makeImmediateExtension(state, fmt, ext_offset);
    } else {
        throw Exception(std::string("invalid effective address mode:") +
                        " mode=" + numberToString(mode) + " reg="  + numberToString(reg),
                        state.insn_va);
    }
#else
    ROSE_ASSERT(false);
#endif
    // fix warning: no return statement in function returning non-void
    return nullptr;
}

SgAsmExpression *
DisassemblerCil::makeAddress(State &state, SgAsmExpression *expr) const
{
    SgAsmExpression *retval = NULL;
    ASSERT_not_null(expr);
    if (SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(expr))
        retval = mre->get_address();
    if (!retval)
        return NULL;

    // If address is pc+X then return a constant instead
    if (SgAsmBinaryAdd *sum = isSgAsmBinaryAdd(retval)) {
        SgAsmDirectRegisterExpression *lhs = isSgAsmDirectRegisterExpression(sum->get_lhs());
        SgAsmIntegerValueExpression *rhs = isSgAsmIntegerValueExpression(sum->get_rhs());
        if (lhs && rhs &&
            lhs->get_descriptor().majorNumber()==Cil_regclass_spr && lhs->get_descriptor().minorNumber()==Cil_spr_pc) {
            retval = SageBuilderAsm::buildValueInteger(state.insn_va + 2 + rhs->get_absoluteValue(), makeType(state, Cil_fmt_i32));
        }
    }

    return retval;
}

DisassemblerCil::ExpressionPair
DisassemblerCil::makeOffsetWidthPair(State &state, unsigned w1) const
{
    SgAsmExpression *offset=NULL, *width=NULL;
    if (extract<11, 11>(w1)) {
        if (extract<9, 11>(w1))
            throw Exception("bits 9 and 10 of extension word 1 should be zero when bit 11 is set", state.insn_va);
        offset = makeDataRegister(state, extract<6, 8>(w1), Cil_fmt_i32);
    } else {
        offset = makeImmediateValue(state, Cil_fmt_i8, extract<6, 10>(w1));
    }
    if (extract<5, 5>(w1)) {
        if (extract<3, 4>(w1))
            throw Exception("bits 3 and 4 of extension word 1 should be zero when bit 5 is set", state.insn_va);
        width = makeDataRegister(state, extract<0, 2>(w1), Cil_fmt_i32);
    } else {
        width = makeImmediateValue(state, Cil_fmt_i8, extract<0, 4>(w1));
    }
    return ExpressionPair(offset, width);
}

SgAsmInstruction *
DisassemblerCil::makeUnknownInstruction(const Disassembler::Exception &e)
{
    SgAsmCilInstruction *insn = new SgAsmCilInstruction(e.ip, "unknown", Cil_unknown_instruction);
    SgAsmOperandList *operands = new SgAsmOperandList;
    insn->set_operandList(operands);
    operands->set_parent(insn);

    if (e.bytes.empty()) {
        SgUnsignedCharList raw_bytes(1, '\0');          // we don't know what, but an instruction is never zero bytes wide
        insn->set_raw_bytes(raw_bytes);
    } else {
        insn->set_raw_bytes(e.bytes);
    }

    return insn;
}

#if 1
SgAsmCilInstruction *
DisassemblerCil::makeUnknownInstruction(rose_addr_t address, uint8_t opt_code)
{
    SgAsmCilInstruction *insn = new SgAsmCilInstruction(address, "unknown", Cil_unknown_instruction);
    SgAsmOperandList *operands = new SgAsmOperandList;
    insn->set_operandList(operands);
    operands->set_parent(insn);

 // SgUnsignedCharList raw_bytes(1, opt_code);          // we don't know what, but an instruction is never zero bytes wide
 // insn->set_raw_bytes(raw_bytes);

    return insn;
}
#endif

// DQ (10/20/2021): We don't need the State &state function parameter for CIL
// SgAsmCilInstruction *
// DisassemblerCil::makeInstruction(State &state, CilInstructionKind kind, const std::string &mnemonic,
//                                SgAsmExpression *op1, SgAsmExpression *op2, SgAsmExpression *op3,
//                                SgAsmExpression *op4, SgAsmExpression *op5, SgAsmExpression *op6,
//                                SgAsmExpression *op7) const
SgAsmCilInstruction *
DisassemblerCil::makeInstruction(rose_addr_t start_va, CilInstructionKind kind, const std::string &mnemonic,
                                  SgAsmExpression *op1, SgAsmExpression *op2, SgAsmExpression *op3,
                                  SgAsmExpression *op4, SgAsmExpression *op5, SgAsmExpression *op6,
                                  SgAsmExpression *op7, SgAsmExpression *op8) const
{
 // DQ (10/19/2021): NOTE: We will need to support up to 8 operands for some CIL functions, only the function parameter list has been modified to support the 8th operand.

    ASSERT_forbid2(Cil_unknown_instruction==kind, "should have called make_unknown_instruction instead");
 // SgAsmCilInstruction *insn = new SgAsmCilInstruction(state.insn_va, mnemonic, kind);
    SgAsmCilInstruction *insn = new SgAsmCilInstruction(start_va,mnemonic, kind);

    SgAsmOperandList *operands = new SgAsmOperandList;
    insn->set_operandList(operands);
    operands->set_parent(insn);

    /* If any operand is null, then the following operands must also be null because analysis assumes that the operand vector
     * indices correspond to argument positions and don't expect null-padding in the vector. */
    ASSERT_require2((!op1 && !op2 && !op3 && !op4 && !op5 && !op6 && !op7) ||
                    ( op1 && !op2 && !op3 && !op4 && !op5 && !op6 && !op7) ||
                    ( op1 &&  op2 && !op3 && !op4 && !op5 && !op6 && !op7) ||
                    ( op1 &&  op2 &&  op3 && !op4 && !op5 && !op6 && !op7) ||
                    ( op1 &&  op2 &&  op3 &&  op4 && !op5 && !op6 && !op7) ||
                    ( op1 &&  op2 &&  op3 &&  op4 &&  op5 && !op6 && !op7) ||
                    ( op1 &&  op2 &&  op3 &&  op4 &&  op5 &&  op6 && !op7) ||
                    ( op1 &&  op2 &&  op3 &&  op4 &&  op5 &&  op6 &&  op7),
                    "if any operand is null then all following operands must also be null");

 // DQ (10/20/2021): Assert this until we properly handle op8.
    ROSE_ASSERT(op8 == NULL);

    if (op1)
        SageBuilderAsm::appendOperand(insn, op1);
    if (op2)
        SageBuilderAsm::appendOperand(insn, op2);
    if (op3)
        SageBuilderAsm::appendOperand(insn, op3);
    if (op4)
        SageBuilderAsm::appendOperand(insn, op4);
    if (op5)
        SageBuilderAsm::appendOperand(insn, op5);
    if (op6)
        SageBuilderAsm::appendOperand(insn, op6);
    if (op7)
        SageBuilderAsm::appendOperand(insn, op7);

    return insn;
}


// Returns the Nth 16-bit word of an instruction, reading it from memory if necessary.  Instructions contain up to 11 16-bit
// words:
//      Single effective address operation word (one word, specifies operation and modes)
//          Single effective address operation word format:
//                 +----+----+----+----+
//              15 |xxxx|xxxx|xxMM|MRRR| 0
//                 +----+----+----+----+
//              MMM bits 3-5 are the effective address mode
//              RRR bits 0-2 are the effective address register
//              Bits 0-5 are collectively called the "effective address"
//          Brief extension word format
//                 +----+----+----+----+
//              15 |DRRR|WSS0|#########| 0
//                 +----+----+----+----+
//              D bit 15 is indicates data register (0) vs. address register (1)
//              R bits 12-14 are the data or address register number
//              W bit 11 is indicates index size: sign extended word (0) or long (1)
//              S bits 9-10 are the scale 1 (0), 2 (1), 4 (2), 8 (3)
//                  MC68000, MC68008, and MC68010 ignore the S bits and always assume zero.
//                  CPU32, MC68020, MC68030, and MC68040 use the S bits.
//              # bits 0-8 are the displacement
//          Full extension word format (MC68020, MC68030, MC68040)
//                 +----+----+----+----+
//              15 |DRRR|WSS1|BIZZ|0YYY| 0
//                 |base displacement  |        0, 1, or 2 words
//                 |outer displacement |        0, 1, or 2 words
//                 +----+----+----+----+
//              D bit 15 "D/A" indicates data register (0) vs. address register (1)
//              R bits 12-14 are the data or address register number
//              W bit 11 "W/L" is indicates index size: sign extended word (0) or long (1)
//              S bits 9-10 "SCALE" 1 (0), 2 (1), 4 (2), 8 (3)
//              B bit 7 "BS" is base register suppress: base register added (0) or suppressed (1)
//              I bit 6 "IS" is index suppress: eval and add index operand (0) or suppress index operand (1)
//              Z bits 4-5 "BD SIZE" are base displacement size: reserved (0), null (1), word (2), long (3)
//              Y bits 0-2 "I/IS": indirect and indexing operand determined in conjunction w/I bit 6, index suppress
//
//              |-------+----------+---------------------------------------------------|
//              |    IS |     I/IS | Operation                                         |
//              | bit 6 | bits 0-2 |                                                   |
//              |-------+----------+---------------------------------------------------|
//              |     0 |      000 | no memory indirect action                         |
//              |     0 |      001 | indirect preindexed with null outer displacement  |
//              |     0 |      010 | indirect preindexed with word outer displacement  |
//              |     0 |      011 | indirect preindexed with long outer displacement  |
//              |     0 |      100 | reserved                                          |
//              |     0 |      101 | indirect postindexed with null outer displacement |
//              |     0 |      110 | indirect postindexed with word outer displacement |
//              |     0 |      111 | indirect postindexed with long outer displacement |
//              |     1 |      000 | no memory indirect action                         |
//              |     1 |      001 | memory indirect with null outer displacement      |
//              |     1 |      010 | memory indirect with word outer displacement      |
//              |     1 |      011 | memory indirect with long outer displacement      |
//              |     1 |      1xx | reserved                                          |
//              |-------+----------+---------------------------------------------------|
//              
//      Special operand specifiers (if any, one or two words)
//      Immediate operand or source effective address extension (if any, one to six words)
//      Destination effective address extension (if any, one to six words)

uint16_t
DisassemblerCil::instructionWord(State &state, size_t n) const
{
    if (n>10)
        throw Exception("malformed instruction uses more than 11 extension words", state.insn_va);
    if (n>=state.niwords)
        throw Exception("short read for instruction word " + numberToString(n), state.insn_va);
    state.niwords_used = std::max(state.niwords_used, n+1);
    return state.iwords[n];
}

size_t
DisassemblerCil::extensionWordsUsed(State &state) const
{
    return state.niwords_used==0 ? 0 : state.niwords_used-1;
}

// see base class
SgAsmInstruction *
DisassemblerCil::disassembleOne(const MemoryMap::Ptr &map, rose_addr_t start_va, AddressSet *successors)
{
#define DEBUG_DISASSEMBLE_ONE 0

#if DEBUG_DISASSEMBLE_ONE || 0
  // DQ (10/18/2021): Tracing through the .Cil disassembly.
    printf ("At TOP of DisassemblerCil::disassembleOne \n");
 // ROSE_ASSERT(false);
#endif

    State state; // all mutable state for this function and its descendences is stored here.
    start_instruction(state, map, start_va);
    if (start_va % instructionAlignment_ != 0)
        throw Exception("instruction is not properly aligned", start_va);

#if DEBUG_DISASSEMBLE_ONE || 0
 // DQ (10/19/2021): Tracing through the CIL disassembly.
    printf ("declare buffer to hold largest possible instrution: sizeof(state.iwords) = %zu \n",sizeof(state.iwords));
#endif

    uint8_t buf[sizeof(state.iwords)]; // largest possible instruction

#if DEBUG_DISASSEMBLE_ONE
 // DQ (10/19/2021): Tracing through the CIL disassembly.
    printf ("declare nbytes: start_va = %p \n",start_va);
#endif

 // DQ (10/20/2021): This fills in the buffer (buf).
    size_t nbytes = map->at(start_va).limit(sizeof buf).require(MemoryMap::EXECUTABLE).read(buf).size();

#if DEBUG_DISASSEMBLE_ONE || 0
    for (size_t i=0; i < nbytes; ++i)
       {
      // DQ (10/19/2021): Tracing through the CIL disassembly.
         printf ("In loop over buf[%zu] = %04x \n",i,buf[i]);
       }
#endif

#if DEBUG_DISASSEMBLE_ONE
 // DQ (10/19/2021): Tracing through the CIL disassembly.
    printf ("nbytes = %zu sizeof(state.iwords[0]) = %zu \n",nbytes,sizeof(state.iwords[0]));
#endif

    state.niwords = nbytes / sizeof(state.iwords[0]);

#if DEBUG_DISASSEMBLE_ONE
 // DQ (10/19/2021): Tracing through the CIL disassembly.
    printf ("state.niwords = %zu \n",state.niwords);
#endif

    if (0==state.niwords)
        throw Exception("short read from memory map", start_va);

#if DEBUG_DISASSEMBLE_ONE
 // DQ (10/19/2021): Tracing through the CIL disassembly.
 // printf ("Loop over state.iwords array: state.iwords.size() = %zu \n",state.iwords.size());
    printf ("Loop over state.iwords array: sizeof(state.iwords) = %zu \n",sizeof(state.iwords));
#endif

    for (size_t i=0; i<state.niwords; ++i)
      {
#if DEBUG_DISASSEMBLE_ONE
     // DQ (10/19/2021): Tracing through the CIL disassembly.
        printf ("In loop over state.iwords array: i = %zu \n",i);
#endif

        state.iwords[i] = ByteOrder::be_to_host(*(uint16_t*)(buf+2*i));
      }

#if DEBUG_DISASSEMBLE_ONE
 // DQ (10/19/2021): Tracing through the CIL disassembly.
    printf ("DONE: Loop over state.iwords array: sizeof(state.iwords) = %zu \n",sizeof(state.iwords));
#endif

#if DEBUG_DISASSEMBLE_ONE
 // DQ (10/19/2021): Tracing through the CIL disassembly.
    printf ("sizeof(state.iwords) = %zu \n",sizeof(state.iwords));
#endif

    state.niwords_used = 1;

#if 0
  // DQ (10/19/2021): Tracing through the CIL disassembly.
    printf ("Calling find_idis() in DisassemblerCil::disassembleOne \n");
#endif

    SgAsmCilInstruction *insn = NULL;

#if DEBUG_DISASSEMBLE_ONE
 // DQ (10/19/2021): Tracing through the CIL disassembly.
    printf ("DONE: Calling find_idis() in DisassemblerCil::disassembleOne \n");
#endif

    ASSERT_require(state.niwords_used>0);

#if DEBUG_DISASSEMBLE_ONE || 0
 // DQ (10/19/2021): Tracing through the CIL disassembly.
    printf ("After assertion in DisassemblerCil::disassembleOne: buf = %p \n",buf);
    printf ("state.niwords_used = %zu \n",state.niwords_used);
#endif

 // SgUnsignedCharList raw_bytes(buf+0, buf+2*state.niwords_used);
    SgUnsignedCharList raw_bytes(buf+0, buf+9);

#if DEBUG_DISASSEMBLE_ONE || 0
 // DQ (10/19/2021): Tracing through the CIL disassembly.
    printf ("raw_bytes.size() = %zu \n",raw_bytes.size());
#endif

    ROSE_ASSERT(insn == NULL);

#if 0
 // insn = (*idis)(state, this, instructionWord(state, 0));
 // insn = new SgAsmCilInstruction();
 // insn = makeInstruction(state,Cil_nop,"nop");
    insn = makeInstruction(Cil_nop,"nop");
#endif

#if DEBUG_DISASSEMBLE_ONE || 1
    static int instruction_counter = 0;
#endif

    printf ("instruction_counter = %d \n",instruction_counter);
    instruction_counter++;

 // DQ (10/20/2021): Swithc statement over the opt code values.
    switch (buf[0])
      {
     // Add all the cases for different values of opt code here!
        case 0x00:
           {
#if 1
             printf ("Found Cil_nop instruction (no operation) \n");
#endif
             insn = makeInstruction(start_va,Cil_nop,"nop");

          // DQ (11/1/2021): Truncate the size of raw_bytes to the size of the instruction.
             raw_bytes.resize(1);

          // DQ (11/1/2021): Now set the raw_bytes data member to the resized local vector.
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x01:
           {
#if 1
             printf ("Found Cil_break instruction (break) \n");
#endif
             insn = makeInstruction(start_va,Cil_break,"break");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x02:
           {
#if 1
             printf ("Found Cil_ldarg_0 instruction (load argument 0 onto the stack) \n");
#endif
             insn = makeInstruction(start_va,Cil_ldarg_0,"ldarg_0");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x03:
           {
#if 1
             printf ("Found Cil_ldarg_1 instruction (load argument 1 onto the stack) \n");
#endif
             insn = makeInstruction(start_va,Cil_ldarg_0,"ldarg_1");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x04:
           {
#if 1
             printf ("Found Cil_ldarg_2 instruction (load argument 2 onto the stack) \n");
#endif
             insn = makeInstruction(start_va,Cil_ldarg_2,"ldarg_2");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x05:
           {
#if 1
             printf ("Found Cil_ldarg_3 instruction (load argument 3 onto the stack) \n");
#endif
             insn = makeInstruction(start_va,Cil_ldarg_0,"ldarg_3");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x06:
           {
#if 1
             printf ("Found Cil_ldloc_0 instruction (load local variable 0 onto stack) \n");
#endif
             insn = makeInstruction(start_va,Cil_ldloc_0,"ldloc_0");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x07:
           {
#if 1
             printf ("Found Cil_ldloc_1 instruction (load local variable 1 onto stack) \n");
#endif
             insn = makeInstruction(start_va,Cil_ldloc_1,"ldloc_1");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x08:
           {
#if 1
             printf ("Found Cil_ldloc_2 instruction (load local variable 2 onto stack) \n");
#endif
             insn = makeInstruction(start_va,Cil_ldloc_2,"ldloc_2");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x09:
           {
#if 1
             printf ("Found Cil_ldloc_3 instruction (load local variable 3 onto stack) \n");
#endif
             insn = makeInstruction(start_va,Cil_ldloc_3,"ldloc_3");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x0A:
           {
#if 1
             printf ("Found Cil_stloc_0 instruction (pop value from stack to local variable) \n");
#endif
             insn = makeInstruction(start_va,Cil_stloc_0,"stloc_0");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x0B:
           {
#if 1
             printf ("Found Cil_stloc_1 instruction (pop value from stack to local variable) \n");
#endif
             insn = makeInstruction(start_va,Cil_stloc_1,"stloc_1");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x0C:
           {
#if 1
             printf ("Found Cil_stloc_2 instruction (pop value from stack to local variable) \n");
#endif
             insn = makeInstruction(start_va,Cil_stloc_2,"stloc_2");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x0D:
           {
#if 1
             printf ("Found Cil_stloc_3 instruction (pop value from stack to local variable) \n");
#endif
             insn = makeInstruction(start_va,Cil_stloc_3,"stloc_3");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x0E:
           {
#if 1
             printf ("Found Cil_ldarg_s instruction (load argument numbered num onto the stack) \n");
#endif
          // DQ (11/1/2021): Need to read the second byte as a value to push onto the stack.
          // SgAsmIntegerValueExpression* operand = new SgAsmIntegerValueExpression(raw_bytes[1],NULL);

             int8_t value = ByteOrder::le_to_host(*((int8_t*)(raw_bytes.data()+1)));
             SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueI8(value);
             ROSE_ASSERT(operand != NULL);

             insn = makeInstruction(start_va,Cil_ldarg_s,"ldarg_s",operand);

             raw_bytes.resize(2);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x0F:
           {
#if 1
             printf ("Found Cil_ldarga_s instruction (fetch the address of the argument argNum) \n");
#endif
             uint8_t value = ByteOrder::le_to_host(*((uint8_t*)(raw_bytes.data()+1)));
             SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueU8(value);
             ROSE_ASSERT(operand != NULL);

             insn = makeInstruction(start_va,Cil_ldarga_s,"ldarga_s",operand);

             raw_bytes.resize(2);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x10:
           {
#if 1
             printf ("Found Cil_starg_s instruction (store value to the argument numbered num) \n");
#endif
          // DQ (11/1/2021): Need to read the second byte as a value to push onto the stack.
          // SgAsmIntegerValueExpression* operand = new SgAsmIntegerValueExpression(raw_bytes[1],NULL);

             uint8_t value = ByteOrder::le_to_host(*((uint8_t*)(raw_bytes.data()+1)));
             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueU8(value);
             ROSE_ASSERT(operand_0 != NULL);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_starg_s,"starg_s",operand_0,operand_1);

             raw_bytes.resize(2);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x11:
           {
#if 1
             printf ("Found Cil_ldloc_s instruction (load local variable of index onto stack) \n");
#endif
          // SgAsmIntegerValueExpression* operand = new SgAsmIntegerValueExpression(raw_bytes[1],NULL);

             uint8_t value = ByteOrder::le_to_host(*((uint8_t*)(raw_bytes.data()+1)));
             SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueU8(value);

             insn = makeInstruction(start_va,Cil_ldloc_s,"ldloc_s",operand);
             raw_bytes.resize(2);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x12:
           {
#if 1
             printf ("Found Cil_ldloca_s instruction (load local variable of index onto stack) \n");
#endif
          // SgAsmIntegerValueExpression* operand = new SgAsmIntegerValueExpression(raw_bytes[1],NULL);

             uint8_t value = ByteOrder::le_to_host(*((uint8_t*)(raw_bytes.data()+1)));
             SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueU8(value);

             insn = makeInstruction(start_va,Cil_ldloca_s,"ldloca_s",operand);
             raw_bytes.resize(2);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x13:
           {
#if 1
             printf ("Found Cil_stloc_s instruction (pop value from stack to local variable) \n");
#endif
          // DQ (11/1/2021): Need to get the second byte as the local variable to put the stack value.
          // SgAsmIntegerValueExpression* operand = new SgAsmIntegerValueExpression(raw_bytes[1],NULL);
          // SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueI32(value);
             uint8_t value = ByteOrder::le_to_host(*((uint8_t*)(raw_bytes.data()+1)));
             SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueU8(value);

             insn = makeInstruction(start_va,Cil_stloc_s,"stloc_s",operand);
             raw_bytes.resize(2);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x14:
           {
#if 1
             printf ("Found Cil_ldnull instruction (push a null reference on the stack) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldc_i4_m1,"ldnull");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x15:
           {
#if 1
             printf ("Found Cil_ldc_i4_m1 instruction (load numeric constant) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldc_i4_m1,"ldc_i4_m1");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x16:
           {
#if 1
             printf ("Found Cil_ldc_i4_0 instruction (load numeric constant) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldc_i4_0,"ldc_i4_0");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x17:
           {
#if 1
             printf ("Found Cil_ldc_i4_1 instruction (load numeric constant) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldc_i4_1,"ldc_i4_1");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x18:
           {
#if 1
             printf ("Found Cil_ldc_i4_2 instruction (load numeric constant) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldc_i4_2,"ldc_i4_2");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x19:
           {
#if 1
             printf ("Found Cil_ldc_i4_3 instruction (load numeric constant) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldc_i4_3,"ldc_i4_3");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x1A:
           {
#if 1
             printf ("Found Cil_ldc_i4_4 instruction (load numeric constant) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldc_i4_4,"ldc_i4_4");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x1B:
           {
#if 1
             printf ("Found Cil_ldc_i4_5 instruction (load numeric constant) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldc_i4_5,"ldc_i4_5");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x1C:
           {
#if 1
             printf ("Found Cil_ldc_i4_6 instruction (load numeric constant) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldc_i4_6,"ldc_i4_6");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x1D:
           {
#if 1
             printf ("Found Cil_ldc_i4_7 instruction (load numeric constant) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldc_i4_7,"ldc_i4_7");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x1E:
           {
#if 1
             printf ("Found Cil_ldc_i4_8 instruction (load numeric constant) \n");
#endif
             insn = makeInstruction(start_va,Cil_ldc_i4_8,"ldc_i4_8");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x1F:
           {
#if 1
             printf ("Found Cil_ldc_i4_s instruction (load numeric constant) \n");
#endif

          // DQ (11/1/2021): Need to read the second byte as a value to push onto the stack.
          // SgAsmIntegerValueExpression* operand = new SgAsmIntegerValueExpression(raw_bytes[1],NULL);
          // SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueI32(value);
             uint8_t value = ByteOrder::le_to_host(*((uint8_t*)(raw_bytes.data()+1)));
             SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueU8(value);

             insn = makeInstruction(start_va,Cil_ldc_i4_s,"ldc_i4_s",operand);
             raw_bytes.resize(2);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x20:
           {
#if 1
             printf ("Found Cil_ldc_i4 instruction (load numeric constant) \n");
#endif
          // DQ (11/1/2021): Need to read the second byte as a value to push onto the stack.
          // Convert data from one byte order to another in place.
          // void convert(void *bytes_, size_t nbytes, Endianness from, Endianness to);
          // int32_t le_to_host(int32_t n);

             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

          // SgAsmIntegerValueExpression* operand = new SgAsmIntegerValueExpression(value,NULL);
          // SgAsmIntegerValueExpression* operand = buildValueInteger((int64_t)value,NULL);
             SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueI32(value);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldc_i4,"ldc_i4",operand);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x21:
           {
#if 1
             printf ("Found Cil_ldc_i8 instruction (load numeric constant) \n");
#endif
          // DQ (11/1/2021): Need to read the second byte as a value to push onto the stack.
          // Convert data from one byte order to another in place.
          // void convert(void *bytes_, size_t nbytes, Endianness from, Endianness to);
          // int32_t le_to_host(int32_t n);

             int64_t value = ByteOrder::le_to_host(*((int64_t*)(raw_bytes.data()+1)));

          // SgAsmIntegerValueExpression* operand = new SgAsmIntegerValueExpression(value,NULL);
             SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueI64(value);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldc_i8,"ldc_i8",operand);
             raw_bytes.resize(9);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

#if 1
        case 0x22:
           {
#if 1
             printf ("Found Cil_ldc_r4 instruction (load numeric constant) \n");
#endif
          // DQ (11/1/2021): Need to read the second byte as a value to push onto the stack.
          // Convert data from one byte order to another in place.
          // void convert(void *bytes_, size_t nbytes, Endianness from, Endianness to);
          // int32_t le_to_host(int32_t n);

          // float32_t value = ByteOrder::le_to_host(*((float32_t*)(raw_bytes.data()+1)));
             uint32_t integer_value = ByteOrder::le_to_host(*((uint32_t*)(raw_bytes.data()+1)));
             float_t* pointer = (float_t*)(&integer_value);
             float_t float_value = *pointer;

             SgAsmFloatValueExpression* operand = SageBuilderAsm::buildValueIeee754Binary32(float_value);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldc_r4,"ldc_r4",operand);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }
#endif

        case 0x23:
           {
#if 1
             printf ("Found Cil_ldc_r8 instruction (load numeric constant) \n");
#endif
          // DQ (11/1/2021): Need to read the next several bytes as a value to push onto the stack.
          // float64_t value = ByteOrder::le_to_host(*((float64_t*)(raw_bytes.data()+1)));
             uint64_t integer_value = ByteOrder::le_to_host(*((uint64_t*)(raw_bytes.data()+1)));
             double_t* pointer = (double_t*)(&integer_value);
             double_t float_value = *pointer;

             SgAsmFloatValueExpression* operand = SageBuilderAsm::buildValueIeee754Binary64(float_value);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldc_r8,"ldc_r8",operand);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x25:
           {
#if 1
             printf ("Found Cil_dup instruction (duplicate the value on the top of the stack) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_dup,"dup",operand);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x26:
           {
#if 1
             printf ("Found Cil_pop instruction (remove the top element from the stack) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_pop,"pop");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x27:
           {
#if 1
             printf ("Found Cil_jmp instruction (exit current method and jump to the specified method) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueI32(value);

             insn = makeInstruction(start_va,Cil_jmp,"jmp",operand);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x28:
           {
#if 1
             printf ("Found Cil_call instruction (call a method) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueI32(value);

             insn = makeInstruction(start_va,Cil_call,"call",operand);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x29:
           {
#if 1
             printf ("Found Cil_callsitedescr instruction (call method indicated on the stack with arguments described by callsitedescr) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueI32(value);

             insn = makeInstruction(start_va,Cil_call,"call",operand);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x2A:
           {
#if 1
             printf ("Found Cil_ret instruction (return) \n");
#endif
          // This may or may not have an operand on the stack that it processes (need to look into this further).
             insn = makeInstruction(start_va,Cil_ret,"ret");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x2B:
           {
#if 1
             printf ("Found Cil_br_s instruction (branch to target) \n");
#endif
             int8_t value = ByteOrder::le_to_host(*((int8_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI8(value);

             insn = makeInstruction(start_va,Cil_br_s,"br_s",operand_0);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x2C:
           {
#if 1
             printf ("Found Cil_brfalse_s instruction (branch to target if value is zero (false)) \n");
#endif
             int8_t value = ByteOrder::le_to_host(*((int8_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI8(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_brfalse_s,"brfalse_s",operand_0,operand_1);
             raw_bytes.resize(2);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x2D:
           {
#if 1
             printf ("Found Cil_brtrue_s instruction (branch to target if value is zero (false)) \n");
#endif
             int8_t value = ByteOrder::le_to_host(*((int8_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI8(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_brtrue_s,"brtrue_s",operand_0,operand_1);
             raw_bytes.resize(2);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x2E:
           {
#if 1
             printf ("Found Cil_beq_s instruction (branch to target if equal to) \n");
#endif
             int8_t value = ByteOrder::le_to_host(*((int8_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI8(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_beq_s,"beq_s",operand_0,operand_1,operand_2);
             raw_bytes.resize(2);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x2F:
           {
#if 1
             printf ("Found Cil_bge_s instruction (branch to target if equal) \n");
#endif
             int8_t value = ByteOrder::le_to_host(*((int8_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI8(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_bge_s,"bge_s",operand_0,operand_1,operand_2);
             raw_bytes.resize(2);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x30:
           {
#if 1
             printf ("Found Cil_bgt_s instruction (branch to target if greater than) \n");
#endif
             int8_t value = ByteOrder::le_to_host(*((int8_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI8(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_bgt_s,"bgt_s",operand_0,operand_1,operand_2);
             raw_bytes.resize(2);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x31:
           {
#if 1
             printf ("Found Cil_ble_s instruction (branch to target if less than or equal to) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_bgt_s,"bgt_s");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x32:
           {
#if 1
             printf ("Found Cil_blt_s instruction (branch to target if less than) \n");
#endif
             int8_t value = ByteOrder::le_to_host(*((int8_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI8(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_blt_s,"blt_s",operand_0,operand_1,operand_2);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x33:
           {
#if 1
             printf ("Found Cil_bne_un_s instruction (branch to target if not equal) \n");
#endif
             int8_t value = ByteOrder::le_to_host(*((int8_t*)(raw_bytes.data()+1)));

          // SgAsmIntegerValueExpression* operand = new SgAsmIntegerValueExpression(value,NULL);
          // SgAsmIntegerValueExpression* operand = buildValueInteger((int64_t)value,NULL);
             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI8(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_bne_un_s,"bne_un_s",operand_0,operand_1,operand_2);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x34:
           {
#if 1
             printf ("Found Cil_bge_un_s instruction (branch to target if greater than or equal) \n");
#endif
             int8_t value = ByteOrder::le_to_host(*((int8_t*)(raw_bytes.data()+1)));

          // SgAsmIntegerValueExpression* operand = new SgAsmIntegerValueExpression(value,NULL);
          // SgAsmIntegerValueExpression* operand = buildValueInteger((int64_t)value,NULL);
             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI8(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_bge_un_s,"bge_un_s",operand_0,operand_1,operand_2);
             raw_bytes.resize(2);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x35:
           {
#if 1
             printf ("Found Cil_bgt_un instruction (branch on less than, unsigned or unordered) \n");
#endif
             int8_t value = ByteOrder::le_to_host(*((int8_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI8(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_bgt_un,"bgt_un",operand_0,operand_1,operand_2);
             raw_bytes.resize(2);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x36:
           {
#if 1
             printf ("Found Cil_ble_un_s instruction (branch to target if less than (unsigned or unordered)) \n");
#endif
             int8_t value = ByteOrder::le_to_host(*((int8_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI8(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_ble_un_s,"ble_un_s",operand_0,operand_1,operand_2);
             raw_bytes.resize(2);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x37:
           {
#if 1
             printf ("Found Cil_blt_un_s instruction (branch to target if less than (unsigned or unordered)) \n");
#endif
             int8_t value = ByteOrder::le_to_host(*((int8_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI8(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_blt_un_s,"blt_un_s",operand_0,operand_1,operand_2);
             raw_bytes.resize(2);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x38:
           {
#if 1
             printf ("Found Cil_br instruction (branch to target) \n");
#endif
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

             insn = makeInstruction(start_va,Cil_br,"br",operand_0);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x39:
           {
#if 1
             printf ("Found Cil_brfalse instruction (branch to target if value is zero (false)) \n");
#endif
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_brfalse,"brfalse",operand_0,operand_1);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x3A:
           {
#if 1
             printf ("Found Cil_brtrue instruction (branch to target if value is nonzero (true)) \n");
#endif
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_brtrue,"brtrue",operand_0,operand_1);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x3B:
           {
#if 1
             printf ("Found Cil_beq instruction (branch to target if equal) \n");
#endif
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_beq,"beq",operand_0,operand_1,operand_2);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x3C:
           {
#if 1
             printf ("Found Cil_bge instruction (branch to target if equal) \n");
#endif
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_bge,"bge",operand_0,operand_1,operand_2);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x3D:
           {
#if 1
             printf ("Found Cil_bgt instruction (branch to target if greater than) \n");
#endif
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_bgt,"bgt",operand_0,operand_1,operand_2);
             raw_bytes.resize(2);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x3E:
           {
#if 1
             printf ("Found Cil_ble instruction (branch to target if less than or equal) \n");
#endif
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_beq,"beq",operand_0,operand_1,operand_2);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x3F:
           {
#if 1
             printf ("Found Cil_blt instruction (branch to target if less than) \n");
#endif
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_blt,"blt",operand_0,operand_1,operand_2);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x40:
           {
#if 1
             printf ("Found Cil_bge_un instruction (branch to target if greater than or equal) \n");
#endif
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

          // SgAsmIntegerValueExpression* operand = new SgAsmIntegerValueExpression(value,NULL);
          // SgAsmIntegerValueExpression* operand = buildValueInteger((int64_t)value,NULL);
             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_bge_un,"bge_un",operand_0,operand_1,operand_2);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x41:
           {
#if 1
             printf ("Found Cil_bge_un instruction (branch to target if greater than or equal) \n");
#endif
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

          // SgAsmIntegerValueExpression* operand = new SgAsmIntegerValueExpression(value,NULL);
          // SgAsmIntegerValueExpression* operand = buildValueInteger((int64_t)value,NULL);
             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_bge_un_s,"bge_un_s",operand_0,operand_1,operand_2);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x42:
           {
#if 1
             printf ("Found Cil_bgt_un instruction (branch on greater than, unsigned or unordered) \n");
#endif
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_bgt_un,"bgt_un",operand_0,operand_1,operand_2);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x43:
           {
#if 1
             printf ("Found Cil_ble_un instruction (branch on less than, unsigned or unordered) \n");
#endif
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_ble_un,"ble_un",operand_0,operand_1,operand_2);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x44:
           {
#if 1
             printf ("Found Cil_blt_un instruction (branch to target if less than (unsigned or unordered)) \n");
#endif
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_blt_un,"blt_un",operand_0,operand_1,operand_2);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x45:
           {
#if 1
             printf ("Found Cil_switch instruction (jump to one of n values)) \n");
#endif
             uint32_t value_0 = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));
             int32_t value_1  = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueU32(value_0);
             SgAsmIntegerValueExpression* operand_1 = SageBuilderAsm::buildValueI32(value_1);

             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_switch,"switch",operand_0,operand_1,operand_2);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x46:
           {
#if 1
             printf ("Found Cil_ldind_i1 instruction (load value indirect onto stack) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldind_i1,"ldind_i1",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x47:
           {
#if 1
             printf ("Found Cil_ldind_u1 instruction (load value indirect onto stack) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldind_u1,"ldind_u1",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x48:
           {
#if 1
             printf ("Found Cil_ldind_i2 instruction (load value indirect onto stack) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldind_i2,"ldind_i2",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x49:
           {
#if 1
             printf ("Found Cil_ldind_u2 instruction (load value indirect onto stack) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldind_u2,"ldind_u2",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x4A:
           {
#if 1
             printf ("Found Cil_ldind_i4 instruction (load value indirect onto stack) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldind_i4,"ldind_i4",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x4B:
           {
#if 1
             printf ("Found Cil_ldind_i4 instruction (load value indirect onto stack) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldind_i4,"ldind_i4",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x4C:
           {
#if 1
             printf ("Found Cil_ldind_u8 instruction (load value indirect onto stack) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldind_u8,"ldind_u8",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x4D:
           {
#if 1
             printf ("Found Cil_ldind_i instruction (load value indirect onto stack) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldind_i,"ldind_i",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x4E:
           {
#if 1
             printf ("Found Cil_ldind_r4 instruction (load value indirect onto stack) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldind_r4,"ldind_r4",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x4F:
           {
#if 1
             printf ("Found Cil_ldind_r8 instruction (load value indirect onto stack) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldind_r8,"ldind_r8",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x50:
           {
#if 1
             printf ("Found Cil_ldind_ref instruction (load value indirect onto stack) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_ldind_ref,"ldind_ref",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x51:
           {
#if 1
             printf ("Found Cil_stind_ref instruction (store value of type object ref (type o) into memory at address) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_stind_ref,"stind_ref",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x52:
           {
#if 1
             printf ("Found Cil_stind_i1 instruction (store value of type int8 into memory at address) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_stind_i1,"stind_i1",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x53:
           {
#if 1
             printf ("Found Cil_stind_i2 instruction (store value of type int16 into memory at address) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_stind_i2,"stind_i2",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x54:
           {
#if 1
             printf ("Found Cil_stind_i4 instruction (store value of type int32 into memory at address) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_stind_i4,"stind_i4",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x55:
           {
#if 1
             printf ("Found Cil_stind_i8 instruction (store value of type int64 into memory at address) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_stind_i8,"stind_i8",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x56:
           {
#if 1
             printf ("Found Cil_stind_r4 instruction (store value of type float32 into memory at address) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_stind_r4,"stind_r4",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x57:
           {
#if 1
             printf ("Found Cil_stind_r8 instruction (store value of type float64 into memory at address) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_stind_r8,"stind_r8",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x58:
           {
#if 1
             printf ("Found Cil_add instruction (add values on stack and replace with result onto stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_add,"add",operand_0,operand_1);

          // DQ (11/1/2021): Truncate the size of raw_bytes to the size of the instruction.
             raw_bytes.resize(1);

          // DQ (11/1/2021): Now set the raw_bytes data member to the resized local vector.
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x59:
           {
#if 1
             printf ("Found Cil_sub instruction (sub values on stack and replace with result onto stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_sub,"sub",operand_0,operand_1);

          // DQ (11/1/2021): Truncate the size of raw_bytes to the size of the instruction.
             raw_bytes.resize(1);

          // DQ (11/1/2021): Now set the raw_bytes data member to the resized local vector.
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x5A:
           {
#if 1
             printf ("Found Cil_mul instruction (multiply values on stack and replace with result onto stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_mul,"mul",operand_0,operand_1);

          // DQ (11/1/2021): Truncate the size of raw_bytes to the size of the instruction.
             raw_bytes.resize(1);

          // DQ (11/1/2021): Now set the raw_bytes data member to the resized local vector.
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x5B:
           {
#if 1
             printf ("Found Cil_div instruction (divide values on stack and replace with result onto stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_div,"div",operand_0,operand_1);

          // DQ (11/1/2021): Truncate the size of raw_bytes to the size of the instruction.
             raw_bytes.resize(1);

          // DQ (11/1/2021): Now set the raw_bytes data member to the resized local vector.
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x5C:
           {
#if 1
             printf ("Found Cil_div_un instruction (divide values on stack and replace with result onto stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_div_un,"div_un",operand_0,operand_1);

          // DQ (11/1/2021): Truncate the size of raw_bytes to the size of the instruction.
             raw_bytes.resize(1);

          // DQ (11/1/2021): Now set the raw_bytes data member to the resized local vector.
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x5D:
           {
#if 1
             printf ("Found Cil_rem instruction (remiander values on stack and replace with result onto stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_rem,"rem",operand_0,operand_1);

          // DQ (11/1/2021): Truncate the size of raw_bytes to the size of the instruction.
             raw_bytes.resize(1);

          // DQ (11/1/2021): Now set the raw_bytes data member to the resized local vector.
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x5E:
           {
#if 1
             printf ("Found Cil_rem_un instruction (remainder values on stack and replace with result onto stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_rem_un,"rem_un",operand_0,operand_1);

          // DQ (11/1/2021): Truncate the size of raw_bytes to the size of the instruction.
             raw_bytes.resize(1);

          // DQ (11/1/2021): Now set the raw_bytes data member to the resized local vector.
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x5F:
           {
#if 1
             printf ("Found Cil_and instruction (and values on stack and replace with result onto stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_and,"and",operand_0,operand_1);

          // DQ (11/1/2021): Truncate the size of raw_bytes to the size of the instruction.
             raw_bytes.resize(1);

          // DQ (11/1/2021): Now set the raw_bytes data member to the resized local vector.
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x60:
           {
#if 1
             printf ("Found Cil_or instruction (or values on stack and replace with result onto stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_or,"or",operand_0,operand_1);

          // DQ (11/1/2021): Truncate the size of raw_bytes to the size of the instruction.
             raw_bytes.resize(1);

          // DQ (11/1/2021): Now set the raw_bytes data member to the resized local vector.
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x61:
           {
#if 1
             printf ("Found Cil_xor instruction (or values on stack and replace with result onto stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             insn = makeInstruction(start_va,Cil_xor,"xor",operand_0,operand_1);

          // DQ (11/1/2021): Truncate the size of raw_bytes to the size of the instruction.
             raw_bytes.resize(1);

          // DQ (11/1/2021): Now set the raw_bytes data member to the resized local vector.
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x62:
           {
#if 1
             printf ("Found Cil_shl instruction (shift an integer left (shift in zero), return an integer) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_shl,"shl",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x63:
           {
#if 1
             printf ("Found Cil_shr instruction (shift an integer right (shift in zero), return an integer) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_shr,"shr",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x64:
           {
#if 1
             printf ("Found Cil_shr_un instruction (shift an integer right (shift in zero), return an integer) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_shr_un,"shr_un",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x65:
           {
#if 1
             printf ("Found Cil_neg instruction (negates value from stack and pushes new value onto the stack) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_neg,"neg",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x66:
           {
#if 1
             printf ("Found Cil_not instruction (bitwise complement) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_not,"not",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x67:
           {
#if 1
             printf ("Found Cil_conv_i1 instruction (data conversion to int8, pushing int32 on stack) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_i1,"conv_i1",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x68:
           {
#if 1
             printf ("Found Cil_conv_i2 instruction (data conversion to int16, pushing int32 on stack) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_i1,"conv_i1",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x69:
           {
#if 1
             printf ("Found Cil_conv_i4 instruction (data conversion to int32, pushing int32 on stack) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_i1,"conv_i1",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x6A:
           {
#if 1
             printf ("Found Cil_conv_i8 instruction (data conversion to int64, pushing int64 on stack) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_i1,"conv_i1",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x6B:
           {
#if 1
             printf ("Found Cil_conv_r4 instruction (data conversion to float32, pushing F on stack) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_i1,"conv_i1",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x6C:
           {
#if 1
             printf ("Found Cil_conv_r8 instruction (data conversion to float64, pushing F on stack) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_r8,"conv_r8",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x6D:
           {
#if 1
             printf ("Found Cil_conv_u4 instruction (data conversion to int32, pushing int32 on stack) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_u4,"conv_u4",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x6E:
           {
#if 1
             printf ("Found Cil_conv_u8 instruction (data conversion to unsigned int64, pushing int64 on stack) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_u8,"conv_u8",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x6F:
           {
#if 1
             printf ("Found Cil_callvirt instruction (call a method associated with an object) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

          // Unclear how we determine the number of parameters at present.
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_callvirt,"callvirt",operand_0,operand_1);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x70:
           {
#if 1
             printf ("Found Cil_cpobj instruction (copy a value type from src to dist) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(0);

          // Need to implement support for src and dist as addresss (pointers).
          // Need to handle type tokens.
             insn = makeInstruction(start_va,Cil_cpobj,"cpobj",operand_0,operand_1,operand_2);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x71:
           {
#if 1
             printf ("Found Cil_ldobj instruction (copy a valuestored at address src to the stack) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);

          // Need to implement support for src and dist as addresss (pointers).
          // Need to handle type tokens.
             insn = makeInstruction(start_va,Cil_ldobj,"ldobj",operand_0,operand_1);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x72:
           {
#if 1
             printf ("Found Cil_ldstr instruction (push a string object for the literal string) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueI32(value);

             insn = makeInstruction(start_va,Cil_ldstr,"ldstr",operand);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x73:
           {
#if 1
             printf ("Found Cil_newobj instruction (allocate an uninitialized object or value type and call ctor) \n");
#endif
             insn = makeInstruction(start_va,Cil_newobj,"newobj");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x74:
           {
#if 1
             printf ("Found Cil_castclass instruction (cast obj to a typeTok) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_castclass,"castclass",operand_0,operand_1);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x75:
           {
#if 1
             printf ("Found Cil_isinst instruction (test if obj is an instance of typeTok, returning null or an instance of theat class or interface) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_isinst,"isinst",operand_0,operand_1);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x76:
           {
#if 1
             printf ("Found Cil_conv_r_un instruction (data conversion unsigned integer, to floating-point,pushing F on stack) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_r_un,"conv_r_un",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

     // Note: instruction 0x77 - 0x78 are not legal instructions.

        case 0x79:
           {
#if 1
             printf ("Found Cil_unbox instruction (extract a value-type from obj, its boxed representation) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_unbox,"unbox",operand_0,operand_1);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x7A:
           {
#if 1
             printf ("Found Cil_throw instruction (throw an exception) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_throw,"throw",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x7B:
           {
#if 1
             printf ("Found Cil_ldfld instruction (push the address of field of object (or value type), onto the stack) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_ldfld,"ldfld",operand_0,operand_1);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x7C:
           {
#if 1
             printf ("Found Cil_ldflda instruction (push the address of field of object (or value type), onto the stack) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_ldflda,"ldflda",operand_0,operand_1);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x7D:
           {
#if 1
             printf ("Found Cil_stfld instruction (replace the value of field of the object obj with value) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_stfld,"stfld",operand_0,operand_1,operand_2);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x7E:
           {
#if 1
             printf ("Found Cil_ldsfld instruction (push the value of field on the stack) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

             insn = makeInstruction(start_va,Cil_ldsfld,"ldsfld",operand_0);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x7F:
           {
#if 1
             printf ("Found Cil_ldsflda instruction (push the address of the static field, field, on the stack) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

             insn = makeInstruction(start_va,Cil_ldsflda,"ldsflda",operand_0);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x80:
           {
#if 1
             printf ("Found Cil_stsfld instruction (replace the value of filed with val) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_stsfld,"stsfld",operand_0,operand_1);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x81:
           {
#if 1
             printf ("Found Cil_stobj instruction (store a value of type typeTok at an address) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueI32(value);

             insn = makeInstruction(start_va,Cil_stobj,"stobj",operand);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x82:
           {
#if 1
             printf ("Found Cil_conv_ovf_i1_un instruction (unsigned data conversion to int8 (on stack as int32) with overflow detection) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_ovf_i1_un,"conv_ovf_i1_un",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x83:
           {
#if 1
             printf ("Found Cil_conv_ovf_i2_un instruction (unsigned data conversion to int16 (on stack as int32) with overflow detection) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_ovf_i2_un,"conv_ovf_i2_un",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x84:
           {
#if 1
             printf ("Found Cil_conv_ovf_i4_un instruction (unsigned data conversion to int32 (on stack as int32) with overflow detection) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_ovf_i4_un,"conv_ovf_i4_un",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x85:
           {
#if 1
             printf ("Found Cil_conv_ovf_i8_un instruction (unsigned data conversion to int64 (on stack as int32) with overflow detection) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_ovf_i8_un,"conv_ovf_i8_un",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x86:
           {
#if 1
             printf ("Found Cil_conv_ovf_u1_un instruction (unsigned data conversion to unsigned int8 (on stack as int32) with overflow detection) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_ovf_u1_un,"conv_ovf_u1_un",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x87:
           {
#if 1
             printf ("Found Cil_conv_ovf_u2_un instruction (unsigned data conversion to unsigned int16 (on stack as int32) with overflow detection) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_ovf_u2_un,"conv_ovf_u2_un",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x88:
           {
#if 1
             printf ("Found Cil_conv_ovf_u4_un instruction (unsigned data conversion to unsigned int32 (on stack as int32) with overflow detection) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_ovf_u4_un,"conv_ovf_u4_un",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x89:
           {
#if 1
             printf ("Found Cil_conv_ovf_u8_un instruction (unsigned data conversion to unsigned int64 (on stack as int32) with overflow detection) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_ovf_u8_un,"conv_ovf_u8_un",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x8A:
           {
#if 1
             printf ("Found Cil_conv_ovf_i_un instruction (unsigned data conversion to native int (on stack as native int) with overflow detection) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_ovf_i_un,"conv_ovf_i_un",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x8B:
           {
#if 1
             printf ("Found Cil_conv_ovf_u_un instruction (unsigned data conversion to native unsigned int (on stack as native int) with overflow detection) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_ovf_u_un,"conv_ovf_u_un");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x8C:
           {
#if 1
             printf ("Found Cil_box instruction (convert a boxable value to its boxed form) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueI32(value);

             insn = makeInstruction(start_va,Cil_ldelema,"ldelema",operand);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x8D:
           {
#if 1
             printf ("Found Cil_newarr instruction (convert a new array with elements of type etype) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueI32(value);

             insn = makeInstruction(start_va,Cil_ldelema,"ldelema",operand);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x8E:
           {
#if 1
             printf ("Found Cil_ldlen instruction (push the length (of type native unsigned int) or array on the stack) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_ldlen,"ldlen");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x8F:
           {
#if 1
             printf ("Found Cil_ldlema instruction (load the address of element at index onto the top of the stack) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

             insn = makeInstruction(start_va,Cil_ldelema,"ldelema",operand_0);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x90:
           {
#if 1
             printf ("Found Cil_ldelem_i1 instruction (load the address of element at index onto the top of the stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_ldelem_i1,"ldelem_i1",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x91:
           {
#if 1
             printf ("Found Cil_ldelem_u1 instruction (load the address of element at index onto the top of the stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_ldelem_u1,"ldelem_u1",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x92:
           {
#if 1
             printf ("Found Cil_ldelem_i2 instruction (load the address of element at index onto the top of the stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_ldelem_i2,"ldelem_i2",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x93:
           {
#if 1
             printf ("Found Cil_ldelem_u2 instruction (load the address of element at index onto the top of the stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_ldelem_u2,"ldelem_u2",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x94:
           {
#if 1
             printf ("Found Cil_ldelem_i4 instruction (load the address of element at index onto the top of the stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_ldelem_i4,"ldelem_i4",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x95:
           {
#if 1
             printf ("Found Cil_ldelem_u4 instruction (load the address of element at index onto the top of the stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_ldelem_u4,"ldelem_u4",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x96:
           {
#if 1
             printf ("Found Cil_ldelem_i8 instruction (load the address of element at index onto the top of the stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_ldelem_i8,"ldelem_i8",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x97:
           {
#if 1
             printf ("Found Cil_ldelem_i instruction (load the element with type native into the top of the stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_ldelem_i,"ldelem_i",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x98:
           {
#if 1
             printf ("Found Cil_ldelem_r4 instruction (load numeric constant) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_ldelem_r4,"ldelem_r4",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x99:
           {
#if 1
             printf ("Found Cil_ldelem_r8 instruction (load numeric constant) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_ldelem_r8,"ldelem_r8",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x9A:
           {
#if 1
             printf ("Found Cil_ldelem_ref instruction (load the address of element at index onto the top of the stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_ldelem_ref,"ldelem_ref",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x9B:
           {
#if 1
             printf ("Found Cil_stelem_i instruction (replace array element at index with the native int value on the stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(2);

             insn = makeInstruction(start_va,Cil_stelem_i,"stelem_i",operand_0,operand_1,operand_2);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x9C:
           {
#if 1
             printf ("Found Cil_stelem_i1 instruction (replace array element at index with the native int value on the stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(2);

             insn = makeInstruction(start_va,Cil_stelem_i1,"stelem_i1",operand_0,operand_1,operand_2);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x9D:
           {
#if 1
             printf ("Found Cil_stelem_i2 instruction (replace array element at index with the native int value on the stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(2);

             insn = makeInstruction(start_va,Cil_stelem_i2,"stelem_i2",operand_0,operand_1,operand_2);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x9E:
           {
#if 1
             printf ("Found Cil_stelem_i4 instruction (replace array element at index with the native int value on the stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(2);

             insn = makeInstruction(start_va,Cil_stelem_i4,"stelem_i4",operand_0,operand_1,operand_2);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x9F:
           {
#if 1
             printf ("Found Cil_stelem_i8 instruction (replace array element at index with the native int value on the stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(2);

             insn = makeInstruction(start_va,Cil_stelem_i8,"stelem_i8",operand_0,operand_1,operand_2);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xA0:
           {
#if 1
             printf ("Found Cil_stelem_r4 instruction (replace array element at index with the native int value on the stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(2);

             insn = makeInstruction(start_va,Cil_stelem_r4,"stelem_r4",operand_0,operand_1,operand_2);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xA1:
           {
#if 1
             printf ("Found Cil_stelem_r8 instruction (replace array element at index with the native int value on the stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(2);

             insn = makeInstruction(start_va,Cil_stelem_r8,"stelem_r8",operand_0,operand_1,operand_2);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xA2:
           {
#if 1
             printf ("Found Cil_stelem_ref instruction (replace array element at index with the native int value on the stack) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(2);

             insn = makeInstruction(start_va,Cil_stelem_ref,"stelem_ref",operand_0,operand_1,operand_2);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xA3:
           {
#if 1
             printf ("Found Cil_ldelem instruction (load the element at index onto the top of the stack) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(2);

             insn = makeInstruction(start_va,Cil_ldelem,"ldelem",operand_0,operand_1,operand_2);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xA4:
           {
#if 1
             printf ("Found Cil_stelem instruction (replace array element at index with the value on the stack) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);
             SgAsmStackExpression* operand_2 = new SgAsmStackExpression(2);

             insn = makeInstruction(start_va,Cil_stelem,"stelem",operand_0,operand_1,operand_2);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xA5:
           {
#if 1
             printf ("Found Cil_unbox_any instruction (extract a value-type from obj, its boxed representation) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_unbox_any,"unbox_any",operand_0,operand_1);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

     // Note: instruction 0xA6 - 0xAF are not legal instructions.

     // Note: instruction 0xB0 - 0xB2 are not legal instructions.

        case 0xB3:
           {
#if 1
             printf ("Found Cil_conv_ovf_i1 instruction (convert to an int8 (on the stack as a int32) and throw exception on overflow) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_ovf_i1,"conv_ovf_i1",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xB4:
           {
#if 1
             printf ("Found Cil_conv_ovf_u1 instruction (convert to an unsigned int8 (on the stack as a int32) and throw exception on overflow) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_ovf_u1,"conv_ovf_u1",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xB5:
           {
#if 1
             printf ("Found Cil_conv_ovf_i2 instruction (convert to an int16 (on the stack as a int32) and throw exception on overflow) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_ovf_i2,"conv_ovf_i2",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xB6:
           {
#if 1
             printf ("Found Cil_conv_ovf_u2 instruction (convert to an unsigned int16 (on the stack as a int32) and throw exception on overflow) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_ovf_u2,"conv_ovf_u2",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xB7:
           {
#if 1
             printf ("Found Cil_conv_ovf_i4 instruction (convert to an int32 (on the stack as a int32) and throw exception on overflow) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_ovf_i4,"conv_ovf_i4",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xB8:
           {
#if 1
             printf ("Found Cil_conv_ovf_u4 instruction (convert to an unsigned int32 (on the stack as a int32) and throw exception on overflow) \n");
#endif
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_ovf_u4,"conv_ovf_u4",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xB9:
           {
#if 1
             printf ("Found Cil_conv_ovf_i8 instruction (convert to an int64 (on the stack as a int64) and throw exception on overflow) \n");
#endif
          // Is the stack arranged as 32bit values or 64bit values?
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_ovf_i8,"conv_ovf_i8",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xBA:
           {
#if 1
             printf ("Found Cil_conv_ovf_u8 instruction (convert to an unsigned int64 (on the stack as a int64) and throw exception on overflow) \n");
#endif
          // Is the stack arranged as 32bit values or 64bit values?
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_ovf_u8,"conv_ovf_u8",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

     // Note: instruction 0xBB - 0xBF are not legal instructions.

     // Note: instruction 0xC0 - 0xC1 are not legal instructions.

        case 0xC2:
           {
#if 1
             printf ("Found Cil_refanyval instruction (push the address stored in a typedef reference) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

          // Unclear how we determine the number of parameters at present.
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_ldtoken,"ldtoken",operand_0,operand_1);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xC3:
           {
#if 1
             printf ("Found Cil_ckfinite instruction (throw ArithmeticException if value is not a finite number) \n");
#endif
          // Is the stack arranged as 32bit values or 64bit values?
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_ckfinite,"ckfinite",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

     // Note: instruction 0xC4 - 0xC5 are not legal instructions.

        case 0xC6:
           {
#if 1
             printf ("Found Cil_mkrefany instruction (push a typed reference ot ptr of type class onto the stack) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

          // Unclear how we determine the number of parameters at present.
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_mkrefany,"mkrefany",operand_0,operand_1);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

     // Note: instruction 0xC7 - 0xCF are not legal instructions.

        case 0xD0:
           {
#if 1
             printf ("Found Cil_ldtoken instruction (convert metadata token to its runtime representation) \n");
#endif
          // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

             SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

          // Unclear how we determine the number of parameters at present.
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_ldtoken,"ldtoken",operand_0,operand_1);
             raw_bytes.resize(5);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xD1:
           {
#if 1
             printf ("Found Cil_conv_u2 instruction (data conversion to unsigned int16, pushing int32 on stack) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_u2,"conv_u2",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xD2:
           {
#if 1
             printf ("Found Cil_conv_u1 instruction (data conversion to unsigned int8, pushing int32 on stack) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_u1,"conv_u1",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xD3:
           {
#if 1
             printf ("Found Cil_conv_i instruction (data conversion to native int, pushing native int on stack) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_u2,"conv_u2",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xD4:
           {
#if 1
             printf ("Found Cil_conv_ovf_i instruction (convert to a native int (on the stack as a native int) and throw exception on overflow) \n");
#endif
          // Is the stack arranged as 32bit values or 64bit values?
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_ovf_i,"conv_ovf_i",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xD5:
           {
#if 1
             printf ("Found Cil_conv_ovf_u instruction (convert to a native unsigned int (on the stack as a native int) and throw exception on overflow) \n");
#endif
          // Is the stack arranged as 32bit values or 64bit values?
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_ovf_u,"conv_ovf_u",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xD6:
           {
#if 1
             printf ("Found Cil_add_ovf instruction (add signed integer values with overflow check) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_add_ovf,"add_ovf",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xD7:
           {
#if 1
             printf ("Found Cil_add_ovf_un instruction (add unsigned integer values with overflow check) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_add_ovf_un,"add_ovf_un",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xD8:
           {
#if 1
             printf ("Found Cil_mul_ovf instruction (multiply signed integer values with overflow check) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_mul_ovf,"mul_ovf",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xD9:
           {
#if 1
             printf ("Found Cil_mul_ovf_un instruction (multiply unsigned integer values with overflow check) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_mul_ovf_un,"mul_ovf_un",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xDA:
           {
#if 1
             printf ("Found Cil_sub_ovf instruction (subtract signed integer values with overflow check) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_sub_ovf,"sub_ovf",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xDB:
           {
#if 1
             printf ("Found Cil_sub_ovf_un instruction (subtract unsigned integer values with overflow check) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_sub_ovf_un,"sub_ovf_un",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xDC:
           {
          // Note that this is aliased to the endfault instruction.
#if 1
             printf ("Found Cil_endfinally (endfault) instruction (end finally clause of an exception block) \n");
#endif
             insn = makeInstruction(start_va,Cil_endfinally,"endfinally");
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xDD:
           {
#if 1
             printf ("Found Cil_leave instruction (Exit a protected region of code) \n");
#endif
             int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));
             SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueI32(value);

             insn = makeInstruction(start_va,Cil_leave,"leave",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xDE:
           {
#if 1
             printf ("Found Cil_leave_s instruction (Exit a protected region of code) \n");
#endif
             int8_t value = ByteOrder::le_to_host(*((int8_t*)(raw_bytes.data()+1)));
             SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueI8(value);

             insn = makeInstruction(start_va,Cil_leave_s,"leave_s",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xDF:
           {
#if 1
             printf ("Found Cil_stind_i instruction (store value of native int into memory at address) \n");
#endif
             SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
             SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

             insn = makeInstruction(start_va,Cil_stind_i,"stind_i",operand_0,operand_1);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xE0:
           {
#if 1
             printf ("Found Cil_conv_u instruction (data conversion native unsigned int, pushing native int on stack) \n");
#endif
          // This should have one operand which should be the target, not yet clear how to resolve 
          // the target, but it should be handled within the makeInstruction() function.
             SgAsmStackExpression* operand = new SgAsmStackExpression(0);

             insn = makeInstruction(start_va,Cil_conv_u,"conv_u",operand);
             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xFE:
           {
             printf ("Opt code prefix 0xFE: buf[0] = %04x \n",buf[0]);

             switch (raw_bytes[1])
                {
                  case 0x00:
                     {
#if 1
                       printf ("Found Cil_arglist instruction (return argument list handle for the current method) \n");
#endif
                       insn = makeInstruction(start_va,Cil_arglist,"arglist");
                       raw_bytes.resize(2);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x01:
                     {
#if 1
                       printf ("Found Cil_ceq instruction (push 1 (of type int32) if value 1 equals value2, else push 0) \n");
#endif
                       SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
                       SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

                       insn = makeInstruction(start_va,Cil_ceq,"ceq",operand_0,operand_1);

                       raw_bytes.resize(2);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x02:
                     {
#if 1
                       printf ("Found Cil_cgt instruction (push 1 (of type int32) if value1 > value2, else push 0) \n");
#endif
                       SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
                       SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

                       insn = makeInstruction(start_va,Cil_cgt,"cgt",operand_0,operand_1);

                       raw_bytes.resize(2);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x03:
                     {
#if 1
                       printf ("Found Cil_cgt_un instruction (push 1 (of type uint32) if value1 > value2, else push 0) \n");
#endif
                       SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
                       SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

                       insn = makeInstruction(start_va,Cil_cgt_un,"cgt_un",operand_0,operand_1);

                       raw_bytes.resize(2);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x04:
                     {
#if 1
                       printf ("Found Cil_clt instruction (push 1 (of type int32) if value1 < value2, else push 0) \n");
#endif
                       SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
                       SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

                       insn = makeInstruction(start_va,Cil_clt,"clt",operand_0,operand_1);

                       raw_bytes.resize(2);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x05:
                     {
#if 1
                       printf ("Found Cil_clt_un instruction (push 1 (of type uint32) if value1 < value2, else push 0) \n");
#endif
                       SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
                       SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);

                       insn = makeInstruction(start_va,Cil_clt_un,"clt_un",operand_0,operand_1);

                       raw_bytes.resize(2);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x06:
                     {
#if 1
                       printf ("Found Cil_ldftn instruction (push a pointer to a method referenced by method, on the stack) \n");
#endif
                    // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
                       int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

                       SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueI32(value);

                       insn = makeInstruction(start_va,Cil_ldftn,"ldftn",operand);
                       raw_bytes.resize(6);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x07:
                     {
#if 1
                       printf ("Found Cil_ldvirtftn instruction (push a pointer to a method referenced by method, on the stack) \n");
#endif
                    // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
                       int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

                       SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);
                       SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);

                       insn = makeInstruction(start_va,Cil_ldvirtftn,"ldvirtftn",operand_0,operand_1);
                       raw_bytes.resize(6);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

               // Note: instruction 0x08 is not a legal instruction.

                  case 0x09:
                     {
#if 1
                       printf ("Found Cil_ldarg instruction (load argument numbered num onto the stack) \n");
#endif
                    // DQ (11/1/2021): Need to read the second byte as a value to push onto the stack.
                       uint16_t value = ByteOrder::le_to_host(*((uint16_t*)(raw_bytes.data()+1)));

                       SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueU16(value);

                       insn = makeInstruction(start_va,Cil_ldarg_s,"ldarg_s",operand);
                       raw_bytes.resize(4);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x0A:
                     {
#if 1
                       printf ("Found Cil_ldarga instruction (fetch the address of the argument argNum) \n");
#endif
                       uint16_t value = ByteOrder::le_to_host(*((uint16_t*)(raw_bytes.data()+1)));
                       SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueU16(value);
                       ROSE_ASSERT(operand != NULL);

                       insn = makeInstruction(start_va,Cil_ldarga,"ldarga",operand);

                       raw_bytes.resize(4);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x0B:
                     {
#if 1
                       printf ("Found Cil_starg instruction (store value to the argument numbered num) \n");
#endif
                       uint16_t value = ByteOrder::le_to_host(*((uint16_t*)(raw_bytes.data()+1)));
                       SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueU16(value);
                       ROSE_ASSERT(operand_0 != NULL);

                       SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);

                       insn = makeInstruction(start_va,Cil_starg,"starg",operand_0,operand_1);

                       raw_bytes.resize(4);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x0C:
                     {
#if 1
                       printf ("Found Cil_ldloc instruction (load local variable of index onto stack) \n");
#endif
                       uint16_t value = ByteOrder::le_to_host(*((uint16_t*)(raw_bytes.data()+1)));

                    // SgAsmIntegerValueExpression* operand = new SgAsmIntegerValueExpression(value,NULL);
                       SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueU16(value);

                       insn = makeInstruction(start_va,Cil_ldloc,"ldloc",operand);
                       raw_bytes.resize(4);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x0D:
                     {
#if 1
                       printf ("Found Cil_ldloca_s instruction (load local variable of index onto stack) \n");
#endif
                    // SgAsmIntegerValueExpression* operand = new SgAsmIntegerValueExpression(raw_bytes[1],NULL);

                       uint8_t value = ByteOrder::le_to_host(*((uint8_t*)(raw_bytes.data()+1)));
                       SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueU8(value);

                       insn = makeInstruction(start_va,Cil_ldloca_s,"ldloca_s",operand);
                       raw_bytes.resize(3);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x0E:
                     {
#if 1
                       printf ("Found Cil_stloc instruction (pop value from stack to local variable) \n");
#endif
                    // DQ (11/1/2021): Need to get the second byte as the local variable to put the stack value.
                    // SgAsmIntegerValueExpression* operand = new SgAsmIntegerValueExpression(raw_bytes[1],NULL);
                    // SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueI32(value);
                       uint16_t value = ByteOrder::le_to_host(*((uint16_t*)(raw_bytes.data()+1)));
                       SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueU16(value);

                       insn = makeInstruction(start_va,Cil_stloc,"stloc",operand);
                       raw_bytes.resize(4);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x0F:
                     {
#if 1
                       printf ("Found Cil_localloc instruction (allocate space from the local memory pool) \n");
#endif
                       insn = makeInstruction(start_va,Cil_localloc,"localloc");
                       raw_bytes.resize(2);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

               // case 0x0D ... 0x1E:
                  case 0x10:
                     {
                    // printf ("Opt code using prefix 0x0D to 0x1E are illegal instructions (calling makeUnknownInstruction): (prefix) 0xFE buf[1] = %04x \n",buf[1]);
                       printf ("Opt code using prefix 0x10 is an illegal instruction (calling makeUnknownInstruction): (prefix) 0xFE buf[1] = %04x \n",buf[1]);

                       insn = makeUnknownInstruction(start_va,buf[0]);

                       raw_bytes.resize(2);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x11:
                     {
#if 1
                       printf ("Found Cil_endfilter instruction (end an exception handling filter clause) \n");
#endif
                       SgAsmStackExpression* operand = new SgAsmStackExpression(0);

                       insn = makeInstruction(start_va,Cil_endfilter,"endfilter",operand);
                       raw_bytes.resize(2);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x12:
                     {
#if 1
                       printf ("Found Cil_unaligned instruction (subsequence pointer instruction might be unaligned) \n");
#endif
                       uint8_t value = ByteOrder::le_to_host(*((uint8_t*)(raw_bytes.data()+1)));

                       SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueU8(value);

                       SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);

                       insn = makeInstruction(start_va,Cil_unaligned,"unaligned",operand_0,operand_1);
                       raw_bytes.resize(3);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x13:
                     {
#if 1
                       printf ("Found Cil_volatile instruction (subsequent pointer reference is volatile) \n");
#endif
                       SgAsmStackExpression* operand = new SgAsmStackExpression(0);

                       insn = makeInstruction(start_va,Cil_volatile,"volatile",operand);
                       raw_bytes.resize(2);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x14:
                     {
#if 1
                       printf ("Found Cil_tail instruction (subsequent call terminates current method) \n");
#endif
                       insn = makeInstruction(start_va,Cil_tail,"tail");
                       raw_bytes.resize(2);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x15:
                     {
#if 1
                       printf ("Found Cil_initobj instruction (initialize the value at address dest) \n");
#endif
                    // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
                       int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

                       SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

                       SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);

                       insn = makeInstruction(start_va,Cil_initobj,"initobj",operand_0,operand_1);
                       raw_bytes.resize(6);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x16:
                     {
#if 1
                       printf ("Found Cil_constrained instruction (call a virtual method on a type constrained to be type T) \n");
#endif
                    // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
                       int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

                       SgAsmIntegerValueExpression* operand_0 = SageBuilderAsm::buildValueI32(value);

                    // Not clear how to handle variable number of stack parameters.
                       SgAsmStackExpression* operand_1 = new SgAsmStackExpression(0);
                       SgAsmStackExpression* operand_2 = new SgAsmStackExpression(1);

                       insn = makeInstruction(start_va,Cil_constrained,"constrained",operand_0,operand_1,operand_2);
                       raw_bytes.resize(6);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x17:
                     {
#if 1
                       printf ("Found Cil_cpblk instruction (copy data from memory to memory) \n");
#endif
                       SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
                       SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);
                       SgAsmStackExpression* operand_2 = new SgAsmStackExpression(2);

                       insn = makeInstruction(start_va,Cil_cpblk,"cpblk",operand_0,operand_1,operand_2);
                       raw_bytes.resize(2);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x18:
                     {
#if 1
                       printf ("Found Cil_initblk instruction (set all bytes in a block of memory to a given byte value) \n");
#endif
                       SgAsmStackExpression* operand_0 = new SgAsmStackExpression(0);
                       SgAsmStackExpression* operand_1 = new SgAsmStackExpression(1);
                       SgAsmStackExpression* operand_2 = new SgAsmStackExpression(2);

                       insn = makeInstruction(start_va,Cil_initblk,"initblk",operand_0,operand_1,operand_2);
                       raw_bytes.resize(2);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x19:
                     {
#if 1
                       printf ("Found Cil_no instruction (the specified fault check(s) normally performed as part of the execution of the subsequent instruction can/shall be skipped) \n");
#endif
                       uint8_t value = ByteOrder::le_to_host(*((uint8_t*)(raw_bytes.data()+1)));

                       SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueU8(value);

                       insn = makeInstruction(start_va,Cil_no,"no",operand);
                       raw_bytes.resize(3);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x1A:
                     {
#if 1
                       printf ("Found Cil_rethrow instruction (rethrow the current exception) \n");
#endif
                       insn = makeInstruction(start_va,Cil_rethrow,"rethrow");
                       raw_bytes.resize(2);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x1C:
                     {
#if 1
                       printf ("Found Cil_sizeof instruction (push the size, in bytes, of a type as an unsigned int32) \n");
#endif
                    // DQ (11/3/2021): This is formated as: 72 <T>, where <T> is a "metadata token, encoded as a 4-byte integer.
                       int32_t value = ByteOrder::le_to_host(*((int32_t*)(raw_bytes.data()+1)));

                       SgAsmIntegerValueExpression* operand = SageBuilderAsm::buildValueI32(value);

                       insn = makeInstruction(start_va,Cil_sizeof,"sizeof",operand);
                       raw_bytes.resize(6);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x1D:
                     {
#if 1
                       printf ("Found Cil_refanytype instruction (push the type token stored in a typed reference) \n");
#endif
                       SgAsmStackExpression* operand = new SgAsmStackExpression(0);

                       insn = makeInstruction(start_va,Cil_refanytype,"refanytype",operand);
                       raw_bytes.resize(2);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x1E:
                     {
#if 1
                       printf ("Found Cil_readonly instruction (specify that the subsequent array address operation performs no type check at runtime, and that it returns a controlled-mutability managed pointer) \n");
#endif
                       insn = makeInstruction(start_va,Cil_readonly,"readonly");
                       raw_bytes.resize(2);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  case 0x08:
                  case 0x1B:
                  case 0x1F ... 0xFF:
                     {
                       printf ("Opt code using prefix 0x1B and 0x1F - 0xFF are illegal instructions (calling makeUnknownInstruction): buf[1] = %04x \n",buf[1]);

                       insn = makeUnknownInstruction(start_va,buf[0]);

                       raw_bytes.resize(1);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }

                  default:
                     {
                       printf ("Opt code using prefix 0xFE not handled: buf[1] = %04x \n",buf[1]);

                       ROSE_ASSERT(false);

                    // DQ (10/20/2021): Fill in with a Cil_nop instruction for now so that we can process the whole file.
                       insn = makeInstruction(start_va,Cil_nop,"nop");

                       raw_bytes.resize(1);
                       insn->set_raw_bytes(raw_bytes);
                       break;
                     }
                }
           }

        case 0x24:
           {
             printf ("Opt code using prefix 0x24 is an illegal instruction (calling makeUnknownInstruction): buf[1] = %04x \n",buf[1]);

             insn = makeUnknownInstruction(start_va,buf[0]);

             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0x77 ... 0x78:
           {
             printf ("Opt code using prefix 0x77 to 0x78 are illegal instructions (calling makeUnknownInstruction): buf[1] = %04x \n",buf[1]);

             insn = makeUnknownInstruction(start_va,buf[0]);

             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xA6 ... 0xAF:
           {
             printf ("Opt code using prefix 0xA6 to 0xAF are illegal instructions (calling makeUnknownInstruction): buf[1] = %04x \n",buf[1]);

             insn = makeUnknownInstruction(start_va,buf[0]);

             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xB0 ... 0xB2:
        case 0xBB ... 0xBF:
           {
             printf ("Opt code using prefix 0xB0 to 0xB2 or 0xBB to 0xBF are illegal instructions (calling makeUnknownInstruction): buf[1] = %04x \n",buf[1]);

             insn = makeUnknownInstruction(start_va,buf[0]);

             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xC0 ... 0xC1:
        case 0xC4 ... 0xC5:
        case 0xC7 ... 0xCF:
           {
             printf ("Opt code using prefix 0xC0 to 0xC1 or 0xC7 to 0xCF are illegal instructions (calling makeUnknownInstruction): buf[1] = %04x \n",buf[1]);

             insn = makeUnknownInstruction(start_va,buf[0]);

             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xE1 ... 0xEF:
           {
             printf ("Opt code using prefix 0xE1 to 0xEF are illegal instructions (calling makeUnknownInstruction): buf[1] = %04x \n",buf[1]);

             insn = makeUnknownInstruction(start_va,buf[0]);

             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xF0 ... 0xFD:
           {
             printf ("Opt code using prefix 0xF0 to 0xFD are illegal instructions (calling makeUnknownInstruction): buf[1] = %04x \n",buf[1]);

             insn = makeUnknownInstruction(start_va,buf[0]);

             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        case 0xFF:
           {
             printf ("Opt code using prefix 0xFF is an illegal instructions (calling makeUnknownInstruction): buf[1] = %04x \n",buf[1]);

             insn = makeUnknownInstruction(start_va,buf[0]);

             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }

        default:
           {
             printf ("Opt code not handled: buf[0] = %04x (building as Cil_unknown_instruction) \n",buf[0]);

             ROSE_ASSERT(false);

          // DQ (10/20/2021): Fill in with a Cil_nop instruction for now so that we can process the whole file.
          // insn = makeInstruction(start_va,Cil_nop,"nop");
          // insn = makeInstruction(start_va,Cil_unknown_instruction,"unknown_instruction");
             insn = makeUnknownInstruction(start_va,buf[0]);

             raw_bytes.resize(1);
             insn->set_raw_bytes(raw_bytes);
             break;
           }
      }

    if (insn == NULL)
       {
         printf ("No instruction built: Opt code not handled: buf[0] = %04x \n",buf[0]);
         ROSE_ASSERT(insn != NULL);
       }
    // DQ (11/1/2021): To support the variable length instructions we will move this into each case above.
      else
       {
#if 0
         insn->set_raw_bytes(raw_bytes);
#endif
         ROSE_ASSERT(insn->get_raw_bytes().size() > 0);
       }

#if 0
    printf ("Exiting as a test! \n");
    ROSE_ASSERT(false);
#endif

#if DEBUG_DISASSEMBLE_ONE || 0
 // DQ (10/19/2021): Tracing through the CIL disassembly.
    printf ("(after processing) raw_bytes.size() = %zu \n",raw_bytes.size());
#endif

#if DEBUG_DISASSEMBLE_ONE
 // DQ (10/19/2021): Tracing through the CIL disassembly.
    printf ("successors = %s \n",successors ? "true" : "false");
#endif

    if (successors) {
        bool complete;
        *successors |= insn->getSuccessors(complete/*out*/);
    }

#if DEBUG_DISASSEMBLE_ONE
 // DQ (10/19/2021): Tracing through the CIL disassembly.
    printf ("After conditional: successors = %s \n",successors ? "true" : "false");
#endif

#if DEBUG_DISASSEMBLE_ONE
  // DQ (10/18/2021): Tracing through the CIL disassembly.
    printf ("Leaving DisassemblerCil::disassembleOne \n");
#endif

    return insn;
}

void
DisassemblerCil::insert_idis(Cil *idis)
{
    ASSERT_not_null(idis);

    // Figure out which idis list to use based on the most significant nybble of the pattern for the first 16-bit opword (these
    // bits are usually invariant for a given instruction).  Entries 0-15 inclusive are for the cases when the high order
    // nybble is invariant, and entry 16 is the catch-all.
    std::pair<uint16_t, uint16_t> invariantValMask = idis->pattern.invariants(0xf000, 0);
    size_t idisIdx = 0xf000==invariantValMask.second ? (invariantValMask.first>>12) & 0xf : 16;
    
    // Check whether this instruction disassembler's bit pattern is ambiguous with an existing pattern
    for (IdisList::iterator ti=idis_table[idisIdx].begin(); ti!=idis_table[idisIdx].end(); ++ti) {
        std::pair<size_t, size_t> alternatives;
        if (idis->pattern.any_same((*ti)->pattern, &alternatives)) {
            // Inserting this instruction-specific disassembler would cause an ambiguity in the table
            std::cerr <<"DisassemblerCil::insert_idis: insertion key ";
            idis->pattern.print(std::cerr, alternatives.first);
            std::cerr <<" for '" <<idis->name <<"' conflicts with existing key ";
            (*ti)->pattern.print(std::cerr, alternatives.second);
            std::cerr <<" for '" <<(*ti)->name <<"'\n";
            throw Exception("insert_idis() would cause an ambiguity in the Cil instruction disassembly table");
        }
    }

    // Insert the new pattern in the list which is sorted by descending number of significant bits.
    IdisList::iterator ti = idis_table[idisIdx].begin();
    while (ti!=idis_table[idisIdx].end() && (*ti)->pattern.nsignificant()>=idis->pattern.nsignificant()) ++ti;
    idis_table[idisIdx].insert(ti, idis);
}

DisassemblerCil::Cil *
DisassemblerCil::find_idis(uint16_t *insn_bytes, size_t nbytes) const
{
#if 1
 // DQ (10/19/2021): Adding CIL support.
    printf ("In DisassemblerCil::find_idis(): nbytes = %zu \n",nbytes);
#endif

    if (nbytes==0)
        return NULL;

#if 1
 // DQ (10/19/2021): Adding CIL support.
    printf ("search the table \n");
#endif

    // First search the table based on the operator bits (high order nybble of the first opword)
    size_t idisIdx = (insn_bytes[0] >> 12) & 0xf;

#if 1
 // DQ (10/19/2021): Adding CIL support.
    printf ("idisIdx = %zu idis_table.size() = %zu \n",idisIdx,idis_table.size());
#endif

    for (IdisList::const_iterator ti=idis_table[idisIdx].begin(); ti!=idis_table[idisIdx].end(); ++ti) {
#if 1
     // DQ (10/19/2021): Adding CIL support.
        printf ("Calling pattern.matches() \n");
#endif
        if ((*ti)->pattern.matches(insn_bytes, nbytes))
            return *ti;
    }

#if 1
 // DQ (10/19/2021): Adding CIL support.
    printf ("If we didn't find it, search table entry 16 \n");
#endif

    // If we didn't find it, search table entry 16, the catch-all slot for instructions whose operator byte is not invariant.
    idisIdx = 16;
    for (IdisList::const_iterator ti=idis_table[idisIdx].begin(); ti!=idis_table[idisIdx].end(); ++ti) {
        if ((*ti)->pattern.matches(insn_bytes, nbytes))
            return *ti;
    }

#if 1
 // DQ (10/19/2021): Adding CIL support.
    printf ("Leaving DisassemblerCil::find_idis(): return NULL \n");
#endif

    return NULL;
}

/*******************************************************************************************************************************
 *                                      Integer instructions
 *******************************************************************************************************************************/

typedef DisassemblerCil::Cil Cil;

// DQ (10/20/2021): Added NOP instruction
struct Cil_nop: Cil 
   {
     Cil_nop(): Cil("nop", Cil_family, OP(4) & BITS<0, 11>(0xe71)) {}

#if 0
  // DQ (10/20/2021): We don't need this functor.
     SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) 
        {
          return d->makeInstruction(state, Cil_nop, "nop");
        }
#endif
   };


// DQ (10/20/2021): Added break instruction
struct Cil_break: Cil 
   {
     Cil_break(): Cil("break", Cil_family, OP(4) & BITS<0, 11>(0xe71)) {}

#if 0
  // DQ (10/20/2021): We don't need this functor.
     SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) 
        {
          return d->makeInstruction(state, Cil_break, "break");
        }
#endif
   };



#if 0
// DQ (10/20/2021): Start of commented out instructions for M68k.

// ABCD.B Dy, Dx
struct Cil_abcd_1: Cil {
    Cil_abcd_1(): Cil("abcd_1", Cil_family,
                        OP(12) & BITS<3, 8>(0x20)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(state, extract<0, 2>(w0), Cil_fmt_i8);
        SgAsmExpression *dst = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i8);
        return d->makeInstruction(state, Cil_abcd, "abcd."+formatLetter(Cil_fmt_i8), src, dst);
    }
};

// ABCD.B -(Ay), -(Ax)
struct Cil_abcd_2: Cil {
    Cil_abcd_2(): Cil("abcd_2", Cil_family,
                        OP(12) & BITS<3, 8>(0x21)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeAddressRegisterPreDecrement(state, extract<0, 2>(w0), Cil_fmt_i8);
        SgAsmExpression *dst = d->makeAddressRegisterPreDecrement(state, extract<9, 11>(w0), Cil_fmt_i8);
        return d->makeInstruction(state, Cil_abcd, "abcd."+formatLetter(Cil_fmt_i8), src, dst);
    }
};

// ADD.B <ea>y, Dx
struct Cil_add_1: Cil {
    Cil_add_1(): Cil("add_1", Cil_family,
                       OP(13) & BIT<8>(0) & BITS<6, 7>(0) & EAM(Cil_eam_all & ~Cil_eam_ard)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeDataRegister(state, extract<9, 11>(w0), fmt);
        return d->makeInstruction(state, Cil_add, "add."+formatLetter(fmt), src, dst);
    }
};

// ADD.W <ea>y, Dx
// ADD.L <ea>y, Dx
struct Cil_add_2: Cil {
    Cil_add_2(): Cil("add_2", Cil_family,
                       OP(13) & BIT<8>(0) & (BITS<6, 7>(1) | BITS<6, 7>(2)) & EAM(Cil_eam_all)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeDataRegister(state, extract<9, 11>(w0), fmt);
        return d->makeInstruction(state, Cil_add, "add."+formatLetter(fmt), src, dst);
    }
};

// ADD.B Dy, <ea>x
// ADD.W Dy, <ea>x
// ADD.L Dy, <ea>x
struct Cil_add_3: Cil {
    Cil_add_3(): Cil("add_3", Cil_family,
                       OP(13) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                       EAM(Cil_eam_memory & Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeDataRegister(state, extract<9, 11>(w0), fmt);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(state, Cil_add, "add."+formatLetter(fmt), src, dst);
    }
};

#if 1
 // DQ (10/9/2021): Eliminate this code for Cil.

// ADDA.W <ea>y, Ax
// ADDA.L <ea>y, Ax
struct Cil_adda: Cil {
    Cil_adda(): Cil("adda", Cil_family,
                      OP(13) & (BITS<6, 8>(3) | BITS<6, 8>(7)) & EAM(Cil_eam_all)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = Cil_fmt_unknown;
        switch (extract<6, 8>(w0)) {
            case 3: fmt = Cil_fmt_i16; break;
            case 7: fmt = Cil_fmt_i32; break;
        }
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeAddressRegister(state, extract<9, 11>(w0), Cil_fmt_i32);
        return d->makeInstruction(state, Cil_adda, "adda."+formatLetter(fmt), src, dst);
    }
};
#endif

#if 1
 // DQ (10/9/2021): Eliminate this code for Cil.

// ADDI.B #<data>, <ea>
// ADDI.W #<data>, <ea>
// ADDI.L #<data>, <ea>
struct Cil_addi: Cil {
    Cil_addi(): Cil("addi", Cil_family,
                      OP(0) & BITS<8, 11>(0x6) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(Cil_eam_data & Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        size_t opandWords = (formatNBits(fmt)+15) / 16;
        SgAsmExpression *src = d->makeImmediateExtension(state, fmt, 0); // immediate argument is in extension word(s)
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, opandWords);
        return d->makeInstruction(state, Cil_addi, "addi."+formatLetter(fmt), src, dst);
    }
};
#endif

#if 1
 // DQ (10/9/2021): Eliminate this code for Cil.

// ADDQ.B #<data>, <ea>x
// ADDQ.W #<data>, <ea>x
// ADDQ.L #<data>, <ea>x
//
// Note: Reference manual text says all "alterable modes" allowed, but the table lacks program counter memory indirect modes.
// I am favoring the table over the text. [Robb P. Matzke 2013-10-07]
struct Cil_addq: Cil {
    Cil_addq(): Cil("addq", Cil_family,
                      OP(5) & BIT<8>(0) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        unsigned imm = extract<9, 11>(w0);
        if (0==imm)
            imm = 8;
        SgAsmExpression *src = d->makeImmediateValue(state, fmt, imm);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(state, Cil_addq, "addq."+formatLetter(fmt), src, dst);
    }
};
#endif

// ADDX.B Dy, Dx
// ADDX.W Dy, Dx
// ADDX.L Dy, Dx
struct Cil_addx_1: Cil {
    Cil_addx_1(): Cil("addx_1", Cil_family,
                        OP(13) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 5>(0)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeDataRegister(state, extract<0, 2>(w0), fmt);
        SgAsmExpression *dst = d->makeDataRegister(state, extract<9, 11>(w0), fmt);
        return d->makeInstruction(state, Cil_addx, "addx."+formatLetter(fmt), src, dst);
    }
};

// ADDX.B -(Ay), -(Ax)
// ADDX.W -(Ay), -(Ax)
// ADDX.L -(Ay), -(Ax)
struct Cil_addx_2: Cil {
    Cil_addx_2(): Cil("addx_1", Cil_family,
                        OP(13) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 5>(1)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeAddressRegisterPreDecrement(state, extract<0, 2>(w0), fmt);
        SgAsmExpression *dst = d->makeAddressRegisterPreDecrement(state, extract<9, 11>(w0), fmt);
        return d->makeInstruction(state, Cil_addx, "addx."+formatLetter(fmt), src, dst);

    }
};

// AND.B <ea>y, Dx
// AND.W <ea>y, Dx
// AND.L <ea>y, Dx
struct Cil_and_1: Cil {
    Cil_and_1(): Cil("and_1", Cil_family,
                       OP(12) & BIT<8>(0) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & EAM(Cil_eam_data)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeDataRegister(state, extract<9, 11>(w0), fmt);
        return d->makeInstruction(state, Cil_and, "and."+formatLetter(fmt), src, dst);
    }
};

// AND.B Dy, <ea>x
// AND.W Dy, <ea>x
// AND.L Dy, <ea>x
//
// Note: Reference manual text says "memory alterable modes" are allowed, but the table lacks program counter memory indirect
// modes.  I am favoring the table over the text. [Robb P. Matzke 2013-10-07]
struct Cil_and_2: Cil {
    Cil_and_2(): Cil("and_2", Cil_family,
                       OP(12) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                       EAM(Cil_eam_memory & Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeDataRegister(state, extract<9, 11>(w0), fmt);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(state, Cil_and, "and."+formatLetter(fmt), src, dst);
    }
};

#if 1
 // DQ (10/9/2021): Eliminate this code for Cil.

// ANDI.B #<data>, <ea>
// ANDI.W #<data>, <ea>
// ANDI.L #<data>, <ea>
struct Cil_andi: Cil {
    Cil_andi(): Cil("andi", Cil_family,
                      OP(0) & BITS<8, 12>(2) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(Cil_eam_data & Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        size_t opandWords = (formatNBits(fmt)+15) / 16;
        SgAsmExpression *src = d->makeImmediateExtension(state, fmt, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, opandWords);
        return d->makeInstruction(state, Cil_andi, "andi."+formatLetter(fmt), src, dst);
    }
};
#endif

// ANDI.B #<data>, CCR
struct Cil_andi_to_ccr: Cil {
    Cil_andi_to_ccr(): Cil("andi_to_ccr", Cil_family,
                             OP(0) & BITS<0, 11>(0x23c) & INSN_WORD<1>(BITS<8, 15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = Cil_fmt_i8;
        SgAsmExpression *src = d->makeImmediateValue(state, fmt, extract<0, 7>(d->instructionWord(state, 1)));
        SgAsmExpression *dst = d->makeConditionCodeRegister(state);
        return d->makeInstruction(state, Cil_andi, "andi."+formatLetter(fmt), src, dst);
    }
};

// ASL.B Dy, Dx         (Dy is the shift count)
// ASL.W Dy, Dx
// ASL.L Dy, Dx
// ASR.B Dy, Dx
// ASR.W Dy, Dx
// ASR.L Dy, Dx
struct Cil_ashift_1: Cil {
    Cil_ashift_1(): Cil("ashift_1", Cil_family,
                          OP(14) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 5>(4)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        bool shift_left = extract<8, 8>(w0) ? true : false;
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i32); // full register, mod 64
        SgAsmExpression *dst = d->makeDataRegister(state, extract<0, 2>(w0), fmt);
        return d->makeInstruction(state, shift_left ? Cil_asl : Cil_asr,
                                  (shift_left ? "asl." : "asr.")+formatLetter(fmt),
                                  src, dst);
    }
};

// ASL.B #<nbits>, Dx        
// ASL.W #<nbits>, Dx        
// ASL.L #<nbits>, Dx        
// ASR.B #<nbits>, Dx        
// ASR.W #<nbits>, Dx        
// ASR.L #<nbits>, Dx        
struct Cil_ashift_2: Cil {
    Cil_ashift_2(): Cil("ashift_2", Cil_family,
                          OP(14) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 5>(0)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        bool shift_left = extract<8, 8>(w0) ? true : false;
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        unsigned count = extract<9, 11>(w0);
        if (0==count)
            count = 8;
        SgAsmExpression *src = d->makeImmediateValue(state, Cil_fmt_i8, count);
        SgAsmExpression *dst = d->makeDataRegister(state, extract<0, 2>(w0), fmt);
        return d->makeInstruction(state, shift_left ? Cil_asl : Cil_asr,
                                  (shift_left ? "asl." : "asr.")+formatLetter(fmt),
                                  src, dst);
    }
};

// ASL.W <ea>x
// ASR.W <ea>x
//
// Note: Reference manual text says "memory alterable modes" are allowed, but the table lacks program counter memory indirect
// modes.  I am favoring the table over the text. [Robb P. Matzke 2013-10-07]
struct Cil_ashift_3: Cil {
    Cil_ashift_3(): Cil("ashift_3", Cil_family,
                          OP(14) & BITS<9, 11>(0) & BITS<6, 7>(3) &
                          EAM(Cil_eam_memory & Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        bool shift_left = extract<8, 8>(w0) ? true : false;
        CilDataFormat fmt = Cil_fmt_i16;
        // src is implied #<1>
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(state, shift_left ? Cil_asl : Cil_asr,
                                  (shift_left ? "asl." : "asr.")+formatLetter(fmt),
                                  dst);
    }
};

#if 1
 // DQ (10/9/2021): Eliminate this code for Cil.

// Bcc.B <label>
// Bcc.W <label>
// Bcc.L <label>
//      Mnemonic Condition              Value (table 3.19)
//               true                   0000    0               (BRA instruction)
//               false                  0001    1               (BSR instruction)
//      hi       hi                     0010    2
//      ls       lo or same             0011    3
//      cc(hi)   carry clear            0100    4
//      cs(lo)   carry set              0101    5
//      ne       not equal              0110    6
//      eq       equal                  0111    7
//      vc       overflow clear         1000    8
//      vs       overflow set           1001    9
//      pl       plus                   1010    a
//      mi       minus                  1011    b
//      ge       greater or equal       1100    c
//      lt       less than              1101    d
//      gt       greater than           1110    e
//      le       less or equal          1111    f
//
// BRA.B <label>
// BRA.W <label>
// BRA.L <label>
// BSR.B <label>
// BSR.W <label>
// BSR.L <label>
struct Cil_branch: Cil {
    Cil_branch(): Cil("branch", Cil_family, OP(6)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        unsigned cc = extract<8, 11>(w0);
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (cc) {
            case  0: kind = Cil_bra; break;
            case  1: kind = Cil_bsr; break;
            case  2: kind = Cil_bhi; break;
            case  3: kind = Cil_bls; break;
            case  4: kind = Cil_bcc; break;
            case  5: kind = Cil_bcs; break;
            case  6: kind = Cil_bne; break;
            case  7: kind = Cil_beq; break;
            case  8: kind = Cil_bvc; break;
            case  9: kind = Cil_bvs; break;
            case 10: kind = Cil_bpl; break;
            case 11: kind = Cil_bmi; break;
            case 12: kind = Cil_bge; break;
            case 13: kind = Cil_blt; break;
            case 14: kind = Cil_bgt; break;
            case 15: kind = Cil_ble; break;
        }
        std::string mnemonic = stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_");
        rose_addr_t base = state.insn_va + 2;
        int32_t offset = signExtend<8, 32>(extract<0, 7>(w0));
        if (0==offset) {
            offset = signExtend<16, 32>((uint32_t)d->instructionWord(state, 1));
            mnemonic += ".w";
        } else if (-1==offset) {
            offset = shiftLeft<32>((uint32_t)d->instructionWord(state, 1), 16) | (uint32_t)d->instructionWord(state, 2);
            mnemonic += ".l";
        } else {
            mnemonic += ".b";
        }
        rose_addr_t target_va = (base + offset) & GenMask<rose_addr_t, 32>::value;
        SgAsmIntegerValueExpression *target = d->makeImmediateValue(state, Cil_fmt_i32, target_va);
        return d->makeInstruction(state, kind, mnemonic, target);
    }
};
#endif

// BCHG.L #<bitnum>, <ea>x
struct Cil_bchg_1: Cil {
    Cil_bchg_1(): Cil("bchg_1", Cil_family,
                        OP(0) & BITS<6, 11>(0x21) & EAM(Cil_eam_drd) & INSN_WORD<1>(BITS<8, 15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(state, Cil_fmt_i8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 1);
        return d->makeInstruction(state, Cil_bchg, "bchg.l", src, dst);
    }
};

// BCHG.B #<bitnum>, <ea>x
struct Cil_bchg_2: Cil {
    Cil_bchg_2(): Cil("bchg_2", Cil_family,
                        OP(0) & BITS<6, 11>(0x21) & EAM(Cil_eam_data & Cil_eam_alter & ~Cil_eam_drd & ~Cil_eam_pcmi) &
                        INSN_WORD<1>(BITS<8, 15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(state, Cil_fmt_i8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i8, 1);
        return d->makeInstruction(state, Cil_bchg, "bchg.b", src, dst);
    }
};

// BCHG.L Dy, <ea>x
struct Cil_bchg_3: Cil {
    Cil_bchg_3(): Cil("bchg_3", Cil_family,
                        OP(0) & BITS<6, 8>(5) & EAM(Cil_eam_drd)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i32);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 0);
        return d->makeInstruction(state, Cil_bchg, "bchg.l", src, dst);
    }
};

// BCHG.B Dy, <ea>x
struct Cil_bchg_4: Cil {
    Cil_bchg_4(): Cil("bchg_4", Cil_family,
                        OP(0) & BITS<6, 8>(5) & EAM(Cil_eam_data & Cil_eam_alter & ~Cil_eam_drd & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i32);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i8, 0);
        return d->makeInstruction(state, Cil_bchg, "bchg.b", src, dst);
    }
};

// BCLR.L #<bitnum>, <ea>x
struct Cil_bclr_1: Cil {
    Cil_bclr_1(): Cil("bclr_1", Cil_family,
                        OP(0) & BITS<6, 11>(0x22) & EAM(Cil_eam_drd) & INSN_WORD<1>(BITS<8, 15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(state, Cil_fmt_i8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 1);
        return d->makeInstruction(state, Cil_bclr, "bclr.l", src, dst);
    }
};

// BCLR.B #<bitnum>, <ea>x
struct Cil_bclr_2: Cil {
    Cil_bclr_2(): Cil("bclr_2", Cil_family,
                        OP(0) & BITS<6, 11>(0x22) & EAM(Cil_eam_data & Cil_eam_alter & ~Cil_eam_drd & ~Cil_eam_pcmi) &
                        INSN_WORD<1>(BITS<8, 15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(state, Cil_fmt_i8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i8, 1);
        return d->makeInstruction(state, Cil_bclr, "bclr.b", src, dst);
    }
};

// BCLR.L Dy, <ea>x
struct Cil_bclr_3: Cil {
    Cil_bclr_3(): Cil("bclr_3", Cil_family,
                        OP(0) & BITS<6, 8>(6) & EAM(Cil_eam_drd)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i32);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 0);
        return d->makeInstruction(state, Cil_bclr, "bclr.l", src, dst);
    }
};

// BCLR.B Dy, <ea>x
struct Cil_bclr_4: Cil {
    Cil_bclr_4(): Cil("bclr_4", Cil_family,
                        OP(0) & BITS<6, 8>(6) & EAM(Cil_eam_data & Cil_eam_alter & ~Cil_eam_drd & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i32);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i8, 0);
        return d->makeInstruction(state, Cil_bclr, "bclr.b", src, dst);
    }
};

#if 1
 // DQ (10/9/2021): Eliminate this code for Cil.

// BFCHG <ea>x {offset:width}
struct Cil_bfchg: Cil {
    Cil_bfchg(): Cil("bfchg", Cil_68020|Cil_68030|Cil_68040,
                       OP(0xe) & BITS<6, 11>(0x2b) & EAM(Cil_eam_drd | (Cil_eam_control & Cil_eam_alter & ~Cil_eam_pcmi)) &
                       INSN_WORD<1>(BITS<12, 15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        DisassemblerCil::ExpressionPair offset_width = d->makeOffsetWidthPair(state, d->instructionWord(state, 1));
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 1);
        return d->makeInstruction(state, Cil_bfchg, "bfchg", dst, offset_width.first, offset_width.second);
    }
};
#endif

#if 1
 // DQ (10/9/2021): Eliminate this code for Cil.

// BFCLR <ea>x {offset:width}
struct Cil_bfclr: Cil {
    Cil_bfclr(): Cil("bfclr", Cil_68020|Cil_68030|Cil_68040,
                       OP(0xe) & BITS<6, 11>(0x33) & EAM(Cil_eam_drd | (Cil_eam_control & Cil_eam_alter & ~Cil_eam_pcmi)) &
                       INSN_WORD<1>(BITS<12, 15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        DisassemblerCil::ExpressionPair offset_width = d->makeOffsetWidthPair(state, d->instructionWord(state, 1));
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 1);
        return d->makeInstruction(state, Cil_bfclr, "bfclr", dst, offset_width.first, offset_width.second);
    }
};
#endif

#if 1
 // DQ (10/9/2021): Eliminate this code for Cil.

// BFEXTS <ea>y {offset:width}, Dx
struct Cil_bfexts: Cil {
    Cil_bfexts(): Cil("bfexts", Cil_68020|Cil_68030|Cil_68040,
                        OP(0xe) & BITS<6, 11>(0x2f) & EAM(Cil_eam_drd | Cil_eam_control) &
                        INSN_WORD<1>(BIT<15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        DisassemblerCil::ExpressionPair offset_width = d->makeOffsetWidthPair(state, d->instructionWord(state, 1));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 1);
        SgAsmExpression *dst = d->makeDataRegister(state, extract<12, 14>(d->instructionWord(state, 1)), Cil_fmt_i32);
        return d->makeInstruction(state, Cil_bfexts, "bfexts", src, offset_width.first, offset_width.second, dst);
    }
};

// BFEXTU <ea>y {offset:width}, Dx
struct Cil_bfextu: Cil {
    Cil_bfextu(): Cil("bfextu", Cil_68020|Cil_68030|Cil_68040,
                        OP(0xe) & BITS<6, 11>(0x27) & EAM(Cil_eam_drd | Cil_eam_control) &
                        INSN_WORD<1>(BIT<15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        DisassemblerCil::ExpressionPair offset_width = d->makeOffsetWidthPair(state, d->instructionWord(state, 1));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 1);
        SgAsmExpression *dst = d->makeDataRegister(state, extract<12, 14>(d->instructionWord(state, 1)), Cil_fmt_i32);
        return d->makeInstruction(state, Cil_bfextu, "bfextu", src, offset_width.first, offset_width.second, dst);
    }
};
#endif

// BFFFO <ea>y {offset:width}, Dx
//
// Disabled because the documentation indicates that BFEXTU and BFFFO have the same bit pattern. [Robb P. Matzke 2013-10-28]
#if 1
struct Cil_bfffo: Cil {
    Cil_bfffo(): Cil("bfffo", Cil_68020|Cil_68030|Cil_68040,
                       OP(0xe) & BITS<6, 11>(0x27) & EAM(Cil_eam_drd | Cil_eam_control) &
                       INSN_WORD<1>(BIT<15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        ExpressionPair offset_width d->makeOffsetWidthPair(state, d->instructionWord(state, 1));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 1);
        SgAsmExpression *dst = d->makeDataRegister(state, extract<12, 14>(d->instructionWord(state, 1)), Cil_fmt_i32);
        return d->makeInstruction(state, Cil_bfffo, "bfffo", src, offset_width.first, offset_width.second, dst);
    }
};
#endif

#if 1
 // DQ (10/9/2021): Eliminate this code for Cil.

// BFINS Dy, <ea>x {offset:width}
struct Cil_bfins: Cil {
    Cil_bfins(): Cil("bfins", Cil_68020|Cil_68030|Cil_68040,
                       OP(0xe) & BITS<6, 11>(0x3f) & EAM(Cil_eam_drd | (Cil_eam_control & Cil_eam_alter & ~Cil_eam_pcmi)) &
                       INSN_WORD<1>(BIT<15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        DisassemblerCil::ExpressionPair offset_width = d->makeOffsetWidthPair(state, d->instructionWord(state, 1));
        SgAsmExpression *src = d->makeDataRegister(state, extract<12, 14>(d->instructionWord(state, 1)), Cil_fmt_i32);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 1);
        return d->makeInstruction(state, Cil_bfins, "bfins", src, dst, offset_width.first, offset_width.second);
    }
};
#endif

#if 1
 // DQ (10/9/2021): Eliminate this code for Cil.

// BFSET <ea>x {offset:width}
struct Cil_bfset: Cil {
    Cil_bfset(): Cil("bfset", Cil_68020|Cil_68030|Cil_68040,
                       OP(0xe) & BITS<6, 11>(0x3b) & EAM(Cil_eam_drd | (Cil_eam_control & Cil_eam_alter & ~Cil_eam_pcmi)) &
                       INSN_WORD<1>(BITS<12, 15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        DisassemblerCil::ExpressionPair offset_width = d->makeOffsetWidthPair(state, d->instructionWord(state, 1));
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 1);
        return d->makeInstruction(state, Cil_bfset, "bfset", dst, offset_width.first, offset_width.second);
    }
};
#endif

#if 1
 // DQ (10/9/2021): Eliminate this code for Cil.

// BFTST <ea>y {offset:width}
struct Cil_bftst: Cil {
    Cil_bftst(): Cil("bftst", Cil_68020|Cil_68030|Cil_68040,
                       OP(0xe) & BITS<6, 11>(0x23) & EAM(Cil_eam_drd | Cil_eam_control) &
                       INSN_WORD<1>(BITS<12, 15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        DisassemblerCil::ExpressionPair offset_width = d->makeOffsetWidthPair(state, d->instructionWord(state, 1));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 1);
        return d->makeInstruction(state, Cil_bftst, "bftst", src, offset_width.first, offset_width.second);
    }
};

// BKPT #<data>
struct Cil_bkpt: Cil {
    Cil_bkpt(): Cil("bkpt", Cil_68ec000|Cil_68010|Cil_68020|Cil_68030|Cil_68040|Cil_freescale_cpu32,
                      OP(4) & BITS<3, 11>(0x109)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *data = d->makeImmediateValue(state, Cil_fmt_i8, extract<0, 2>(w0));
        return d->makeInstruction(state, Cil_bkpt, "bkpt", data);
    }
};
#endif

// BSET.L #<bitnum>, <ea>x
struct Cil_bset_1: Cil {
    Cil_bset_1(): Cil("bset_1", Cil_family,
                        OP(0) & BITS<6, 11>(0x23) & EAM(Cil_eam_drd) & INSN_WORD<1>(BITS<9, 15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(state, Cil_fmt_i8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 1);
        return d->makeInstruction(state, Cil_bset, "bset.l", src, dst);
    }
};

// BSET.B #<bitnum>, <ea>x
struct Cil_bset_2: Cil {
    Cil_bset_2(): Cil("bset_2", Cil_family,
                        OP(0) & BITS<6, 11>(0x23) & EAM(Cil_eam_data & Cil_eam_alter & ~Cil_eam_drd & ~Cil_eam_pcmi) &
                        INSN_WORD<1>(BITS<9, 15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(state, Cil_fmt_i8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 1);
        return d->makeInstruction(state, Cil_bset, "bset.b", src, dst);
    }
};

// BSET.L Dy, <ea>x
struct Cil_bset_3: Cil {
    Cil_bset_3(): Cil("bset_3", Cil_family,
                        OP(0) & BITS<6, 8>(7) & EAM(Cil_eam_drd)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i32);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 0);
        return d->makeInstruction(state, Cil_bset, "bset.l", src, dst);
    }
};

// BSET.B Dy, <ea>x
struct Cil_bset_4: Cil {
    Cil_bset_4(): Cil("bset_4", Cil_family,
                        OP(0) & BITS<6, 8>(7) & EAM(Cil_eam_data & Cil_eam_alter & ~Cil_eam_drd & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i32);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i8, 0);
        return d->makeInstruction(state, Cil_bset, "bset.b", src, dst);
    }
};

// BTST.L #<bitnum>, <ea>x
struct Cil_btst_1: Cil {
    Cil_btst_1(): Cil("btst_1", Cil_family,
                        OP(0) & BITS<6, 11>(0x20) & EAM(Cil_eam_drd) &
                        INSN_WORD<1>(BITS<8, 15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(state, Cil_fmt_i8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 1);
        return d->makeInstruction(state, Cil_btst, "btst.l", src, dst);
    }
};

// BTST.B #<bitnum>, <ea>x
//
// Note: Text says data addressing modes can be used, but the table excludes immediate mode. [Robb P. Matzke 2013-10-28]
struct Cil_btst_2: Cil {
    Cil_btst_2(): Cil("btst_2", Cil_family,
                        OP(0) & BITS<6, 11>(0x20) & EAM(Cil_eam_data & ~Cil_eam_imm & ~Cil_eam_drd) &
                        INSN_WORD<1>(BITS<8, 15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(state, Cil_fmt_i8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i8, 1);
        return d->makeInstruction(state, Cil_btst, "btst.b", src, dst);
    }
};

// BTST.L Dy, <ea>x
struct Cil_btst_3: Cil {
    Cil_btst_3(): Cil("btst_3", Cil_family,
                        OP(0) & BITS<6, 8>(4) & EAM(Cil_eam_drd)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i32);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 0);
        return d->makeInstruction(state, Cil_btst, "btst.l", src, dst);
    }
};

// BTST.B Dy, <ea>x
struct Cil_btst_4: Cil {
    Cil_btst_4(): Cil("btst_4", Cil_family,
                        OP(0) & BITS<6, 8>(4) & EAM(Cil_eam_data & ~Cil_eam_drd)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i32);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i8, 0);
        return d->makeInstruction(state, Cil_btst, "btst.b", src, dst);
    }
};

#if 1
 // DQ (10/9/2021): Eliminate this code for Cil.

// CALLM #<data>, <ea>y
struct Cil_callm: Cil {
    Cil_callm(): Cil("callm", Cil_68020,
                       OP(0) & BITS<6, 11>(0x1b) & EAM(Cil_eam_control) &
                       INSN_WORD<1>(BITS<8, 15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *argcount = d->makeImmediateExtension(state, Cil_fmt_i8, 0);
        SgAsmExpression *ea = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 1);
        SgAsmExpression *target = d->makeAddress(state, ea);
        ASSERT_not_null2(target, "CALLM instruction must have a memory-referencing operand");
        return d->makeInstruction(state, Cil_callm, "callm", argcount, target);
    }
};

// CAS Dc,Du <ea>x
//
// Note: The text say memory alterable addressing modes can be used, but the table excludes program counter memory indirect
// modes. I am preferring the table over the text. [Robb P. Matzke 2013-10-28]
struct Cil_cas: Cil {
    Cil_cas(): Cil("cas", Cil_68020|Cil_68030|Cil_68040,
                     OP(0) & BIT<11>(1) & BITS<6, 8>(3) & EAM(Cil_eam_memory & Cil_eam_alter & ~Cil_eam_pcmi) &
                     INSN_WORD<1>(BITS<9, 15>(0) & BITS<3, 5>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        unsigned w1 = d->instructionWord(state, 1);
        SgAsmExpression *du = d->makeDataRegister(state, extract<6, 8>(w1), Cil_fmt_i32);
        SgAsmExpression *dc = d->makeDataRegister(state, extract<0, 2>(w1), Cil_fmt_i32);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 1);
        return d->makeInstruction(state, Cil_cas, "cas", dc, du, dst);
    }
};

// CAS2 Dc1:Dc2, Du1:Du2, (Rx1):(Rx2)
struct Cil_cas2: Cil {
    Cil_cas2(): Cil("cas2", Cil_68020|Cil_68030|Cil_68040,
                      OP(0) & BIT<11>(1) & BITS<0, 8>(0x0fc) &
                      INSN_WORD<1>(BITS<9, 11>(0) & BITS<3, 5>(0)) &
                      INSN_WORD<2>(BITS<9, 11>(0) & BITS<3, 5>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        unsigned w1 = d->instructionWord(state, 1);
        unsigned w2 = d->instructionWord(state, 2);
        SgAsmExpression *rx1 = d->makeDataAddressRegister(state, extract<12, 15>(w1), Cil_fmt_i32);
        SgAsmExpression *rx2 = d->makeDataAddressRegister(state, extract<12, 15>(w2), Cil_fmt_i32);
        SgAsmExpression *du1 = d->makeDataRegister(state, extract<6, 8>(w1), Cil_fmt_i32);
        SgAsmExpression *du2 = d->makeDataRegister(state, extract<6, 8>(w2), Cil_fmt_i32);
        SgAsmExpression *dc1 = d->makeDataRegister(state, extract<0, 2>(w1), Cil_fmt_i32);
        SgAsmExpression *dc2 = d->makeDataRegister(state, extract<0, 2>(w2), Cil_fmt_i32);
        return d->makeInstruction(state, Cil_cas2, "cas2", dc1, dc2, du1, du2, rx1, rx2);
    }
};
#endif

#if 1
 // DQ (10/9/2021): Eliminate this code for Cil.

// CHK.W <ea>, Dn
// CHK.L <ea>, Dn
struct Cil_chk: Cil {
    Cil_chk(): Cil("chk", Cil_family,
                     OP(4) & (BITS<7, 8>(2) | BITS<7, 8>(3)) & BIT<6>(0) & EAM(Cil_eam_data)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = 3==extract<7, 8>(w0) ? Cil_fmt_i16 : Cil_fmt_i32;
        SgAsmExpression *bound = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *reg = d->makeDataRegister(state, extract<9, 11>(w0), fmt);
        return d->makeInstruction(state, Cil_chk, "chk."+formatLetter(fmt), bound, reg);
    }
};
#endif

#if 1
 // DQ (10/9/2021): Eliminate this code for Cil.

// CHK2.B <ea>, Rn
// CHK2.W <ea>, Rn
// CHK2.L <ea>, Rn
struct Cil_chk2: Cil {
    Cil_chk2(): Cil("chk2", Cil_68020|Cil_68030|Cil_68040|Cil_freescale_cpu32,
                      OP(0) & BIT<11>(0) & (BITS<9, 10>(0) | BITS<9, 10>(1) | BITS<9, 10>(2)) & BITS<6, 8>(3) &
                      EAM(Cil_eam_control) &
                      INSN_WORD<1>(BITS<0, 11>(0x800))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<9, 10>(w0));
        SgAsmExpression *bounds = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *reg = d->makeDataAddressRegister(state, extract<12, 15>(w0), fmt);
        return d->makeInstruction(state, Cil_chk2, "chk2."+formatLetter(fmt), bounds, reg);
    }
};
#endif

#if 1
 // DQ (10/9/2021): Eliminate this code for Cil.

// CLR.B <ea>x
// CLR.W <ea>x
// CLR.L <ea>x
struct Cil_clr: Cil {
    Cil_clr(): Cil("clr", Cil_family,
                     OP(4) & BITS<8, 11>(2) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                     EAM(Cil_eam_data & Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(state, Cil_clr, "clr."+formatLetter(fmt), dst);
    }
};
#endif

#if 1
 // DQ (10/9/2021): Eliminate this code for Cil.

// CMP.B <ea>y, Dx
// CMP.W <ea>y, Dx
// CMP.L <ea>y, Dx
struct Cil_cmp: Cil {
    Cil_cmp(): Cil("cmp", Cil_family,
                     OP(11) & (BITS<6, 8>(0) | BITS<6, 8>(1) | BITS<6, 8>(2)) & EAM(Cil_eam_all)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 8>(w0));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeDataRegister(state, extract<9, 11>(w0), fmt);
        return d->makeInstruction(state, Cil_cmp, "cmp."+formatLetter(fmt), src, dst);
    }
};
#endif

#if 1
 // DQ (10/9/2021): Eliminate this code for Cil.

// CMPA.W <ea>y, Ax
// CMPA.L <ea>y, Ax
struct Cil_cmpa: Cil {
    Cil_cmpa(): Cil("cmpa", Cil_family,
                      OP(11) & (BITS<6, 8>(3) | BITS<6, 8>(7)) & EAM(Cil_eam_all)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = 3==extract<6, 8>(w0) ? Cil_fmt_i16 : Cil_fmt_i32;
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeAddressRegister(state, extract<9, 11>(w0), Cil_fmt_i32);
        return d->makeInstruction(state, Cil_cmpa, "cmpa."+formatLetter(fmt), src, dst);
    }
};
#endif

#if 1
 // DQ (10/9/2021): Eliminate this code for Cil.

// CMPI.B #<data>, <ea>
// CMPI.W #<data>, <ea>
// CMPI.L #<data>, <ea>
struct Cil_cmpi: Cil {
    Cil_cmpi(): Cil("cmpi", Cil_family,
                      OP(0) & BITS<8, 11>(0xc) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(Cil_eam_data & ~Cil_eam_imm)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        size_t opandWords = (formatNBits(fmt)+15) / 16;
        SgAsmExpression *src = d->makeImmediateExtension(state, fmt, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, opandWords);
        return d->makeInstruction(state, Cil_cmpi, "cmpi."+formatLetter(fmt), src, dst);
    }
};

// CMPM.B (Ay)+, (Ax)+
// CMPM.W (Ay)+, (Ax)+
// CMPM.L (Ay)+, (Ax)+
struct Cil_cmpm: Cil {
    Cil_cmpm(): Cil("cmpm", Cil_family,
                      OP(11) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 5>(1)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeAddressRegisterPostIncrement(state, extract<0, 2>(w0), fmt);
        SgAsmExpression *dst = d->makeAddressRegisterPostIncrement(state, extract<9, 11>(w0), fmt);
        return d->makeInstruction(state, Cil_cmpm, "cmpm."+formatLetter(fmt), src, dst);
    }
};

// CMP2.B <ea>, Rn
// CMP2.W <ea>, Rn
// CMP2.L <ea>, Rn
struct Cil_cmp2: Cil {
    Cil_cmp2(): Cil("cmp2", Cil_68020|Cil_68030|Cil_68040|Cil_freescale_cpu32,
                      OP(0) & BIT<11>(0) & (BITS<9, 10>(0) | BITS<9, 10>(1) | BITS<9, 10>(2)) &
                      BITS<6, 8>(3) & EAM(Cil_eam_control) &
                      INSN_WORD<1>(BITS<0, 11>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<9, 10>(w0));
        SgAsmExpression *bounds = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *src2 = d->makeDataAddressRegister(state, extract<12, 15>(d->instructionWord(state, 1)), fmt);
        return d->makeInstruction(state, Cil_cmp2, "cmp2."+formatLetter(fmt), bounds, src2);
    }
};

// CPBcc.W <label>
// CPBcc.L <label>
struct Cil_cpbcc: Cil {
    Cil_cpbcc(): Cil("cpbcc", Cil_68020|Cil_68030,
                       OP(15) & BITS<7, 8>(1)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        // Not implemented because we don't know what co-processors are present, and the co-processor determines the
        // size of this instruction. [Robb P. Matzke 2014-02-20]
        throw DisassemblerCil::Exception("Cil CPBcc is not implemented: " + addrToString(w0, 16), state.insn_va);
    }
};

// CPDBcc.W Dn, <label>
struct Cil_cpdbcc: Cil {
    Cil_cpdbcc(): Cil("cpdbcc", Cil_68020|Cil_68030,
                        OP(15) & BITS<3, 8>(0x09) &
                        INSN_WORD<1>(BITS<6, 15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        // Not implemented because we don't know what co-processors are present, and the co-processor determines the
        // size of this instruction. [Robb P. Matzke 2014-02-20]
        throw DisassemblerCil::Exception("Cil CPDBcc is not implemented", state.insn_va);
    }
};

// CPGEN <...>
struct Cil_cpgen: Cil {
    Cil_cpgen(): Cil("cpgen", Cil_68020|Cil_68030,
                       OP(15) & BITS<6, 8>(0)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        // Not implemented because we don't know what co-processors are present, and the co-processor determines the
        // size of this instruction. [Robb P. Matzke 2014-02-20]
        throw DisassemblerCil::Exception("Cil CPGEN is not implemented", state.insn_va);
    }
};

// CPSCC.B <ea>
struct Cil_cpscc: Cil {
    Cil_cpscc(): Cil("cpscc", Cil_68020|Cil_68030,
                       OP(15) & BITS<6, 8>(1) & EAM(Cil_eam_alter & ~Cil_eam_pcmi) &
                       INSN_WORD<1>(BITS<6, 15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        // Not implemented because we don't know what co-processors are present, and the co-processor determines the
        // size of this instruction. [Robb P. Matzke 2014-02-20]
        throw DisassemblerCil::Exception("Cil CPScc is not implemented", state.insn_va);
    }
};

// CPTRAPcc.W #<data>
// CPTRAPcc.L #<data>
struct Cil_cptrapcc: Cil {
    Cil_cptrapcc(): Cil("cptrapcc", Cil_68020|Cil_68030,
                          OP(15) & BITS<3, 8>(0x0f) & (BITS<0, 2>(2) | BITS<0, 2>(3) | BITS<0, 2>(4)) &
                          INSN_WORD<1>(BITS<6, 15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        // Not implemented because we don't know what co-processors are present, and the co-processor determines the
        // size of this instruction. [Robb P. Matzke 2014-02-20]
        throw DisassemblerCil::Exception("Cil CPTRAPcc is not implemented", state.insn_va);
    }
};
#endif

#if 1
 // DQ (10/9/2021): Eliminate this code for Cil.

// CPUSHL dc, (Ax)
// CPUSHL ic, (Ax)
// CPUSHL bc, (Ax)
// CPUSHP dc, (Ax)
// CPUSHP ic, (Ax)
// CPUSHP bc, (Ax)
// CPUSHA dc, (Ax)
// CPUSHA ic, (Ax)
// CPUSHA bc, (Ax)
struct Cil_cpush: Cil {
    Cil_cpush(): Cil("cpush", Cil_generation_3,
                       OP(15) & BITS<8, 11>(4) & (BITS<6, 7>(1) | BITS<6, 7>(2) | BITS<6, 7>(3)) &
                       BIT<5>(1) & (BITS<3, 4>(1) | BITS<3, 4>(2) | BITS<3, 4>(3))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<3, 4>(w0)) {
            case 1: kind = Cil_cpushl; break;
            case 2: kind = Cil_cpushp; break;
            case 3: kind = Cil_cpusha; break;
            default: ASSERT_not_reachable("pattern should not have matched");
        }
        unsigned cacheId = extract<6, 7>(w0);
        SgAsmExpression *cache = d->makeImmediateValue(state, Cil_fmt_i8, cacheId);
        switch (cacheId) {
            case 1: cache->set_comment("data cache"); break;
            case 2: cache->set_comment("instruction cache"); break;
            case 3: cache->set_comment("data and instruction caches"); break;
            default: ASSERT_not_reachable("pattern should not have matched");
        }
        SgAsmExpression *ax = d->makeAddressRegister(state, extract<0, 2>(w0), Cil_fmt_i32);
        std::string mnemonic = stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_");
        return d->makeInstruction(state, kind, mnemonic, cache, ax);
    }
};

// DBcc.W Dn, <label>
//      Mnemonic Condition              Value (table 3.19)
//               true                   0000    0               (BRA instruction)
//               false                  0001    1               (BSR instruction)
//      hi       hi                     0010    2
//      ls       lo or same             0011    3
//      cc(hi)   carry clear            0100    4
//      cs(lo)   carry set              0101    5
//      ne       not equal              0110    6
//      eq       equal                  0111    7
//      vc       overflow clear         1000    8
//      vs       overflow set           1001    9
//      pl       plus                   1010    a
//      mi       minus                  1011    b
//      ge       greater or equal       1100    c
//      lt       less than              1101    d
//      gt       greater than           1110    e
//      le       less or equal          1111    f
struct Cil_dbcc: Cil {
    Cil_dbcc(): Cil("dbcc", Cil_family,
                      OP(5) & BITS<3, 7>(0x19)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        unsigned cc = extract<8, 11>(w0);
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (cc) {
            case  0: kind = Cil_dbt; break;
            case  1: kind = Cil_dbf; break;
            case  2: kind = Cil_dbhi; break;
            case  3: kind = Cil_dbls; break;
            case  4: kind = Cil_dbcc; break;
            case  5: kind = Cil_dbcs; break;
            case  6: kind = Cil_dbne; break;
            case  7: kind = Cil_dbeq; break;
            case  8: kind = Cil_dbvc; break;
            case  9: kind = Cil_dbvs; break;
            case 10: kind = Cil_dbpl; break;
            case 11: kind = Cil_dbmi; break;
            case 12: kind = Cil_dbge; break;
            case 13: kind = Cil_dblt; break;
            case 14: kind = Cil_dbgt; break;
            case 15: kind = Cil_dble; break;
        }
        std::string mnemonic = stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_");
        SgAsmExpression *src = d->makeDataRegister(state, extract<0, 2>(w0), Cil_fmt_i32);
        rose_addr_t target_va = state.insn_va + 2 + signExtend<16, 32>((rose_addr_t)d->instructionWord(state, 1));
        target_va &= GenMask<rose_addr_t, 32>::value;
        SgAsmIntegerValueExpression *target = d->makeImmediateValue(state, Cil_fmt_i32, target_va);
        return d->makeInstruction(state, kind, mnemonic+".w", src, target);
    }
};

// DIVS.W <ea>, Dx                      (Dx[16] / <ea>[16]) : (Dx[16] % <ea>[16]) -> Dq[32]
struct Cil_divs_w: Cil {
    Cil_divs_w(): Cil("divs_w", Cil_family,
                        OP(8) & BITS<6, 8>(7) & EAM(Cil_eam_data)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i16, 0);
        SgAsmExpression *dst = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i32);
        return d->makeInstruction(state, Cil_divs, "divs.w", src, dst);
    }
};

// DIVU.W
struct Cil_divu_w: Cil {
    Cil_divu_w(): Cil("divu_w", Cil_family,
                        OP(8) & BITS<6, 8>(3) & EAM(Cil_eam_data)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i16, 0);
        SgAsmExpression *dst = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i32);
        return d->makeInstruction(state, Cil_divu, "divu.w", src, dst);
    }
};
    
// DIVS.L <ea>, Dq                      Dq[32] / <ea>[32] -> Dq
// DIVU.L <ea>, Dq                      Dq[32] / <ea>[32] -> Dq
// DIVS.L <ea>, Dr, Dq                  (Dr:Dq)[64] / <ea>[32] -> Dq[32] and (Dr:Dq)[64] % <ea>[32] -> Dr[32]
// DIVU.L <ea>, Dr, Dq                  (Dr:Dq)[64] / <ea>[32] -> Dq[32] and (Dr:Dq)[64] % <ea>[32] -> Dr[32]
// DIVSL.L <ea>, Dr, Dq                 Dq[32] / <ea>[32] -> Dq and Dq[32] % <ea>[32] -> Dr[32]
// DIVUL.L <ea>, Dr, Dq                 Dq[32] / <ea>[32] -> Dq and Dq[32] % <ea>[32] -> Dr[32]
struct Cil_divide: Cil {
    Cil_divide(): Cil("divide", Cil_68020|Cil_68030|Cil_68040|Cil_freescale_cpu32,
                        OP(4) & BITS<6, 11>(0x31) & EAM(Cil_eam_data) &
                        INSN_WORD<1>(BIT<15>(0) & BITS<3, 9>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *ea = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 1);
        SgAsmExpression *dq = d->makeDataRegister(state, extract<12, 14>(d->instructionWord(state, 1)), Cil_fmt_i32);
        if (extract<12, 14>(d->instructionWord(state, 1)) == extract<0, 2>(d->instructionWord(state, 1))) {
            // first form, 32-bit dividend, storing only the quotient
            if (extract<10, 10>(d->instructionWord(state, 1)))
                return NULL;
            CilInstructionKind kind = extract<11, 11>(d->instructionWord(state, 1)) ? Cil_divs : Cil_divu;
            return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+".l", ea, dq);
        } else if (extract<10, 10>(d->instructionWord(state, 1))) {
            // second form, 64-bit dividend, storing both quotient and remainder
            SgAsmExpression *dr = d->makeDataRegister(state, extract<0, 2>(d->instructionWord(state, 1)), Cil_fmt_i32);
            CilInstructionKind kind = extract<11, 11>(d->instructionWord(state, 1)) ? Cil_divs : Cil_divu;
            return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+".l", ea, dr, dq);
        } else {
            // third form, 32-bit dividend, storing both quotient and remainder
            SgAsmExpression *dr = d->makeDataRegister(state, extract<0, 2>(d->instructionWord(state, 1)), Cil_fmt_i32);
            CilInstructionKind kind = extract<11, 11>(d->instructionWord(state, 1)) ? Cil_divsl : Cil_divul;
            return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+".l", ea, dr, dq);
        }
    }
};

// EOR.B Dy, <ea>x
// EOR.W Dy, <ea>x
// EOR.L Dy, <ea>x
struct Cil_eor: Cil {
    Cil_eor(): Cil("eor", Cil_family,
                     OP(11) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                     EAM(Cil_eam_data & Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeDataRegister(state, extract<9, 11>(w0), fmt);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(state, Cil_eor, "eor."+formatLetter(fmt), src, dst);
    }
};

// EORI.B #<data>, <ea>
// EORI.W #<data>, <ea>
// EORI.L #<data>, <ea>
struct Cil_eori: Cil {
    Cil_eori(): Cil("eori", Cil_family,
                      OP(0) & BITS<8, 11>(0xa) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(Cil_eam_data & Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        size_t opandWords = (formatNBits(fmt)+15) / 16;
        SgAsmExpression *src = d->makeImmediateExtension(state, fmt, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, opandWords);
        return d->makeInstruction(state, Cil_eori, "eori."+formatLetter(fmt), src, dst);
    }
};

// EORI.B #<data>, CCR
struct Cil_eori_to_ccr: Cil {
    Cil_eori_to_ccr(): Cil("eori_to_ccr", Cil_family,
                             OP(0) & BITS<0, 11>(0xa3c) & INSN_WORD<1>(BITS<8, 15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(state, Cil_fmt_i8, 0);
        SgAsmExpression *dst = d->makeConditionCodeRegister(state);
        return d->makeInstruction(state, Cil_eori, "eori.b", src, dst);
    }
};

// EXG.L Dx, Dy
// EXG.L Ax, Ay
// EXG.L Dx, Ay
struct Cil_exg: Cil {
    Cil_exg(): Cil("exg", Cil_family,
                     OP(12) & BIT<8>(1) & (BITS<3, 7>(0x08) | BITS<3, 7>(0x09) | BITS<3, 7>(0x11))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *rx=NULL, *ry=NULL;
        switch (extract<3, 7>(w0)) {
            case 0x08:
                rx = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i32);
                ry = d->makeDataRegister(state, extract<0, 2>(w0), Cil_fmt_i32);
                break;
            case 0x09:
                rx = d->makeAddressRegister(state, extract<9, 11>(w0), Cil_fmt_i32);
                ry = d->makeAddressRegister(state, extract<0, 2>(w0), Cil_fmt_i32);
                break;
            case 0x11:
                rx = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i32);
                ry = d->makeAddressRegister(state, extract<0, 2>(w0), Cil_fmt_i32);
                break;
        }
        return d->makeInstruction(state, Cil_exg, "exg.l", rx, ry);
    }
};

// EXT.W Dx
struct Cil_ext_w: Cil {
    Cil_ext_w(): Cil("ext_w", Cil_family,
                       OP(4) & BITS<3, 11>(0x110)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *rx = d->makeDataRegister(state, extract<0, 2>(w0), Cil_fmt_i16);
        return d->makeInstruction(state, Cil_ext, "ext.w", rx);
    }
};

// EXT.L Dx
struct Cil_ext_l: Cil {
    Cil_ext_l(): Cil("ext_l", Cil_family,
                       OP(4) & BITS<3, 11>(0x118)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *rx = d->makeDataRegister(state, extract<0, 2>(w0), Cil_fmt_i32);
        return d->makeInstruction(state, Cil_ext, "ext.l", rx);
    }
};

// EXTB.L Dx
struct Cil_extb_l: Cil {
    Cil_extb_l(): Cil("extb_l", Cil_family,
                        OP(4) & BITS<3, 11>(0x138)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *rx = d->makeDataRegister(state, extract<0, 2>(w0), Cil_fmt_i32);
        return d->makeInstruction(state, Cil_extb, "extb.l", rx);
    }
};

// FABS.B Dx, FPy
// FABS.W Dx, FPy
// FABS.L Dx, FPy
// FABS.S Dx, FPy
// FSABS.B Dx, FPy
// FSABS.W Dx, FPy
// FSABS.L Dx, FPy
// FSABS.S Dx, FPy
// FDABS.B Dx, FPy
// FDABS.W Dx, FPy
// FDABS.L Dx, FPy
// FDABS.S Dx, FPy
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct Cil_fabs_from_drd: Cil {
    Cil_fabs_from_drd(): Cil("fabs_from_drd", Cil_generation_3|Cil_freescale_fpu,
                               OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_drd) &
                               INSN_WORD<1>(BITS<13, 15>(2) &
                                            (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                             BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                            (BITS<0, 6>(0x18) | BITS<0, 6>(0x58) | BITS<0, 6>(0x5c)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x18: kind = Cil_fabs; break;
            case 0x58: kind = Cil_fsabs; break;
            case 0x5c: kind = Cil_fdabs; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FABS.B <ea>, FPx
// FABS.W <ea>, FPx
// FABS.L <ea>, FPx
// FABS.S <ea>, FPx
// FABS.D <ea>, FPx
// FABS.X <ea>, FPx
// FABS.P <ea>, FPx
// FSABS.B <ea>, FPx
// FSABS.W <ea>, FPx
// FSABS.L <ea>, FPx
// FSABS.S <ea>, FPx
// FSABS.D <ea>, FPx
// FSABS.X <ea>, FPx
// FSABS.P <ea>, FPx
// FDABS.B <ea>, FPx
// FDABS.W <ea>, FPx
// FDABS.L <ea>, FPx
// FDABS.S <ea>, FPx
// FDABS.D <ea>, FPx
// FDABS.X <ea>, FPx
// FDABS.P <ea>, FPx
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
//    Note: data register direct mode is handled by Cil_fabs_from_drd
struct Cil_fabs_mr: Cil {
    Cil_fabs_mr(): Cil("fabs_mr", Cil_generation_3|Cil_freescale_fpu,
                         OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_data & ~Cil_eam_drd) &
                         INSN_WORD<1>(BITS<13, 15>(2) &
                                      (BITS<0, 6>(0x18) | BITS<0, 6>(0x58) | BITS<0, 6>(0x5c)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x18: kind = Cil_fabs; break;
            case 0x58: kind = Cil_fsabs; break;
            case 0x5c: kind = Cil_fdabs; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FABS.<fmt> FPx, FPy
// FSABS.<fmt> FPx, FPy
// FDABS.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct Cil_fabs_rr: Cil {
    Cil_fabs_rr(): Cil("fabs_rr", Cil_generation_3|Cil_freescale_fpu,
                         OP(15) & BITS<0, 11>(0x200) &
                         INSN_WORD<1>(BITS<13, 15>(0) &
                                      (BITS<0, 6>(0x18) | BITS<0, 6>(0x58) | BITS<0, 6>(0x5c)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x18: kind = Cil_fabs; break;
            case 0x58: kind = Cil_fsabs; break;
            case 0x5c: kind = Cil_fdabs; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FADD.B Dx, FPy
// FADD.W Dx, FPy
// FADD.L Dx, FPy
// FADD.S Dx, FPy
// FSADD.B Dx, FPy
// FSADD.W Dx, FPy
// FSADD.L Dx, FPy
// FSADD.S Dx, FPy
// FDADD.B Dx, FPy
// FDADD.W Dx, FPy
// FDADD.L Dx, FPy
// FDADD.S Dx, FPy
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct Cil_fadd_from_drd: Cil {
    Cil_fadd_from_drd(): Cil("fadd_from_drd", Cil_generation_3|Cil_freescale_fpu,
                               OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_drd) &
                               INSN_WORD<1>(BITS<13, 15>(2) &
                                            (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                             BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                            (BITS<0, 6>(0x22) | BITS<0, 6>(0x62) | BITS<0, 6>(0x66)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x22: kind = Cil_fadd; break;
            case 0x62: kind = Cil_fsadd; break;
            case 0x66: kind = Cil_fdadd; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FADD.B <ea>, FPx
// FADD.W <ea>, FPx
// FADD.L <ea>, FPx
// FADD.S <ea>, FPx
// FADD.D <ea>, FPx
// FADD.X <ea>, FPx
// FADD.P <ea>, FPx
// FSADD.B <ea>, FPx
// FSADD.W <ea>, FPx
// FSADD.L <ea>, FPx
// FSADD.S <ea>, FPx
// FSADD.D <ea>, FPx
// FSADD.X <ea>, FPx
// FSADD.P <ea>, FPx
// FDADD.B <ea>, FPx
// FDADD.W <ea>, FPx
// FDADD.L <ea>, FPx
// FDADD.S <ea>, FPx
// FDADD.D <ea>, FPx
// FDADD.X <ea>, FPx
// FDADD.P <ea>, FPx
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
//    Note: data register direct mode is handled by Cil_fadd_from_drd
struct Cil_fadd_mr: Cil {
    Cil_fadd_mr(): Cil("fadd_mr", Cil_generation_3|Cil_freescale_fpu,
                         OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_data & ~Cil_eam_drd) &
                         INSN_WORD<1>(BITS<13, 15>(2) &
                                      (BITS<0, 6>(0x22) | BITS<0, 6>(0x62) | BITS<0, 6>(0x66)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x22: kind = Cil_fadd; break;
            case 0x62: kind = Cil_fsadd; break;
            case 0x66: kind = Cil_fdadd; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FADD.<fmt> FPx, FPy
// FSADD.<fmt> FPx, FPy
// FDADD.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct Cil_fadd_rr: Cil {
    Cil_fadd_rr(): Cil("fadd_rr", Cil_generation_3|Cil_freescale_fpu,
                         OP(15) & BITS<0, 11>(0x200) &
                         INSN_WORD<1>(BITS<13, 15>(0) &
                                      (BITS<0, 6>(0x22) | BITS<0, 6>(0x62) | BITS<0, 6>(0x66)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x22: kind = Cil_fadd; break;
            case 0x62: kind = Cil_fsadd; break;
            case 0x66: kind = Cil_fdadd; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FBEQ.W <label>
// FBEQ.L <label>
// FBNE.W <label>
// FBNE.L <label>
// FBGT.W <label>
// FBGT.L <label>
// FBNGT.W <label>
// FBNGT.L <label>
// FBGE.W <label>
// FBGE.L <label>
// FBNGE.W <label>
// FBNGE.L <label>
// FBLT.W <label>
// FBLT.L <label>
// FBNLT.W <label>
// FBNLT.L <label>
// FBLE.W <label>
// FBLE.L <label>
// FBNLE.W <label>
// FBNLE.L <label>
// FBGL.W <label>
// FBGL.L <label>
// FBNGL.W <label>
// FBNGL.L <label>
// FBGLE.W <label>
// FBGLE.L <label>
// FBNGLE.W <label>
// FBNGLE.L <label>
// FBOGT.W <label>
// FBOGT.L <label>
// FBULE.W <label>
// FBULE.L <label>
// FBOGE.W <label>
// FBOGE.L <label>
// FBULT.W <label>
// FBULT.L <label>
// FBOLT.W <label>
// FBOLT.L <label>
// FBUGE.W <label>
// FBUGE.L <label>
// FBOLE.W <label>
// FBOLE.L <label>
// FBUGT.W <label>
// FBUGT.L <label>
// FBOGL.W <label>
// FBOGL.L <label>
// FBUEQ.W <label>
// FBUEQ.L <label>
// FBOR.W <label>
// FBOR.L <label>
// FBUN.W <label>
// FBUN.L <label>
// FBF.W <label>
// FBF.L <label>
// FBT.W <label>
// FBT.L <label>
// FBSF.W <label>
// FBSF.L <label>
// FBST.W <label>
// FBST.L <label>
// FBSEQ.W <label>
// FBSEQ.L <label>
// FBSNE.W <label>
// FBSNE.L <label>
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct Cil_fb: Cil {
    Cil_fb(): Cil("fb", Cil_generation_3|Cil_freescale_fpu,
                    OP(15) & BITS<9, 11>(1) & BITS<7, 8>(1) & BIT<5>(0)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilInstructionKind kind = Cil_unknown_instruction;
        // predicate: table 2-23 "Floating-point conditional tests" in "M68000 family progammer's reference manual"
        switch (extract<0, 5>(w0)) {
            case 0x01: kind = Cil_fbeq; break;
            case 0x0e: kind = Cil_fbne; break;
            case 0x12: kind = Cil_fbgt; break;
            case 0x1d: kind = Cil_fbngt; break;
            case 0x13: kind = Cil_fbge; break;
            case 0x1c: kind = Cil_fbnge; break;
            case 0x14: kind = Cil_fblt; break;
            case 0x1b: kind = Cil_fbnlt; break;
            case 0x15: kind = Cil_fble; break;
            case 0x1a: kind = Cil_fbnle; break;
            case 0x16: kind = Cil_fbgl; break;
            case 0x19: kind = Cil_fbngl; break;
            case 0x17: kind = Cil_fbgle; break;
            case 0x18: kind = Cil_fbngle; break;
            case 0x02: kind = Cil_fbogt; break;
            case 0x0d: kind = Cil_fbule; break;
            case 0x03: kind = Cil_fboge; break;
            case 0x0c: kind = Cil_fbult; break;
            case 0x04: kind = Cil_fbolt; break;
            case 0x0b: kind = Cil_fbuge; break;
            case 0x05: kind = Cil_fbole; break;
            case 0x0a: kind = Cil_fbugt; break;
            case 0x06: kind = Cil_fbogl; break;
            case 0x09: kind = Cil_fbueq; break;
            case 0x07: kind = Cil_fbor; break;
            case 0x08: kind = Cil_fbun; break;
            case 0x00: kind = Cil_fbf; break;
            case 0x0f: kind = Cil_fbt; break;
            case 0x10: kind = Cil_fbsf; break;
            case 0x1f: kind = Cil_fbst; break;
            case 0x11: kind = Cil_fbseq; break;
            case 0x1e: kind = Cil_fbsne; break;
        }
        CilDataFormat fmt = Cil_fmt_unknown;
        int32_t offset = 0;
        if (extract<6, 6>(w0)) {
            fmt = Cil_fmt_i32;
            offset = shiftLeft<32>((uint32_t)d->instructionWord(state, 1), 16) | (uint32_t)d->instructionWord(state, 2);
        } else {
            fmt = Cil_fmt_i16;
            offset = signExtend<16, 32>((uint32_t)d->instructionWord(state, 1));
        }
        rose_addr_t base = state.insn_va + 2;
        rose_addr_t target_va = (base + offset) & GenMask<rose_addr_t, 32>::value;
        SgAsmIntegerValueExpression *target = d->makeImmediateValue(state, Cil_fmt_i32, target_va);
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), target);
    }
};

// FCMP.B Dx, FPy
// FCMP.W Dx, FPy
// FCMP.L Dx, FPy
// FCMP.S Dx, FPy
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
//    The ColdFire documentation says bits<0,6> of the first extension word are 0x38, but the M68000 documentation
//    says that they are 0x0c. I'm using the ColdFire version.
struct Cil_fcmp_from_drd: Cil {
    Cil_fcmp_from_drd(): Cil("fcmp_from_drd", Cil_generation_3|Cil_freescale_fpu,
                               OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_drd) &
                               INSN_WORD<1>(BITS<13, 15>(2) &
                                            (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                             BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                            BITS<0, 6>(0x38))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        return d->makeInstruction(state, Cil_fcmp, "fcmp."+formatLetter(fmt), src, dst);
    }
};

// FCMP.B <ea>, FPx
// FCMP.W <ea>, FPx
// FCMP.L <ea>, FPx
// FCMP.S <ea>, FPx
// FCMP.D <ea>, FPx
// FCMP.X <ea>, FPx
// FCMP.P <ea>, FPx
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
//    Note: data register direct mode is handled by Cil_fcmp_from_drd
//    The ColdFire documentation says bits<0,6> of the first extension word are 0x38, but the M68000 documentation
//    says that they are 0x0c. I'm using the ColdFire version.
struct Cil_fcmp_mr: Cil {
    Cil_fcmp_mr(): Cil("fcmp_mr", Cil_generation_3|Cil_freescale_fpu,
                         OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_data & ~Cil_eam_drd) &
                         INSN_WORD<1>(BITS<13, 15>(2) & BITS<0, 6>(0x38))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        return d->makeInstruction(state, Cil_fcmp, "fcmp."+formatLetter(fmt), src, dst);
    }
};

// FCMP.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
//    The ColdFire documentation says bits<0,6> of the first extension word are 0x38, but the M68000 documentation
//    says that they are 0x0c. I'm using the ColdFire version.
struct Cil_fcmp_rr: Cil {
    Cil_fcmp_rr(): Cil("fcmp_rr", Cil_generation_3|Cil_freescale_fpu,
                         OP(15) & BITS<0, 11>(0x200) &
                         INSN_WORD<1>(BITS<13, 15>(0) & BITS<0, 6>(0x38))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        return d->makeInstruction(state, Cil_fcmp, "fcmp."+formatLetter(fmt), src, dst);
    }
};

// FDIV.B Dx, FPy
// FDIV.W Dx, FPy
// FDIV.L Dx, FPy
// FDIV.S Dx, FPy
// FSDIV.B Dx, FPy
// FSDIV.W Dx, FPy
// FSDIV.L Dx, FPy
// FSDIV.S Dx, FPy
// FDDIV.B Dx, FPy
// FDDIV.W Dx, FPy
// FDDIV.L Dx, FPy
// FDDIV.S Dx, FPy
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct Cil_fdiv_from_drd: Cil {
    Cil_fdiv_from_drd(): Cil("fdiv_from_drd", Cil_generation_3|Cil_freescale_fpu,
                               OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_drd) &
                               INSN_WORD<1>(BITS<13, 15>(2) &
                                            (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                             BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                            (BITS<0, 6>(0x20) | BITS<0, 6>(0x60) | BITS<0, 6>(0x64)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x20: kind = Cil_fdiv; break;
            case 0x60: kind = Cil_fsdiv; break;
            case 0x64: kind = Cil_fddiv; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FDIV.B <ea>, FPx
// FDIV.W <ea>, FPx
// FDIV.L <ea>, FPx
// FDIV.S <ea>, FPx
// FDIV.D <ea>, FPx
// FDIV.X <ea>, FPx
// FDIV.P <ea>, FPx
// FSDIV.B <ea>, FPx
// FSDIV.W <ea>, FPx
// FSDIV.L <ea>, FPx
// FSDIV.S <ea>, FPx
// FSDIV.D <ea>, FPx
// FSDIV.X <ea>, FPx
// FSDIV.P <ea>, FPx
// FDDIV.B <ea>, FPx
// FDDIV.W <ea>, FPx
// FDDIV.L <ea>, FPx
// FDDIV.S <ea>, FPx
// FDDIV.D <ea>, FPx
// FDDIV.X <ea>, FPx
// FDDIV.P <ea>, FPx
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
//    Note: data register direct mode is handled by Cil_fdiv_from_drd
struct Cil_fdiv_mr: Cil {
    Cil_fdiv_mr(): Cil("fdiv_mr", Cil_generation_3|Cil_freescale_fpu,
                         OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_data & ~Cil_eam_drd) &
                         INSN_WORD<1>(BITS<13, 15>(2) &
                                      (BITS<0, 6>(0x20) | BITS<0, 6>(0x60) | BITS<0, 6>(0x64)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x20: kind = Cil_fdiv; break;
            case 0x60: kind = Cil_fsdiv; break;
            case 0x64: kind = Cil_fddiv; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FDIV.<fmt> FPx, FPy
// FSDIV.<fmt> FPx, FPy
// FDDIV.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct Cil_fdiv_rr: Cil {
    Cil_fdiv_rr(): Cil("fdiv_rr", Cil_generation_3|Cil_freescale_fpu,
                         OP(15) & BITS<0, 11>(0x200) &
                         INSN_WORD<1>(BITS<13, 15>(0) &
                                      (BITS<0, 6>(0x20) | BITS<0, 6>(0x60) | BITS<0, 6>(0x64)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x20: kind = Cil_fdiv; break;
            case 0x60: kind = Cil_fsdiv; break;
            case 0x64: kind = Cil_fddiv; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FINT.B Dx, FPy
// FINT.W Dx, FPy
// FINT.L Dx, FPy
// FINT.S Dx, FPy
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct Cil_fint_from_drd: Cil {
    Cil_fint_from_drd(): Cil("fint_from_drd", Cil_generation_3|Cil_freescale_fpu,
                               OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_drd) &
                               INSN_WORD<1>(BITS<13, 15>(2) &
                                            (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                             BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                            BITS<0, 6>(0x01))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        return d->makeInstruction(state, Cil_fint, "fint."+formatLetter(fmt), src, dst);
    }
};

// FINT.B <ea>, FPx
// FINT.W <ea>, FPx
// FINT.L <ea>, FPx
// FINT.S <ea>, FPx
// FINT.D <ea>, FPx
// FINT.X <ea>, FPx
// FINT.P <ea>, FPx
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
//    Note: data register direct mode is handled by Cil_fint_from_drd
struct Cil_fint_mr: Cil {
    Cil_fint_mr(): Cil("fint_mr", Cil_generation_3|Cil_freescale_fpu,
                         OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_data & ~Cil_eam_drd) &
                         INSN_WORD<1>(BITS<13, 15>(2) & BITS<0, 6>(0x01))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        return d->makeInstruction(state, Cil_fint, "fint."+formatLetter(fmt), src, dst);
    }
};

// FINT.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct Cil_fint_rr: Cil {
    Cil_fint_rr(): Cil("fint_rr", Cil_generation_3|Cil_freescale_fpu,
                         OP(15) & BITS<0, 11>(0x200) &
                         INSN_WORD<1>(BITS<13, 15>(0) & BITS<0, 6>(0x01))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        return d->makeInstruction(state, Cil_fint, "fint."+formatLetter(fmt), src, dst);
    }
};

// FINTRZ.B Dx, FPy
// FINTRZ.W Dx, FPy
// FINTRZ.L Dx, FPy
// FINTRZ.S Dx, FPy
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct Cil_fintrz_from_drd: Cil {
    Cil_fintrz_from_drd(): Cil("fintrz_from_drd", Cil_generation_3|Cil_freescale_fpu,
                                 OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_drd) &
                                 INSN_WORD<1>(BITS<13, 15>(2) &
                                              (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                               BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                              BITS<0, 6>(0x03))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        return d->makeInstruction(state, Cil_fintrz, "fintrz."+formatLetter(fmt), src, dst);
    }
};

// FINTRZ.B <ea>, FPx
// FINTRZ.W <ea>, FPx
// FINTRZ.L <ea>, FPx
// FINTRZ.S <ea>, FPx
// FINTRZ.D <ea>, FPx
// FINTRZ.X <ea>, FPx
// FINTRZ.P <ea>, FPx
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
//    Note: data register direct mode is handled by Cil_fintrz_from_drd
struct Cil_fintrz_mr: Cil {
    Cil_fintrz_mr(): Cil("fintrz_mr", Cil_generation_3|Cil_freescale_fpu,
                           OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_data & ~Cil_eam_drd) &
                           INSN_WORD<1>(BITS<13, 15>(2) & BITS<0, 6>(0x03))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        return d->makeInstruction(state, Cil_fintrz, "fintrz."+formatLetter(fmt), src, dst);
    }
};

// FINTRZ.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct Cil_fintrz_rr: Cil {
    Cil_fintrz_rr(): Cil("fintrz_rr", Cil_generation_3|Cil_freescale_fpu,
                           OP(15) & BITS<0, 11>(0x200) &
                           INSN_WORD<1>(BITS<13, 15>(0) & BITS<0, 6>(0x03))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        return d->makeInstruction(state, Cil_fintrz, "fintrz."+formatLetter(fmt), src, dst);
    }
};

// FMOVE.B Dy, FPx
// FMOVE.W Dy, FPx
// FMOVE.L Dy, FPx
// FMOVE.S Dy, FPx
// FSMOVE.B Dy, FPx
// FSMOVE.W Dy, FPx
// FSMOVE.L Dy, FPx
// FSMOVE.S Dy, FPx
// FDMOVE.B Dy, FPx
// FDMOVE.W Dy, FPx
// FDMOVE.L Dy, FPx
// FDMOVE.S Dy, FPx
//    FIXME[Robb P. Matzke 2014-07-16]: Coprocessor 1 only
struct Cil_fmove_from_drd: Cil {
    Cil_fmove_from_drd(): Cil("fmove_from_drd", Cil_generation_3|Cil_freescale_fpu,
                                OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_drd) &
                                INSN_WORD<1>(BITS<13, 15>(2) &
                                             (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                              BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                             (BITS<0, 6>(0) | BITS<0, 6>(0x40) | BITS<0, 6>(0x44)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x00: kind = Cil_fmove; break;
            case 0x40: kind = Cil_fsmove; break;
            case 0x44: kind = Cil_fdmove; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FMOVE.B <ea>, FPx
// FMOVE.W <ea>, FPx
// FMOVE.L <ea>, FPx
// FMOVE.S <ea>, FPx
// FMOVE.D <ea>, FPx
// FMOVE.X <ea>, FPx
// FSMOVE.B <ea>, FPx
// FSMOVE.W <ea>, FPx
// FSMOVE.L <ea>, FPx
// FSMOVE.S <ea>, FPx
// FSMOVE.D <ea>, FPx
// FSMOVE.X <ea>, FPx
// FDMOVE.B <ea>, FPx
// FDMOVE.W <ea>, FPx
// FDMOVE.L <ea>, FPx
// FDMOVE.S <ea>, FPx
// FDMOVE.D <ea>, FPx
// FDMOVE.X <ea>, FPx
//    FIXME[Robb P. Matzke 2014-07-16]: Coprocessor 1 only
//    Note: the data register direct source is handled by Cil_fmove_from_drd
struct Cil_fmove_mr: Cil {
    Cil_fmove_mr(): Cil("fmove_mr", Cil_generation_3|Cil_freescale_fpu,
                          OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_data & ~Cil_eam_drd) &
                          INSN_WORD<1>(BITS<13, 15>(2) &
                                       (BITS<0, 6>(0) | BITS<0, 6>(0x40) | BITS<0, 6>(0x44)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x00: kind = Cil_fmove; break;
            case 0x40: kind = Cil_fsmove; break;
            case 0x44: kind = Cil_fdmove; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FMOVE.B FPx, <ea>
// FMOVE.W FPx, <ea>
// FMOVE.L FPx, <ea>
// FMOVE.S FPx, <ea>
// FMOVE.D FPx, <ea>
// FMOVE.X FPx, <ea>
//    FIXME[Robb P. Matzke 2014-07-16]: Coprocessor 1 only
//    FIXME[Robb P. Matzke 2014-07-16]: BCD format (P) is not implemented yet; k-factor bits are always zero
//    Note: the data register direct destination is handled by Cil_fmove_to_drd
struct Cil_fmove_rm: Cil {
    Cil_fmove_rm(): Cil("fmove_rm", Cil_generation_3|Cil_freescale_fpu,
                          OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) &
                          EAM(Cil_eam_data & Cil_eam_alter & ~Cil_eam_pcmi & ~Cil_eam_drd) &
                          INSN_WORD<1>(BITS<13, 15>(0x3) & BITS<0, 6>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        return d->makeInstruction(state, Cil_fmove, "fmove."+formatLetter(fmt), src, dst);
    }
};

// FMOVE.<fmt> FPx, FPy
// FSMOVE.<fmt> FPx, FPy
// FDMOVE.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-16]: Coprocessor 1 only
struct Cil_fmove_rr: Cil {
    Cil_fmove_rr(): Cil("fmove_rr", Cil_generation_3|Cil_freescale_fpu,
                          OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & BITS<0, 5>(0) &
                          INSN_WORD<1>(BITS<13, 15>(0) &
                                       (BITS<0, 6>(0) | BITS<0, 6>(0x40) | BITS<0, 6>(0x44)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x00: kind = Cil_fmove; break;
            case 0x40: kind = Cil_fsmove; break;
            case 0x44: kind = Cil_fdmove; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FMOVE.B FPx, Dy
// FMOVE.W FPx, Dy
// FMOVE.L FPx, Dy
// FMOVE.S FPx, Dy
//    FIXME[Robb P. Matzke 2014-07-16]: Coprocessor 1 only
//    FIXME[Robb P. Matzke 2014-07-16]: BCD format (P) is not implemented yet; k-factor bits are always zero
struct Cil_fmove_to_drd: Cil {
    Cil_fmove_to_drd(): Cil("fmove_to_drd", Cil_generation_3|Cil_freescale_fpu,
                              OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_drd) &
                              INSN_WORD<1>(BITS<13, 15>(3) &
                                           (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                            BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                           BITS<0, 6>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        ASSERT_require(fmt==Cil_fmt_i8 || fmt==Cil_fmt_i16 || fmt==Cil_fmt_i32 || fmt==Cil_fmt_f32);
        SgAsmExpression *src = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        return d->makeInstruction(state, Cil_fmove, "fmove." + formatLetter(fmt), src, dst);
    }
};

// FMOVEM.<fmt> <ea>, #list
//    where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-16]: Coprocessor 1 only
struct Cil_fmovem_mr: Cil {
    Cil_fmovem_mr(): Cil("fmovem_mr", Cil_generation_3|Cil_freescale_fpu,
                           OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) &
                           EAM(Cil_eam_control | Cil_eam_inc) &
                           INSN_WORD<1>(BITS<14, 15>(3) & BIT<13>(0) & BITS<8, 10>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        bool dynamicList = extract<11, 11>(w0) != 0;
        bool postIncMode = extract<12, 12>(w0) != 0;
        if (!postIncMode && extract<3, 5>(w0)==3) {
            throw DisassemblerCil::Exception("pre-decrement mode and [Ax+] source address are "
                                              "incompatible for the register-to-memory version of FMOVEM",
                                              state.insn_va);
        }
        SgAsmExpression *dst = NULL;
        if (dynamicList) {
            dst = d->makeDataRegister(state, extract<4, 6>(d->instructionWord(state, 1)), Cil_fmt_i8, 0);
        } else {
            dst = d->makeFPRegistersFromMask(state, extract<0, 8>(d->instructionWord(state, 1)), fmt, true); // order is FN7 to FN0
        }
        return d->makeInstruction(state, Cil_fmovem, "fmovem."+formatLetter(fmt), src, dst);
    }
};

// FMOVEM.<fmt> #list, <ea>
//    where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-16]: Coprocessor 1 only
struct Cil_fmovem_rm: Cil {
    Cil_fmovem_rm(): Cil("fmovem_rm", Cil_generation_3|Cil_freescale_fpu,
                           OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) &
                           EAM((Cil_eam_control & Cil_eam_alter & ~Cil_eam_pcmi) | Cil_eam_dec) &
                           INSN_WORD<1>(BITS<14, 15>(3) & BIT<13>(1) & BITS<8, 10>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormatForFamily(d->get_family());
        bool dynamicList = extract<11, 11>(w0) != 0;
        bool postIncMode = extract<12, 12>(w0) != 0;
        if (postIncMode && extract<3, 5>(w0)==4) {
            throw DisassemblerCil::Exception("post-increment mode and [-Ax] destination address are "
                                              "incompatible for the register-to-memory version of FMOVEM",
                                              state.insn_va);
        }
        SgAsmExpression *src = NULL;
        if (dynamicList) {
            src = d->makeDataRegister(state, extract<4, 6>(d->instructionWord(state, 1)), Cil_fmt_i8, 0);
        } else {
            src = d->makeFPRegistersFromMask(state, extract<0, 8>(d->instructionWord(state, 1)), fmt); // order is FN0 to FN7
        }
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        return d->makeInstruction(state, Cil_fmovem, "fmovem."+formatLetter(fmt), src, dst);
    }
};

// FMUL.B Dx, FPy
// FMUL.W Dx, FPy
// FMUL.L Dx, FPy
// FMUL.S Dx, FPy
// FSMUL.B Dx, FPy
// FSMUL.W Dx, FPy
// FSMUL.L Dx, FPy
// FSMUL.S Dx, FPy
// FDMUL.B Dx, FPy
// FDMUL.W Dx, FPy
// FDMUL.L Dx, FPy
// FDMUL.S Dx, FPy
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct Cil_fmul_from_drd: Cil {
    Cil_fmul_from_drd(): Cil("fmul_from_drd", Cil_generation_3|Cil_freescale_fpu,
                               OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_drd) &
                               INSN_WORD<1>(BITS<13, 15>(2) &
                                            (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                             BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                            (BITS<0, 6>(0x23) | BITS<0, 6>(0x63) | BITS<0, 6>(0x67)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x23: kind = Cil_fmul; break;
            case 0x63: kind = Cil_fsmul; break;
            case 0x67: kind = Cil_fdmul; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FMUL.B <ea>, FPx
// FMUL.W <ea>, FPx
// FMUL.L <ea>, FPx
// FMUL.S <ea>, FPx
// FMUL.D <ea>, FPx
// FMUL.X <ea>, FPx
// FMUL.P <ea>, FPx
// FSMUL.B <ea>, FPx
// FSMUL.W <ea>, FPx
// FSMUL.L <ea>, FPx
// FSMUL.S <ea>, FPx
// FSMUL.D <ea>, FPx
// FSMUL.X <ea>, FPx
// FSMUL.P <ea>, FPx
// FDMUL.B <ea>, FPx
// FDMUL.W <ea>, FPx
// FDMUL.L <ea>, FPx
// FDMUL.S <ea>, FPx
// FDMUL.D <ea>, FPx
// FDMUL.X <ea>, FPx
// FDMUL.P <ea>, FPx
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
//    Note: data register direct mode is handled by Cil_fmul_from_drd
struct Cil_fmul_mr: Cil {
    Cil_fmul_mr(): Cil("fmul_mr", Cil_generation_3|Cil_freescale_fpu,
                         OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_data & ~Cil_eam_drd) &
                         INSN_WORD<1>(BITS<13, 15>(2) &
                                      (BITS<0, 6>(0x23) | BITS<0, 6>(0x63) | BITS<0, 6>(0x67)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x23: kind = Cil_fmul; break;
            case 0x63: kind = Cil_fsmul; break;
            case 0x67: kind = Cil_fdmul; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FMUL.<fmt> FPx, FPy
// FSMUL.<fmt> FPx, FPy
// FDMUL.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct Cil_fmul_rr: Cil {
    Cil_fmul_rr(): Cil("fmul_rr", Cil_generation_3|Cil_freescale_fpu,
                         OP(15) & BITS<0, 11>(0x200) &
                         INSN_WORD<1>(BITS<13, 15>(0) &
                                      (BITS<0, 6>(0x23) | BITS<0, 6>(0x63) | BITS<0, 6>(0x67)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x23: kind = Cil_fmul; break;
            case 0x63: kind = Cil_fsmul; break;
            case 0x67: kind = Cil_fdmul; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FNEG.B Dx, FPy
// FNEG.W Dx, FPy
// FNEG.L Dx, FPy
// FNEG.S Dx, FPy
// FSNEG.B Dx, FPy
// FSNEG.W Dx, FPy
// FSNEG.L Dx, FPy
// FSNEG.S Dx, FPy
// FDNEG.B Dx, FPy
// FDNEG.W Dx, FPy
// FDNEG.L Dx, FPy
// FDNEG.S Dx, FPy
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct Cil_fneg_from_drd: Cil {
    Cil_fneg_from_drd(): Cil("fneg_from_drd", Cil_generation_3|Cil_freescale_fpu,
                               OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_drd) &
                               INSN_WORD<1>(BITS<13, 15>(2) &
                                            (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                             BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                            (BITS<0, 6>(0x1a) | BITS<0, 6>(0x5a) | BITS<0, 6>(0x5e)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x1a: kind = Cil_fneg; break;
            case 0x5a: kind = Cil_fsneg; break;
            case 0x5e: kind = Cil_fdneg; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FNEG.B <ea>, FPx
// FNEG.W <ea>, FPx
// FNEG.L <ea>, FPx
// FNEG.S <ea>, FPx
// FNEG.D <ea>, FPx
// FNEG.X <ea>, FPx
// FNEG.P <ea>, FPx
// FSNEG.B <ea>, FPx
// FSNEG.W <ea>, FPx
// FSNEG.L <ea>, FPx
// FSNEG.S <ea>, FPx
// FSNEG.D <ea>, FPx
// FSNEG.X <ea>, FPx
// FSNEG.P <ea>, FPx
// FDNEG.B <ea>, FPx
// FDNEG.W <ea>, FPx
// FDNEG.L <ea>, FPx
// FDNEG.S <ea>, FPx
// FDNEG.D <ea>, FPx
// FDNEG.X <ea>, FPx
// FDNEG.P <ea>, FPx
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
//    Note: data register direct mode is handled by Cil_fneg_from_drd
struct Cil_fneg_mr: Cil {
    Cil_fneg_mr(): Cil("fneg_mr", Cil_generation_3|Cil_freescale_fpu,
                         OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_data & ~Cil_eam_drd) &
                         INSN_WORD<1>(BITS<13, 15>(2) &
                                      (BITS<0, 6>(0x1a) | BITS<0, 6>(0x5a) | BITS<0, 6>(0x5e)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x1a: kind = Cil_fneg; break;
            case 0x5a: kind = Cil_fsneg; break;
            case 0x5e: kind = Cil_fdneg; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FNEG.<fmt> FPx, FPy
// FSNEG.<fmt> FPx, FPy
// FDNEG.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct Cil_fneg_rr: Cil {
    Cil_fneg_rr(): Cil("fneg_rr", Cil_generation_3|Cil_freescale_fpu,
                          OP(15) & BITS<0, 11>(0x200) &
                          INSN_WORD<1>(BITS<13, 15>(0) &
                                       (BITS<0, 6>(0x1a) | BITS<0, 6>(0x5a) | BITS<0, 6>(0x5e)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x1a: kind = Cil_fneg; break;
            case 0x5a: kind = Cil_fsneg; break;
            case 0x5e: kind = Cil_fdneg; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FNOP
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct Cil_fnop: Cil {
    Cil_fnop(): Cil("fnop", Cil_generation_3|Cil_freescale_fpu,
                      OP(15) & BITS<9, 11>(1) & BITS<0, 8>(0x080) &
                      INSN_WORD<1>(BITS<0, 15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        return d->makeInstruction(state, Cil_fnop, "fnop");
    }
};

// FSQRT.B Dx, FPy
// FSQRT.W Dx, FPy
// FSQRT.L Dx, FPy
// FSQRT.S Dx, FPy
// FSSQRT.B Dx, FPy
// FSSQRT.W Dx, FPy
// FSSQRT.L Dx, FPy
// FSSQRT.S Dx, FPy
// FDSQRT.B Dx, FPy
// FDSQRT.W Dx, FPy
// FDSQRT.L Dx, FPy
// FDSQRT.S Dx, FPy
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct Cil_fsqrt_from_drd: Cil {
    Cil_fsqrt_from_drd(): Cil("fsqrt_from_drd", Cil_generation_3|Cil_freescale_fpu,
                                OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_drd) &
                                INSN_WORD<1>(BITS<13, 15>(2) &
                                             (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                              BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                             (BITS<0, 6>(0x04) | BITS<0, 6>(0x41) | BITS<0, 6>(0x45)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x04: kind = Cil_fsqrt; break;
            case 0x41: kind = Cil_fssqrt; break;
            case 0x45: kind = Cil_fdsqrt; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FSQRT.B <ea>, FPx
// FSQRT.W <ea>, FPx
// FSQRT.L <ea>, FPx
// FSQRT.S <ea>, FPx
// FSQRT.D <ea>, FPx
// FSQRT.X <ea>, FPx
// FSQRT.P <ea>, FPx
// FSSQRT.B <ea>, FPx
// FSSQRT.W <ea>, FPx
// FSSQRT.L <ea>, FPx
// FSSQRT.S <ea>, FPx
// FSSQRT.D <ea>, FPx
// FSSQRT.X <ea>, FPx
// FSSQRT.P <ea>, FPx
// FDSQRT.B <ea>, FPx
// FDSQRT.W <ea>, FPx
// FDSQRT.L <ea>, FPx
// FDSQRT.S <ea>, FPx
// FDSQRT.D <ea>, FPx
// FDSQRT.X <ea>, FPx
// FDSQRT.P <ea>, FPx
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
//    Note: data register direct mode is handled by Cil_fsqrt_from_drd
struct Cil_fsqrt_mr: Cil {
    Cil_fsqrt_mr(): Cil("fsqrt_mr", Cil_generation_3|Cil_freescale_fpu,
                          OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_data & ~Cil_eam_drd) &
                          INSN_WORD<1>(BITS<13, 15>(2) &
                                       (BITS<0, 6>(0x04) | BITS<0, 6>(0x41) | BITS<0, 6>(0x45)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x04: kind = Cil_fsqrt; break;
            case 0x41: kind = Cil_fssqrt; break;
            case 0x45: kind = Cil_fdsqrt; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FSQRT.<fmt> FPx, FPy
// FSSQRT.<fmt> FPx, FPy
// FDSQRT.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct Cil_fsqrt_rr: Cil {
    Cil_fsqrt_rr(): Cil("fsqrt_rr", Cil_generation_3|Cil_freescale_fpu,
                          OP(15) & BITS<0, 11>(0x200) &
                          INSN_WORD<1>(BITS<13, 15>(0) &
                                       (BITS<0, 6>(0x04) | BITS<0, 6>(0x41) | BITS<0, 6>(0x45)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x04: kind = Cil_fsqrt; break;
            case 0x41: kind = Cil_fssqrt; break;
            case 0x45: kind = Cil_fdsqrt; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FSUB.B Dx, FPy
// FSUB.W Dx, FPy
// FSUB.L Dx, FPy
// FSUB.S Dx, FPy
// FSSUB.B Dx, FPy
// FSSUB.W Dx, FPy
// FSSUB.L Dx, FPy
// FSSUB.S Dx, FPy
// FDSUB.B Dx, FPy
// FDSUB.W Dx, FPy
// FDSUB.L Dx, FPy
// FDSUB.S Dx, FPy
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct Cil_fsub_from_drd: Cil {
    Cil_fsub_from_drd(): Cil("fsub_from_drd", Cil_generation_3|Cil_freescale_fpu,
                               OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_drd) &
                               INSN_WORD<1>(BITS<13, 15>(2) &
                                            (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                             BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                            (BITS<0, 6>(0x28) | BITS<0, 6>(0x68) | BITS<0, 6>(0x6c)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x28: kind = Cil_fsub; break;
            case 0x68: kind = Cil_fssub; break;
            case 0x6c: kind = Cil_fdsub; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FSUB.B <ea>, FPx
// FSUB.W <ea>, FPx
// FSUB.L <ea>, FPx
// FSUB.S <ea>, FPx
// FSUB.D <ea>, FPx
// FSUB.X <ea>, FPx
// FSUB.P <ea>, FPx
// FSSUB.B <ea>, FPx
// FSSUB.W <ea>, FPx
// FSSUB.L <ea>, FPx
// FSSUB.S <ea>, FPx
// FSSUB.D <ea>, FPx
// FSSUB.X <ea>, FPx
// FSSUB.P <ea>, FPx
// FDSUB.B <ea>, FPx
// FDSUB.W <ea>, FPx
// FDSUB.L <ea>, FPx
// FDSUB.S <ea>, FPx
// FDSUB.D <ea>, FPx
// FDSUB.X <ea>, FPx
// FDSUB.P <ea>, FPx
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
//    Note: data register direct mode is handled by Cil_fsub_from_drd
struct Cil_fsub_mr: Cil {
    Cil_fsub_mr(): Cil("fsub_mr", Cil_generation_3|Cil_freescale_fpu,
                         OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_data & ~Cil_eam_drd) &
                         INSN_WORD<1>(BITS<13, 15>(2) &
                                      (BITS<0, 6>(0x28) | BITS<0, 6>(0x68) | BITS<0, 6>(0x6c)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x28: kind = Cil_fsub; break;
            case 0x68: kind = Cil_fssub; break;
            case 0x6c: kind = Cil_fdsub; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FSUB.<fmt> FPx, FPy
// FSSUB.<fmt> FPx, FPy
// FDSUB.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct Cil_fsub_rr: Cil {
    Cil_fsub_rr(): Cil("fsub_rr", Cil_generation_3|Cil_freescale_fpu,
                         OP(15) & BITS<0, 11>(0x200) &
                         INSN_WORD<1>(BITS<13, 15>(0) &
                                      (BITS<0, 6>(0x28) | BITS<0, 6>(0x68) | BITS<0, 6>(0x6c)))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *dst = d->makeFPRegister(state, extract<7, 9>(d->instructionWord(state, 1)));
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(state, 1))) {
            case 0x28: kind = Cil_fsub; break;
            case 0x68: kind = Cil_fssub; break;
            case 0x6c: kind = Cil_fdsub; break;
        }
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FTST.B Dx, FPy
// FTST.W Dx, FPy
// FTST.L Dx, FPy
// FTST.S Dx, FPy
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
//    Destination register field is unused (bits 7-9) and recommended, but not required, to be zero
struct Cil_ftst_from_drd: Cil {
    Cil_ftst_from_drd(): Cil("ftst_from_drd", Cil_generation_3|Cil_freescale_fpu,
                               OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_drd) &
                               INSN_WORD<1>(BITS<13, 15>(2) &
                                            (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                             BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                            BITS<0, 6>(0x3a))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        return d->makeInstruction(state, Cil_ftst, "ftst."+formatLetter(fmt), src);
    }
};

// FTST.B <ea>, FPx
// FTST.W <ea>, FPx
// FTST.L <ea>, FPx
// FTST.S <ea>, FPx
// FTST.D <ea>, FPx
// FTST.X <ea>, FPx
// FTST.P <ea>, FPx
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
//    Note: data register direct mode is handled by Cil_ftst_from_drd
//    Destination register field is unused (bits 7-9) and recommended, but not required, to be zero
struct Cil_ftst_mr: Cil {
    Cil_ftst_mr(): Cil("ftst_mr", Cil_generation_3|Cil_freescale_fpu,
                         OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(Cil_eam_data & ~Cil_eam_drd) &
                         INSN_WORD<1>(BITS<13, 15>(2) & BITS<0, 6>(0x3a))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormat(state, extract<10, 12>(d->instructionWord(state, 1)));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        return d->makeInstruction(state, Cil_ftst, "ftst."+formatLetter(fmt), src);
    }
};

// FTST.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
//    Destination register field is unused (bits 7-9) and recommended, but not required, to be zero
struct Cil_ftst_rr: Cil {
    Cil_ftst_rr(): Cil("ftst_rr", Cil_generation_3|Cil_freescale_fpu,
                         OP(15) & BITS<0, 11>(0x200) &
                         INSN_WORD<1>(BITS<13, 15>(0) & BITS<0, 6>(0x3a))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(state, extract<10, 12>(d->instructionWord(state, 1)));
        return d->makeInstruction(state, Cil_ftst, "ftst."+formatLetter(fmt), src);
    }
};

// ILLEGAL
struct Cil_illegal: Cil {
    Cil_illegal(): Cil("illegal", Cil_family,
                         OP(4) & BITS<0, 11>(0xafc)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        return d->makeInstruction(state, Cil_illegal, "illegal");
    }
};

// JMP <ea>x
struct Cil_jmp: Cil {
    Cil_jmp(): Cil("jmp", Cil_family,
                     OP(4) & BITS<6, 11>(0x3b) & EAM(Cil_eam_control)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *ea = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 0);
        SgAsmExpression *target = d->makeAddress(state, ea);
        ASSERT_not_null2(target, "JMP instruction must have a memory-referencing operand");
        return d->makeInstruction(state, Cil_jmp, "jmp", target);
    }
};
                
// JSR <ea>x
struct Cil_jsr: Cil {
    Cil_jsr(): Cil("jsr", Cil_family,
                     OP(4) & BITS<6, 11>(0x3a) & EAM(Cil_eam_control)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *ea = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 0);
        SgAsmExpression *target = d->makeAddress(state, ea);
        ASSERT_not_null2(target, "JSR instruction must have a memory-referencing operand");
        return d->makeInstruction(state, Cil_jsr, "jsr", target);
    }
};

// LEA.L <ea>y, Ax
struct Cil_lea: Cil {
    Cil_lea(): Cil("lea", Cil_family,
                     OP(4) & BITS<6, 8>(7) & EAM(Cil_eam_control)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 0);
        SgAsmExpression *dst = d->makeAddressRegister(state, extract<9, 11>(w0), Cil_fmt_i32);
        return d->makeInstruction(state, Cil_lea, "lea.l", src, dst);
    }
};

// LINK.W An, #<displacement>
struct Cil_link_w: Cil {
    Cil_link_w(): Cil("link_w", Cil_family,
                        OP(4) & BITS<3, 11>(0x1ca)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *rx = d->makeAddressRegister(state, extract<0, 2>(w0), Cil_fmt_i32);
        SgAsmExpression *dsp = d->makeImmediateExtension(state, Cil_fmt_i16, 0);
        return d->makeInstruction(state, Cil_link, "link.w", rx, dsp);
    }
};

// LINK.L An, #<displacement>
struct Cil_link_l: Cil {
    Cil_link_l(): Cil("link_l", Cil_family,
                        OP(4) & BITS<3, 11>(0x101)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *rx = d->makeAddressRegister(state, extract<0, 2>(w0), Cil_fmt_i32);
        SgAsmExpression *dsp = d->makeImmediateExtension(state, Cil_fmt_i32, 0);
        return d->makeInstruction(state, Cil_link, "link.l", rx, dsp);
    }
};

// LSL.B Dy, Dx         as in (L)ogical (S)hift (L)eft
// LSL.W Dy, Dx
// LSL.L Dy, Dx
// LSR.B Dy, Dx
// LSR.W Dy, Dx
// LSR.L Dy, Dx
// Only register shifts, not memory
struct Cil_lshift_rr: Cil {
    Cil_lshift_rr(): Cil("lshift_rr", Cil_family,
                           OP(14) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 5>(5)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *dy = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i32, 0);
        SgAsmExpression *dx = d->makeDataRegister(state, extract<0, 2>(w0), fmt, 0);
        CilInstructionKind kind = extract<8, 8>(w0) ? Cil_lsl : Cil_lsr;
        std::string mnemonic = stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_") + "." + formatLetter(fmt);
        return d->makeInstruction(state, kind, mnemonic, dy, dx);
    }
};

// LSL.B #<nbits>, Dx   as in (L)ogical (S)hift (L)eft
// LSL.W #<nbits>, Dx
// LSL.L #<nbits>, Dx
// LSR.B #<nbits>, Dx
// LSR.W #<nbits>, Dx
// LSR.L #<nbits>, Dx
// Only register shifts, not memory
struct Cil_lshift_ir: Cil {
    Cil_lshift_ir(): Cil("lshift_ir", Cil_family,
                           OP(14) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 5>(1)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        unsigned n = extract<9, 11>(w0);
        SgAsmExpression *sa = d->makeImmediateValue(state, Cil_fmt_i8, 0==n ? 8 : n);
        SgAsmExpression *dx = d->makeDataRegister(state, extract<0, 2>(w0), fmt, 0);
        CilInstructionKind kind = extract<8, 8>(w0) ? Cil_lsl : Cil_lsr;
        std::string mnemonic = stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_") + "." + formatLetter(fmt);
        return d->makeInstruction(state, kind, mnemonic, sa, dx);
    }
};

// LSL.W <ea>         as in (L)ogical (S)hift (L)eft
// LSR.W <ea>
// Only memory shifts, not registers
struct Cil_lshift_mem: Cil {
    Cil_lshift_mem(): Cil("lshift_mem", Cil_family,
                            OP(14) & BITS<9, 11>(1) & BITS<6, 7>(3) &
                            EAM(Cil_eam_memory & Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *ea = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i16, 0);
        CilInstructionKind kind = extract<8, 8>(w0) ? Cil_lsl : Cil_lsr;
        std::string mnemonic = stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_") + ".w";
        return d->makeInstruction(state, kind, mnemonic, ea);
    }
};

// Multiply accumulate (4 arguments)
// MAC.W Ry, Rx, SF, ACCx
// MAC.L Ry, Rx, SF, ACCx
struct Cil_mac: Cil {
    Cil_mac(): Cil("mac", Cil_freescale_emac,
                     OP(10) & BIT<8>(0) & BITS<4, 5>(0) &
                     INSN_WORD<1>((BITS<9, 10>(0) | BITS<9, 10>(1) | BITS<9, 10>(3)) & BIT<8>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *ry=NULL, *rx=NULL;
        unsigned ryNumber = extract<0, 3>(w0);
        unsigned rxNumber = (extract<6, 6>(w0) << 3) | extract<9, 11>(w0);
        CilDataFormat fmt = Cil_fmt_unknown;
        if (extract<11, 11>(d->instructionWord(state, 1))) {
            fmt = Cil_fmt_i32;
            ry = d->makeDataAddressRegister(state, ryNumber, fmt, 0);
            rx = d->makeDataAddressRegister(state, rxNumber, fmt, 0);
        } else {
            fmt = Cil_fmt_i16;
            ry = d->makeDataAddressRegister(state, ryNumber, fmt, extract<6, 6>(d->instructionWord(state, 1)) ? 16 : 0);
            rx = d->makeDataAddressRegister(state, rxNumber, fmt, extract<7, 7>(d->instructionWord(state, 1)) ? 16 : 0);
        }
        SgAsmExpression *sf = d->makeImmediateValue(state, Cil_fmt_i8, extract<9, 10>(d->instructionWord(state, 1)));
        switch (extract<9, 10>(d->instructionWord(state, 1))) {
            case 0:
                sf->set_comment("unscaled");
                break;
            case 1:
                sf->set_comment("leftshift");
                break;
            case 3:
                sf->set_comment("rightshift");
                break;
        }
        unsigned accNumber = (extract<4, 4>(d->instructionWord(state, 1)) << 1) | extract<7, 7>(w0);
        SgAsmExpression *acc = d->makeMacAccumulatorRegister(state, accNumber);
        return d->makeInstruction(state, Cil_mac, "mac."+formatLetter(fmt), ry, rx, sf, acc);
    }
};

// Multiply accumulate with load (7 arguments)
// MAC.L Ry, Rx, SF, <ea>y&, Rw
//   ACC += scale(Ry * Rx); Rw = mask(ea<y>)    where scale and mask are optional
//   The U/Lx (bit w1.7) and U/Ly (bit w1.6) are not used for this 32-bit instruction
struct Cil_mac_l2: Cil {
    Cil_mac_l2(): Cil("mac_l2", Cil_freescale_mac,
                        OP(10) & BITS<7, 8>(1) & EAM(Cil_eam_ari|Cil_eam_inc|Cil_eam_dec|Cil_eam_dsp) &
                        INSN_WORD<1>(BIT<11>(1) & BIT<8>(0) & BIT<4>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        // Multiplicand 1
        unsigned ryRegisterNumber = extract<0, 3>(d->instructionWord(state, 1));
        SgAsmExpression *ry = d->makeDataAddressRegister(state, ryRegisterNumber, Cil_fmt_i32, 0);

        // Multiplicand 2
        unsigned rxRegisterNumber = extract<12, 15>(d->instructionWord(state, 1));
        SgAsmExpression *rx = d->makeDataAddressRegister(state, rxRegisterNumber, Cil_fmt_i32, 0);

        // How to scale product: 0=>none; 1=>multiply by 2; 2=>reserved; 3=>divide by 2
        SgAsmExpression *scaleFactor = d->makeImmediateValue(state, Cil_fmt_i8, extract<9, 10>(d->instructionWord(state, 1)));

        // Accumulator register
        SgAsmExpression *accRegister = NULL;
        unsigned accNumber = (extract<4, 4>(d->instructionWord(state, 1))<<1) | extract<7, 7>(w0);
        switch (accNumber) {
            case 0: accRegister = d->makeMacRegister(state, Cil_mac_acc1); break;
            case 1: accRegister = d->makeMacRegister(state, Cil_mac_acc0); break;
            case 2: accRegister = d->makeMacRegister(state, Cil_mac_acc3); break;
            case 3: accRegister = d->makeMacRegister(state, Cil_mac_acc2); break;
        }

        // Source loaded into Rw in parallel with ACC += product
        SgAsmExpression *source = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 1);

        // Whether to use the MASK register when loading the source into Rw
        SgAsmExpression *useMask = d->makeImmediateValue(state, Cil_fmt_i8, extract<5, 5>(d->instructionWord(state, 1)));

        // Destination register for loading source
        unsigned rwRegisterNumber = (extract<6, 6>(w0)<<3) | extract<9, 11>(w0);
        SgAsmExpression *rw = d->makeDataAddressRegister(state, rwRegisterNumber, Cil_fmt_i32, 0);

        return d->makeInstruction(state, Cil_mac, "mac.l", ry, rx, scaleFactor, accRegister, source, useMask, rw);
    }
};

// Multiply accumulate with load (7 arguments)
// MAC.W Ry.{U,L}, Rx.{U,L} SF, <ea>y[&], Rw
//   ACC += scale(Ry * Rx); Rw = mask(ea<y>)    where scale and mask are optional
struct Cil_mac_w2: Cil {
    Cil_mac_w2(): Cil("mac_w2", Cil_freescale_mac,
                        OP(10) & BITS<7, 8>(1) & EAM(Cil_eam_ari|Cil_eam_inc|Cil_eam_dec|Cil_eam_dsp) &
                        INSN_WORD<1>(BIT<11>(0) & BIT<8>(0) & BIT<4>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        // Multiplicand 1
        size_t ryOffset = extract<6, 6>(d->instructionWord(state, 1)) ? 16 : 0;
        unsigned ryRegisterNumber = extract<0, 3>(d->instructionWord(state, 1));
        SgAsmExpression *ry = d->makeDataAddressRegister(state, ryRegisterNumber, Cil_fmt_i16, ryOffset);

        // Multiplicand 2
        size_t rxOffset = extract<7, 7>(d->instructionWord(state, 1)) ? 16 : 0;
        unsigned rxRegisterNumber = extract<12, 15>(d->instructionWord(state, 1));
        SgAsmExpression *rx = d->makeDataAddressRegister(state, rxRegisterNumber, Cil_fmt_i16, rxOffset);

        // How to scale product: 0=>none; 1=>multiply by 2; 2=>reserved; 3=>divide by 2
        SgAsmExpression *scaleFactor = d->makeImmediateValue(state, Cil_fmt_i8, extract<9, 10>(d->instructionWord(state, 1)));

        // Accumulator register
        SgAsmExpression *accRegister = NULL;
        unsigned accNumber = (extract<4, 4>(d->instructionWord(state, 1))<<1) | extract<7, 7>(w0);
        switch (accNumber) {
            case 0: accRegister = d->makeMacRegister(state, Cil_mac_acc1); break;
            case 1: accRegister = d->makeMacRegister(state, Cil_mac_acc0); break;
            case 2: accRegister = d->makeMacRegister(state, Cil_mac_acc3); break;
            case 3: accRegister = d->makeMacRegister(state, Cil_mac_acc2); break;
        }

        // Source loaded into Rw in parallel with ACC += product
        SgAsmExpression *source = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 1);

        // Whether to use the MASK register when loading the source into Rw
        SgAsmExpression *useMask = d->makeImmediateValue(state, Cil_fmt_i8, extract<5, 5>(d->instructionWord(state, 1)));

        // Destination register for loading source
        unsigned rwRegisterNumber = (extract<6, 6>(w0)<<3) | extract<9, 11>(w0);
        SgAsmExpression *rw = d->makeDataAddressRegister(state, rwRegisterNumber, Cil_fmt_i32, 0);

        return d->makeInstruction(state, Cil_mac, "mac.w", ry, rx, scaleFactor, accRegister, source, useMask, rw);
    }
};

// MOV3Q.L #<data>, <ea>x
struct Cil_mov3q: Cil {
    Cil_mov3q(): Cil("mov3q", Cil_freescale_isab,
                       OP(10) & BITS<6, 8>(5) &
                       EAM(Cil_eam_alter & ~Cil_eam_234)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        int32_t val = signExtend<3, 32>((int32_t)extract<9, 11>(w0));
        if (0==val)
            val = -1;
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 0);
        return d->makeInstruction(state, Cil_mov3q, "mov3q.l", d->makeImmediateValue(state, Cil_fmt_i32, val), dst);
    }
};

// MOVCLR.L ACCy, Rx
struct Cil_movclr: Cil {
    Cil_movclr(): Cil("movclr", Cil_freescale_emac,
                        OP(10) & BIT<11>(0) & BITS<4, 8>(0x1c)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *acc = d->makeMacAccumulatorRegister(state, extract<9, 10>(w0));
        SgAsmExpression *dst = d->makeDataAddressRegister(state, extract<0, 3>(w0), Cil_fmt_i32, 0);
        return d->makeInstruction(state, Cil_movclr, "movclr.l", acc, dst);
    }
};

// MOVE.B <ea>y, <ea>x
// MOVE.W <ea>y, <ea>x
// MOVE.L <ea>y, <ea>x
struct Cil_move: Cil {
    Cil_move(): Cil("move", Cil_family,
                      (OP(1) | OP(2) | OP(3)) &
                      EAM_BACKWARD(Cil_eam_data & Cil_eam_alter & ~Cil_eam_pcmi, 6) &
                      EAM(Cil_eam_all)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = Cil_fmt_unknown;
        switch (extract<12, 13>(w0)) {
            case 1: fmt = Cil_fmt_i8; break;
            case 2: fmt = Cil_fmt_i32; break;                  // yes, not the usual order
            case 3: fmt = Cil_fmt_i16; break;
        }
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<6, 8>(w0), extract<9, 11>(w0), fmt, d->extensionWordsUsed(state));
        return d->makeInstruction(state, Cil_move, "move."+formatLetter(fmt), src, dst);
    }
};

// MOVE.L ACC, Rx
struct Cil_move_from_acc: Cil {
    Cil_move_from_acc(): Cil("move_from_acc", Cil_freescale_mac,
                               OP(10) & BIT<11>(0) & BITS<4, 8>(0x18)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        unsigned accumNumber = extract<9, 10>(w0);
        CilMacRegister accumulatorMinor = (CilMacRegister)(Cil_mac_acc0 + accumNumber);
        return d->makeInstruction(state, Cil_move_acc, "move.l",
                                  d->makeMacRegister(state, accumulatorMinor),
                                  d->makeDataAddressRegister(state, extract<0, 3>(w0), Cil_fmt_i32));
    }
};

// MOVE.L ACCext01, Rx
// MOVE.L ACCext23, Rx
struct Cil_move_from_accext: Cil {
    Cil_move_from_accext(): Cil("move_from_accext", Cil_freescale_emac,
                                  OP(10) & BIT<11>(1) & BITS<4, 9>(0x38)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilMacRegister extreg = extract<10, 10>(w0) ? Cil_mac_ext23 : Cil_mac_ext01;
        return d->makeInstruction(state, Cil_move_accext, "move.l",
                                  d->makeMacRegister(state, extreg),
                                  d->makeDataAddressRegister(state, extract<0, 3>(w0), Cil_fmt_i32));
    }
};

// MOVE.W CCR, Dx
struct Cil_move_from_ccr: Cil {
    Cil_move_from_ccr(): Cil("move_from_ccr", Cil_68010|Cil_68020|Cil_68030|Cil_68040|Cil_freescale_cpu32,
                               OP(4) & BITS<6, 11>(0x0b) &
                               EAM(Cil_eam_data & Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeConditionCodeRegister(state);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i16, 0);
        return d->makeInstruction(state, Cil_move_ccr, "move.w", src, dst);
    }
};

// MOVE.L MACSR, Rx
struct Cil_move_from_macsr: Cil {
    Cil_move_from_macsr(): Cil("move_from_macsr", Cil_freescale_mac,
                                 OP(10) & BITS<4, 11>(0x98)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        return d->makeInstruction(state, Cil_move_macsr, "move.l",
                                  d->makeMacRegister(state, Cil_mac_macsr),
                                  d->makeDataAddressRegister(state, extract<0, 3>(w0), Cil_fmt_i32));
    }
};

// MOVE.L MASK, Rx
struct Cil_move_from_mask: Cil {
    Cil_move_from_mask(): Cil("move_from_mask", Cil_freescale_mac,
                                OP(10) & BITS<4, 11>(0xd8)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        return d->makeInstruction(state, Cil_move_mask, "move.l",
                                  d->makeMacRegister(state, Cil_mac_mask),
                                  d->makeDataAddressRegister(state, extract<0, 3>(w0), Cil_fmt_i32));
    }
};

// MOVE.W SR, <ea>x
struct Cil_move_from_sr: Cil {
    Cil_move_from_sr(): Cil("move_from_sr", Cil_68000|Cil_68008,
                              OP(4) & BITS<6, 11>(0x03) & EAM(Cil_eam_data & Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeStatusRegister(state);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i16, 0);
        return d->makeInstruction(state, Cil_move_sr, "move.w", src, dst);
    }
};

// MOVE.L Ry, ACC
// MOVE.L #<data>, ACC
struct Cil_move_to_acc: Cil {
    Cil_move_to_acc(): Cil("move_to_acc", Cil_freescale_mac,
                             OP(10) & BIT<11>(0) & BITS<6, 8>(4) & EAM(Cil_eam_direct|Cil_eam_imm)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        unsigned accumNumber = extract<9, 10>(w0);
        CilMacRegister accumulatorMinor = (CilMacRegister)(Cil_mac_acc0 + accumNumber);
        return d->makeInstruction(state, Cil_move_acc, "move.l",
                                  d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 0),
                                  d->makeMacRegister(state, accumulatorMinor));
    }
};

// MOVE.L Ry, ACCext01
// MOVE.L Ry, ACCext23
struct Cil_move_to_accext: Cil {
    Cil_move_to_accext(): Cil("move_to_accext", Cil_freescale_emac,
                                OP(10) & BIT<11>(1) & BITS<6, 9>(0xc) & EAM(Cil_eam_direct|Cil_eam_imm)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilMacRegister extreg = extract<10, 10>(w0) ? Cil_mac_ext23 : Cil_mac_ext01;
        return d->makeInstruction(state, Cil_move_accext, "move.l",
                                  d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 0),
                                  d->makeMacRegister(state, extreg));
    }
};

// MOVE.W <ea>y, CCR
struct Cil_move_to_ccr: Cil {
    Cil_move_to_ccr(): Cil("move_to_ccr", Cil_family,
                             OP(4) & BITS<6, 11>(0x13) & EAM(Cil_eam_data)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i16, 0);
        SgAsmExpression *dst = d->makeConditionCodeRegister(state);
        return d->makeInstruction(state, Cil_move_ccr, "move.w", src, dst);
    }
};

// MOVE.L Ry, MACSR
// MOVE.L #<data>, MACSR
struct Cil_move_to_macsr: Cil {
    Cil_move_to_macsr(): Cil("move_to_macsr", Cil_freescale_mac,
                               OP(10) & BITS<6, 11>(0x24) & EAM(Cil_eam_direct|Cil_eam_imm)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        return d->makeInstruction(state, Cil_move_macsr, "move.l",
                                  d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 0),
                                  d->makeMacRegister(state, Cil_mac_macsr));
    }
};

// MOVE.L Ry, MASK
// MOVE.L #<data>, MASK
struct Cil_move_to_mask: Cil {
    Cil_move_to_mask(): Cil("move_to_mask", Cil_freescale_mac,
                              OP(10) & BITS<6, 11>(0x34) & EAM(Cil_eam_direct|Cil_eam_imm)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        return d->makeInstruction(state, Cil_move_mask, "move.l",
                                  d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 0),
                                  d->makeMacRegister(state, Cil_mac_mask));
    }
};

// MOVE.W <ea>, SR
struct Cil_move_to_sr: Cil {
    Cil_move_to_sr(): Cil("move_to_sr", Cil_family,
                            OP(4) & BITS<6, 11>(0x1b) & EAM(Cil_eam_drd|Cil_eam_imm)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        return d->makeInstruction(state, Cil_move_sr, "move.w",
                                  d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i16, 0),
                                  d->makeStatusRegister(state));
    }
};

// MOVE16 (Ax)+, (Ay)+
struct Cil_move16_pp: Cil {
    Cil_move16_pp(): Cil("move16_pp", Cil_68040,
                           OP(15) & BITS<3, 11>(0xc4) & INSN_WORD<1>(BIT<15>(1) & BITS<0, 11>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeAddressRegisterPostIncrement(state, extract<0, 2>(w0), Cil_fmt_i32);
        SgAsmExpression *dst = d->makeAddressRegisterPostIncrement(state, extract<12, 14>(d->instructionWord(state, 1)), Cil_fmt_i32);
        return d->makeInstruction(state, Cil_move16, "move16", src, dst);
    }
};

// MOVE16 (xxx).L, (An)+
// MOVE16 (An)+, (xxx).L
// MOVE16 (xxx).L, (An)
// MOVE16 (An), (xxx).L
struct Cil_move16_a: Cil {
    Cil_move16_a(): Cil("move16_a", Cil_68040,
                          OP(15) & BITS<5, 11>(0x30)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src=NULL, *dst=NULL;
        switch (extract<3, 4>(w0)) {
            case 0:                                     // MOVE16 (An)+, (xxx).L
                src = d->makeEffectiveAddress(state, 030|extract<0, 2>(w0), Cil_fmt_i32, 0);
                dst = d->makeEffectiveAddress(state, 071, Cil_fmt_i32, 0);
                break;
            case 1:                                     // MOVE16 (xxx).L, (An)+
                src = d->makeEffectiveAddress(state, 071, Cil_fmt_i32, 0);
                dst = d->makeEffectiveAddress(state, 030|extract<0, 2>(w0), Cil_fmt_i32, 0);
                break;
            case 2:                                     // MOVE16 (An), (xxx).L
                src = d->makeEffectiveAddress(state, 020|extract<0, 2>(w0), Cil_fmt_i32, 0);
                dst = d->makeEffectiveAddress(state, 071, Cil_fmt_i32, 0);
                break;
            case 3:                                     // MOVE16 (xxx).L, (An)
                src = d->makeEffectiveAddress(state, 071, Cil_fmt_i32, 0);
                dst = d->makeEffectiveAddress(state, 020|extract<0, 2>(w0), Cil_fmt_i32, 0);
                break;
        }
        return d->makeInstruction(state, Cil_move16, "move16", src, dst);
    }
};

// MOVEA.W <ea>y, Ax
// MOVEA.L <ea>y, Ax
struct Cil_movea: Cil {
    Cil_movea(): Cil("movea", Cil_family,
                       (OP(2)|OP(3)) & EAM(Cil_eam_all)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = 2==extract<12, 13>(w0) ? Cil_fmt_i32 : Cil_fmt_i16;
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeAddressRegister(state, extract<9, 11>(w0), Cil_fmt_i32);
        return d->makeInstruction(state, Cil_movea, "movea."+formatLetter(fmt), src, dst);
    }
};

// MOVEC.L <ea>y, Rc
struct Cil_move_to_cr: Cil {
    Cil_move_to_cr(): Cil("move_to_cr", Cil_family,
                            OP(4) & BITS<0, 11>(0xe7b)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataAddressRegister(state, extract<12, 15>(d->instructionWord(state, 1)), Cil_fmt_i32, 0);
        SgAsmExpression *dst = d->makeColdFireControlRegister(state, extract<0, 11>(d->instructionWord(state, 1)));
        return d->makeInstruction(state, Cil_movec, "movec.l", src, dst);
    }
};

// MOVEM.W <ea>y, #list
// MOVEM.L <ea>y, #list
struct Cil_movem_mr: Cil {
    Cil_movem_mr(): Cil("movem_mr", Cil_family,
                          OP(4) & BITS<7, 11>(0x19) & EAM(Cil_eam_control | Cil_eam_inc)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = extract<6, 6>(w0) ? Cil_fmt_i32 : Cil_fmt_i16;
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeRegistersFromMask(state, d->instructionWord(state, 1), Cil_fmt_i32);
        return d->makeInstruction(state, Cil_movem, "movem."+formatLetter(fmt), src, dst);
    }
};

// MOVEM.W #list, <ea>x
// MOVEM.L #list, <ea>x
struct Cil_movem_rm: Cil {
    Cil_movem_rm(): Cil("movem_rm", Cil_family,
                          OP(4) & BITS<7, 11>(0x11) &
                          EAM((Cil_eam_control & Cil_eam_alter & ~Cil_eam_pcmi) | Cil_eam_dec)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = extract<6, 6>(w0) ? Cil_fmt_i32 : Cil_fmt_i16;
        bool isPreDecrement = extract<3, 5>(w0) == 4;
        SgAsmRegisterNames *src = d->makeRegistersFromMask(state, d->instructionWord(state, 1), Cil_fmt_i32, isPreDecrement);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 1);
        return d->makeInstruction(state, Cil_movem, "movem."+formatLetter(fmt), src, dst);
    }
};
                
// MOVEP.W Dx, (d16,Ay)
// MOVEP.L Dx, (d16,Ay)
// MOVEP.W (d16,Ay), Dx
// MOVEP.L (d16,Ay), Dx
struct Cil_movep: Cil {
    Cil_movep(): Cil("movep", Cil_family,
                       OP(0) & BIT<8>(1) & BITS<3, 5>(1)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = extract<6, 6>(w0) ? Cil_fmt_i32 : Cil_fmt_i16;
        SgAsmExpression *src = d->makeEffectiveAddress(state, 050|extract<0, 2>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeDataRegister(state, extract<9, 11>(w0), fmt);
        if (extract<7, 7>(w0))
            std::swap(src, dst);                        // register-to-memory
        return d->makeInstruction(state, Cil_movep, "movep."+formatLetter(fmt), src, dst);
    }
};

// MOVEQ.L #<data>, Dx
struct Cil_moveq: Cil {
    Cil_moveq(): Cil("moveq", Cil_family,
                       OP(7) & BIT<8>(0)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateValue(state, Cil_fmt_i32, signExtend<8, 32>(extract<0, 7>(w0)));
        SgAsmExpression *dst = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i32);
        return d->makeInstruction(state, Cil_moveq, "moveq.l", src, dst);
    }
};

// MSAC.W Ry, Rx, SF, ACCx
// MSAC.L Ry, Rx, SF, ACCx
struct Cil_msac: Cil {
    Cil_msac(): Cil("msac", Cil_freescale_emac,
                      OP(10) & BIT<8>(0) & BITS<4, 5>(0) &
                      INSN_WORD<1>((BITS<9, 10>(0) | BITS<9, 10>(1) | BITS<9, 10>(3)) & BIT<8>(1))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *ry=NULL, *rx=NULL;
        unsigned ryNumber = extract<0, 3>(w0);
        unsigned rxNumber = (extract<6, 6>(w0) << 3) | extract<9, 11>(w0);
        CilDataFormat fmt = Cil_fmt_unknown;
        if (extract<11, 11>(d->instructionWord(state, 1))) {
            fmt = Cil_fmt_i32;
            ry = d->makeDataAddressRegister(state, ryNumber, fmt, 0);
            rx = d->makeDataAddressRegister(state, rxNumber, fmt, 0);
        } else {
            fmt = Cil_fmt_i16;
            ry = d->makeDataAddressRegister(state, ryNumber, fmt, extract<6, 6>(d->instructionWord(state, 1)) ? 16 : 0);
            rx = d->makeDataAddressRegister(state, rxNumber, fmt, extract<7, 7>(d->instructionWord(state, 1)) ? 16 : 0);
        }
        SgAsmExpression *sf = d->makeImmediateValue(state, Cil_fmt_i8, extract<9, 10>(d->instructionWord(state, 1)));
        switch (extract<9, 10>(d->instructionWord(state, 1))) {
            case 0:
                sf->set_comment("unscaled");
                break;
            case 1:
                sf->set_comment("leftshift");
                break;
            case 3:
                sf->set_comment("rightshift");
                break;
        }
        unsigned accNumber = (extract<4, 4>(d->instructionWord(state, 1)) << 1) | extract<7, 7>(w0);
        SgAsmExpression *acc = d->makeMacAccumulatorRegister(state, accNumber);
        return d->makeInstruction(state, Cil_msac, "msac."+formatLetter(fmt), ry, rx, sf, acc);
    }
};

// MULS.W <ea>y, Dx             16 x 16 -> 32
struct Cil_muls_w: Cil {
    Cil_muls_w(): Cil("muls_w", Cil_family,
                        OP(12) & BITS<6, 8>(7) & EAM(Cil_eam_data)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *a0 = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i16, 0);
        SgAsmExpression *a1 = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i16);
        SgAsmExpression *dst = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i32);
        return d->makeInstruction(state, Cil_muls, "muls.w", a0, a1, dst);
    }
};

// MULS.L <ea>y, Dx             32 x 32 -> 32
// MULU.L <ea>y, Dx             32 x 32 -> 32
struct Cil_multiply_l: Cil {
    Cil_multiply_l(): Cil("multiply_l", Cil_68020|Cil_68030|Cil_68040|Cil_freescale_cpu32,
                            OP(4) & BITS<6, 11>(0x30) & EAM(Cil_eam_data) &
                            INSN_WORD<1>(BIT<15>(0) & BITS<3, 10>(0))) {} // BITS<0,2> are unused and unspecified
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilInstructionKind kind = extract<11, 11>(d->instructionWord(state, 1)) ? Cil_muls : Cil_mulu;
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 1);
        SgAsmExpression *dst = d->makeDataRegister(state, extract<12, 14>(d->instructionWord(state, 1)), Cil_fmt_i32);
        return d->makeInstruction(state, kind, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+".l", src, dst);
    }
};

// MULS.L <ea>y, Dh, Dl         32 x 32 -> 64
// MULU.L <ea>y, Dh, DL         32 x 32 -> 64
struct Cil_multiply_64: Cil {
    Cil_multiply_64(): Cil("multiply_64", Cil_68020|Cil_68030|Cil_68040|Cil_freescale_cpu32,
                             OP(4) & BITS<6, 11>(0x30) & EAM(Cil_eam_data) &
                             INSN_WORD<1>(BIT<15>(0) & BITS<3, 10>(0x80))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilInstructionKind kind = extract<11, 11>(d->instructionWord(state, 1)) ? Cil_muls : Cil_mulu;
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 1);
        SgAsmExpression *dh = d->makeDataRegister(state, extract<0, 2>(d->instructionWord(state, 1)), Cil_fmt_i32);
        SgAsmExpression *dl = d->makeDataRegister(state, extract<12, 14>(d->instructionWord(state, 1)), Cil_fmt_i32);
        return d->makeInstruction(state, Cil_muls, stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_")+".l", src, dl, dh);
    }
};

// MULU.W <ea>y, Dx             16 x 16 -> 32
struct Cil_mulu_w: Cil {
    Cil_mulu_w(): Cil("mulu_w", Cil_family,
                        OP(12) & BITS<6, 8>(3) & EAM(Cil_eam_data)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *a0 = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i16, 0);
        SgAsmExpression *a1 = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i16);
        SgAsmExpression *dst = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i32);
        return d->makeInstruction(state, Cil_mulu, "mulu.w", a0, a1, dst);
    }
};

// MVS.B <ea>y, Dx
// MVS.W <ea>y, Dx
struct Cil_mvs: Cil {
    Cil_mvs(): Cil("mvs", Cil_freescale_isab,
                     OP(7) & BITS<7, 8>(2) & EAM(Cil_eam_all & ~Cil_eam_234)) {}

    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = extract<6, 6>(w0) ? Cil_fmt_i16 : Cil_fmt_i8;
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i32);
        return d->makeInstruction(state, Cil_mvs, "mvs."+formatLetter(fmt), src, dst);
    }
};

// MVZ.B <ea>y, Dx
// MVZ.W <ea>y, Dx
struct Cil_mvz: Cil {
    Cil_mvz(): Cil("mvz", Cil_freescale_isab,
                     OP(7) & BITS<7, 8>(3) & EAM(Cil_eam_all & ~Cil_eam_234)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = extract<6, 6>(w0) ? Cil_fmt_i16 : Cil_fmt_i8;
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i32);
        return d->makeInstruction(state, Cil_mvz, "mvz."+formatLetter(fmt), src, dst);
    }
};

// NBCD.B <ea>
struct Cil_nbcd: Cil {
    Cil_nbcd(): Cil("nbcd", Cil_family,
                      OP(8) & BITS<6, 11>(0x20) & EAM(Cil_eam_data & Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *ea = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i8, 0);
        return d->makeInstruction(state, Cil_nbcd, "nbcd.b", ea);
    }
};

// NEG.B <ea>
// NEG.W <ea>
// NEG.L <ea>
struct Cil_neg: Cil {
    Cil_neg(): Cil("neg", Cil_family,
                     OP(4) & BITS<8, 11>(4) & (BITS<6, 7>(0) |BITS<6, 7>(1) | BITS<6, 7>(2)) &
                     EAM(Cil_eam_data & Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *ea = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(state, Cil_neg, "neg."+formatLetter(fmt), ea);
    }
};

// NEGX.B <ea>
// NEGX.W <ea>
// NEGX.L <ea>
struct Cil_negx: Cil {
    Cil_negx(): Cil("negx", Cil_family,
                      OP(4) & BITS<8, 11>(0) & (BITS<6, 7>(0) |BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(Cil_eam_data & Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *ea = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(state, Cil_negx, "negx."+formatLetter(fmt), ea);
    }
};

// NOP
struct Cil_nop: Cil {
    Cil_nop(): Cil("nop", Cil_family,
                     OP(4) & BITS<0, 11>(0xe71)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        return d->makeInstruction(state, Cil_nop, "nop");
    }
};

// NOT.B <ea>
// NOT.W <ea>
// NOT.L <ea>
struct Cil_not: Cil {
    Cil_not(): Cil("not", Cil_family,
                     OP(4) & BITS<8, 11>(6) & (BITS<6, 7>(0) |BITS<6, 7>(1) | BITS<6, 7>(2)) &
                     EAM(Cil_eam_data & Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *ea = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(state, Cil_not, "not."+formatLetter(fmt), ea);
    }
};

// OR.B <ea>y, Dx
// OR.W <ea>y, Dx
// OR.L <ea>y, Dx
struct Cil_or_1: Cil {
    Cil_or_1(): Cil("or_1", Cil_family,
                      OP(8) & BIT<8>(0) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & EAM(Cil_eam_data)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeDataRegister(state, extract<9, 11>(w0), fmt);
        return d->makeInstruction(state, Cil_or, "or."+formatLetter(fmt), src, dst);
    }
};

// OR.B Dy, <ea>x
// OR.W Dy, <ea>x
// OR.L Dy, <ea>x
struct Cil_or_2: Cil {
    Cil_or_2(): Cil("or_2", Cil_family,
                      OP(8) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(Cil_eam_memory & Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeDataRegister(state, extract<9, 11>(w0), fmt);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(state, Cil_or, "or."+formatLetter(fmt), src, dst);
    }
};

// ORI.B #<data>, <ea>
// ORI.W #<data>, <ea>
// ORI.L #<data>, <ea>
struct Cil_ori: Cil {
    Cil_ori(): Cil("ori", Cil_family,
                     OP(0) & BITS<8, 11>(0) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                     EAM(Cil_eam_data & Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        size_t opandWords = (formatNBits(fmt)+15) / 16;
        SgAsmExpression *src = d->makeImmediateExtension(state, fmt, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 2>(w0), fmt, opandWords);
        return d->makeInstruction(state, Cil_ori, "ori."+formatLetter(fmt), src, dst);
    }
};

// ORI.B #<data>, CCR
struct Cil_ori_to_ccr: Cil {
    Cil_ori_to_ccr(): Cil("ori_to_ccr", Cil_family,
                            OP(0) & BITS<0, 11>(0x03c) & INSN_WORD<1>(BITS<8, 15>(0))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateValue(state, Cil_fmt_i8, extract<0, 7>(d->instructionWord(state, 1)));
        SgAsmExpression *dst = d->makeConditionCodeRegister(state);
        return d->makeInstruction(state, Cil_ori, "ori.b", src, dst);
    }
};

// PACK -(Ax), -(Ay), #<adjustment>
// PACK Dx, Dy, #<adjustment>
struct Cil_pack: Cil {
    Cil_pack(): Cil("pack", Cil_68020|Cil_68030|Cil_68040,
                      OP(8) & BITS<4, 8>(0x14)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src=NULL, *dst=NULL;
        if (extract<3, 3>(w0)) {
            // PACK -(Ax), -(Ay)
            src = d->makeAddressRegisterPreDecrement(state, extract<0, 2>(w0), Cil_fmt_i16);
            dst = d->makeAddressRegisterPreDecrement(state, extract<9, 11>(w0), Cil_fmt_i16);
        } else {
            // PACK Dx, Dy
            src = d->makeDataRegister(state, extract<0, 2>(w0), Cil_fmt_i16, 0);
            dst = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i16, 0);
        }
        SgAsmExpression *adj = d->makeImmediateExtension(state, Cil_fmt_i16, 0);
        return d->makeInstruction(state, Cil_pack, "pack", src, dst, adj);
    }
};

// PEA.L <ea>y
struct Cil_pea: Cil {
    Cil_pea(): Cil("pea", Cil_family,
                     OP(4) & BITS<6, 11>(0x21) & EAM(Cil_eam_control)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i32, 0);
        return d->makeInstruction(state, Cil_pea, "pea.l", src);
    }
};

// ROL.B Dx, Dy         as in (RO)tate (L)eft
// ROL.W Dx, Dy
// ROL.L Dx, Dy
// ROL.B #<data>, Dy
// ROL.W #<data>, Dy
// ROL.L #<data>, Dy
// ROR.B Dx, Dy         as in (RO)tate (R)ight
// ROR.W Dx, Dy
// ROR.L Dx, Dy
// ROR.B #<data>, Dy
// ROR.W #<data>, Dy
// ROR.L #<data>, Dy
struct Cil_rotate_reg: Cil {
    Cil_rotate_reg(): Cil("rotate_reg", Cil_family,
                            OP(14) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 4>(3)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        unsigned n = extract<9, 11>(w0);                // "count/regisger"
        SgAsmExpression *sa = NULL;
        if (extract<5, 5>(w0)) {                        // "i/r"
            sa = d->makeDataRegister(state, n, Cil_fmt_i32, 0);
        } else {
            sa = d->makeImmediateValue(state, Cil_fmt_i8, 0==n ? 8 : n);
        }
        SgAsmExpression *dy = d->makeDataRegister(state, extract<0, 2>(w0), fmt, 0);
        CilInstructionKind kind = extract<8, 8>(w0) ? Cil_rol : Cil_ror; // "dr"
        std::string mnemonic = stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_") + "." + formatLetter(fmt);
        return d->makeInstruction(state, kind, mnemonic, sa, dy);
    }
};

// ROL.W <ea>y          memory rotate left
// ROR.W <ea>y          memory rotate right
struct Cil_rotate_mem: Cil {
    Cil_rotate_mem(): Cil("rotate_mem", Cil_family,
                            OP(14) & BITS<9, 11>(3) & BITS<6, 7>(3)
                            & EAM(Cil_eam_memory & Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *eay = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i16, 0);
        CilInstructionKind kind = extract<8, 8>(w0) ? Cil_rol : Cil_ror; // "dr"
        std::string mnemonic = stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_") + ".w";
        return d->makeInstruction(state, kind, mnemonic, eay);
    }
};

// ROXL.B Dx, Dy         as in (RO)tate with e(X)tend (L)eft
// ROXL.W Dx, Dy
// ROXL.L Dx, Dy
// ROXL.B #<data>, Dy
// ROXL.W #<data>, Dy
// ROXL.L #<data>, Dy
// ROXR.B Dx, Dy
// ROXR.W Dx, Dy
// ROXR.L Dx, Dy
// ROXR.B #<data>, Dy
// ROXR.W #<data>, Dy
// ROXR.L #<data>, Dy
struct Cil_rotate_extend_reg: Cil {
    Cil_rotate_extend_reg(): Cil("rotate_extend_reg", Cil_family,
                                   OP(14) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 4>(2)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        unsigned n = extract<9, 11>(w0);                // "count/register"
        SgAsmExpression *sa = NULL;
        if (extract<5, 5>(w0)) {                        // "i/r"
            sa = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i32, 0);
        } else {
            sa = d->makeImmediateValue(state, Cil_fmt_i8, 0==n ? 8 : n);
        }
        SgAsmExpression *dy = d->makeDataRegister(state, extract<0, 2>(w0), fmt, 0);
        CilInstructionKind kind = extract<8, 8>(w0) ? Cil_roxl : Cil_roxr; // "dr"
        std::string mnemonic = stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_") + "." + formatLetter(fmt);
        return d->makeInstruction(state, kind, mnemonic, sa, dy);
    }
};

// ROXL.W <ea>y          memory rotate left
// ROXR.W <ea>y          memory rotate right
struct Cil_rotate_extend_mem: Cil {
    Cil_rotate_extend_mem(): Cil("rotate_extend_mem", Cil_family,
                                   OP(14) & BITS<9, 11>(2) & BITS<6, 7>(3)
                                   & EAM(Cil_eam_memory & Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *eay = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i16, 0);
        CilInstructionKind kind = extract<8, 8>(w0) ? Cil_roxl : Cil_roxr; // "dr"
        std::string mnemonic = stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_") + ".w";
        return d->makeInstruction(state, kind, mnemonic, eay);
    }
};

// RTD #<displacement>
struct Cil_rtd: Cil {
    Cil_rtd(): Cil("rtd", Cil_68010|Cil_68020|Cil_68030|Cil_68040|Cil_freescale_cpu32,
                     OP(4) & BITS<0, 11>(0xe74)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *disp = d->makeImmediateExtension(state, Cil_fmt_i16, 0);
        return d->makeInstruction(state, Cil_rtd, "rtd", disp);
    }
};

// RTM Rn
struct Cil_rtm: Cil {
    Cil_rtm(): Cil("rtm", Cil_68020,
                     OP(0) & BITS<4, 11>(0x6c)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *rn = d->makeDataAddressRegister(state, extract<0, 3>(w0), Cil_fmt_i32, 0);
        return d->makeInstruction(state, Cil_rtm, "rtm", rn);
    }
};

// RTR
struct Cil_rtr: Cil {
    Cil_rtr(): Cil("rtr", Cil_family,
                     OP(4) & BITS<0, 11>(0xe77)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        return d->makeInstruction(state, Cil_rtr, "rtr");
    }
};

// RTS
struct Cil_rts: Cil {
    Cil_rts(): Cil("rts", Cil_family,
                     OP(4) & BITS<0, 11>(0xe75)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        return d->makeInstruction(state, Cil_rts, "rts");
    }
};

// SBCD.B Dx, Dy
// SBCD.B -(Ax), -(Ay)
struct Cil_sbcd: Cil {
    Cil_sbcd(): Cil("sbcd", Cil_family,
                      OP(8) & BITS<4, 8>(0x10)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src=NULL, *dst=NULL;
        if (extract<3, 3>(w0)) {
            // SBCD.B -(Ax), -(Ay)
            src = d->makeAddressRegisterPreDecrement(state, extract<0, 2>(w0), Cil_fmt_i8);
            dst = d->makeAddressRegisterPreDecrement(state, extract<9, 11>(w0), Cil_fmt_i8);
        } else {
            // SBCD.B Dx, Dy
            src = d->makeDataRegister(state, extract<0, 2>(w0), Cil_fmt_i8, 0);
            dst = d->makeDataRegister(state, extract<9, 11>(w0), Cil_fmt_i8, 0);
        }
        return d->makeInstruction(state, Cil_sbcd, "sbcd.b", src, dst);
    }
};

// Scc.B <ea>
//      Mnemonic Condition              Value (table 3.19)
//      t        true                   0000    0
//      f        false                  0001    1
//      hi       hi                     0010    2
//      ls       lo or same             0011    3
//      cc(hi)   carry clear            0100    4
//      cs(lo)   carry set              0101    5
//      ne       not equal              0110    6
//      eq       equal                  0111    7
//      vc       overflow clear         1000    8
//      vs       overflow set           1001    9
//      pl       plus                   1010    a
//      mi       minus                  1011    b
//      ge       greater or equal       1100    c
//      lt       less than              1101    d
//      gt       greater than           1110    e
//      le       less or equal          1111    f
struct Cil_scc: Cil {
    Cil_scc(): Cil("scc", Cil_family,
                     OP(5) & BITS<6, 7>(3) & EAM(Cil_eam_data & Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<8, 11>(w0)) {
            case 0x0: kind = Cil_st;  break;
            case 0x1: kind = Cil_sf;  break;
            case 0x2: kind = Cil_shi; break;
            case 0x3: kind = Cil_sls; break;
            case 0x4: kind = Cil_scc; break;
            case 0x5: kind = Cil_scs; break;
            case 0x6: kind = Cil_sne; break;
            case 0x7: kind = Cil_seq; break;
            case 0x8: kind = Cil_svc; break;
            case 0x9: kind = Cil_svs; break;
            case 0xa: kind = Cil_spl; break;
            case 0xb: kind = Cil_smi; break;
            case 0xc: kind = Cil_sge; break;
            case 0xd: kind = Cil_slt; break;
            case 0xe: kind = Cil_sgt; break;
            case 0xf: kind = Cil_sle; break;
        }
        std::string mnemonic = stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_") + ".b";
        SgAsmExpression *ea = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i8, 0);
        return d->makeInstruction(state, kind, mnemonic, ea);
    }
};

// SUB.B <ea>y, Dx
// SUB.W <ea>y, Dx
// SUB.L <ea>y, Dx
struct Cil_sub_1: Cil {
    Cil_sub_1(): Cil("sub_1", Cil_family,
                       OP(9) & BIT<8>(0) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & EAM(Cil_eam_all)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeDataRegister(state, extract<9, 11>(w0), fmt);
        return d->makeInstruction(state, Cil_sub, "sub."+formatLetter(fmt), src, dst);
    }
};

// SUB.B Dy, <ea>x
// SUB.W Dy, <ea>x
// SUB.L Dy, <ea>x
struct Cil_sub_2: Cil {
    Cil_sub_2(): Cil("sub_2", Cil_family,
                       OP(9) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                       EAM(Cil_eam_memory & Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeDataRegister(state, extract<9, 11>(w0), fmt);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(state, Cil_sub, "sub."+formatLetter(fmt), src, dst);
    }
};

// SUBA.W <ea>, Ax
// SUBA.L <ea>, Ax
struct Cil_suba: Cil {
    Cil_suba(): Cil("suba", Cil_family,
                      OP(9) & BITS<6, 7>(3) & EAM(Cil_eam_all)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = extract<8, 8>(w0) ? Cil_fmt_i32 : Cil_fmt_i16;
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeAddressRegister(state, extract<9, 11>(w0), Cil_fmt_i32, 0);
        return d->makeInstruction(state, Cil_suba, "suba."+formatLetter(fmt), src, dst);
    }
};

// SUBI.B #<data>, Dx
// SUBI.W #<data>, Dx
// SUBI.L #<data>, Dx
struct Cil_subi: Cil {
    Cil_subi(): Cil("subi", Cil_family,
                      OP(0) & BITS<8, 10>(4) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(Cil_eam_data & Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        size_t opandWords = (formatNBits(fmt)+15) / 16;
        SgAsmExpression *src = d->makeImmediateExtension(state, fmt, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, opandWords);
        return d->makeInstruction(state, Cil_subi, "subi."+formatLetter(fmt), src, dst);
    }
};

// SUBQ.B #<data>, <ea>x
// SUBQ.W #<data>, <ea>x
// SUBQ.L #<data>, <ea>x
//      Address register direct modes are handled by subq_a
struct Cil_subq: Cil {
    Cil_subq(): Cil("subq", Cil_family,
                      OP(5) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(Cil_eam_alter & ~Cil_eam_pcmi & ~Cil_eam_ard)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        unsigned n = extract<9, 11>(w0);
        SgAsmExpression *src = d->makeImmediateValue(state, fmt, n?n:8);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(state, Cil_subq, "subq."+formatLetter(fmt), src, dst);
    }
};

// SUBQ.W #<data>, Ax  (but treated as long)
// SUBQ.L #<data>, Ax
struct Cil_subq_a: Cil {
    Cil_subq_a(): Cil("subq_a", Cil_family,
                        OP(5) & BIT<8>(1) & (BITS<6, 7>(1) | BITS<6, 7>(2)) & EAM(Cil_eam_ard)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = Cil_fmt_i32;              // word-mode is like long-mode when writing to an address register
        unsigned n = extract<9, 11>(w0);
        SgAsmExpression *src = d->makeImmediateValue(state, fmt, n?n:8);
        SgAsmExpression *dst = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(state, Cil_subq, "subq."+formatLetter(fmt), src, dst);
    }
};

// SUBX.B Dx, Dy
// SUBX.W Dx, Dy
// SUBX.L Dx, Dy
// SUBX.B -(Ax), -(Ay)
// SUBX.W -(Ax), -(Ay)
// SUBX.L -(Ax), -(Ay)
struct Cil_subx: Cil {
    Cil_subx(): Cil("subx", Cil_family,
                      OP(9) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<4, 5>(0)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src=NULL, *dst=NULL;
        if (extract<3, 3>(w0)) {
            // SUBX.? -(Ax), -(Ay)
            src = d->makeAddressRegisterPreDecrement(state, extract<0, 2>(w0), fmt);
            dst = d->makeAddressRegisterPreDecrement(state, extract<9, 11>(w0), fmt);
        } else {
            // SUBX.? Dx, Dy
            src = d->makeDataRegister(state, extract<0, 2>(w0), fmt, 0);
            dst = d->makeDataRegister(state, extract<9, 11>(w0), fmt, 0);
        }
        return d->makeInstruction(state, Cil_subx, "subx."+formatLetter(fmt), src, dst);
    }
};

// SWAP.W Dx
struct Cil_swap: Cil {
    Cil_swap(): Cil("swap", Cil_family,
                      OP(4) & BITS<3, 11>(0x108)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(state, extract<0, 2>(w0), Cil_fmt_i32, 0);
        return d->makeInstruction(state, Cil_swap, "swap.w", src); // mnemonic is "w" because it swaps words of a 32-bit reg
    }
};

// TAS.B <ea>x
struct Cil_tas: Cil {
    Cil_tas(): Cil("tas", Cil_family,
                     OP(4) & BITS<6, 11>(0x2b) & EAM(Cil_eam_data & Cil_eam_alter & ~Cil_eam_pcmi)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), Cil_fmt_i8, 0);
        return d->makeInstruction(state, Cil_tas, "tas.b", src);
    }
};

// TRAP #<vector>
struct Cil_trap: Cil {
    Cil_trap(): Cil("trap", Cil_family,
                      OP(4) & BITS<4, 11>(0x0e4)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *vector = d->makeImmediateValue(state, Cil_fmt_i8, extract<0, 3>(w0));
        return d->makeInstruction(state, Cil_trap, "trap", vector);
    }
};

// TRAPcc
// TRAPcc.W #<data>
// TRAPcc.L #<data>
//      Mnemonic Condition              Value (table 3.19)
//      t        true                   0000    0
//      f        false                  0001    1
//      hi       hi                     0010    2
//      ls       lo or same             0011    3
//      cc(hi)   carry clear            0100    4
//      cs(lo)   carry set              0101    5
//      ne       not equal              0110    6
//      eq       equal                  0111    7
//      vc       overflow clear         1000    8
//      vs       overflow set           1001    9
//      pl       plus                   1010    a
//      mi       minus                  1011    b
//      ge       greater or equal       1100    c
//      lt       less than              1101    d
//      gt       greater than           1110    e
//      le       less or equal          1111    f
struct Cil_trapcc: Cil {
    Cil_trapcc(): Cil("trapcc", Cil_68020|Cil_68030|Cil_68040|Cil_freescale_cpu32,
                        OP(5) & BITS<3, 7>(0x1f) & (BITS<0, 2>(2) | BITS<0, 2>(3) | BITS<0, 2>(4))) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = Cil_fmt_unknown;
        switch (extract<0, 2>(w0)) {
            case 2: fmt = Cil_fmt_i16; break;
            case 3: fmt = Cil_fmt_i32; break;
            case 4: fmt = Cil_fmt_unknown; break;                   // no operand
        }
        CilInstructionKind kind = Cil_unknown_instruction;
        switch (extract<8, 11>(w0)) {
            case 0x0: kind = Cil_trapt;  break;
            case 0x1: kind = Cil_trapf;  break;        // aka. TPF
            case 0x2: kind = Cil_traphi; break;
            case 0x3: kind = Cil_trapls; break;
            case 0x4: kind = Cil_trapcc; break;
            case 0x5: kind = Cil_trapcs; break;
            case 0x6: kind = Cil_trapne; break;
            case 0x7: kind = Cil_trapeq; break;
            case 0x8: kind = Cil_trapvc; break;
            case 0x9: kind = Cil_trapvs; break;
            case 0xa: kind = Cil_trappl; break;
            case 0xb: kind = Cil_trapmi; break;
            case 0xc: kind = Cil_trapge; break;
            case 0xd: kind = Cil_traplt; break;
            case 0xe: kind = Cil_trapgt; break;
            case 0xf: kind = Cil_traple; break;
        }
        if (Cil_fmt_unknown==fmt) {
            std::string mnemonic = stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_");
            return d->makeInstruction(state, kind, mnemonic);
        } else {
            SgAsmExpression *data = d->makeImmediateExtension(state, fmt, 0);
            std::string mnemonic = stringifyBinaryAnalysisCilInstructionKind(kind, "Cil_") + "." + formatLetter(fmt);
            return d->makeInstruction(state, kind, mnemonic, data);
        }
    }
};
        
// TRAPV
struct Cil_trapv: Cil {
    Cil_trapv(): Cil("trapv", Cil_family,
                       OP(4) & BITS<0, 11>(0xe76)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        return d->makeInstruction(state, Cil_trapv, "trapv");
    }
};

// TST.B <ea>y
// TST.W <ea>y
// TST.L <ea>y
struct Cil_tst: Cil {
    Cil_tst(): Cil("tst", Cil_family,
                     OP(4) & BITS<8, 11>(0xa) & EAM(Cil_eam_all)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        CilDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(state, Cil_tst, "tst."+formatLetter(fmt), src);
    }
};

// UNLK Ax
struct Cil_unlk: Cil {
    Cil_unlk(): Cil("unlk", Cil_family,
                      OP(4) & BITS<3, 11>(0x1cb)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *dst = d->makeAddressRegister(state, extract<0, 2>(w0), Cil_fmt_i32, 0);
        return d->makeInstruction(state, Cil_unlk, "unlk", dst);
    }
};

// UNPK Dx, Dy, #<adjustment>
// UNPK -(Ax), -(Ay), #<adjustment>
struct Cil_unpk: Cil {
    Cil_unpk(): Cil("unpk", Cil_68020|Cil_68030|Cil_68040,
                      OP(8) & BITS<4, 8>(0x18)) {}
    SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
        SgAsmExpression *src=NULL, *dst=NULL;
        if (extract<3, 3>(w0)) {
            // UNPK -(Ax), -(Ay), #<adjustment>
            src = d->makeAddressRegisterPreDecrement(state, extract<0, 2>(w0), Cil_fmt_i16);
            dst = d->makeAddressRegisterPreDecrement(state, extract<9, 11>(w0), Cil_fmt_i16);
        } else {
            // UNPK Dx, Dy, #<adjustment>
            src = d->makeDataRegister(state, extract<0, 2>(w0), Cil_fmt_i16, 0);
            dst = d->makeDataRegister(state, extract<0, 2>(w0), Cil_fmt_i16, 0);
        }
        SgAsmExpression *adj = d->makeImmediateExtension(state, Cil_fmt_i16, 0);
        return d->makeInstruction(state, Cil_unpk, "unpk", src, dst, adj);
    }
};

#endif

// DQ (10/20/2021): End of commented out instructions for M68k.
#endif


    
//                              Instructions from MFC5484 that we don't use anymore
//  
//  
//  
//  // BITREV.L Dx
//  struct Cil_bitrev: Cil {
//      Cil_bitrev(): Cil("bitrev", OP(0) & BITS<3, 11>(0x18)) {}
//      SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
//          return d->makeInstruction(state, Cil_bitrev, "bitrev.l", d->makeDataRegister(state, extract<0, 2>(w0), 32));
//      }
//  };
//  
//  // BYTEREV.L Dx
//  struct Cil_byterev: Cil {
//      Cil_byterev(): Cil("byterev", OP(0) & BITS<3, 11>(0x58)) {}
//      SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
//          return d->makeInstruction(state, Cil_byterev, "byterev.l", d->makeDataRegister(state, extract<0, 2>(w0), 32));
//      }
//  };
//  
//  // FF1.L Dx
//  struct Cil_ff1: Cil {
//      Cil_ff1(): Cil("ff1", OP(0) & BITS<3, 11>(0x98)) {}
//      SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
//          return d->makeInstruction(state, Cil_ff1, "ff1.l", d->makeDataRegister(state, extract<0, 2>(w0), 32));
//      }
//  };
//  
//  
//  
//  // MOVE.L MACSR, CCR
//  struct Cil_move_macsr2ccr: Cil {
//      Cil_move_macsr2ccr(): Cil("move_macsr2ccr", OP(10) & BITS<0, 11>(0x9c0)) {}
//      SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
//          return d->makeInstruction(state, Cil_move, "move.l",
//                                    d->makeRegister(RegisterDescriptor(Cil_regclass_mac, Cil_mac_macsr, 0, 32)),
//                                    d->makeConditionCodeRegister(state));
//      }
//  };
//  
//  
//  // MSAC.W Ry.{U,L}, Rx.{U,L} SF, <ea>y&, Rw
//  struct Cil_msac_w2: Cil {
//      Cil_msac_w2(): Cil("msac_w2", OP(10) & BITS<7, 8>(1) & EAM(Cil_eam_ari|Cil_eam_inc|Cil_eam_dec|Cil_eam_dsp) &
//                           INSN_WORD<1>(BIT<11>(0) & BIT<8>(1) & BIT<4>(0))) {}
//      SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
//          size_t ry_offset = extract<6, 6>(d->instructionWord(state, 1)) ? 16 : 0;
//          size_t rx_offset = extract<7, 7>(d->instructionWord(state, 1)) ? 16 : 0;
//          SgAsmExpression *ry = d->makeDataAddressRegister(state, extract<0, 3>(d->instructionWord(state, 1)), 16, ry_offset);
//          SgAsmExpression *rx = d->makeDataAddressRegister(state, extract<12, 15>(d->instructionWord(state, 1)), 16, rx_offset);
//          SgAsmExpression *sf = d->makeImmediateValue(state, 8, extract<9, 10>(d->instructionWord(state, 1)));
//          SgAsmExpression *eay = d->makeEffectiveAddress(state, extract<0, 5>(w0), 32, 1);
//          SgAsmExpression *use_mask = d->makeImmediateValue(state, 1, extract<5, 5>(d->instructionWord(state, 1)));
//          SgAsmExpression *rw = extract<6, 6>(w0) ?
//                                d->makeAddressRegister(state, extract<9, 11>(w0), 32) :
//                                d->makeDataRegister(state, extract<9, 11>(w0), 32);
//          return d->makeInstruction(state, Cil_msac, "msac.w", ry, rx, sf, eay, use_mask, rw);
//      }
//  };
//  
//  // PULSE
//  struct Cil_pulse: Cil {
//      Cil_pulse(): Cil("pulse", OP(4) & BITS<0, 11>(0xacc)) {}
//      SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
//          return d->makeInstruction(state, Cil_pulse, "pulse");
//      }
//  };
//  
//  // SATS.L Dx
//  struct Cil_sats: Cil {
//      Cil_sats(): Cil("sats", OP(4) & BITS<3, 11>(0x190)) {}
//      SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
//          SgAsmExpression *dx = d->makeDataRegister(state, extract<0, 2>(w0), 32);
//          return d->makeInstruction(state, Cil_sats, "sats.l", dx);
//      }
//  };
//  
//                  
//  // TPF
//  // TPF.W #<data>
//  // TPF.L #<data>
//  struct Cil_tpf: Cil {
//      Cil_tpf(): Cil("tpf", OP(5) & BITS<3, 11>(0x3f)) {}
//      SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
//          SgAsmExpression *src = NULL;
//          unsigned opmode = extract<0, 2>(w0);
//          switch (opmode) {
//              case 2:
//                  src = d->makeImmediateExtension(state, 16, 0);
//                  return d->makeInstruction(state, Cil_tpf, "tpf.w", src);
//              case 3:
//                  src = d->makeImmediateExtension(state, 32, 0);
//                  return d->makeInstruction(state, Cil_tpf, "tpf.l", src);
//              case 4:
//                  return d->makeInstruction(state, Cil_tpf, "tpf");
//              default:
//                  throw Disassembler::Exception("invalid opmode "+numberToString(opmode)+" for TPF instruction",
//                                                state.insn_va);
//          }
//      }
//  };
//  
//  // WDDATA.B <ea>y
//  // WDDATA.W <ea>y
//  // WDDATA.L <ea>y
//  //
//  // Note: The reference manual has a table which indicates that word and long absolute addressing modes are permitted, but the
//  // text states "use only those memory alterable addressing modes listed in the following table". According to section 2.2.13,
//  // the absolute addressing modes are not in the set of alterable addressing modes. [Robb P. Matzke 2013-10-02]
//  struct Cil_wddata: Cil {
//      Cil_wddata(): Cil("wddata", OP(15) & BITS<8, 11>(0xb) & EAM(Cil_eam_memory & Cil_eam_alter)) {}
//      SgAsmCilInstruction *operator()(DisassemblerCil::State &state, const D *d, unsigned w0) {
//          size_t nbits = 0;
//          switch (extract<6, 7>(w0)) {
//              case 0: nbits = 8; break;
//              case 1: nbits = 16; break;
//              case 2: nbits = 32; break;
//              case 3: throw Disassembler::Exception("invalid size for WDDATA instruction", state.insn_va);
//          }
//          SgAsmExpression *src = d->makeEffectiveAddress(state, extract<0, 5>(w0), nbits, 0);
//          return d->makeInstruction(state, Cil_wddata, "wddata."+sizeToLetter(nbits), src);
//      }
//  };
        
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
DisassemblerCil::init()
{
    // Default register dictionary
    const RegisterDictionary *regdict = NULL;


#if 1
 // DQ (10/9/2021): Eliminate this code for Cil.
    name("Cil");
    regdict = RegisterDictionary::dictionary_Cil();
#endif

    registerDictionary(regdict);
    REG_IP = registerDictionary()->findOrThrow("pc");
    REG_SP = registerDictionary()->findOrThrow("a7");

    p_proto_dispatcher = InstructionSemantics2::DispatcherCil::instance();
    p_proto_dispatcher->addressWidth(32);
    p_proto_dispatcher->registerDictionary(regdict);

    wordSizeBytes(2);
    instructionAlignment_ = 2;
    byteOrder(ByteOrder::ORDER_MSB);

#if 0
 // DQ (10/9/2021): Eliminate this code for Cil, this line of code does not compile.
    callingConventions(CallingConvention::dictionaryCil());
#else
    callingConventions(CallingConvention::dictionaryM68k());
#endif

    idis_table.resize(17);

#define Cil_DECODER(NAME)                                                                                                     \
    static Cil_##NAME *v_##NAME=NULL;                                                                                         \
    if (NULL==v_##NAME)                                                                                                        \
        v_##NAME = new Cil_##NAME;                                                                                            \
    insert_idis(v_##NAME);

#if 0
 // DQ (10/9/2021): Eliminate this code for Cil.

    // Integer instructions from pages 4-1 through 4-198
    Cil_DECODER(abcd_1);
    Cil_DECODER(abcd_2);
    Cil_DECODER(add_1);
    Cil_DECODER(add_2);
    Cil_DECODER(add_3);
    Cil_DECODER(adda);
    Cil_DECODER(addi);
    Cil_DECODER(addq);
    Cil_DECODER(addx_1);
    Cil_DECODER(addx_2);
    Cil_DECODER(and_1);
    Cil_DECODER(and_2);
    Cil_DECODER(andi);
    Cil_DECODER(andi_to_ccr);
    Cil_DECODER(ashift_1);
    Cil_DECODER(ashift_2);
    Cil_DECODER(ashift_3);
    Cil_DECODER(branch);
    Cil_DECODER(bchg_1);
    Cil_DECODER(bchg_2);
    Cil_DECODER(bchg_3);
    Cil_DECODER(bchg_4);
    Cil_DECODER(bclr_1);
    Cil_DECODER(bclr_2);
    Cil_DECODER(bclr_3);
    Cil_DECODER(bclr_4);
    Cil_DECODER(bfchg);
    Cil_DECODER(bfclr);
    Cil_DECODER(bfexts);
    Cil_DECODER(bfextu);
    Cil_DECODER(bfins);
    Cil_DECODER(bfset);
    Cil_DECODER(bftst);
    Cil_DECODER(bkpt);
    Cil_DECODER(bset_1);
    Cil_DECODER(bset_2);
    Cil_DECODER(bset_3);
    Cil_DECODER(bset_4);
    Cil_DECODER(btst_1);
    Cil_DECODER(btst_2);
    Cil_DECODER(btst_3);
    Cil_DECODER(btst_4);
    Cil_DECODER(callm);
    Cil_DECODER(cas);
    Cil_DECODER(cas2);
    Cil_DECODER(chk);
    Cil_DECODER(chk2);
    Cil_DECODER(clr);
    Cil_DECODER(cmp);
    Cil_DECODER(cmpa);
    Cil_DECODER(cmpi);
    Cil_DECODER(cmpm);
    Cil_DECODER(cmp2);
    Cil_DECODER(cpbcc);
    Cil_DECODER(cpdbcc);
    Cil_DECODER(cpscc);
    Cil_DECODER(cptrapcc);
    Cil_DECODER(cpush);
    Cil_DECODER(dbcc);
    Cil_DECODER(divide);
    Cil_DECODER(divs_w);
    Cil_DECODER(divu_w);
    Cil_DECODER(eor);
    Cil_DECODER(eori);
    Cil_DECODER(eori_to_ccr);
    Cil_DECODER(exg);
    Cil_DECODER(ext_w);
    Cil_DECODER(ext_l);
    Cil_DECODER(extb_l);
    Cil_DECODER(fabs_from_drd);
    Cil_DECODER(fabs_mr);
    Cil_DECODER(fabs_rr);
    Cil_DECODER(fadd_from_drd);
    Cil_DECODER(fadd_mr);
    Cil_DECODER(fadd_rr);
    Cil_DECODER(fb);
    Cil_DECODER(fcmp_from_drd);
    Cil_DECODER(fcmp_mr);
    Cil_DECODER(fcmp_rr);
    Cil_DECODER(fdiv_from_drd);
    Cil_DECODER(fdiv_mr);
    Cil_DECODER(fdiv_rr);
    Cil_DECODER(fint_from_drd);
    Cil_DECODER(fint_mr);
    Cil_DECODER(fint_rr);
    Cil_DECODER(fintrz_from_drd);
    Cil_DECODER(fintrz_mr);
    Cil_DECODER(fintrz_rr);
    Cil_DECODER(fmove_from_drd);
    Cil_DECODER(fmove_mr);
    Cil_DECODER(fmove_rm);
    Cil_DECODER(fmove_rr);
    Cil_DECODER(fmove_to_drd);
    Cil_DECODER(fmovem_mr);
    Cil_DECODER(fmovem_rm);
    Cil_DECODER(fmul_from_drd);
    Cil_DECODER(fmul_mr);
    Cil_DECODER(fmul_rr);
    Cil_DECODER(fneg_from_drd);
    Cil_DECODER(fneg_mr);
    Cil_DECODER(fneg_rr);
    Cil_DECODER(fnop);
    Cil_DECODER(fsqrt_from_drd);
    Cil_DECODER(fsqrt_mr);
    Cil_DECODER(fsqrt_rr);
    Cil_DECODER(fsub_from_drd);
    Cil_DECODER(fsub_mr);
    Cil_DECODER(fsub_rr);
    Cil_DECODER(ftst_from_drd);
    Cil_DECODER(ftst_mr);
    Cil_DECODER(ftst_rr);
    Cil_DECODER(illegal);
    Cil_DECODER(jmp);
    Cil_DECODER(jsr);
    Cil_DECODER(lea);
    Cil_DECODER(link_w);
    Cil_DECODER(link_l);
    Cil_DECODER(lshift_rr);
    Cil_DECODER(lshift_ir);
    Cil_DECODER(lshift_mem);
    Cil_DECODER(mac);
    Cil_DECODER(mac_l2);
    Cil_DECODER(mac_w2);
    Cil_DECODER(mov3q);
    Cil_DECODER(movclr);
    Cil_DECODER(move);
    Cil_DECODER(move_from_acc);
    Cil_DECODER(move_from_accext);
    Cil_DECODER(move_from_ccr);
    Cil_DECODER(move_from_macsr);
    Cil_DECODER(move_from_mask);
    Cil_DECODER(move_from_sr);
    Cil_DECODER(move_to_acc);
    Cil_DECODER(move_to_accext);
    Cil_DECODER(move_to_ccr);
    Cil_DECODER(move_to_cr);
    Cil_DECODER(move_to_macsr);
    Cil_DECODER(move_to_mask);
    Cil_DECODER(move_to_sr);
    Cil_DECODER(move16_pp);
    Cil_DECODER(move16_a);
    Cil_DECODER(movea);
    Cil_DECODER(movem_mr);
    Cil_DECODER(movem_rm);
    Cil_DECODER(movep);
    Cil_DECODER(moveq);
    Cil_DECODER(msac);
    Cil_DECODER(muls_w);
    Cil_DECODER(multiply_l);
    Cil_DECODER(multiply_64);
    Cil_DECODER(mulu_w);
    Cil_DECODER(mvs);
    Cil_DECODER(mvz);
    Cil_DECODER(nbcd);
    Cil_DECODER(neg);
    Cil_DECODER(negx);
    Cil_DECODER(nop);
    Cil_DECODER(not);
    Cil_DECODER(or_1);
    Cil_DECODER(or_2);
    Cil_DECODER(ori);
    Cil_DECODER(ori_to_ccr);
    Cil_DECODER(pack);
    Cil_DECODER(pea);
    Cil_DECODER(rotate_reg);
    Cil_DECODER(rotate_mem);
    Cil_DECODER(rotate_extend_reg);
    Cil_DECODER(rotate_extend_mem);
    Cil_DECODER(rtd);
    Cil_DECODER(rtm);
    Cil_DECODER(rtr);
    Cil_DECODER(rts);
    Cil_DECODER(sbcd);
    Cil_DECODER(scc);
    Cil_DECODER(sub_1);
    Cil_DECODER(sub_2);
    Cil_DECODER(suba);
    Cil_DECODER(subi);
    Cil_DECODER(subq);
    Cil_DECODER(subq_a);
    Cil_DECODER(subx);
    Cil_DECODER(swap);
    Cil_DECODER(tas);
    Cil_DECODER(trap);
    Cil_DECODER(trapcc);
    Cil_DECODER(trapv);
    Cil_DECODER(tst);
    Cil_DECODER(unlk);
    Cil_DECODER(unpk);
#endif

    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"Cil instruction disassembly table indexed by high-order nybble of first 16-bit word:\n";
        for (size_t i=0; i<idis_table.size(); ++i) {
            mlog[DEBUG] <<"  bucket " <<StringUtility::addrToString(i, 8) <<" has "
                        <<StringUtility::plural(idis_table[i].size(), "entries") <<":";
            for (IdisList::iterator li=idis_table[i].begin(); li!=idis_table[i].end(); ++li)
                mlog[DEBUG] <<" " <<(*li)->name;
            mlog[DEBUG] <<"\n";
        }
    }
}

} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::DisassemblerCil);
#endif

#endif
