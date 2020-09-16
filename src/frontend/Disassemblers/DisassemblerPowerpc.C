#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"
#include "DisassemblerPowerpc.h"

#include "Assembler.h"
#include "AssemblerX86.h"
#include "AsmUnparser_compat.h"
#include "Disassembler.h"
#include "SageBuilderAsm.h"
#include "BinaryUnparserPowerpc.h"
#include "DispatcherPowerpc.h"

namespace Rose {
namespace BinaryAnalysis {

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

bool
DisassemblerPowerpc::canDisassemble(SgAsmGenericHeader *header) const {
    // Check the architecture and word size
    SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    switch (wordSize_) {
        case powerpc_32:
            if (SgAsmExecutableFileFormat::ISA_PowerPC != isa)
                return false;
            break;
        case powerpc_64:
            if (SgAsmExecutableFileFormat::ISA_PowerPC_64bit != isa)
                return false;
            break;
        default:
            return false;
    }

    // Check the byte order
    SgAsmGenericFormat *fmt = header->get_exec_format();
    ASSERT_not_null(fmt);
    if (fmt->get_sex() != sex_)
        return false;

    return true;
}

Unparser::BasePtr
DisassemblerPowerpc::unparser() const {
    return Unparser::Powerpc::instance();
}

void
DisassemblerPowerpc::init() {
    const RegisterDictionary *regdict = NULL;
    switch (wordSize_) {
        case powerpc_32:
            name("ppc32");
            wordSizeBytes(4);
            regdict = RegisterDictionary::dictionary_powerpc32();
            callingConventions(CallingConvention::dictionaryPowerpc32());
            break;
        case powerpc_64:
            name("ppc64");
            wordSizeBytes(8);
            regdict = RegisterDictionary::dictionary_powerpc64();
            callingConventions(CallingConvention::dictionaryPowerpc64());
            break;
    }

    byteOrder(ByteOrder::ORDER_MSB);
    switch (sex_) {
        case ByteOrder::ORDER_MSB:
            name(name() + "-be");
            break;
        case ByteOrder::ORDER_LSB:
            name(name() + "-le");
            break;
        default:
            ASSERT_not_reachable("invalid byte order");
    }

    REG_IP = regdict->findOrThrow("iar");
    REG_SP = regdict->findOrThrow("r1");
    REG_LINK = regdict->findOrThrow("lr");
    InstructionSemantics2::DispatcherPowerpcPtr d = InstructionSemantics2::DispatcherPowerpc::instance(8*wordSizeBytes(), regdict);
    d->set_register_dictionary(regdict);
    p_proto_dispatcher = d;
    registerDictionary(regdict);
}

// This is a bit of a kludge for now because we're trying to use an unmodified version of the PowerpcDisassembler name space.
SgAsmInstruction*
DisassemblerPowerpc::disassembleOne(const MemoryMap::Ptr &map, rose_addr_t start_va, AddressSet *successors) {
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
        c = ByteOrder::swap_bytes(c);

    // Disassemble the instruction
    State state;
    startInstruction(state, start_va, c);
    SgAsmPowerpcInstruction *insn = disassemble(state);      // throws an exception on error
    ASSERT_not_null(insn);

    // Note successors if necessary
    if (successors) {
        bool complete;
        *successors |= insn->getSuccessors(complete/*out*/);
    }

    return insn;
}

SgAsmInstruction*
DisassemblerPowerpc::makeUnknownInstruction(const Exception &e) {
    SgAsmPowerpcInstruction *insn = new SgAsmPowerpcInstruction(e.ip, "unknown", powerpc_unknown_instruction);
    SgAsmOperandList *operands = new SgAsmOperandList();
    insn->set_operandList(operands);
    operands->set_parent(insn);
    insn->set_raw_bytes(e.bytes);
    return insn;
}

template <size_t First, size_t Last>
uint64_t
DisassemblerPowerpc::fld(State &state) const {
    return (state.insn >> (31 - Last)) & (IntegerOps::GenMask<uint32_t, Last - First + 1>::value);
}

// FIXME[Robb Matzke 2019-08-20]: Replace with proper C++ since nested macros make debugging hard
#define MAKE_INSN0(Mne) (makeInstructionWithoutOperands(state.ip, #Mne, powerpc_##Mne, state.insn))
#define MAKE_INSN0_RC(Mne) (Rc(state) ? MAKE_INSN0(Mne##_record) : MAKE_INSN0(Mne))
#define MAKE_INSN0_O_RC(Mne) (OE() ? MAKE_INSN0_RC(Mne##o) : MAKE_INSN0_RC(Mne))
#define MAKE_INSN1(Mne, Op1) (SageBuilderAsm::appendOperand(MAKE_INSN0(Mne), (Op1)))
#define MAKE_INSN1_RC(Mne, Op1) (SageBuilderAsm::appendOperand(MAKE_INSN0_RC(Mne), (Op1)))
#define MAKE_INSN1_O_RC(Mne, Op1) (SageBuilderAsm::appendOperand(MAKE_INSN0_O_RC(Mne), (Op1)))
#define MAKE_INSN2(Mne, Op1, Op2) (SageBuilderAsm::appendOperand(MAKE_INSN1(Mne, Op1), (Op2)))
#define MAKE_INSN2_RC(Mne, Op1, Op2) (SageBuilderAsm::appendOperand(MAKE_INSN1_RC(Mne, Op1), (Op2)))
#define MAKE_INSN2_O_RC(Mne, Op1, Op2) (SageBuilderAsm::appendOperand(MAKE_INSN1_O_RC(Mne, Op1), (Op2)))
#define MAKE_INSN3(Mne, Op1, Op2, Op3) (SageBuilderAsm::appendOperand(MAKE_INSN2(Mne, Op1, Op2), (Op3)))
#define MAKE_INSN3_RC(Mne, Op1, Op2, Op3) (SageBuilderAsm::appendOperand(MAKE_INSN2_RC(Mne, Op1, Op2), (Op3)))
#define MAKE_INSN3_O_RC(Mne, Op1, Op2, Op3) (SageBuilderAsm::appendOperand(MAKE_INSN2_O_RC(Mne, Op1, Op2), (Op3)))
#define MAKE_INSN4(Mne, Op1, Op2, Op3, Op4) (SageBuilderAsm::appendOperand(MAKE_INSN3(Mne, Op1, Op2, Op3), (Op4)))
#define MAKE_INSN4_RC(Mne, Op1, Op2, Op3, Op4) (SageBuilderAsm::appendOperand(MAKE_INSN3_RC(Mne, Op1, Op2, Op3), (Op4)))
#define MAKE_INSN5(Mne, Op1, Op2, Op3, Op4, Op5) (SageBuilderAsm::appendOperand(MAKE_INSN4(Mne, Op1, Op2, Op3, Op4), (Op5)))
#define MAKE_INSN5_RC(Mne, Op1, Op2, Op3, Op4, Op5) (SageBuilderAsm::appendOperand(MAKE_INSN4_RC(Mne, Op1, Op2, Op3, Op4), (Op5)))

SgAsmIntegerValueExpression*
DisassemblerPowerpc::makeBranchTarget(uint64_t targetAddr) const {
    switch (wordSize_) {
        case powerpc_32:
            return SageBuilderAsm::buildValueU32(targetAddr);
        case powerpc_64:
            return SageBuilderAsm::buildValueU64(targetAddr);
    }
    ASSERT_not_reachable("invalid word size");
}

bool
DisassemblerPowerpc::is64bitInsn(PowerpcInstructionKind kind) {
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
DisassemblerPowerpc::makeInstructionWithoutOperands(uint64_t address, const std::string& mnemonic, PowerpcInstructionKind kind,
                                                    uint32_t insn) {

    if (powerpc_32 == wordSize_ && is64bitInsn(kind)) {
        ASSERT_forbid(powerpc_unknown_instruction == kind);
        return makeInstructionWithoutOperands(address, "unknown", powerpc_unknown_instruction, insn);
    }

    // Constructor: SgAsmPowerpcInstruction(rose_addr_t address = 0, std::string mnemonic = "", PowerpcInstructionKind kind =
    // powerpc_unknown_instruction);
    SgAsmPowerpcInstruction* instruction = new SgAsmPowerpcInstruction(address, mnemonic, kind);
    ASSERT_not_null(instruction);

    if (mnemonic.size() >= 7 && mnemonic.substr(mnemonic.size() - 7) == "_record")
        instruction->set_mnemonic(mnemonic.substr(0, mnemonic.size() - 7) + ".");

    SgAsmOperandList* operands = new SgAsmOperandList();
    instruction->set_operandList(operands);
    operands->set_parent(instruction);

    // PowerPC uses a fixed length instruction set (like ARM, but unlike x86)
    SgUnsignedCharList bytes(4, '\0');
    for (int i = 0; i < 4; ++i)
        bytes[i] = (insn >> (24 - (8 * i))) & 0xFF; // Force big-endian

    instruction->set_raw_bytes(bytes);
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
DisassemblerPowerpc::makeRegister(State &state, PowerpcRegisterClass reg_class, int reg_number,
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
        case powerpc_regclass_unknown:
        case powerpc_last_register_class:
            throw ExceptionPowerpc("not a real register", state);
        // Don't add a "default" or else we won't get compiler warnings if a new class is defined.
    }
    ASSERT_forbid(name.empty());
    ASSERT_not_null(registerType);

    // Obtain a register descriptor from the dictionary
    ASSERT_not_null(registerDictionary());
    const RegisterDescriptor rdesc = registerDictionary()->find(name);
    if (!rdesc)
        throw ExceptionPowerpc("register \"" + name + "\" is not available for " + registerDictionary()->get_architecture_name(), state);
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
DisassemblerPowerpc::disassemble(State &state) {
    // The Primary Opcode Field is bits 0-5, 6-bits wide, so there are max 64 primary opcode values
    uint8_t primaryOpcode = (state.insn >> 26) & 0x3F;

    switch (primaryOpcode) {
        case 0x00: {
            // These are the BGL specific PowerPC440 FP2 Architecture instructions.  Depending on if this is A form or X form,
            // the extended opCode maps to different bit ranges, so this code is incorrect!
            uint8_t a_Opcode = (state.insn >> 1) & 0x1F;

            // Different parts of the instruction are used to identify what kind of instruction this is!
            if (a_Opcode == 5 || (a_Opcode >= 8 && a_Opcode <= 31)) {
                return decode_A_formInstruction_00(state);
            } else {
                return decode_X_formInstruction_00(state);
            }
            break;
        }

        case 0x01: throw ExceptionPowerpc("invalid primary opcode (0x01)", state, 26);
        case 0x02: return MAKE_INSN3(tdi, TO(state), RA(state), SI(state));
        case 0x03: return MAKE_INSN3(twi, TO(state), RA(state), SI(state));
        case 0x04: return decode_A_formInstruction_04(state);
        case 0x05: throw ExceptionPowerpc("invalid primary opcode (0x05)", state, 26);
        case 0x06: throw ExceptionPowerpc("invalid primary opcode (0x06)", state, 26);
        case 0x07: return MAKE_INSN3(mulli, RT(state), RA(state), SI(state));
        case 0x08: return MAKE_INSN3(subfic, RT(state), RA(state), SI(state));
        case 0x09: throw ExceptionPowerpc("invalid primary opcode (0x09)", state, 26);
        case 0x0A: return MAKE_INSN4(cmpli, BF_cr(state), L_10(state), RA(state), UI(state));
        case 0x0B: return MAKE_INSN4(cmpi, BF_cr(state), L_10(state), RA(state), SI(state));
        case 0x0C: return MAKE_INSN3(addic, RT(state), RA(state), SI(state));
        case 0x0D: return MAKE_INSN3(addic_record, RT(state), RA(state), SI(state));
        case 0x0E: return MAKE_INSN3(addi, RT(state), RA_or_zero(state), SI(state));
        case 0x0F: return MAKE_INSN3(addis, RT(state), RA_or_zero(state), SI(state));
        case 0x10: return decode_B_formInstruction(state);
        case 0x11: return decode_SC_formInstruction(state);
        case 0x12: return decode_I_formInstruction(state);
        case 0x13: return decode_XL_formInstruction(state);
        case 0x14: return MAKE_INSN5_RC(rlwimi, RA(state), RS(state), SH_32bit(state), MB_32bit(state), ME_32bit(state));
        case 0x15: return MAKE_INSN5_RC(rlwinm, RA(state), RS(state), SH_32bit(state), MB_32bit(state), ME_32bit(state));
        case 0x17: return MAKE_INSN5_RC(rlwnm, RA(state), RS(state), RB(state), MB_32bit(state), ME_32bit(state));
        case 0x18: return MAKE_INSN3(ori, RA(state), RS(state), UI(state));
        case 0x19: return MAKE_INSN3(oris, RA(state), RS(state), UI(state));
        case 0x1A: return MAKE_INSN3(xori, RA(state), RS(state), UI(state));
        case 0x1B: return MAKE_INSN3(xoris, RA(state), RS(state), UI(state));
        case 0x1C: return MAKE_INSN3(andi_record, RA(state), RS(state), UI(state));
        case 0x1D: return MAKE_INSN3(andis_record, RA(state), RS(state), UI(state));
        case 0x1E: return decode_MD_formInstruction(state);
        case 0x1F: return decode_X_formInstruction_1F(state);
        case 0x20: return MAKE_INSN2(lwz, RT(state), memref(state, T_U32));
        case 0x21:
            if (fld<11, 15>(state) == 0 || fld<11, 15>(state) == fld<6, 10>(state))
                throw ExceptionPowerpc("invalid LWZU instruction", state);
            return MAKE_INSN2(lwzu, RT(state), memrefu(state, T_U32));
        case 0x22: return MAKE_INSN2(lbz, RT(state), memref(state, T_U8));
        case 0x23:
            if (fld<11, 15>(state) == 0 || fld<11, 15>(state) == fld<6, 10>(state))
                throw ExceptionPowerpc("invalid LBZU instruction", state);
            return MAKE_INSN2(lbzu, RT(state), memrefu(state, T_U8));
        case 0x24: return MAKE_INSN2(stw, RS(state), memref(state, T_U32));
        case 0x25:
            if (fld<11, 15>(state) == 0)
                throw ExceptionPowerpc("invalid STWU instruction", state);
            return MAKE_INSN2(stwu, RS(state), memrefu(state, T_U32));
        case 0x26: return MAKE_INSN2(stb, RS(state), memref(state, T_U8));
        case 0x27:
            if (fld<11, 15>(state) == 0)
                throw ExceptionPowerpc("invalid STBU instruction", state);
            return MAKE_INSN2(stbu, RS(state), memrefu(state, T_U8));
        case 0x28: return MAKE_INSN2(lhz, RT(state), memref(state, T_U16));
        case 0x29:
            if (fld<11, 15>(state) == 0 || fld<11, 15>(state) == fld<6, 10>(state))
                throw ExceptionPowerpc("invalid LHZU instruction", state);
            return MAKE_INSN2(lhzu, RT(state), memrefu(state, T_U16));
        case 0x2A: return MAKE_INSN2(lha, RT(state), memref(state, T_U16));
        case 0x2B:
            if (fld<11, 15>(state) == 0 || fld<11, 15>(state) == fld<6, 10>(state))
                throw ExceptionPowerpc("invalid LHAU instruction", state);
            return MAKE_INSN2(lhau, RT(state), memrefu(state, T_U16));
        case 0x2C: return MAKE_INSN2(sth, RS(state), memref(state, T_U16));
        case 0x2D:
            if (fld<11, 15>(state) == 0)
                throw ExceptionPowerpc("invalid STHU instruction", state);
            return MAKE_INSN2(sthu, RS(state), memrefu(state, T_U16));
        case 0x2E: return MAKE_INSN2(lmw, RT(state), memref(state, T_U32));
        case 0x2F: return MAKE_INSN2(stmw, RS(state), memref(state, T_U32));
        case 0x30: return MAKE_INSN2(lfs, FRT(state), memref(state, T_FLOAT32));
        case 0x31: return MAKE_INSN2(lfsu, FRT(state), memrefu(state, T_FLOAT32));
        case 0x32: return MAKE_INSN2(lfd, FRT(state), memref(state, T_FLOAT64));
        case 0x33: return MAKE_INSN2(lfdu, FRT(state), memrefu(state, T_FLOAT64));
        case 0x34: return MAKE_INSN2(stfs, FRS(state), memref(state, T_FLOAT32));
        case 0x35: return MAKE_INSN2(stfsu, FRS(state), memrefu(state, T_FLOAT32));
        case 0x36: return MAKE_INSN2(stfd, FRS(state), memref(state, T_FLOAT64));
        case 0x37: return MAKE_INSN2(stfdu, FRS(state), memrefu(state, T_FLOAT64));
        case 0x38: throw ExceptionPowerpc("invalid primary opcode (0x38)", state, 26);
        case 0x39: throw ExceptionPowerpc("invalid primary opcode (0x39)", state, 26);
        case 0x3A: {
            switch (unsigned xo = state.insn & 0x3) {
                case 0: return MAKE_INSN2(ld, RT(state), memrefds(state, T_U64));
                case 1:
                    if (fld<11, 15>(state) == 0 || fld<11, 15>(state) == fld<6, 10>(state))
                        throw ExceptionPowerpc("invalid LDU instruction", state);
                    return MAKE_INSN2(ldu, RT(state), memrefds(state, T_U64));
                case 2: return MAKE_INSN2(lwa, RT(state), memrefds(state, T_U32));
                default:
                    throw ExceptionPowerpc("decoding error for pimary opcode 0x3a, XO=" + StringUtility::numberToString(xo),
                                           state);
            }
            break;
        }
        case 0x3B: return decode_A_formInstruction_3B(state);
        case 0x3C: throw ExceptionPowerpc("invalid primary opcode (0x3c)", state, 26);
        case 0x3D: throw ExceptionPowerpc("invalid primary opcode (0x3d)", state, 26);
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
            throw ExceptionPowerpc("illegal primary opcode: "+StringUtility::addrToString(primaryOpcode), state, 26);
    }

    ASSERT_not_reachable("op code not handled");
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_I_formInstruction(State &state) {
    uint8_t primaryOpcode = (state.insn >> 26) & 0x3F;
    switch(primaryOpcode) {
        case 0x12: {
            rose_addr_t targetBranchAddress = LI(state);
            if (AA(state) == 0)
                targetBranchAddress += state.ip;

            SgAsmIntegerValueExpression* targetAddressExpression = makeBranchTarget(targetBranchAddress);

            if (AA(state) == 0) {
                if (LK(state) == 0) {
                    return MAKE_INSN1(b, targetAddressExpression);
                } else {
                    return MAKE_INSN1(bl, targetAddressExpression);
                }
            } else {
                if (LK(state) == 0) {
                    return MAKE_INSN1(ba, targetAddressExpression);
                } else {
                    return MAKE_INSN1(bla, targetAddressExpression);
                }
            }
            break;
        }

        default:
            throw ExceptionPowerpc("invaild I-Form primary opcode: " + StringUtility::addrToString(primaryOpcode), state);
    }
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_B_formInstruction(State &state) {
     uint8_t primaryOpcode = (state.insn >> 26) & 0x3F;
     switch(primaryOpcode) {
         case 0x10: {
             uint64_t targetBranchAddress = BD(state);
             if (AA(state) == 0)
                 targetBranchAddress += state.ip;
             SgAsmIntegerValueExpression* targetAddressExpression = makeBranchTarget(targetBranchAddress);

             if (LK(state) == 0) {
                 if (AA(state) == 0) {
                     return MAKE_INSN3(bc, BO(state), BI(state), targetAddressExpression);
                 } else {
                     return MAKE_INSN3(bca, BO(state), BI(state), targetAddressExpression);
                 }
             } else {
                 if (AA(state) == 0) {
                     return MAKE_INSN3(bcl, BO(state), BI(state), targetAddressExpression);
                 } else {
                     return MAKE_INSN3(bcla, BO(state), BI(state), targetAddressExpression);
                 }
             }
             break;
         }

         default:
             throw ExceptionPowerpc("invalid B-Form primary opcode: " + StringUtility::addrToString(primaryOpcode), state);
     }
     ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_SC_formInstruction(State &state) {
    uint8_t primaryOpcode = (state.insn >> 26) & 0x3F;
    ASSERT_always_require(primaryOpcode == 0x11);

    // Get bit 30, 1 bit as the reserved flag
    uint8_t constantOneOpcode = (state.insn >> 1) & 0x1;
    if (constantOneOpcode!=1)
        throw ExceptionPowerpc("expected bit to be set", state, 1);

    return MAKE_INSN1(sc, LEV(state));
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_DS_formInstruction(State &state) {
    uint8_t primaryOpcode = (state.insn >> 26) & 0x3f;
    ASSERT_always_require(primaryOpcode == 0x3e);

    switch (state.insn & 0x03) {
        case 0: return MAKE_INSN2(std, RS(state), memrefds(state, T_U64));
        case 1:
            if (fld<11, 15>(state) == 0)
                throw ExceptionPowerpc("invalid STDU instruction", state);
            return MAKE_INSN2(stdu, RS(state), memrefds(state, T_U64));
        default: throw ExceptionPowerpc("invalid DS-form instruction", state);
    }
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_X_formInstruction_1F(State &state) {
    uint8_t primaryOpcode = (state.insn >> 26) & 0x3F;
    ASSERT_always_require(primaryOpcode == 0x1F);

    // Get the bits 21-30, next 10 bits
    uint16_t xoOpcode = (state.insn >> 1) & 0x3FF;
    switch(xoOpcode) {
        case 0x000: return MAKE_INSN4(cmp, BF_cr(state), L_10(state), RA(state), RB(state));
        case 0x004: return MAKE_INSN3(tw, TO(state), RA(state), RB(state));
        case 0x008: return MAKE_INSN3_RC(subfc, RT(state), RA(state), RB(state));
        case 0x009: return MAKE_INSN3_RC(mulhdu, RT(state), RA(state), RB(state));
        case 0x00A: return MAKE_INSN3_RC(addc, RT(state), RA(state), RB(state));
        case 0x00B: return MAKE_INSN3_RC(mulhwu, RT(state), RA(state), RB(state));
        case 0x014: return MAKE_INSN2(lwarx, RT(state), memrefx(state, T_U32));
        case 0x015: return MAKE_INSN2(ldx, RT(state), memrefx(state, T_U64));
        case 0x017: return MAKE_INSN2(lwzx, RT(state), memrefx(state, T_U32));
        case 0x018: return MAKE_INSN3_RC(slw, RA(state), RS(state), RB(state));
        case 0x01A: return MAKE_INSN2_RC(cntlzw, RA(state), RS(state));
        case 0x01B: return MAKE_INSN3_RC(sld, RA(state), RS(state), RB(state));
        case 0x01C: return MAKE_INSN3_RC(and, RA(state), RS(state), RB(state));
        case 0x020: return MAKE_INSN4(cmpl, BF_cr(state), L_10(state), RA(state), RB(state));
        case 0x028: return MAKE_INSN3_RC(subf, RT(state), RA(state), RB(state));
        case 0x035:
            if (fld<11, 15>(state) == 0 || fld<11, 15>(state) == fld<6, 10>(state))
                throw ExceptionPowerpc("invalid LDUX instruction", state);
            return MAKE_INSN2(ldux, RT(state), memrefux(state, T_U64));
        case 0x037:
            if (fld<11, 15>(state) == 0 || fld<11, 15>(state) == fld<6, 10>(state))
                throw ExceptionPowerpc("invalid LWZUX instruction", state);
            return MAKE_INSN2(lwzux, RT(state), memrefux(state, T_U32));
        case 0x03a: return MAKE_INSN2_RC(cntlzd, RA(state), RS(state));
        case 0x03C: return MAKE_INSN3_RC(andc, RA(state), RS(state), RB(state));
        case 0x044: return MAKE_INSN3(td, TO(state), RA(state), RB(state));
        case 0x049: return MAKE_INSN3_RC(mulhd, RT(state), RA(state), RB(state));
        case 0x04B: return MAKE_INSN3_RC(mulhw, RT(state), RA(state), RB(state));
        case 0x053: return MAKE_INSN1(mfmsr, RT(state));
        case 0x057: return MAKE_INSN2(lbzx, RT(state), memrefx(state, T_U8));
        case 0x068: return MAKE_INSN2_RC(neg, RT(state), RA(state));
        case 0x077:
            if (fld<11, 15>(state) == 0 || fld<11, 15>(state) == fld<6, 10>(state))
                throw ExceptionPowerpc("invalid LBZUX instruction", state);
            return MAKE_INSN2(lbzux, RT(state), memrefux(state, T_U8));
        case 0x07A: return MAKE_INSN2(popcntb, RA(state), RS(state));
        case 0x07C: return MAKE_INSN3_RC(nor, RA(state), RS(state), RB(state));
        case 0x088: return MAKE_INSN3_RC(subfe, RT(state), RA(state), RB(state));
        case 0x08A: return MAKE_INSN3_RC(adde, RT(state), RA(state), RB(state));
        case 0x08E: return MAKE_INSN2(lfssx, FRT(state), memrefx(state, T_FLOAT32));
        case 0x095: return MAKE_INSN2(stdx, RS(state), memrefx(state, T_U64));
        case 0x096: return MAKE_INSN2(stwcx_record, RS(state), memrefx(state, T_U32));
        case 0x097: return MAKE_INSN2(stwx, RS(state), memrefx(state, T_U32));
        case 0x0AE: return MAKE_INSN2(lfssux, FRT(state), memrefux(state, T_FLOAT32));
        case 0x0B5:
            if (fld<11, 15>(state) == 0)
                throw ExceptionPowerpc("invalid STDUX instruction", state);
            return MAKE_INSN2(stdux, RS(state), memrefux(state, T_U64));
        case 0x0B7:
            if (fld<11, 15>(state) == 0)
                throw ExceptionPowerpc("invalid STWUX instruction", state);
            return MAKE_INSN2(stwux, RS(state), memrefux(state, T_U32));
        case 0x0C8: return MAKE_INSN2_RC(subfze, RT(state), RA(state));
        case 0x0CA: return MAKE_INSN2_RC(addze, RT(state), RA(state));
        case 0x0CE: return MAKE_INSN2(lfsdx, FRT(state), memrefx(state, T_FLOAT64));
        case 0x0D6: return MAKE_INSN2(stdcx_record, RS(state), memrefx(state, T_U64));
        case 0x0D7: return MAKE_INSN2(stbx, RS(state), memrefx(state, T_U8));
        case 0x0E8: return MAKE_INSN2_RC(subfme, RT(state), RA(state));
        case 0x0EA: return MAKE_INSN2_RC(addme, RT(state), RA(state));
        case 0x0EB: return MAKE_INSN3_RC(mullw, RT(state), RA(state), RB(state));
        case 0x0EE: return MAKE_INSN2(lfsdux, FRT(state), memrefux(state, T_FLOAT64));
        case 0x0f4: return MAKE_INSN3_RC(mulld, RT(state), RA(state), RB(state));
        case 0x0F7:
            if (fld<11, 15>(state) == 0)
                throw ExceptionPowerpc("invalid STBUX instruction", state);
            return MAKE_INSN2(stbux, RS(state), memrefux(state, T_U8));
        case 0x10A: return MAKE_INSN3_RC(add, RT(state), RA(state), RB(state));
        case 0x10E: return MAKE_INSN2(lfxsx, FRT(state), memrefx(state, T_V2_FLOAT32));
        case 0x116: return MAKE_INSN1(dcbt, memrefx(state, T_U8));
        case 0x117: return MAKE_INSN2(lhzx, RT(state), memrefx(state, T_U16));
        case 0x11C: return MAKE_INSN3_RC(eqv, RA(state), RS(state), RB(state));
        case 0x12E: return MAKE_INSN2(lfxsux, FRT(state), memrefux(state, T_V2_FLOAT32));
        case 0x137:
            if (fld<11, 15>(state) == 0 || fld<11, 15>(state) == fld<6, 10>(state))
                throw ExceptionPowerpc("invalid LHZUX instruction", state);
            return MAKE_INSN2(lhzux, RT(state), memrefux(state, T_U16));
        case 0x13C: return MAKE_INSN3_RC(xor, RA(state), RS(state), RB(state));
        case 0x14E: return MAKE_INSN2(lfxdx, FRT(state), memrefx(state, T_V2_FLOAT64));
        case 0x153: return MAKE_INSN2(mfspr, RT(state), SPR(state));
        case 0x155: return MAKE_INSN2(lwax, RT(state), memrefx(state, T_U32));
        case 0x157: return MAKE_INSN2(lhax, RT(state), memrefx(state, T_U16));
        case 0x16E: return MAKE_INSN2(lfxdux, FRT(state), memrefux(state, T_V2_FLOAT64));
        case 0x175:
            if (fld<11, 15>(state) == 0 || fld<11, 15>(state) == fld<6, 10>(state))
                throw ExceptionPowerpc("invalid LWAUX instruction", state);
            return MAKE_INSN2(lwaux, RT(state), memrefux(state, T_U32));
        case 0x177:
            if (fld<11, 15>(state) == 0 || fld<11, 15>(state) == fld<6, 10>(state))
                throw ExceptionPowerpc("invalid LHAUX instruction", state);
            return MAKE_INSN2(lhaux, RT(state), memrefux(state, T_U16));
        case 0x18E: return MAKE_INSN2(lfpsx, FRT(state), memrefx(state, T_V2_FLOAT32));
        case 0x197: return MAKE_INSN2(sthx, RS(state), memrefx(state, T_U16));
        case 0x19C: return MAKE_INSN3_RC(orc, RA(state), RS(state), RB(state));
        case 0x1AE: return MAKE_INSN2(lfpsux, FRT(state), memrefux(state, T_V2_FLOAT32));
        case 0x1B7:
            if (fld<11, 15>(state) == 0)
                throw ExceptionPowerpc("invalid STHUX instruction", state);
            return MAKE_INSN2(sthux, RS(state), memrefux(state, T_U16));
        case 0x1BC: return MAKE_INSN3_RC(or, RA(state), RS(state), RB(state));
        case 0x1C9: return MAKE_INSN3_RC(divdu, RT(state), RA(state), RB(state));
        case 0x1CB: return MAKE_INSN3_RC(divwu, RT(state), RA(state), RB(state));
        case 0x1CE: return MAKE_INSN2(lfpdx, FRT(state), memrefx(state, T_V2_FLOAT64));
        case 0x1D3: return MAKE_INSN2(mtspr, SPR(state), RS(state));
        case 0x1DC: return MAKE_INSN3_RC(nand, RA(state), RS(state), RB(state));
        case 0x1E9: return MAKE_INSN3_RC(divd, RT(state), RA(state), RB(state));
        case 0x1EB: return MAKE_INSN3_RC(divw, RT(state), RA(state), RB(state));
        case 0x1EE: return MAKE_INSN2(lfpdux, FRT(state), memrefux(state, T_V2_FLOAT64));
        case 0x208: return MAKE_INSN3_RC(subfco, RT(state), RA(state), RB(state));
        case 0x20A: return MAKE_INSN3_RC(addco, RT(state), RA(state), RB(state));
        case 0x20E: return MAKE_INSN2(stfpiwx, FRT(state), memrefx(state, T_V2_U32));
        case 0x215: return MAKE_INSN2(lswx, RT(state), memrefx(state, T_U8));
        case 0x216: return MAKE_INSN2(lwbrx, RT(state), memrefx(state, T_U32));
        case 0x217: return MAKE_INSN2(lfsx, FRT(state), memrefx(state, T_FLOAT32));
        case 0x218: return MAKE_INSN3_RC(srw, RA(state), RS(state), RB(state));
        case 0x21B: return MAKE_INSN3_RC(srd, RA(state), RS(state), RB(state));
        case 0x228: return MAKE_INSN3_RC(subfo, RT(state), RA(state), RB(state));
        case 0x237: return MAKE_INSN2(lfsux, FRT(state), memrefux(state, T_FLOAT32));
        case 0x255: return MAKE_INSN3(lswi, RT(state), memrefra(state, T_U8), NB(state));
        case 0x257: return MAKE_INSN2(lfdx, FRT(state), memrefx(state, T_FLOAT64));
        case 0x268: return MAKE_INSN2_RC(nego, RT(state), RA(state));
        case 0x288: return MAKE_INSN3_RC(subfeo, RT(state), RA(state), RB(state));
        case 0x28A: return MAKE_INSN3_RC(addeo, RT(state), RA(state), RB(state));
        case 0x28E: return MAKE_INSN2(stfssx, FRT(state), memrefx(state, T_FLOAT32));
        case 0x295: return MAKE_INSN2(stswx, RS(state), memrefx(state, T_U8));
        case 0x296: return MAKE_INSN2(stwbrx, RS(state), memrefx(state, T_U32));
        case 0x2AE: return MAKE_INSN2(stfssux, FRT(state), memrefux(state, T_FLOAT32));
        case 0x2C8: return MAKE_INSN2_RC(subfzeo, RT(state), RA(state));
        case 0x2CA: return MAKE_INSN2_RC(addzeo, RT(state), RA(state));
        case 0x2CE: return MAKE_INSN2(stfsdx, FRT(state), memrefx(state, T_FLOAT64));
        case 0x2D5: return MAKE_INSN3(stswi, RS(state), memrefra(state, T_U8), NB(state));
        case 0x2D7: return MAKE_INSN2(stfdx, FRS(state), memrefx(state, T_FLOAT64));
        case 0x2E8: return MAKE_INSN2_RC(subfmeo, RT(state), RA(state));
        case 0x2EA: return MAKE_INSN2_RC(addmeo, RT(state), RA(state));
        case 0x2EB: return MAKE_INSN3_RC(mullwo, RT(state), RA(state), RB(state));
        case 0x2EE: return MAKE_INSN2(stfsdux, FRT(state), memrefux(state, T_FLOAT64));
        case 0x2F4: return MAKE_INSN3_RC(mulldo, RT(state), RA(state), RB(state));
        case 0x2F7: return MAKE_INSN2(stfdux, FRS(state), memrefx(state, T_FLOAT64));
        case 0x30A: return MAKE_INSN3_RC(addo, RT(state), RA(state), RB(state));
        case 0x30E: return MAKE_INSN2(stfxsx, FRT(state), memrefx(state, T_V2_FLOAT32));
        case 0x316: return MAKE_INSN2(lhbrx, RT(state), memrefx(state, T_U16));
        case 0x318: return MAKE_INSN3_RC(sraw, RA(state), RS(state), RB(state));
        case 0x31A: return MAKE_INSN3_RC(srad, RA(state), RS(state), RB(state));
        case 0x32E: return MAKE_INSN2(stfxsux, FRT(state), memrefux(state, T_V2_FLOAT32));
        case 0x338: return MAKE_INSN3_RC(srawi, RA(state), RS(state), SH_32bit(state));
        case 0x33A: return MAKE_INSN3_RC(sradi, RA(state), RS(state), SH_64bit(state)); // The last bit of the ext. opcode is part of SH
        case 0x33B: return MAKE_INSN3_RC(sradi, RA(state), RS(state), SH_64bit(state)); // Same as previous insn
        case 0x34E: return MAKE_INSN2(stfxdx, FRT(state), memrefx(state, T_V2_FLOAT64));
        case 0x356: return MAKE_INSN0(eieio);
        case 0x36E: return MAKE_INSN2(stfxdux, FRT(state), memrefux(state, T_V2_FLOAT64));
        case 0x38E: return MAKE_INSN2(stfpsx, FRT(state), memrefx(state, T_V2_FLOAT32));
        case 0x396: return MAKE_INSN2(sthbrx, RS(state), memrefx(state, T_U16));
        case 0x39A: return MAKE_INSN2_RC(extsh, RA(state), RS(state));
        case 0x3AE: return MAKE_INSN2(stfpsux, FRT(state), memrefux(state, T_V2_FLOAT32));
        case 0x3BA: return MAKE_INSN2_RC(extsb, RA(state), RS(state));
        case 0x3C9: return MAKE_INSN3_RC(divduo, RT(state), RA(state), RB(state));
        case 0x3CB: return MAKE_INSN3_RC(divwuo, RT(state), RA(state), RB(state));
        case 0x3CE: return MAKE_INSN2(stfpdx, FRT(state), memrefx(state, T_V2_FLOAT64));
        case 0x3D7: return MAKE_INSN2(stfiwx, FRS(state), memrefx(state, T_U32));
        case 0x3DA: return MAKE_INSN2_RC(extsw, RA(state), RS(state));
        case 0x3E9: return MAKE_INSN3_RC(divdo, RT(state), RA(state), RB(state));
        case 0x3EB: return MAKE_INSN3_RC(divwo, RT(state), RA(state), RB(state));
        case 0x3EE: return MAKE_INSN2(stfpdux, FRT(state), memrefux(state, T_V2_FLOAT64));
        case 0x3F6: return MAKE_INSN1(dcbz, memrefx(state, T_U8));
        case 0x13:
            if (fld<11, 11>(state) == 0) {
                return MAKE_INSN1(mfcr, RT(state));
            } else {
                return MAKE_INSN1(mfcr, RT(state));
            }
            break;
        case 0x90:
            if (fld<11, 11>(state) == 0) {
                return MAKE_INSN2(mtcrf, FXM(state), RS(state));
            } else {
                return MAKE_INSN2(mtcrf, FXM(state), RS(state));
            }
            break;
        default:
            throw ExceptionPowerpc("X-Form 1F xoOpcode not handled: " + StringUtility::addrToString(xoOpcode), state, 1);
    }
    ASSERT_not_reachable("unhandled opcode");
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_X_formInstruction_3F(State &state) {
    uint8_t primaryOpcode = (state.insn >> 26) & 0x3F;
    ASSERT_always_require(primaryOpcode == 0x3F);

    // Get the bits 21-30, next 10 bits
    uint16_t xoOpcode = (state.insn >> 1) & 0x3FF;
    switch(xoOpcode) {
        case 0x000: return MAKE_INSN3(fcmpu, BF_cr(state), FRA(state), FRB(state));
        case 0x00C: return MAKE_INSN2_RC(frsp, FRT(state), FRB(state));
        case 0x00E: return MAKE_INSN2_RC(fctiw, FRT(state), FRB(state));
        case 0x00F: return MAKE_INSN2_RC(fctiwz, FRT(state), FRB(state));
        case 0x026: return MAKE_INSN1(mtfsb1, BT(state));
        case 0x028: return MAKE_INSN2_RC(fneg, FRT(state), FRB(state));
        case 0x046: return MAKE_INSN1_RC(mtfsb0, BT(state));
        case 0x048: return MAKE_INSN2_RC(fmr, FRT(state), FRB(state));
        case 0x086: return MAKE_INSN2_RC(mtfsfi, BF_fpscr(state), U(state));
        case 0x088: return MAKE_INSN2_RC(fnabs, FRT(state), FRB(state));
        case 0x108: return MAKE_INSN2_RC(fabs, FRT(state), FRB(state));
        case 0x247: return MAKE_INSN1_RC(mffs, FRT(state));
        case 0x2C7: return MAKE_INSN2_RC(mtfsf, FLM(state), FRB(state));
        default:
            throw ExceptionPowerpc("X-Form 3F xoOpcode not handled: " + StringUtility::addrToString(xoOpcode), state, 1);
    }
    ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_X_formInstruction_00(State &state) {
    if (state.insn == 0)
        throw ExceptionPowerpc("zero instruction", state, 0);

    // Get the bits 21-30, next 10 bits
    uint16_t xoOpcode = (state.insn >> 1) & 0x3FF;
    switch(xoOpcode) {
        case 0x020: return MAKE_INSN2(fpmr, FRT(state), FRB(state));
        case 0x060: return MAKE_INSN2(fpabs, FRT(state), FRB(state));
        case 0x0A0: return MAKE_INSN2(fpneg, FRT(state), FRB(state));
        case 0x0C0: return MAKE_INSN2(fprsp, FRT(state), FRB(state));
        case 0x0E0: return MAKE_INSN2(fpnabs, FRT(state), FRB(state));
        case 0x120: return MAKE_INSN2(fsmr, FRT(state), FRB(state));
        case 0x160: return MAKE_INSN2(fsabs, FRT(state), FRB(state));
        case 0x1A0: return MAKE_INSN2(fsneg, FRT(state), FRB(state));
        case 0x1E0: return MAKE_INSN2(fsnabs, FRT(state), FRB(state));
        case 0x220: return MAKE_INSN2(fxmr, FRT(state), FRB(state));
        case 0x240: return MAKE_INSN2(fpctiw, FRT(state), FRB(state));
        case 0x2C0: return MAKE_INSN2(fpctiwz, FRT(state), FRB(state));
        case 0x320: return MAKE_INSN2(fsmtp, FRT(state), FRB(state));
        case 0x3A0: return MAKE_INSN2(fsmfp, FRT(state), FRB(state));
        default:
            throw ExceptionPowerpc("X-Form 00 xoOpcode not handled: " + StringUtility::addrToString(xoOpcode), state, 1);
    }
    ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_XL_formInstruction(State &state) {
    // The primaryOpcode
    uint8_t primaryOpcode = (state.insn >> 26) & 0x3F;
    ASSERT_always_require(primaryOpcode == 0x13);

    // Get the bits 21-30, next 10 bits
    uint16_t xoOpcode = (state.insn >> 1) & 0x3FF;
    switch(xoOpcode) {
        case 0x000: return MAKE_INSN2(mcrf, BF_cr(state), BFA_cr(state));
        case 0x010:
            if (LK(state) == 0) {
                return MAKE_INSN3(bclr, BO(state), BI(state), BH(state));
            } else {
                return MAKE_INSN3(bclrl, BO(state), BI(state), BH(state));
            }
            break;
        case 0x021: return MAKE_INSN3(crnor, BT(state), BA(state), BB(state));
        case 0x081: return MAKE_INSN3(crandc, BT(state), BA(state), BB(state));
        case 0x0C1: return MAKE_INSN3(crxor, BT(state), BA(state), BB(state));
        case 0x0E1: return MAKE_INSN3(crnand, BT(state), BA(state), BB(state));
        case 0x101: return MAKE_INSN3(crand, BT(state), BA(state), BB(state));
        case 0x121: return MAKE_INSN3(creqv, BT(state), BA(state), BB(state));
        case 0x1A1: return MAKE_INSN3(crorc, BT(state), BA(state), BB(state));
        case 0x1C1: return MAKE_INSN3(cror, BT(state), BA(state), BB(state));
        case 0x210:
            if (LK(state) == 0) {
                return MAKE_INSN3(bcctr, BO(state), BI(state), BH(state));
            } else {
                return MAKE_INSN3(bcctrl, BO(state), BI(state), BH(state));
            }
            break;

        default:
            throw ExceptionPowerpc("invalid XL-Form opcode: " + StringUtility::addrToString(xoOpcode), state);
    }

    ASSERT_not_reachable("xoOpcode not handled");
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_A_formInstruction_00(State &state) {
    switch(fld<26, 30>(state)) {
        case 0x05: return MAKE_INSN4(fpsel, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x08: return MAKE_INSN3(fpmul, FRT(state), FRA(state), FRC(state));
        case 0x09: return MAKE_INSN3(fxmul, FRT(state), FRA(state), FRC(state));
        case 0x0A: return MAKE_INSN3(fxpmul, FRT(state), FRA(state), FRC(state));
        case 0x0B: return MAKE_INSN3(fxsmul, FRT(state), FRA(state), FRC(state));
        case 0x0C: return MAKE_INSN3(fpadd, FRT(state), FRA(state), FRB(state));
        case 0x0D: return MAKE_INSN3(fpsub, FRT(state), FRA(state), FRB(state));
        case 0x0E: return MAKE_INSN2(fpre, FRT(state), FRB(state));
        case 0x0F: return MAKE_INSN2(fprsqrte, FRT(state), FRB(state));
        case 0x10: return MAKE_INSN4(fpmadd, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x11: return MAKE_INSN4(fxmadd, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x12: return MAKE_INSN4(fxcpmadd, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x13: return MAKE_INSN4(fxcsmadd, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x14: return MAKE_INSN4(fpnmadd, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x15: return MAKE_INSN4(fxnmadd, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x16: return MAKE_INSN4(fxcpnmadd, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x17: return MAKE_INSN4(fxcsnmadd, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x18: return MAKE_INSN4(fpmsub, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x19: return MAKE_INSN4(fxmsub, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1A: return MAKE_INSN4(fxcpmsub, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1B: return MAKE_INSN4(fxcsmsub, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1C: return MAKE_INSN4(fpnmsub, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1D: return MAKE_INSN4(fxnmsub, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1E: return MAKE_INSN4(fxcpnmsub, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1F: return MAKE_INSN4(fxcsnmsub, FRT(state), FRA(state), FRB(state), FRC(state));
        default:
            throw ExceptionPowerpc("invalid A-Form opcode: " + StringUtility::addrToString(int(fld<26, 30>(state))), state);
    }
    ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_A_formInstruction_04(State &state) {
    // FIXME: Make the floating point registers use the powerpc_regclass_fpr instead of powerpc_regclass_gpr
    switch(fld<26, 30>(state)) {
        case 0x18: return MAKE_INSN4(fxcpnpma, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x19: return MAKE_INSN4(fxcsnpma, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1A: return MAKE_INSN4(fxcpnsma, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1B: return MAKE_INSN4(fxcsnsma, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1C: return MAKE_INSN4(fxcxma, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1D: return MAKE_INSN4(fxcxnpma, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1E: return MAKE_INSN4(fxcxnsma, FRT(state), FRA(state), FRB(state), FRC(state));
        case 0x1F: return MAKE_INSN4(fxcxnms, FRT(state), FRA(state), FRB(state), FRC(state));
        default:
            throw ExceptionPowerpc("invalid A-Form opcode: " + StringUtility::addrToString(int(fld<26, 30>(state))), state);
    }
    ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_A_formInstruction_3B(State &state) {
    switch(fld<26, 30>(state)) {
        case 0x12: return MAKE_INSN3_RC(fdivs, FRT(state), FRA(state), FRB(state));
        case 0x14: return MAKE_INSN3_RC(fsubs, FRT(state), FRA(state), FRB(state));
        case 0x15: return MAKE_INSN3_RC(fadds, FRT(state), FRA(state), FRB(state));
        case 0x16: return MAKE_INSN2_RC(fsqrts, FRT(state), FRB(state));
        case 0x18: return MAKE_INSN2_RC(fres, FRT(state), FRB(state));
        case 0x19: return MAKE_INSN3_RC(fmuls, FRT(state), FRA(state), FRC(state));
        case 0x1A: return MAKE_INSN2_RC(frsqrtes, FRT(state), FRB(state));
        case 0x1C: return MAKE_INSN4_RC(fmsubs, FRT(state), FRA(state), FRC(state), FRB(state));
        case 0x1D: return MAKE_INSN4_RC(fmadds, FRT(state), FRA(state), FRC(state), FRB(state));
        case 0x1E: return MAKE_INSN4_RC(fnmsubs, FRT(state), FRA(state), FRC(state), FRB(state));
        case 0x1F: return MAKE_INSN4_RC(fnmadds, FRT(state), FRA(state), FRC(state), FRB(state));
        default:
            throw ExceptionPowerpc("invalid A-Form opcode: " + StringUtility::addrToString(int(fld<26, 30>(state))), state);
    }
    ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_A_formInstruction_3F(State &state) {
    // FIXME: Make the floating point registers use the powerpc_regclass_fpr instead of powerpc_regclass_gpr
    switch(fld<26, 30>(state)) {
        case 0x12: return MAKE_INSN3_RC(fdiv, FRT(state), FRA(state), FRB(state));
        case 0x14: return MAKE_INSN3_RC(fsub, FRT(state), FRA(state), FRB(state));
        case 0x15: return MAKE_INSN3_RC(fadd, FRT(state), FRA(state), FRB(state));
        case 0x16: return MAKE_INSN2_RC(fsqrt, FRT(state), FRB(state));
        case 0x17: return MAKE_INSN4_RC(fsel, FRT(state), FRA(state), FRC(state), FRB(state));
        case 0x18: return MAKE_INSN2_RC(fre, FRT(state), FRB(state));
        case 0x19: return MAKE_INSN3_RC(fmul, FRT(state), FRA(state), FRC(state));
        case 0x1A: return MAKE_INSN2_RC(frsqrte, FRT(state), FRB(state));
        case 0x1C: return MAKE_INSN4_RC(fmsub, FRT(state), FRA(state), FRC(state), FRB(state));
        case 0x1D: return MAKE_INSN4_RC(fmadd, FRT(state), FRA(state), FRC(state), FRB(state));
        case 0x1E: return MAKE_INSN4_RC(fnmsub, FRT(state), FRA(state), FRC(state), FRB(state));
        case 0x1F: return MAKE_INSN4_RC(fnmadd, FRT(state), FRA(state), FRC(state), FRB(state));
        default:
            throw ExceptionPowerpc("invalid A-Form opcode: " + StringUtility::addrToString(int(fld<26, 30>(state))), state);
    }
    ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_MD_formInstruction(State &state) {
    switch (fld<27, 29>(state)) {
        case 0: return MAKE_INSN4_RC(rldicl, RA(state), RS(state), SH_64bit(state), MB_64bit(state));
        case 1: return MAKE_INSN4_RC(rldicr, RA(state), RS(state), SH_64bit(state), ME_64bit(state));
        case 2: return MAKE_INSN4_RC(rldic, RA(state), RS(state), SH_64bit(state), MB_64bit(state));
        case 3: return MAKE_INSN4_RC(rldimi, RA(state), RS(state), SH_64bit(state), MB_64bit(state));
        case 4: return decode_MDS_formInstruction(state);
        default:
            throw ExceptionPowerpc("invalid MD-Form extended opcode: " + StringUtility::addrToString(int(fld<27, 29>(state))), state);
    }
    ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_MDS_formInstruction(State &state) {
    switch (fld<27, 30>(state)) {
        case 8: return MAKE_INSN4_RC(rldcl, RA(state), RS(state), RB(state), MB_64bit(state));
        case 9: return MAKE_INSN4_RC(rldcr, RA(state), RS(state), RB(state), ME_64bit(state));
        default:
            throw ExceptionPowerpc("invalid MDS-Form extended opcode: " + StringUtility::addrToString(int(fld<27, 30>(state))), state);
    }
    ASSERT_not_reachable("opcode not handled");
}

} // namespace
} // namespace

#endif
