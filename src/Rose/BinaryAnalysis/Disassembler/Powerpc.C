#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Disassembler/Powerpc.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

#include <SgAsmBinaryAdd.h>
#include <SgAsmDirectRegisterExpression.h>
#include <SgAsmFloatType.h>
#include <SgAsmIntegerType.h>
#include <SgAsmMemoryReferenceExpression.h>
#include <SgAsmPowerpcInstruction.h>
#include <SgAsmVectorType.h>

#include "Assembler.h"
#include "AssemblerX86.h"
#include "SageBuilderAsm.h"


namespace Rose {
namespace BinaryAnalysis {
namespace Disassembler {

/* See header file for full documentation. */

/* References:
 *
 * [1] "Programming Environments Manual for 32-Bit Implementations of the PowerPC Architecture", MPCFPE32B Rev. 3, 9/2005, by
 * Freescale Semiconductor (www.freescale.com). Freescale Semiconductor Technical Information Center, 1300 N. Alma School Road,
 * Chandler, AZ 85224. 1-800-521-6274.  A copy can be found on the web at
 * http://www-106.ibm.com/developerworks/linux/library/l-powarch/
 */

#define T_U8 (SageBuilderAsm::buildTypeU8())
#define T_U16 (SageBuilderAsm::buildTypeU16())
#define T_U32 (SageBuilderAsm::buildTypeU32())
#define T_U64 (SageBuilderAsm::buildTypeU64())
#define T_FLOAT32 (SageBuilderAsm::buildIeee754Binary32())
#define T_FLOAT64 (SageBuilderAsm::buildIeee754Binary64())
#define T_V2_U32 (SageBuilderAsm::buildTypeVector(2, T_U32))
#define T_V2_FLOAT32 (SageBuilderAsm::buildTypeVector(2, T_FLOAT32))
#define T_V2_FLOAT64 (SageBuilderAsm::buildTypeVector(2, T_FLOAT64))

Powerpc::ExceptionPowerpc::ExceptionPowerpc(const std::string &mesg, const State &state, size_t bit)
: Exception(mesg, state.ip) {
    // Convert four-byte instruction to big-endian buffer. Note that PowerPC is big-endian, but PowerPC can support
    // both big- and little-endian processor modes (with much weirdness; e.g. PDP endian like propoerties). */
    bytes.push_back((state.insn>>24) & 0xff);
    bytes.push_back((state.insn>>16) & 0xff);
    bytes.push_back((state.insn>>8) & 0xff);
    bytes.push_back(state.insn & 0xff);
    ASSERT_require(bit<=32);
    this->bit = 8*(4-(bit/8)) + bit%8;          // convert from native uint32_t bit position to big-endian
}

Powerpc::Powerpc(const Architecture::Base::ConstPtr &arch)
    : Base(arch),
      wordSize_(4 == arch->bytesPerWord() ? powerpc_32 : powerpc_64),
      sex_(arch->byteOrder()) {
    init();
}

Powerpc::Ptr
Powerpc::instance(const Architecture::Base::ConstPtr &arch) {
    ASSERT_not_null(arch);
    return Ptr(new Powerpc(arch));
}

Base::Ptr
Powerpc::clone() const {
    return Ptr(new Powerpc(*this));
}

void
Powerpc::init() {
    REG_IP =   architecture()->registerDictionary()->instructionPointerRegister();
    REG_SP =   architecture()->registerDictionary()->stackPointerRegister();
    REG_SF =   architecture()->registerDictionary()->stackFrameRegister();
    REG_LINK = architecture()->registerDictionary()->callReturnRegister();
    ASSERT_require(REG_IP);
    ASSERT_require(REG_SP);
    ASSERT_require(REG_SF);
    ASSERT_require(REG_LINK);
    capabilities_.set(powerpc_capability_default);
}

bool
Powerpc::strictReserved() const {
    return strictReserved_;
}

void
Powerpc::strictReserved(const bool b) {
    strictReserved_ = b;
}

// This is a bit of a kludge for now because we're trying to use an unmodified version of the PowerpcDisassembler name space.
SgAsmInstruction*
Powerpc::disassembleOne(const MemoryMap::Ptr &map, Address start_va, AddressSet *successors) {
    // The old PowerpcDisassembler::disassemble() function doesn't understand MemoryMap mappings. Therefore, remap the next few
    // bytes (enough for at least one instruction) into a temporary buffer.
    unsigned char temp[4];
    size_t tempsz = map->at(start_va).limit(sizeof temp).require(MemoryMap::EXECUTABLE).read(temp).size();

    // Treat the bytes as a big-endian instruction.  Note that PowerPC is big-endian, but PowerPC can support both big- and
    // little-endian processor modes (with much weirdness; e.g. PDP endian like propoerties).
    if (tempsz<4)
        throw Exception("short read", start_va);
    uint32_t c = (temp[0]<<24) | (temp[1]<<16) | (temp[2]<<8) | temp[3];
    if (ByteOrder::ORDER_LSB == sex_)
        c = ByteOrder::swapBytes(c);

    // Disassemble the instruction
    SgAsmPowerpcInstruction *insn = nullptr;
    try {
        State state;
        startInstruction(state, start_va, c);
        insn = disassemble(state);                      // throws an exception on error, but we want to return an unknown insn
        ASSERT_not_null(insn);
    } catch (const ExceptionPowerpc &e) {
        return makeUnknownInstruction(e);
    }

    // Note successors if necessary
    if (successors) {
        bool complete;
        *successors |= architecture()->getSuccessors(insn, complete/*out*/);
    }

    return insn;
}

SgAsmInstruction*
Powerpc::makeUnknownInstruction(const Exception &e) {
    SgAsmPowerpcInstruction *insn = new SgAsmPowerpcInstruction(e.ip, *architecture()->registrationId(),
                                                                powerpc_unknown_instruction);
    SgAsmOperandList *operands = new SgAsmOperandList();
    insn->set_operandList(operands);
    operands->set_parent(insn);
    insn->set_rawBytes(e.bytes);
    return insn;
}

const Sawyer::BitFlags<PowerpcCapability>&
Powerpc::capabilities() const {
    return capabilities_;
}

Sawyer::BitFlags<PowerpcCapability>&
Powerpc::capabilities() {
    return capabilities_;
}

template <size_t First, size_t Last>
uint64_t
Powerpc::fld(State &state) const {
    return (state.insn >> (31 - Last)) & (IntegerOps::GenMask<uint32_t, Last - First + 1>::value);
}

// Test whether the specified field is zero if strict reserved fields are enabled
template<size_t First, size_t Last>                     // inclusive
bool
Powerpc::reservedOk(State &state) const {
    return !strictReserved_ || fld<First, Last>(state) == 0;
}

// FIXME[Robb Matzke 2019-08-20]: Replace with proper C++ since nested macros make debugging hard
// `Mne` is the `PowerpcInstructionKind` enum without the trailing "_record"
// `Cap` is the `PowerpcCapability` enum without the leading "powerpc_capability_"
// `Op1`, `Op2`, etc. are non-null `SgAsmExpression*` operand expressions
// The "*_O_RC" forms append "o" to the `Mne` symbol if the `OE` bit is set, and/or append "_record" if the `Rc` bit is set
// The "*_RC" forms of the macros append "_record" to the `Mne` symbol if the `Rc` bit is set in the machine instruction
#define MAKE_INSN0(Mne, Cap) (makeInstructionWithoutOperands(state.ip, powerpc_##Mne, state.insn, powerpc_capability_##Cap))
#define MAKE_INSN0_RC(Mne, Cap) (Rc(state) ? MAKE_INSN0(Mne##_record, Cap) : MAKE_INSN0(Mne, Cap))
#define MAKE_INSN0_O_RC(Mne, Cap) (OE() ? MAKE_INSN0_RC(Mne##o, Cap) : MAKE_INSN0_RC(Mne, Cap))
#define MAKE_INSN1(Mne, Cap, Op1) (SageBuilderAsm::appendOperand(MAKE_INSN0(Mne, Cap), (Op1)))
#define MAKE_INSN1_RC(Mne, Cap, Op1) (SageBuilderAsm::appendOperand(MAKE_INSN0_RC(Mne, Cap), (Op1)))
#define MAKE_INSN1_O_RC(Mne, Cap, Op1) (SageBuilderAsm::appendOperand(MAKE_INSN0_O_RC(Mne, Cap), (Op1)))
#define MAKE_INSN2(Mne, Cap, Op1, Op2) (SageBuilderAsm::appendOperand(MAKE_INSN1(Mne, Cap, Op1), (Op2)))
#define MAKE_INSN2_RC(Mne, Cap, Op1, Op2) (SageBuilderAsm::appendOperand(MAKE_INSN1_RC(Mne, Cap, Op1), (Op2)))
#define MAKE_INSN2_O_RC(Mne, Cap, Op1, Op2) (SageBuilderAsm::appendOperand(MAKE_INSN1_O_RC(Mne, Cap, Op1), (Op2)))
#define MAKE_INSN3(Mne, Cap, Op1, Op2, Op3) (SageBuilderAsm::appendOperand(MAKE_INSN2(Mne, Cap, Op1, Op2), (Op3)))
#define MAKE_INSN3_RC(Mne, Cap, Op1, Op2, Op3) (SageBuilderAsm::appendOperand(MAKE_INSN2_RC(Mne, Cap, Op1, Op2), (Op3)))
#define MAKE_INSN3_O_RC(Mne, Cap, Op1, Op2, Op3) (SageBuilderAsm::appendOperand(MAKE_INSN2_O_RC(Mne, Cap, Op1, Op2), (Op3)))
#define MAKE_INSN4(Mne, Cap, Op1, Op2, Op3, Op4) (SageBuilderAsm::appendOperand(MAKE_INSN3(Mne, Cap, Op1, Op2, Op3), (Op4)))
#define MAKE_INSN4_RC(Mne, Cap, Op1, Op2, Op3, Op4) (SageBuilderAsm::appendOperand(MAKE_INSN3_RC(Mne, Cap, Op1, Op2, Op3), (Op4)))
#define MAKE_INSN5(Mne, Cap, Op1, Op2, Op3, Op4, Op5) (SageBuilderAsm::appendOperand(MAKE_INSN4(Mne, Cap, Op1, Op2, Op3, Op4), (Op5)))
#define MAKE_INSN5_RC(Mne, Cap, Op1, Op2, Op3, Op4, Op5) (SageBuilderAsm::appendOperand(MAKE_INSN4_RC(Mne, Cap, Op1, Op2, Op3, Op4), (Op5)))
#define MAKE_UNKNOWN() (makeInstructionWithoutOperands(state.ip, powerpc_unknown_instruction, state.insn, powerpc_capability_all))

SgAsmIntegerValueExpression*
Powerpc::makeBranchTarget(uint64_t targetAddr) const {
    switch (wordSize_) {
        case powerpc_32:
            return SageBuilderAsm::buildValueU32(targetAddr);
        case powerpc_64:
            return SageBuilderAsm::buildValueU64(targetAddr);
    }
    ASSERT_not_reachable("invalid word size");
}

bool
Powerpc::is64bitInsn(PowerpcInstructionKind kind) {
    switch (kind) {
        case powerpc_cntlzd:
        case powerpc_cntlzd_record:
        case powerpc_divd:
        case powerpc_divd_record:
        case powerpc_divdo:
        case powerpc_divdo_record:
        case powerpc_divdu:
        case powerpc_divdu_record:
        case powerpc_divduo:
        case powerpc_divduo_record:
        case powerpc_extsw:
        case powerpc_extsw_record:
        case powerpc_ld:
        case powerpc_ldu:
        case powerpc_ldux:
        case powerpc_ldx:
        case powerpc_mulhd:
        case powerpc_mulhd_record:
        case powerpc_mulhdu:
        case powerpc_mulhdu_record:
        case powerpc_mulld:
        case powerpc_mulld_record:
        case powerpc_mulldo:
        case powerpc_mulldo_record:
        case powerpc_rldcl:
        case powerpc_rldcl_record:
        case powerpc_rldcr:
        case powerpc_rldcr_record:
        case powerpc_rldic:
        case powerpc_rldic_record:
        case powerpc_rldicl:
        case powerpc_rldicl_record:
        case powerpc_rldicr:
        case powerpc_rldicr_record:
        case powerpc_rldimi:
        case powerpc_rldimi_record:
        case powerpc_sld:
        case powerpc_sld_record:
        case powerpc_srad:
        case powerpc_srad_record:
        case powerpc_sradi:
        case powerpc_sradi_record:
        case powerpc_srd:
        case powerpc_srd_record:
        case powerpc_std:
        case powerpc_stdu:
        case powerpc_stdux:
        case powerpc_stdx:
        case powerpc_td:
        case powerpc_tdi:

            return true;
        default:
            return false;
    }
}

SgAsmPowerpcInstruction*
Powerpc::makeInstructionWithoutOperands(uint64_t address, PowerpcInstructionKind kind, uint32_t insn, const PowerpcCapability cap) {

    if (capabilities_.isClear(cap) || (powerpc_32 == wordSize_ && is64bitInsn(kind))) {
        // This machine instruction encoding is not valid for this situation. E.g., we found an encoding for an instruction that's
        // part of the Virtual Environment Architecture (VEA) but this decoder is not configured to decode such instructions.
        ASSERT_forbid(powerpc_unknown_instruction == kind);
        return makeInstructionWithoutOperands(address, powerpc_unknown_instruction, insn, powerpc_capability_all);
    }

    SgAsmPowerpcInstruction* instruction = new SgAsmPowerpcInstruction(address, *architecture()->registrationId(), kind);
    ASSERT_not_null(instruction);

    SgAsmOperandList* operands = new SgAsmOperandList();
    instruction->set_operandList(operands);
    operands->set_parent(instruction);

    // PowerPC uses a fixed length instruction set.
    SgUnsignedCharList bytes(4, '\0');
    for (int i = 0; i < 4; ++i)
        bytes[i] = (insn >> (24 - (8 * i))) & 0xFF; // Force big-endian

    instruction->set_rawBytes(bytes);
    return instruction;
}

// At one time this function created PowerPC-specific register reference expressions (RREs) that had hard-coded values for
// register class, register number, and register position.
//
// The new approach (added Oct 2010) replaces the PowerPC-specific numbers with a more generic RegisterDescriptor struct, where
// each register is described by a major number (fomerly the PowerpcRegisterClass), a minor number (formerly the reg_number),
// and a bit offset and size (usually zero and 32, but also implied by the PowerpcConditionRegisterAccessGranularity and
// reg_number combination).  The new idea is that a RegisterDescriptor does not need to contain machine-specific
// values. Therefore, we've added a level of indirection: makeRegister() converts machine specific values to a register name,
// which is then looked up in a RegisterDictionary to return a RegisterDescriptor.  The entries in the dictionary determine
// what registers are available to the disassembler.
//
// Currently (2010-10-09) the old class and numbers are used as the major and minor values (except in the case of the "cr"
// register), but users should not assume that this is always the case. They can assume that unrelated registers (e.g., "r0"
// and "r1") have descriptors that map to non-overlapping areas of the descriptor address space {major,minor,offset,size} while
// related registers (e.g., "spr8" and "lr") map to overlapping areas of the descriptor address space.
SgAsmRegisterReferenceExpression*
Powerpc::makeRegister(State &state, PowerpcRegisterClass reg_class, int reg_number,
                                  PowerpcConditionRegisterAccessGranularity cr_granularity,
                                  SgAsmType *registerType /*=NULL*/) const {
    // Obtain a register name and override the registerType for certain registers
    std::string name;
    switch (reg_class) {
        case powerpc_regclass_gpr:
            if (reg_number<0 || reg_number>=1024)
                throw ExceptionPowerpc("invalid gpr register number", state);
            name = "r" + StringUtility::numberToString(reg_number);
            switch (wordSize_) {
                case powerpc_32:
                    registerType = SageBuilderAsm::buildTypeU32();
                    break;
                case powerpc_64:
                    registerType = SageBuilderAsm::buildTypeU64();
                    break;
            }
            break;
        case powerpc_regclass_fpr:
            if (reg_number<0 || reg_number>=1024)
                throw ExceptionPowerpc("invalid fpr register number", state);
            name = "f" + StringUtility::numberToString(reg_number);
            registerType = SageBuilderAsm::buildIeee754Binary64();
            break;
        case powerpc_regclass_cr:
            name = "cr";
            switch (cr_granularity) {
                case powerpc_condreggranularity_whole:
                    if (0!=reg_number)
                        throw ExceptionPowerpc("invalid register number for cr", state);
                    break;
                case powerpc_condreggranularity_field:
                    // cr has eight 4-bit fields numbered 0..7 with names like "cr0"
                    if (reg_number<0 || reg_number>=8)
                        throw ExceptionPowerpc("invalid condition register granularity field", state);
                    name += StringUtility::numberToString(reg_number);
                    registerType = SageBuilderAsm::buildTypeU4();
                    break;
                case powerpc_condreggranularity_bit: {
                    // Each field has four bits with names. The full name of each bit is "crF.B" where "F" is the field number
                    // like above and "B" is the bit name. For instance, "cr0.eq".
                    if (reg_number<0 || reg_number>=32)
                        throw ExceptionPowerpc("invalid condition register granularity bit", state);
                    static const char *bitname[] = {"lt", "gt", "eq", "so"};
                    name += StringUtility::numberToString(reg_number/4) + "." + bitname[reg_number%4];
                    registerType = SageBuilderAsm::buildTypeU1();
                    break;
                }
            }
            break;
        case powerpc_regclass_fpscr:
            if (0!=reg_number)
                throw ExceptionPowerpc("invalid register number for fpscr", state);
            name = "fpscr";
            registerType = SageBuilderAsm::buildTypeU32();
            break;
        case powerpc_regclass_spr:
            // Some special purpose registers have special names, but the dictionary has the generic name as well.
            if (reg_number<0 || reg_number>=1024)
                throw ExceptionPowerpc("invalid spr register number", state);
            name = "spr" + StringUtility::numberToString(reg_number);
            switch (wordSize_) {
                case powerpc_32:
                    registerType = SageBuilderAsm::buildTypeU32();
                    break;
                case powerpc_64:
                    registerType = SageBuilderAsm::buildTypeU64();
                    break;
            }
            break;
        case powerpc_regclass_tbr:
            // Some time base registers have special names, but the dictionary has the generic name as well.
            if (reg_number<0 || reg_number>=1024)
                throw ExceptionPowerpc("invalid tbr register number", state);
            name = "tbr" + StringUtility::numberToString(reg_number);
            switch (wordSize_) {
                case powerpc_32:
                    registerType = SageBuilderAsm::buildTypeU32();
                    break;
                case powerpc_64:
                    registerType = SageBuilderAsm::buildTypeU64();
                    break;
            }
            break;
        case powerpc_regclass_msr:
            if (0!=reg_number)
                throw ExceptionPowerpc("invalid msr register number", state);
            name = "msr";
            switch (wordSize_) {
                case powerpc_32:
                    registerType = SageBuilderAsm::buildTypeU32();
                    break;
                case powerpc_64:
                    registerType = SageBuilderAsm::buildTypeU64();
                    break;
            }
            break;
        case powerpc_regclass_sr:
            // FIXME[Robb Matzke 2010-10-09]: not implemented yet
            name = "sr" + StringUtility::numberToString(reg_number);
            break;
        case powerpc_regclass_iar:
            // Instruction address register is not a real register, and so should never appear in disassembled code.
            throw ExceptionPowerpc("iar is not a real register", state);
        case powerpc_regclass_pvr:
            if (0!=reg_number)
                throw ExceptionPowerpc("invalid pvr register number", state);
            name = "pvr";
            registerType = SageBuilderAsm::buildTypeU32();
            break;
        case powerpc_regclass_vector:
            throw ExceptionPowerpc("register class not implemented yet", state);
        case powerpc_regclass_unknown:
        case powerpc_last_register_class:
            throw ExceptionPowerpc("not a real register", state);
        // Don't add a "default" or else we won't get compiler warnings if a new class is defined.
    }
    ASSERT_forbid(name.empty());
    ASSERT_not_null(registerType);

    // Obtain a register descriptor from the dictionary
    const RegisterDescriptor rdesc = architecture()->registerDictionary()->find(name);
    if (!rdesc)
        throw ExceptionPowerpc("register \"" + name + "\" is not available for " + architecture()->registerDictionary()->name(), state);
    ASSERT_require2(rdesc.nBits() == registerType->get_nBits(),
                    (boost::format("register width (%|u|) doesn't match type width (%|u|)")
                     % rdesc.nBits() % registerType->get_nBits()).str());

    // Construct the return value
    SgAsmRegisterReferenceExpression *rre = new SgAsmDirectRegisterExpression(rdesc);
    ASSERT_not_null(rre);
    rre->set_type(registerType);
    return rre;
}

SgAsmPowerpcInstruction*
Powerpc::disassemble(State &state) {
    // The Primary Opcode Field is bits 0-5, 6-bits wide, so there are max 64 primary opcode values
    const uint8_t primaryOpcode = (state.insn >> 26) & 0x3F;

    switch (primaryOpcode) {
        case 0x00: {
            // An instruction consisting of all zero bytes is guaranteed to be illegal and will invoke the illegal instruction error
            // handler and have no other effect.
            if (0 == state.insn)
                return MAKE_INSN0(illegal, uisa);

            // These are the BGL specific PowerPC440 FP2 Architecture instructions.  Depending on if this is A form or X form, the
            // extended opCode maps to different bit ranges, so this code is incorrect!
            const uint8_t a_Opcode = (state.insn >> 1) & 0x1F;

            // Different parts of the instruction are used to identify what kind of instruction this is!
            if (a_Opcode == 5 || (a_Opcode >= 8 && a_Opcode <= 31)) {
                return decode_A_formInstruction_00(state);
            } else {
                return decode_X_formInstruction_00(state);
            }
            break;
        }

        case 0x02: return MAKE_INSN3(tdi, uisa, TO(state), RA(state), SI(state));
        case 0x03: return MAKE_INSN3(twi, uisa, TO(state), RA(state), SI(state));
        case 0x04: return decode_A_formInstruction_04(state);
        case 0x07: return MAKE_INSN3(mulli, uisa, RT(state), RA(state), SI(state));
        case 0x08: return MAKE_INSN3(subfic, uisa, RT(state), RA(state), SI(state));
        case 0x0A:
            if (!reservedOk<9, 9>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN4(cmpli, uisa, BF_cr(state), L_10(state), RA(state), UI(state));
            }
        case 0x0B:
            if (!reservedOk<9, 9>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN4(cmpi, uisa, BF_cr(state), L_10(state), RA(state), SI(state));
            }
        case 0x0C: return MAKE_INSN3(addic, uisa, RT(state), RA(state), SI(state));
        case 0x0D: return MAKE_INSN3(addic_record, uisa, RT(state), RA(state), SI(state));
        case 0x0E: return MAKE_INSN3(addi, uisa, RT(state), RA_or_zero(state), SI(state));
        case 0x0F: return MAKE_INSN3(addis, uisa, RT(state), RA_or_zero(state), SI(state));
        case 0x10: return decode_B_formInstruction(state);
        case 0x11: return decode_SC_formInstruction(state);
        case 0x12: return decode_I_formInstruction(state);
        case 0x13: return decode_XL_formInstruction(state);
        case 0x14: return MAKE_INSN5_RC(rlwimi, uisa, RA(state), RS(state), SH_32bit(state), MB_32bit(state), ME_32bit(state));
        case 0x15: return MAKE_INSN5_RC(rlwinm, uisa, RA(state), RS(state), SH_32bit(state), MB_32bit(state), ME_32bit(state));
        case 0x17: return MAKE_INSN5_RC(rlwnm, uisa, RA(state), RS(state), RB(state), MB_32bit(state), ME_32bit(state));
        case 0x18: return MAKE_INSN3(ori, uisa, RA(state), RS(state), UI(state));
        case 0x19: return MAKE_INSN3(oris, uisa, RA(state), RS(state), UI(state));
        case 0x1A: return MAKE_INSN3(xori, uisa, RA(state), RS(state), UI(state));
        case 0x1B: return MAKE_INSN3(xoris, uisa, RA(state), RS(state), UI(state));
        case 0x1C: return MAKE_INSN3(andi_record, uisa, RA(state), RS(state), UI(state));
        case 0x1D: return MAKE_INSN3(andis_record, uisa, RA(state), RS(state), UI(state));
        case 0x1E: return decode_MD_formInstruction(state);
        case 0x1F: return decode_X_formInstruction_1F(state);
        case 0x20: return MAKE_INSN2(lwz, uisa, RT(state), memref(state, T_U32));
        case 0x21:
            if (fld<11, 15>(state) == 0 || fld<11, 15>(state) == fld<6, 10>(state))
                return MAKE_UNKNOWN();
            return MAKE_INSN2(lwzu, uisa, RT(state), memrefu(state, T_U32));
        case 0x22: return MAKE_INSN2(lbz, uisa, RT(state), memref(state, T_U8));
        case 0x23:
            if (fld<11, 15>(state) == 0 || fld<11, 15>(state) == fld<6, 10>(state))
                return MAKE_UNKNOWN();
            return MAKE_INSN2(lbzu, uisa, RT(state), memrefu(state, T_U8));
        case 0x24: return MAKE_INSN2(stw, uisa, RS(state), memref(state, T_U32));
        case 0x25:
            if (fld<11, 15>(state) == 0)
                return MAKE_UNKNOWN();
            return MAKE_INSN2(stwu, uisa, RS(state), memrefu(state, T_U32));
        case 0x26: return MAKE_INSN2(stb, uisa, RS(state), memref(state, T_U8));
        case 0x27:
            if (fld<11, 15>(state) == 0)
                return MAKE_UNKNOWN();
            return MAKE_INSN2(stbu, uisa, RS(state), memrefu(state, T_U8));
        case 0x28: return MAKE_INSN2(lhz, uisa, RT(state), memref(state, T_U16));
        case 0x29:
            if (fld<11, 15>(state) == 0 || fld<11, 15>(state) == fld<6, 10>(state))
                return MAKE_UNKNOWN();
            return MAKE_INSN2(lhzu, uisa, RT(state), memrefu(state, T_U16));
        case 0x2A: return MAKE_INSN2(lha, uisa, RT(state), memref(state, T_U16));
        case 0x2B:
            if (fld<11, 15>(state) == 0 || fld<11, 15>(state) == fld<6, 10>(state))
                return MAKE_UNKNOWN();
            return MAKE_INSN2(lhau, uisa, RT(state), memrefu(state, T_U16));
        case 0x2C: return MAKE_INSN2(sth, uisa, RS(state), memref(state, T_U16));
        case 0x2D:
            if (fld<11, 15>(state) == 0)
                return MAKE_UNKNOWN();
            return MAKE_INSN2(sthu, uisa, RS(state), memrefu(state, T_U16));
        case 0x2E: return MAKE_INSN2(lmw, uisa, RT(state), memref(state, T_U32));
        case 0x2F: return MAKE_INSN2(stmw, uisa, RS(state), memref(state, T_U32));
        case 0x30: return MAKE_INSN2(lfs, uisa, FRT(state), memref(state, T_FLOAT32));
        case 0x31: return MAKE_INSN2(lfsu, uisa, FRT(state), memrefu(state, T_FLOAT32));
        case 0x32: return MAKE_INSN2(lfd, uisa, FRT(state), memref(state, T_FLOAT64));
        case 0x33: return MAKE_INSN2(lfdu, uisa, FRT(state), memrefu(state, T_FLOAT64));
        case 0x34: return MAKE_INSN2(stfs, uisa, FRS(state), memref(state, T_FLOAT32));
        case 0x35: return MAKE_INSN2(stfsu, uisa, FRS(state), memrefu(state, T_FLOAT32));
        case 0x36: return MAKE_INSN2(stfd, uisa, FRS(state), memref(state, T_FLOAT64));
        case 0x37: return MAKE_INSN2(stfdu, uisa, FRS(state), memrefu(state, T_FLOAT64));
        case 0x3A: {
            switch (state.insn & 0x3) {
                case 0: return MAKE_INSN2(ld, uisa, RT(state), memrefds(state, T_U64));
                case 1:
                    if (fld<11, 15>(state) == 0 || fld<11, 15>(state) == fld<6, 10>(state))
                        return MAKE_UNKNOWN();
                    return MAKE_INSN2(ldu, uisa, RT(state), memrefds(state, T_U64));
                case 2: return MAKE_INSN2(lwa, uisa, RT(state), memrefds(state, T_U32));
                default:
                    return MAKE_UNKNOWN();
            }
            break;
        }
        case 0x3B: return decode_A_formInstruction_3B(state);
        case 0x3E: return decode_DS_formInstruction(state);
        case 0x3F: {
            // Depending on if this is A form or X form, the extended opCode maps to different bit ranges, so this code is
            // incorrect!
            uint8_t  a_Opcode   = (state.insn >> 1) & 0x1F;

            // Different parts of the instruction are used to identify what kind of instruction this is!
            if (a_Opcode == 18 || (a_Opcode >= 20 && a_Opcode <= 31)) {
                return decode_A_formInstruction_3F(state);
            } else {
                return decode_X_formInstruction_3F(state); // Also includes XFL form
            }
            break;
        }

        default:
            return MAKE_UNKNOWN();
    }

    ASSERT_not_reachable("op code not handled");
}

SgAsmPowerpcInstruction*
Powerpc::decode_I_formInstruction(State &state) {
    uint8_t primaryOpcode = (state.insn >> 26) & 0x3F;
    switch(primaryOpcode) {
        case 0x12: {
            Address targetBranchAddress = LI(state);
            if (AA(state) == 0)
                targetBranchAddress += state.ip;

            SgAsmIntegerValueExpression* targetAddressExpression = makeBranchTarget(targetBranchAddress);

            if (AA(state) == 0) {
                if (LK(state) == 0) {
                    return MAKE_INSN1(b, uisa, targetAddressExpression);
                } else {
                    return MAKE_INSN1(bl, uisa, targetAddressExpression);
                }
            } else {
                if (LK(state) == 0) {
                    return MAKE_INSN1(ba, uisa, targetAddressExpression);
                } else {
                    return MAKE_INSN1(bla, uisa, targetAddressExpression);
                }
            }
            break;
        }

        default:
            return MAKE_UNKNOWN();
    }
}

SgAsmPowerpcInstruction*
Powerpc::decode_B_formInstruction(State &state) {
     uint8_t primaryOpcode = (state.insn >> 26) & 0x3F;
     switch(primaryOpcode) {
         case 0x10: {
             uint64_t targetBranchAddress = BD(state);
             if (AA(state) == 0)
                 targetBranchAddress += state.ip;
             SgAsmIntegerValueExpression* targetAddressExpression = makeBranchTarget(targetBranchAddress);

             if (LK(state) == 0) {
                 if (AA(state) == 0) {
                     return MAKE_INSN3(bc, uisa, BO(state), BI(state), targetAddressExpression);
                 } else {
                     return MAKE_INSN3(bca, uisa, BO(state), BI(state), targetAddressExpression);
                 }
             } else {
                 if (AA(state) == 0) {
                     return MAKE_INSN3(bcl, uisa, BO(state), BI(state), targetAddressExpression);
                 } else {
                     return MAKE_INSN3(bcla, uisa, BO(state), BI(state), targetAddressExpression);
                 }
             }
             break;
         }

         default:
             return MAKE_UNKNOWN();
     }
     ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
Powerpc::decode_SC_formInstruction(State &state) {
    uint8_t primaryOpcode = (state.insn >> 26) & 0x3F;
    ASSERT_always_require(primaryOpcode == 0x11);

    // Get bit 30, 1 bit as the reserved flag
    uint8_t constantOneOpcode = (state.insn >> 1) & 0x1;
    if (constantOneOpcode!=1)
        return MAKE_UNKNOWN();

    if (!reservedOk<6, 19>(state) || !reservedOk<27, 29>(state) || !reservedOk<31, 31>(state)) {
        return MAKE_UNKNOWN();
    } else {
        return MAKE_INSN1(sc, uisa, LEV(state));
    }
}

SgAsmPowerpcInstruction*
Powerpc::decode_DS_formInstruction(State &state) {
    uint8_t primaryOpcode = (state.insn >> 26) & 0x3f;
    ASSERT_always_require(primaryOpcode == 0x3e);

    switch (state.insn & 0x03) {
        case 0: return MAKE_INSN2(std, uisa, RS(state), memrefds(state, T_U64));
        case 1:
            if (fld<11, 15>(state) == 0)
                return MAKE_UNKNOWN();
            return MAKE_INSN2(stdu, uisa, RS(state), memrefds(state, T_U64));
        default: return MAKE_UNKNOWN();
    }
}

SgAsmPowerpcInstruction*
Powerpc::decode_X_formInstruction_1F(State &state) {
    const uint8_t primaryOpcode = (state.insn >> 26) & 0x3F;
    ASSERT_always_require(primaryOpcode == 0x1F);

    // Get the bits 21-30, next 10 bits
    const uint16_t xoOpcode = (state.insn >> 1) & 0x3FF;
    switch(xoOpcode) {
        case 0x000:
            if (!reservedOk<9, 9>(state) || !reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN4(cmp, uisa, BF_cr(state), L_10(state), RA(state), RB(state));
            }
        case 0x004:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3(tw, uisa, TO(state), RA(state), RB(state));
            }
        case 0x008: return MAKE_INSN3_RC(subfc, uisa, RT(state), RA(state), RB(state));
        case 0x009:
            if (!reservedOk<21, 21>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3_RC(mulhdu, uisa, RT(state), RA(state), RB(state));
            }
        case 0x00A: return MAKE_INSN3_RC(addc, uisa, RT(state), RA(state), RB(state));
        case 0x00B:
            if (!reservedOk<21, 21>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3_RC(mulhwu, uisa, RT(state), RA(state), RB(state));
            }
        case 0x014: return MAKE_INSN2(lwarx, vea, RT(state), memrefx(state, T_U32));
        case 0x015:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(ldx, uisa, RT(state), memrefx(state, T_U64));
            }
        case 0x017:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(lwzx, uisa, RT(state), memrefx(state, T_U32));
            }
        case 0x018: return MAKE_INSN3_RC(slw, uisa, RA(state), RS(state), RB(state));
        case 0x01A:
            if (!reservedOk<16, 20>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(cntlzw, uisa, RA(state), RS(state));
            }
        case 0x01B: return MAKE_INSN3_RC(sld, uisa, RA(state), RS(state), RB(state));
        case 0x01C: return MAKE_INSN3_RC(and, uisa, RA(state), RS(state), RB(state));
        case 0x020:
            if (!reservedOk<9, 9>(state) || !reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN4(cmpl, uisa, BF_cr(state), L_10(state), RA(state), RB(state));
            }
        case 0x028: return MAKE_INSN3_RC(subf, uisa, RT(state), RA(state), RB(state));
        case 0x035:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else if (fld<11, 15>(state) == 0 || fld<11, 15>(state) == fld<6, 10>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(ldux, uisa, RT(state), memrefux(state, T_U64));
            }
        case 0x037:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else if (fld<11, 15>(state) == 0 || fld<11, 15>(state) == fld<6, 10>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(lwzux, uisa, RT(state), memrefux(state, T_U32));
            }
        case 0x03a:
            if (!reservedOk<16, 20>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(cntlzd, uisa, RA(state), RS(state));
            }
        case 0x03C: return MAKE_INSN3_RC(andc, uisa, RA(state), RS(state), RB(state));
        case 0x044:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3(td, uisa, TO(state), RA(state), RB(state));
            }
        case 0x049:
            if (!reservedOk<21, 21>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3_RC(mulhd, uisa, RT(state), RA(state), RB(state));
            }
        case 0x04B:
            if (!reservedOk<21, 21>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3_RC(mulhw, uisa, RT(state), RA(state), RB(state));
            }
        case 0x053: return MAKE_INSN1(mfmsr, oea, RT(state));
        case 0x057:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(lbzx, uisa, RT(state), memrefx(state, T_U8));
            }
        case 0x068:
            if (!reservedOk<16, 20>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(neg, uisa, RT(state), RA(state));
            }
        case 0x077:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else if (fld<11, 15>(state) == 0 || fld<11, 15>(state) == fld<6, 10>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(lbzux, uisa, RT(state), memrefux(state, T_U8));
            }

