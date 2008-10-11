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
          PowerPC_InstructionCondition cond;

          SingleInstructionDisassembler(const Parameters& p, uint32_t insn, std::set<uint64_t>* knownSuccessorsReturn): p(p), insn(insn), knownSuccessorsReturn(knownSuccessorsReturn), cond(powerpc_cond_unknown) {}

          SgAsmPowerpcInstruction* disassemble();
        };

     SgAsmPowerPCInstruction* makeInstructionWithoutOperands(uint32_t address, const std::string& mnemonic, int condPos, PowerPC_InstructionKind kind, PowerPC_InstructionCondition cond, uint32_t insn);
     SgAsmPowerPC_RegisterReferenceExpression* makeRegister(uint8_t reg);

     SgAsmPowerpcInstruction* disassemble(const Parameters& p, const uint8_t* const insn, const uint64_t insnSize, size_t positionInVector, set<uint64_t>* knownSuccessorsReturn);

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
   }



SgAsmPowerPCInstruction*
PowerpcDisassembler::makeInstructionWithoutOperands(uint32_t address, const std::string& mnemonic, int condPos, PowerPC_InstructionKind kind, PowerPC_InstructionCondition cond, uint32_t insn)
   {

     SgAsmPowerpcInstruction* instruction = new SgAsmPowerpcInstruction(address, mnemonic, kind, cond, condPos);
     ROSE_ASSERT (instruction);

     SgAsmOperandList* operands = new SgAsmOperandList();
     instruction->set_operandList(operands);
     operands->set_parent(instruction);

  // PowerPC uses a fixed length instruction set (like ARM, but unlike x86)
     SgUnsignedCharList bytes(4, '\0');
     for (int i = 0; i < 4; ++i)
        {
          bytes[i] = (insn >> (8 * i)) & 0xFF; // Force little-endian
        }

     instruction->set_raw_bytes(bytes);

     return instruction;
   }

SgAsmPowerPC_RegisterReferenceExpression*
PowerpcDisassembler::makeRegister(uint8_t reg)
   {
     SgAsmPowerPC_RegisterReferenceExpression* r = new SgAsmPowerPC_RegisterReferenceExpression();

     r->set_powerpc_register_code((SgAsmPowerPC_RegisterReferenceExpression::powerpc_register_enum)(reg + 1));
     return r;
  }

SgAsmPowerpcInstruction*
PowerpcDisassembler::disassemble(const Parameters& p, const uint8_t* const insn, const uint64_t insnSize, size_t positionInVector, set<uint64_t>* knownSuccessorsReturn)
     {
       SingleInstructionDisassembler dis(p, insn, insnSize, positionInVector, knownSuccessorsReturn);

       return dis.disassemble();
     }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::disassemble()
    {
   // The Primary Opcode Field is bits 0-5, so there are max 64 primary opcode values
      uint8_t opcode;

   // This should clear the upper two bits of the byte (so we only evaluate the Primary Opcode Field)
      getByte(opcode);

      SgAsmPowerpcInstruction* instruction = NULL;

   // Handle all the different legal Primary Opcode values
      switch (opcode)
         {
           case 0x02: { decode_D_formInstruction(); break }
           case 0x03: { decode_D_formInstruction(); break }
           case 0x07: { decode_D_formInstruction(); break }
           case 0x08: { decode_D_formInstruction(); break }
           case 0x0A: { decode_D_formInstruction(); break }
           case 0x0B: { decode_D_formInstruction(); break }
           case 0x0C: { decode_D_formInstruction(); break }
           case 0x0D: { decode_D_formInstruction(); break }
           case 0x0E: { decode_D_formInstruction(); break }
           case 0x0F: { decode_D_formInstruction(); break }

           case 0x10: { decode_B_formInstruction(); break }

           case 0x11: { decode_SC_formInstruction(); break }

           case 0x12: { decode_I_formInstruction(); break }

           case 0x13: { decode_XL_formInstruction(); break }

        // 20
           case 0x14: { decode_M_formInstruction(); break }
           case 0x15: { decode_M_formInstruction(); break }
           case 0x17: { decode_M_formInstruction(); break }

        // 24
           case 0x18: { decode_D_formInstruction(); break }
           case 0x19: { decode_D_formInstruction(); break }
           case 0x1A: { decode_D_formInstruction(); break }
           case 0x1B: { decode_D_formInstruction(); break }
           case 0x1C: { decode_D_formInstruction(); break }
           case 0x1D: { decode_D_formInstruction(); break }

        // 30
           case 0x1E: { decode_MD_formInstruction(); break }

        // And so one until we handle all 64 opcodes!

           default:
              {
                printf ("Primary opcode not handled yet: opcode = %d \n",opcode);
                ROSE_ASSERT(false);
              }
         }

    return instruction;
  }


   
