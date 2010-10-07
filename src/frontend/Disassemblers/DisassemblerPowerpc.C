// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "Assembler.h"
#include "AssemblerX86.h"
#include "AsmUnparser_compat.h"
#include "Disassembler.h"
#include "sageBuilderAsm.h"
#include "DisassemblerPowerpc.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* See header file for full documentation. */

/* References:
 *
 * [1] "Programming Environments Manual for 32-Bit Implementations of the PowerPC Architecture", MPCFPE32B Rev. 3, 9/2005, by
 * Freescale Semiconductor (www.freescale.com). Freescale Semiconductor Technical Information Center, 1300 N. Alma School Road,
 * Chandler, AZ 85224. 1-800-521-6274.  A copy can be found on the web at
 * http://www-106.ibm.com/developerworks/linux/library/l-powarch/
 */

#define DEBUG_OPCODES 0
#define DEBUG_BRANCH_LOGIC 0

// These are macros to make them look like constants while they are really
// function calls
#define BYTET (SgAsmTypeByte::createType())
#define WORDT (SgAsmTypeWord::createType())
#define DWORDT (SgAsmTypeDoubleWord::createType())
#define QWORDT (SgAsmTypeQuadWord::createType())
#define FLOATT (SgAsmTypeSingleFloat::createType())
#define DOUBLET (SgAsmTypeDoubleFloat::createType())
#define V2DWORDT (SgAsmTypeVector::createType(2, DWORDT))
#define V2FLOATT (SgAsmTypeVector::createType(2, FLOATT))
#define V2DOUBLET (SgAsmTypeVector::createType(2, DOUBLET))

Disassembler::AddressSet
SgAsmPowerpcInstruction::get_successors(bool *complete) {
    Disassembler::AddressSet retval;
    *complete = true; /*assume retval is the complete set of successors for now*/

    switch (get_kind()) {
        case powerpc_bc:
        case powerpc_bca:
        case powerpc_bcl:
        case powerpc_bcla: {
            /* Conditional branches: bcX BO,BI,TARGET */
            const std::vector<SgAsmExpression*> &exprs = get_operandList()->get_operands();
            ROSE_ASSERT(exprs.size()==3);
            ROSE_ASSERT(isSgAsmValueExpression(exprs[2]));
            rose_addr_t target = SageInterface::getAsmConstant(isSgAsmValueExpression(exprs[2]));
            retval.insert(target);
            retval.insert(get_address()+get_raw_bytes().size());
            break;
        }

        case powerpc_bcctr:
        case powerpc_bcctrl:
        case powerpc_bclr:
        case powerpc_bclrl:
            /* Conditional branches to count register; target is unknown */
            *complete = false;
            retval.insert(get_address()+get_raw_bytes().size());
            break;

        case powerpc_b:
        case powerpc_ba:
        case powerpc_bl:
        case powerpc_bla: {
            /* Unconditional branches */
            const std::vector<SgAsmExpression*> &exprs = get_operandList()->get_operands();
            ROSE_ASSERT(exprs.size()==1);
            ROSE_ASSERT(isSgAsmValueExpression(exprs[0]));
            rose_addr_t target = SageInterface::getAsmConstant(isSgAsmValueExpression(exprs[0]));
            retval.insert(target);
            break;
        }

        case powerpc_unknown_instruction:
        case powerpc_tw:
        case powerpc_twi:
        case powerpc_rfi:
        case powerpc_sc:
            /* No known successors */
            *complete = false;
            break;

        default:
            /* All others fall through to next instruction */
            retval.insert(get_address()+get_raw_bytes().size());
            break;
    }
    return retval;
}

bool
SgAsmPowerpcInstruction::terminatesBasicBlock() {
    switch (get_kind()) {
        case powerpc_unknown_instruction:
        case powerpc_b:         /* branch instructions... */
        case powerpc_ba:
        case powerpc_bl:
        case powerpc_bla:
        case powerpc_bc:
        case powerpc_bca:
        case powerpc_bcl:
        case powerpc_bcla:
        case powerpc_bcctr:
        case powerpc_bcctrl:
        case powerpc_bclr:
        case powerpc_bclrl:
        case powerpc_tw:        /* trap instructions... */
        case powerpc_twi:
        case powerpc_sc:        /* system call */
        case powerpc_rfi:       /* return from interrupt */
            return true;

        default:
            return false;
    }
}

bool
DisassemblerPowerpc::can_disassemble(SgAsmGenericHeader *header) const
{
    SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return isa == SgAsmExecutableFileFormat::ISA_PowerPC;
}

void
DisassemblerPowerpc::init()
{
    set_wordsize(4);
    set_alignment(4);
    set_sex(SgAsmExecutableFileFormat::ORDER_LSB);
    set_registers(RegisterDictionary::powerpc());
}

/* This is a bit of a kludge for now because we're trying to use an unmodified version of the PowerpcDisassembler name space. */
SgAsmInstruction *
DisassemblerPowerpc::disassembleOne(const MemoryMap *map, rose_addr_t start_va, AddressSet *successors)
{
    /* The old PowerpcDisassembler::disassemble() function doesn't understand MemoryMap mappings. Therefore, remap the next
     * few bytes (enough for at least one instruction) into a temporary buffer. */
    unsigned char temp[4];
    size_t tempsz = map->read(temp, start_va, sizeof temp, get_protection());

    /* Treat the bytes as a big-endian instruction.  Note that PowerPC is big-endian, but PowerPC can support both big- and
     * little-endian processor modes (with much weirdness; e.g. PDP endian like propoerties). */
    if (tempsz<4)
        throw Exception("short read", start_va);
    uint32_t c = (temp[0]<<24) | (temp[1]<<16) | (temp[2]<<8) | temp[3];
    
    /* Disassemble the instruction */
    startInstruction(start_va, c);
    SgAsmPowerpcInstruction *insn = disassemble(); /*throws an exception on error*/
    ROSE_ASSERT(insn);

    /* Note successors if necessary */
    if (successors) {
        bool complete;
        AddressSet suc2 = insn->get_successors(&complete);
        successors->insert(suc2.begin(), suc2.end());
    }

    update_progress(insn);
    return insn;
}

SgAsmInstruction *
DisassemblerPowerpc::make_unknown_instruction(const Exception &e) 
{
    SgAsmPowerpcInstruction *insn = new SgAsmPowerpcInstruction(e.ip, "unknown", powerpc_unknown_instruction);
    SgAsmOperandList *operands = new SgAsmOperandList();
    insn->set_operandList(operands);
    operands->set_parent(insn);
    insn->set_raw_bytes(e.bytes);
    return insn;
}

template <size_t First, size_t Last>
uint32_t        
DisassemblerPowerpc::fld() const {
    return (insn >> (31 - Last)) & (IntegerOps::GenMask<uint32_t, Last - First + 1>::value);
}

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

SgAsmQuadWordValueExpression *
DisassemblerPowerpc::makeBranchTarget ( uint64_t targetAddr ) const
{
    return SageBuilderAsm::makeQWordValue(targetAddr);
}

