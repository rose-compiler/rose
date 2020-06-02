#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"
#include "DisassemblerM68k.h"

#include "Diagnostics.h"
#include "integerOps.h"
#include "stringify.h"
#include "SageBuilderAsm.h"
#include "DispatcherM68k.h"
#include "BinaryUnparserM68k.h"

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
            M68kEffectiveAddressMode eam = (M68kEffectiveAddressMode)bit;
            switch (eam) {
                case m68k_eam_drd:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, reg, 0);
                    break;
                case m68k_eam_ard:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, 0x08|reg, 0);
                    break;
                case m68k_eam_ari:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, 0x10|reg, 0);
                    break;
                case m68k_eam_inc:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, 0x18|reg, 0);
                    break;
                case m68k_eam_dec:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, 0x20|reg, 0);
                    break;
                case m68k_eam_dsp:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, 0x28|reg, 0);
                    break;
                case m68k_eam_idx8:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, 0x30|reg, 0);
                    break;
                case m68k_eam_idxbd:
                    if (0 == (eamodes & m68k_eam_idx8)) {
                        for (size_t reg=0; reg<8; ++reg)
                            retval |= Pattern(lobit, hibit, 0x30|reg, 0);
                    }
                    break;
                case m68k_eam_mpost:
                    if (0 == (eamodes & m68k_eam_idx)) {
                        for (size_t reg=0; reg<8; ++reg)
                            retval |= Pattern(lobit, hibit, 0x30|reg, 0);
                    }
                    break;
                case m68k_eam_mpre:
                    if (0 == (eamodes & (m68k_eam_idx | m68k_eam_mpost))) {
                        for (size_t reg=0; reg<8; ++reg)
                            retval |= Pattern(lobit, hibit, 0x30|reg, 0);
                    }
                    break;
                case m68k_eam_pcdsp:
                    retval |= Pattern(lobit, hibit, 0x3a, 0);
                    break;
                case m68k_eam_pcidx8:
                    retval |= Pattern(lobit, hibit, 0x3b, 0);
                    break;
                case m68k_eam_pcidxbd:
                    if (0 == (eamodes & m68k_eam_pcidx8))
                        retval |= Pattern(lobit, hibit, 0x3b, 0);
                    break;
                case m68k_eam_pcmpost:
                    if (0 == (eamodes & m68k_eam_pcidx))
                        retval |= Pattern(lobit, hibit, 0x3b, 0);
                    break;
                case m68k_eam_pcmpre:
                    if (0 == (eamodes & (m68k_eam_pcidx | m68k_eam_pcmpost)))
                        retval |= Pattern(lobit, hibit, 0x3b, 0);
                    break;
                case m68k_eam_absw:
                    retval |= Pattern(lobit, hibit, 0x38, 0);
                    break;
                case m68k_eam_absl:
                    retval |= Pattern(lobit, hibit, 0x39, 0);
                    break;
                case m68k_eam_imm:
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
            M68kEffectiveAddressMode eam = (M68kEffectiveAddressMode)bit;
            switch (eam) {
                case m68k_eam_drd:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, reg<<3, 0);
                    break;
                case m68k_eam_ard:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, (reg<<3)|1, 0);
                    break;
                case m68k_eam_ari:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, (reg<<3)|2, 0);
                    break;
                case m68k_eam_inc:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, (reg<<3)|3, 0);
                    break;
                case m68k_eam_dec:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, (reg<<3)|4, 0);
                    break;
                case m68k_eam_dsp:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, (reg<<3)|5, 0);
                    break;
                case m68k_eam_idx8:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, (reg<<3)|6, 0);
                    break;
                case m68k_eam_idxbd:
                    if (0 == (eamodes & m68k_eam_idx8)) {
                        for (size_t reg=0; reg<8; ++reg)
                            retval |= Pattern(lobit, hibit, (reg<<3)|6, 0);
                    }
                    break;
                case m68k_eam_mpost:
                    if (0 == (eamodes & m68k_eam_idx)) {
                        for (size_t reg=0; reg<8; ++reg)
                            retval |= Pattern(lobit, hibit, (reg<<3)|6, 0);
                    }
                    break;
                case m68k_eam_mpre:
                    if (0 == (eamodes & (m68k_eam_idx | m68k_eam_mpost))) {
                        for (size_t reg=0; reg<8; ++reg)
                            retval |= Pattern(lobit, hibit, (reg<<3)|6, 0);
                    }
                    break;
                case m68k_eam_pcdsp:
                    retval |= Pattern(lobit, hibit, 0x17, 0);
                    break;
                case m68k_eam_pcidx8:
                    retval |= Pattern(lobit, hibit, 0x1f, 0);
                    break;
                case m68k_eam_pcidxbd:
                    if (0 == (eamodes & m68k_eam_pcidx8))
                        retval |= Pattern(lobit, hibit, 0x1f, 0);
                    break;
                case m68k_eam_pcmpost:
                    if (0 == (eamodes & m68k_eam_pcidx))
                        retval |= Pattern(lobit, hibit, 0x1f, 0);
                    break;
                case m68k_eam_pcmpre:
                    if (0 == (eamodes & (m68k_eam_pcidx | m68k_eam_pcmpost)))
                        retval |= Pattern(lobit, hibit, 0x1f, 0);
                    break;
                case m68k_eam_absw:
                    retval |= Pattern(lobit, hibit, 0x07, 0);
                    break;
                case m68k_eam_absl:
                    retval |= Pattern(lobit, hibit, 0x0f, 0);
                    break;
                case m68k_eam_imm:
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

static M68kDataFormat
integerFormat(unsigned fmtNumber)
{
    switch (fmtNumber) {
        case 0: return m68k_fmt_i8;
        case 1: return m68k_fmt_i16;
        case 2: return m68k_fmt_i32;
        case 3: return m68k_fmt_i16;
    }
    ASSERT_not_reachable("invalid integer format number: " + numberToString(fmtNumber));
}

// Floating point format as encoded in a floating-point instruction.  The M68kDataFormat enums have the same value as the
// format field in the instruction.
static M68kDataFormat
floatingFormat(unsigned fmtNumber)
{
    switch (fmtNumber) {
        case m68k_fmt_i32:
        case m68k_fmt_f32:
        case m68k_fmt_f96:
        case m68k_fmt_p96:
        case m68k_fmt_i16:
        case m68k_fmt_f64:
        case m68k_fmt_i8 :
            return (M68kDataFormat)fmtNumber;
        default:
            throw Disassembler::Exception("invalid floating point format code=" + StringUtility::numberToString(fmtNumber));
    }
}

// Default format for floating-point operations. Floating point values are converted to this type internally before any
// operations are performed on them.
static M68kDataFormat
floatingFormatForFamily(M68kFamily family)
{
    if (0 != (family & m68k_freescale)) {
        // FreeScale ColdFire processors operate on 64-bit double-precision real format ("D") values stored in 64-bit
        // floating-point data registers.
        return m68k_fmt_f64;
    }
    if (0 != (family & m68k_generation_3)) {
        // The M68040 family of processors operate on 96-bit extended-precision real format ("X") values stored in 80-bit
        // floating-point data registers (16 bits of an X-format value are always zero).
        return m68k_fmt_f96;
    }
    ASSERT_not_reachable("M68k family has no floating point hardware: " + addrToString(family));
}

// Convert a numeric format to a letter that's appended to instruction mnemonics.
static std::string
formatLetter(M68kDataFormat fmt)
{
    switch (fmt) {
        case m68k_fmt_i32: return "l";
        case m68k_fmt_f32: return "s";
        case m68k_fmt_f96: return "x";
        case m68k_fmt_p96: return "p";
        case m68k_fmt_i16: return "w";
        case m68k_fmt_f64: return "d";
        case m68k_fmt_i8:  return "b";
        case m68k_fmt_unknown: ASSERT_not_reachable("m68k_fmt_unknown is not a valid data format");
    }
    ASSERT_not_reachable("invalid m68k data format: " + stringifyBinaryAnalysisM68kDataFormat(fmt));
}

static size_t
formatNBits(M68kDataFormat fmt)
{
    switch (fmt) {
        case m68k_fmt_i32: return 32;
        case m68k_fmt_f32: return 32;
        case m68k_fmt_f96: return 96;
        case m68k_fmt_p96: return 96;
        case m68k_fmt_i16: return 16;
        case m68k_fmt_f64: return 64;
        case m68k_fmt_i8:  return 8;
        case m68k_fmt_unknown: ASSERT_not_reachable("m68k_fmt_unknown is not a valid data format");
    }
    ASSERT_not_reachable("invalid m68k data format: " + stringifyBinaryAnalysisM68kDataFormat(fmt));
}

// see base class
bool
DisassemblerM68k::canDisassemble(SgAsmGenericHeader *header) const
{
    SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_M68K_Family;
}

Unparser::BasePtr
DisassemblerM68k::unparser() const {
    return Unparser::M68k::instance();
}

SgAsmType *
DisassemblerM68k::makeType(M68kDataFormat fmt)
{
    switch (fmt) {
        case m68k_fmt_i32: return SageBuilderAsm::buildTypeU32();
        case m68k_fmt_f32: return SageBuilderAsm::buildIeee754Binary32();
        case m68k_fmt_f96: return SageBuilderAsm::buildTypeM68kFloat96();
        case m68k_fmt_p96: throw Exception("96-bit binary coded decimal not implemented");
        case m68k_fmt_i16: return SageBuilderAsm::buildTypeU16();
        case m68k_fmt_f64: return SageBuilderAsm::buildIeee754Binary64();
        case m68k_fmt_i8:  return SageBuilderAsm::buildTypeU8();
        case m68k_fmt_unknown: ASSERT_not_reachable("m68k_fmt_unknown is not a valid data format");
    }
    ASSERT_not_reachable("invalid m68k data format: " + stringifyBinaryAnalysisM68kDataFormat(fmt));
}

SgAsmRegisterReferenceExpression *
DisassemblerM68k::makeDataRegister(unsigned regnum, M68kDataFormat fmt, size_t bit_offset)
{
    size_t nbits = formatNBits(fmt);
    ASSERT_require2(regnum <= 7, "register number " + numberToString(regnum));
    ASSERT_require2(8==nbits || 16==nbits || 32==nbits, "register is " + plural(nbits, "bits"));
    ASSERT_require2(0==bit_offset || 16==bit_offset, "register offset is " + plural(bit_offset, "bits"));
    ASSERT_require(bit_offset+nbits <= 32);
    RegisterDescriptor desc(m68k_regclass_data, regnum, bit_offset, nbits);
    SgAsmRegisterReferenceExpression *expr = new SgAsmDirectRegisterExpression(desc);
    expr->set_type(makeType(fmt));
    return expr;
}

SgAsmRegisterReferenceExpression *
DisassemblerM68k::makeAddressRegister(unsigned regnum, M68kDataFormat fmt, size_t bit_offset)
{
    size_t nbits = formatNBits(fmt);
    ASSERT_require2(regnum <= 7, "register number " + numberToString(regnum));
    ASSERT_require2(0==bit_offset || 16==bit_offset, "register offset is " + plural(bit_offset, "bits"));
    ASSERT_require(bit_offset+nbits <= 32);
    if (16!=nbits && 32!=nbits) // in particular, 8-bit access is not possible
        throw Exception("invalid address register size: " + numberToString(nbits));
    RegisterDescriptor desc(m68k_regclass_addr, regnum, bit_offset, nbits);
    SgAsmRegisterReferenceExpression *expr = new SgAsmDirectRegisterExpression(desc);
    expr->set_type(makeType(fmt));
    return expr;
}

SgAsmMemoryReferenceExpression *
DisassemblerM68k::makeAddressRegisterPreDecrement(unsigned regnum, M68kDataFormat fmt)
{
    SgAsmType *type = makeType(fmt);
    SgAsmRegisterReferenceExpression *rre = makeAddressRegister(regnum, m68k_fmt_i32);
    size_t nbits = formatNBits(fmt);
    ASSERT_require(nbits % 8 == 0);
    rre->set_adjustment(-(nbits/8)); // pre decrement number of bytes
    return SageBuilderAsm::buildMemoryReferenceExpression(rre, NULL/*segment*/, type);
}

SgAsmMemoryReferenceExpression *
DisassemblerM68k::makeAddressRegisterPostIncrement(unsigned regnum, M68kDataFormat fmt)
{
    SgAsmType *type = makeType(fmt);
    SgAsmRegisterReferenceExpression *rre = makeAddressRegister(regnum, m68k_fmt_i32);
    size_t nbits = formatNBits(fmt);
    ASSERT_require(nbits % 8 == 0);
    rre->set_adjustment(nbits/8); // post increment number of bytes
    return SageBuilderAsm::buildMemoryReferenceExpression(rre, NULL/*segment*/, type);
}
    

SgAsmRegisterReferenceExpression *
DisassemblerM68k::makeDataAddressRegister(unsigned regnum, M68kDataFormat fmt, size_t bit_offset)
{
    ASSERT_require2(regnum <= 15, "register number is " + numberToString(regnum));
    return regnum <= 7 ? makeDataRegister(regnum, fmt, bit_offset) : makeAddressRegister(regnum-8, fmt, bit_offset);
}

SgAsmRegisterNames *
DisassemblerM68k::makeRegistersFromMask(unsigned mask, M68kDataFormat fmt, bool reverse)
{
    SgAsmRegisterNames *reglist = new SgAsmRegisterNames;
    for (unsigned bitnum=0; bitnum<16; ++bitnum) {
        if (0 != (mask & IntegerOps::shl1<unsigned>(bitnum))) {
            SgAsmRegisterReferenceExpression *reg = makeDataAddressRegister(reverse?15-bitnum:bitnum, fmt, 0);
            reglist->get_registers().push_back(reg);
            reg->set_parent(reglist);
        }
    }
    reglist->set_mask(mask);
    reglist->set_type(makeType(fmt));
    return reglist;
}

SgAsmRegisterNames *
DisassemblerM68k::makeFPRegistersFromMask(unsigned mask, M68kDataFormat fmt, bool reverse)
{
    SgAsmRegisterNames *reglist = new SgAsmRegisterNames;
    for (unsigned bitnum=0; bitnum<8; ++bitnum) {
        if (0 != (mask & IntegerOps::shl1<unsigned>(bitnum))) {
            SgAsmRegisterReferenceExpression *reg = makeFPRegister(reverse?8-bitnum:bitnum);
            reglist->get_registers().push_back(reg);
            reg->set_parent(reglist);
        }
    }
    reglist->set_mask(mask);
    reglist->set_type(makeType(fmt));
    return reglist;
}

SgAsmRegisterReferenceExpression *
DisassemblerM68k::makeStatusRegister()
{
    RegisterDescriptor desc(m68k_regclass_spr, m68k_spr_sr, 0, 16);
    SgAsmRegisterReferenceExpression *expr = new SgAsmDirectRegisterExpression(desc);
    expr->set_type(makeType(m68k_fmt_i16));
    return expr;
}

SgAsmRegisterReferenceExpression *
DisassemblerM68k::makeConditionCodeRegister()
{
    RegisterDescriptor desc(m68k_regclass_spr, m68k_spr_sr, 0, 8); // CCR is the low byte of the status register
    SgAsmRegisterReferenceExpression *expr = new SgAsmDirectRegisterExpression(desc);
    expr->set_type(makeType(m68k_fmt_i8));
    return expr;
}

SgAsmRegisterReferenceExpression *
DisassemblerM68k::makeColdFireControlRegister(unsigned regnum) {
    // From table 8.3: "Coldfire CPU Space Assignments" in "CFPRM ColdFire Family Programmer's Reference Manual"
    // for the privileged MOVEC instruction.
    RegisterDescriptor rd;
    switch (regnum) {
        // Memory management control registers
        case 0x002:                                     // cache control register (CACR)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_cacr, 0, 32);
            break;
        case 0x003:                                     // address space identifier register (ASID)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_asid, 0, 32);
            break;
        case 0x004:                                     // access control registers 0 (ACR0)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_acr0, 0, 32);
            break;
        case 0x005:                                     // access control registers 1 (ACR1)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_acr1, 0, 32);
            break;
        case 0x006:                                     // access control registers 2 (ACR2)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_acr2, 0, 32);
            break;
        case 0x007:                                     // access control registers 3 (ACR3)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_acr3, 0, 32);
            break;
        case 0x008:                                     // mmu base address register (MMUBAR)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_mmubar, 0, 32);
            break;

        // Processor miscellaneous registers
        case 0x801:                                     // vector base register (VBR)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_vbr, 0, 32);
            break;
        case 0x80f:                                     // program counter (PC)
            return makeProgramCounter();

        // Local memory and module control registers
        case 0xc00:                                     // ROM base address register 0 (ROMBAR0)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_rombar0, 0, 32);
            break;
        case 0xc01:                                     // ROM base address register 1 (ROMBAR1)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_rombar1, 0, 32);
            break;
        case 0xc04:                                     // RAM base address register 0 (RAMBAR0)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_rambar0, 0, 32);
            break;
        case 0xc05:                                     // RAM base address register 1( RAMBAR1)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_rambar1, 0, 32);
            break;
        case 0xc0c:                                     // multiprocessor control register (MPCR)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_mpcr, 0, 32);
            break;
        case 0xc0d:                                     // embedded DRAM base address register (EDRAMBAR)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_edrambar, 0, 32);
            break;
        case 0xc0e:                                     // secondary module base address register (SECMBAR)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_secmbar, 0, 32);
            break;
        case 0xc0f:                                     // primary module base address register (MBAR)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_mbar, 0, 32);
            break;

        // Local memory address permutation control registers
        case 0xd02:                                     // 32 msbs of RAM 0 permutation control register 1 (PCR1U0)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_0_pcr1, 32, 32);
            break;
        case 0xd03:                                     // 32 lsbs of RAM 0 permutation control register 1 (PCR1L0)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_0_pcr1, 0, 32);
            break;
        case 0xd04:                                     // 32 msbs of RAM 0 permutation control register 2 (PCR2U0)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_0_pcr2, 32, 32);
            break;
        case 0xd05:                                     // 32 lsbs of RAM 0 permutation control register 2 (PCR2L0)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_0_pcr2, 0, 32);
            break;
        case 0xd06:                                     // 32 msbs of RAM 0 permutation control register 3 (PCR3U0)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_0_pcr3, 32, 32);
            break;
        case 0xd07:                                     // 32 lsbs of RAM 0 permutation control register 3 (PCR3L0)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_0_pcr3, 0, 32);
            break;
        case 0xd0a:                                     // 32 msbs of RAM 1 permutation control register 1 (PCR1U1)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_1_pcr1, 32, 32);
            break;
        case 0xd0b:                                     // 32 lsbs of RAM 1 permutation control register 1 (PCR1L1)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_1_pcr1, 0, 32);
            break;
        case 0xd0c:                                     // 32 msbs of RAM 1 permutation control register 2 (PCR2U1)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_1_pcr2, 32, 32);
            break;
        case 0xd0d:                                     // 32 lsbs of RAM 1 permutation control register 2 (PCR2L1)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_1_pcr2, 0, 32);
            break;
        case 0xd0e:                                     // 32 msbs of RAM 1 permutation control register 3 (PCR3U1)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_1_pcr3, 32, 32);
            break;
        case 0xd0f:                                     // 32 lsbs of RAM 1 permutation control register 3 (PCR3L1)
            rd = RegisterDescriptor(m68k_regclass_sup, m68k_sup_1_pcr3, 0, 32);
            break;

        default:
            // "Not all control registers are implemented in every processor design. Attempted access to undefined or
            // unimplemented control register space produces undefined results." [Programmer's Reference Manual].
            throw Disassembler::Exception("invalid control register number: " +
                                          StringUtility::toHex2(regnum, 12, false, false));
    }
    SgAsmRegisterReferenceExpression *expr = new SgAsmDirectRegisterExpression(rd);
    ASSERT_require(rd.nBits() == 32);
    expr->set_type(makeType(m68k_fmt_i32));
    return expr;
}

SgAsmRegisterReferenceExpression *
DisassemblerM68k::makeProgramCounter()
{
    RegisterDescriptor desc(m68k_regclass_spr, m68k_spr_pc, 0, 32);
    SgAsmRegisterReferenceExpression *expr = new SgAsmDirectRegisterExpression(desc);
    expr->set_type(makeType(m68k_fmt_i32));
    return expr;
}

SgAsmRegisterReferenceExpression *
DisassemblerM68k::makeMacRegister(M68kMacRegister minor)
{
    RegisterDescriptor desc(m68k_regclass_mac, minor, 0, 32);
    SgAsmRegisterReferenceExpression *expr = new SgAsmDirectRegisterExpression(desc);
    expr->set_type(makeType(m68k_fmt_i32));
    return expr;
}

SgAsmRegisterReferenceExpression *
DisassemblerM68k::makeMacAccumulatorRegister(unsigned accumIndex)
{
    ASSERT_require(accumIndex < 4);
    RegisterDescriptor desc(m68k_regclass_mac, m68k_mac_acc0+accumIndex, 0, 32);
    SgAsmRegisterReferenceExpression *expr = new SgAsmDirectRegisterExpression(desc);
    expr->set_type(makeType(m68k_fmt_i32));
    return expr;
}

SgAsmRegisterReferenceExpression *
DisassemblerM68k::makeFPRegister(unsigned regnum)
{
    M68kDataFormat fmt = floatingFormatForFamily(family);
    SgAsmType *type = NULL;
    switch (fmt) {
        case m68k_fmt_f64:
            type = makeType(fmt);
            break;
        case m68k_fmt_f96:
            // The Motorola 68xxx 96-bit "extended real" type has 16 bits of zeros smack in the middle. The floating-point
            // registers don't store the zeros and are therefore only 80 bits wide.
            type = SageBuilderAsm::buildTypeM68kFloat80();
            break;
        default:
            ASSERT_not_reachable("invalid default floating-point format: " + stringifyBinaryAnalysisM68kDataFormat(fmt));
    }
    RegisterDescriptor desc(m68k_regclass_fpr, regnum, 0, type->get_nBits());
    SgAsmRegisterReferenceExpression *expr = new SgAsmDirectRegisterExpression(desc);
    expr->set_type(type);
    return expr;
}

