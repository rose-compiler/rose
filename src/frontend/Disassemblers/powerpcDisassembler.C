#include "rose.h"
#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <fcntl.h>
#include <vector>


#include "integerOps.h"

using namespace std;
using namespace SageBuilderAsm;

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

namespace PowerpcDisassembler
   {
     SgAsmPowerpcRegisterReferenceExpression* makeRegister(PowerpcRegisterClass reg_class, int reg_number, PowerpcConditionRegisterAccessGranularity reg_grainularity = powerpc_condreggranularity_whole );

     struct SingleInstructionDisassembler
        {
          const Parameters& p;
          uint32_t insn;
          std::set<uint64_t>* knownSuccessorsReturn;

          SingleInstructionDisassembler(const Parameters& p, uint32_t insn, std::set<uint64_t>* knownSuccessorsReturn): p(p), insn(insn), knownSuccessorsReturn(knownSuccessorsReturn) {}

       // Helper function to use field definitions (counted with bits from left and inclusive on both sides) from manual
          template <size_t First, size_t Last> uint32_t fld() const {
            return (insn >> (31 - Last)) & (IntegerOps::GenMask<uint32_t, Last - First + 1>::value);
          }

       // Decoded fields from section 1.7.16 of the v2.01 UISA
          bool AA() const {return fld<30, 30>();}
          SgAsmPowerpcRegisterReferenceExpression* BA() const {return makeRegister(powerpc_regclass_cr, fld<11, 15>(), powerpc_condreggranularity_bit);}
          SgAsmPowerpcRegisterReferenceExpression* BB() const {return makeRegister(powerpc_regclass_cr, fld<16, 20>(), powerpc_condreggranularity_bit);}
          uint64_t BD() const {return IntegerOps::signExtend<16, 64>((uint64_t)insn & 0xfffc);}
          SgAsmPowerpcRegisterReferenceExpression* BF_cr() const {return makeRegister(powerpc_regclass_cr, fld<6, 8>(), powerpc_condreggranularity_field);}
          SgAsmPowerpcRegisterReferenceExpression* BF_fpscr() const {return makeRegister(powerpc_regclass_fpscr, fld<6, 8>(), powerpc_condreggranularity_field);}
          SgAsmPowerpcRegisterReferenceExpression* BFA_cr() const {return makeRegister(powerpc_regclass_cr, fld<11, 13>(), powerpc_condreggranularity_field);}
          SgAsmPowerpcRegisterReferenceExpression* BFA_fpscr() const {return makeRegister(powerpc_regclass_fpscr, fld<11, 13>(), powerpc_condreggranularity_field);}
          SgAsmValueExpression* BH() const {return makeByteValue(fld<19, 20>());}
          SgAsmPowerpcRegisterReferenceExpression* BI() const {return BA();}
          SgAsmValueExpression* BO() const {return makeByteValue(fld<6, 10>());}
          SgAsmPowerpcRegisterReferenceExpression* BT() const {return makeRegister(powerpc_regclass_cr, fld<6, 10>(), powerpc_condreggranularity_bit);}
          SgAsmValueExpression* D() const {return makeQWordValue(IntegerOps::signExtend<16, 64>((uint64_t)fld<16, 31>()));}
          SgAsmValueExpression* DS() const {return makeQWordValue(IntegerOps::signExtend<16, 64>((uint64_t)fld<16, 31>() & 0xfffc));}
          SgAsmValueExpression* FLM() const {return makeByteValue(fld<7, 14>());}
          SgAsmPowerpcRegisterReferenceExpression* FRA() const {return makeRegister(powerpc_regclass_fpr, fld<11, 15>());}
          SgAsmPowerpcRegisterReferenceExpression* FRB() const {return makeRegister(powerpc_regclass_fpr, fld<16, 20>());}
          SgAsmPowerpcRegisterReferenceExpression* FRC() const {return makeRegister(powerpc_regclass_fpr, fld<21, 25>());}
          SgAsmPowerpcRegisterReferenceExpression* FRS() const {return makeRegister(powerpc_regclass_fpr, fld<6, 10>());}
          SgAsmPowerpcRegisterReferenceExpression* FRT() const {return FRS();}
          SgAsmValueExpression* FXM() const {return makeByteValue(fld<12, 19>());}
          
          SgAsmValueExpression* L_10() const {return makeByteValue(fld<10, 10>());}
          SgAsmValueExpression* L_15() const {return makeByteValue(fld<15, 15>());}
          uint8_t L_sync() const {return fld<9, 10>();}
          SgAsmValueExpression* LEV() const {return makeByteValue(fld<20, 26>());}
          uint64_t LI() const {return IntegerOps::signExtend<26, 64>(uint64_t(fld<6, 29>() * 4));}
          bool LK() const {return fld<31, 31>();}
          SgAsmValueExpression* MB_32bit() const {return makeByteValue(fld<21, 25>());}
          SgAsmValueExpression* ME_32bit() const {return makeByteValue(fld<26, 30>());}
          SgAsmValueExpression* MB_64bit() const {return makeByteValue(fld<21, 26>());} // FIXME check for splitting
          SgAsmValueExpression* ME_64bit() const {return makeByteValue(fld<21, 26>());} // FIXME check for splitting
          SgAsmValueExpression* NB() const {return makeByteValue(fld<16, 20>() == 0 ? 32 : fld<16, 20>());}
          bool OE() const {return fld<21, 21>();}
          SgAsmPowerpcRegisterReferenceExpression* RA() const {return makeRegister(powerpc_regclass_gpr, fld<11, 15>());}
          SgAsmExpression* RA_or_zero() const {return fld<11, 15>() == 0 ? (SgAsmExpression*)makeByteValue(0) : RA();}
          SgAsmPowerpcRegisterReferenceExpression* RB() const {return makeRegister(powerpc_regclass_gpr, fld<16, 20>());}
          bool Rc() const {return fld<31, 31>();}
          SgAsmPowerpcRegisterReferenceExpression* RS() const {return makeRegister(powerpc_regclass_gpr, fld<6, 10>());}
          SgAsmPowerpcRegisterReferenceExpression* RT() const {return RS();}
          SgAsmValueExpression* SH_32bit() const {return makeByteValue(fld<16, 20>());}
          SgAsmValueExpression* SH_64bit() const {return makeByteValue(fld<16, 20>() + fld<30, 30>() * 32);} // FIXME check
          SgAsmValueExpression* SI() const {return D();}
          SgAsmPowerpcRegisterReferenceExpression* SPR() const {return makeRegister(powerpc_regclass_spr, fld<16, 20>() * 32 + fld<11, 15>());}
          SgAsmPowerpcRegisterReferenceExpression* SR() const {return makeRegister(powerpc_regclass_sr, fld<12, 15>());}
          SgAsmPowerpcRegisterReferenceExpression* TBR() const {return makeRegister(powerpc_regclass_tbr, fld<16, 20>() * 32 + fld<11, 15>());}
          SgAsmValueExpression* TH() const {return makeByteValue(fld<9, 10>());}
          SgAsmValueExpression* TO() const {return makeByteValue(fld<6, 10>());}
          SgAsmValueExpression* U() const {return makeByteValue(fld<16, 19>());}
          SgAsmValueExpression* UI() const {return makeQWordValue(fld<16, 31>());}

          SgAsmMemoryReferenceExpression* memref(SgAsmType* t) const {
            return makeMemoryReference(makeAdd(RA_or_zero(), D()), NULL, t);
          }
          SgAsmMemoryReferenceExpression* memrefx(SgAsmType* t) const {
            return makeMemoryReference(makeAdd(RA_or_zero(), RB()), NULL, t);
          }
          SgAsmMemoryReferenceExpression* memrefu(SgAsmType* t) const {
            if (fld<11, 15>() == 0) throw BadInstruction();
            return makeMemoryReference(makeAdd(RA(), D()), NULL, t);
          }
          SgAsmMemoryReferenceExpression* memrefux(SgAsmType* t) const {
            if (fld<11, 15>() == 0) throw BadInstruction();
            return makeMemoryReference(makeAdd(RA(), RB()), NULL, t);
          }

       // There are 15 different forms of PowerPC instructions, but all are 32-bit (fixed length instruction set).
          SgAsmPowerpcInstruction* decode_I_formInstruction();
          SgAsmPowerpcInstruction* decode_B_formInstruction();
          SgAsmPowerpcInstruction* decode_SC_formInstruction();
          SgAsmPowerpcInstruction* decode_DS_formInstruction();
          SgAsmPowerpcInstruction* decode_X_formInstruction_00();
          SgAsmPowerpcInstruction* decode_X_formInstruction_1F();
          SgAsmPowerpcInstruction* decode_X_formInstruction_3F();
          SgAsmPowerpcInstruction* decode_XL_formInstruction();
          SgAsmPowerpcInstruction* decode_XS_formInstruction();
          SgAsmPowerpcInstruction* decode_A_formInstruction_00();
          SgAsmPowerpcInstruction* decode_A_formInstruction_04();
          SgAsmPowerpcInstruction* decode_A_formInstruction_3B();
          SgAsmPowerpcInstruction* decode_A_formInstruction_3F();
          SgAsmPowerpcInstruction* decode_MD_formInstruction();
          SgAsmPowerpcInstruction* decode_MDS_formInstruction();

          SgAsmQuadWordValueExpression* makeBranchTarget( uint64_t targetAddr ) const;


          SgAsmPowerpcInstruction* disassemble();
        };

     SgAsmPowerpcInstruction* makeInstructionWithoutOperands(uint64_t address, const std::string& mnemonic, PowerpcInstructionKind kind, uint32_t insn);

  // SgAsmPowerpcInstruction* disassemble(const Parameters& p, const uint8_t* const insn, const uint64_t insnSize, size_t positionInVector, set<uint64_t>* knownSuccessorsReturn);
  // SgAsmPowerpcInstruction* disassemble(const Parameters& p, const uint32_t* const insn, const uint64_t insnSize, size_t positionInVector, set<uint64_t>* knownSuccessorsReturn);
  // SgAsmPowerpcInstruction* disassemble(const Parameters& p, const uint32_t insn, const uint64_t insnSize, size_t positionInVector, set<uint64_t>* knownSuccessorsReturn);
     SgAsmPowerpcInstruction* disassemble(const Parameters& p, const uint8_t* const instructionList, const uint64_t insnSize, size_t positionInVector, set<uint64_t>* knownSuccessorsReturn);

// end of namespace: PowerpcDisassembler
   }