SgAsmPowerpcInstruction *
DisassemblerPowerpc::makeInstructionWithoutOperands(uint64_t address, const std::string& mnemonic, PowerpcInstructionKind kind,
                                                    uint32_t insn)
{
    // Constructor: SgAsmPowerpcInstruction(rose_addr_t address = 0, std::string mnemonic = "", PowerpcInstructionKind kind = powerpc_unknown_instruction);
    SgAsmPowerpcInstruction* instruction = new SgAsmPowerpcInstruction(address, mnemonic, kind);
    ROSE_ASSERT (instruction);

    if (mnemonic.size() >= 7 && mnemonic.substr(mnemonic.size() - 7) == "_record") {
        instruction->set_mnemonic(mnemonic.substr(0, mnemonic.size() - 7) + ".");
    }

    SgAsmOperandList* operands = new SgAsmOperandList();
    instruction->set_operandList(operands);
    operands->set_parent(instruction);

    // PowerPC uses a fixed length instruction set (like ARM, but unlike x86)
    SgUnsignedCharList bytes(4, '\0');
    for (int i = 0; i < 4; ++i) {
        bytes[i] = (insn >> (24 - (8 * i))) & 0xFF; // Force big-endian
    }

    instruction->set_raw_bytes(bytes);
     return instruction;
}

/* At one time this function created PowerPC-specific register reference expressions (RREs) that had hard-coded values for
 * register class, register number, and register position.
 *
 * The new approach (added Oct 2010) replaces the PowerPC-specific numbers with a more generic RegisterDescriptor struct, where
 * each register is described by a major number (fomerly the PowerpcRegisterClass), a minor number (formerly the reg_number),
 * and a bit offset and size (usually zero and 32, but also implied by the PowerpcConditionRegisterAccessGranularity and
 * reg_number combination).  The new idea is that a RegisterDescriptor does not need to contain machine-specific
 * values. Therefore, we've added a level of indirection: makeRegister() converts machine specific values to a register name,
 * which is then looked up in a RegisterDictionary to return a RegisterDescriptor.  The entries in the dictionary determine
 * what registers are available to the disassembler.
 *
 * Currently (2010-10-09) the old class and numbers are used as the major and minor values (except in the case of the "cr"
 * register), but users should not assume that this is always the case. They can assume that unrelated registers (e.g., "r0"
 * and "r1") have descriptors that map to non-overlapping areas of the descriptor address space {major,minor,offset,size} while
 * related registers (e.g., "spr8" and "lr") map to overlapping areas of the descriptor address space. */
