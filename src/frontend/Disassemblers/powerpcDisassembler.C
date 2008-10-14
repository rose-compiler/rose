#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <fcntl.h>
#include <vector>
#include "rose.h"

using namespace std;
using namespace SageBuilderAsm;

// DQ (10/11/2008): This implementation follows the design of the disassemblers for the x86 and ARM instruction sets.

// ***********************
// THIS WORK IS UNFINISHED
// ***********************

namespace PowerpcDisassembler
   {
     struct SingleInstructionDisassembler
        {
          const Parameters& p;
          uint32_t insn;
          std::set<uint64_t>* knownSuccessorsReturn;

       // PowerPC instructions don't have conditions like ARM, as I understand it.
       // PowerpcInstructionCondition cond;
       // SingleInstructionDisassembler(const Parameters& p, uint32_t insn, std::set<uint64_t>* knownSuccessorsReturn): p(p), insn(insn), knownSuccessorsReturn(knownSuccessorsReturn), cond(powerpc_cond_unknown) {}

          SingleInstructionDisassembler(const Parameters& p, uint32_t insn, std::set<uint64_t>* knownSuccessorsReturn): p(p), insn(insn), knownSuccessorsReturn(knownSuccessorsReturn) {}

          SgAsmPowerpcInstruction* disassemble();

       // There are 15 different forms of PowerPC instructions, but all are 32-bit (fixed length instruction set).
          SgAsmPowerpcInstruction* decode_I_formInstruction();
          SgAsmPowerpcInstruction* decode_B_formInstruction();
          SgAsmPowerpcInstruction* decode_SC_formInstruction();
          SgAsmPowerpcInstruction* decode_D_formInstruction();
          SgAsmPowerpcInstruction* decode_DS_formInstruction();
          SgAsmPowerpcInstruction* decode_X_formInstruction();
          SgAsmPowerpcInstruction* decode_XL_formInstruction();
          SgAsmPowerpcInstruction* decode_XFX_formInstruction();
          SgAsmPowerpcInstruction* decode_XFL_formInstruction();
          SgAsmPowerpcInstruction* decode_XS_formInstruction();
          SgAsmPowerpcInstruction* decode_XO_formInstruction();
          SgAsmPowerpcInstruction* decode_A_formInstruction();
          SgAsmPowerpcInstruction* decode_M_formInstruction();
          SgAsmPowerpcInstruction* decode_MD_formInstruction();
          SgAsmPowerpcInstruction* decode_MDS_formInstruction();
        };

     SgAsmPowerpcInstruction* makeInstructionWithoutOperands(uint32_t address, const std::string& mnemonic, PowerpcInstructionKind kind, uint32_t insn);

  // SgAsmPowerpcRegisterReferenceExpression* makeRegister(uint8_t reg);
     SgAsmPowerpcRegisterReferenceExpression* makeRegister(PowerpcRegisterClass reg_class, int reg_number, PowerpcConditionRegisterAccessGranularity reg_grainularity);

  // SgAsmPowerpcInstruction* disassemble(const Parameters& p, const uint8_t* const insn, const uint64_t insnSize, size_t positionInVector, set<uint64_t>* knownSuccessorsReturn);
  // SgAsmPowerpcInstruction* disassemble(const Parameters& p, const uint32_t* const insn, const uint64_t insnSize, size_t positionInVector, set<uint64_t>* knownSuccessorsReturn);
  // SgAsmPowerpcInstruction* disassemble(const Parameters& p, const uint32_t insn, const uint64_t insnSize, size_t positionInVector, set<uint64_t>* knownSuccessorsReturn);
     SgAsmPowerpcInstruction* disassemble(const Parameters& p, const uint8_t* const insn, const uint64_t insnSize, size_t positionInVector, set<uint64_t>* knownSuccessorsReturn);

// end of namespace: PowerpcDisassembler
   }