#define MAKE_INSN0(Mne) (makeInstructionWithoutOperands(p.ip, #Mne, powerpc_##Mne, insn))
#define MAKE_INSN0_RC(Mne) (Rc() ? MAKE_INSN0(Mne##_record) : MAKE_INSN0(Mne))
#define MAKE_INSN0_O_RC(Mne) (OE() ? MAKE_INSN0_RC(Mne##o) : MAKE_INSN0_RC(Mne))
#define MAKE_INSN1(Mne, Op1) (appendOperand(MAKE_INSN0(Mne), (Op1)))
#define MAKE_INSN1_RC(Mne, Op1) (appendOperand(MAKE_INSN0_RC(Mne), (Op1)))
#define MAKE_INSN1_O_RC(Mne, Op1) (appendOperand(MAKE_INSN0_O_RC(Mne), (Op1)))
#define MAKE_INSN2(Mne, Op1, Op2) (appendOperand(MAKE_INSN1(Mne, Op1), (Op2)))
#define MAKE_INSN2_RC(Mne, Op1, Op2) (appendOperand(MAKE_INSN1_RC(Mne, Op1), (Op2)))
#define MAKE_INSN2_O_RC(Mne, Op1, Op2) (appendOperand(MAKE_INSN1_O_RC(Mne, Op1), (Op2)))
#define MAKE_INSN3(Mne, Op1, Op2, Op3) (appendOperand(MAKE_INSN2(Mne, Op1, Op2), (Op3)))
#define MAKE_INSN3_RC(Mne, Op1, Op2, Op3) (appendOperand(MAKE_INSN2_RC(Mne, Op1, Op2), (Op3)))
#define MAKE_INSN3_O_RC(Mne, Op1, Op2, Op3) (appendOperand(MAKE_INSN2_O_RC(Mne, Op1, Op2), (Op3)))
#define MAKE_INSN4(Mne, Op1, Op2, Op3, Op4) (appendOperand(MAKE_INSN3(Mne, Op1, Op2, Op3), (Op4)))
#define MAKE_INSN4_RC(Mne, Op1, Op2, Op3, Op4) (appendOperand(MAKE_INSN3_RC(Mne, Op1, Op2, Op3), (Op4)))
#define MAKE_INSN5(Mne, Op1, Op2, Op3, Op4, Op5) (appendOperand(MAKE_INSN4(Mne, Op1, Op2, Op3, Op4), (Op5)))
#define MAKE_INSN5_RC(Mne, Op1, Op2, Op3, Op4, Op5) (appendOperand(MAKE_INSN4_RC(Mne, Op1, Op2, Op3, Op4), (Op5)))

