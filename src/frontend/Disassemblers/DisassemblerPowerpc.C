#include "sage3basic.h"
#include "Assembler.h"
#include "AssemblerX86.h"
#include "AsmUnparser_compat.h"
#include "Disassembler.h"
#include "SageBuilderAsm.h"
#include "DisassemblerPowerpc.h"
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
    SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    switch (wordSize_) {
        case powerpc_32:
            return isa == SgAsmExecutableFileFormat::ISA_PowerPC;
        case powerpc_64:
            return isa == SgAsmExecutableFileFormat::ISA_PowerPC_64bit;
        default:
            return false;
    }
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
    REG_IP = *regdict->lookup("iar");
    REG_SP = *regdict->lookup("r1");
    REG_LINK = *regdict->lookup("lr");
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

    // Disassemble the instruction
    startInstruction(start_va, c);
    SgAsmPowerpcInstruction *insn = disassemble();      // throws an exception on error
    ASSERT_not_null(insn);

    // Note successors if necessary
    if (successors) {
        bool complete;
        AddressSet suc2 = insn->getSuccessors(&complete);
        successors->insert(suc2.begin(), suc2.end());
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
DisassemblerPowerpc::fld() const {
    return (insn >> (31 - Last)) & (IntegerOps::GenMask<uint32_t, Last - First + 1>::value);
}

// FIXME[Robb Matzke 2019-08-20]: Replace with proper C++ since nested macros make debugging hard
#define MAKE_INSN0(Mne) (makeInstructionWithoutOperands(ip, #Mne, powerpc_##Mne, insn))
#define MAKE_INSN0_RC(Mne) (Rc() ? MAKE_INSN0(Mne##_record) : MAKE_INSN0(Mne))
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
DisassemblerPowerpc::makeRegister(PowerpcRegisterClass reg_class, int reg_number,
                                  PowerpcConditionRegisterAccessGranularity cr_granularity,
                                  SgAsmType *registerType /*=NULL*/) const {
    // Obtain a register name and override the registerType for certain registers
    std::string name;
    switch (reg_class) {
        case powerpc_regclass_gpr:
            if (reg_number<0 || reg_number>=1024)
                throw ExceptionPowerpc("invalid gpr register number", this);
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
                throw ExceptionPowerpc("invalid fpr register number", this);
            name = "f" + StringUtility::numberToString(reg_number);
            registerType = SageBuilderAsm::buildIeee754Binary64();
            break;
        case powerpc_regclass_cr:
            name = "cr";
            switch (cr_granularity) {
                case powerpc_condreggranularity_whole:
                    if (0!=reg_number)
                        throw ExceptionPowerpc("invalid register number for cr", this);
                    break;
                case powerpc_condreggranularity_field:
                    // cr has eight 4-bit fields numbered 0..7 with names like "cr0"
                    if (reg_number<0 || reg_number>=8)
                        throw ExceptionPowerpc("invalid condition register granularity field", this);
                    name += StringUtility::numberToString(reg_number);
                    registerType = SageBuilderAsm::buildTypeU4();
                    break;
                case powerpc_condreggranularity_bit: {
                    // Each field has four bits with names. The full name of each bit is "crF.B" where "F" is the field number
                    // like above and "B" is the bit name. For instance, "cr0.eq".
                    if (reg_number<0 || reg_number>=32)
                        throw ExceptionPowerpc("invalid condition register granularity bit", this);
                    static const char *bitname[] = {"lt", "gt", "eq", "so"};
                    name += StringUtility::numberToString(reg_number/4) + "." + bitname[reg_number%4];
                    registerType = SageBuilderAsm::buildTypeU1();
                    break;
                }
            }
            break;
        case powerpc_regclass_fpscr:
            if (0!=reg_number)
                throw ExceptionPowerpc("invalid register number for fpscr", this);
            name = "fpscr";
            registerType = SageBuilderAsm::buildTypeU32();
            break;
        case powerpc_regclass_spr:
            // Some special purpose registers have special names, but the dictionary has the generic name as well.
            if (reg_number<0 || reg_number>=1024)
                throw ExceptionPowerpc("invalid spr register number", this);
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
                throw ExceptionPowerpc("invalid tbr register number", this);
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
                throw ExceptionPowerpc("invalid msr register number", this);
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
            throw ExceptionPowerpc("iar is not a real register", this);
        case powerpc_regclass_pvr:
            if (0!=reg_number)
                throw ExceptionPowerpc("invalid pvr register number", this);
            name = "pvr";
            registerType = SageBuilderAsm::buildTypeU32();
            break;
        case powerpc_regclass_unknown:
        case powerpc_last_register_class:
            throw ExceptionPowerpc("not a real register", this);
        // Don't add a "default" or else we won't get compiler warnings if a new class is defined.
    }
    ASSERT_forbid(name.empty());
    ASSERT_not_null(registerType);

    // Obtain a register descriptor from the dictionary
    ASSERT_not_null(registerDictionary());
    const RegisterDescriptor *rdesc = registerDictionary()->lookup(name);
    if (!rdesc)
        throw ExceptionPowerpc("register \"" + name + "\" is not available for " + registerDictionary()->get_architecture_name(), this);
    ASSERT_require2(rdesc->nBits() == registerType->get_nBits(),
                    (boost::format("register width (%|u|) doesn't match type width (%|u|)")
                     % rdesc->nBits() % registerType->get_nBits()).str());

    // Construct the return value
    SgAsmRegisterReferenceExpression *rre = new SgAsmDirectRegisterExpression(*rdesc);
    ASSERT_not_null(rre);
    rre->set_type(registerType);
    return rre;
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::disassemble() {
    // The Primary Opcode Field is bits 0-5, 6-bits wide, so there are max 64 primary opcode values
    uint8_t primaryOpcode = (insn >> 26) & 0x3F;

    switch (primaryOpcode) {
        case 0x00: {
            // These are the BGL specific PowerPC440 FP2 Architecture instructions.  Depending on if this is A form or X form,
            // the extended opCode maps to different bit ranges, so this code is incorrect!
            uint8_t a_Opcode = (insn >> 1) & 0x1F;

            // Different parts of the instruction are used to identify what kind of instruction this is!
            if (a_Opcode == 5 || (a_Opcode >= 8 && a_Opcode <= 31)) {
                return decode_A_formInstruction_00();
            } else {
                return decode_X_formInstruction_00();
            }
            break;
        }

        case 0x01: throw ExceptionPowerpc("invalid primary opcode (0x01)", this, 26);
        case 0x02: return MAKE_INSN3(tdi, TO(), RA(), SI());
        case 0x03: return MAKE_INSN3(twi, TO(), RA(), SI());
        case 0x04: return decode_A_formInstruction_04();
        case 0x05: throw ExceptionPowerpc("invalid primary opcode (0x05)", this, 26);
        case 0x06: throw ExceptionPowerpc("invalid primary opcode (0x06)", this, 26);
        case 0x07: return MAKE_INSN3(mulli, RT(), RA(), SI());
        case 0x08: return MAKE_INSN3(subfic, RT(), RA(), SI());
        case 0x09: throw ExceptionPowerpc("invalid primary opcode (0x09)", this, 26);
        case 0x0A: return MAKE_INSN4(cmpli, BF_cr(), L_10(), RA(), UI());
        case 0x0B: return MAKE_INSN4(cmpi, BF_cr(), L_10(), RA(), SI());
        case 0x0C: return MAKE_INSN3(addic, RT(), RA(), SI());
        case 0x0D: return MAKE_INSN3(addic_record, RT(), RA(), SI());
        case 0x0E: return MAKE_INSN3(addi, RT(), RA_or_zero(), SI());
        case 0x0F: return MAKE_INSN3(addis, RT(), RA_or_zero(), SI());
        case 0x10: return decode_B_formInstruction();
        case 0x11: return decode_SC_formInstruction();
        case 0x12: return decode_I_formInstruction();
        case 0x13: return decode_XL_formInstruction();
        case 0x14: return MAKE_INSN5_RC(rlwimi, RA(), RS(), SH_32bit(), MB_32bit(), ME_32bit());
        case 0x15: return MAKE_INSN5_RC(rlwinm, RA(), RS(), SH_32bit(), MB_32bit(), ME_32bit());
        case 0x17: return MAKE_INSN5_RC(rlwnm, RA(), RS(), RB(), MB_32bit(), ME_32bit());
        case 0x18: return MAKE_INSN3(ori, RA(), RS(), UI());
        case 0x19: return MAKE_INSN3(oris, RA(), RS(), UI());
        case 0x1A: return MAKE_INSN3(xori, RA(), RS(), UI());
        case 0x1B: return MAKE_INSN3(xoris, RA(), RS(), UI());
        case 0x1C: return MAKE_INSN3(andi_record, RA(), RS(), UI());
        case 0x1D: return MAKE_INSN3(andis_record, RA(), RS(), UI());
        case 0x1E: return decode_MD_formInstruction();
        case 0x1F: return decode_X_formInstruction_1F();
        case 0x20: return MAKE_INSN2(lwz, RT(), memref(T_U32));
        case 0x21: return MAKE_INSN2(lwzu, RT(), memrefu(T_U32));
        case 0x22: return MAKE_INSN2(lbz, RT(), memref(T_U8));
        case 0x23: return MAKE_INSN2(lbzu, RT(), memrefu(T_U8));
        case 0x24: return MAKE_INSN2(stw, RS(), memref(T_U32));
        case 0x25: return MAKE_INSN2(stwu, RS(), memrefu(T_U32));
        case 0x26: return MAKE_INSN2(stb, RS(), memref(T_U8));
        case 0x27: return MAKE_INSN2(stbu, RS(), memrefu(T_U8));
        case 0x28: return MAKE_INSN2(lhz, RT(), memref(T_U16));
        case 0x29: return MAKE_INSN2(lhzu, RT(), memrefu(T_U16));
        case 0x2A: return MAKE_INSN2(lha, RT(), memref(T_U16));
        case 0x2B: return MAKE_INSN2(lhau, RT(), memrefu(T_U16));
        case 0x2C: return MAKE_INSN2(sth, RS(), memref(T_U16));
        case 0x2D: return MAKE_INSN2(sthu, RS(), memrefu(T_U16));
        case 0x2E: return MAKE_INSN2(lmw, RT(), memref(T_U32));
        case 0x2F: return MAKE_INSN2(stmw, RS(), memref(T_U32));
        case 0x30: return MAKE_INSN2(lfs, FRT(), memref(T_FLOAT32));
        case 0x31: return MAKE_INSN2(lfsu, FRT(), memrefu(T_FLOAT32));
        case 0x32: return MAKE_INSN2(lfd, FRT(), memref(T_FLOAT64));
        case 0x33: return MAKE_INSN2(lfdu, FRT(), memrefu(T_FLOAT64));
        case 0x34: return MAKE_INSN2(stfs, FRS(), memref(T_FLOAT32));
        case 0x35: return MAKE_INSN2(stfsu, FRS(), memrefu(T_FLOAT32));
        case 0x36: return MAKE_INSN2(stfd, FRS(), memref(T_FLOAT64));
        case 0x37: return MAKE_INSN2(stfdu, FRS(), memrefu(T_FLOAT64));
        case 0x38: throw ExceptionPowerpc("invalid primary opcode (0x38)", this, 26);
        case 0x39: throw ExceptionPowerpc("invalid primary opcode (0x39)", this, 26);
        case 0x3A: {
            switch (unsigned xo = insn & 0x3) {
                case 0: return MAKE_INSN2(ld, RT(), memrefds(T_U64));
                case 1: return MAKE_INSN2(ldu, RT(), memrefds(T_U64));
                case 2: return MAKE_INSN2(lwa, RT(), memrefds(T_U32));
                default:
                    throw ExceptionPowerpc("decoding error for pimary opcode 0x3a, XO=" + StringUtility::numberToString(xo),
                                           this);
            }
            break;
        }
        case 0x3B: return decode_A_formInstruction_3B();
        case 0x3C: throw ExceptionPowerpc("invalid primary opcode (0x3c)", this, 26);
        case 0x3D: throw ExceptionPowerpc("invalid primary opcode (0x3d)", this, 26);
        case 0x3E: return decode_DS_formInstruction();
        case 0x3F: {
            // Depending on if this is A form or X form, the extended opCode maps to different bit ranges, so this code is
            // incorrect!
            uint8_t  a_Opcode   = (insn >> 1) & 0x1F;

            // Different parts of the instruction are used to identify what kind of instruction this is!
            if (a_Opcode == 18 || (a_Opcode >= 20 && a_Opcode <= 31)) {
                return decode_A_formInstruction_3F();
            } else {
                return decode_X_formInstruction_3F(); // Also includes XFL form
            }
            break;
        }

        default:
            throw ExceptionPowerpc("illegal primary opcode: "+StringUtility::addrToString(primaryOpcode), this, 26);
    }

    ASSERT_not_reachable("op code not handled");
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_I_formInstruction() {
    uint8_t primaryOpcode = (insn >> 26) & 0x3F;
    switch(primaryOpcode) {
        case 0x12: {
            rose_addr_t targetBranchAddress = LI();
            if (AA() == 0)
                targetBranchAddress += ip;

            SgAsmIntegerValueExpression* targetAddressExpression = makeBranchTarget(targetBranchAddress);

            if (AA() == 0) {
                if (LK() == 0) {
                    return MAKE_INSN1(b, targetAddressExpression);
                } else {
                    return MAKE_INSN1(bl, targetAddressExpression);
                }
            } else {
                if (LK() == 0) {
                    return MAKE_INSN1(ba, targetAddressExpression);
                } else {
                    return MAKE_INSN1(bla, targetAddressExpression);
                }
            }
            break;
        }

        default:
            throw ExceptionPowerpc("invaild I-Form primary opcode: " + StringUtility::addrToString(primaryOpcode), this);
    }
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_B_formInstruction() {
     uint8_t primaryOpcode = (insn >> 26) & 0x3F;
     switch(primaryOpcode) {
         case 0x10: {
             uint64_t targetBranchAddress = BD();
             if (AA() == 0)
                 targetBranchAddress += ip;
             SgAsmIntegerValueExpression* targetAddressExpression = makeBranchTarget(targetBranchAddress);

             if (LK() == 0) {
                 if (AA() == 0) {
                     return MAKE_INSN3(bc, BO(), BI(), targetAddressExpression);
                 } else {
                     return MAKE_INSN3(bca, BO(), BI(), targetAddressExpression);
                 }
             } else {
                 if (AA() == 0) {
                     return MAKE_INSN3(bcl, BO(), BI(), targetAddressExpression);
                 } else {
                     return MAKE_INSN3(bcla, BO(), BI(), targetAddressExpression);
                 }
             }
             break;
         }

         default:
             throw ExceptionPowerpc("invalid B-Form primary opcode: " + StringUtility::addrToString(primaryOpcode), this);
     }
     ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_SC_formInstruction() {
    uint8_t primaryOpcode = (insn >> 26) & 0x3F;
    ASSERT_always_require(primaryOpcode == 0x11);

    // Get bit 30, 1 bit as the reserved flag
    uint8_t constantOneOpcode = (insn >> 1) & 0x1;
    if (constantOneOpcode!=1)
        throw ExceptionPowerpc("expected bit to be set", this, 1);

    return MAKE_INSN1(sc, LEV());
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_DS_formInstruction() {
    uint8_t primaryOpcode = (insn >> 26) & 0x3f;
    ASSERT_always_require(primaryOpcode == 0x3e);

    switch (insn & 0x03) {
        case 0: return MAKE_INSN2(std, RS(), memrefds(T_U64));
        case 1: return MAKE_INSN2(stdu, RS(), memrefds(T_U64));
        default: throw ExceptionPowerpc("invalid DS-form instruction", this);
    }
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_X_formInstruction_1F() {
    uint8_t primaryOpcode = (insn >> 26) & 0x3F;
    ASSERT_always_require(primaryOpcode == 0x1F);

    // Get the bits 21-30, next 10 bits
    uint16_t xoOpcode = (insn >> 1) & 0x3FF;
    switch(xoOpcode) {
        case 0x000: return MAKE_INSN4(cmp, BF_cr(), L_10(), RA(), RB());
        case 0x004: return MAKE_INSN3(tw, TO(), RA(), RB());
        case 0x008: return MAKE_INSN3_RC(subfc, RT(), RA(), RB());
        case 0x009: return MAKE_INSN3_RC(mulhdu, RT(), RA(), RB());
        case 0x00A: return MAKE_INSN3_RC(addc, RT(), RA(), RB());
        case 0x00B: return MAKE_INSN3_RC(mulhwu, RT(), RA(), RB());
        case 0x014: return MAKE_INSN2(lwarx, RT(), memrefx(T_U32));
        case 0x015: return MAKE_INSN2(ldx, RT(), memrefx(T_U64));
        case 0x017: return MAKE_INSN2(lwzx, RT(), memrefx(T_U32));
        case 0x018: return MAKE_INSN3_RC(slw, RA(), RS(), RB());
        case 0x01A: return MAKE_INSN2_RC(cntlzw, RA(), RS());
        case 0x01B: return MAKE_INSN3_RC(sld, RA(), RS(), RB());
        case 0x01C: return MAKE_INSN3_RC(and, RA(), RS(), RB());
        case 0x020: return MAKE_INSN4(cmpl, BF_cr(), L_10(), RA(), RB());
        case 0x028: return MAKE_INSN3_RC(subf, RT(), RA(), RB());
        case 0x035: return MAKE_INSN2(ldux, RT(), memrefux(T_U64));
        case 0x037: return MAKE_INSN2(lwzux, RT(), memrefux(T_U32));
        case 0x03a: return MAKE_INSN2_RC(cntlzd, RA(), RS());
        case 0x03C: return MAKE_INSN3_RC(andc, RA(), RS(), RB());
        case 0x044: return MAKE_INSN3(td, TO(), RA(), RB());
        case 0x049: return MAKE_INSN3_RC(mulhd, RT(), RA(), RB());
        case 0x04B: return MAKE_INSN3_RC(mulhw, RT(), RA(), RB());
        case 0x053: return MAKE_INSN1(mfmsr, RT());
        case 0x057: return MAKE_INSN2(lbzx, RT(), memrefx(T_U8));
        case 0x068: return MAKE_INSN2_RC(neg, RT(), RA());
        case 0x077: return MAKE_INSN2(lbzux, RT(), memrefux(T_U8));
        case 0x07A: return MAKE_INSN2(popcntb, RA(), RS());
        case 0x07C: return MAKE_INSN3_RC(nor, RA(), RS(), RB());
        case 0x088: return MAKE_INSN3_RC(subfe, RT(), RA(), RB());
        case 0x08A: return MAKE_INSN3_RC(adde, RT(), RA(), RB());
        case 0x08E: return MAKE_INSN2(lfssx, FRT(), memrefx(T_FLOAT32));
        case 0x095: return MAKE_INSN2(stdx, RS(), memrefx(T_U64));
        case 0x096: return MAKE_INSN2(stwcx_record, RS(), memrefx(T_U32));
        case 0x097: return MAKE_INSN2(stwx, RS(), memrefx(T_U32));
        case 0x0AE: return MAKE_INSN2(lfssux, FRT(), memrefux(T_FLOAT32));
        case 0x0B5: return MAKE_INSN2(stdux, RS(), memrefux(T_U64));
        case 0x0B7: return MAKE_INSN2(stwux, RS(), memrefux(T_U32));
        case 0x0C8: return MAKE_INSN2_RC(subfze, RT(), RA());
        case 0x0CA: return MAKE_INSN2_RC(addze, RT(), RA());
        case 0x0CE: return MAKE_INSN2(lfsdx, FRT(), memrefx(T_FLOAT64));
        case 0x0D6: return MAKE_INSN2(stdcx_record, RS(), memrefx(T_U64));
        case 0x0D7: return MAKE_INSN2(stbx, RS(), memrefx(T_U8));
        case 0x0E8: return MAKE_INSN2_RC(subfme, RT(), RA());
        case 0x0EA: return MAKE_INSN2_RC(addme, RT(), RA());
        case 0x0EB: return MAKE_INSN3_RC(mullw, RT(), RA(), RB());
        case 0x0EE: return MAKE_INSN2(lfsdux, FRT(), memrefux(T_FLOAT64));
        case 0x0f4: return MAKE_INSN3_RC(mulld, RT(), RA(), RB());
        case 0x0F7: return MAKE_INSN2(stbux, RS(), memrefux(T_U8));
        case 0x10A: return MAKE_INSN3_RC(add, RT(), RA(), RB());
        case 0x10E: return MAKE_INSN2(lfxsx, FRT(), memrefx(T_V2_FLOAT32));
        case 0x116: return MAKE_INSN1(dcbt, memrefx(T_U8));
        case 0x117: return MAKE_INSN2(lhzx, RT(), memrefx(T_U16));
        case 0x11C: return MAKE_INSN3_RC(eqv, RA(), RS(), RB());
        case 0x12E: return MAKE_INSN2(lfxsux, FRT(), memrefux(T_V2_FLOAT32));
        case 0x137: return MAKE_INSN2(lhzux, RT(), memrefux(T_U16));
        case 0x13C: return MAKE_INSN3_RC(xor, RA(), RS(), RB());
        case 0x14E: return MAKE_INSN2(lfxdx, FRT(), memrefx(T_V2_FLOAT64));
        case 0x153: return MAKE_INSN2(mfspr, RT(), SPR());
        case 0x155: return MAKE_INSN2(lwax, RT(), memrefx(T_U32));
        case 0x157: return MAKE_INSN2(lhax, RT(), memrefx(T_U16));
        case 0x16E: return MAKE_INSN2(lfxdux, FRT(), memrefux(T_V2_FLOAT64));
        case 0x175: return MAKE_INSN2(lwaux, RT(), memrefux(T_U32));
        case 0x177: return MAKE_INSN2(lhaux, RT(), memrefux(T_U16));
        case 0x18E: return MAKE_INSN2(lfpsx, FRT(), memrefx(T_V2_FLOAT32));
        case 0x197: return MAKE_INSN2(sthx, RS(), memrefx(T_U16));
        case 0x19C: return MAKE_INSN3_RC(orc, RA(), RS(), RB());
        case 0x1AE: return MAKE_INSN2(lfpsux, FRT(), memrefux(T_V2_FLOAT32));
        case 0x1B7: return MAKE_INSN2(sthux, RS(), memrefux(T_U16));
        case 0x1BC: return MAKE_INSN3_RC(or, RA(), RS(), RB());
        case 0x1C9: return MAKE_INSN3_RC(divdu, RT(), RA(), RB());
        case 0x1CB: return MAKE_INSN3_RC(divwu, RT(), RA(), RB());
        case 0x1CE: return MAKE_INSN2(lfpdx, FRT(), memrefx(T_V2_FLOAT64));
        case 0x1D3: return MAKE_INSN2(mtspr, SPR(), RS());
        case 0x1DC: return MAKE_INSN3_RC(nand, RA(), RS(), RB());
        case 0x1E9: return MAKE_INSN3_RC(divd, RT(), RA(), RB());
        case 0x1EB: return MAKE_INSN3_RC(divw, RT(), RA(), RB());
        case 0x1EE: return MAKE_INSN2(lfpdux, FRT(), memrefux(T_V2_FLOAT64));
        case 0x208: return MAKE_INSN3_RC(subfco, RT(), RA(), RB());
        case 0x20A: return MAKE_INSN3_RC(addco, RT(), RA(), RB());
        case 0x20E: return MAKE_INSN2(stfpiwx, FRT(), memrefx(T_V2_U32));
        case 0x215: return MAKE_INSN2(lswx, RT(), memrefx(T_U8));
        case 0x216: return MAKE_INSN2(lwbrx, RT(), memrefx(T_U32));
        case 0x217: return MAKE_INSN2(lfsx, FRT(), memrefx(T_FLOAT32));
        case 0x218: return MAKE_INSN3_RC(srw, RA(), RS(), RB());
        case 0x21B: return MAKE_INSN3_RC(srd, RA(), RS(), RB());
        case 0x228: return MAKE_INSN3_RC(subfo, RT(), RA(), RB());
        case 0x237: return MAKE_INSN2(lfsux, FRT(), memrefux(T_FLOAT32));
        case 0x255: return MAKE_INSN3(lswi, RT(), memrefra(T_U8), NB());
        case 0x257: return MAKE_INSN2(lfdx, FRT(), memrefx(T_FLOAT64));
        case 0x268: return MAKE_INSN2_RC(nego, RT(), RA());
        case 0x288: return MAKE_INSN3_RC(subfeo, RT(), RA(), RB());
        case 0x28A: return MAKE_INSN3_RC(addeo, RT(), RA(), RB());
        case 0x28E: return MAKE_INSN2(stfssx, FRT(), memrefx(T_FLOAT32));
        case 0x295: return MAKE_INSN2(stswx, RS(), memrefx(T_U8));
        case 0x296: return MAKE_INSN2(stwbrx, RS(), memrefx(T_U32));
        case 0x2AE: return MAKE_INSN2(stfssux, FRT(), memrefux(T_FLOAT32));
        case 0x2C8: return MAKE_INSN2_RC(subfzeo, RT(), RA());
        case 0x2CA: return MAKE_INSN2_RC(addzeo, RT(), RA());
        case 0x2CE: return MAKE_INSN2(stfsdx, FRT(), memrefx(T_FLOAT64));
        case 0x2D5: return MAKE_INSN3(stswi, RS(), memrefra(T_U8), NB());
        case 0x2D7: return MAKE_INSN2(stfdx, FRS(), memrefx(T_FLOAT64));
        case 0x2E8: return MAKE_INSN2_RC(subfmeo, RT(), RA());
        case 0x2EA: return MAKE_INSN2_RC(addmeo, RT(), RA());
        case 0x2EB: return MAKE_INSN3_RC(mullwo, RT(), RA(), RB());
        case 0x2EE: return MAKE_INSN2(stfsdux, FRT(), memrefux(T_FLOAT64));
        case 0x2F4: return MAKE_INSN3_RC(mulldo, RT(), RA(), RB());
        case 0x2F7: return MAKE_INSN2(stfdux, FRS(), memrefx(T_FLOAT64));
        case 0x30A: return MAKE_INSN3_RC(addo, RT(), RA(), RB());
        case 0x30E: return MAKE_INSN2(stfxsx, FRT(), memrefx(T_V2_FLOAT32));
        case 0x316: return MAKE_INSN2(lhbrx, RT(), memrefx(T_U16));
        case 0x318: return MAKE_INSN3_RC(sraw, RA(), RS(), RB());
        case 0x31A: return MAKE_INSN3_RC(srad, RA(), RS(), RB());
        case 0x32E: return MAKE_INSN2(stfxsux, FRT(), memrefux(T_V2_FLOAT32));
        case 0x338: return MAKE_INSN3_RC(srawi, RA(), RS(), SH_32bit());
        case 0x33A: return MAKE_INSN3_RC(sradi, RA(), RS(), SH_64bit()); // The last bit of the ext. opcode is part of SH
        case 0x33B: return MAKE_INSN3_RC(sradi, RA(), RS(), SH_64bit()); // Same as previous insn
        case 0x34E: return MAKE_INSN2(stfxdx, FRT(), memrefx(T_V2_FLOAT64));
        case 0x356: return MAKE_INSN0(eieio);
        case 0x36E: return MAKE_INSN2(stfxdux, FRT(), memrefux(T_V2_FLOAT64));
        case 0x38E: return MAKE_INSN2(stfpsx, FRT(), memrefx(T_V2_FLOAT32));
        case 0x396: return MAKE_INSN2(sthbrx, RS(), memrefx(T_U16));
        case 0x39A: return MAKE_INSN2_RC(extsh, RA(), RS());
        case 0x3AE: return MAKE_INSN2(stfpsux, FRT(), memrefux(T_V2_FLOAT32));
        case 0x3BA: return MAKE_INSN2_RC(extsb, RA(), RS());
        case 0x3C9: return MAKE_INSN3_RC(divduo, RT(), RA(), RB());
        case 0x3CB: return MAKE_INSN3_RC(divwuo, RT(), RA(), RB());
        case 0x3CE: return MAKE_INSN2(stfpdx, FRT(), memrefx(T_V2_FLOAT64));
        case 0x3D7: return MAKE_INSN2(stfiwx, FRS(), memrefx(T_U32));
        case 0x3DA: return MAKE_INSN2_RC(extsw, RA(), RS());
        case 0x3E9: return MAKE_INSN3_RC(divdo, RT(), RA(), RB());
        case 0x3EB: return MAKE_INSN3_RC(divwo, RT(), RA(), RB());
        case 0x3EE: return MAKE_INSN2(stfpdux, FRT(), memrefux(T_V2_FLOAT64));
        case 0x3F6: return MAKE_INSN1(dcbz, memrefx(T_U8));
        case 0x13:
            if (fld<11, 11>() == 0) {
                return MAKE_INSN1(mfcr, RT());
            } else {
                return MAKE_INSN1(mfcr, RT());
            }
            break;
        case 0x90:
            if (fld<11, 11>() == 0) {
                return MAKE_INSN2(mtcrf, FXM(), RS());
            } else {
                return MAKE_INSN2(mtcrf, FXM(), RS());
            }
            break;
        default:
            throw ExceptionPowerpc("X-Form 1F xoOpcode not handled: " + StringUtility::addrToString(xoOpcode), this, 1);
    }
    ASSERT_not_reachable("unhandled opcode");
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_X_formInstruction_3F() {
    uint8_t primaryOpcode = (insn >> 26) & 0x3F;
    ASSERT_always_require(primaryOpcode == 0x3F);

    // Get the bits 21-30, next 10 bits
    uint16_t xoOpcode = (insn >> 1) & 0x3FF;
    switch(xoOpcode) {
        case 0x000: return MAKE_INSN3(fcmpu, BF_cr(), FRA(), FRB());
        case 0x00C: return MAKE_INSN2_RC(frsp, FRT(), FRB());
        case 0x00E: return MAKE_INSN2_RC(fctiw, FRT(), FRB());
        case 0x00F: return MAKE_INSN2_RC(fctiwz, FRT(), FRB());
        case 0x026: return MAKE_INSN1(mtfsb1, BT());
        case 0x028: return MAKE_INSN2_RC(fneg, FRT(), FRB());
        case 0x046: return MAKE_INSN1_RC(mtfsb0, BT());
        case 0x048: return MAKE_INSN2_RC(fmr, FRT(), FRB());
        case 0x086: return MAKE_INSN2_RC(mtfsfi, BF_fpscr(), U());
        case 0x088: return MAKE_INSN2_RC(fnabs, FRT(), FRB());
        case 0x108: return MAKE_INSN2_RC(fabs, FRT(), FRB());
        case 0x247: return MAKE_INSN1_RC(mffs, FRT());
        case 0x2C7: return MAKE_INSN2_RC(mtfsf, FLM(), FRB());
        default:
            throw ExceptionPowerpc("X-Form 3F xoOpcode not handled: " + StringUtility::addrToString(xoOpcode), this, 1);
    }
    ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_X_formInstruction_00() {
    if (insn == 0)
        throw ExceptionPowerpc("zero instruction", this, 0);

    // Get the bits 21-30, next 10 bits
    uint16_t xoOpcode = (insn >> 1) & 0x3FF;
    switch(xoOpcode) {
        case 0x020: return MAKE_INSN2(fpmr, FRT(), FRB());
        case 0x060: return MAKE_INSN2(fpabs, FRT(), FRB());
        case 0x0A0: return MAKE_INSN2(fpneg, FRT(), FRB());
        case 0x0C0: return MAKE_INSN2(fprsp, FRT(), FRB());
        case 0x0E0: return MAKE_INSN2(fpnabs, FRT(), FRB());
        case 0x120: return MAKE_INSN2(fsmr, FRT(), FRB());
        case 0x160: return MAKE_INSN2(fsabs, FRT(), FRB());
        case 0x1A0: return MAKE_INSN2(fsneg, FRT(), FRB());
        case 0x1E0: return MAKE_INSN2(fsnabs, FRT(), FRB());
        case 0x220: return MAKE_INSN2(fxmr, FRT(), FRB());
        case 0x240: return MAKE_INSN2(fpctiw, FRT(), FRB());
        case 0x2C0: return MAKE_INSN2(fpctiwz, FRT(), FRB());
        case 0x320: return MAKE_INSN2(fsmtp, FRT(), FRB());
        case 0x3A0: return MAKE_INSN2(fsmfp, FRT(), FRB());
        default:
            throw ExceptionPowerpc("X-Form 00 xoOpcode not handled: " + StringUtility::addrToString(xoOpcode), this, 1);
    }
    ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_XL_formInstruction() {
    // The primaryOpcode
    uint8_t primaryOpcode = (insn >> 26) & 0x3F;
    ASSERT_always_require(primaryOpcode == 0x13);

    // Get the bits 21-30, next 10 bits
    uint16_t xoOpcode = (insn >> 1) & 0x3FF;
    switch(xoOpcode) {
        case 0x000: return MAKE_INSN2(mcrf, BF_cr(), BFA_cr());
        case 0x010:
            if (LK() == 0) {
                return MAKE_INSN3(bclr, BO(), BI(), BH());
            } else {
                return MAKE_INSN3(bclrl, BO(), BI(), BH());
            }
            break;
        case 0x021: return MAKE_INSN3(crnor, BT(), BA(), BB());
        case 0x081: return MAKE_INSN3(crandc, BT(), BA(), BB());
        case 0x0C1: return MAKE_INSN3(crxor, BT(), BA(), BB());
        case 0x0E1: return MAKE_INSN3(crnand, BT(), BA(), BB());
        case 0x101: return MAKE_INSN3(crand, BT(), BA(), BB());
        case 0x121: return MAKE_INSN3(creqv, BT(), BA(), BB());
        case 0x1A1: return MAKE_INSN3(crorc, BT(), BA(), BB());
        case 0x1C1: return MAKE_INSN3(cror, BT(), BA(), BB());
        case 0x210:
            if (LK() == 0) {
                return MAKE_INSN3(bcctr, BO(), BI(), BH());
            } else {
                return MAKE_INSN3(bcctrl, BO(), BI(), BH());
            }
            break;

        default:
            throw ExceptionPowerpc("invalid XL-Form opcode: " + StringUtility::addrToString(xoOpcode), this);
    }

    ASSERT_not_reachable("xoOpcode not handled");
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_A_formInstruction_00() {
    switch(fld<26, 30>()) {
        case 0x05: return MAKE_INSN4(fpsel, FRT(), FRA(), FRB(), FRC());
        case 0x08: return MAKE_INSN3(fpmul, FRT(), FRA(), FRC());
        case 0x09: return MAKE_INSN3(fxmul, FRT(), FRA(), FRC());
        case 0x0A: return MAKE_INSN3(fxpmul, FRT(), FRA(), FRC());
        case 0x0B: return MAKE_INSN3(fxsmul, FRT(), FRA(), FRC());
        case 0x0C: return MAKE_INSN3(fpadd, FRT(), FRA(), FRB());
        case 0x0D: return MAKE_INSN3(fpsub, FRT(), FRA(), FRB());
        case 0x0E: return MAKE_INSN2(fpre, FRT(), FRB());
        case 0x0F: return MAKE_INSN2(fprsqrte, FRT(), FRB());
        case 0x10: return MAKE_INSN4(fpmadd, FRT(), FRA(), FRB(), FRC());
        case 0x11: return MAKE_INSN4(fxmadd, FRT(), FRA(), FRB(), FRC());
        case 0x12: return MAKE_INSN4(fxcpmadd, FRT(), FRA(), FRB(), FRC());
        case 0x13: return MAKE_INSN4(fxcsmadd, FRT(), FRA(), FRB(), FRC());
        case 0x14: return MAKE_INSN4(fpnmadd, FRT(), FRA(), FRB(), FRC());
        case 0x15: return MAKE_INSN4(fxnmadd, FRT(), FRA(), FRB(), FRC());
        case 0x16: return MAKE_INSN4(fxcpnmadd, FRT(), FRA(), FRB(), FRC());
        case 0x17: return MAKE_INSN4(fxcsnmadd, FRT(), FRA(), FRB(), FRC());
        case 0x18: return MAKE_INSN4(fpmsub, FRT(), FRA(), FRB(), FRC());
        case 0x19: return MAKE_INSN4(fxmsub, FRT(), FRA(), FRB(), FRC());
        case 0x1A: return MAKE_INSN4(fxcpmsub, FRT(), FRA(), FRB(), FRC());
        case 0x1B: return MAKE_INSN4(fxcsmsub, FRT(), FRA(), FRB(), FRC());
        case 0x1C: return MAKE_INSN4(fpnmsub, FRT(), FRA(), FRB(), FRC());
        case 0x1D: return MAKE_INSN4(fxnmsub, FRT(), FRA(), FRB(), FRC());
        case 0x1E: return MAKE_INSN4(fxcpnmsub, FRT(), FRA(), FRB(), FRC());
        case 0x1F: return MAKE_INSN4(fxcsnmsub, FRT(), FRA(), FRB(), FRC());
        default:
            throw ExceptionPowerpc("invalid A-Form opcode: " + StringUtility::addrToString(int(fld<26, 30>())), this);
    }
    ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_A_formInstruction_04() {
    // FIXME: Make the floating point registers use the powerpc_regclass_fpr instead of powerpc_regclass_gpr
    switch(fld<26, 30>()) {
        case 0x18: return MAKE_INSN4(fxcpnpma, FRT(), FRA(), FRB(), FRC());
        case 0x19: return MAKE_INSN4(fxcsnpma, FRT(), FRA(), FRB(), FRC());
        case 0x1A: return MAKE_INSN4(fxcpnsma, FRT(), FRA(), FRB(), FRC());
        case 0x1B: return MAKE_INSN4(fxcsnsma, FRT(), FRA(), FRB(), FRC());
        case 0x1C: return MAKE_INSN4(fxcxma, FRT(), FRA(), FRB(), FRC());
        case 0x1D: return MAKE_INSN4(fxcxnpma, FRT(), FRA(), FRB(), FRC());
        case 0x1E: return MAKE_INSN4(fxcxnsma, FRT(), FRA(), FRB(), FRC());
        case 0x1F: return MAKE_INSN4(fxcxnms, FRT(), FRA(), FRB(), FRC());
        default:
            throw ExceptionPowerpc("invalid A-Form opcode: " + StringUtility::addrToString(int(fld<26, 30>())), this);
    }
    ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_A_formInstruction_3B() {
    switch(fld<26, 30>()) {
        case 0x12: return MAKE_INSN3_RC(fdivs, FRT(), FRA(), FRB());
        case 0x14: return MAKE_INSN3_RC(fsubs, FRT(), FRA(), FRB());
        case 0x15: return MAKE_INSN3_RC(fadds, FRT(), FRA(), FRB());
        case 0x16: return MAKE_INSN2_RC(fsqrts, FRT(), FRB());
        case 0x18: return MAKE_INSN2_RC(fres, FRT(), FRB());
        case 0x19: return MAKE_INSN3_RC(fmuls, FRT(), FRA(), FRC());
        case 0x1A: return MAKE_INSN2_RC(frsqrtes, FRT(), FRB());
        case 0x1C: return MAKE_INSN4_RC(fmsubs, FRT(), FRA(), FRC(), FRB());
        case 0x1D: return MAKE_INSN4_RC(fmadds, FRT(), FRA(), FRC(), FRB());
        case 0x1E: return MAKE_INSN4_RC(fnmsubs, FRT(), FRA(), FRC(), FRB());
        case 0x1F: return MAKE_INSN4_RC(fnmadds, FRT(), FRA(), FRC(), FRB());
        default:
            throw ExceptionPowerpc("invalid A-Form opcode: " + StringUtility::addrToString(int(fld<26, 30>())), this);
    }
    ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_A_formInstruction_3F() {
    // FIXME: Make the floating point registers use the powerpc_regclass_fpr instead of powerpc_regclass_gpr
    switch(fld<26, 30>()) {
        case 0x12: return MAKE_INSN3_RC(fdiv, FRT(), FRA(), FRB());
        case 0x14: return MAKE_INSN3_RC(fsub, FRT(), FRA(), FRB());
        case 0x15: return MAKE_INSN3_RC(fadd, FRT(), FRA(), FRB());
        case 0x16: return MAKE_INSN2_RC(fsqrt, FRT(), FRB());
        case 0x17: return MAKE_INSN4_RC(fsel, FRT(), FRA(), FRC(), FRB());
        case 0x18: return MAKE_INSN2_RC(fre, FRT(), FRB());
        case 0x19: return MAKE_INSN3_RC(fmul, FRT(), FRA(), FRC());
        case 0x1A: return MAKE_INSN2_RC(frsqrte, FRT(), FRB());
        case 0x1C: return MAKE_INSN4_RC(fmsub, FRT(), FRA(), FRC(), FRB());
        case 0x1D: return MAKE_INSN4_RC(fmadd, FRT(), FRA(), FRC(), FRB());
        case 0x1E: return MAKE_INSN4_RC(fnmsub, FRT(), FRA(), FRC(), FRB());
        case 0x1F: return MAKE_INSN4_RC(fnmadd, FRT(), FRA(), FRC(), FRB());
        default:
            throw ExceptionPowerpc("invalid A-Form opcode: " + StringUtility::addrToString(int(fld<26, 30>())), this);
    }
    ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_MD_formInstruction() {
    switch (fld<27, 29>()) {
        case 0: return MAKE_INSN4_RC(rldicl, RA(), RS(), SH_64bit(), MB_64bit());
        case 1: return MAKE_INSN4_RC(rldicr, RA(), RS(), SH_64bit(), ME_64bit());
        case 2: return MAKE_INSN4_RC(rldic, RA(), RS(), SH_64bit(), MB_64bit());
        case 3: return MAKE_INSN4_RC(rldimi, RA(), RS(), SH_64bit(), MB_64bit());
        case 4: return decode_MDS_formInstruction();
        default:
            throw ExceptionPowerpc("invalid MD-Form extended opcode: " + StringUtility::addrToString(int(fld<27, 29>())), this);
    }
    ASSERT_not_reachable("opcode not handled");
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_MDS_formInstruction() {
    switch (fld<27, 30>()) {
        case 8: return MAKE_INSN4_RC(rldcl, RA(), RS(), RB(), MB_64bit());
        case 9: return MAKE_INSN4_RC(rldcr, RA(), RS(), RB(), ME_64bit());
        default:
            throw ExceptionPowerpc("invalid MDS-Form extended opcode: " + StringUtility::addrToString(int(fld<27, 30>())), this);
    }
    ASSERT_not_reachable("opcode not handled");
}

} // namespace
} // namespace