SgAsmIntegerValueExpression *
DisassemblerM68k::makeImmediateValue(M68kDataFormat fmt, unsigned value)
{
    return SageBuilderAsm::buildValueInteger(value, makeType(fmt));
}

SgAsmIntegerValueExpression *
DisassemblerM68k::makeImmediateExtension(M68kDataFormat fmt, size_t ext_word_number)
{
    size_t nBits = formatNBits(fmt);
    ASSERT_require(nBits > 0);
    if (8==nBits)
        return SageBuilderAsm::buildValueInteger(instructionWord(ext_word_number+1) & 0xff, makeType(fmt));
        
    ASSERT_require(nBits % 16 == 0);
    size_t nWords = nBits / 16;
    Sawyer::Container::BitVector bv(nBits);
    for (size_t i=0; i<nWords; ++i) {                   // values are stored from high to low word; each word big endian
        uint16_t word = instructionWord(ext_word_number+i+1);// +1 for the opcode word itself
        size_t bitOffset = (nWords-(i+1))*16;
        bv.fromInteger(bv.baseSize(bitOffset, 16), word);
    }
    return SageBuilderAsm::buildValueInteger(bv, makeType(fmt));
}
            
// The modreg should be 6 bits: upper three bits are the mode, lower three bits are usually a register number.
SgAsmExpression *
DisassemblerM68k::makeEffectiveAddress(unsigned modreg, M68kDataFormat fmt, size_t ext_offset)
{
    ASSERT_require2(0 == (modreg & ~0x3f), "modreg should be 6 bits wide; got " + addrToString(modreg));
    unsigned mode = (modreg >> 3) & 7;
    unsigned reg  = modreg & 7;
    return makeEffectiveAddress(mode, reg, fmt, ext_offset);
}

SgAsmExpression *
DisassemblerM68k::makeEffectiveAddress(unsigned mode, unsigned reg, M68kDataFormat fmt, size_t ext_offset)
{
    ASSERT_require(mode < 8);
    ASSERT_require(reg < 8);

    SgAsmType *type = makeType(fmt);

    if (0==mode) {
        // m68k_eam_drd: data register direct
        return makeDataRegister(reg, fmt);
    } else if (1==mode) {
        // m68k_eam_ard: address register direct
        return makeAddressRegister(reg, fmt);
    } else if (2==mode) {
        // m68k_eam_ari: address register indirect
        SgAsmRegisterReferenceExpression *rre = makeAddressRegister(reg, m68k_fmt_i32);
        return SageBuilderAsm::buildMemoryReferenceExpression(rre, NULL/*segment*/, type);
    } else if (3==mode) {
        // m68k_eam_inc: address register indirect with post increment
        return makeAddressRegisterPostIncrement(reg, fmt);
    } else if (4==mode) {
        // m68k_eam_dec: address register indirect with pre decrement
        return makeAddressRegisterPreDecrement(reg, fmt);
    } else if (5==mode) {
        // m68k_eam_dsp: address register indirect with displacement
        SgAsmRegisterReferenceExpression *rre = makeAddressRegister(reg, m68k_fmt_i32);
        uint64_t displacement_n = signExtend<16, 32>((uint64_t)instructionWord(ext_offset+1));
        SgAsmIntegerValueExpression *displacement = SageBuilderAsm::buildValueInteger(displacement_n, makeType(m68k_fmt_i32));
        SgAsmExpression *address = SageBuilderAsm::buildAddExpression(rre, displacement);
        address->set_type(makeType(m68k_fmt_i32));
        return SageBuilderAsm::buildMemoryReferenceExpression(address, NULL/*segment*/, type);
    } else if (6==mode) {
        if (family & (m68k_generation_2 | m68k_generation_3)) {
            // m68k_eam_idxbd: address register indirect with index (base displacement)
            throw Exception("FIXME[Robb P. Matzke 2014-07-14]: m68k_eam_idxbd not implemented yet");
        } else {
            // m68k_eam_idx8: address register indirect with index (8-bit displacement)
            // Uses the brief extension word format (1 word having D/A, Register, W/L, Scale, and 8-bit displacement)
            if (0!=extract<8, 8>(instructionWord(ext_offset+1)))
                throw Exception("m68k_eam_idx8 mode requires that bit 8 of the extension word is clear");
            if (extract<11, 11>(instructionWord(ext_offset+1))) {
                // 32-bit index
                SgAsmExpression *address = makeAddressRegister(reg, m68k_fmt_i32);
                uint32_t disp = signExtend<8, 32>((uint32_t)extract<0, 7>(instructionWord(ext_offset+1)));
                SgAsmIntegerValueExpression *dispExpr = SageBuilderAsm::buildValueInteger(disp, makeType(m68k_fmt_i32));
                address = SageBuilderAsm::buildAddExpression(address, dispExpr);
                address->set_type(makeType(m68k_fmt_i32));

                unsigned indexRegisterNumber = extract<12, 15>(instructionWord(ext_offset+1));
                SgAsmRegisterReferenceExpression *indexRRE = makeDataAddressRegister(indexRegisterNumber, m68k_fmt_i32);
                uint32_t scale = IntegerOps::shl1<uint32_t>(extract<9, 10>(instructionWord(ext_offset+1)));
                SgAsmIntegerValueExpression *scaleExpr = SageBuilderAsm::buildValueInteger(scale, makeType(m68k_fmt_i32));
                SgAsmExpression *product = SageBuilderAsm::buildMultiplyExpression(indexRRE, scaleExpr);
                product->set_type(makeType(m68k_fmt_i32));
                address = SageBuilderAsm::buildAddExpression(address, product);
                address->set_type(makeType(m68k_fmt_i32));
                return SageBuilderAsm::buildMemoryReferenceExpression(address, NULL /*segment*/, type);
            } else {
                throw Exception("FIXME[Robb P. Matzke 2014-07-14]: word index register not implemented yet");
            }
        }
    } else if (7==mode && 0==reg) {
        // m68k_eam_absw: absolute short addressing mode
        uint64_t val = signExtend<16, 32>((uint64_t)instructionWord(ext_offset+1));
        SgAsmIntegerValueExpression *address = SageBuilderAsm::buildValueInteger(val, makeType(m68k_fmt_i32));
        return SageBuilderAsm::buildMemoryReferenceExpression(address, NULL/*segment*/, type);
    } else if (7==mode && 1==reg) {
        // m68k_eam_absl: absolute long addressing mode
        uint64_t val = shiftLeft<32>((uint64_t)instructionWord(ext_offset+1), 16) | (uint64_t)instructionWord(ext_offset+2);
        SgAsmIntegerValueExpression *address = SageBuilderAsm::buildValueInteger(val, makeType(m68k_fmt_i32));
        return SageBuilderAsm::buildMemoryReferenceExpression(address, NULL/*segment*/, type);
    } else if (7==mode && 2==reg) {
        // m68k_eam_pcdsp: program counter indirect with displacement
        SgAsmRegisterReferenceExpression *rre = makeProgramCounter();
        uint64_t displacement_n = signExtend<16, 32>((uint64_t)instructionWord(ext_offset+1));
        SgAsmIntegerValueExpression *displacement = SageBuilderAsm::buildValueInteger(displacement_n, makeType(m68k_fmt_i32));
        SgAsmExpression *address = SageBuilderAsm::buildAddExpression(rre, displacement);
        address->set_type(makeType(m68k_fmt_i32));
        return SageBuilderAsm::buildMemoryReferenceExpression(address, NULL/*segment*/, type);
    } else if (7==mode && 3==reg) {
        if (0==extract<8, 8>(instructionWord(ext_offset+1))) { // brief extension word format (8-bit displacement)
            unsigned indexRegisterNumber = extract<12, 15>(instructionWord(ext_offset+1));
            SgAsmRegisterReferenceExpression *indexRRE = extract<11, 11>(instructionWord(ext_offset+1)) ?
                                                         makeDataAddressRegister(indexRegisterNumber, m68k_fmt_i32, 0) :
                                                         makeDataAddressRegister(indexRegisterNumber, m68k_fmt_i16, 0);
            uint32_t scale = IntegerOps::shl1<uint32_t>(extract<9, 10>(instructionWord(ext_offset+1)));
            SgAsmIntegerValueExpression *scaleExpr = SageBuilderAsm::buildValueInteger(scale, makeType(m68k_fmt_i32));
            SgAsmExpression *product = SageBuilderAsm::buildMultiplyExpression(indexRRE, scaleExpr);
            product->set_type(makeType(m68k_fmt_i32));

            uint32_t disp = signExtend<8, 32>((uint32_t)extract<0, 7>(instructionWord(ext_offset+1)));
            SgAsmIntegerValueExpression *dispExpr = SageBuilderAsm::buildValueInteger(disp, makeType(m68k_fmt_i32));

            SgAsmExpression *address = makeProgramCounter();
            address = SageBuilderAsm::buildAddExpression(address, dispExpr);
            address->set_type(makeType(m68k_fmt_i32));
            address = SageBuilderAsm::buildAddExpression(address, product);
            address->set_type(makeType(m68k_fmt_i32));
            return SageBuilderAsm::buildMemoryReferenceExpression(address, NULL /*segment*/, type);
        } else {
            // m68k_eam_pcidxbd: program counter indirect with index (base displacement)
            // m68k_eam_pcmpost: program counter memory indirect postindexed
            // m68k_eam_pcmpre: program counter memory indirect preindexed
            throw Exception("FIXME[Robb P. Matzke 2014-06-19]: cannot distinguish between m68k_eam_{pcidxbd,pcmpost,pcmpre}");
        }
    } else if (7==mode && 4==reg) {
        // m68k_eam_imm: immediate data (1, 2, 4, or 6 extension words)
        return makeImmediateExtension(fmt, ext_offset);
    } else {
        throw Exception(std::string("invalid effective address mode:") +
                        " mode=" + numberToString(mode) + " reg="  + numberToString(reg));
    }
}

SgAsmExpression *
DisassemblerM68k::makeAddress(SgAsmExpression *expr)
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
            lhs->get_descriptor().majorNumber()==m68k_regclass_spr && lhs->get_descriptor().minorNumber()==m68k_spr_pc) {
            retval = SageBuilderAsm::buildValueInteger(get_insn_va() + 2 + rhs->get_absoluteValue(), makeType(m68k_fmt_i32));
        }
    }

    return retval;
}

DisassemblerM68k::ExpressionPair
DisassemblerM68k::makeOffsetWidthPair(unsigned w1)
{
    SgAsmExpression *offset=NULL, *width=NULL;
    if (extract<11, 11>(w1)) {
        if (extract<9, 11>(w1))
            throw Exception("bits 9 and 10 of extension word 1 should be zero when bit 11 is set");
        offset = makeDataRegister(extract<6, 8>(w1), m68k_fmt_i32);
    } else {
        offset = makeImmediateValue(m68k_fmt_i8, extract<6, 10>(w1));
    }
    if (extract<5, 5>(w1)) {
        if (extract<3, 4>(w1))
            throw Exception("bits 3 and 4 of extension word 1 should be zero when bit 5 is set");
        width = makeDataRegister(extract<0, 2>(w1), m68k_fmt_i32);
    } else {
        width = makeImmediateValue(m68k_fmt_i8, extract<0, 4>(w1));
    }
    return ExpressionPair(offset, width);
}

SgAsmInstruction *
DisassemblerM68k::makeUnknownInstruction(const Disassembler::Exception &e)
{
    SgAsmM68kInstruction *insn = new SgAsmM68kInstruction(get_insn_va(), "unknown", m68k_unknown_instruction);
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

SgAsmM68kInstruction *
DisassemblerM68k::makeInstruction(M68kInstructionKind kind, const std::string &mnemonic,
                                  SgAsmExpression *op1, SgAsmExpression *op2, SgAsmExpression *op3,
                                  SgAsmExpression *op4, SgAsmExpression *op5, SgAsmExpression *op6,
                                  SgAsmExpression *op7)
{
    ASSERT_forbid2(m68k_unknown_instruction==kind, "should have called make_unknown_instruction instead");
    SgAsmM68kInstruction *insn = new SgAsmM68kInstruction(get_insn_va(), mnemonic, kind);

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
DisassemblerM68k::instructionWord(size_t n)
{
    if (n>10)
        throw Exception("malformed instruction uses more than 11 extension words");
    if (n>=niwords)
        throw Exception("short read for instruction word " + numberToString(n));
    niwords_used = std::max(niwords_used, n+1);
    return iwords[n];
}

size_t
DisassemblerM68k::extensionWordsUsed() const
{
    return niwords_used==0 ? 0 : niwords_used-1;
}

// see base class
SgAsmInstruction *
DisassemblerM68k::disassembleOne(const MemoryMap::Ptr &map, rose_addr_t start_va, AddressSet *successors)
{
    start_instruction(map, start_va);
    if (0!=start_va%2)
        throw Exception("instruction is not properly aligned", start_va);
    uint8_t buf[sizeof(iwords)]; // largest possible instruction
    size_t nbytes = map->at(start_va).limit(sizeof buf).require(MemoryMap::EXECUTABLE).read(buf).size();
    niwords = nbytes / sizeof(iwords[0]);
    if (0==niwords)
        throw Exception("short read from memory map", start_va);
    for (size_t i=0; i<niwords; ++i)
        iwords[i] = ByteOrder::be_to_host(*(uint16_t*)(buf+2*i));
    niwords_used = 1;

    SgAsmM68kInstruction *insn = NULL;
    if (M68k *idis = find_idis(iwords, niwords))
        insn = (*idis)(this, instructionWord(0));
    if (!insn)
        throw Exception("cannot disassemble m68k instruction: "+addrToString(instructionWord(0), 16), start_va);

    ASSERT_require(niwords_used>0);
    SgUnsignedCharList raw_bytes(buf+0, buf+2*niwords_used);
    insn->set_raw_bytes(raw_bytes);

    if (successors) {
        bool complete;
        AddressSet suc2 = insn->getSuccessors(&complete);
        successors->insert(suc2.begin(), suc2.end());
    }

    return insn;
}

void
DisassemblerM68k::insert_idis(M68k *idis)
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
            std::cerr <<"DisassemblerM68k::insert_idis: insertion key ";
            idis->pattern.print(std::cerr, alternatives.first);
            std::cerr <<" for '" <<idis->name <<"' conflicts with existing key ";
            (*ti)->pattern.print(std::cerr, alternatives.second);
            std::cerr <<" for '" <<(*ti)->name <<"'\n";
            throw Exception("insert_idis() would cause an ambiguity in the M68k instruction disassembly table");
        }
    }

    // Insert the new pattern in the list which is sorted by descending number of significant bits.
    IdisList::iterator ti = idis_table[idisIdx].begin();
    while (ti!=idis_table[idisIdx].end() && (*ti)->pattern.nsignificant()>=idis->pattern.nsignificant()) ++ti;
    idis_table[idisIdx].insert(ti, idis);
}

DisassemblerM68k::M68k *
DisassemblerM68k::find_idis(uint16_t *insn_bytes, size_t nbytes) const
{
    if (nbytes==0)
        return NULL;

    // First search the table based on the operator bits (high order nybble of the first opword)
    size_t idisIdx = (insn_bytes[0] >> 12) & 0xf;
    for (IdisList::const_iterator ti=idis_table[idisIdx].begin(); ti!=idis_table[idisIdx].end(); ++ti) {
        if ((*ti)->pattern.matches(insn_bytes, nbytes))
            return *ti;
    }

    // If we didn't find it, search table entry 16, the catch-all slot for instructions whose operator byte is not invariant.
    idisIdx = 16;
    for (IdisList::const_iterator ti=idis_table[idisIdx].begin(); ti!=idis_table[idisIdx].end(); ++ti) {
        if ((*ti)->pattern.matches(insn_bytes, nbytes))
            return *ti;
    }

    return NULL;
}

/*******************************************************************************************************************************
 *                                      Integer instructions
 *******************************************************************************************************************************/

typedef DisassemblerM68k::M68k M68k;

// ABCD.B Dy, Dx
struct M68k_abcd_1: M68k {
    M68k_abcd_1(): M68k("abcd_1", m68k_family,
                        OP(12) & BITS<3, 8>(0x20)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<0, 2>(w0), m68k_fmt_i8);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i8);
        return d->makeInstruction(m68k_abcd, "abcd."+formatLetter(m68k_fmt_i8), src, dst);
    }
};

// ABCD.B -(Ay), -(Ax)
struct M68k_abcd_2: M68k {
    M68k_abcd_2(): M68k("abcd_2", m68k_family,
                        OP(12) & BITS<3, 8>(0x21)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeAddressRegisterPreDecrement(extract<0, 2>(w0), m68k_fmt_i8);
        SgAsmExpression *dst = d->makeAddressRegisterPreDecrement(extract<9, 11>(w0), m68k_fmt_i8);
        return d->makeInstruction(m68k_abcd, "abcd."+formatLetter(m68k_fmt_i8), src, dst);
    }
};

// ADD.B <ea>y, Dx
struct M68k_add_1: M68k {
    M68k_add_1(): M68k("add_1", m68k_family,
                       OP(13) & BIT<8>(0) & BITS<6, 7>(0) & EAM(m68k_eam_all & ~m68k_eam_ard)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), fmt);
        return d->makeInstruction(m68k_add, "add."+formatLetter(fmt), src, dst);
    }
};

// ADD.W <ea>y, Dx
// ADD.L <ea>y, Dx
struct M68k_add_2: M68k {
    M68k_add_2(): M68k("add_2", m68k_family,
                       OP(13) & BIT<8>(0) & (BITS<6, 7>(1) | BITS<6, 7>(2)) & EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), fmt);
        return d->makeInstruction(m68k_add, "add."+formatLetter(fmt), src, dst);
    }
};

// ADD.B Dy, <ea>x
// ADD.W Dy, <ea>x
// ADD.L Dy, <ea>x
struct M68k_add_3: M68k {
    M68k_add_3(): M68k("add_3", m68k_family,
                       OP(13) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                       EAM(m68k_eam_memory & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), fmt);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(m68k_add, "add."+formatLetter(fmt), src, dst);
    }
};

// ADDA.W <ea>y, Ax
// ADDA.L <ea>y, Ax
struct M68k_adda: M68k {
    M68k_adda(): M68k("adda", m68k_family,
                      OP(13) & (BITS<6, 8>(3) | BITS<6, 8>(7)) & EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = m68k_fmt_unknown;
        switch (extract<6, 8>(w0)) {
            case 3: fmt = m68k_fmt_i16; break;
            case 7: fmt = m68k_fmt_i32; break;
        }
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeAddressRegister(extract<9, 11>(w0), m68k_fmt_i32);
        return d->makeInstruction(m68k_adda, "adda."+formatLetter(fmt), src, dst);
    }
};

// ADDI.B #<data>, <ea>
// ADDI.W #<data>, <ea>
// ADDI.L #<data>, <ea>
struct M68k_addi: M68k {
    M68k_addi(): M68k("addi", m68k_family,
                      OP(0) & BITS<8, 11>(0x6) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        size_t opandWords = (formatNBits(fmt)+15) / 16;
        SgAsmExpression *src = d->makeImmediateExtension(fmt, 0); // immediate argument is in extension word(s)
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, opandWords);
        return d->makeInstruction(m68k_addi, "addi."+formatLetter(fmt), src, dst);
    }
};

// ADDQ.B #<data>, <ea>x
// ADDQ.W #<data>, <ea>x
// ADDQ.L #<data>, <ea>x
//
// Note: Reference manual text says all "alterable modes" allowed, but the table lacks program counter memory indirect modes.
// I am favoring the table over the text. [Robb P. Matzke 2013-10-07]
struct M68k_addq: M68k {
    M68k_addq(): M68k("addq", m68k_family,
                      OP(5) & BIT<8>(0) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        unsigned imm = extract<9, 11>(w0);
        if (0==imm)
            imm = 8;
        SgAsmExpression *src = d->makeImmediateValue(fmt, imm);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(m68k_addq, "addq."+formatLetter(fmt), src, dst);
    }
};

// ADDX.B Dy, Dx
// ADDX.W Dy, Dx
// ADDX.L Dy, Dx
struct M68k_addx_1: M68k {
    M68k_addx_1(): M68k("addx_1", m68k_family,
                        OP(13) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 5>(0)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeDataRegister(extract<0, 2>(w0), fmt);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), fmt);
        return d->makeInstruction(m68k_addx, "addx."+formatLetter(fmt), src, dst);
    }
};

// ADDX.B -(Ay), -(Ax)
// ADDX.W -(Ay), -(Ax)
// ADDX.L -(Ay), -(Ax)
struct M68k_addx_2: M68k {
    M68k_addx_2(): M68k("addx_1", m68k_family,
                        OP(13) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 5>(1)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeAddressRegisterPreDecrement(extract<0, 2>(w0), fmt);
        SgAsmExpression *dst = d->makeAddressRegisterPreDecrement(extract<9, 11>(w0), fmt);
        return d->makeInstruction(m68k_addx, "addx."+formatLetter(fmt), src, dst);

    }
};

// AND.B <ea>y, Dx
// AND.W <ea>y, Dx
// AND.L <ea>y, Dx
struct M68k_and_1: M68k {
    M68k_and_1(): M68k("and_1", m68k_family,
                       OP(12) & BIT<8>(0) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & EAM(m68k_eam_data)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), fmt);
        return d->makeInstruction(m68k_and, "and."+formatLetter(fmt), src, dst);
    }
};

// AND.B Dy, <ea>x
// AND.W Dy, <ea>x
// AND.L Dy, <ea>x
//
// Note: Reference manual text says "memory alterable modes" are allowed, but the table lacks program counter memory indirect
// modes.  I am favoring the table over the text. [Robb P. Matzke 2013-10-07]
struct M68k_and_2: M68k {
    M68k_and_2(): M68k("and_2", m68k_family,
                       OP(12) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                       EAM(m68k_eam_memory & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), fmt);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(m68k_and, "and."+formatLetter(fmt), src, dst);
    }
};

// ANDI.B #<data>, <ea>
// ANDI.W #<data>, <ea>
// ANDI.L #<data>, <ea>
struct M68k_andi: M68k {
    M68k_andi(): M68k("andi", m68k_family,
                      OP(0) & BITS<8, 12>(2) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        size_t opandWords = (formatNBits(fmt)+15) / 16;
        SgAsmExpression *src = d->makeImmediateExtension(fmt, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, opandWords);
        return d->makeInstruction(m68k_andi, "andi."+formatLetter(fmt), src, dst);
    }
};