SgAsmQuadWordValueExpression*
PowerpcDisassembler::SingleInstructionDisassembler::makeBranchTarget ( uint64_t targetAddr ) const
   {
     if (knownSuccessorsReturn) knownSuccessorsReturn->insert(targetAddr);
     return makeQWordValue(targetAddr);
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::makeInstructionWithoutOperands(uint64_t address, const std::string& mnemonic, PowerpcInstructionKind kind, uint32_t insn)
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
     for (int i = 0; i < 4; ++i)
        {
          bytes[i] = (insn >> (24 - (8 * i))) & 0xFF; // Force big-endian
        }

     instruction->set_raw_bytes(bytes);

     return instruction;
   }

SgAsmPowerpcRegisterReferenceExpression*
PowerpcDisassembler::makeRegister(PowerpcRegisterClass reg_class, int reg_number, PowerpcConditionRegisterAccessGranularity reg_grainularity)
   {
  // Make sure this will translate to a value register.
     ROSE_ASSERT(reg_class > powerpc_regclass_unknown);
     ROSE_ASSERT(reg_class < powerpc_last_register_class);

     ROSE_ASSERT(reg_number >= 0);
     ROSE_ASSERT(reg_number < 1024);

     ROSE_ASSERT(reg_grainularity >= powerpc_condreggranularity_whole);
     ROSE_ASSERT(reg_grainularity <= powerpc_condreggranularity_bit);

  // DQ (10/13/2008): Need to sync up with Jeremiah on work he may be doing here!
  // r->set_powerpc_register_code((SgAsmPowerpcRegisterReferenceExpression::powerpc_register_enum)(reg + 1));
  // r->set_register_class((SgAsmPowerpcRegisterReferenceExpression::powerpc_register_enum)(reg + 1));

     SgAsmPowerpcRegisterReferenceExpression* r = new SgAsmPowerpcRegisterReferenceExpression(reg_class, reg_number, reg_grainularity);

     return r;
  }