#define MAKE_INSN0(Mne) (makeInstructionWithoutOperands(p.ip, #Mne, powerpc_##Mne, insn))
#define MAKE_INSN1(Mne, Op1) (appendOperand(MAKE_INSN0(Mne), (Op1)))
#define MAKE_INSN2(Mne, Op1, Op2) (appendOperand(MAKE_INSN1(Mne, Op1), (Op2)))
#define MAKE_INSN3(Mne, Op1, Op2, Op3) (appendOperand(MAKE_INSN2(Mne, Op1, Op2), (Op3)))
#define MAKE_INSN4(Mne, Op1, Op2, Op3, Op4) (appendOperand(MAKE_INSN3(Mne, Op1, Op2, Op3), (Op4)))

SgAsmPowerpcInstruction*
PowerpcDisassembler::makeInstructionWithoutOperands(uint32_t address, const std::string& mnemonic, PowerpcInstructionKind kind, uint32_t insn)
   {
  // Constructor: SgAsmPowerpcInstruction(rose_addr_t address = 0, std::string mnemonic = "", PowerpcInstructionKind kind = powerpc_unknown_instruction);
     SgAsmPowerpcInstruction* instruction = new SgAsmPowerpcInstruction(address, mnemonic, kind);
     ROSE_ASSERT (instruction);

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

     SgAsmPowerpcRegisterReferenceExpression* r = new SgAsmPowerpcRegisterReferenceExpression(reg_class,reg_number,reg_grainularity);

     return r;
  }