        case 0x07A:
            if (!reservedOk<16, 20>(state) || !reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(popcntb, uisa, RA(state), RS(state));
            }
        case 0x07C: return MAKE_INSN3_RC(nor, uisa, RA(state), RS(state), RB(state));
        case 0x088: return MAKE_INSN3_RC(subfe, uisa, RT(state), RA(state), RB(state));
        case 0x08A: return MAKE_INSN3_RC(adde, uisa, RT(state), RA(state), RB(state));
        case 0x08E: return MAKE_INSN2(lfssx, uncategorized, FRT(state), memrefx(state, T_FLOAT32));
        case 0x092:
            if (fld<11, 20>(state) == 0 && fld<31, 31>(state) == 0) {
                return MAKE_INSN1(mtmsr, oea, RS(state));
            } else {
                return MAKE_UNKNOWN();
            }
        case 0x095:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(stdx, uisa, RS(state), memrefx(state, T_U64));
            }
        case 0x096: return MAKE_INSN2(stwcx_record, vea, RS(state), memrefx(state, T_U32));
        case 0x097:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(stwx, uisa, RS(state), memrefx(state, T_U32));
            }
        case 0x0AE: return MAKE_INSN2(lfssux, uncategorized, FRT(state), memrefux(state, T_FLOAT32));
        case 0x0B5:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else if (fld<11, 15>(state) == 0) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(stdux, uisa, RS(state), memrefux(state, T_U64));
            }
        case 0x0B7:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else if (fld<11, 15>(state) == 0) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(stwux, uisa, RS(state), memrefux(state, T_U32));
            }
        case 0x0C8:
            if (!reservedOk<16, 20>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(subfze, uisa, RT(state), RA(state));
            }
        case 0x0CA:
            if (!reservedOk<16, 20>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(addze, uisa, RT(state), RA(state));
            }
        case 0x0CE: return MAKE_INSN2(lfsdx, uncategorized, FRT(state), memrefx(state, T_FLOAT64));
        case 0x0D6: return MAKE_INSN2(stdcx_record, vea, RS(state), memrefx(state, T_U64));
        case 0x0D7:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(stbx, uisa, RS(state), memrefx(state, T_U8));
            }
        case 0x0E8:
            if (!reservedOk<16, 20>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(subfme, uisa, RT(state), RA(state));
            }
        case 0x0EA:
            if (!reservedOk<16, 20>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(addme, uisa, RT(state), RA(state));
            }
        case 0x0EB: return MAKE_INSN3_RC(mullw, uisa, RT(state), RA(state), RB(state));
        case 0x0EE: return MAKE_INSN2(lfsdux, uncategorized, FRT(state), memrefux(state, T_FLOAT64));
        case 0x0f4: return MAKE_INSN3_RC(mulld, uisa, RT(state), RA(state), RB(state));
        case 0x0F7:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else if (fld<11, 15>(state) == 0) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(stbux, uisa, RS(state), memrefux(state, T_U8));
            }
        case 0x10A: return MAKE_INSN3_RC(add, uisa, RT(state), RA(state), RB(state));
        case 0x10E: return MAKE_INSN2(lfxsx, uncategorized, FRT(state), memrefx(state, T_V2_FLOAT32));
        case 0x116: return MAKE_INSN1(dcbt, vea, memrefx(state, T_U8));
        case 0x117:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(lhzx, uisa, RT(state), memrefx(state, T_U16));
            }
        case 0x11C: return MAKE_INSN3_RC(eqv, uisa, RA(state), RS(state), RB(state));
        case 0x12E: return MAKE_INSN2(lfxsux, uncategorized, FRT(state), memrefux(state, T_V2_FLOAT32));
        case 0x132:
            if (fld<6, 15>(state) == 0 && fld<31, 31>(state) == 0)
                return MAKE_INSN1(tlbie, oea, RB(state));
            return MAKE_UNKNOWN();
        case 0x137:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else if (fld<11, 15>(state) == 0 || fld<11, 15>(state) == fld<6, 10>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(lhzux, uisa, RT(state), memrefux(state, T_U16));
            }
        case 0x13C: return MAKE_INSN3_RC(xor, uisa, RA(state), RS(state), RB(state));
        case 0x14E: return MAKE_INSN2(lfxdx, uncategorized, FRT(state), memrefx(state, T_V2_FLOAT64));
        case 0x153:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(mfspr, uisa, RT(state), SPR(state));
            }
        case 0x155:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(lwax, uisa, RT(state), memrefx(state, T_U32));
            }
        case 0x157:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(lhax, uisa, RT(state), memrefx(state, T_U16));
            }
        case 0x16E: return MAKE_INSN2(lfxdux, uncategorized, FRT(state), memrefux(state, T_V2_FLOAT64));
        case 0x173:
            if (fld<31, 31>(state) == 0) {
                const unsigned tbr = fld<16, 20>(state) * 32 + fld<11, 15>(state);
                if (tbr == 0b01000'01100 || tbr == 0b01000'01101)
                    return MAKE_INSN2(mftb, vea, D(state), TBR(state));
            }
            return MAKE_UNKNOWN();
        case 0x175:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else if (fld<11, 15>(state) == 0 || fld<11, 15>(state) == fld<6, 10>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(lwaux, uisa, RT(state), memrefux(state, T_U32));
            }
        case 0x177:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else if (fld<11, 15>(state) == 0 || fld<11, 15>(state) == fld<6, 10>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(lhaux, uisa, RT(state), memrefux(state, T_U16));
            }
        case 0x18E: return MAKE_INSN2(lfpsx, uncategorized, FRT(state), memrefx(state, T_V2_FLOAT32));
        case 0x197:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(sthx, uisa, RS(state), memrefx(state, T_U16));
            }
        case 0x19C: return MAKE_INSN3_RC(orc, uisa, RA(state), RS(state), RB(state));
        case 0x1AE: return MAKE_INSN2(lfpsux, uncategorized, FRT(state), memrefux(state, T_V2_FLOAT32));
        case 0x1B7:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else if (fld<11, 15>(state) == 0) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(sthux, uisa, RS(state), memrefux(state, T_U16));
            }
        case 0x1BC: return MAKE_INSN3_RC(or, uisa, RA(state), RS(state), RB(state));
        case 0x1C9: return MAKE_INSN3_RC(divdu, uisa, RT(state), RA(state), RB(state));
        case 0x1CB: return MAKE_INSN3_RC(divwu, uisa, RT(state), RA(state), RB(state));
        case 0x1CE: return MAKE_INSN2(lfpdx, uncategorized, FRT(state), memrefx(state, T_V2_FLOAT64));
        case 0x1D3:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(mtspr, uisa, SPR(state), RS(state));
            }
        case 0x1DC: return MAKE_INSN3_RC(nand, uisa, RA(state), RS(state), RB(state));
        case 0x1E9: return MAKE_INSN3_RC(divd, uisa, RT(state), RA(state), RB(state));
        case 0x1EB: return MAKE_INSN3_RC(divw, uisa, RT(state), RA(state), RB(state));
        case 0x1EE: return MAKE_INSN2(lfpdux, uncategorized, FRT(state), memrefux(state, T_V2_FLOAT64));
        case 0x208: return MAKE_INSN3_RC(subfco, uisa, RT(state), RA(state), RB(state));
        case 0x20A: return MAKE_INSN3_RC(addco, uisa, RT(state), RA(state), RB(state));
        case 0x20E: return MAKE_INSN2(stfpiwx, uncategorized, FRT(state), memrefx(state, T_V2_U32));
        case 0x215:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(lswx, uisa, RT(state), memrefx(state, T_U8));
            }
        case 0x216:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(lwbrx, uisa, RT(state), memrefx(state, T_U32));
            }
        case 0x217:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(lfsx, uisa, FRT(state), memrefx(state, T_FLOAT32));
            }
        case 0x218: return MAKE_INSN3_RC(srw, uisa, RA(state), RS(state), RB(state));
        case 0x21B: return MAKE_INSN3_RC(srd, uisa, RA(state), RS(state), RB(state));
        case 0x228: return MAKE_INSN3_RC(subfo, uisa, RT(state), RA(state), RB(state));
        case 0x237:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(lfsux, uisa, FRT(state), memrefux(state, T_FLOAT32));
            }
        case 0x255:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3(lswi, uisa, RT(state), memrefra(state, T_U8), NB(state));
            }
        case 0x256:
            if (fld<6, 20>(state) == 0 && fld<31, 31>(state) == 0) {
                return MAKE_INSN0(sync, vea);
            } else {
                return MAKE_UNKNOWN();
            }
        case 0x257:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(lfdx, uisa, FRT(state), memrefx(state, T_FLOAT64));
            }
        case 0x268:
            if (!reservedOk<16, 20>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(nego, uisa, RT(state), RA(state));
            }
        case 0x288: return MAKE_INSN3_RC(subfeo, uisa, RT(state), RA(state), RB(state));
        case 0x28A: return MAKE_INSN3_RC(addeo, uisa, RT(state), RA(state), RB(state));
        case 0x28E: return MAKE_INSN2(stfssx, uncategorized, FRT(state), memrefx(state, T_FLOAT32));
        case 0x295:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(stswx, uisa, RS(state), memrefx(state, T_U8));
            }
        case 0x296:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(stwbrx, uisa, RS(state), memrefx(state, T_U32));
            }
        case 0x2AE: return MAKE_INSN2(stfssux, uncategorized, FRT(state), memrefux(state, T_FLOAT32));
        case 0x2C8:
            if (!reservedOk<16, 20>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(subfzeo, uisa, RT(state), RA(state));
            }
        case 0x2CA:
            if (!reservedOk<16, 20>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(addzeo, uisa, RT(state), RA(state));
            }
        case 0x2CE: return MAKE_INSN2(stfsdx, uncategorized, FRT(state), memrefx(state, T_FLOAT64));
        case 0x2D5:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3(stswi, uisa, RS(state), memrefra(state, T_U8), NB(state));
            }
        case 0x2D7:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(stfdx, uisa, FRS(state), memrefx(state, T_FLOAT64));
            }
        case 0x2E8:
            if (!reservedOk<16, 20>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(subfmeo, uisa, RT(state), RA(state));
            }
        case 0x2EA:
            if (!reservedOk<16, 20>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(addmeo, uisa, RT(state), RA(state));
            }
        case 0x2EB: return MAKE_INSN3_RC(mullwo, uisa, RT(state), RA(state), RB(state));
        case 0x2EE: return MAKE_INSN2(stfsdux, uncategorized, FRT(state), memrefux(state, T_FLOAT64));
        case 0x2F4: return MAKE_INSN3_RC(mulldo, uisa, RT(state), RA(state), RB(state));
        case 0x2F7:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(stfdux, uisa, FRS(state), memrefx(state, T_FLOAT64));
            }
        case 0x30A: return MAKE_INSN3_RC(addo, uisa, RT(state), RA(state), RB(state));
        case 0x30E: return MAKE_INSN2(stfxsx, uncategorized, FRT(state), memrefx(state, T_V2_FLOAT32));
        case 0x316:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(lhbrx, uisa, RT(state), memrefx(state, T_U16));
            }
        case 0x318: return MAKE_INSN3_RC(sraw, uisa, RA(state), RS(state), RB(state));
        case 0x31A: return MAKE_INSN3_RC(srad, uisa, RA(state), RS(state), RB(state));
        case 0x32E: return MAKE_INSN2(stfxsux, uncategorized, FRT(state), memrefux(state, T_V2_FLOAT32));
        case 0x338: return MAKE_INSN3_RC(srawi, uisa, RA(state), RS(state), SH_32bit(state));
        case 0x33A: return MAKE_INSN3_RC(sradi, uisa, RA(state), RS(state), SH_64bit(state)); // The last bit of the ext. opcode is part of SH
        case 0x33B: return MAKE_INSN3_RC(sradi, uisa, RA(state), RS(state), SH_64bit(state)); // Same as previous insn
        case 0x34E: return MAKE_INSN2(stfxdx, uncategorized, FRT(state), memrefx(state, T_V2_FLOAT64));
        case 0x356: return MAKE_INSN0(eieio, vea);
        case 0x36E: return MAKE_INSN2(stfxdux, uncategorized, FRT(state), memrefux(state, T_V2_FLOAT64));
        case 0x38E: return MAKE_INSN2(stfpsx, uncategorized, FRT(state), memrefx(state, T_V2_FLOAT32));
        case 0x396:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(sthbrx, uisa, RS(state), memrefx(state, T_U16));
            }
        case 0x39A: return MAKE_INSN2_RC(extsh, uisa, RA(state), RS(state));
        case 0x3AE: return MAKE_INSN2(stfpsux, uncategorized, FRT(state), memrefux(state, T_V2_FLOAT32));
        case 0x3BA: return MAKE_INSN2_RC(extsb, uisa, RA(state), RS(state));
        case 0x3C9: return MAKE_INSN3_RC(divduo, uisa, RT(state), RA(state), RB(state));
        case 0x3CB: return MAKE_INSN3_RC(divwuo, uisa, RT(state), RA(state), RB(state));
        case 0x3CE: return MAKE_INSN2(stfpdx, uncategorized, FRT(state), memrefx(state, T_V2_FLOAT64));
        case 0x3D7:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(stfiwx, uisa, FRS(state), memrefx(state, T_U32));
            }
        case 0x3DA: return MAKE_INSN2_RC(extsw, uisa, RA(state), RS(state));
        case 0x3E9: return MAKE_INSN3_RC(divdo, uisa, RT(state), RA(state), RB(state));
        case 0x3EB: return MAKE_INSN3_RC(divwo, uisa, RT(state), RA(state), RB(state));
        case 0x3EE: return MAKE_INSN2(stfpdux, uncategorized, FRT(state), memrefux(state, T_V2_FLOAT64));
        case 0x3F6: return MAKE_INSN1(dcbz, vea, memrefx(state, T_U8));
        case 0x13:
            if (!reservedOk<12, 20>(state) || !reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else if (fld<11, 11>(state) != 0) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN1(mfcr, uisa, RT(state));
            }
            break;
        case 0x90:
            if (!reservedOk<20, 20>(state) || !reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else if (fld<11, 11>(state) != 0) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(mtcrf, uisa, FXM(state), RS(state));
            }
            break;
        default:
            return MAKE_UNKNOWN();
    }
    ASSERT_not_reachable("unhandled opcode");
}