SgAsmPowerpcInstruction*
PowerpcDisassembler::disassemble(const Parameters& p, const uint8_t* const instructionList, const uint64_t insnSize, size_t positionInVector, set<uint64_t>* knownSuccessorsReturn)
   {
  // printf ("Inside of PowerpcDisassembler::disassemble(): instructionList = %p insnSize = %zu positionInVector = %zu \n", instructionList, insnSize, positionInVector);

     ROSE_ASSERT(knownSuccessorsReturn != NULL);

  // This is the way it is to deal with overflows
     if (positionInVector >= insnSize || positionInVector + 4 > insnSize)
        {
          abort();
        }

  // Note that PowerPC is big-endian...but PowerPC can support both big and little 
  // endian processor modes (with much weirdness; e.g. PDP endian like propoerties).
     uint32_t c = instructionList[positionInVector + 0];
     c = (c << 8) | instructionList[positionInVector + 1];
     c = (c << 8) | instructionList[positionInVector + 2];
     c = (c << 8) | instructionList[positionInVector + 3];

  // printf ("Single instruction opcode = 0x%x (calling disassembler) \n", c);

  // Added this to get pass zero instruction (padding?)
     SingleInstructionDisassembler sid(p, c, knownSuccessorsReturn);
     return sid.disassemble();
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::disassemble()
   {
  // The Primary Opcode Field is bits 0-5, 6-bits wide, so there are max 64 primary opcode values
     uint8_t primaryOpcode = (insn >> 26) & 0x3F;

  // printf ("instruction opcode = 0x%08"PRIx32" primaryOpcode = 0x%"PRIx8" \n", insn, primaryOpcode);

     SgAsmPowerpcInstruction* instruction = NULL;

     ROSE_ASSERT(knownSuccessorsReturn != NULL);

  // Test for unconditional branch
     if (primaryOpcode != 18)
        {
       // This is NOT an unconditional branch
          knownSuccessorsReturn->insert(p.ip + 4);
        }

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

          case 0x01: throw BadInstruction(); break;
          case 0x02: instruction = MAKE_INSN3(tdi, TO(), RA(), SI()); break;
          case 0x03: instruction = MAKE_INSN3(twi, TO(), RA(), SI()); break;

       // 4 : These are the BGL specific PowerPC440 FP2 Architecture instructions
          case 0x04: { instruction = decode_A_formInstruction_04(); break; }

          case 0x05: throw BadInstruction(); break;
          case 0x06: throw BadInstruction(); break;
          case 0x07: instruction = MAKE_INSN3(mulli, RT(), RA(), SI()); break;
          case 0x08: instruction = MAKE_INSN3(subfic, RT(), RA(), SI()); break;
          case 0x09: throw BadInstruction(); break;
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
          case 0x38: throw BadInstruction(); break;
          case 0x39: throw BadInstruction(); break;

       // 58
          case 0x3A: { instruction = decode_DS_formInstruction(); break; }

       // 59
          case 0x3B: { instruction = decode_A_formInstruction_3B(); break; }

       // 60, 61
          case 0x3C: throw BadInstruction(); break;
          case 0x3D: throw BadInstruction(); break;

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
               ROSE_ASSERT(false);
            // throw BadInstruction();
             }
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
   }


   