// ANDI.B #<data>, CCR
struct M68k_andi_to_ccr: M68k {
    M68k_andi_to_ccr(): M68k("andi_to_ccr", m68k_family,
                             OP(0) & BITS<0, 11>(0x23c) & INSN_WORD<1>(BITS<8, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = m68k_fmt_i8;
        SgAsmExpression *src = d->makeImmediateValue(fmt, extract<0, 7>(d->instructionWord(1)));
        SgAsmExpression *dst = d->makeConditionCodeRegister();
        return d->makeInstruction(m68k_andi, "andi."+formatLetter(fmt), src, dst);
    }
};

// ASL.B Dy, Dx         (Dy is the shift count)
// ASL.W Dy, Dx
// ASL.L Dy, Dx
// ASR.B Dy, Dx
// ASR.W Dy, Dx
// ASR.L Dy, Dx
struct M68k_ashift_1: M68k {
    M68k_ashift_1(): M68k("ashift_1", m68k_family,
                          OP(14) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 5>(4)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        bool shift_left = extract<8, 8>(w0) ? true : false;
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i32); // full register, mod 64
        SgAsmExpression *dst = d->makeDataRegister(extract<0, 2>(w0), fmt);
        return d->makeInstruction(shift_left ? m68k_asl : m68k_asr,
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
struct M68k_ashift_2: M68k {
    M68k_ashift_2(): M68k("ashift_2", m68k_family,
                          OP(14) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 5>(0)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        bool shift_left = extract<8, 8>(w0) ? true : false;
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        unsigned count = extract<9, 11>(w0);
        if (0==count)
            count = 8;
        SgAsmExpression *src = d->makeImmediateValue(m68k_fmt_i8, count);
        SgAsmExpression *dst = d->makeDataRegister(extract<0, 2>(w0), fmt);
        return d->makeInstruction(shift_left ? m68k_asl : m68k_asr,
                                  (shift_left ? "asl." : "asr.")+formatLetter(fmt),
                                  src, dst);
    }
};

// ASL.W <ea>x
// ASR.W <ea>x
//
// Note: Reference manual text says "memory alterable modes" are allowed, but the table lacks program counter memory indirect
// modes.  I am favoring the table over the text. [Robb P. Matzke 2013-10-07]
struct M68k_ashift_3: M68k {
    M68k_ashift_3(): M68k("ashift_3", m68k_family,
                          OP(14) & BITS<9, 11>(0) & BITS<6, 7>(3) &
                          EAM(m68k_eam_memory & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        bool shift_left = extract<8, 8>(w0) ? true : false;
        M68kDataFormat fmt = m68k_fmt_i16;
        // src is implied #<1>
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(shift_left ? m68k_asl : m68k_asr,
                                  (shift_left ? "asl." : "asr.")+formatLetter(fmt),
                                  dst);
    }
};

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
struct M68k_branch: M68k {
    M68k_branch(): M68k("branch", m68k_family, OP(6)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        unsigned cc = extract<8, 11>(w0);
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (cc) {
            case  0: kind = m68k_bra; break;
            case  1: kind = m68k_bsr; break;
            case  2: kind = m68k_bhi; break;
            case  3: kind = m68k_bls; break;
            case  4: kind = m68k_bcc; break;
            case  5: kind = m68k_bcs; break;
            case  6: kind = m68k_bne; break;
            case  7: kind = m68k_beq; break;
            case  8: kind = m68k_bvc; break;
            case  9: kind = m68k_bvs; break;
            case 10: kind = m68k_bpl; break;
            case 11: kind = m68k_bmi; break;
            case 12: kind = m68k_bge; break;
            case 13: kind = m68k_blt; break;
            case 14: kind = m68k_bgt; break;
            case 15: kind = m68k_ble; break;
        }
        std::string mnemonic = stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_");
        rose_addr_t base = d->get_insn_va() + 2;
        int32_t offset = signExtend<8, 32>(extract<0, 7>(w0));
        if (0==offset) {
            offset = signExtend<16, 32>((uint32_t)d->instructionWord(1));
            mnemonic += ".w";
        } else if (-1==offset) {
            offset = shiftLeft<32>((uint32_t)d->instructionWord(1), 16) | (uint32_t)d->instructionWord(2);
            mnemonic += ".l";
        } else {
            mnemonic += ".b";
        }
        rose_addr_t target_va = (base + offset) & GenMask<rose_addr_t, 32>::value;
        SgAsmIntegerValueExpression *target = d->makeImmediateValue(m68k_fmt_i32, target_va);
        return d->makeInstruction(kind, mnemonic, target);
    }
};

// BCHG.L #<bitnum>, <ea>x
struct M68k_bchg_1: M68k {
    M68k_bchg_1(): M68k("bchg_1", m68k_family,
                        OP(0) & BITS<6, 11>(0x21) & EAM(m68k_eam_drd) & INSN_WORD<1>(BITS<8, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(m68k_fmt_i8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 1);
        return d->makeInstruction(m68k_bchg, "bchg.l", src, dst);
    }
};

// BCHG.B #<bitnum>, <ea>x
struct M68k_bchg_2: M68k {
    M68k_bchg_2(): M68k("bchg_2", m68k_family,
                        OP(0) & BITS<6, 11>(0x21) & EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_drd & ~m68k_eam_pcmi) &
                        INSN_WORD<1>(BITS<8, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(m68k_fmt_i8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i8, 1);
        return d->makeInstruction(m68k_bchg, "bchg.b", src, dst);
    }
};

// BCHG.L Dy, <ea>x
struct M68k_bchg_3: M68k {
    M68k_bchg_3(): M68k("bchg_3", m68k_family,
                        OP(0) & BITS<6, 8>(5) & EAM(m68k_eam_drd)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 0);
        return d->makeInstruction(m68k_bchg, "bchg.l", src, dst);
    }
};

// BCHG.B Dy, <ea>x
struct M68k_bchg_4: M68k {
    M68k_bchg_4(): M68k("bchg_4", m68k_family,
                        OP(0) & BITS<6, 8>(5) & EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_drd & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i8, 0);
        return d->makeInstruction(m68k_bchg, "bchg.b", src, dst);
    }
};

// BCLR.L #<bitnum>, <ea>x
struct M68k_bclr_1: M68k {
    M68k_bclr_1(): M68k("bclr_1", m68k_family,
                        OP(0) & BITS<6, 11>(0x22) & EAM(m68k_eam_drd) & INSN_WORD<1>(BITS<8, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(m68k_fmt_i8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 1);
        return d->makeInstruction(m68k_bclr, "bclr.l", src, dst);
    }
};

// BCLR.B #<bitnum>, <ea>x
struct M68k_bclr_2: M68k {
    M68k_bclr_2(): M68k("bclr_2", m68k_family,
                        OP(0) & BITS<6, 11>(0x22) & EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_drd & ~m68k_eam_pcmi) &
                        INSN_WORD<1>(BITS<8, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(m68k_fmt_i8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i8, 1);
        return d->makeInstruction(m68k_bclr, "bclr.b", src, dst);
    }
};

// BCLR.L Dy, <ea>x
struct M68k_bclr_3: M68k {
    M68k_bclr_3(): M68k("bclr_3", m68k_family,
                        OP(0) & BITS<6, 8>(6) & EAM(m68k_eam_drd)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 0);
        return d->makeInstruction(m68k_bclr, "bclr.l", src, dst);
    }
};

// BCLR.B Dy, <ea>x
struct M68k_bclr_4: M68k {
    M68k_bclr_4(): M68k("bclr_4", m68k_family,
                        OP(0) & BITS<6, 8>(6) & EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_drd & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i8, 0);
        return d->makeInstruction(m68k_bclr, "bclr.b", src, dst);
    }
};

// BFCHG <ea>x {offset:width}
struct M68k_bfchg: M68k {
    M68k_bfchg(): M68k("bfchg", m68k_68020|m68k_68030|m68k_68040,
                       OP(0xe) & BITS<6, 11>(0x2b) & EAM(m68k_eam_drd | (m68k_eam_control & m68k_eam_alter & ~m68k_eam_pcmi)) &
                       INSN_WORD<1>(BITS<12, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        DisassemblerM68k::ExpressionPair offset_width = d->makeOffsetWidthPair(d->instructionWord(1));
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 1);
        return d->makeInstruction(m68k_bfchg, "bfchg", dst, offset_width.first, offset_width.second);
    }
};

// BFCLR <ea>x {offset:width}
struct M68k_bfclr: M68k {
    M68k_bfclr(): M68k("bfclr", m68k_68020|m68k_68030|m68k_68040,
                       OP(0xe) & BITS<6, 11>(0x33) & EAM(m68k_eam_drd | (m68k_eam_control & m68k_eam_alter & ~m68k_eam_pcmi)) &
                       INSN_WORD<1>(BITS<12, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        DisassemblerM68k::ExpressionPair offset_width = d->makeOffsetWidthPair(d->instructionWord(1));
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 1);
        return d->makeInstruction(m68k_bfclr, "bfclr", dst, offset_width.first, offset_width.second);
    }
};

// BFEXTS <ea>y {offset:width}, Dx
struct M68k_bfexts: M68k {
    M68k_bfexts(): M68k("bfexts", m68k_68020|m68k_68030|m68k_68040,
                        OP(0xe) & BITS<6, 11>(0x2f) & EAM(m68k_eam_drd | m68k_eam_control) &
                        INSN_WORD<1>(BIT<15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        DisassemblerM68k::ExpressionPair offset_width = d->makeOffsetWidthPair(d->instructionWord(1));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 1);
        SgAsmExpression *dst = d->makeDataRegister(extract<12, 14>(d->instructionWord(1)), m68k_fmt_i32);
        return d->makeInstruction(m68k_bfexts, "bfexts", src, offset_width.first, offset_width.second, dst);
    }
};

// BFEXTU <ea>y {offset:width}, Dx
struct M68k_bfextu: M68k {
    M68k_bfextu(): M68k("bfextu", m68k_68020|m68k_68030|m68k_68040,
                        OP(0xe) & BITS<6, 11>(0x27) & EAM(m68k_eam_drd | m68k_eam_control) &
                        INSN_WORD<1>(BIT<15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        DisassemblerM68k::ExpressionPair offset_width = d->makeOffsetWidthPair(d->instructionWord(1));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 1);
        SgAsmExpression *dst = d->makeDataRegister(extract<12, 14>(d->instructionWord(1)), m68k_fmt_i32);
        return d->makeInstruction(m68k_bfextu, "bfextu", src, offset_width.first, offset_width.second, dst);
    }
};

// BFFFO <ea>y {offset:width}, Dx
//
// Disabled because the documentation indicates that BFEXTU and BFFFO have the same bit pattern. [Robb P. Matzke 2013-10-28]
#if 0
struct M68k_bfffo: M68k {
    M68k_bfffo(): M68k("bfffo", m68k_68020|m68k_68030|m68k_68040,
                       OP(0xe) & BITS<6, 11>(0x27) & EAM(m68k_eam_drd | m68k_eam_control) &
                       INSN_WORD<1>(BIT<15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        ExpressionPair offset_width d->makeOffsetWidthPair(d->instructionWord(1));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 1);
        SgAsmExpression *dst = d->makeDataRegister(extract<12, 14>(d->instructionWord(1)), m68k_fmt_i32);
        return d->makeInstruction(m68k_bfffo, "bfffo", src, offset_width.first, offset_width.second, dst);
    }
};
#endif

// BFINS Dy, <ea>x {offset:width}
struct M68k_bfins: M68k {
    M68k_bfins(): M68k("bfins", m68k_68020|m68k_68030|m68k_68040,
                       OP(0xe) & BITS<6, 11>(0x3f) & EAM(m68k_eam_drd | (m68k_eam_control & m68k_eam_alter & ~m68k_eam_pcmi)) &
                       INSN_WORD<1>(BIT<15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        DisassemblerM68k::ExpressionPair offset_width = d->makeOffsetWidthPair(d->instructionWord(1));
        SgAsmExpression *src = d->makeDataRegister(extract<12, 14>(d->instructionWord(1)), m68k_fmt_i32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 1);
        return d->makeInstruction(m68k_bfins, "bfins", src, dst, offset_width.first, offset_width.second);
    }
};

// BFSET <ea>x {offset:width}
struct M68k_bfset: M68k {
    M68k_bfset(): M68k("bfset", m68k_68020|m68k_68030|m68k_68040,
                       OP(0xe) & BITS<6, 11>(0x3b) & EAM(m68k_eam_drd | (m68k_eam_control & m68k_eam_alter & ~m68k_eam_pcmi)) &
                       INSN_WORD<1>(BITS<12, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        DisassemblerM68k::ExpressionPair offset_width = d->makeOffsetWidthPair(d->instructionWord(1));
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 1);
        return d->makeInstruction(m68k_bfset, "bfset", dst, offset_width.first, offset_width.second);
    }
};

// BFTST <ea>y {offset:width}
struct M68k_bftst: M68k {
    M68k_bftst(): M68k("bftst", m68k_68020|m68k_68030|m68k_68040,
                       OP(0xe) & BITS<6, 11>(0x23) & EAM(m68k_eam_drd | m68k_eam_control) &
                       INSN_WORD<1>(BITS<12, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        DisassemblerM68k::ExpressionPair offset_width = d->makeOffsetWidthPair(d->instructionWord(1));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 1);
        return d->makeInstruction(m68k_bftst, "bftst", src, offset_width.first, offset_width.second);
    }
};

// BKPT #<data>
struct M68k_bkpt: M68k {
    M68k_bkpt(): M68k("bkpt", m68k_68ec000|m68k_68010|m68k_68020|m68k_68030|m68k_68040|m68k_freescale_cpu32,
                      OP(4) & BITS<3, 11>(0x109)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *data = d->makeImmediateValue(m68k_fmt_i8, extract<0, 2>(w0));
        return d->makeInstruction(m68k_bkpt, "bkpt", data);
    }
};

// BSET.L #<bitnum>, <ea>x
struct M68k_bset_1: M68k {
    M68k_bset_1(): M68k("bset_1", m68k_family,
                        OP(0) & BITS<6, 11>(0x23) & EAM(m68k_eam_drd) & INSN_WORD<1>(BITS<9, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(m68k_fmt_i8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 1);
        return d->makeInstruction(m68k_bset, "bset.l", src, dst);
    }
};

// BSET.B #<bitnum>, <ea>x
struct M68k_bset_2: M68k {
    M68k_bset_2(): M68k("bset_2", m68k_family,
                        OP(0) & BITS<6, 11>(0x23) & EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_drd & ~m68k_eam_pcmi) &
                        INSN_WORD<1>(BITS<9, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(m68k_fmt_i8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 1);
        return d->makeInstruction(m68k_bset, "bset.b", src, dst);
    }
};

// BSET.L Dy, <ea>x
struct M68k_bset_3: M68k {
    M68k_bset_3(): M68k("bset_3", m68k_family,
                        OP(0) & BITS<6, 8>(7) & EAM(m68k_eam_drd)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 0);
        return d->makeInstruction(m68k_bset, "bset.l", src, dst);
    }
};

// BSET.B Dy, <ea>x
struct M68k_bset_4: M68k {
    M68k_bset_4(): M68k("bset_4", m68k_family,
                        OP(0) & BITS<6, 8>(7) & EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_drd & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i8, 0);
        return d->makeInstruction(m68k_bset, "bset.b", src, dst);
    }
};

// BTST.L #<bitnum>, <ea>x
struct M68k_btst_1: M68k {
    M68k_btst_1(): M68k("btst_1", m68k_family,
                        OP(0) & BITS<6, 11>(0x20) & EAM(m68k_eam_drd) &
                        INSN_WORD<1>(BITS<8, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(m68k_fmt_i8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 1);
        return d->makeInstruction(m68k_btst, "btst.l", src, dst);
    }
};

// BTST.B #<bitnum>, <ea>x
//
// Note: Text says data addressing modes can be used, but the table excludes immediate mode. [Robb P. Matzke 2013-10-28]
struct M68k_btst_2: M68k {
    M68k_btst_2(): M68k("btst_2", m68k_family,
                        OP(0) & BITS<6, 11>(0x20) & EAM(m68k_eam_data & ~m68k_eam_imm & ~m68k_eam_drd) &
                        INSN_WORD<1>(BITS<8, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(m68k_fmt_i8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i8, 1);
        return d->makeInstruction(m68k_btst, "btst.b", src, dst);
    }
};

// BTST.L Dy, <ea>x
struct M68k_btst_3: M68k {
    M68k_btst_3(): M68k("btst_3", m68k_family,
                        OP(0) & BITS<6, 8>(4) & EAM(m68k_eam_drd)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 0);
        return d->makeInstruction(m68k_btst, "btst.l", src, dst);
    }
};

// BTST.B Dy, <ea>x
struct M68k_btst_4: M68k {
    M68k_btst_4(): M68k("btst_4", m68k_family,
                        OP(0) & BITS<6, 8>(4) & EAM(m68k_eam_data & ~m68k_eam_drd)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i8, 0);
        return d->makeInstruction(m68k_btst, "btst.b", src, dst);
    }
};

// CALLM #<data>, <ea>y
struct M68k_callm: M68k {
    M68k_callm(): M68k("callm", m68k_68020,
                       OP(0) & BITS<6, 11>(0x1b) & EAM(m68k_eam_control) &
                       INSN_WORD<1>(BITS<8, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *argcount = d->makeImmediateExtension(m68k_fmt_i8, 0);
        SgAsmExpression *ea = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 1);
        SgAsmExpression *target = d->makeAddress(ea);
        ASSERT_not_null2(target, "CALLM instruction must have a memory-referencing operand");
        return d->makeInstruction(m68k_callm, "callm", argcount, target);
    }
};

// CAS Dc,Du <ea>x
//
// Note: The text say memory alterable addressing modes can be used, but the table excludes program counter memory indirect
// modes. I am preferring the table over the text. [Robb P. Matzke 2013-10-28]
struct M68k_cas: M68k {
    M68k_cas(): M68k("cas", m68k_68020|m68k_68030|m68k_68040,
                     OP(0) & BIT<11>(1) & BITS<6, 8>(3) & EAM(m68k_eam_memory & m68k_eam_alter & ~m68k_eam_pcmi) &
                     INSN_WORD<1>(BITS<9, 15>(0) & BITS<3, 5>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        unsigned w1 = d->instructionWord(1);
        SgAsmExpression *du = d->makeDataRegister(extract<6, 8>(w1), m68k_fmt_i32);
        SgAsmExpression *dc = d->makeDataRegister(extract<0, 2>(w1), m68k_fmt_i32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 1);
        return d->makeInstruction(m68k_cas, "cas", dc, du, dst);
    }
};

// CAS2 Dc1:Dc2, Du1:Du2, (Rx1):(Rx2)
struct M68k_cas2: M68k {
    M68k_cas2(): M68k("cas2", m68k_68020|m68k_68030|m68k_68040,
                      OP(0) & BIT<11>(1) & BITS<0, 8>(0x0fc) &
                      INSN_WORD<1>(BITS<9, 11>(0) & BITS<3, 5>(0)) &
                      INSN_WORD<2>(BITS<9, 11>(0) & BITS<3, 5>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        unsigned w1 = d->instructionWord(1);
        unsigned w2 = d->instructionWord(2);
        SgAsmExpression *rx1 = d->makeDataAddressRegister(extract<12, 15>(w1), m68k_fmt_i32);
        SgAsmExpression *rx2 = d->makeDataAddressRegister(extract<12, 15>(w2), m68k_fmt_i32);
        SgAsmExpression *du1 = d->makeDataRegister(extract<6, 8>(w1), m68k_fmt_i32);
        SgAsmExpression *du2 = d->makeDataRegister(extract<6, 8>(w2), m68k_fmt_i32);
        SgAsmExpression *dc1 = d->makeDataRegister(extract<0, 2>(w1), m68k_fmt_i32);
        SgAsmExpression *dc2 = d->makeDataRegister(extract<0, 2>(w2), m68k_fmt_i32);
        return d->makeInstruction(m68k_cas2, "cas2", dc1, dc2, du1, du2, rx1, rx2);
    }
};

// CHK.W <ea>, Dn
// CHK.L <ea>, Dn
struct M68k_chk: M68k {
    M68k_chk(): M68k("chk", m68k_family,
                     OP(4) & (BITS<7, 8>(2) | BITS<7, 8>(3)) & BIT<6>(0) & EAM(m68k_eam_data)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = 3==extract<7, 8>(w0) ? m68k_fmt_i16 : m68k_fmt_i32;
        SgAsmExpression *bound = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *reg = d->makeDataRegister(extract<9, 11>(w0), fmt);
        return d->makeInstruction(m68k_chk, "chk."+formatLetter(fmt), bound, reg);
    }
};

// CHK2.B <ea>, Rn
// CHK2.W <ea>, Rn
// CHK2.L <ea>, Rn
struct M68k_chk2: M68k {
    M68k_chk2(): M68k("chk2", m68k_68020|m68k_68030|m68k_68040|m68k_freescale_cpu32,
                      OP(0) & BIT<11>(0) & (BITS<9, 10>(0) | BITS<9, 10>(1) | BITS<9, 10>(2)) & BITS<6, 8>(3) &
                      EAM(m68k_eam_control) &
                      INSN_WORD<1>(BITS<0, 11>(0x800))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<9, 10>(w0));
        SgAsmExpression *bounds = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *reg = d->makeDataAddressRegister(extract<12, 15>(w0), fmt);
        return d->makeInstruction(m68k_chk2, "chk2."+formatLetter(fmt), bounds, reg);
    }
};

// CLR.B <ea>x
// CLR.W <ea>x
// CLR.L <ea>x
struct M68k_clr: M68k {
    M68k_clr(): M68k("clr", m68k_family,
                     OP(4) & BITS<8, 11>(2) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                     EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(m68k_clr, "clr."+formatLetter(fmt), dst);
    }
};

// CMP.B <ea>y, Dx
// CMP.W <ea>y, Dx
// CMP.L <ea>y, Dx
struct M68k_cmp: M68k {
    M68k_cmp(): M68k("cmp", m68k_family,
                     OP(11) & (BITS<6, 8>(0) | BITS<6, 8>(1) | BITS<6, 8>(2)) & EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 8>(w0));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), fmt);
        return d->makeInstruction(m68k_cmp, "cmp."+formatLetter(fmt), src, dst);
    }
};