SgAsmPowerpcRegisterReferenceExpression*
DisassemblerPowerpc::makeRegister(PowerpcRegisterClass reg_class, int reg_number,
                                  PowerpcConditionRegisterAccessGranularity cr_granularity) const
{
    /* Obtain a register name */
    std::string name;
    switch (reg_class) {
        case powerpc_regclass_gpr:
            ROSE_ASSERT(reg_number>=0 && reg_number<1024);
            name = "r" + StringUtility::numberToString(reg_number);
            break;
        case powerpc_regclass_fpr:
            ROSE_ASSERT(reg_number>=0 && reg_number<1024);
            name = "f" + StringUtility::numberToString(reg_number);
            break;
        case powerpc_regclass_cr:
            name = "cr";
            switch (cr_granularity) {
                case powerpc_condreggranularity_whole:
                    ROSE_ASSERT(0==reg_number);
                    break;
                case powerpc_condreggranularity_field:
                    /* cr has eight 4-bit fields numbered 0..7 with names like "cr0" */
                    ROSE_ASSERT(reg_number>=0 && reg_number<8);
                    name += StringUtility::numberToString(reg_number);
                    break;
                case powerpc_condreggranularity_bit: {
                    /* each field has four bits with names. The full name of each bit is "crF*4+B" where "F" is the field
                     * number like above and "B" is the bit name. For instance, "cr0*4+eq". */
                    ROSE_ASSERT(reg_number>=0 && reg_number<32);
                    static const char *bitname[] = {"lt", "gt", "eq", "so"};
                    name += StringUtility::numberToString(reg_number/4) + "*4+" + bitname[reg_number%4];
                    break;
                }
            }
            break;
        case powerpc_regclass_fpscr:
            ROSE_ASSERT(0==reg_number);
            name = "fpscr";
            break;
        case powerpc_regclass_spr:
            /* Some special purpose registers have special names, but the dictionary has the generic name as well. */
            ROSE_ASSERT(reg_number>=0 && reg_number<1024);
            name = "spr" + StringUtility::numberToString(reg_number);
            break;
        case powerpc_regclass_tbr:
            /* Some time base registers have special names, but the dictionary has the generic name as well. */
            ROSE_ASSERT(reg_number>=0 && reg_number<1024);
            name = "tbr" + StringUtility::numberToString(reg_number);
            break;
        case powerpc_regclass_msr:
            ROSE_ASSERT(0==reg_number);
            name = "msr";
            break;
        case powerpc_regclass_sr:
            /* FIXME: not implemented yet [RPM 2010-10-09] */
            name = "sr" + StringUtility::numberToString(reg_number);
            break;
        case powerpc_regclass_iar:
            /* Instruction address register is not a real register, and so should never appear in disassembled code. */
            ROSE_ASSERT(!"iar is not a real register");
        case powerpc_regclass_pvr:
            ROSE_ASSERT(0==reg_number);
            name = "pvr";
            break;
        case powerpc_regclass_unknown:
        case powerpc_last_register_class:
            ROSE_ASSERT(!"not a real register");
        /* Don't add a "default" or else we won't get compiler warnings if a new class is defined. */
    }
    ROSE_ASSERT(!name.empty());

    /* Obtain a register descriptor from the dictionary */
    ROSE_ASSERT(get_registers()!=NULL);
    const RegisterDescriptor *rdesc = get_registers()->lookup(name);
    if (!rdesc)
        throw Exception("register \"" + name + "\" is not available for " + get_registers()->get_architecture_name());
    
    /* Construct the return value */
    SgAsmPowerpcRegisterReferenceExpression *rre = new SgAsmPowerpcRegisterReferenceExpression(*rdesc);
    ROSE_ASSERT(rre);
    return rre;
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::disassemble()
{
  // The Primary Opcode Field is bits 0-5, 6-bits wide, so there are max 64 primary opcode values
     uint8_t primaryOpcode = (insn >> 26) & 0x3F;

  // printf ("instruction opcode = 0x%08"PRIx32" primaryOpcode = 0x%"PRIx8" \n", insn, primaryOpcode);

     SgAsmPowerpcInstruction* instruction = NULL;

   // Handle all the different legal Primary Opcode values
     switch (primaryOpcode)
        {
       // 0 : These are the BGL specific PowerPC440 FP2 Architecture instructions
           case 0x00: // { instruction = decode_A_formInstruction(); break; }
             {
            // Depending on if this is A form or X form, the extended opCode maps to different bit ranges, so this code is incorrect!
            // uint16_t x_Opcode   = (insn >> 1) & 0x3FF;
               uint8_t  a_Opcode   = (insn >> 1) & 0x1F;

            // printf ("x_Opcode = 0x%x = %d xfl_Opcode = 0x%x = %d a_Opcode = 0x%x = %d \n", x_Opcode, x_Opcode, xfl_Opcode, xfl_Opcode, a_Opcode, a_Opcode);

            // Different parts of the instruction are used to identify what kind of instruction this is!
               if (  a_Opcode == 5 || (a_Opcode >= 8 && a_Opcode <= 31) )
                  {
                    instruction = decode_A_formInstruction_00();
                  }
                 else
                  {
                    instruction = decode_X_formInstruction_00();
                  }

               ROSE_ASSERT(instruction != NULL);
               break;
             }

          case 0x01: throw ExceptionPowerpc("invalid primary opcode (0x01)", this, 26); break;
          case 0x02: instruction = MAKE_INSN3(tdi, TO(), RA(), SI()); break;
          case 0x03: instruction = MAKE_INSN3(twi, TO(), RA(), SI()); break;

       // 4 : These are the BGL specific PowerPC440 FP2 Architecture instructions
          case 0x04: { instruction = decode_A_formInstruction_04(); break; }

          case 0x05: throw ExceptionPowerpc("invalid primary opcode (0x05)", this, 26); break;
          case 0x06: throw ExceptionPowerpc("invalid primary opcode (0x06)", this, 26); break;
          case 0x07: instruction = MAKE_INSN3(mulli, RT(), RA(), SI()); break;
          case 0x08: instruction = MAKE_INSN3(subfic, RT(), RA(), SI()); break;
          case 0x09: throw ExceptionPowerpc("invalid primary opcode (0x09)", this, 26); break;
          case 0x0A: instruction = MAKE_INSN4(cmpli, BF_cr(), L_10(), RA(), UI()); break;
          case 0x0B: instruction = MAKE_INSN4(cmpi, BF_cr(), L_10(), RA(), SI()); break;
          case 0x0C: instruction = MAKE_INSN3(addic, RT(), RA(), SI()); break;
          case 0x0D: instruction = MAKE_INSN3(addic_record, RT(), RA(), SI()); break;
          case 0x0E: instruction = MAKE_INSN3(addi, RT(), RA_or_zero(), SI()); break;
          case 0x0F: instruction = MAKE_INSN3(addis, RT(), RA_or_zero(), SI()); break;

       // Branch instruction
          case 0x10: { instruction = decode_B_formInstruction(); break; }

          case 0x11: { instruction = decode_SC_formInstruction(); break; }

       // Conditional branch
          case 0x12: { instruction = decode_I_formInstruction(); break; }

          case 0x13: { instruction = decode_XL_formInstruction(); break; }

          case 0x14: instruction = MAKE_INSN5_RC(rlwimi, RA(), RS(), SH_32bit(), MB_32bit(), ME_32bit()); break;
          case 0x15: instruction = MAKE_INSN5_RC(rlwinm, RA(), RS(), SH_32bit(), MB_32bit(), ME_32bit()); break;
          case 0x17: instruction = MAKE_INSN5_RC(rlwnm, RA(), RS(), RB(), MB_32bit(), ME_32bit()); break;
          case 0x18: instruction = MAKE_INSN3(ori, RS(), RA(), UI()); break;
          case 0x19: instruction = MAKE_INSN3(oris, RS(), RA(), UI()); break;
          case 0x1A: instruction = MAKE_INSN3(xori, RS(), RA(), UI()); break;
          case 0x1B: instruction = MAKE_INSN3(xoris, RS(), RA(), UI()); break;
          case 0x1C: instruction = MAKE_INSN3(andi_record, RS(), RA(), UI()); break;
          case 0x1D: instruction = MAKE_INSN3(andis_record, RS(), RA(), UI()); break;
       // 30
          case 0x1E: { instruction = decode_MD_formInstruction(); break; }

       // 31: includes X form, XO form, XFX form, and XS form
          case 0x1F: { instruction = decode_X_formInstruction_1F(); break; }

          case 0x20: instruction = MAKE_INSN2(lwz, RT(), memref(DWORDT)); break;
          case 0x21: instruction = MAKE_INSN2(lwzu, RT(), memrefu(DWORDT)); break;
          case 0x22: instruction = MAKE_INSN2(lbz, RT(), memref(BYTET)); break;
          case 0x23: instruction = MAKE_INSN2(lbzu, RT(), memrefu(BYTET)); break;
          case 0x24: instruction = MAKE_INSN2(stw, RS(), memref(DWORDT)); break;
          case 0x25: instruction = MAKE_INSN2(stwu, RS(), memrefu(DWORDT)); break;
          case 0x26: instruction = MAKE_INSN2(stb, RS(), memref(BYTET)); break;
          case 0x27: instruction = MAKE_INSN2(stbu, RS(), memrefu(BYTET)); break;
          case 0x28: instruction = MAKE_INSN2(lhz, RT(), memref(WORDT)); break;
          case 0x29: instruction = MAKE_INSN2(lhzu, RT(), memrefu(WORDT)); break;
          case 0x2A: instruction = MAKE_INSN2(lha, RT(), memref(WORDT)); break;
          case 0x2B: instruction = MAKE_INSN2(lhau, RT(), memrefu(WORDT)); break;
          case 0x2C: instruction = MAKE_INSN2(sth, RS(), memref(WORDT)); break;
          case 0x2D: instruction = MAKE_INSN2(sthu, RS(), memrefu(WORDT)); break;
          case 0x2E: instruction = MAKE_INSN2(lmw, RT(), memref(DWORDT)); break;
          case 0x2F: instruction = MAKE_INSN2(stmw, RS(), memref(DWORDT)); break;
          case 0x30: instruction = MAKE_INSN2(lfs, FRT(), memref(FLOATT)); break;
          case 0x31: instruction = MAKE_INSN2(lfsu, FRT(), memrefu(FLOATT)); break;
          case 0x32: instruction = MAKE_INSN2(lfd, FRT(), memref(DOUBLET)); break;
          case 0x33: instruction = MAKE_INSN2(lfdu, FRT(), memrefu(DOUBLET)); break;
          case 0x34: instruction = MAKE_INSN2(stfs, FRS(), memref(FLOATT)); break;
          case 0x35: instruction = MAKE_INSN2(stfsu, FRS(), memrefu(FLOATT)); break;
          case 0x36: instruction = MAKE_INSN2(stfd, FRS(), memref(DOUBLET)); break;
          case 0x37: instruction = MAKE_INSN2(stfdu, FRS(), memrefu(DOUBLET)); break;
       // 56
          case 0x38: throw ExceptionPowerpc("invalid primary opcode (0x38)", this, 26); break;
          case 0x39: throw ExceptionPowerpc("invalid primary opcode (0x39)", this, 26); break;

       // 58
          case 0x3A: { instruction = decode_DS_formInstruction(); break; }

       // 59
          case 0x3B: { instruction = decode_A_formInstruction_3B(); break; }

       // 60, 61
          case 0x3C: throw ExceptionPowerpc("invalid primary opcode (0x3c)", this, 26); break;
          case 0x3D: throw ExceptionPowerpc("invalid primary opcode (0x3d)", this, 26); break;

       // 62
          case 0x3E: { instruction = decode_DS_formInstruction(); break; }

       // 63: includes A form, X form, and XFL form instructions
          case 0x3F:
             {
            // Depending on if this is A form or X form, the extended opCode maps to different bit ranges, so this code is incorrect!
               uint8_t  a_Opcode   = (insn >> 1) & 0x1F;

            // printf ("x_Opcode = 0x%x = %d xfl_Opcode = 0x%x = %d a_Opcode = 0x%x = %d \n", x_Opcode, x_Opcode, xfl_Opcode, xfl_Opcode, a_Opcode, a_Opcode);

            // Different parts of the instruction are used to identify what kind of instruction this is!
               if (  a_Opcode == 18 || (a_Opcode >= 20 && a_Opcode <= 31) )
                  {
                    instruction = decode_A_formInstruction_3F();
                  }
                 else
                  {
                    instruction = decode_X_formInstruction_3F(); // Also includes XFL form
                  }

               ROSE_ASSERT(instruction != NULL);
               break;
             }

          default:
             {
            // The default case is now used for handling illegal instructions 
            // (during development it was for those not yet implemented).
               printf ("Primary opcode not handled yet: primaryOpcode = %d \n", primaryOpcode);
               throw ExceptionPowerpc("illegal primary opcode", this, 26);
             }
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
}
   
SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_I_formInstruction()
{
     SgAsmPowerpcInstruction* instruction = NULL;

     uint8_t primaryOpcode = (insn >> 26) & 0x3F;
     switch(primaryOpcode)
        {
          case 0x12:
             {
               uint32_t targetBranchAddress = LI();
               if (AA() == 0)
                  {
                 // In this case the address is the sum of LI || 0b00 sign-extended AND "the address of this instruction".
                    targetBranchAddress += ip;
                  }

               SgAsmQuadWordValueExpression* targetAddressExpression = makeBranchTarget(targetBranchAddress);

               if (AA() == 0)
                  {
                    if (LK() == 0)
                       {
                         instruction = MAKE_INSN1(b, targetAddressExpression);
                       }
                      else
                       {
                         instruction = MAKE_INSN1(bl, targetAddressExpression);
                       }
                  }
                 else
                  {
                    if (LK() == 0)
                       {
                         instruction = MAKE_INSN1(ba, targetAddressExpression);
                       }
                      else
                       {
                         instruction = MAKE_INSN1(bla, targetAddressExpression);
                       }
                  }

#if DEBUG_BRANCH_LOGIC
               printf ("Computation of targetAddressExpression = 0x%x \n", targetAddressExpression->get_value());
#endif
               break;
             }

          default:
             {
            // The default case is now used for handling illegal instructions 
            // (during development it was for those not yet implemented).
               printf ("Error: I-Form primaryOpcode = %d (illegal instruction) \n", primaryOpcode);
               ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_B_formInstruction()
{
     SgAsmPowerpcInstruction* instruction = NULL;

     uint8_t primaryOpcode = (insn >> 26) & 0x3F;
#if DEBUG_OPCODES
     printf ("B-Form instruction opcode = 0x%x boOpcode = 0x%x biOpcode = 0x%x bdOpcode = 0x%x aaOpcode = 0x%x lkOpcode = 0x%x \n", insn, boOpcode, biOpcode, bdOpcode, aaOpcode, lkOpcode);
#endif

     switch(primaryOpcode)
        {
          case 0x10:
             {
            // DQ (10/15/2008): Compute the address of the branch to restart the disassembly.
            // The address is BD || 0b00 sign-extended.
               uint32_t targetBranchAddress = BD();
               if (AA() == 0)
                  {
                 // In this case the address is the sum of BD || 0b00 sign-extended AND "the address of this instruction".
                    targetBranchAddress += ip;
                  }

               SgAsmQuadWordValueExpression* targetAddressExpression = makeBranchTarget(targetBranchAddress);

               if (LK() == 0)
                  {
                    if (AA() == 0)
                       {
                         instruction = MAKE_INSN3(bc, BO(), BI(), targetAddressExpression);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(bca, BO(), BI(), targetAddressExpression);
                       }
                  }
                 else
                  {
                    if (AA() == 0)
                       {
                         instruction = MAKE_INSN3(bcl, BO(), BI(), targetAddressExpression);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(bcla, BO(), BI(), targetAddressExpression);
                       }
                  }

               break;
             }

          default:
             {
            // The default case is now used for handling illegal instructions 
            // (during development it was for those not yet implemented).
               printf ("Error: B-Form primaryOpcode = %d not handled! \n", primaryOpcode);
               ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_SC_formInstruction()
{
     SgAsmPowerpcInstruction* instruction = NULL;

  // The primaryOpcode 
     uint8_t primaryOpcode = (insn >> 26) & 0x3F;
     ROSE_ASSERT(primaryOpcode == 0x11);

  // Get bit 30, 1 bit as the reserved flag
     uint8_t constantOneOpcode = (insn >> 1) & 0x1;
     ROSE_ASSERT(constantOneOpcode == 1);

#if DEBUG_OPCODES
     printf ("SC-Form instruction opcode = 0x%x levOpcode = 0x%x constantOneOpcode = 0x%x \n", insn, levOpcode, constantOneOpcode);
#endif

     instruction = MAKE_INSN1(sc, LEV());

     ROSE_ASSERT(instruction != NULL);
     return instruction;
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_DS_formInstruction()
{
     SgAsmPowerpcInstruction* instruction = NULL;

     printf ("DS-Form instructions not implemented yet, I have no examples so far! \n");

     ROSE_ASSERT(instruction != NULL);
     return instruction;
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_X_formInstruction_1F()
{
     SgAsmPowerpcInstruction* instruction = NULL;

  // The primaryOpcode 
     uint8_t primaryOpcode = (insn >> 26) & 0x3F;
     ROSE_ASSERT (primaryOpcode == 0x1F);

  // Get the bits 21-30, next 10 bits
     uint16_t xoOpcode = (insn >> 1) & 0x3FF;

#if DEBUG_OPCODES
     printf ("X-Form instruction 1F opcode = 0x%x primaryOpcode = 0x%x xoOpcode = 0x%x \n", insn, primaryOpcode, xoOpcode);
#endif

     switch(xoOpcode)
        {
          case 0x000: instruction = MAKE_INSN4(cmp, BF_cr(), L_10(), RA(), RB()); break;
          case 0x004: instruction = MAKE_INSN3(tw, TO(), RA(), RB()); break;
          case 0x008: instruction = MAKE_INSN3_RC(subfc, RT(), RA(), RB()); break;
          case 0x009: instruction = MAKE_INSN3_RC(mulhdu, RT(), RA(), RB()); break;
          case 0x00A: instruction = MAKE_INSN3_RC(addc, RT(), RA(), RB()); break;
          case 0x00B: instruction = MAKE_INSN3_RC(mulhwu, RT(), RA(), RB()); break;
          case 0x014: instruction = MAKE_INSN2(lwarx, RT(), memrefx(DWORDT)); break;
          case 0x015: instruction = MAKE_INSN2(ldx, RT(), memrefx(QWORDT)); break;
          case 0x017: instruction = MAKE_INSN2(lwzx, RT(), memrefx(DWORDT)); break;
          case 0x018: instruction = MAKE_INSN3_RC(slw, RA(), RS(), RB()); break;
          case 0x01A: instruction = MAKE_INSN2_RC(cntlzw, RA(), RS()); break;
          case 0x01B: instruction = MAKE_INSN3_RC(sld, RA(), RS(), RB()); break;
          case 0x01C: instruction = MAKE_INSN3_RC(and, RA(), RS(), RB()); break;
          case 0x020: instruction = MAKE_INSN4(cmpl, BF_cr(), L_10(), RA(), RB()); break;
          case 0x028: instruction = MAKE_INSN3_RC(subf, RT(), RA(), RB()); break;
          case 0x035: instruction = MAKE_INSN2(ldux, RT(), memrefux(QWORDT)); break;
          case 0x037: instruction = MAKE_INSN2(lwzux, RT(), memrefux(DWORDT)); break;
          case 0x03C: instruction = MAKE_INSN3_RC(andc, RA(), RS(), RB()); break;
          case 0x044: instruction = MAKE_INSN3(td, TO(), RA(), RB()); break;
          case 0x049: instruction = MAKE_INSN3_RC(mulhd, RT(), RA(), RB()); break;
          case 0x04B: instruction = MAKE_INSN3_RC(mulhw, RT(), RA(), RB()); break;
          case 0x053: instruction = MAKE_INSN1(mfmsr, RT()); break;
          case 0x057: instruction = MAKE_INSN2(lbzx, RT(), memrefx(BYTET)); break;
          case 0x068: instruction = MAKE_INSN2_RC(neg, RT(), RA()); break;
          case 0x077: instruction = MAKE_INSN2(lbzux, RT(), memrefux(BYTET)); break;
          case 0x07C: instruction = MAKE_INSN3_RC(nor, RA(), RS(), RB()); break;
          case 0x088: instruction = MAKE_INSN3_RC(adde, RT(), RA(), RB()); break;
          case 0x08A: instruction = MAKE_INSN3_RC(subfe, RT(), RA(), RB()); break;
          case 0x08E: instruction = MAKE_INSN2(lfssx, FRT(), memrefx(FLOATT)); break;
          case 0x095: instruction = MAKE_INSN2(stdx, RS(), memrefx(QWORDT)); break;
          case 0x096: instruction = MAKE_INSN2(stwcx_record, RS(), memrefx(DWORDT)); break;
          case 0x097: instruction = MAKE_INSN2(stwx, RS(), memrefx(DWORDT)); break;
          case 0x0AE: instruction = MAKE_INSN2(lfssux, FRT(), memrefux(FLOATT)); break;
          case 0x0B5: instruction = MAKE_INSN2(stdux, RS(), memrefux(QWORDT)); break;
          case 0x0B7: instruction = MAKE_INSN2(stwux, RS(), memrefux(DWORDT)); break;
          case 0x0C8: instruction = MAKE_INSN2_RC(subfze, RT(), RA()); break;
          case 0x0CA: instruction = MAKE_INSN2_RC(addze, RT(), RA()); break;
          case 0x0CE: instruction = MAKE_INSN2(lfsdx, FRT(), memrefx(DOUBLET)); break;
          case 0x0D6: instruction = MAKE_INSN2(stdcx_record, RS(), memrefx(QWORDT)); break;
          case 0x0D7: instruction = MAKE_INSN2(stbx, RS(), memrefx(BYTET)); break;
          case 0x0E8: instruction = MAKE_INSN2_RC(subfme, RT(), RA()); break;
          case 0x0EA: instruction = MAKE_INSN2_RC(addme, RT(), RA()); break;
          case 0x0EB: instruction = MAKE_INSN3_RC(mullw, RT(), RA(), RB()); break;
          case 0x0EE: instruction = MAKE_INSN2(lfsdux, FRT(), memrefux(DOUBLET)); break;
          case 0x0F7: instruction = MAKE_INSN2(stbux, RS(), memrefux(BYTET)); break;
          case 0x10A: instruction = MAKE_INSN3_RC(add, RT(), RA(), RB()); break;
          case 0x10E: instruction = MAKE_INSN2(lfxsx, FRT(), memrefx(V2FLOATT)); break;
          case 0x116: instruction = MAKE_INSN1(dcbt, memrefx(BYTET)); break;
          case 0x117: instruction = MAKE_INSN2(lhzx, RT(), memrefx(WORDT)); break;
          case 0x11C: instruction = MAKE_INSN3_RC(eqv, RA(), RS(), RB()); break;
          case 0x12E: instruction = MAKE_INSN2(lfxsux, FRT(), memrefux(V2FLOATT)); break;
          case 0x137: instruction = MAKE_INSN2(lhzux, RT(), memrefux(WORDT)); break;
          case 0x13C: instruction = MAKE_INSN3_RC(xor, RA(), RS(), RB()); break;
          case 0x14E: instruction = MAKE_INSN2(lfxdx, FRT(), memrefx(V2DOUBLET)); break;
          case 0x153: instruction = MAKE_INSN2(mfspr, RT(), SPR()); break;
          case 0x155: instruction = MAKE_INSN2(lwax, RT(), memrefx(DWORDT)); break;
          case 0x157: instruction = MAKE_INSN2(lhax, RT(), memrefx(WORDT)); break;
          case 0x16E: instruction = MAKE_INSN2(lfxdux, FRT(), memrefux(V2DOUBLET)); break;
          case 0x175: instruction = MAKE_INSN2(lwaux, RT(), memrefux(DWORDT)); break;
          case 0x177: instruction = MAKE_INSN2(lhaux, RT(), memrefux(WORDT)); break;
          case 0x18E: instruction = MAKE_INSN2(lfpsx, FRT(), memrefx(V2FLOATT)); break;
          case 0x197: instruction = MAKE_INSN2(sthx, RS(), memrefx(WORDT)); break;
          case 0x19C: instruction = MAKE_INSN3_RC(orc, RA(), RS(), RB()); break;
          case 0x1AE: instruction = MAKE_INSN2(lfpsux, FRT(), memrefux(V2FLOATT)); break;
          case 0x1B7: instruction = MAKE_INSN2(sthux, RS(), memrefux(WORDT)); break;
          case 0x1BC: instruction = MAKE_INSN3_RC(or, RA(), RS(), RB()); break;
          case 0x1C9: instruction = MAKE_INSN3_RC(divdu, RT(), RA(), RB()); break;
          case 0x1CB: instruction = MAKE_INSN3_RC(divwu, RT(), RA(), RB()); break;
          case 0x1CE: instruction = MAKE_INSN2(lfpdx, FRT(), memrefx(V2DOUBLET)); break;
          case 0x1D3: instruction = MAKE_INSN2(mtspr, RS(), SPR()); break;
          case 0x1DC: instruction = MAKE_INSN3_RC(nand, RA(), RS(), RB()); break;
          case 0x1E9: instruction = MAKE_INSN3_RC(divd, RT(), RA(), RB()); break;
          case 0x1EB: instruction = MAKE_INSN3_RC(divw, RT(), RA(), RB()); break;
          case 0x1EE: instruction = MAKE_INSN2(lfpdux, FRT(), memrefux(V2DOUBLET)); break;
          case 0x208: instruction = MAKE_INSN3_RC(subfco, RT(), RA(), RB()); break;
          case 0x20A: instruction = MAKE_INSN3_RC(addco, RT(), RA(), RB()); break;
          case 0x20E: instruction = MAKE_INSN2(stfpiwx, FRT(), memrefx(V2DWORDT)); break;
          case 0x215: instruction = MAKE_INSN2(lswx, RT(), memrefx(DWORDT)); break;
          case 0x216: instruction = MAKE_INSN2(lwbrx, RT(), memrefx(DWORDT)); break;
          case 0x217: instruction = MAKE_INSN2(lfsx, FRT(), memrefx(FLOATT)); break;
          case 0x218: instruction = MAKE_INSN3_RC(srw, RA(), RS(), RB()); break;
          case 0x21B: instruction = MAKE_INSN3_RC(srd, RA(), RS(), RB()); break;
          case 0x228: instruction = MAKE_INSN3_RC(subfo, RT(), RA(), RB()); break;
          case 0x237: instruction = MAKE_INSN2(lfsux, FRT(), memrefux(FLOATT)); break;
          case 0x255: instruction = MAKE_INSN2(lswx, RT(), memrefx(DWORDT)); break;
          case 0x257: instruction = MAKE_INSN2(lfdx, FRT(), memrefx(DOUBLET)); break;
          case 0x268: instruction = MAKE_INSN2_RC(nego, RT(), RA()); break;
          case 0x288: instruction = MAKE_INSN3_RC(addeo, RT(), RA(), RB()); break;
          case 0x28A: instruction = MAKE_INSN3_RC(subfeo, RT(), RA(), RB()); break;
          case 0x28E: instruction = MAKE_INSN2(stfssx, FRT(), memrefx(FLOATT)); break;
          case 0x295: instruction = MAKE_INSN2(stswx, RS(), memrefx(DWORDT)); break;
          case 0x296: instruction = MAKE_INSN2(stwbrx, RS(), memrefx(DWORDT)); break;
          case 0x2AE: instruction = MAKE_INSN2(stfssux, FRT(), memrefux(FLOATT)); break;
          case 0x2C8: instruction = MAKE_INSN2_RC(subfzeo, RT(), RA()); break;
          case 0x2CA: instruction = MAKE_INSN2_RC(addzeo, RT(), RA()); break;
          case 0x2CE: instruction = MAKE_INSN2(stfsdx, FRT(), memrefx(DOUBLET)); break;
          case 0x2D5: instruction = MAKE_INSN2(stswi, RS(), memrefx(DWORDT)); break;
          case 0x2D7: instruction = MAKE_INSN2(stfdx, FRS(), memrefx(DOUBLET)); break;
          case 0x2E8: instruction = MAKE_INSN2_RC(subfmeo, RT(), RA()); break;
          case 0x2EA: instruction = MAKE_INSN2_RC(addmeo, RT(), RA()); break;
          case 0x2EB: instruction = MAKE_INSN3_RC(mullwo, RT(), RA(), RB()); break;
          case 0x2EE: instruction = MAKE_INSN2(stfsdux, FRT(), memrefux(DOUBLET)); break;
          case 0x2F7: instruction = MAKE_INSN2(stfdux, FRS(), memrefx(DOUBLET)); break;
          case 0x30A: instruction = MAKE_INSN3_RC(addo, RT(), RA(), RB()); break;
          case 0x30E: instruction = MAKE_INSN2(stfxsx, FRT(), memrefx(V2FLOATT)); break;
          case 0x316: instruction = MAKE_INSN2(lhbrx, RT(), memrefx(WORDT)); break;
          case 0x318: instruction = MAKE_INSN3_RC(sraw, RA(), RS(), RB()); break;
          case 0x31A: instruction = MAKE_INSN3_RC(srad, RA(), RS(), RB()); break;
          case 0x32E: instruction = MAKE_INSN2(stfxsux, FRT(), memrefux(V2FLOATT)); break;
          case 0x338: instruction = MAKE_INSN3_RC(srawi, RA(), RS(), SH_32bit()); break;
          case 0x33A: instruction = MAKE_INSN3_RC(sradi, RA(), RS(), SH_64bit()); break; // The last bit of the ext. opcode is part of SH
          case 0x33B: instruction = MAKE_INSN3_RC(sradi, RA(), RS(), SH_64bit()); break; // Same as previous insn
          case 0x34E: instruction = MAKE_INSN2(stfxdx, FRT(), memrefx(V2DOUBLET)); break;
          case 0x356: instruction = MAKE_INSN0(eieio); break;
          case 0x36E: instruction = MAKE_INSN2(stfxdux, FRT(), memrefux(V2DOUBLET)); break;
          case 0x38E: instruction = MAKE_INSN2(stfpsx, FRT(), memrefx(V2FLOATT)); break;
          case 0x396: instruction = MAKE_INSN2(sthbrx, RS(), memrefx(WORDT)); break;
          case 0x39A: instruction = MAKE_INSN2_RC(extsh, RA(), RS()); break;
          case 0x3AE: instruction = MAKE_INSN2(stfpsux, FRT(), memrefux(V2FLOATT)); break;
          case 0x3BA: instruction = MAKE_INSN2_RC(extsb, RA(), RS()); break;
          case 0x3C9: instruction = MAKE_INSN3_RC(divduo, RT(), RA(), RB()); break;
          case 0x3CB: instruction = MAKE_INSN3_RC(divwuo, RT(), RA(), RB()); break;
          case 0x3CE: instruction = MAKE_INSN2(stfpdx, FRT(), memrefx(V2DOUBLET)); break;
          case 0x3D7: instruction = MAKE_INSN2(stfiwx, FRS(), memrefx(DWORDT)); break;
          case 0x3DA: instruction = MAKE_INSN2_RC(extsw, RA(), RS()); break;
          case 0x3E9: instruction = MAKE_INSN3_RC(divdo, RT(), RA(), RB()); break;
          case 0x3EB: instruction = MAKE_INSN3_RC(divwo, RT(), RA(), RB()); break;
          case 0x3EE: instruction = MAKE_INSN2(stfpdux, FRT(), memrefux(V2DOUBLET)); break;
          case 0x3F6: instruction = MAKE_INSN1(dcbz, memrefx(BYTET)); break;

        // FIXME: merge these into list
          case 0x13:
             {
               if (fld<11, 11>() == 0)
                  {
                    instruction = MAKE_INSN1(mfcr, RT());
                  }
                 else
                  {
                 // instruction = MAKE_INSN1(mfocr, RT());
                    instruction = MAKE_INSN1(mfcr, RT());

                 // See note on page 124 of User Instruction Set Architecture version 2.02
                    printf ("This mfocr instruction is an old form of the mfcr instruction ... \n");
                    ROSE_ASSERT(false);
                  }
               break;
             }

          case 0x90:
             {
               if (fld<11, 11>() == 0)
                  {
                    instruction = MAKE_INSN2(mtcrf, FXM(), RS());
                  }
                 else
                  {
                 // instruction = MAKE_INSN2(mtocrf, FXM(), RS());
                    instruction = MAKE_INSN2(mtcrf, FXM(), RS());

                 // See note on page 124 of User Instruction Set Architecture version 2.02
                    printf ("This mtocrf instruction is an old form of the mtcrf instruction ... \n");
                    ROSE_ASSERT(false);
                  }
               break;
             }

          default:
             {
            // The default case is now used for handling illegal instructions 
            // (during development it was for those not yet implemented).
               printf ("Error: X-Form 1F xoOpcode = %d not handled! \n", xoOpcode);
               throw ExceptionPowerpc("X-Form 1F xoOpcode not handled", this, 1);
             }
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_X_formInstruction_3F()
{
     SgAsmPowerpcInstruction* instruction = NULL;

  // The primaryOpcode 
     uint8_t primaryOpcode = (insn >> 26) & 0x3F;
     ROSE_ASSERT (primaryOpcode == 0x3F);

  // Get the bits 21-30, next 10 bits
     uint16_t xoOpcode = (insn >> 1) & 0x3FF;

#if DEBUG_OPCODES
     printf ("X-Form instruction opcode = 0x%x primaryOpcode = 0x%x xoOpcode = 0x%x \n", insn, primaryOpcode, xoOpcode);
#endif

     switch(xoOpcode)
        {
          case 0x0: instruction = MAKE_INSN3(fcmpu, BF_cr(), FRA(), FRB()); break;
          case 0xC: instruction = MAKE_INSN2_RC(frsp, FRT(), FRB()); break;
          case 0xE: instruction = MAKE_INSN2_RC(fctiw, FRT(), FRB()); break;
          case 0xF: instruction = MAKE_INSN2_RC(fctiwz, FRT(), FRB()); break;
          case 0x26: instruction = MAKE_INSN1(mtfsb1, BT()); break;
          case 0x28: instruction = MAKE_INSN2_RC(fneg, FRT(), FRB()); break;
          case 0x46: instruction = MAKE_INSN1_RC(mtfsb0, BT()); break;
          case 0x48: instruction = MAKE_INSN2_RC(fmr, FRT(), FRB()); break;
          case 0x86: instruction = MAKE_INSN2_RC(mtfsfi, BF_fpscr(), U()); break;
          case 0x88: instruction = MAKE_INSN2_RC(fnabs, FRT(), FRB()); break;
          case 0x108: instruction = MAKE_INSN2_RC(fabs, FRT(), FRB()); break;
          case 0x247: instruction = MAKE_INSN1_RC(mffs, FRT()); break;
          case 0x2C7: instruction = MAKE_INSN2_RC(mtfsf, FLM(), FRB()); break;

          default:
             {
            // The default case is now used for handling illegal instructions 
            // (during development it was for those not yet implemented).
               printf ("Error: X-Form 3F xoOpcode = %d not handled! \n", xoOpcode);
               throw ExceptionPowerpc("X-Form 3F xoOpcode not handled", this, 1);
             }
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_X_formInstruction_00()
{
     SgAsmPowerpcInstruction* instruction = NULL;

     if (insn == 0)
         throw ExceptionPowerpc("zero instruction", this, 0);
         
  // Get the bits 21-30, next 10 bits
     uint16_t xoOpcode = (insn >> 1) & 0x3FF;

     switch(xoOpcode)
        {
          case 0x020: instruction = MAKE_INSN2(fpmr, FRT(), FRB()); break;
          case 0x060: instruction = MAKE_INSN2(fpabs, FRT(), FRB()); break;
          case 0x0A0: instruction = MAKE_INSN2(fpneg, FRT(), FRB()); break;
          case 0x0C0: instruction = MAKE_INSN2(fprsp, FRT(), FRB()); break;
          case 0x0E0: instruction = MAKE_INSN2(fpnabs, FRT(), FRB()); break;
          case 0x120: instruction = MAKE_INSN2(fsmr, FRT(), FRB()); break;
          case 0x160: instruction = MAKE_INSN2(fsabs, FRT(), FRB()); break;
          case 0x1A0: instruction = MAKE_INSN2(fsneg, FRT(), FRB()); break;
          case 0x1E0: instruction = MAKE_INSN2(fsnabs, FRT(), FRB()); break;
          case 0x220: instruction = MAKE_INSN2(fxmr, FRT(), FRB()); break;
          case 0x240: instruction = MAKE_INSN2(fpctiw, FRT(), FRB()); break;
          case 0x2C0: instruction = MAKE_INSN2(fpctiwz, FRT(), FRB()); break;
          case 0x320: instruction = MAKE_INSN2(fsmtp, FRT(), FRB()); break;
          case 0x3A0: instruction = MAKE_INSN2(fsmfp, FRT(), FRB()); break;
          default:
             {
            // The default case is now used for handling illegal instructions 
            // (during development it was for those not yet implemented).
               printf ("Error: X-Form 00 xoOpcode = %d not handled! \n", xoOpcode);
               throw ExceptionPowerpc("X-Form 00 xoOpcode not handled", this, 1);
             }
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_XL_formInstruction()
{
     SgAsmPowerpcInstruction* instruction = NULL;

  // The primaryOpcode 
     uint8_t primaryOpcode = (insn >> 26) & 0x3F;
     ROSE_ASSERT(primaryOpcode == 0x13);

  // Get the bits 21-30, next 10 bits
     uint16_t xoOpcode = (insn >> 1) & 0x3FF;

#if DEBUG_OPCODES
     printf ("XL-Form instruction opcode = 0x%x xoOpcode = 0x%x = %d btOpcode = 0x%x = %d baOpcode = 0x%x = %d bbOpcode = 0x%x = %d bhOpcode = 0x%x = %d lkOpcode = 0x%x \n",
          insn, xoOpcode, xoOpcode, btOpcode, btOpcode, baOpcode, baOpcode, bbOpcode, bbOpcode, bhOpcode, bhOpcode, lkOpcode);
#endif

     switch(xoOpcode)
        {
          case 0x000: instruction = MAKE_INSN2(mcrf, BF_cr(), BFA_cr()); break;
          case 0x010:
             {
               if (LK() == 0)
                  {
                    instruction = MAKE_INSN3(bclr, BO(), BI(), BH());
                  }
                 else
                  {
                    instruction = MAKE_INSN3(bclrl, BO(), BI(), BH());
                  }
               break;
             }
          case 0x021: instruction = MAKE_INSN3(crnor, BT(), BA(), BB()); break;
          case 0x081: instruction = MAKE_INSN3(crandc, BT(), BA(), BB()); break;
          case 0x0C1: instruction = MAKE_INSN3(crxor, BT(), BA(), BB()); break;
          case 0x0E1: instruction = MAKE_INSN3(crnand, BT(), BA(), BB()); break;
          case 0x101: instruction = MAKE_INSN3(crand, BT(), BA(), BB()); break;
          case 0x121: instruction = MAKE_INSN3(creqv, BT(), BA(), BB()); break;
          case 0x1A1: instruction = MAKE_INSN3(crorc, BT(), BA(), BB()); break;
          case 0x1C1: instruction = MAKE_INSN3(cror, BT(), BA(), BB()); break;
          case 0x210:
             {
               if (LK() == 0)
                  {
                    instruction = MAKE_INSN3(bcctr, BO(), BI(), BH());
                  }
                 else
                  {
                    instruction = MAKE_INSN3(bcctrl, BO(), BI(), BH());
                  }
               break;
             }

          default:
             {
            // The default case is now used for handling illegal instructions 
            // (during development it was for those not yet implemented).
               printf ("Error: XL-Form xoOpcode = %d not handled! \n", xoOpcode);
               ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_A_formInstruction_00()
{
     SgAsmPowerpcInstruction* instruction = NULL;

#if DEBUG_OPCODES
     printf ("A-Form instruction opcode = 0x%x primaryOpcode = 0x%x frtOpcode = 0x%x fraOpcode = 0x%x frbOpcode = 0x%x frcOpcode = 0x%x xOpcode = 0x%x rcOpcode = 0x%x \n", insn, primaryOpcode, frtOpcode, fraOpcode, frbOpcode, frcOpcode, xOpcode, rcOpcode);
#endif

     switch(fld<26, 30>())
        {
          case 0x05: instruction = MAKE_INSN4(fpsel, FRT(), FRA(), FRB(), FRC()); break;
          case 0x08: instruction = MAKE_INSN3(fpmul, FRT(), FRA(), FRC()); break;
          case 0x09: instruction = MAKE_INSN3(fxmul, FRT(), FRA(), FRC()); break;
          case 0x0A: instruction = MAKE_INSN3(fxpmul, FRT(), FRA(), FRC()); break;
          case 0x0B: instruction = MAKE_INSN3(fxsmul, FRT(), FRA(), FRC()); break;
          case 0x0C: instruction = MAKE_INSN3(fpadd, FRT(), FRA(), FRB()); break;
          case 0x0D: instruction = MAKE_INSN3(fpsub, FRT(), FRA(), FRB()); break;
          case 0x0E: instruction = MAKE_INSN2(fpre, FRT(), FRB()); break;
          case 0x0F: instruction = MAKE_INSN2(fprsqrte, FRT(), FRB()); break;
          case 0x10: instruction = MAKE_INSN4(fpmadd, FRT(), FRA(), FRB(), FRC()); break;
          case 0x11: instruction = MAKE_INSN4(fxmadd, FRT(), FRA(), FRB(), FRC()); break;
          case 0x12: instruction = MAKE_INSN4(fxcpmadd, FRT(), FRA(), FRB(), FRC()); break;
          case 0x13: instruction = MAKE_INSN4(fxcsmadd, FRT(), FRA(), FRB(), FRC()); break;
          case 0x14: instruction = MAKE_INSN4(fpnmadd, FRT(), FRA(), FRB(), FRC()); break;
          case 0x15: instruction = MAKE_INSN4(fxnmadd, FRT(), FRA(), FRB(), FRC()); break;
          case 0x16: instruction = MAKE_INSN4(fxcpnmadd, FRT(), FRA(), FRB(), FRC()); break;
          case 0x17: instruction = MAKE_INSN4(fxcsnmadd, FRT(), FRA(), FRB(), FRC()); break;
          case 0x18: instruction = MAKE_INSN4(fpmsub, FRT(), FRA(), FRB(), FRC()); break;
          case 0x19: instruction = MAKE_INSN4(fxmsub, FRT(), FRA(), FRB(), FRC()); break;
          case 0x1A: instruction = MAKE_INSN4(fxcpmsub, FRT(), FRA(), FRB(), FRC()); break;
          case 0x1B: instruction = MAKE_INSN4(fxcsmsub, FRT(), FRA(), FRB(), FRC()); break;
          case 0x1C: instruction = MAKE_INSN4(fpnmsub, FRT(), FRA(), FRB(), FRC()); break;
          case 0x1D: instruction = MAKE_INSN4(fxnmsub, FRT(), FRA(), FRB(), FRC()); break;
          case 0x1E: instruction = MAKE_INSN4(fxcpnmsub, FRT(), FRA(), FRB(), FRC()); break;
          case 0x1F: instruction = MAKE_INSN4(fxcsnmsub, FRT(), FRA(), FRB(), FRC()); break;
          default:
             {
            // The default case is now used for handling illegal instructions 
            // (during development it was for those not yet implemented).
               printf ("Error: A-Form xOpcode = %d (illegal instruction) \n", int(fld<26, 30>()));
               ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_A_formInstruction_04()
{
     SgAsmPowerpcInstruction* instruction = NULL;

#if DEBUG_OPCODES
     printf ("A-Form instruction opcode = 0x%x primaryOpcode = 0x%x frtOpcode = 0x%x fraOpcode = 0x%x frbOpcode = 0x%x frcOpcode = 0x%x xOpcode = 0x%x rcOpcode = 0x%x \n", insn, primaryOpcode, frtOpcode, fraOpcode, frbOpcode, frcOpcode, xOpcode, rcOpcode);
#endif

  // FIX ME: Make the floating point registers use the powerpc_regclass_fpr instead of powerpc_regclass_gpr

     switch(fld<26, 30>())
        {
          case 0x18: instruction = MAKE_INSN4(fxcpnpma, FRT(), FRA(), FRB(), FRC()); break;
          case 0x19: instruction = MAKE_INSN4(fxcsnpma, FRT(), FRA(), FRB(), FRC()); break;
          case 0x1A: instruction = MAKE_INSN4(fxcpnsma, FRT(), FRA(), FRB(), FRC()); break;
          case 0x1B: instruction = MAKE_INSN4(fxcsnsma, FRT(), FRA(), FRB(), FRC()); break;
          case 0x1C: instruction = MAKE_INSN4(fxcxma, FRT(), FRA(), FRB(), FRC()); break;
          case 0x1D: instruction = MAKE_INSN4(fxcxnpma, FRT(), FRA(), FRB(), FRC()); break;
          case 0x1E: instruction = MAKE_INSN4(fxcxnsma, FRT(), FRA(), FRB(), FRC()); break;
          case 0x1F: instruction = MAKE_INSN4(fxcxnms, FRT(), FRA(), FRB(), FRC()); break;
          default:
             {
            // The default case is now used for handling illegal instructions 
            // (during development it was for those not yet implemented).
               printf ("Error: A-Form xOpcode = %d (illegal instruction) \n", int(fld<26, 30>()));
               ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_A_formInstruction_3B()
{
     SgAsmPowerpcInstruction* instruction = NULL;

     switch(fld<26, 30>())
        {
          case 0x12: instruction = MAKE_INSN3_RC(fdivs, FRT(), FRA(), FRB()); break;
          case 0x14: instruction = MAKE_INSN3_RC(fsubs, FRT(), FRA(), FRB()); break;
          case 0x15: instruction = MAKE_INSN3_RC(fadds, FRT(), FRA(), FRB()); break;
          case 0x16: instruction = MAKE_INSN2_RC(fsqrts, FRT(), FRB()); break;
          case 0x18: instruction = MAKE_INSN2_RC(fres, FRT(), FRB()); break;
          case 0x19: instruction = MAKE_INSN3_RC(fmuls, FRT(), FRA(), FRC()); break;
          case 0x1A: instruction = MAKE_INSN2_RC(frsqrtes, FRT(), FRB()); break;
          case 0x1C: instruction = MAKE_INSN4_RC(fmsubs, FRT(), FRA(), FRC(), FRB()); break;
          case 0x1D: instruction = MAKE_INSN4_RC(fmadds, FRT(), FRA(), FRC(), FRB()); break;
          case 0x1E: instruction = MAKE_INSN4_RC(fnmsubs, FRT(), FRA(), FRC(), FRB()); break;
          case 0x1F: instruction = MAKE_INSN4_RC(fnmadds, FRT(), FRA(), FRC(), FRB()); break;
          default:
             {
            // The default case is now used for handling illegal instructions 
            // (during development it was for those not yet implemented).
               printf ("Error: A-Form xOpcode = %d (illegal instruction) \n", int(fld<26, 30>()));
               ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_A_formInstruction_3F()
{
     SgAsmPowerpcInstruction* instruction = NULL;

#if DEBUG_OPCODES
     printf ("A-Form instruction opcode = 0x%x primaryOpcode = 0x%x frtOpcode = 0x%x fraOpcode = 0x%x frbOpcode = 0x%x frcOpcode = 0x%x xOpcode = 0x%x rcOpcode = 0x%x \n", insn, primaryOpcode, frtOpcode, fraOpcode, frbOpcode, frcOpcode, xOpcode, rcOpcode);
#endif

  // FIX ME: Make the floating point registers use the powerpc_regclass_fpr instead of powerpc_regclass_gpr

     switch(fld<26, 30>())
        {
          case 0x12: instruction = MAKE_INSN3_RC(fdiv, FRT(), FRA(), FRB()); break;
          case 0x14: instruction = MAKE_INSN3_RC(fsub, FRT(), FRA(), FRB()); break;
          case 0x15: instruction = MAKE_INSN3_RC(fadd, FRT(), FRA(), FRB()); break;
          case 0x16: instruction = MAKE_INSN2_RC(fsqrt, FRT(), FRB()); break;
          case 0x17: instruction = MAKE_INSN4_RC(fsel, FRT(), FRA(), FRC(), FRB()); break;
          case 0x18: instruction = MAKE_INSN2_RC(fre, FRT(), FRB()); break;
          case 0x19: instruction = MAKE_INSN3_RC(fmul, FRT(), FRA(), FRC()); break;
          case 0x1A: instruction = MAKE_INSN2_RC(frsqrte, FRT(), FRB()); break;
          case 0x1C: instruction = MAKE_INSN4_RC(fmsub, FRT(), FRA(), FRC(), FRB()); break;
          case 0x1D: instruction = MAKE_INSN4_RC(fmadd, FRT(), FRA(), FRC(), FRB()); break;
          case 0x1E: instruction = MAKE_INSN4_RC(fnmsub, FRT(), FRA(), FRC(), FRB()); break;
          case 0x1F: instruction = MAKE_INSN4_RC(fnmadd, FRT(), FRA(), FRC(), FRB()); break;
          default:
             {
            // The default case is now used for handling illegal instructions 
            // (during development it was for those not yet implemented).
               printf ("Error: A-Form xOpcode = %d (illegal instruction) \n", int(fld<26, 30>()));
               ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_MD_formInstruction()
{
     SgAsmPowerpcInstruction* instruction = NULL;

     printf ("MD-Form instructions not implemented yet, I have no examples so far! \n");

     ROSE_ASSERT(instruction != NULL);
     return instruction;
}

SgAsmPowerpcInstruction*
DisassemblerPowerpc::decode_MDS_formInstruction()
{
     SgAsmPowerpcInstruction* instruction = NULL;

     printf ("MDS-Form instructions not implemented yet, I have no examples so far! \n");

     ROSE_ASSERT(instruction != NULL);
     return instruction;
}