// SgAsmPowerpcInstruction* PowerpcDisassembler::SingleInstructionDisassembler::decode_I_formInstruction(const uint8_t* const instructionList, size_t positionInVector)
SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_I_formInstruction()
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
                    targetBranchAddress += p.ip;
                  }

               SgAsmQuadWordValueExpression* targetAddressExpression = makeBranchTarget(targetBranchAddress);
               ROSE_ASSERT(knownSuccessorsReturn->empty() == false);

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

     if (LK() == 1)
        {
       // Also need to disassemble the next instruction!
          knownSuccessorsReturn->insert(p.ip + 4);
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_B_formInstruction()
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
                    targetBranchAddress += p.ip;
                  }

               SgAsmQuadWordValueExpression* targetAddressExpression = makeBranchTarget(targetBranchAddress);
               ROSE_ASSERT(knownSuccessorsReturn->empty() == false);

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

     if (LK() == 1)
        {
       // Also need to disassemble the next instruction!
          knownSuccessorsReturn->insert(p.ip + 4);
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_SC_formInstruction()
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
PowerpcDisassembler::SingleInstructionDisassembler::decode_DS_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;

     printf ("DS-Form instructions not implemented yet, I have no examples so far! \n");

     ROSE_ASSERT(instruction != NULL);
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_X_formInstruction_1F()
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
               throw BadInstruction();
             }
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_X_formInstruction_3F()
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
               ROSE_ASSERT(false);

           // throw BadInstruction();
             }
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_X_formInstruction_00()
   {
     SgAsmPowerpcInstruction* instruction = NULL;

     if (insn == 0) throw BadInstruction(); // Special, known bad instruction form

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
               ROSE_ASSERT(false);

           // throw BadInstruction();
             }
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_XL_formInstruction()
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
                    knownSuccessorsReturn->insert(p.ip + 4);
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
                    knownSuccessorsReturn->insert(p.ip + 4);
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
PowerpcDisassembler::SingleInstructionDisassembler::decode_A_formInstruction_00()
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
PowerpcDisassembler::SingleInstructionDisassembler::decode_A_formInstruction_04()
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
PowerpcDisassembler::SingleInstructionDisassembler::decode_A_formInstruction_3B()
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
PowerpcDisassembler::SingleInstructionDisassembler::decode_A_formInstruction_3F()
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
PowerpcDisassembler::SingleInstructionDisassembler::decode_MD_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;

     printf ("MD-Form instructions not implemented yet, I have no examples so far! \n");

     ROSE_ASSERT(instruction != NULL);
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_MDS_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;

     printf ("MDS-Form instructions not implemented yet, I have no examples so far! \n");

     ROSE_ASSERT(instruction != NULL);
     return instruction;
   }