// CMPA.W <ea>y, Ax
// CMPA.L <ea>y, Ax
struct M68k_cmpa: M68k {
    M68k_cmpa(): M68k("cmpa", m68k_family,
                      OP(11) & (BITS<6, 8>(3) | BITS<6, 8>(7)) & EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = 3==extract<6, 8>(w0) ? m68k_fmt_i16 : m68k_fmt_i32;
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeAddressRegister(extract<9, 11>(w0), m68k_fmt_i32);
        return d->makeInstruction(m68k_cmpa, "cmpa."+formatLetter(fmt), src, dst);
    }
};

// CMPI.B #<data>, <ea>
// CMPI.W #<data>, <ea>
// CMPI.L #<data>, <ea>
struct M68k_cmpi: M68k {
    M68k_cmpi(): M68k("cmpi", m68k_family,
                      OP(0) & BITS<8, 11>(0xc) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(m68k_eam_data & ~m68k_eam_imm)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        size_t opandWords = (formatNBits(fmt)+15) / 16;
        SgAsmExpression *src = d->makeImmediateExtension(fmt, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, opandWords);
        return d->makeInstruction(m68k_cmpi, "cmpi."+formatLetter(fmt), src, dst);
    }
};

// CMPM.B (Ay)+, (Ax)+
// CMPM.W (Ay)+, (Ax)+
// CMPM.L (Ay)+, (Ax)+
struct M68k_cmpm: M68k {
    M68k_cmpm(): M68k("cmpm", m68k_family,
                      OP(11) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 5>(1)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeAddressRegisterPostIncrement(extract<0, 2>(w0), fmt);
        SgAsmExpression *dst = d->makeAddressRegisterPostIncrement(extract<9, 11>(w0), fmt);
        return d->makeInstruction(m68k_cmpm, "cmpm."+formatLetter(fmt), src, dst);
    }
};

// CMP2.B <ea>, Rn
// CMP2.W <ea>, Rn
// CMP2.L <ea>, Rn
struct M68k_cmp2: M68k {
    M68k_cmp2(): M68k("cmp2", m68k_68020|m68k_68030|m68k_68040|m68k_freescale_cpu32,
                      OP(0) & BIT<11>(0) & (BITS<9, 10>(0) | BITS<9, 10>(1) | BITS<9, 10>(2)) &
                      BITS<6, 8>(3) & EAM(m68k_eam_control) &
                      INSN_WORD<1>(BITS<0, 11>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<9, 10>(w0));
        SgAsmExpression *bounds = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *src2 = d->makeDataAddressRegister(extract<12, 15>(d->instructionWord(1)), fmt);
        return d->makeInstruction(m68k_cmp2, "cmp2."+formatLetter(fmt), bounds, src2);
    }
};

// CPBcc.W <label>
// CPBcc.L <label>
struct M68k_cpbcc: M68k {
    M68k_cpbcc(): M68k("cpbcc", m68k_68020|m68k_68030,
                       OP(15) & BITS<7, 8>(1)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        // Not implemented because we don't know what co-processors are present, and the co-processor determines the
        // size of this instruction. [Robb P. Matzke 2014-02-20]
        throw DisassemblerM68k::Exception("M68k CPBcc is not implemented: " + addrToString(w0, 16));
    }
};

// CPDBcc.W Dn, <label>
struct M68k_cpdbcc: M68k {
    M68k_cpdbcc(): M68k("cpdbcc", m68k_68020|m68k_68030,
                        OP(15) & BITS<3, 8>(0x09) &
                        INSN_WORD<1>(BITS<6, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        // Not implemented because we don't know what co-processors are present, and the co-processor determines the
        // size of this instruction. [Robb P. Matzke 2014-02-20]
        throw DisassemblerM68k::Exception("M68k CPDBcc is not implemented");
    }
};

// CPGEN <...>
struct M68k_cpgen: M68k {
    M68k_cpgen(): M68k("cpgen", m68k_68020|m68k_68030,
                       OP(15) & BITS<6, 8>(0)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        // Not implemented because we don't know what co-processors are present, and the co-processor determines the
        // size of this instruction. [Robb P. Matzke 2014-02-20]
        throw DisassemblerM68k::Exception("M68k CPGEN is not implemented");
    }
};

// CPSCC.B <ea>
struct M68k_cpscc: M68k {
    M68k_cpscc(): M68k("cpscc", m68k_68020|m68k_68030,
                       OP(15) & BITS<6, 8>(1) & EAM(m68k_eam_alter & ~m68k_eam_pcmi) &
                       INSN_WORD<1>(BITS<6, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        // Not implemented because we don't know what co-processors are present, and the co-processor determines the
        // size of this instruction. [Robb P. Matzke 2014-02-20]
        throw DisassemblerM68k::Exception("M68k CPScc is not implemented");
    }
};

// CPTRAPcc.W #<data>
// CPTRAPcc.L #<data>
struct M68k_cptrapcc: M68k {
    M68k_cptrapcc(): M68k("cptrapcc", m68k_68020|m68k_68030,
                          OP(15) & BITS<3, 8>(0x0f) & (BITS<0, 2>(2) | BITS<0, 2>(3) | BITS<0, 2>(4)) &
                          INSN_WORD<1>(BITS<6, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        // Not implemented because we don't know what co-processors are present, and the co-processor determines the
        // size of this instruction. [Robb P. Matzke 2014-02-20]
        throw DisassemblerM68k::Exception("M68k CPTRAPcc is not implemented");
    }
};

// CPUSHL dc, (Ax)
// CPUSHL ic, (Ax)
// CPUSHL bc, (Ax)
// CPUSHP dc, (Ax)
// CPUSHP ic, (Ax)
// CPUSHP bc, (Ax)
// CPUSHA dc, (Ax)
// CPUSHA ic, (Ax)
// CPUSHA bc, (Ax)
struct M68k_cpush: M68k {
    M68k_cpush(): M68k("cpush", m68k_generation_3,
                       OP(15) & BITS<8, 11>(4) & (BITS<6, 7>(1) | BITS<6, 7>(2) | BITS<6, 7>(3)) &
                       BIT<5>(1) & (BITS<3, 4>(1) | BITS<3, 4>(2) | BITS<3, 4>(3))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<3, 4>(w0)) {
            case 1: kind = m68k_cpushl; break;
            case 2: kind = m68k_cpushp; break;
            case 3: kind = m68k_cpusha; break;
            default: ASSERT_not_reachable("pattern should not have matched");
        }
        unsigned cacheId = extract<6, 7>(w0);
        SgAsmExpression *cache = d->makeImmediateValue(m68k_fmt_i8, cacheId);
        switch (cacheId) {
            case 1: cache->set_comment("data cache"); break;
            case 2: cache->set_comment("instruction cache"); break;
            case 3: cache->set_comment("data and instruction caches"); break;
            default: ASSERT_not_reachable("pattern should not have matched");
        }
        SgAsmExpression *ax = d->makeAddressRegister(extract<0, 2>(w0), m68k_fmt_i32);
        std::string mnemonic = stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_");
        return d->makeInstruction(kind, mnemonic, cache, ax);
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
struct M68k_dbcc: M68k {
    M68k_dbcc(): M68k("dbcc", m68k_family,
                      OP(5) & BITS<3, 7>(0x19)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        unsigned cc = extract<8, 11>(w0);
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (cc) {
            case  0: kind = m68k_dbt; break;
            case  1: kind = m68k_dbf; break;
            case  2: kind = m68k_dbhi; break;
            case  3: kind = m68k_dbls; break;
            case  4: kind = m68k_dbcc; break;
            case  5: kind = m68k_dbcs; break;
            case  6: kind = m68k_dbne; break;
            case  7: kind = m68k_dbeq; break;
            case  8: kind = m68k_dbvc; break;
            case  9: kind = m68k_dbvs; break;
            case 10: kind = m68k_dbpl; break;
            case 11: kind = m68k_dbmi; break;
            case 12: kind = m68k_dbge; break;
            case 13: kind = m68k_dblt; break;
            case 14: kind = m68k_dbgt; break;
            case 15: kind = m68k_dble; break;
        }
        std::string mnemonic = stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_");
        SgAsmExpression *src = d->makeDataRegister(extract<0, 2>(w0), m68k_fmt_i32);
        rose_addr_t target_va = d->get_insn_va() + 2 + signExtend<16, 32>((rose_addr_t)d->instructionWord(1));
        target_va &= GenMask<rose_addr_t, 32>::value;
        SgAsmIntegerValueExpression *target = d->makeImmediateValue(m68k_fmt_i32, target_va);
        return d->makeInstruction(kind, mnemonic+".w", src, target);
    }
};

// DIVS.W <ea>, Dx                      (Dx[16] / <ea>[16]) : (Dx[16] % <ea>[16]) -> Dq[32]
struct M68k_divs_w: M68k {
    M68k_divs_w(): M68k("divs_w", m68k_family,
                        OP(8) & BITS<6, 8>(7) & EAM(m68k_eam_data)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i16, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i32);
        return d->makeInstruction(m68k_divs, "divs.w", src, dst);
    }
};

// DIVU.W
struct M68k_divu_w: M68k {
    M68k_divu_w(): M68k("divu_w", m68k_family,
                        OP(8) & BITS<6, 8>(3) & EAM(m68k_eam_data)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i16, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i32);
        return d->makeInstruction(m68k_divu, "divu.w", src, dst);
    }
};
    
// DIVS.L <ea>, Dq                      Dq[32] / <ea>[32] -> Dq
// DIVU.L <ea>, Dq                      Dq[32] / <ea>[32] -> Dq
// DIVS.L <ea>, Dr, Dq                  (Dr:Dq)[64] / <ea>[32] -> Dq[32] and (Dr:Dq)[64] % <ea>[32] -> Dr[32]
// DIVU.L <ea>, Dr, Dq                  (Dr:Dq)[64] / <ea>[32] -> Dq[32] and (Dr:Dq)[64] % <ea>[32] -> Dr[32]
// DIVSL.L <ea>, Dr, Dq                 Dq[32] / <ea>[32] -> Dq and Dq[32] % <ea>[32] -> Dr[32]
// DIVUL.L <ea>, Dr, Dq                 Dq[32] / <ea>[32] -> Dq and Dq[32] % <ea>[32] -> Dr[32]
struct M68k_divide: M68k {
    M68k_divide(): M68k("divide", m68k_68020|m68k_68030|m68k_68040|m68k_freescale_cpu32,
                        OP(4) & BITS<6, 11>(0x31) & EAM(m68k_eam_data) &
                        INSN_WORD<1>(BIT<15>(0) & BITS<3, 9>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *ea = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 1);
        SgAsmExpression *dq = d->makeDataRegister(extract<12, 14>(d->instructionWord(1)), m68k_fmt_i32);
        if (extract<12, 14>(d->instructionWord(1)) == extract<0, 2>(d->instructionWord(1))) {
            // first form, 32-bit dividend, storing only the quotient
            if (extract<10, 10>(d->instructionWord(1)))
                return NULL;
            M68kInstructionKind kind = extract<11, 11>(d->instructionWord(1)) ? m68k_divs : m68k_divu;
            return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+".l", ea, dq);
        } else if (extract<10, 10>(d->instructionWord(1))) {
            // second form, 64-bit dividend, storing both quotient and remainder
            SgAsmExpression *dr = d->makeDataRegister(extract<0, 2>(d->instructionWord(1)), m68k_fmt_i32);
            M68kInstructionKind kind = extract<11, 11>(d->instructionWord(1)) ? m68k_divs : m68k_divu;
            return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+".l", ea, dr, dq);
        } else {
            // third form, 32-bit dividend, storing both quotient and remainder
            SgAsmExpression *dr = d->makeDataRegister(extract<0, 2>(d->instructionWord(1)), m68k_fmt_i32);
            M68kInstructionKind kind = extract<11, 11>(d->instructionWord(1)) ? m68k_divsl : m68k_divul;
            return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+".l", ea, dr, dq);
        }
    }
};

// EOR.B Dy, <ea>x
// EOR.W Dy, <ea>x
// EOR.L Dy, <ea>x
struct M68k_eor: M68k {
    M68k_eor(): M68k("eor", m68k_family,
                     OP(11) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                     EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), fmt);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(m68k_eor, "eor."+formatLetter(fmt), src, dst);
    }
};

// EORI.B #<data>, <ea>
// EORI.W #<data>, <ea>
// EORI.L #<data>, <ea>
struct M68k_eori: M68k {
    M68k_eori(): M68k("eori", m68k_family,
                      OP(0) & BITS<8, 11>(0xa) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        size_t opandWords = (formatNBits(fmt)+15) / 16;
        SgAsmExpression *src = d->makeImmediateExtension(fmt, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, opandWords);
        return d->makeInstruction(m68k_eori, "eori."+formatLetter(fmt), src, dst);
    }
};

// EORI.B #<data>, CCR
struct M68k_eori_to_ccr: M68k {
    M68k_eori_to_ccr(): M68k("eori_to_ccr", m68k_family,
                             OP(0) & BITS<0, 11>(0xa3c) & INSN_WORD<1>(BITS<8, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(m68k_fmt_i8, 0);
        SgAsmExpression *dst = d->makeConditionCodeRegister();
        return d->makeInstruction(m68k_eori, "eori.b", src, dst);
    }
};

// EXG.L Dx, Dy
// EXG.L Ax, Ay
// EXG.L Dx, Ay
struct M68k_exg: M68k {
    M68k_exg(): M68k("exg", m68k_family,
                     OP(12) & BIT<8>(1) & (BITS<3, 7>(0x08) | BITS<3, 7>(0x09) | BITS<3, 7>(0x11))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *rx=NULL, *ry=NULL;
        switch (extract<3, 7>(w0)) {
            case 0x08:
                rx = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i32);
                ry = d->makeDataRegister(extract<0, 2>(w0), m68k_fmt_i32);
                break;
            case 0x09:
                rx = d->makeAddressRegister(extract<9, 11>(w0), m68k_fmt_i32);
                ry = d->makeAddressRegister(extract<0, 2>(w0), m68k_fmt_i32);
                break;
            case 0x11:
                rx = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i32);
                ry = d->makeAddressRegister(extract<0, 2>(w0), m68k_fmt_i32);
                break;
        }
        return d->makeInstruction(m68k_exg, "exg.l", rx, ry);
    }
};

// EXT.W Dx
struct M68k_ext_w: M68k {
    M68k_ext_w(): M68k("ext_w", m68k_family,
                       OP(4) & BITS<3, 11>(0x110)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *rx = d->makeDataRegister(extract<0, 2>(w0), m68k_fmt_i16);
        return d->makeInstruction(m68k_ext, "ext.w", rx);
    }
};

// EXT.L Dx
struct M68k_ext_l: M68k {
    M68k_ext_l(): M68k("ext_l", m68k_family,
                       OP(4) & BITS<3, 11>(0x118)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *rx = d->makeDataRegister(extract<0, 2>(w0), m68k_fmt_i32);
        return d->makeInstruction(m68k_ext, "ext.l", rx);
    }
};