SgAsmPowerpcInstruction*
PowerpcDisassembler::disassemble(const Parameters& p, const uint8_t* const insn, const uint64_t insnSize, size_t positionInVector, set<uint64_t>* knownSuccessorsReturn)
   {
     printf ("Inside of PowerpcDisassembler::disassemble(): insn = %p insnSize = %zu positionInVector = %zu \n",insn,insnSize,positionInVector);

  // This is the way it is to deal with overflows
     if (positionInVector >= insnSize || positionInVector + 4 > insnSize)
        {
          abort();
        }

  // Note that PowerPC is big-endian...but that is can support both big and little endian processor modes (with much weirdness).
     uint32_t c = insn[positionInVector + 0];
     c = (c << 8) | insn[positionInVector + 1];
     c = (c << 8) | insn[positionInVector + 2];
     c = (c << 8) | insn[positionInVector + 3];

     printf ("Single instruction opcode = 0x%x (calling disassembler) \n",c);

     SingleInstructionDisassembler sid(p, c, knownSuccessorsReturn);
     return sid.disassemble();
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::disassemble()
    {
   // The Primary Opcode Field is bits 0-5, 6-bits wide, so there are max 64 primary opcode values
      uint8_t primaryOpcode = (insn >> 26) & 0x3F;

      printf ("instruction opcode = 0x%x primaryOpcode = 0x%x \n",insn,primaryOpcode);

   // This should clear the upper two bits of the byte (so we only evaluate the Primary Opcode Field)
   // getByte(opcode);

      SgAsmPowerpcInstruction* instruction = NULL;

      if (knownSuccessorsReturn != NULL)
         {
        // Test for unconditional branch
           if (primaryOpcode != 18)
              {
             // This is NOT an unconditional branch
                knownSuccessorsReturn->insert(p.ip + 4);
              }
         }

   // Handle all the different legal Primary Opcode values
      switch (primaryOpcode)
         {
           case 0x00: 
           case 0x01: { /* illegal instruction */ ROSE_ASSERT(false); break; }

           case 0x02: { instruction = decode_D_formInstruction(); break; }
           case 0x03: { instruction = decode_D_formInstruction(); break; }

           case 0x04: 
           case 0x05: 
           case 0x06: { /* illegal instruction */ ROSE_ASSERT(false); break; }

           case 0x07: { instruction = decode_D_formInstruction(); break; }
           case 0x08: { instruction = decode_D_formInstruction(); break; }

           case 0x09: { /* illegal instruction */ ROSE_ASSERT(false); break; }

           case 0x0A: { instruction = decode_D_formInstruction(); break; }
           case 0x0B: { instruction = decode_D_formInstruction(); break; }
           case 0x0C: { instruction = decode_D_formInstruction(); break; }
           case 0x0D: { instruction = decode_D_formInstruction(); break; }
           case 0x0E: { instruction = decode_D_formInstruction(); break; }
           case 0x0F: { instruction = decode_D_formInstruction(); break; }

        // Branch instruction
           case 0x10: { instruction = decode_B_formInstruction(); break; }

           case 0x11: { instruction = decode_SC_formInstruction(); break; }

        // Conditional branch
           case 0x12: { instruction = decode_I_formInstruction(); break; }

           case 0x13: { instruction = decode_XL_formInstruction(); break; }

        // 20
           case 0x14: { instruction = decode_M_formInstruction(); break; }
           case 0x15: { instruction = decode_M_formInstruction(); break; }

           case 0x16: { /* illegal instruction */ ROSE_ASSERT(false); break; }

           case 0x17: { instruction = decode_M_formInstruction(); break; }

        // 24
           case 0x18: { instruction = decode_D_formInstruction(); break; }
           case 0x19: { instruction = decode_D_formInstruction(); break; }
           case 0x1A: { instruction = decode_D_formInstruction(); break; }
           case 0x1B: { instruction = decode_D_formInstruction(); break; }
           case 0x1C: { instruction = decode_D_formInstruction(); break; }
           case 0x1D: { instruction = decode_D_formInstruction(); break; }

        // 30
           case 0x1E: { instruction = decode_MD_formInstruction(); break; }

        // 31: includes X form, XO form, XFX form, and XS form
           case 0x1F: { instruction = decode_X_formInstruction(); break; }

        // 32 load store instructions
           case 0x20: { instruction = decode_D_formInstruction(); break; }
           case 0x21: { instruction = decode_D_formInstruction(); break; }
           case 0x22: { instruction = decode_D_formInstruction(); break; }
           case 0x23: { instruction = decode_D_formInstruction(); break; }
           case 0x24: { instruction = decode_D_formInstruction(); break; }
           case 0x25: { instruction = decode_D_formInstruction(); break; }
           case 0x26: { instruction = decode_D_formInstruction(); break; }
           case 0x27: { instruction = decode_D_formInstruction(); break; }
           case 0x28: { instruction = decode_D_formInstruction(); break; }
           case 0x29: { instruction = decode_D_formInstruction(); break; }
           case 0x2A: { instruction = decode_D_formInstruction(); break; }
           case 0x2B: { instruction = decode_D_formInstruction(); break; }
           case 0x2C: { instruction = decode_D_formInstruction(); break; }
           case 0x2D: { instruction = decode_D_formInstruction(); break; }
           case 0x2E: { instruction = decode_D_formInstruction(); break; }
           case 0x2F: { instruction = decode_D_formInstruction(); break; }
           case 0x30: { instruction = decode_D_formInstruction(); break; }
           case 0x31: { instruction = decode_D_formInstruction(); break; }
           case 0x32: { instruction = decode_D_formInstruction(); break; }
           case 0x33: { instruction = decode_D_formInstruction(); break; }
           case 0x34: { instruction = decode_D_formInstruction(); break; }
           case 0x35: { instruction = decode_D_formInstruction(); break; }
           case 0x36: { instruction = decode_D_formInstruction(); break; }
           case 0x37: { instruction = decode_D_formInstruction(); break; }

        // 56
           case 0x38: 
           case 0x39: { /* illegal instruction */ ROSE_ASSERT(false); break; }

        // 58
           case 0x3A: { instruction = decode_DS_formInstruction(); break; }

           case 0x3B: { instruction = decode_A_formInstruction(); break; }

        // 60
           case 0x3C: 
           case 0x3D: { /* illeagal instruction */ ROSE_ASSERT(false); break; }

        // 62
           case 0x3E: { instruction = decode_DS_formInstruction(); break; }

        // 63: includes A form, X form, and XFL form instructions
           case 0x3F:
              {
             // Depending on if this is A form or X form, the extended opCode maps to different bit ranges, so this code is incorrect!
                uint8_t secodaryOpcode = (insn >> 21) & 0x1F;
                switch (secodaryOpcode)
                   {
                  // Computed from bits 21-30
                  // Values: 0, 12, 14, 15
                     case 0x00:
                     case 0x0C:
                     case 0x0E:
                     case 0x0F: { instruction = decode_X_formInstruction(); break; }

                  // Computed from bits 26-30
                  // Values: 18, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
                     case 0x12:
                     case 0x14:
                     case 0x15:
                     case 0x16:
                     case 0x17:
                     case 0x18:
                     case 0x19:
                     case 0x1A:
                     case 0x1B:
                     case 0x1C:
                     case 0x1D:
                     case 0x1E:
                     case 0x1F: { instruction = decode_A_formInstruction(); break; }

                  // Computed from bits 21-30
                  // 32
                     case 0x20:
                     case 0x21:
                     case 0x22:

                  // 38
                     case 38:
                     case 40:
                     case 64:
                     case 70:
                     case 72:
                     case 134:
                     case 136:
                     case 264:
                     case 583: { instruction = decode_X_formInstruction(); break; }

                  // Computed from bits 21-30
                     case 711: { instruction = decode_XFL_formInstruction(); break; }

                     case 814:
                     case 815:

                  // This is 1101100000 (a 10-bit value)
                     case 864: { instruction = decode_A_formInstruction(); break; }

                     default:
                        {
                          printf ("Secondary opcode not handled yet (or illegal instruction): secodaryOpcode = %d \n",secodaryOpcode);
                          ROSE_ASSERT(false);
                        }
                   }

                instruction = decode_X_formInstruction();
                break;
              }

        // And so on until we handle all 64 opcodes (the legal instructions only)!

           default:
              {
                printf ("Primary opcode not handled yet: primaryOpcode = %d \n",primaryOpcode);
                ROSE_ASSERT(false);
              }
         }

    return instruction;
  }


   
SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_I_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_B_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;

     uint8_t primaryOpcode = (insn >> 26) & 0x3F;

  // Get bits 6-8, 3 bits as the BF opcode
     uint8_t boOpcode = (insn >> 21) & 0x1F;

  // Get the bits 11-15, next 5 bits, as the tertiary opcode
     uint8_t biOpcode = (insn >> 16) & 0x1F;

  // Get the bits 16-31, next 16 bits, as the tertiary opcode
     uint16_t bdOpcode = (insn >> 2) & 0x3FFF;

  // Get bit 30, 1 bit as the reserved flag
     uint8_t aaOpcode = (insn >> 1) & 0x1;

  // Get bit 31, 1 bit as the reserved flag
     uint8_t lkOpcode = (insn >> 0) & 0x1;

     printf ("B-Form instruction opcode = 0x%x boOpcode = 0x%x biOpcode = 0x%x bdOpcode = 0x%x aaOpcode = 0x%x lkOpcode = 0x%x \n",insn,boOpcode,biOpcode,bdOpcode,aaOpcode,lkOpcode);

  // SgAsmPowerpcInstruction(rose_addr_t address = 0, std::string mnemonic = "", PowerpcInstructionKind kind = powerpc_unknown_instruction);
     switch(primaryOpcode)
        {
          case 0x10:
             {
            // Skip the registers for now!
               SgAsmExpression* BO = new SgAsmByteValueExpression(boOpcode);
               SgAsmExpression* BI = new SgAsmByteValueExpression(biOpcode);

            // SgAsmExpression* SI = new SgAsmDoubleWordValueExpression(bdOpcode);
               SgAsmExpression* targetAddress = NULL;
               if (aaOpcode == 0)
                  {
                 // The addess is the summ of BD || 0x0b00 sign extended, and the address of the instruction.
                    targetAddress = new SgAsmDoubleWordValueExpression(bdOpcode);
                  }
                 else
                  {
                    ROSE_ASSERT(aaOpcode == 1);

                 // This should be the BD || 0x0b00 sign extended
                    targetAddress = new SgAsmDoubleWordValueExpression(bdOpcode);
                  }

            // Check the link flag
               if (lkOpcode == 1)
                  {
                 // The addess of the next instruction should be placed into the knownSuccessorsReturn set (as in function call return).
                    printf ("The addess of the next instruction should be placed into the knownSuccessorsReturn set. \n");
                    ROSE_ASSERT(false);
                  }

            // Do we want to handle the special cases where "bc" translates to the forms "blt", "bne", and "bdnz"?
               instruction = MAKE_INSN3(bc,BO,BI,targetAddress);
               break;
             }

          default:
             {
               printf ("Error: B-Form primaryOpcode = %d not handled! \n",primaryOpcode);
               ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_SC_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_D_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;

  // The primaryOpcode does not have to be 0x0B!
     uint8_t primaryOpcode = (insn >> 26) & 0x3F;
  // ROSE_ASSERT(primaryOpcode == 0x0B);

  // Get the bits 6-10, next 5 bits, as the secondary opcode: RT, RS, TO, FRT, FRS
     uint8_t secodaryOpcode = (insn >> 21) & 0x1F;

  // Get the bits 11-15, next 5 bits, as the tertiary opcode
     uint8_t raOpcode = (insn >> 16) & 0x1F;

  // Get bits 6-8, 3 bits as the BF opcode
     uint8_t bfOpcode = (insn >> 21) & 0x8;

  // Get bit 9, 1 bit as the reserved flag
     uint8_t reservedOpcode = (insn >> 24) & 0x1;

  // Get bit 10, 1 bit as the length flag
     uint8_t lengthOpcode = (insn >> 25) & 0x1;

  // Get the bits 16-31, next 16 bits, as the tertiary opcode
     uint8_t lastOpcode = (insn >> 0) & 0xFFFF;

     printf ("D-Form instruction opcode = 0x%x secodaryOpcode = 0x%x bfOpcode = 0x%x reservedOpcode = 0x%x lengthOpcode = 0x%x raOpcode = 0x%x lastOpcode 0x%x \n",insn,secodaryOpcode,bfOpcode,reservedOpcode,lengthOpcode,raOpcode,lastOpcode);

  // SgAsmPowerpcInstruction(rose_addr_t address = 0, std::string mnemonic = "", PowerpcInstructionKind kind = powerpc_unknown_instruction);
     switch(primaryOpcode)
        {
       // 11
          case 0x0B:
             {
               SgAsmExpression* BF = new SgAsmByteValueExpression(bfOpcode);
               SgAsmExpression* L  = new SgAsmByteValueExpression(lengthOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode,powerpc_condreggranularity_field);
               SgAsmExpression* SI = new SgAsmWordValueExpression(lastOpcode);
               instruction = MAKE_INSN4(cmpi,BF,L,RA,SI);
               break;
             }

       // 14
          case 0x0E:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,secodaryOpcode,powerpc_condreggranularity_field);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode,powerpc_condreggranularity_field);
               SgAsmExpression* D = new SgAsmWordValueExpression(lastOpcode);
               instruction = MAKE_INSN3(addi,RT,RA,D);
               break;
             }

       // 32
          case 0x20:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,secodaryOpcode,powerpc_condreggranularity_field);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode,powerpc_condreggranularity_field);
               SgAsmExpression* D = new SgAsmWordValueExpression(lastOpcode);
               instruction = MAKE_INSN3(lwz,RT,RA,D);
               break;
             }

       // 37
          case 0x25:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,secodaryOpcode,powerpc_condreggranularity_field);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode,powerpc_condreggranularity_field);
               SgAsmExpression* D = new SgAsmWordValueExpression(lastOpcode);
               instruction = MAKE_INSN3(stwu,RT,RA,D);
               break;
             }

          default:
             {
               printf ("Error: D-Form primaryOpcode = %d not handled! \n",primaryOpcode);
               ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_DS_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_X_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_XL_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_XFX_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_XFL_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_XS_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_XO_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_A_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_M_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_MD_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_MDS_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;
     return instruction;
   }