SgAsmPowerpcInstruction*
Powerpc::decode_X_formInstruction_3F(State &state) {
    uint8_t primaryOpcode = (state.insn >> 26) & 0x3F;
    ASSERT_always_require(primaryOpcode == 0x3F);

    // Get the bits 21-30, next 10 bits
    uint16_t xoOpcode = (state.insn >> 1) & 0x3FF;
    switch(xoOpcode) {
        case 0x000:
            if (!reservedOk<9, 10>(state) || !reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3(fcmpu, uisa, BF_cr(state), FRA(state), FRB(state));
            }
        case 0x00C:
            if (!reservedOk<11, 15>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(frsp, uisa, FRT(state), FRB(state));
            }
        case 0x00E:
            if (!reservedOk<11, 15>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(fctiw, uisa, FRT(state), FRB(state));
            }
        case 0x00F:
            if (!reservedOk<11, 15>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(fctiwz, uisa, FRT(state), FRB(state));
            }
        case 0x026:
            if (!reservedOk<11, 20>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN1(mtfsb1, uisa, BT(state));
            }
        case 0x028:
            if (!reservedOk<11, 15>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(fneg, uisa, FRT(state), FRB(state));
            }
        case 0x046:
            if (!reservedOk<11, 20>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN1_RC(mtfsb0, uisa, BT(state));
            }
        case 0x048:
            if (!reservedOk<11, 15>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(fmr, uisa, FRT(state), FRB(state));
            }
        case 0x086:
            if (!reservedOk<9, 15>(state) || !reservedOk<20, 20>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(mtfsfi, uisa, BF_fpscr(state), U(state));
            }
        case 0x088:
            if (!reservedOk<11, 15>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(fnabs, uisa, FRT(state), FRB(state));
            }
        case 0x108:
            if (!reservedOk<11, 15>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(fabs, uisa, FRT(state), FRB(state));
            }
        case 0x247:
            if (!reservedOk<11, 20>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN1_RC(mffs, uisa, FRT(state));
            }
        case 0x2C7:
            if (!reservedOk<6, 6>(state) || !reservedOk<15, 15>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(mtfsf, uisa, FLM(state), FRB(state));
            }
        default: return MAKE_UNKNOWN();

    }
    ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
Powerpc::decode_X_formInstruction_00(State &state) {
    if (state.insn == 0)
        return MAKE_UNKNOWN();

    // Get the bits 21-30, next 10 bits
    uint16_t xoOpcode = (state.insn >> 1) & 0x3FF;
    switch(xoOpcode) {
        case 0x020: return MAKE_INSN2(fpmr, 440fpu, FRT(state), FRB(state));
        case 0x060: return MAKE_INSN2(fpabs, 440fpu, FRT(state), FRB(state));
        case 0x0A0: return MAKE_INSN2(fpneg, 440fpu, FRT(state), FRB(state));
        case 0x0C0: return MAKE_INSN2(fprsp, 440fpu, FRT(state), FRB(state));
        case 0x0E0: return MAKE_INSN2(fpnabs, 440fpu, FRT(state), FRB(state));
        case 0x120: return MAKE_INSN2(fsmr, 440fpu, FRT(state), FRB(state));
        case 0x160: return MAKE_INSN2(fsabs, 440fpu, FRT(state), FRB(state));
        case 0x1A0: return MAKE_INSN2(fsneg, 440fpu, FRT(state), FRB(state));
        case 0x1E0: return MAKE_INSN2(fsnabs, 440fpu, FRT(state), FRB(state));
        case 0x220: return MAKE_INSN2(fxmr, 440fpu, FRT(state), FRB(state));
        case 0x240: return MAKE_INSN2(fpctiw, 440fpu, FRT(state), FRB(state));
        case 0x2C0: return MAKE_INSN2(fpctiwz, 440fpu, FRT(state), FRB(state));
        case 0x320: return MAKE_INSN2(fsmtp, 440fpu, FRT(state), FRB(state));
        case 0x3A0: return MAKE_INSN2(fsmfp, 440fpu, FRT(state), FRB(state));
        default: return MAKE_UNKNOWN();
    }
    ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
Powerpc::decode_XL_formInstruction(State &state) {
    // The primaryOpcode
    uint8_t primaryOpcode = (state.insn >> 26) & 0x3F;
    ASSERT_always_require(primaryOpcode == 0x13);

    // Get the bits 21-30, next 10 bits
    uint16_t xoOpcode = (state.insn >> 1) & 0x3FF;
    switch(xoOpcode) {
        case 0x000:
            if (!reservedOk<9, 10>(state) || !reservedOk<14, 20>(state) || !reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2(mcrf, uisa, BF_cr(state), BFA_cr(state));
            }
        case 0x010:
            if (!reservedOk<16, 18>(state)) {
                return MAKE_UNKNOWN();
            } else if (LK(state) == 0) {
                return MAKE_INSN3(bclr, uisa, BO(state), BI(state), BH(state));
            } else {
                return MAKE_INSN3(bclrl, uisa, BO(state), BI(state), BH(state));
            }
            break;
        case 0x021:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3(crnor, uisa, BT(state), BA(state), BB(state));
            }
        case 0x32:
            if (fld<6, 20>(state) == 0 && fld<31, 31>(state) == 0)
                return MAKE_INSN0(rfi, uncategorized);
            return MAKE_UNKNOWN();
        case 0x081:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3(crandc, uisa, BT(state), BA(state), BB(state));
            }
        case 0x096:
            if (fld<6, 20>(state) == 0 && fld<31, 31>(state) == 0)
                return MAKE_INSN0(isync, vea);
            return MAKE_UNKNOWN();
        case 0x0C1:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3(crxor, uisa, BT(state), BA(state), BB(state));
            }
        case 0x0E1:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3(crnand, uisa, BT(state), BA(state), BB(state));
            }
        case 0x101:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3(crand, uisa, BT(state), BA(state), BB(state));
            }
        case 0x121:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3(creqv, uisa, BT(state), BA(state), BB(state));
            }
        case 0x1A1:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3(crorc, uisa, BT(state), BA(state), BB(state));
            }
        case 0x1C1:
            if (!reservedOk<31, 31>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3(cror, uisa, BT(state), BA(state), BB(state));
            }
        case 0x210:
            if (!reservedOk<16, 18>(state)) {
                return MAKE_UNKNOWN();
            } else if (LK(state) == 0) {
                return MAKE_INSN3(bcctr, uisa, BO(state), BI(state), BH(state));
            } else {
                return MAKE_INSN3(bcctrl, uisa, BO(state), BI(state), BH(state));
            }
            break;

        default:
            return MAKE_UNKNOWN();
    }

    ASSERT_not_reachable("xoOpcode not handled");
}