// EXTB.L Dx
struct M68k_extb_l: M68k {
    M68k_extb_l(): M68k("extb_l", m68k_family,
                        OP(4) & BITS<3, 11>(0x138)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *rx = d->makeDataRegister(extract<0, 2>(w0), m68k_fmt_i32);
        return d->makeInstruction(m68k_extb, "extb.l", rx);
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
struct M68k_fabs_from_drd: M68k {
    M68k_fabs_from_drd(): M68k("fabs_from_drd", m68k_generation_3|m68k_freescale_fpu,
                               OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_drd) &
                               INSN_WORD<1>(BITS<13, 15>(2) &
                                            (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                             BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                            (BITS<0, 6>(0x18) | BITS<0, 6>(0x58) | BITS<0, 6>(0x5c)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x18: kind = m68k_fabs; break;
            case 0x58: kind = m68k_fsabs; break;
            case 0x5c: kind = m68k_fdabs; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
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
//    Note: data register direct mode is handled by M68k_fabs_from_drd
struct M68k_fabs_mr: M68k {
    M68k_fabs_mr(): M68k("fabs_mr", m68k_generation_3|m68k_freescale_fpu,
                         OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_data & ~m68k_eam_drd) &
                         INSN_WORD<1>(BITS<13, 15>(2) &
                                      (BITS<0, 6>(0x18) | BITS<0, 6>(0x58) | BITS<0, 6>(0x5c)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x18: kind = m68k_fabs; break;
            case 0x58: kind = m68k_fsabs; break;
            case 0x5c: kind = m68k_fdabs; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FABS.<fmt> FPx, FPy
// FSABS.<fmt> FPx, FPy
// FDABS.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct M68k_fabs_rr: M68k {
    M68k_fabs_rr(): M68k("fabs_rr", m68k_generation_3|m68k_freescale_fpu,
                         OP(15) & BITS<0, 11>(0x200) &
                         INSN_WORD<1>(BITS<13, 15>(0) &
                                      (BITS<0, 6>(0x18) | BITS<0, 6>(0x58) | BITS<0, 6>(0x5c)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x18: kind = m68k_fabs; break;
            case 0x58: kind = m68k_fsabs; break;
            case 0x5c: kind = m68k_fdabs; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
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
struct M68k_fadd_from_drd: M68k {
    M68k_fadd_from_drd(): M68k("fadd_from_drd", m68k_generation_3|m68k_freescale_fpu,
                               OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_drd) &
                               INSN_WORD<1>(BITS<13, 15>(2) &
                                            (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                             BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                            (BITS<0, 6>(0x22) | BITS<0, 6>(0x62) | BITS<0, 6>(0x66)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x22: kind = m68k_fadd; break;
            case 0x62: kind = m68k_fsadd; break;
            case 0x66: kind = m68k_fdadd; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
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
//    Note: data register direct mode is handled by M68k_fadd_from_drd
struct M68k_fadd_mr: M68k {
    M68k_fadd_mr(): M68k("fadd_mr", m68k_generation_3|m68k_freescale_fpu,
                         OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_data & ~m68k_eam_drd) &
                         INSN_WORD<1>(BITS<13, 15>(2) &
                                      (BITS<0, 6>(0x22) | BITS<0, 6>(0x62) | BITS<0, 6>(0x66)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x22: kind = m68k_fadd; break;
            case 0x62: kind = m68k_fsadd; break;
            case 0x66: kind = m68k_fdadd; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FADD.<fmt> FPx, FPy
// FSADD.<fmt> FPx, FPy
// FDADD.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct M68k_fadd_rr: M68k {
    M68k_fadd_rr(): M68k("fadd_rr", m68k_generation_3|m68k_freescale_fpu,
                         OP(15) & BITS<0, 11>(0x200) &
                         INSN_WORD<1>(BITS<13, 15>(0) &
                                      (BITS<0, 6>(0x22) | BITS<0, 6>(0x62) | BITS<0, 6>(0x66)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x22: kind = m68k_fadd; break;
            case 0x62: kind = m68k_fsadd; break;
            case 0x66: kind = m68k_fdadd; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
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
struct M68k_fb: M68k {
    M68k_fb(): M68k("fb", m68k_generation_3|m68k_freescale_fpu,
                    OP(15) & BITS<9, 11>(1) & BITS<7, 8>(1) & BIT<5>(0)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kInstructionKind kind = m68k_unknown_instruction;
        // predicate: table 2-23 "Floating-point conditional tests" in "M68000 family progammer's reference manual"
        switch (extract<0, 5>(w0)) {
            case 0x01: kind = m68k_fbeq; break;
            case 0x0e: kind = m68k_fbne; break;
            case 0x12: kind = m68k_fbgt; break;
            case 0x1d: kind = m68k_fbngt; break;
            case 0x13: kind = m68k_fbge; break;
            case 0x1c: kind = m68k_fbnge; break;
            case 0x14: kind = m68k_fblt; break;
            case 0x1b: kind = m68k_fbnlt; break;
            case 0x15: kind = m68k_fble; break;
            case 0x1a: kind = m68k_fbnle; break;
            case 0x16: kind = m68k_fbgl; break;
            case 0x19: kind = m68k_fbngl; break;
            case 0x17: kind = m68k_fbgle; break;
            case 0x18: kind = m68k_fbngle; break;
            case 0x02: kind = m68k_fbogt; break;
            case 0x0d: kind = m68k_fbule; break;
            case 0x03: kind = m68k_fboge; break;
            case 0x0c: kind = m68k_fbult; break;
            case 0x04: kind = m68k_fbolt; break;
            case 0x0b: kind = m68k_fbuge; break;
            case 0x05: kind = m68k_fbole; break;
            case 0x0a: kind = m68k_fbugt; break;
            case 0x06: kind = m68k_fbogl; break;
            case 0x09: kind = m68k_fbueq; break;
            case 0x07: kind = m68k_fbor; break;
            case 0x08: kind = m68k_fbun; break;
            case 0x00: kind = m68k_fbf; break;
            case 0x0f: kind = m68k_fbt; break;
            case 0x10: kind = m68k_fbsf; break;
            case 0x1f: kind = m68k_fbst; break;
            case 0x11: kind = m68k_fbseq; break;
            case 0x1e: kind = m68k_fbsne; break;
        }
        M68kDataFormat fmt = m68k_fmt_unknown;
        int32_t offset = 0;
        if (extract<6, 6>(w0)) {
            fmt = m68k_fmt_i32;
            offset = shiftLeft<32>((uint32_t)d->instructionWord(1), 16) | (uint32_t)d->instructionWord(2);
        } else {
            fmt = m68k_fmt_i16;
            offset = signExtend<16, 32>((uint32_t)d->instructionWord(1));
        }
        rose_addr_t base = d->get_insn_va() + 2;
        rose_addr_t target_va = (base + offset) & GenMask<rose_addr_t, 32>::value;
        SgAsmIntegerValueExpression *target = d->makeImmediateValue(m68k_fmt_i32, target_va);
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
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
struct M68k_fcmp_from_drd: M68k {
    M68k_fcmp_from_drd(): M68k("fcmp_from_drd", m68k_generation_3|m68k_freescale_fpu,
                               OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_drd) &
                               INSN_WORD<1>(BITS<13, 15>(2) &
                                            (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                             BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                            BITS<0, 6>(0x38))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        return d->makeInstruction(m68k_fcmp, "fcmp."+formatLetter(fmt), src, dst);
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
//    Note: data register direct mode is handled by M68k_fcmp_from_drd
//    The ColdFire documentation says bits<0,6> of the first extension word are 0x38, but the M68000 documentation
//    says that they are 0x0c. I'm using the ColdFire version.
struct M68k_fcmp_mr: M68k {
    M68k_fcmp_mr(): M68k("fcmp_mr", m68k_generation_3|m68k_freescale_fpu,
                         OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_data & ~m68k_eam_drd) &
                         INSN_WORD<1>(BITS<13, 15>(2) & BITS<0, 6>(0x38))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        return d->makeInstruction(m68k_fcmp, "fcmp."+formatLetter(fmt), src, dst);
    }
};

// FCMP.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
//    The ColdFire documentation says bits<0,6> of the first extension word are 0x38, but the M68000 documentation
//    says that they are 0x0c. I'm using the ColdFire version.
struct M68k_fcmp_rr: M68k {
    M68k_fcmp_rr(): M68k("fcmp_rr", m68k_generation_3|m68k_freescale_fpu,
                         OP(15) & BITS<0, 11>(0x200) &
                         INSN_WORD<1>(BITS<13, 15>(0) & BITS<0, 6>(0x38))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        return d->makeInstruction(m68k_fcmp, "fcmp."+formatLetter(fmt), src, dst);
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
struct M68k_fdiv_from_drd: M68k {
    M68k_fdiv_from_drd(): M68k("fdiv_from_drd", m68k_generation_3|m68k_freescale_fpu,
                               OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_drd) &
                               INSN_WORD<1>(BITS<13, 15>(2) &
                                            (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                             BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                            (BITS<0, 6>(0x20) | BITS<0, 6>(0x60) | BITS<0, 6>(0x64)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x20: kind = m68k_fdiv; break;
            case 0x60: kind = m68k_fsdiv; break;
            case 0x64: kind = m68k_fddiv; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
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
//    Note: data register direct mode is handled by M68k_fdiv_from_drd
struct M68k_fdiv_mr: M68k {
    M68k_fdiv_mr(): M68k("fdiv_mr", m68k_generation_3|m68k_freescale_fpu,
                         OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_data & ~m68k_eam_drd) &
                         INSN_WORD<1>(BITS<13, 15>(2) &
                                      (BITS<0, 6>(0x20) | BITS<0, 6>(0x60) | BITS<0, 6>(0x64)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x20: kind = m68k_fdiv; break;
            case 0x60: kind = m68k_fsdiv; break;
            case 0x64: kind = m68k_fddiv; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FDIV.<fmt> FPx, FPy
// FSDIV.<fmt> FPx, FPy
// FDDIV.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct M68k_fdiv_rr: M68k {
    M68k_fdiv_rr(): M68k("fdiv_rr", m68k_generation_3|m68k_freescale_fpu,
                         OP(15) & BITS<0, 11>(0x200) &
                         INSN_WORD<1>(BITS<13, 15>(0) &
                                      (BITS<0, 6>(0x20) | BITS<0, 6>(0x60) | BITS<0, 6>(0x64)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x20: kind = m68k_fdiv; break;
            case 0x60: kind = m68k_fsdiv; break;
            case 0x64: kind = m68k_fddiv; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FINT.B Dx, FPy
// FINT.W Dx, FPy
// FINT.L Dx, FPy
// FINT.S Dx, FPy
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct M68k_fint_from_drd: M68k {
    M68k_fint_from_drd(): M68k("fint_from_drd", m68k_generation_3|m68k_freescale_fpu,
                               OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_drd) &
                               INSN_WORD<1>(BITS<13, 15>(2) &
                                            (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                             BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                            BITS<0, 6>(0x01))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        return d->makeInstruction(m68k_fint, "fint."+formatLetter(fmt), src, dst);
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
//    Note: data register direct mode is handled by M68k_fint_from_drd
struct M68k_fint_mr: M68k {
    M68k_fint_mr(): M68k("fint_mr", m68k_generation_3|m68k_freescale_fpu,
                         OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_data & ~m68k_eam_drd) &
                         INSN_WORD<1>(BITS<13, 15>(2) & BITS<0, 6>(0x01))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        return d->makeInstruction(m68k_fint, "fint."+formatLetter(fmt), src, dst);
    }
};

// FINT.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct M68k_fint_rr: M68k {
    M68k_fint_rr(): M68k("fint_rr", m68k_generation_3|m68k_freescale_fpu,
                         OP(15) & BITS<0, 11>(0x200) &
                         INSN_WORD<1>(BITS<13, 15>(0) & BITS<0, 6>(0x01))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        return d->makeInstruction(m68k_fint, "fint."+formatLetter(fmt), src, dst);
    }
};

// FINTRZ.B Dx, FPy
// FINTRZ.W Dx, FPy
// FINTRZ.L Dx, FPy
// FINTRZ.S Dx, FPy
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct M68k_fintrz_from_drd: M68k {
    M68k_fintrz_from_drd(): M68k("fintrz_from_drd", m68k_generation_3|m68k_freescale_fpu,
                                 OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_drd) &
                                 INSN_WORD<1>(BITS<13, 15>(2) &
                                              (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                               BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                              BITS<0, 6>(0x03))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        return d->makeInstruction(m68k_fintrz, "fintrz."+formatLetter(fmt), src, dst);
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
//    Note: data register direct mode is handled by M68k_fintrz_from_drd
struct M68k_fintrz_mr: M68k {
    M68k_fintrz_mr(): M68k("fintrz_mr", m68k_generation_3|m68k_freescale_fpu,
                           OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_data & ~m68k_eam_drd) &
                           INSN_WORD<1>(BITS<13, 15>(2) & BITS<0, 6>(0x03))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        return d->makeInstruction(m68k_fintrz, "fintrz."+formatLetter(fmt), src, dst);
    }
};

// FINTRZ.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct M68k_fintrz_rr: M68k {
    M68k_fintrz_rr(): M68k("fintrz_rr", m68k_generation_3|m68k_freescale_fpu,
                           OP(15) & BITS<0, 11>(0x200) &
                           INSN_WORD<1>(BITS<13, 15>(0) & BITS<0, 6>(0x03))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        return d->makeInstruction(m68k_fintrz, "fintrz."+formatLetter(fmt), src, dst);
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
struct M68k_fmove_from_drd: M68k {
    M68k_fmove_from_drd(): M68k("fmove_from_drd", m68k_generation_3|m68k_freescale_fpu,
                                OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_drd) &
                                INSN_WORD<1>(BITS<13, 15>(2) &
                                             (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                              BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                             (BITS<0, 6>(0) | BITS<0, 6>(0x40) | BITS<0, 6>(0x44)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x00: kind = m68k_fmove; break;
            case 0x40: kind = m68k_fsmove; break;
            case 0x44: kind = m68k_fdmove; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
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
//    Note: the data register direct source is handled by M68k_fmove_from_drd
struct M68k_fmove_mr: M68k {
    M68k_fmove_mr(): M68k("fmove_mr", m68k_generation_3|m68k_freescale_fpu,
                          OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_data & ~m68k_eam_drd) &
                          INSN_WORD<1>(BITS<13, 15>(2) &
                                       (BITS<0, 6>(0) | BITS<0, 6>(0x40) | BITS<0, 6>(0x44)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x00: kind = m68k_fmove; break;
            case 0x40: kind = m68k_fsmove; break;
            case 0x44: kind = m68k_fdmove; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
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
//    Note: the data register direct destination is handled by M68k_fmove_to_drd
struct M68k_fmove_rm: M68k {
    M68k_fmove_rm(): M68k("fmove_rm", m68k_generation_3|m68k_freescale_fpu,
                          OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) &
                          EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi & ~m68k_eam_drd) &
                          INSN_WORD<1>(BITS<13, 15>(0x3) & BITS<0, 6>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        return d->makeInstruction(m68k_fmove, "fmove."+formatLetter(fmt), src, dst);
    }
};

// FMOVE.<fmt> FPx, FPy
// FSMOVE.<fmt> FPx, FPy
// FDMOVE.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-16]: Coprocessor 1 only
struct M68k_fmove_rr: M68k {
    M68k_fmove_rr(): M68k("fmove_rr", m68k_generation_3|m68k_freescale_fpu,
                          OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & BITS<0, 5>(0) &
                          INSN_WORD<1>(BITS<13, 15>(0) &
                                       (BITS<0, 6>(0) | BITS<0, 6>(0x40) | BITS<0, 6>(0x44)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x00: kind = m68k_fmove; break;
            case 0x40: kind = m68k_fsmove; break;
            case 0x44: kind = m68k_fdmove; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FMOVE.B FPx, Dy
// FMOVE.W FPx, Dy
// FMOVE.L FPx, Dy
// FMOVE.S FPx, Dy
//    FIXME[Robb P. Matzke 2014-07-16]: Coprocessor 1 only
//    FIXME[Robb P. Matzke 2014-07-16]: BCD format (P) is not implemented yet; k-factor bits are always zero
struct M68k_fmove_to_drd: M68k {
    M68k_fmove_to_drd(): M68k("fmove_to_drd", m68k_generation_3|m68k_freescale_fpu,
                              OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_drd) &
                              INSN_WORD<1>(BITS<13, 15>(3) &
                                           (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                            BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                           BITS<0, 6>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        ASSERT_require(fmt==m68k_fmt_i8 || fmt==m68k_fmt_i16 || fmt==m68k_fmt_i32 || fmt==m68k_fmt_f32);
        SgAsmExpression *src = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        return d->makeInstruction(m68k_fmove, "fmove." + formatLetter(fmt), src, dst);
    }
};

// FMOVEM.<fmt> <ea>, #list
//    where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-16]: Coprocessor 1 only
struct M68k_fmovem_mr: M68k {
    M68k_fmovem_mr(): M68k("fmovem_mr", m68k_generation_3|m68k_freescale_fpu,
                           OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) &
                           EAM(m68k_eam_control | m68k_eam_inc) &
                           INSN_WORD<1>(BITS<14, 15>(3) & BIT<13>(0) & BITS<8, 10>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        bool dynamicList = extract<11, 11>(w0) != 0;
        bool postIncMode = extract<12, 12>(w0) != 0;
        if (!postIncMode && extract<3, 5>(w0)==3) {
            throw DisassemblerM68k::Exception("pre-decrement mode and [Ax+] source address are "
                                              "incompatible for the register-to-memory version of FMOVEM");
        }
        SgAsmExpression *dst = NULL;
        if (dynamicList) {
            dst = d->makeDataRegister(extract<4, 6>(d->instructionWord(1)), m68k_fmt_i8, 0);
        } else {
            dst = d->makeFPRegistersFromMask(extract<0, 8>(d->instructionWord(1)), fmt, true); // order is FN7 to FN0
        }
        return d->makeInstruction(m68k_fmovem, "fmovem."+formatLetter(fmt), src, dst);
    }
};

// FMOVEM.<fmt> #list, <ea>
//    where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-16]: Coprocessor 1 only
struct M68k_fmovem_rm: M68k {
    M68k_fmovem_rm(): M68k("fmovem_rm", m68k_generation_3|m68k_freescale_fpu,
                           OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) &
                           EAM((m68k_eam_control & m68k_eam_alter & ~m68k_eam_pcmi) | m68k_eam_dec) &
                           INSN_WORD<1>(BITS<14, 15>(3) & BIT<13>(1) & BITS<8, 10>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormatForFamily(d->get_family());
        bool dynamicList = extract<11, 11>(w0) != 0;
        bool postIncMode = extract<12, 12>(w0) != 0;
        if (postIncMode && extract<3, 5>(w0)==4) {
            throw DisassemblerM68k::Exception("post-increment mode and [-Ax] destination address are "
                                              "incompatible for the register-to-memory version of FMOVEM");
        }
        SgAsmExpression *src = NULL;
        if (dynamicList) {
            src = d->makeDataRegister(extract<4, 6>(d->instructionWord(1)), m68k_fmt_i8, 0);
        } else {
            src = d->makeFPRegistersFromMask(extract<0, 8>(d->instructionWord(1)), fmt); // order is FN0 to FN7
        }
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        return d->makeInstruction(m68k_fmovem, "fmovem."+formatLetter(fmt), src, dst);
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
struct M68k_fmul_from_drd: M68k {
    M68k_fmul_from_drd(): M68k("fmul_from_drd", m68k_generation_3|m68k_freescale_fpu,
                               OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_drd) &
                               INSN_WORD<1>(BITS<13, 15>(2) &
                                            (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                             BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                            (BITS<0, 6>(0x23) | BITS<0, 6>(0x63) | BITS<0, 6>(0x67)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x23: kind = m68k_fmul; break;
            case 0x63: kind = m68k_fsmul; break;
            case 0x67: kind = m68k_fdmul; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
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
//    Note: data register direct mode is handled by M68k_fmul_from_drd
struct M68k_fmul_mr: M68k {
    M68k_fmul_mr(): M68k("fmul_mr", m68k_generation_3|m68k_freescale_fpu,
                         OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_data & ~m68k_eam_drd) &
                         INSN_WORD<1>(BITS<13, 15>(2) &
                                      (BITS<0, 6>(0x23) | BITS<0, 6>(0x63) | BITS<0, 6>(0x67)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x23: kind = m68k_fmul; break;
            case 0x63: kind = m68k_fsmul; break;
            case 0x67: kind = m68k_fdmul; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FMUL.<fmt> FPx, FPy
// FSMUL.<fmt> FPx, FPy
// FDMUL.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct M68k_fmul_rr: M68k {
    M68k_fmul_rr(): M68k("fmul_rr", m68k_generation_3|m68k_freescale_fpu,
                         OP(15) & BITS<0, 11>(0x200) &
                         INSN_WORD<1>(BITS<13, 15>(0) &
                                      (BITS<0, 6>(0x23) | BITS<0, 6>(0x63) | BITS<0, 6>(0x67)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x23: kind = m68k_fmul; break;
            case 0x63: kind = m68k_fsmul; break;
            case 0x67: kind = m68k_fdmul; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
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
struct M68k_fneg_from_drd: M68k {
    M68k_fneg_from_drd(): M68k("fneg_from_drd", m68k_generation_3|m68k_freescale_fpu,
                               OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_drd) &
                               INSN_WORD<1>(BITS<13, 15>(2) &
                                            (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                             BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                            (BITS<0, 6>(0x1a) | BITS<0, 6>(0x5a) | BITS<0, 6>(0x5e)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x1a: kind = m68k_fneg; break;
            case 0x5a: kind = m68k_fsneg; break;
            case 0x5e: kind = m68k_fdneg; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
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
//    Note: data register direct mode is handled by M68k_fneg_from_drd
struct M68k_fneg_mr: M68k {
    M68k_fneg_mr(): M68k("fneg_mr", m68k_generation_3|m68k_freescale_fpu,
                         OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_data & ~m68k_eam_drd) &
                         INSN_WORD<1>(BITS<13, 15>(2) &
                                      (BITS<0, 6>(0x1a) | BITS<0, 6>(0x5a) | BITS<0, 6>(0x5e)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x1a: kind = m68k_fneg; break;
            case 0x5a: kind = m68k_fsneg; break;
            case 0x5e: kind = m68k_fdneg; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FNEG.<fmt> FPx, FPy
// FSNEG.<fmt> FPx, FPy
// FDNEG.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct M68k_fneg_rr: M68k {
    M68k_fneg_rr(): M68k("fneg_rr", m68k_generation_3|m68k_freescale_fpu,
                          OP(15) & BITS<0, 11>(0x200) &
                          INSN_WORD<1>(BITS<13, 15>(0) &
                                       (BITS<0, 6>(0x1a) | BITS<0, 6>(0x5a) | BITS<0, 6>(0x5e)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x1a: kind = m68k_fneg; break;
            case 0x5a: kind = m68k_fsneg; break;
            case 0x5e: kind = m68k_fdneg; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FNOP
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct M68k_fnop: M68k {
    M68k_fnop(): M68k("fnop", m68k_generation_3|m68k_freescale_fpu,
                      OP(15) & BITS<9, 11>(1) & BITS<0, 8>(0x080) &
                      INSN_WORD<1>(BITS<0, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_fnop, "fnop");
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
struct M68k_fsqrt_from_drd: M68k {
    M68k_fsqrt_from_drd(): M68k("fsqrt_from_drd", m68k_generation_3|m68k_freescale_fpu,
                                OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_drd) &
                                INSN_WORD<1>(BITS<13, 15>(2) &
                                             (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                              BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                             (BITS<0, 6>(0x04) | BITS<0, 6>(0x41) | BITS<0, 6>(0x45)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x04: kind = m68k_fsqrt; break;
            case 0x41: kind = m68k_fssqrt; break;
            case 0x45: kind = m68k_fdsqrt; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
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
//    Note: data register direct mode is handled by M68k_fsqrt_from_drd
struct M68k_fsqrt_mr: M68k {
    M68k_fsqrt_mr(): M68k("fsqrt_mr", m68k_generation_3|m68k_freescale_fpu,
                          OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_data & ~m68k_eam_drd) &
                          INSN_WORD<1>(BITS<13, 15>(2) &
                                       (BITS<0, 6>(0x04) | BITS<0, 6>(0x41) | BITS<0, 6>(0x45)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x04: kind = m68k_fsqrt; break;
            case 0x41: kind = m68k_fssqrt; break;
            case 0x45: kind = m68k_fdsqrt; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FSQRT.<fmt> FPx, FPy
// FSSQRT.<fmt> FPx, FPy
// FDSQRT.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct M68k_fsqrt_rr: M68k {
    M68k_fsqrt_rr(): M68k("fsqrt_rr", m68k_generation_3|m68k_freescale_fpu,
                          OP(15) & BITS<0, 11>(0x200) &
                          INSN_WORD<1>(BITS<13, 15>(0) &
                                       (BITS<0, 6>(0x04) | BITS<0, 6>(0x41) | BITS<0, 6>(0x45)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x04: kind = m68k_fsqrt; break;
            case 0x41: kind = m68k_fssqrt; break;
            case 0x45: kind = m68k_fdsqrt; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
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
struct M68k_fsub_from_drd: M68k {
    M68k_fsub_from_drd(): M68k("fsub_from_drd", m68k_generation_3|m68k_freescale_fpu,
                               OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_drd) &
                               INSN_WORD<1>(BITS<13, 15>(2) &
                                            (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                             BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                            (BITS<0, 6>(0x28) | BITS<0, 6>(0x68) | BITS<0, 6>(0x6c)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x28: kind = m68k_fsub; break;
            case 0x68: kind = m68k_fssub; break;
            case 0x6c: kind = m68k_fdsub; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
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
//    Note: data register direct mode is handled by M68k_fsub_from_drd
struct M68k_fsub_mr: M68k {
    M68k_fsub_mr(): M68k("fsub_mr", m68k_generation_3|m68k_freescale_fpu,
                         OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_data & ~m68k_eam_drd) &
                         INSN_WORD<1>(BITS<13, 15>(2) &
                                      (BITS<0, 6>(0x28) | BITS<0, 6>(0x68) | BITS<0, 6>(0x6c)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x28: kind = m68k_fsub; break;
            case 0x68: kind = m68k_fssub; break;
            case 0x6c: kind = m68k_fdsub; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FSUB.<fmt> FPx, FPy
// FSSUB.<fmt> FPx, FPy
// FDSUB.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
struct M68k_fsub_rr: M68k {
    M68k_fsub_rr(): M68k("fsub_rr", m68k_generation_3|m68k_freescale_fpu,
                         OP(15) & BITS<0, 11>(0x200) &
                         INSN_WORD<1>(BITS<13, 15>(0) &
                                      (BITS<0, 6>(0x28) | BITS<0, 6>(0x68) | BITS<0, 6>(0x6c)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *dst = d->makeFPRegister(extract<7, 9>(d->instructionWord(1)));
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<0, 6>(d->instructionWord(1))) {
            case 0x28: kind = m68k_fsub; break;
            case 0x68: kind = m68k_fssub; break;
            case 0x6c: kind = m68k_fdsub; break;
        }
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+
                                  "."+formatLetter(fmt), src, dst);
    }
};

// FTST.B Dx, FPy
// FTST.W Dx, FPy
// FTST.L Dx, FPy
// FTST.S Dx, FPy
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
//    Destination register field is unused (bits 7-9) and recommended, but not required, to be zero
struct M68k_ftst_from_drd: M68k {
    M68k_ftst_from_drd(): M68k("ftst_from_drd", m68k_generation_3|m68k_freescale_fpu,
                               OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_drd) &
                               INSN_WORD<1>(BITS<13, 15>(2) &
                                            (BITS<10, 12>(0)/*i32*/ | BITS<10, 12>(1)/*f32*/ |
                                             BITS<10, 12>(4)/*i16*/ | BITS<10, 12>(6)/*i8*/) &
                                            BITS<0, 6>(0x3a))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        return d->makeInstruction(m68k_ftst, "ftst."+formatLetter(fmt), src);
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
//    Note: data register direct mode is handled by M68k_ftst_from_drd
//    Destination register field is unused (bits 7-9) and recommended, but not required, to be zero
struct M68k_ftst_mr: M68k {
    M68k_ftst_mr(): M68k("ftst_mr", m68k_generation_3|m68k_freescale_fpu,
                         OP(15) & BITS<9, 11>(1) & BITS<6, 8>(0) & EAM(m68k_eam_data & ~m68k_eam_drd) &
                         INSN_WORD<1>(BITS<13, 15>(2) & BITS<0, 6>(0x3a))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormat(extract<10, 12>(d->instructionWord(1)));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        return d->makeInstruction(m68k_ftst, "ftst."+formatLetter(fmt), src);
    }
};

// FTST.<fmt> FPx, FPy
//    Where <fmt> is either "D" or "X" depending on the architecture
//    FIXME[Robb P. Matzke 2014-07-17]: Coprocessor 1 only
//    Destination register field is unused (bits 7-9) and recommended, but not required, to be zero
struct M68k_ftst_rr: M68k {
    M68k_ftst_rr(): M68k("ftst_rr", m68k_generation_3|m68k_freescale_fpu,
                         OP(15) & BITS<0, 11>(0x200) &
                         INSN_WORD<1>(BITS<13, 15>(0) & BITS<0, 6>(0x3a))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = floatingFormatForFamily(d->get_family());
        SgAsmExpression *src = d->makeFPRegister(extract<10, 12>(d->instructionWord(1)));
        return d->makeInstruction(m68k_ftst, "ftst."+formatLetter(fmt), src);
    }
};

// ILLEGAL
struct M68k_illegal: M68k {
    M68k_illegal(): M68k("illegal", m68k_family,
                         OP(4) & BITS<0, 11>(0xafc)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_illegal, "illegal");
    }
};

// JMP <ea>x
struct M68k_jmp: M68k {
    M68k_jmp(): M68k("jmp", m68k_family,
                     OP(4) & BITS<6, 11>(0x3b) & EAM(m68k_eam_control)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *ea = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 0);
        SgAsmExpression *target = d->makeAddress(ea);
        ASSERT_not_null2(target, "JMP instruction must have a memory-referencing operand");
        return d->makeInstruction(m68k_jmp, "jmp", target);
    }
};
                
// JSR <ea>x
struct M68k_jsr: M68k {
    M68k_jsr(): M68k("jsr", m68k_family,
                     OP(4) & BITS<6, 11>(0x3a) & EAM(m68k_eam_control)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *ea = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 0);
        SgAsmExpression *target = d->makeAddress(ea);
        ASSERT_not_null2(target, "JSR instruction must have a memory-referencing operand");
        return d->makeInstruction(m68k_jsr, "jsr", target);
    }
};

// LEA.L <ea>y, Ax
struct M68k_lea: M68k {
    M68k_lea(): M68k("lea", m68k_family,
                     OP(4) & BITS<6, 8>(7) & EAM(m68k_eam_control)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 0);
        SgAsmExpression *dst = d->makeAddressRegister(extract<9, 11>(w0), m68k_fmt_i32);
        return d->makeInstruction(m68k_lea, "lea.l", src, dst);
    }
};

// LINK.W An, #<displacement>
struct M68k_link_w: M68k {
    M68k_link_w(): M68k("link_w", m68k_family,
                        OP(4) & BITS<3, 11>(0x1ca)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *rx = d->makeAddressRegister(extract<0, 2>(w0), m68k_fmt_i32);
        SgAsmExpression *dsp = d->makeImmediateExtension(m68k_fmt_i16, 0);
        return d->makeInstruction(m68k_link, "link.w", rx, dsp);
    }
};

// LINK.L An, #<displacement>
struct M68k_link_l: M68k {
    M68k_link_l(): M68k("link_l", m68k_family,
                        OP(4) & BITS<3, 11>(0x101)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *rx = d->makeAddressRegister(extract<0, 2>(w0), m68k_fmt_i32);
        SgAsmExpression *dsp = d->makeImmediateExtension(m68k_fmt_i32, 0);
        return d->makeInstruction(m68k_link, "link.l", rx, dsp);
    }
};

// LSL.B Dy, Dx         as in (L)ogical (S)hift (L)eft
// LSL.W Dy, Dx
// LSL.L Dy, Dx
// LSR.B Dy, Dx
// LSR.W Dy, Dx
// LSR.L Dy, Dx
// Only register shifts, not memory
struct M68k_lshift_rr: M68k {
    M68k_lshift_rr(): M68k("lshift_rr", m68k_family,
                           OP(14) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 5>(5)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *dy = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i32, 0);
        SgAsmExpression *dx = d->makeDataRegister(extract<0, 2>(w0), fmt, 0);
        M68kInstructionKind kind = extract<8, 8>(w0) ? m68k_lsl : m68k_lsr;
        std::string mnemonic = stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_") + "." + formatLetter(fmt);
        return d->makeInstruction(kind, mnemonic, dy, dx);
    }
};

// LSL.B #<nbits>, Dx   as in (L)ogical (S)hift (L)eft
// LSL.W #<nbits>, Dx
// LSL.L #<nbits>, Dx
// LSR.B #<nbits>, Dx
// LSR.W #<nbits>, Dx
// LSR.L #<nbits>, Dx
// Only register shifts, not memory
struct M68k_lshift_ir: M68k {
    M68k_lshift_ir(): M68k("lshift_ir", m68k_family,
                           OP(14) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 5>(1)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        unsigned n = extract<9, 11>(w0);
        SgAsmExpression *sa = d->makeImmediateValue(m68k_fmt_i8, 0==n ? 8 : n);
        SgAsmExpression *dx = d->makeDataRegister(extract<0, 2>(w0), fmt, 0);
        M68kInstructionKind kind = extract<8, 8>(w0) ? m68k_lsl : m68k_lsr;
        std::string mnemonic = stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_") + "." + formatLetter(fmt);
        return d->makeInstruction(kind, mnemonic, sa, dx);
    }
};

// LSL.W <ea>         as in (L)ogical (S)hift (L)eft
// LSR.W <ea>
// Only memory shifts, not registers
struct M68k_lshift_mem: M68k {
    M68k_lshift_mem(): M68k("lshift_mem", m68k_family,
                            OP(14) & BITS<9, 11>(1) & BITS<6, 7>(3) &
                            EAM(m68k_eam_memory & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *ea = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i16, 0);
        M68kInstructionKind kind = extract<8, 8>(w0) ? m68k_lsl : m68k_lsr;
        std::string mnemonic = stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_") + ".w";
        return d->makeInstruction(kind, mnemonic, ea);
    }
};

// Multiply accumulate (4 arguments)
// MAC.W Ry, Rx, SF, ACCx
// MAC.L Ry, Rx, SF, ACCx
struct M68k_mac: M68k {
    M68k_mac(): M68k("mac", m68k_freescale_emac,
                     OP(10) & BIT<8>(0) & BITS<4, 5>(0) &
                     INSN_WORD<1>((BITS<9, 10>(0) | BITS<9, 10>(1) | BITS<9, 10>(3)) & BIT<8>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *ry=NULL, *rx=NULL;
        unsigned ryNumber = extract<0, 3>(w0);
        unsigned rxNumber = (extract<6, 6>(w0) << 3) | extract<9, 11>(w0);
        M68kDataFormat fmt = m68k_fmt_unknown;
        if (extract<11, 11>(d->instructionWord(1))) {
            fmt = m68k_fmt_i32;
            ry = d->makeDataAddressRegister(ryNumber, fmt, 0);
            rx = d->makeDataAddressRegister(rxNumber, fmt, 0);
        } else {
            fmt = m68k_fmt_i16;
            ry = d->makeDataAddressRegister(ryNumber, fmt, extract<6, 6>(d->instructionWord(1)) ? 16 : 0);
            rx = d->makeDataAddressRegister(rxNumber, fmt, extract<7, 7>(d->instructionWord(1)) ? 16 : 0);
        }
        SgAsmExpression *sf = d->makeImmediateValue(m68k_fmt_i8, extract<9, 10>(d->instructionWord(1)));
        switch (extract<9, 10>(d->instructionWord(1))) {
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
        unsigned accNumber = (extract<4, 4>(d->instructionWord(1)) << 1) | extract<7, 7>(w0);
        SgAsmExpression *acc = d->makeMacAccumulatorRegister(accNumber);
        return d->makeInstruction(m68k_mac, "mac."+formatLetter(fmt), ry, rx, sf, acc);
    }
};

// Multiply accumulate with load (7 arguments)
// MAC.L Ry, Rx, SF, <ea>y&, Rw
//   ACC += scale(Ry * Rx); Rw = mask(ea<y>)    where scale and mask are optional
//   The U/Lx (bit w1.7) and U/Ly (bit w1.6) are not used for this 32-bit instruction
struct M68k_mac_l2: M68k {
    M68k_mac_l2(): M68k("mac_l2", m68k_freescale_mac,
                        OP(10) & BITS<7, 8>(1) & EAM(m68k_eam_ari|m68k_eam_inc|m68k_eam_dec|m68k_eam_dsp) &
                        INSN_WORD<1>(BIT<11>(1) & BIT<8>(0) & BIT<4>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        // Multiplicand 1
        unsigned ryRegisterNumber = extract<0, 3>(d->instructionWord(1));
        SgAsmExpression *ry = d->makeDataAddressRegister(ryRegisterNumber, m68k_fmt_i32, 0);

        // Multiplicand 2
        unsigned rxRegisterNumber = extract<12, 15>(d->instructionWord(1));
        SgAsmExpression *rx = d->makeDataAddressRegister(rxRegisterNumber, m68k_fmt_i32, 0);

        // How to scale product: 0=>none; 1=>multiply by 2; 2=>reserved; 3=>divide by 2
        SgAsmExpression *scaleFactor = d->makeImmediateValue(m68k_fmt_i8, extract<9, 10>(d->instructionWord(1)));

        // Accumulator register
        SgAsmExpression *accRegister = NULL;
        unsigned accNumber = (extract<4, 4>(d->instructionWord(1))<<1) | extract<7, 7>(w0);
        switch (accNumber) {
            case 0: accRegister = d->makeMacRegister(m68k_mac_acc1); break;
            case 1: accRegister = d->makeMacRegister(m68k_mac_acc0); break;
            case 2: accRegister = d->makeMacRegister(m68k_mac_acc3); break;
            case 3: accRegister = d->makeMacRegister(m68k_mac_acc2); break;
        }

        // Source loaded into Rw in parallel with ACC += product
        SgAsmExpression *source = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 1);

        // Whether to use the MASK register when loading the source into Rw
        SgAsmExpression *useMask = d->makeImmediateValue(m68k_fmt_i8, extract<5, 5>(d->instructionWord(1)));

        // Destination register for loading source
        unsigned rwRegisterNumber = (extract<6, 6>(w0)<<3) | extract<9, 11>(w0);
        SgAsmExpression *rw = d->makeDataAddressRegister(rwRegisterNumber, m68k_fmt_i32, 0);

        return d->makeInstruction(m68k_mac, "mac.l", ry, rx, scaleFactor, accRegister, source, useMask, rw);
    }
};

// Multiply accumulate with load (7 arguments)
// MAC.W Ry.{U,L}, Rx.{U,L} SF, <ea>y[&], Rw
//   ACC += scale(Ry * Rx); Rw = mask(ea<y>)    where scale and mask are optional
struct M68k_mac_w2: M68k {
    M68k_mac_w2(): M68k("mac_w2", m68k_freescale_mac,
                        OP(10) & BITS<7, 8>(1) & EAM(m68k_eam_ari|m68k_eam_inc|m68k_eam_dec|m68k_eam_dsp) &
                        INSN_WORD<1>(BIT<11>(0) & BIT<8>(0) & BIT<4>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        // Multiplicand 1
        size_t ryOffset = extract<6, 6>(d->instructionWord(1)) ? 16 : 0;
        unsigned ryRegisterNumber = extract<0, 3>(d->instructionWord(1));
        SgAsmExpression *ry = d->makeDataAddressRegister(ryRegisterNumber, m68k_fmt_i16, ryOffset);

        // Multiplicand 2
        size_t rxOffset = extract<7, 7>(d->instructionWord(1)) ? 16 : 0;
        unsigned rxRegisterNumber = extract<12, 15>(d->instructionWord(1));
        SgAsmExpression *rx = d->makeDataAddressRegister(rxRegisterNumber, m68k_fmt_i16, rxOffset);

        // How to scale product: 0=>none; 1=>multiply by 2; 2=>reserved; 3=>divide by 2
        SgAsmExpression *scaleFactor = d->makeImmediateValue(m68k_fmt_i8, extract<9, 10>(d->instructionWord(1)));

        // Accumulator register
        SgAsmExpression *accRegister = NULL;
        unsigned accNumber = (extract<4, 4>(d->instructionWord(1))<<1) | extract<7, 7>(w0);
        switch (accNumber) {
            case 0: accRegister = d->makeMacRegister(m68k_mac_acc1); break;
            case 1: accRegister = d->makeMacRegister(m68k_mac_acc0); break;
            case 2: accRegister = d->makeMacRegister(m68k_mac_acc3); break;
            case 3: accRegister = d->makeMacRegister(m68k_mac_acc2); break;
        }

        // Source loaded into Rw in parallel with ACC += product
        SgAsmExpression *source = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 1);

        // Whether to use the MASK register when loading the source into Rw
        SgAsmExpression *useMask = d->makeImmediateValue(m68k_fmt_i8, extract<5, 5>(d->instructionWord(1)));

        // Destination register for loading source
        unsigned rwRegisterNumber = (extract<6, 6>(w0)<<3) | extract<9, 11>(w0);
        SgAsmExpression *rw = d->makeDataAddressRegister(rwRegisterNumber, m68k_fmt_i32, 0);

        return d->makeInstruction(m68k_mac, "mac.w", ry, rx, scaleFactor, accRegister, source, useMask, rw);
    }
};

// MOV3Q.L #<data>, <ea>x
struct M68k_mov3q: M68k {
    M68k_mov3q(): M68k("mov3q", m68k_freescale_isab,
                       OP(10) & BITS<6, 8>(5) &
                       EAM(m68k_eam_alter & ~m68k_eam_234)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        int32_t val = signExtend<3, 32>((int32_t)extract<9, 11>(w0));
        if (0==val)
            val = -1;
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 0);
        return d->makeInstruction(m68k_mov3q, "mov3q.l", d->makeImmediateValue(m68k_fmt_i32, val), dst);
    }
};

// MOVCLR.L ACCy, Rx
struct M68k_movclr: M68k {
    M68k_movclr(): M68k("movclr", m68k_freescale_emac,
                        OP(10) & BIT<11>(0) & BITS<4, 8>(0x1c)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *acc = d->makeMacAccumulatorRegister(extract<9, 10>(w0));
        SgAsmExpression *dst = d->makeDataAddressRegister(extract<0, 3>(w0), m68k_fmt_i32, 0);
        return d->makeInstruction(m68k_movclr, "movclr.l", acc, dst);
    }
};

// MOVE.B <ea>y, <ea>x
// MOVE.W <ea>y, <ea>x
// MOVE.L <ea>y, <ea>x
struct M68k_move: M68k {
    M68k_move(): M68k("move", m68k_family,
                      (OP(1) | OP(2) | OP(3)) &
                      EAM_BACKWARD(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi, 6) &
                      EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = m68k_fmt_unknown;
        switch (extract<12, 13>(w0)) {
            case 1: fmt = m68k_fmt_i8; break;
            case 2: fmt = m68k_fmt_i32; break;                  // yes, not the usual order
            case 3: fmt = m68k_fmt_i16; break;
        }
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<6, 8>(w0), extract<9, 11>(w0), fmt, d->extensionWordsUsed());
        return d->makeInstruction(m68k_move, "move."+formatLetter(fmt), src, dst);
    }
};

// MOVE.L ACC, Rx
struct M68k_move_from_acc: M68k {
    M68k_move_from_acc(): M68k("move_from_acc", m68k_freescale_mac,
                               OP(10) & BIT<11>(0) & BITS<4, 8>(0x18)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        unsigned accumNumber = extract<9, 10>(w0);
        M68kMacRegister accumulatorMinor = (M68kMacRegister)(m68k_mac_acc0 + accumNumber);
        return d->makeInstruction(m68k_move_acc, "move.l",
                                  d->makeMacRegister(accumulatorMinor),
                                  d->makeDataAddressRegister(extract<0, 3>(w0), m68k_fmt_i32));
    }
};

// MOVE.L ACCext01, Rx
// MOVE.L ACCext23, Rx
struct M68k_move_from_accext: M68k {
    M68k_move_from_accext(): M68k("move_from_accext", m68k_freescale_emac,
                                  OP(10) & BIT<11>(1) & BITS<4, 9>(0x38)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kMacRegister extreg = extract<10, 10>(w0) ? m68k_mac_ext23 : m68k_mac_ext01;
        return d->makeInstruction(m68k_move_accext, "move.l",
                                  d->makeMacRegister(extreg),
                                  d->makeDataAddressRegister(extract<0, 3>(w0), m68k_fmt_i32));
    }
};

// MOVE.W CCR, Dx
struct M68k_move_from_ccr: M68k {
    M68k_move_from_ccr(): M68k("move_from_ccr", m68k_68010|m68k_68020|m68k_68030|m68k_68040|m68k_freescale_cpu32,
                               OP(4) & BITS<6, 11>(0x0b) &
                               EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeConditionCodeRegister();
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i16, 0);
        return d->makeInstruction(m68k_move_ccr, "move.w", src, dst);
    }
};

// MOVE.L MACSR, Rx
struct M68k_move_from_macsr: M68k {
    M68k_move_from_macsr(): M68k("move_from_macsr", m68k_freescale_mac,
                                 OP(10) & BITS<4, 11>(0x98)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_move_macsr, "move.l",
                                  d->makeMacRegister(m68k_mac_macsr),
                                  d->makeDataAddressRegister(extract<0, 3>(w0), m68k_fmt_i32));
    }
};

// MOVE.L MASK, Rx
struct M68k_move_from_mask: M68k {
    M68k_move_from_mask(): M68k("move_from_mask", m68k_freescale_mac,
                                OP(10) & BITS<4, 11>(0xd8)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_move_mask, "move.l",
                                  d->makeMacRegister(m68k_mac_mask),
                                  d->makeDataAddressRegister(extract<0, 3>(w0), m68k_fmt_i32));
    }
};

// MOVE.W SR, <ea>x
struct M68k_move_from_sr: M68k {
    M68k_move_from_sr(): M68k("move_from_sr", m68k_68000|m68k_68008,
                              OP(4) & BITS<6, 11>(0x03) & EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeStatusRegister();
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i16, 0);
        return d->makeInstruction(m68k_move_sr, "move.w", src, dst);
    }
};

// MOVE.L Ry, ACC
// MOVE.L #<data>, ACC
struct M68k_move_to_acc: M68k {
    M68k_move_to_acc(): M68k("move_to_acc", m68k_freescale_mac,
                             OP(10) & BIT<11>(0) & BITS<6, 8>(4) & EAM(m68k_eam_direct|m68k_eam_imm)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        unsigned accumNumber = extract<9, 10>(w0);
        M68kMacRegister accumulatorMinor = (M68kMacRegister)(m68k_mac_acc0 + accumNumber);
        return d->makeInstruction(m68k_move_acc, "move.l",
                                  d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 0),
                                  d->makeMacRegister(accumulatorMinor));
    }
};

// MOVE.L Ry, ACCext01
// MOVE.L Ry, ACCext23
struct M68k_move_to_accext: M68k {
    M68k_move_to_accext(): M68k("move_to_accext", m68k_freescale_emac,
                                OP(10) & BIT<11>(1) & BITS<6, 9>(0xc) & EAM(m68k_eam_direct|m68k_eam_imm)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kMacRegister extreg = extract<10, 10>(w0) ? m68k_mac_ext23 : m68k_mac_ext01;
        return d->makeInstruction(m68k_move_accext, "move.l",
                                  d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 0),
                                  d->makeMacRegister(extreg));
    }
};

// MOVE.W <ea>y, CCR
struct M68k_move_to_ccr: M68k {
    M68k_move_to_ccr(): M68k("move_to_ccr", m68k_family,
                             OP(4) & BITS<6, 11>(0x13) & EAM(m68k_eam_data)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i16, 0);
        SgAsmExpression *dst = d->makeConditionCodeRegister();
        return d->makeInstruction(m68k_move_ccr, "move.w", src, dst);
    }
};

// MOVE.L Ry, MACSR
// MOVE.L #<data>, MACSR
struct M68k_move_to_macsr: M68k {
    M68k_move_to_macsr(): M68k("move_to_macsr", m68k_freescale_mac,
                               OP(10) & BITS<6, 11>(0x24) & EAM(m68k_eam_direct|m68k_eam_imm)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_move_macsr, "move.l",
                                  d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 0),
                                  d->makeMacRegister(m68k_mac_macsr));
    }
};

// MOVE.L Ry, MASK
// MOVE.L #<data>, MASK
struct M68k_move_to_mask: M68k {
    M68k_move_to_mask(): M68k("move_to_mask", m68k_freescale_mac,
                              OP(10) & BITS<6, 11>(0x34) & EAM(m68k_eam_direct|m68k_eam_imm)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_move_mask, "move.l",
                                  d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 0),
                                  d->makeMacRegister(m68k_mac_mask));
    }
};

// MOVE.W <ea>, SR
struct M68k_move_to_sr: M68k {
    M68k_move_to_sr(): M68k("move_to_sr", m68k_family,
                            OP(4) & BITS<6, 11>(0x1b) & EAM(m68k_eam_drd|m68k_eam_imm)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_move_sr, "move.w",
                                  d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i16, 0),
                                  d->makeStatusRegister());
    }
};

// MOVE16 (Ax)+, (Ay)+
struct M68k_move16_pp: M68k {
    M68k_move16_pp(): M68k("move16_pp", m68k_68040,
                           OP(15) & BITS<3, 11>(0xc4) & INSN_WORD<1>(BIT<15>(1) & BITS<0, 11>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeAddressRegisterPostIncrement(extract<0, 2>(w0), m68k_fmt_i32);
        SgAsmExpression *dst = d->makeAddressRegisterPostIncrement(extract<12, 14>(d->instructionWord(1)), m68k_fmt_i32);
        return d->makeInstruction(m68k_move16, "move16", src, dst);
    }
};

// MOVE16 (xxx).L, (An)+
// MOVE16 (An)+, (xxx).L
// MOVE16 (xxx).L, (An)
// MOVE16 (An), (xxx).L
struct M68k_move16_a: M68k {
    M68k_move16_a(): M68k("move16_a", m68k_68040,
                          OP(15) & BITS<5, 11>(0x30)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src=NULL, *dst=NULL;
        switch (extract<3, 4>(w0)) {
            case 0:                                     // MOVE16 (An)+, (xxx).L
                src = d->makeEffectiveAddress(030|extract<0, 2>(w0), m68k_fmt_i32, 0);
                dst = d->makeEffectiveAddress(071, m68k_fmt_i32, 0);
                break;
            case 1:                                     // MOVE16 (xxx).L, (An)+
                src = d->makeEffectiveAddress(071, m68k_fmt_i32, 0);
                dst = d->makeEffectiveAddress(030|extract<0, 2>(w0), m68k_fmt_i32, 0);
                break;
            case 2:                                     // MOVE16 (An), (xxx).L
                src = d->makeEffectiveAddress(020|extract<0, 2>(w0), m68k_fmt_i32, 0);
                dst = d->makeEffectiveAddress(071, m68k_fmt_i32, 0);
                break;
            case 3:                                     // MOVE16 (xxx).L, (An)
                src = d->makeEffectiveAddress(071, m68k_fmt_i32, 0);
                dst = d->makeEffectiveAddress(020|extract<0, 2>(w0), m68k_fmt_i32, 0);
                break;
        }
        return d->makeInstruction(m68k_move16, "move16", src, dst);
    }
};

// MOVEA.W <ea>y, Ax
// MOVEA.L <ea>y, Ax
struct M68k_movea: M68k {
    M68k_movea(): M68k("movea", m68k_family,
                       (OP(2)|OP(3)) & EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = 2==extract<12, 13>(w0) ? m68k_fmt_i32 : m68k_fmt_i16;
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeAddressRegister(extract<9, 11>(w0), m68k_fmt_i32);
        return d->makeInstruction(m68k_movea, "movea."+formatLetter(fmt), src, dst);
    }
};

// MOVEC.L <ea>y, Rc
struct M68k_move_to_cr: M68k {
    M68k_move_to_cr(): M68k("move_to_cr", m68k_family,
                            OP(4) & BITS<0, 11>(0xe7b)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataAddressRegister(extract<12, 15>(d->instructionWord(1)), m68k_fmt_i32, 0);
        SgAsmExpression *dst = d->makeColdFireControlRegister(extract<0, 11>(d->instructionWord(1)));
        return d->makeInstruction(m68k_movec, "movec.l", src, dst);
    }
};

// MOVEM.W <ea>y, #list
// MOVEM.L <ea>y, #list
struct M68k_movem_mr: M68k {
    M68k_movem_mr(): M68k("movem_mr", m68k_family,
                          OP(4) & BITS<7, 11>(0x19) & EAM(m68k_eam_control | m68k_eam_inc)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = extract<6, 6>(w0) ? m68k_fmt_i32 : m68k_fmt_i16;
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        SgAsmExpression *dst = d->makeRegistersFromMask(d->instructionWord(1), m68k_fmt_i32);
        return d->makeInstruction(m68k_movem, "movem."+formatLetter(fmt), src, dst);
    }
};

// MOVEM.W #list, <ea>x
// MOVEM.L #list, <ea>x
struct M68k_movem_rm: M68k {
    M68k_movem_rm(): M68k("movem_rm", m68k_family,
                          OP(4) & BITS<7, 11>(0x11) &
                          EAM((m68k_eam_control & m68k_eam_alter & ~m68k_eam_pcmi) | m68k_eam_dec)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = extract<6, 6>(w0) ? m68k_fmt_i32 : m68k_fmt_i16;
        bool isPreDecrement = extract<3, 5>(w0) == 4;
        SgAsmRegisterNames *src = d->makeRegistersFromMask(d->instructionWord(1), m68k_fmt_i32, isPreDecrement);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 1);
        return d->makeInstruction(m68k_movem, "movem."+formatLetter(fmt), src, dst);
    }
};
                
// MOVEP.W Dx, (d16,Ay)
// MOVEP.L Dx, (d16,Ay)
// MOVEP.W (d16,Ay), Dx
// MOVEP.L (d16,Ay), Dx
struct M68k_movep: M68k {
    M68k_movep(): M68k("movep", m68k_family,
                       OP(0) & BIT<8>(1) & BITS<3, 5>(1)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = extract<6, 6>(w0) ? m68k_fmt_i32 : m68k_fmt_i16;
        SgAsmExpression *src = d->makeEffectiveAddress(050|extract<0, 2>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), fmt);
        if (extract<7, 7>(w0))
            std::swap(src, dst);                        // register-to-memory
        return d->makeInstruction(m68k_movep, "movep."+formatLetter(fmt), src, dst);
    }
};

// MOVEQ.L #<data>, Dx
struct M68k_moveq: M68k {
    M68k_moveq(): M68k("moveq", m68k_family,
                       OP(7) & BIT<8>(0)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateValue(m68k_fmt_i32, signExtend<8, 32>(extract<0, 7>(w0)));
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i32);
        return d->makeInstruction(m68k_moveq, "moveq.l", src, dst);
    }
};

// MSAC.W Ry, Rx, SF, ACCx
// MSAC.L Ry, Rx, SF, ACCx
struct M68k_msac: M68k {
    M68k_msac(): M68k("msac", m68k_freescale_emac,
                      OP(10) & BIT<8>(0) & BITS<4, 5>(0) &
                      INSN_WORD<1>((BITS<9, 10>(0) | BITS<9, 10>(1) | BITS<9, 10>(3)) & BIT<8>(1))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *ry=NULL, *rx=NULL;
        unsigned ryNumber = extract<0, 3>(w0);
        unsigned rxNumber = (extract<6, 6>(w0) << 3) | extract<9, 11>(w0);
        M68kDataFormat fmt = m68k_fmt_unknown;
        if (extract<11, 11>(d->instructionWord(1))) {
            fmt = m68k_fmt_i32;
            ry = d->makeDataAddressRegister(ryNumber, fmt, 0);
            rx = d->makeDataAddressRegister(rxNumber, fmt, 0);
        } else {
            fmt = m68k_fmt_i16;
            ry = d->makeDataAddressRegister(ryNumber, fmt, extract<6, 6>(d->instructionWord(1)) ? 16 : 0);
            rx = d->makeDataAddressRegister(rxNumber, fmt, extract<7, 7>(d->instructionWord(1)) ? 16 : 0);
        }
        SgAsmExpression *sf = d->makeImmediateValue(m68k_fmt_i8, extract<9, 10>(d->instructionWord(1)));
        switch (extract<9, 10>(d->instructionWord(1))) {
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
        unsigned accNumber = (extract<4, 4>(d->instructionWord(1)) << 1) | extract<7, 7>(w0);
        SgAsmExpression *acc = d->makeMacAccumulatorRegister(accNumber);
        return d->makeInstruction(m68k_msac, "msac."+formatLetter(fmt), ry, rx, sf, acc);
    }
};

// MULS.W <ea>y, Dx             16 x 16 -> 32
struct M68k_muls_w: M68k {
    M68k_muls_w(): M68k("muls_w", m68k_family,
                        OP(12) & BITS<6, 8>(7) & EAM(m68k_eam_data)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *a0 = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i16, 0);
        SgAsmExpression *a1 = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i16);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i32);
        return d->makeInstruction(m68k_muls, "muls.w", a0, a1, dst);
    }
};

// MULS.L <ea>y, Dx             32 x 32 -> 32
// MULU.L <ea>y, Dx             32 x 32 -> 32
struct M68k_multiply_l: M68k {
    M68k_multiply_l(): M68k("multiply_l", m68k_68020|m68k_68030|m68k_68040|m68k_freescale_cpu32,
                            OP(4) & BITS<6, 11>(0x30) & EAM(m68k_eam_data) &
                            INSN_WORD<1>(BIT<15>(0) & BITS<3, 10>(0))) {} // BITS<0,2> are unused and unspecified
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kInstructionKind kind = extract<11, 11>(d->instructionWord(1)) ? m68k_muls : m68k_mulu;
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 1);
        SgAsmExpression *dst = d->makeDataRegister(extract<12, 14>(d->instructionWord(1)), m68k_fmt_i32);
        return d->makeInstruction(kind, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+".l", src, dst);
    }
};

// MULS.L <ea>y, Dh, Dl         32 x 32 -> 64
// MULU.L <ea>y, Dh, DL         32 x 32 -> 64
struct M68k_multiply_64: M68k {
    M68k_multiply_64(): M68k("multiply_64", m68k_68020|m68k_68030|m68k_68040|m68k_freescale_cpu32,
                             OP(4) & BITS<6, 11>(0x30) & EAM(m68k_eam_data) &
                             INSN_WORD<1>(BIT<15>(0) & BITS<3, 10>(0x80))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kInstructionKind kind = extract<11, 11>(d->instructionWord(1)) ? m68k_muls : m68k_mulu;
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 1);
        SgAsmExpression *dh = d->makeDataRegister(extract<0, 2>(d->instructionWord(1)), m68k_fmt_i32);
        SgAsmExpression *dl = d->makeDataRegister(extract<12, 14>(d->instructionWord(1)), m68k_fmt_i32);
        return d->makeInstruction(m68k_muls, stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_")+".l", src, dl, dh);
    }
};

// MULU.W <ea>y, Dx             16 x 16 -> 32
struct M68k_mulu_w: M68k {
    M68k_mulu_w(): M68k("mulu_w", m68k_family,
                        OP(12) & BITS<6, 8>(3) & EAM(m68k_eam_data)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *a0 = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i16, 0);
        SgAsmExpression *a1 = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i16);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i32);
        return d->makeInstruction(m68k_mulu, "mulu.w", a0, a1, dst);
    }
};

// MVS.B <ea>y, Dx
// MVS.W <ea>y, Dx
struct M68k_mvs: M68k {
    M68k_mvs(): M68k("mvs", m68k_freescale_isab,
                     OP(7) & BITS<7, 8>(2) & EAM(m68k_eam_all & ~m68k_eam_234)) {}

    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = extract<6, 6>(w0) ? m68k_fmt_i16 : m68k_fmt_i8;
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i32);
        return d->makeInstruction(m68k_mvs, "mvs."+formatLetter(fmt), src, dst);
    }
};

// MVZ.B <ea>y, Dx
// MVZ.W <ea>y, Dx
struct M68k_mvz: M68k {
    M68k_mvz(): M68k("mvz", m68k_freescale_isab,
                     OP(7) & BITS<7, 8>(3) & EAM(m68k_eam_all & ~m68k_eam_234)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = extract<6, 6>(w0) ? m68k_fmt_i16 : m68k_fmt_i8;
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i32);
        return d->makeInstruction(m68k_mvz, "mvz."+formatLetter(fmt), src, dst);
    }
};

// NBCD.B <ea>
struct M68k_nbcd: M68k {
    M68k_nbcd(): M68k("nbcd", m68k_family,
                      OP(8) & BITS<6, 11>(0x20) & EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *ea = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i8, 0);
        return d->makeInstruction(m68k_nbcd, "nbcd.b", ea);
    }
};

// NEG.B <ea>
// NEG.W <ea>
// NEG.L <ea>
struct M68k_neg: M68k {
    M68k_neg(): M68k("neg", m68k_family,
                     OP(4) & BITS<8, 11>(4) & (BITS<6, 7>(0) |BITS<6, 7>(1) | BITS<6, 7>(2)) &
                     EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *ea = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(m68k_neg, "neg."+formatLetter(fmt), ea);
    }
};

// NEGX.B <ea>
// NEGX.W <ea>
// NEGX.L <ea>
struct M68k_negx: M68k {
    M68k_negx(): M68k("negx", m68k_family,
                      OP(4) & BITS<8, 11>(0) & (BITS<6, 7>(0) |BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *ea = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(m68k_negx, "negx."+formatLetter(fmt), ea);
    }
};

// NOP
struct M68k_nop: M68k {
    M68k_nop(): M68k("nop", m68k_family,
                     OP(4) & BITS<0, 11>(0xe71)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_nop, "nop");
    }
};

// NOT.B <ea>
// NOT.W <ea>
// NOT.L <ea>
struct M68k_not: M68k {
    M68k_not(): M68k("not", m68k_family,
                     OP(4) & BITS<8, 11>(6) & (BITS<6, 7>(0) |BITS<6, 7>(1) | BITS<6, 7>(2)) &
                     EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *ea = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(m68k_not, "not."+formatLetter(fmt), ea);
    }
};

// OR.B <ea>y, Dx
// OR.W <ea>y, Dx
// OR.L <ea>y, Dx
struct M68k_or_1: M68k {
    M68k_or_1(): M68k("or_1", m68k_family,
                      OP(8) & BIT<8>(0) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & EAM(m68k_eam_data)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), fmt);
        return d->makeInstruction(m68k_or, "or."+formatLetter(fmt), src, dst);
    }
};

// OR.B Dy, <ea>x
// OR.W Dy, <ea>x
// OR.L Dy, <ea>x
struct M68k_or_2: M68k {
    M68k_or_2(): M68k("or_2", m68k_family,
                      OP(8) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(m68k_eam_memory & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), fmt);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(m68k_or, "or."+formatLetter(fmt), src, dst);
    }
};

// ORI.B #<data>, <ea>
// ORI.W #<data>, <ea>
// ORI.L #<data>, <ea>
struct M68k_ori: M68k {
    M68k_ori(): M68k("ori", m68k_family,
                     OP(0) & BITS<8, 11>(0) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                     EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        size_t opandWords = (formatNBits(fmt)+15) / 16;
        SgAsmExpression *src = d->makeImmediateExtension(fmt, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 2>(w0), fmt, opandWords);
        return d->makeInstruction(m68k_ori, "ori."+formatLetter(fmt), src, dst);
    }
};

// ORI.B #<data>, CCR
struct M68k_ori_to_ccr: M68k {
    M68k_ori_to_ccr(): M68k("ori_to_ccr", m68k_family,
                            OP(0) & BITS<0, 11>(0x03c) & INSN_WORD<1>(BITS<8, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateValue(m68k_fmt_i8, extract<0, 7>(d->instructionWord(1)));
        SgAsmExpression *dst = d->makeConditionCodeRegister();
        return d->makeInstruction(m68k_ori, "ori.b", src, dst);
    }
};

// PACK -(Ax), -(Ay), #<adjustment>
// PACK Dx, Dy, #<adjustment>
struct M68k_pack: M68k {
    M68k_pack(): M68k("pack", m68k_68020|m68k_68030|m68k_68040,
                      OP(8) & BITS<4, 8>(0x14)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src=NULL, *dst=NULL;
        if (extract<3, 3>(w0)) {
            // PACK -(Ax), -(Ay)
            src = d->makeAddressRegisterPreDecrement(extract<0, 2>(w0), m68k_fmt_i16);
            dst = d->makeAddressRegisterPreDecrement(extract<9, 11>(w0), m68k_fmt_i16);
        } else {
            // PACK Dx, Dy
            src = d->makeDataRegister(extract<0, 2>(w0), m68k_fmt_i16, 0);
            dst = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i16, 0);
        }
        SgAsmExpression *adj = d->makeImmediateExtension(m68k_fmt_i16, 0);
        return d->makeInstruction(m68k_pack, "pack", src, dst, adj);
    }
};

// PEA.L <ea>y
struct M68k_pea: M68k {
    M68k_pea(): M68k("pea", m68k_family,
                     OP(4) & BITS<6, 11>(0x21) & EAM(m68k_eam_control)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i32, 0);
        return d->makeInstruction(m68k_pea, "pea.l", src);
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
struct M68k_rotate_reg: M68k {
    M68k_rotate_reg(): M68k("rotate_reg", m68k_family,
                            OP(14) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 4>(3)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        unsigned n = extract<9, 11>(w0);                // "count/regisger"
        SgAsmExpression *sa = NULL;
        if (extract<5, 5>(w0)) {                        // "i/r"
            sa = d->makeDataRegister(n, m68k_fmt_i32, 0);
        } else {
            sa = d->makeImmediateValue(m68k_fmt_i8, 0==n ? 8 : n);
        }
        SgAsmExpression *dy = d->makeDataRegister(extract<0, 2>(w0), fmt, 0);
        M68kInstructionKind kind = extract<8, 8>(w0) ? m68k_rol : m68k_ror; // "dr"
        std::string mnemonic = stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_") + "." + formatLetter(fmt);
        return d->makeInstruction(kind, mnemonic, sa, dy);
    }
};

// ROL.W <ea>y          memory rotate left
// ROR.W <ea>y          memory rotate right
struct M68k_rotate_mem: M68k {
    M68k_rotate_mem(): M68k("rotate_mem", m68k_family,
                            OP(14) & BITS<9, 11>(3) & BITS<6, 7>(3)
                            & EAM(m68k_eam_memory & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *eay = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i16, 0);
        M68kInstructionKind kind = extract<8, 8>(w0) ? m68k_rol : m68k_ror; // "dr"
        std::string mnemonic = stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_") + ".w";
        return d->makeInstruction(kind, mnemonic, eay);
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
struct M68k_rotate_extend_reg: M68k {
    M68k_rotate_extend_reg(): M68k("rotate_extend_reg", m68k_family,
                                   OP(14) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 4>(2)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        unsigned n = extract<9, 11>(w0);                // "count/register"
        SgAsmExpression *sa = NULL;
        if (extract<5, 5>(w0)) {                        // "i/r"
            sa = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i32, 0);
        } else {
            sa = d->makeImmediateValue(m68k_fmt_i8, 0==n ? 8 : n);
        }
        SgAsmExpression *dy = d->makeDataRegister(extract<0, 2>(w0), fmt, 0);
        M68kInstructionKind kind = extract<8, 8>(w0) ? m68k_roxl : m68k_roxr; // "dr"
        std::string mnemonic = stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_") + "." + formatLetter(fmt);
        return d->makeInstruction(kind, mnemonic, sa, dy);
    }
};

// ROXL.W <ea>y          memory rotate left
// ROXR.W <ea>y          memory rotate right
struct M68k_rotate_extend_mem: M68k {
    M68k_rotate_extend_mem(): M68k("rotate_extend_mem", m68k_family,
                                   OP(14) & BITS<9, 11>(2) & BITS<6, 7>(3)
                                   & EAM(m68k_eam_memory & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *eay = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i16, 0);
        M68kInstructionKind kind = extract<8, 8>(w0) ? m68k_roxl : m68k_roxr; // "dr"
        std::string mnemonic = stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_") + ".w";
        return d->makeInstruction(kind, mnemonic, eay);
    }
};

// RTD #<displacement>
struct M68k_rtd: M68k {
    M68k_rtd(): M68k("rtd", m68k_68010|m68k_68020|m68k_68030|m68k_68040|m68k_freescale_cpu32,
                     OP(4) & BITS<0, 11>(0xe74)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *disp = d->makeImmediateExtension(m68k_fmt_i16, 0);
        return d->makeInstruction(m68k_rtd, "rtd", disp);
    }
};

// RTM Rn
struct M68k_rtm: M68k {
    M68k_rtm(): M68k("rtm", m68k_68020,
                     OP(0) & BITS<4, 11>(0x6c)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *rn = d->makeDataAddressRegister(extract<0, 3>(w0), m68k_fmt_i32, 0);
        return d->makeInstruction(m68k_rtm, "rtm", rn);
    }
};

// RTR
struct M68k_rtr: M68k {
    M68k_rtr(): M68k("rtr", m68k_family,
                     OP(4) & BITS<0, 11>(0xe77)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_rtr, "rtr");
    }
};

// RTS
struct M68k_rts: M68k {
    M68k_rts(): M68k("rts", m68k_family,
                     OP(4) & BITS<0, 11>(0xe75)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_rts, "rts");
    }
};

// SBCD.B Dx, Dy
// SBCD.B -(Ax), -(Ay)
struct M68k_sbcd: M68k {
    M68k_sbcd(): M68k("sbcd", m68k_family,
                      OP(8) & BITS<4, 8>(0x10)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src=NULL, *dst=NULL;
        if (extract<3, 3>(w0)) {
            // SBCD.B -(Ax), -(Ay)
            src = d->makeAddressRegisterPreDecrement(extract<0, 2>(w0), m68k_fmt_i8);
            dst = d->makeAddressRegisterPreDecrement(extract<9, 11>(w0), m68k_fmt_i8);
        } else {
            // SBCD.B Dx, Dy
            src = d->makeDataRegister(extract<0, 2>(w0), m68k_fmt_i8, 0);
            dst = d->makeDataRegister(extract<9, 11>(w0), m68k_fmt_i8, 0);
        }
        return d->makeInstruction(m68k_sbcd, "sbcd.b", src, dst);
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
struct M68k_scc: M68k {
    M68k_scc(): M68k("scc", m68k_family,
                     OP(5) & BITS<6, 7>(3) & EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<8, 11>(w0)) {
            case 0x0: kind = m68k_st;  break;
            case 0x1: kind = m68k_sf;  break;
            case 0x2: kind = m68k_shi; break;
            case 0x3: kind = m68k_sls; break;
            case 0x4: kind = m68k_scc; break;
            case 0x5: kind = m68k_scs; break;
            case 0x6: kind = m68k_sne; break;
            case 0x7: kind = m68k_seq; break;
            case 0x8: kind = m68k_svc; break;
            case 0x9: kind = m68k_svs; break;
            case 0xa: kind = m68k_spl; break;
            case 0xb: kind = m68k_smi; break;
            case 0xc: kind = m68k_sge; break;
            case 0xd: kind = m68k_slt; break;
            case 0xe: kind = m68k_sgt; break;
            case 0xf: kind = m68k_sle; break;
        }
        std::string mnemonic = stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_") + ".b";
        SgAsmExpression *ea = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i8, 0);
        return d->makeInstruction(kind, mnemonic, ea);
    }
};

// SUB.B <ea>y, Dx
// SUB.W <ea>y, Dx
// SUB.L <ea>y, Dx
struct M68k_sub_1: M68k {
    M68k_sub_1(): M68k("sub_1", m68k_family,
                       OP(9) & BIT<8>(0) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), fmt);
        return d->makeInstruction(m68k_sub, "sub."+formatLetter(fmt), src, dst);
    }
};

// SUB.B Dy, <ea>x
// SUB.W Dy, <ea>x
// SUB.L Dy, <ea>x
struct M68k_sub_2: M68k {
    M68k_sub_2(): M68k("sub_2", m68k_family,
                       OP(9) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                       EAM(m68k_eam_memory & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), fmt);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(m68k_sub, "sub."+formatLetter(fmt), src, dst);
    }
};

// SUBA.W <ea>, Ax
// SUBA.L <ea>, Ax
struct M68k_suba: M68k {
    M68k_suba(): M68k("suba", m68k_family,
                      OP(9) & BITS<6, 7>(3) & EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = extract<8, 8>(w0) ? m68k_fmt_i32 : m68k_fmt_i16;
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        SgAsmExpression *dst = d->makeAddressRegister(extract<9, 11>(w0), m68k_fmt_i32, 0);
        return d->makeInstruction(m68k_suba, "suba."+formatLetter(fmt), src, dst);
    }
};

// SUBI.B #<data>, Dx
// SUBI.W #<data>, Dx
// SUBI.L #<data>, Dx
struct M68k_subi: M68k {
    M68k_subi(): M68k("subi", m68k_family,
                      OP(0) & BITS<8, 10>(4) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        size_t opandWords = (formatNBits(fmt)+15) / 16;
        SgAsmExpression *src = d->makeImmediateExtension(fmt, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, opandWords);
        return d->makeInstruction(m68k_subi, "subi."+formatLetter(fmt), src, dst);
    }
};

// SUBQ.B #<data>, <ea>x
// SUBQ.W #<data>, <ea>x
// SUBQ.L #<data>, <ea>x
//      Address register direct modes are handled by subq_a
struct M68k_subq: M68k {
    M68k_subq(): M68k("subq", m68k_family,
                      OP(5) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(m68k_eam_alter & ~m68k_eam_pcmi & ~m68k_eam_ard)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        unsigned n = extract<9, 11>(w0);
        SgAsmExpression *src = d->makeImmediateValue(fmt, n?n:8);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(m68k_subq, "subq."+formatLetter(fmt), src, dst);
    }
};

// SUBQ.W #<data>, Ax  (but treated as long)
// SUBQ.L #<data>, Ax
struct M68k_subq_a: M68k {
    M68k_subq_a(): M68k("subq_a", m68k_family,
                        OP(5) & BIT<8>(1) & (BITS<6, 7>(1) | BITS<6, 7>(2)) & EAM(m68k_eam_ard)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = m68k_fmt_i32;              // word-mode is like long-mode when writing to an address register
        unsigned n = extract<9, 11>(w0);
        SgAsmExpression *src = d->makeImmediateValue(fmt, n?n:8);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(m68k_subq, "subq."+formatLetter(fmt), src, dst);
    }
};

// SUBX.B Dx, Dy
// SUBX.W Dx, Dy
// SUBX.L Dx, Dy
// SUBX.B -(Ax), -(Ay)
// SUBX.W -(Ax), -(Ay)
// SUBX.L -(Ax), -(Ay)
struct M68k_subx: M68k {
    M68k_subx(): M68k("subx", m68k_family,
                      OP(9) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<4, 5>(0)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src=NULL, *dst=NULL;
        if (extract<3, 3>(w0)) {
            // SUBX.? -(Ax), -(Ay)
            src = d->makeAddressRegisterPreDecrement(extract<0, 2>(w0), fmt);
            dst = d->makeAddressRegisterPreDecrement(extract<9, 11>(w0), fmt);
        } else {
            // SUBX.? Dx, Dy
            src = d->makeDataRegister(extract<0, 2>(w0), fmt, 0);
            dst = d->makeDataRegister(extract<9, 11>(w0), fmt, 0);
        }
        return d->makeInstruction(m68k_subx, "subx."+formatLetter(fmt), src, dst);
    }
};

// SWAP.W Dx
struct M68k_swap: M68k {
    M68k_swap(): M68k("swap", m68k_family,
                      OP(4) & BITS<3, 11>(0x108)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<0, 2>(w0), m68k_fmt_i32, 0);
        return d->makeInstruction(m68k_swap, "swap.w", src); // mnemonic is "w" because it swaps words of a 32-bit reg
    }
};

// TAS.B <ea>x
struct M68k_tas: M68k {
    M68k_tas(): M68k("tas", m68k_family,
                     OP(4) & BITS<6, 11>(0x2b) & EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), m68k_fmt_i8, 0);
        return d->makeInstruction(m68k_tas, "tas.b", src);
    }
};

// TRAP #<vector>
struct M68k_trap: M68k {
    M68k_trap(): M68k("trap", m68k_family,
                      OP(4) & BITS<4, 11>(0x0e4)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *vector = d->makeImmediateValue(m68k_fmt_i8, extract<0, 3>(w0));
        return d->makeInstruction(m68k_trap, "trap", vector);
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
struct M68k_trapcc: M68k {
    M68k_trapcc(): M68k("trapcc", m68k_68020|m68k_68030|m68k_68040|m68k_freescale_cpu32,
                        OP(5) & BITS<3, 7>(0x1f) & (BITS<0, 2>(2) | BITS<0, 2>(3) | BITS<0, 2>(4))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = m68k_fmt_unknown;
        switch (extract<0, 2>(w0)) {
            case 2: fmt = m68k_fmt_i16; break;
            case 3: fmt = m68k_fmt_i32; break;
            case 4: fmt = m68k_fmt_unknown; break;                   // no operand
        }
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<8, 11>(w0)) {
            case 0x0: kind = m68k_trapt;  break;
            case 0x1: kind = m68k_trapf;  break;        // aka. TPF
            case 0x2: kind = m68k_traphi; break;
            case 0x3: kind = m68k_trapls; break;
            case 0x4: kind = m68k_trapcc; break;
            case 0x5: kind = m68k_trapcs; break;
            case 0x6: kind = m68k_trapne; break;
            case 0x7: kind = m68k_trapeq; break;
            case 0x8: kind = m68k_trapvc; break;
            case 0x9: kind = m68k_trapvs; break;
            case 0xa: kind = m68k_trappl; break;
            case 0xb: kind = m68k_trapmi; break;
            case 0xc: kind = m68k_trapge; break;
            case 0xd: kind = m68k_traplt; break;
            case 0xe: kind = m68k_trapgt; break;
            case 0xf: kind = m68k_traple; break;
        }
        if (m68k_fmt_unknown==fmt) {
            std::string mnemonic = stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_");
            return d->makeInstruction(kind, mnemonic);
        } else {
            SgAsmExpression *data = d->makeImmediateExtension(fmt, 0);
            std::string mnemonic = stringifyBinaryAnalysisM68kInstructionKind(kind, "m68k_") + "." + formatLetter(fmt);
            return d->makeInstruction(kind, mnemonic, data);
        }
    }
};
        
// TRAPV
struct M68k_trapv: M68k {
    M68k_trapv(): M68k("trapv", m68k_family,
                       OP(4) & BITS<0, 11>(0xe76)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_trapv, "trapv");
    }
};

// TST.B <ea>y
// TST.W <ea>y
// TST.L <ea>y
struct M68k_tst: M68k {
    M68k_tst(): M68k("tst", m68k_family,
                     OP(4) & BITS<8, 11>(0xa) & EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kDataFormat fmt = integerFormat(extract<6, 7>(w0));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), fmt, 0);
        return d->makeInstruction(m68k_tst, "tst."+formatLetter(fmt), src);
    }
};

// UNLK Ax
struct M68k_unlk: M68k {
    M68k_unlk(): M68k("unlk", m68k_family,
                      OP(4) & BITS<3, 11>(0x1cb)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *dst = d->makeAddressRegister(extract<0, 2>(w0), m68k_fmt_i32, 0);
        return d->makeInstruction(m68k_unlk, "unlk", dst);
    }
};

// UNPK Dx, Dy, #<adjustment>
// UNPK -(Ax), -(Ay), #<adjustment>
struct M68k_unpk: M68k {
    M68k_unpk(): M68k("unpk", m68k_68020|m68k_68030|m68k_68040,
                      OP(8) & BITS<4, 8>(0x18)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src=NULL, *dst=NULL;
        if (extract<3, 3>(w0)) {
            // UNPK -(Ax), -(Ay), #<adjustment>
            src = d->makeAddressRegisterPreDecrement(extract<0, 2>(w0), m68k_fmt_i16);
            dst = d->makeAddressRegisterPreDecrement(extract<9, 11>(w0), m68k_fmt_i16);
        } else {
            // UNPK Dx, Dy, #<adjustment>
            src = d->makeDataRegister(extract<0, 2>(w0), m68k_fmt_i16, 0);
            dst = d->makeDataRegister(extract<0, 2>(w0), m68k_fmt_i16, 0);
        }
        SgAsmExpression *adj = d->makeImmediateExtension(m68k_fmt_i16, 0);
        return d->makeInstruction(m68k_unpk, "unpk", src, dst, adj);
    }
};



    
//                              Instructions from MFC5484 that we don't use anymore
//  
//  
//  
//  // BITREV.L Dx
//  struct M68k_bitrev: M68k {
//      M68k_bitrev(): M68k("bitrev", OP(0) & BITS<3, 11>(0x18)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          return d->makeInstruction(m68k_bitrev, "bitrev.l", d->makeDataRegister(extract<0, 2>(w0), 32));
//      }
//  };
//  
//  // BYTEREV.L Dx
//  struct M68k_byterev: M68k {
//      M68k_byterev(): M68k("byterev", OP(0) & BITS<3, 11>(0x58)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          return d->makeInstruction(m68k_byterev, "byterev.l", d->makeDataRegister(extract<0, 2>(w0), 32));
//      }
//  };
//  
//  // FF1.L Dx
//  struct M68k_ff1: M68k {
//      M68k_ff1(): M68k("ff1", OP(0) & BITS<3, 11>(0x98)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          return d->makeInstruction(m68k_ff1, "ff1.l", d->makeDataRegister(extract<0, 2>(w0), 32));
//      }
//  };
//  
//  
//  
//  // MOVE.L MACSR, CCR
//  struct M68k_move_macsr2ccr: M68k {
//      M68k_move_macsr2ccr(): M68k("move_macsr2ccr", OP(10) & BITS<0, 11>(0x9c0)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          return d->makeInstruction(m68k_move, "move.l",
//                                    d->makeRegister(RegisterDescriptor(m68k_regclass_mac, m68k_mac_macsr, 0, 32)),
//                                    d->makeConditionCodeRegister());
//      }
//  };
//  
//  
//  // MSAC.W Ry.{U,L}, Rx.{U,L} SF, <ea>y&, Rw
//  struct M68k_msac_w2: M68k {
//      M68k_msac_w2(): M68k("msac_w2", OP(10) & BITS<7, 8>(1) & EAM(m68k_eam_ari|m68k_eam_inc|m68k_eam_dec|m68k_eam_dsp) &
//                           INSN_WORD<1>(BIT<11>(0) & BIT<8>(1) & BIT<4>(0))) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          size_t ry_offset = extract<6, 6>(d->instructionWord(1)) ? 16 : 0;
//          size_t rx_offset = extract<7, 7>(d->instructionWord(1)) ? 16 : 0;
//          SgAsmExpression *ry = d->makeDataAddressRegister(extract<0, 3>(d->instructionWord(1)), 16, ry_offset);
//          SgAsmExpression *rx = d->makeDataAddressRegister(extract<12, 15>(d->instructionWord(1)), 16, rx_offset);
//          SgAsmExpression *sf = d->makeImmediateValue(8, extract<9, 10>(d->instructionWord(1)));
//          SgAsmExpression *eay = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
//          SgAsmExpression *use_mask = d->makeImmediateValue(1, extract<5, 5>(d->instructionWord(1)));
//          SgAsmExpression *rw = extract<6, 6>(w0) ?
//                                d->makeAddressRegister(extract<9, 11>(w0), 32) :
//                                d->makeDataRegister(extract<9, 11>(w0), 32);
//          return d->makeInstruction(m68k_msac, "msac.w", ry, rx, sf, eay, use_mask, rw);
//      }
//  };
//  
//  // PULSE
//  struct M68k_pulse: M68k {
//      M68k_pulse(): M68k("pulse", OP(4) & BITS<0, 11>(0xacc)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          return d->makeInstruction(m68k_pulse, "pulse");
//      }
//  };
//  
//  // SATS.L Dx
//  struct M68k_sats: M68k {
//      M68k_sats(): M68k("sats", OP(4) & BITS<3, 11>(0x190)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          SgAsmExpression *dx = d->makeDataRegister(extract<0, 2>(w0), 32);
//          return d->makeInstruction(m68k_sats, "sats.l", dx);
//      }
//  };
//  
//                  
//  // TPF
//  // TPF.W #<data>
//  // TPF.L #<data>
//  struct M68k_tpf: M68k {
//      M68k_tpf(): M68k("tpf", OP(5) & BITS<3, 11>(0x3f)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          SgAsmExpression *src = NULL;
//          unsigned opmode = extract<0, 2>(w0);
//          switch (opmode) {
//              case 2:
//                  src = d->makeImmediateExtension(16, 0);
//                  return d->makeInstruction(m68k_tpf, "tpf.w", src);
//              case 3:
//                  src = d->makeImmediateExtension(32, 0);
//                  return d->makeInstruction(m68k_tpf, "tpf.l", src);
//              case 4:
//                  return d->makeInstruction(m68k_tpf, "tpf");
//              default:
//                  throw Disassembler::Exception("invalid opmode "+numberToString(opmode)+" for TPF instruction");
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
//  struct M68k_wddata: M68k {
//      M68k_wddata(): M68k("wddata", OP(15) & BITS<8, 11>(0xb) & EAM(m68k_eam_memory & m68k_eam_alter)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          size_t nbits = 0;
//          switch (extract<6, 7>(w0)) {
//              case 0: nbits = 8; break;
//              case 1: nbits = 16; break;
//              case 2: nbits = 32; break;
//              case 3: throw Disassembler::Exception("invalid size for WDDATA instruction");
//          }
//          SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
//          return d->makeInstruction(m68k_wddata, "wddata."+sizeToLetter(nbits), src);
//      }
//  };

        
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
DisassemblerM68k::init()
{
    // Default register dictionary
    const RegisterDictionary *regdict = NULL;
    if ((family & m68k_freescale) != 0) {
        name("coldfire");
        regdict = RegisterDictionary::dictionary_coldfire_emac();
    } else {
        name("m68040");
        regdict = RegisterDictionary::dictionary_m68000();
    }
    registerDictionary(regdict);
    REG_IP = registerDictionary()->findOrThrow("pc");
    REG_SP = registerDictionary()->findOrThrow("a7");

    p_proto_dispatcher = InstructionSemantics2::DispatcherM68k::instance();
    p_proto_dispatcher->addressWidth(32);
    p_proto_dispatcher->set_register_dictionary(regdict);

    wordSizeBytes(2);
    byteOrder(ByteOrder::ORDER_MSB);
    callingConventions(CallingConvention::dictionaryM68k());

    idis_table.resize(17);

#define M68k_DECODER(NAME)                                                                                                     \
    static M68k_##NAME *v_##NAME=NULL;                                                                                         \
    if (NULL==v_##NAME)                                                                                                        \
        v_##NAME = new M68k_##NAME;                                                                                            \
    insert_idis(v_##NAME);

    // Integer instructions from pages 4-1 through 4-198
    M68k_DECODER(abcd_1);
    M68k_DECODER(abcd_2);
    M68k_DECODER(add_1);
    M68k_DECODER(add_2);
    M68k_DECODER(add_3);
    M68k_DECODER(adda);
    M68k_DECODER(addi);
    M68k_DECODER(addq);
    M68k_DECODER(addx_1);
    M68k_DECODER(addx_2);
    M68k_DECODER(and_1);
    M68k_DECODER(and_2);
    M68k_DECODER(andi);
    M68k_DECODER(andi_to_ccr);
    M68k_DECODER(ashift_1);
    M68k_DECODER(ashift_2);
    M68k_DECODER(ashift_3);
    M68k_DECODER(branch);
    M68k_DECODER(bchg_1);
    M68k_DECODER(bchg_2);
    M68k_DECODER(bchg_3);
    M68k_DECODER(bchg_4);
    M68k_DECODER(bclr_1);
    M68k_DECODER(bclr_2);
    M68k_DECODER(bclr_3);
    M68k_DECODER(bclr_4);
    M68k_DECODER(bfchg);
    M68k_DECODER(bfclr);
    M68k_DECODER(bfexts);
    M68k_DECODER(bfextu);
    M68k_DECODER(bfins);
    M68k_DECODER(bfset);
    M68k_DECODER(bftst);
    M68k_DECODER(bkpt);
    M68k_DECODER(bset_1);
    M68k_DECODER(bset_2);
    M68k_DECODER(bset_3);
    M68k_DECODER(bset_4);
    M68k_DECODER(btst_1);
    M68k_DECODER(btst_2);
    M68k_DECODER(btst_3);
    M68k_DECODER(btst_4);
    M68k_DECODER(callm);
    M68k_DECODER(cas);
    M68k_DECODER(cas2);
    M68k_DECODER(chk);
    M68k_DECODER(chk2);
    M68k_DECODER(clr);
    M68k_DECODER(cmp);
    M68k_DECODER(cmpa);
    M68k_DECODER(cmpi);
    M68k_DECODER(cmpm);
    M68k_DECODER(cmp2);
    M68k_DECODER(cpbcc);
    M68k_DECODER(cpdbcc);
    M68k_DECODER(cpscc);
    M68k_DECODER(cptrapcc);
    M68k_DECODER(cpush);
    M68k_DECODER(dbcc);
    M68k_DECODER(divide);
    M68k_DECODER(divs_w);
    M68k_DECODER(divu_w);
    M68k_DECODER(eor);
    M68k_DECODER(eori);
    M68k_DECODER(eori_to_ccr);
    M68k_DECODER(exg);
    M68k_DECODER(ext_w);
    M68k_DECODER(ext_l);
    M68k_DECODER(extb_l);
    M68k_DECODER(fabs_from_drd);
    M68k_DECODER(fabs_mr);
    M68k_DECODER(fabs_rr);
    M68k_DECODER(fadd_from_drd);
    M68k_DECODER(fadd_mr);
    M68k_DECODER(fadd_rr);
    M68k_DECODER(fb);
    M68k_DECODER(fcmp_from_drd);
    M68k_DECODER(fcmp_mr);
    M68k_DECODER(fcmp_rr);
    M68k_DECODER(fdiv_from_drd);
    M68k_DECODER(fdiv_mr);
    M68k_DECODER(fdiv_rr);
    M68k_DECODER(fint_from_drd);
    M68k_DECODER(fint_mr);
    M68k_DECODER(fint_rr);
    M68k_DECODER(fintrz_from_drd);
    M68k_DECODER(fintrz_mr);
    M68k_DECODER(fintrz_rr);
    M68k_DECODER(fmove_from_drd);
    M68k_DECODER(fmove_mr);
    M68k_DECODER(fmove_rm);
    M68k_DECODER(fmove_rr);
    M68k_DECODER(fmove_to_drd);
    M68k_DECODER(fmovem_mr);
    M68k_DECODER(fmovem_rm);
    M68k_DECODER(fmul_from_drd);
    M68k_DECODER(fmul_mr);
    M68k_DECODER(fmul_rr);
    M68k_DECODER(fneg_from_drd);
    M68k_DECODER(fneg_mr);
    M68k_DECODER(fneg_rr);
    M68k_DECODER(fnop);
    M68k_DECODER(fsqrt_from_drd);
    M68k_DECODER(fsqrt_mr);
    M68k_DECODER(fsqrt_rr);
    M68k_DECODER(fsub_from_drd);
    M68k_DECODER(fsub_mr);
    M68k_DECODER(fsub_rr);
    M68k_DECODER(ftst_from_drd);
    M68k_DECODER(ftst_mr);
    M68k_DECODER(ftst_rr);
    M68k_DECODER(illegal);
    M68k_DECODER(jmp);
    M68k_DECODER(jsr);
    M68k_DECODER(lea);
    M68k_DECODER(link_w);
    M68k_DECODER(link_l);
    M68k_DECODER(lshift_rr);
    M68k_DECODER(lshift_ir);
    M68k_DECODER(lshift_mem);
    M68k_DECODER(mac);
    M68k_DECODER(mac_l2);
    M68k_DECODER(mac_w2);
    M68k_DECODER(mov3q);
    M68k_DECODER(movclr);
    M68k_DECODER(move);
    M68k_DECODER(move_from_acc);
    M68k_DECODER(move_from_accext);
    M68k_DECODER(move_from_ccr);
    M68k_DECODER(move_from_macsr);
    M68k_DECODER(move_from_mask);
    M68k_DECODER(move_from_sr);
    M68k_DECODER(move_to_acc);
    M68k_DECODER(move_to_accext);
    M68k_DECODER(move_to_ccr);
    M68k_DECODER(move_to_cr);
    M68k_DECODER(move_to_macsr);
    M68k_DECODER(move_to_mask);
    M68k_DECODER(move_to_sr);
    M68k_DECODER(move16_pp);
    M68k_DECODER(move16_a);
    M68k_DECODER(movea);
    M68k_DECODER(movem_mr);
    M68k_DECODER(movem_rm);
    M68k_DECODER(movep);
    M68k_DECODER(moveq);
    M68k_DECODER(msac);
    M68k_DECODER(muls_w);
    M68k_DECODER(multiply_l);
    M68k_DECODER(multiply_64);
    M68k_DECODER(mulu_w);
    M68k_DECODER(mvs);
    M68k_DECODER(mvz);
    M68k_DECODER(nbcd);
    M68k_DECODER(neg);
    M68k_DECODER(negx);
    M68k_DECODER(nop);
    M68k_DECODER(not);
    M68k_DECODER(or_1);
    M68k_DECODER(or_2);
    M68k_DECODER(ori);
    M68k_DECODER(ori_to_ccr);
    M68k_DECODER(pack);
    M68k_DECODER(pea);
    M68k_DECODER(rotate_reg);
    M68k_DECODER(rotate_mem);
    M68k_DECODER(rotate_extend_reg);
    M68k_DECODER(rotate_extend_mem);
    M68k_DECODER(rtd);
    M68k_DECODER(rtm);
    M68k_DECODER(rtr);
    M68k_DECODER(rts);
    M68k_DECODER(sbcd);
    M68k_DECODER(scc);
    M68k_DECODER(sub_1);
    M68k_DECODER(sub_2);
    M68k_DECODER(suba);
    M68k_DECODER(subi);
    M68k_DECODER(subq);
    M68k_DECODER(subq_a);
    M68k_DECODER(subx);
    M68k_DECODER(swap);
    M68k_DECODER(tas);
    M68k_DECODER(trap);
    M68k_DECODER(trapcc);
    M68k_DECODER(trapv);
    M68k_DECODER(tst);
    M68k_DECODER(unlk);
    M68k_DECODER(unpk);

    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"M68k instruction disassembly table indexed by high-order nybble of first 16-bit word:\n";
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
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::DisassemblerM68k);
#endif

#endif