SgAsmPowerpcInstruction*
Powerpc::decode_A_formInstruction_00(State &state) {
    switch(fld<26, 30>(state)) {
        case 0x05: return MAKE_INSN4(fpsel, 440fpu, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x08: return MAKE_INSN3(fpmul, 440fpu, FRT(state), FRA(state), FRC(state));
        case 0x09: return MAKE_INSN3(fxmul, 440fpu, FRT(state), FRA(state), FRC(state));
        case 0x0A: return MAKE_INSN3(fxpmul, 440fpu, FRT(state), FRA(state), FRC(state));
        case 0x0B: return MAKE_INSN3(fxsmul, 440fpu, FRT(state), FRA(state), FRC(state));
        case 0x0C: return MAKE_INSN3(fpadd, 440fpu, FRT(state), FRA(state), FRB(state));
        case 0x0D: return MAKE_INSN3(fpsub, 440fpu, FRT(state), FRA(state), FRB(state));
        case 0x0E: return MAKE_INSN2(fpre, 440fpu, FRT(state), FRB(state));
        case 0x0F: return MAKE_INSN2(fprsqrte, 440fpu, FRT(state), FRB(state));
        case 0x10: return MAKE_INSN4(fpmadd, 440fpu, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x11: return MAKE_INSN4(fxmadd, 440fpu, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x12: return MAKE_INSN4(fxcpmadd, 440fpu, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x13: return MAKE_INSN4(fxcsmadd, 440fpu, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x14: return MAKE_INSN4(fpnmadd, 440fpu, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x15: return MAKE_INSN4(fxnmadd, 440fpu, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x16: return MAKE_INSN4(fxcpnmadd, 440fpu, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x17: return MAKE_INSN4(fxcsnmadd, 440fpu, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x18: return MAKE_INSN4(fpmsub, 440fpu, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x19: return MAKE_INSN4(fxmsub, 440fpu, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1A: return MAKE_INSN4(fxcpmsub, 440fpu, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1B: return MAKE_INSN4(fxcsmsub, 440fpu, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1C: return MAKE_INSN4(fpnmsub, 440fpu, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1D: return MAKE_INSN4(fxnmsub, 440fpu, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1E: return MAKE_INSN4(fxcpnmsub, 440fpu, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1F: return MAKE_INSN4(fxcsnmsub, 440fpu, FRT(state), FRA(state), FRB(state), FRC(state));
        default: return MAKE_UNKNOWN();
    }
    ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
Powerpc::decode_A_formInstruction_04(State &state) {
    // FIXME: Make the floating point registers use the powerpc_regclass_fpr instead of powerpc_regclass_gpr
    switch(fld<26, 30>(state)) {
        case 0x18: return MAKE_INSN4(fxcpnpma, uncategorized, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x19: return MAKE_INSN4(fxcsnpma, uncategorized, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1A: return MAKE_INSN4(fxcpnsma, uncategorized, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1B: return MAKE_INSN4(fxcsnsma, uncategorized, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1C: return MAKE_INSN4(fxcxma, uncategorized, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1D: return MAKE_INSN4(fxcxnpma, uncategorized, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1E: return MAKE_INSN4(fxcxnsma, uncategorized, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1F: return MAKE_INSN4(fxcxnms, uncategorized, FRT(state), FRA(state), FRB(state), FRC(state));
        default: return MAKE_UNKNOWN();
    }
    ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
Powerpc::decode_A_formInstruction_3B(State &state) {
    switch(fld<26, 30>(state)) {
        case 0x12:
            if (!reservedOk<21, 25>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3_RC(fdivs, uisa, FRT(state), FRA(state), FRB(state));
            }
        case 0x14:
            if (!reservedOk<21, 25>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3_RC(fsubs, uisa, FRT(state), FRA(state), FRB(state));
            }
        case 0x15:
            if (!reservedOk<21, 25>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3_RC(fadds, uisa, FRT(state), FRA(state), FRB(state));
            }
        case 0x16:
            if (!reservedOk<11, 15>(state) || !reservedOk<21, 25>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(fsqrts, uisa, FRT(state), FRB(state));
            }
        case 0x18:
            if (!reservedOk<11, 15>(state) || !reservedOk<21, 25>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(fres, uisa, FRT(state), FRB(state));
            }
        case 0x19:
            if (!reservedOk<16, 20>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3_RC(fmuls, uisa, FRT(state), FRA(state), FRC(state));
            }
        case 0x1A:
            if (!reservedOk<11, 15>(state) || !reservedOk<21, 25>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(frsqrtes, uisa, FRT(state), FRB(state));
            }
        case 0x1C: return MAKE_INSN4_RC(fmsubs, uisa, FRT(state), FRA(state), FRC(state), FRB(state));
        case 0x1D: return MAKE_INSN4_RC(fmadds, uisa, FRT(state), FRA(state), FRC(state), FRB(state));
        case 0x1E: return MAKE_INSN4_RC(fnmsubs, uisa, FRT(state), FRA(state), FRC(state), FRB(state));
        case 0x1F: return MAKE_INSN4_RC(fnmadds, uisa, FRT(state), FRA(state), FRC(state), FRB(state));
        default: return MAKE_UNKNOWN();
    }
    ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
Powerpc::decode_A_formInstruction_3F(State &state) {
    // FIXME: Make the floating point registers use the powerpc_regclass_fpr instead of powerpc_regclass_gpr
    switch(fld<26, 30>(state)) {
        case 0x12:
            if (!reservedOk<21, 25>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3_RC(fdiv, uisa, FRT(state), FRA(state), FRB(state));
            }
        case 0x14:
            if (!reservedOk<21, 25>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3_RC(fsub, uisa, FRT(state), FRA(state), FRB(state));
            }
        case 0x15:
            if (!reservedOk<21, 25>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3_RC(fadd, uisa, FRT(state), FRA(state), FRB(state));
            }
        case 0x16:
            if (!reservedOk<11, 15>(state) || !reservedOk<21, 25>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(fsqrt, uisa, FRT(state), FRB(state));
            }
        case 0x17:
            return MAKE_INSN4_RC(fsel, uisa, FRT(state), FRA(state), FRC(state), FRB(state));
        case 0x18:
            if (!reservedOk<11, 15>(state) || !reservedOk<21, 25>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(fre, uisa, FRT(state), FRB(state));
            }
        case 0x19:
            if (!reservedOk<16, 20>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN3_RC(fmul, uisa, FRT(state), FRA(state), FRC(state));
            }
        case 0x1A:
            if (!reservedOk<11, 15>(state) || !reservedOk<21, 25>(state)) {
                return MAKE_UNKNOWN();
            } else {
                return MAKE_INSN2_RC(frsqrte, uisa, FRT(state), FRB(state));
            }
        case 0x1C: return MAKE_INSN4_RC(fmsub, uisa, FRT(state), FRA(state), FRC(state), FRB(state));
        case 0x1D: return MAKE_INSN4_RC(fmadd, uisa, FRT(state), FRA(state), FRC(state), FRB(state));
        case 0x1E: return MAKE_INSN4_RC(fnmsub, uisa, FRT(state), FRA(state), FRC(state), FRB(state));
        case 0x1F: return MAKE_INSN4_RC(fnmadd, uisa, FRT(state), FRA(state), FRC(state), FRB(state));
        default: return MAKE_UNKNOWN();
    }
    ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
Powerpc::decode_MD_formInstruction(State &state) {
    switch (fld<27, 29>(state)) {
        case 0: return MAKE_INSN4_RC(rldicl, uisa, RA(state), RS(state), SH_64bit(state), MB_64bit(state));
        case 1: return MAKE_INSN4_RC(rldicr, uisa, RA(state), RS(state), SH_64bit(state), ME_64bit(state));
        case 2: return MAKE_INSN4_RC(rldic, uisa, RA(state), RS(state), SH_64bit(state), MB_64bit(state));
        case 3: return MAKE_INSN4_RC(rldimi, uisa, RA(state), RS(state), SH_64bit(state), MB_64bit(state));
        case 4: return decode_MDS_formInstruction(state);
        default: return MAKE_UNKNOWN();
    }
    ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
Powerpc::decode_MDS_formInstruction(State &state) {
    switch (fld<27, 30>(state)) {
        case 8: return MAKE_INSN4_RC(rldcl, uisa, RA(state), RS(state), RB(state), MB_64bit(state));
        case 9: return MAKE_INSN4_RC(rldcr, uisa, RA(state), RS(state), RB(state), ME_64bit(state));
        default: return MAKE_UNKNOWN();
    }
    ASSERT_not_reachable("opcode not handled");
}

bool
Powerpc::AA(State &state) const {
    return fld<30, 30>(state);
}

SgAsmRegisterReferenceExpression*
Powerpc::BA(State &state) const {
    return makeRegister(state, powerpc_regclass_cr, fld<11, 15>(state), powerpc_condreggranularity_bit);
}

SgAsmRegisterReferenceExpression*
Powerpc::BB(State &state) const {
    return makeRegister(state, powerpc_regclass_cr, fld<16, 20>(state), powerpc_condreggranularity_bit);
}

uint64_t
Powerpc::BD(State &state) const {
    return IntegerOps::signExtend<16, 64>((uint64_t)state.insn & 0xfffc);
}

SgAsmRegisterReferenceExpression*
Powerpc::BF_cr(State &state) const {
    return makeRegister(state, powerpc_regclass_cr, fld<6, 8>(state), powerpc_condreggranularity_field);
}

SgAsmRegisterReferenceExpression*
Powerpc::BF_fpscr(State &state) const {
    return makeRegister(state, powerpc_regclass_fpscr, fld<6, 8>(state), powerpc_condreggranularity_field);
}

SgAsmRegisterReferenceExpression*
Powerpc::BFA_cr(State &state) const {
    return makeRegister(state, powerpc_regclass_cr, fld<11, 13>(state), powerpc_condreggranularity_field);
}

SgAsmRegisterReferenceExpression*
Powerpc::BFA_fpscr(State &state) const {
    return makeRegister(state, powerpc_regclass_fpscr, fld<11, 13>(state), powerpc_condreggranularity_field);
}

SgAsmValueExpression*
Powerpc::BH(State &state) const {
    return SageBuilderAsm::buildValueU8(fld<19, 20>(state));
}

SgAsmRegisterReferenceExpression*
Powerpc::BI(State &state) const {
    return BA(state);
}

SgAsmValueExpression*
Powerpc::BO(State &state) const {
    return SageBuilderAsm::buildValueU8(fld<6, 10>(state));
}

SgAsmRegisterReferenceExpression*
Powerpc::BT(State &state) const {
    return makeRegister(state, powerpc_regclass_cr, fld<6, 10>(state), powerpc_condreggranularity_bit);
}

SgAsmValueExpression*
Powerpc::D(State &state) const {
    switch (wordSize_) {
        case powerpc_32:
            return SageBuilderAsm::buildValueU32(IntegerOps::signExtend<16, 32>((uint64_t)fld<16, 31>(state)));
        case powerpc_64:
            return SageBuilderAsm::buildValueU64(IntegerOps::signExtend<16, 64>((uint64_t)fld<16, 31>(state)));
    }
    ASSERT_not_reachable("invalid word size");
}

SgAsmValueExpression*
Powerpc::DS(State &state) const {
    switch (wordSize_) {
        case powerpc_32:
            return SageBuilderAsm::buildValueU32(IntegerOps::signExtend<16, 32>((uint64_t)fld<16, 31>(state) & 0xfffc));
        case powerpc_64:
            return SageBuilderAsm::buildValueU64(IntegerOps::signExtend<16, 64>((uint64_t)fld<16, 31>(state) & 0xfffc));
    }
    ASSERT_not_reachable("invalid word size");
}

SgAsmValueExpression*
Powerpc::FLM(State &state) const {
    return SageBuilderAsm::buildValueU8(fld<7, 14>(state));
}

SgAsmRegisterReferenceExpression*
Powerpc::FRA(State &state) const {
    return makeRegister(state, powerpc_regclass_fpr, fld<11, 15>(state));
}

SgAsmRegisterReferenceExpression*
Powerpc::FRB(State &state) const {
    return makeRegister(state, powerpc_regclass_fpr, fld<16, 20>(state));
}

SgAsmRegisterReferenceExpression*
Powerpc::FRC(State &state) const {
    return makeRegister(state, powerpc_regclass_fpr, fld<21, 25>(state));
}

SgAsmRegisterReferenceExpression*
Powerpc::FRS(State &state) const {
    return makeRegister(state, powerpc_regclass_fpr, fld<6, 10>(state));
}

SgAsmRegisterReferenceExpression*
Powerpc::FRT(State &state) const {
    return FRS(state);
}

SgAsmValueExpression*
Powerpc::FXM(State &state) const {
    return SageBuilderAsm::buildValueU8(fld<12, 19>(state));
}

SgAsmValueExpression*
Powerpc::L_10(State &state) const {
    return SageBuilderAsm::buildValueU8(fld<10, 10>(state));
}

SgAsmValueExpression*
Powerpc::L_15(State &state) const {
    return SageBuilderAsm::buildValueU8(fld<15, 15>(state));
}

uint8_t
Powerpc::L_sync(State &state) const {
    return fld<9, 10>(state);
}

SgAsmValueExpression*
Powerpc::LEV(State &state) const {
    return SageBuilderAsm::buildValueU8(fld<20, 26>(state));
}

uint64_t
Powerpc::LI(State &state) const {
    return IntegerOps::signExtend<26, 64>(uint64_t(fld<6, 29>(state) * 4));
}

bool
Powerpc::LK(State &state) const {
    return fld<31, 31>(state);
}

SgAsmValueExpression*
Powerpc::MB_32bit(State &state) const {
    return SageBuilderAsm::buildValueU8(fld<21, 25>(state));
}

SgAsmValueExpression*
Powerpc::ME_32bit(State &state) const {
    return SageBuilderAsm::buildValueU8(fld<26, 30>(state));
}

SgAsmValueExpression*
Powerpc::MB_64bit(State &state) const {
    return SageBuilderAsm::buildValueU8(fld<21, 25>(state) + 32 * fld<26, 26>(state));
}

SgAsmValueExpression*
Powerpc::ME_64bit(State &state) const {
    return SageBuilderAsm::buildValueU8(fld<21, 25>(state) + 32 * fld<26, 26>(state));
}

SgAsmValueExpression*
Powerpc::NB(State &state) const {
    return SageBuilderAsm::buildValueU8(fld<16, 20>(state) == 0 ? 32 : fld<16, 20>(state));
}

bool
Powerpc::OE(State &state) const {
    return fld<21, 21>(state);
}

SgAsmRegisterReferenceExpression*
Powerpc::RA(State &state) const {
    return makeRegister(state, powerpc_regclass_gpr, fld<11, 15>(state));
}

SgAsmExpression*
Powerpc::RA_or_zero(State &state) const {
    return fld<11, 15>(state) == 0 ? (SgAsmExpression*)SageBuilderAsm::buildValueU8(0) : RA(state);
}

SgAsmRegisterReferenceExpression*
Powerpc::RB(State &state) const {
    return makeRegister(state, powerpc_regclass_gpr, fld<16, 20>(state));
}

bool
Powerpc::Rc(State &state) const {
    return fld<31, 31>(state);
}

SgAsmRegisterReferenceExpression*
Powerpc::RS(State &state) const {
    return makeRegister(state, powerpc_regclass_gpr, fld<6, 10>(state));
}

SgAsmRegisterReferenceExpression*
Powerpc::RT(State &state) const {
    return RS(state);
}

SgAsmValueExpression*
Powerpc::SH_32bit(State &state) const {
    return SageBuilderAsm::buildValueU8(fld<16, 20>(state));
}

SgAsmValueExpression*
Powerpc::SH_64bit(State &state) const {
    return SageBuilderAsm::buildValueU8(fld<16, 20>(state) + fld<30, 30>(state) * 32); // FIXME check
}

SgAsmValueExpression*
Powerpc::SI(State &state) const {
    return D(state);
}

SgAsmRegisterReferenceExpression*
Powerpc::SPR(State &state) const {
    return makeRegister(state, powerpc_regclass_spr, fld<16, 20>(state) * 32 + fld<11, 15>(state));
}

SgAsmRegisterReferenceExpression*
Powerpc::SR(State &state) const {
    return makeRegister(state, powerpc_regclass_sr, fld<12, 15>(state));
}

SgAsmRegisterReferenceExpression*
Powerpc::TBR(State &state) const {
    return makeRegister(state, powerpc_regclass_tbr, fld<16, 20>(state) * 32 + fld<11, 15>(state));
}

SgAsmValueExpression*
Powerpc::TH(State &state) const {
    return SageBuilderAsm::buildValueU8(fld<9, 10>(state));
}

SgAsmValueExpression*
Powerpc::TO(State &state) const {
    return SageBuilderAsm::buildValueU8(fld<6, 10>(state));
}

SgAsmValueExpression*
Powerpc::U(State &state) const {
    return SageBuilderAsm::buildValueU8(fld<16, 19>(state));
}

SgAsmValueExpression*
Powerpc::UI(State &state) const {
    switch (wordSize_) {
        case powerpc_32:
            return SageBuilderAsm::buildValueU32(fld<16, 31>(state));
        case powerpc_64:
            return SageBuilderAsm::buildValueU64(fld<16, 31>(state));
    }
    ASSERT_not_reachable("invalid word size");
}

SgAsmMemoryReferenceExpression*
Powerpc::memref(State &state, SgAsmType* t) const {
    return SageBuilderAsm::buildMemoryReferenceExpression(SageBuilderAsm::buildAddExpression(RA_or_zero(state), D(state)),
                                                          nullptr, t);
}

SgAsmMemoryReferenceExpression*
Powerpc::memrefds(State &state, SgAsmType *t) const {
    return SageBuilderAsm::buildMemoryReferenceExpression(SageBuilderAsm::buildAddExpression(RA_or_zero(state), DS(state)),
                                                          nullptr, t);
}

SgAsmMemoryReferenceExpression*
Powerpc::memrefra(State &state, SgAsmType *t) const {
    return SageBuilderAsm::buildMemoryReferenceExpression(RA_or_zero(state), nullptr, t);
}

SgAsmMemoryReferenceExpression*
Powerpc::memrefx(State &state, SgAsmType* t) const {
    return SageBuilderAsm::buildMemoryReferenceExpression(SageBuilderAsm::buildAddExpression(RA_or_zero(state), RB(state)),
                                                          nullptr, t);
}

SgAsmMemoryReferenceExpression*
Powerpc::memrefu(State &state, SgAsmType* t) const {
    if (fld<11, 15>(state) == 0)
        throw ExceptionPowerpc("bits 11-15 must be nonzero", state);
    return SageBuilderAsm::buildMemoryReferenceExpression(SageBuilderAsm::buildAddExpression(RA(state), D(state)),
                                                          nullptr, t);
}

SgAsmMemoryReferenceExpression*
Powerpc::memrefux(State &state, SgAsmType* t) const {
    if (fld<11, 15>(state) == 0)
        throw ExceptionPowerpc("bits 11-15 must be nonzero", state);
    return SageBuilderAsm::buildMemoryReferenceExpression(SageBuilderAsm::buildAddExpression(RA(state), RB(state)),
                                                          nullptr, t);
}

void
Powerpc::startInstruction(State &state, Address start_va, uint32_t c) const {
    state.ip = start_va;
    state.insn = c;
}

} // namespace
} // namespace
} // namespace

#endif
