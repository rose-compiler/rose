#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <fcntl.h>
#include <vector>
#include "rose.h"

using namespace std;
using namespace SageBuilderAsm;

#define DEBUG_OPCODES 0
#define DEBUG_BRANCH_LOGIC 0


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

          SgAsmPowerpcInstruction* disassemble(const uint8_t* const instructionList, size_t positionInVector);

       // There are 15 different forms of PowerPC instructions, but all are 32-bit (fixed length instruction set).
       // SgAsmPowerpcInstruction* decode_I_formInstruction(const uint8_t* const instructionList, size_t positionInVector);
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

          SgAsmQuadWordValueExpression* makeBranchTarget( uint64_t targetAddr ) const;
        };

     SgAsmPowerpcInstruction* makeInstructionWithoutOperands(uint64_t address, const std::string& mnemonic, PowerpcInstructionKind kind, uint32_t insn);

  // SgAsmPowerpcRegisterReferenceExpression* makeRegister(uint8_t reg);
     SgAsmPowerpcRegisterReferenceExpression* makeRegister(PowerpcRegisterClass reg_class, int reg_number, PowerpcConditionRegisterAccessGranularity reg_grainularity = powerpc_condreggranularity_whole );

  // SgAsmPowerpcInstruction* disassemble(const Parameters& p, const uint8_t* const insn, const uint64_t insnSize, size_t positionInVector, set<uint64_t>* knownSuccessorsReturn);
  // SgAsmPowerpcInstruction* disassemble(const Parameters& p, const uint32_t* const insn, const uint64_t insnSize, size_t positionInVector, set<uint64_t>* knownSuccessorsReturn);
  // SgAsmPowerpcInstruction* disassemble(const Parameters& p, const uint32_t insn, const uint64_t insnSize, size_t positionInVector, set<uint64_t>* knownSuccessorsReturn);
     SgAsmPowerpcInstruction* disassemble(const Parameters& p, const uint8_t* const instructionList, const uint64_t insnSize, size_t positionInVector, set<uint64_t>* knownSuccessorsReturn);

// end of namespace: PowerpcDisassembler
   }

#define MAKE_INSN0(Mne) (makeInstructionWithoutOperands(p.ip, #Mne, powerpc_##Mne, insn))
#define MAKE_INSN1(Mne, Op1) (appendOperand(MAKE_INSN0(Mne), (Op1)))
#define MAKE_INSN2(Mne, Op1, Op2) (appendOperand(MAKE_INSN1(Mne, Op1), (Op2)))
#define MAKE_INSN3(Mne, Op1, Op2, Op3) (appendOperand(MAKE_INSN2(Mne, Op1, Op2), (Op3)))
#define MAKE_INSN4(Mne, Op1, Op2, Op3, Op4) (appendOperand(MAKE_INSN3(Mne, Op1, Op2, Op3), (Op4)))
#define MAKE_INSN5(Mne, Op1, Op2, Op3, Op4, Op5) (appendOperand(MAKE_INSN4(Mne, Op1, Op2, Op3, Op4), (Op5)))

SgAsmQuadWordValueExpression*
PowerpcDisassembler::SingleInstructionDisassembler::makeBranchTarget ( uint64_t targetAddr ) const
   {
  // int32_t val = insn & 0xFFFFFF;
  // val <<= 8;
  // val >>= 6; // Arithmetic shift to copy highest bit of immediate
#if 0
     uint32_t val_temp = (LI >= (1U << 23)) ? LI - (1U << 24) : LI;
     val_temp <<= 2;

     uint32_t targetAddr = p.ip + val_temp;
#endif

     if (knownSuccessorsReturn) knownSuccessorsReturn->insert(targetAddr);
     return makeQWordValue(targetAddr);
   }

bool
PowerpcDisassembler::doesBBStartFunction(SgAsmBlock* bb, bool use64bit)
   {
  // DQ (10/14/2008): Provide a default implementation for now!
     return false;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::makeInstructionWithoutOperands(uint64_t address, const std::string& mnemonic, PowerpcInstructionKind kind, uint32_t insn)
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
PowerpcDisassembler::disassemble(const Parameters& p, const uint8_t* const instructionList, const uint64_t insnSize, size_t positionInVector, set<uint64_t>* knownSuccessorsReturn)
   {
  // printf ("Inside of PowerpcDisassembler::disassemble(): instructionList = %p insnSize = %zu positionInVector = %zu \n",instructionList,insnSize,positionInVector);

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

  // printf ("Single instruction opcode = 0x%x (calling disassembler) \n",c);

  // Added this to get pass zero instruction (padding?)
     if (c == 0)
        {
#if DEBUG_OPCODES
          printf ("####### In PowerpcDisassembler::disassemble(): Return NULL pointer for case of c == 0 ####### \n");
#endif
          throw BadInstruction();
        }

     SingleInstructionDisassembler sid(p, c, knownSuccessorsReturn);
     return sid.disassemble(instructionList,positionInVector);
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::disassemble(const uint8_t* const instructionList, size_t positionInVector)
   {
  // The Primary Opcode Field is bits 0-5, 6-bits wide, so there are max 64 primary opcode values
     uint8_t primaryOpcode = (insn >> 26) & 0x3F;

  // printf ("instruction opcode = 0x%x primaryOpcode = 0x%x \n",insn,primaryOpcode);

  // This should not happend on a PowerPC system
     ROSE_ASSERT(insn != 0);

  // This should clear the upper two bits of the byte (so we only evaluate the Primary Opcode Field)
  // getByte(opcode);

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
#if 0
          case 0x00:
          case 0x01: { /* illegal instruction */ ROSE_ASSERT(false); break; }
#endif
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
       // case 0x12: { instruction = decode_I_formInstruction(instructionList,positionInVector); break; }
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
           case 0x1F:
             {
            // Depending on the extended Opcode this is an X form, XO form, XFX form,
            // and XS form, the extended opCode maps to different bit ranges.

               uint16_t x_Opcode   = (insn >> 1) & 0x3FF;
               uint16_t xfx_Opcode = x_Opcode;
            // uint16_t xl_Opcode  = x_Opcode;
            // uint16_t xfl_Opcode = x_Opcode;
               uint16_t xs_Opcode  = (insn >> 2) & 0x1FF;
               uint16_t xo_Opcode  = (insn >> 1) & 0x1FF;

            // printf ("x_Opcode = 0x%x = %d xfx_Opcode = 0x%x = %d xs_Opcode = 0x%x = %d xo_Opcode = 0x%x = %d \n",x_Opcode,x_Opcode,xfx_Opcode,xfx_Opcode,xs_Opcode,xs_Opcode,xo_Opcode,xo_Opcode);

            // Different parts of the instruction are used to identify what kind of instruction this is!
               if (  xfx_Opcode == 19 || xfx_Opcode == 144 || xfx_Opcode == 339 || xfx_Opcode == 371 || xfx_Opcode == 467 )
                  {
                    instruction = decode_XFX_formInstruction();
                  }
                 else
                  {
                    if ( (xo_Opcode >= 8 && xo_Opcode <= 11) || xo_Opcode == 40 || xo_Opcode == 73 || xo_Opcode == 75 || 
                         xo_Opcode == 104 || xo_Opcode == 136 || xo_Opcode == 138 || xo_Opcode == 200 || xo_Opcode == 202 || 
                         (xo_Opcode >= 232 && xo_Opcode <= 235) || xo_Opcode == 266 || xo_Opcode == 457 || xo_Opcode == 459 || 
                         xo_Opcode == 489 || xo_Opcode == 491 )
                       {
                         instruction = decode_XO_formInstruction();
                       }
                      else
                       {
                         if ( xs_Opcode == 413 )
                            {
                              instruction = decode_XS_formInstruction();
                            }
                           else
                            {
                              instruction = decode_X_formInstruction();
                            }
                       }
                  }

               ROSE_ASSERT(instruction != NULL);
               break;
             }

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
               uint16_t x_Opcode   = (insn >> 1) & 0x3FF;
               uint16_t xfl_Opcode = x_Opcode;
               uint8_t  a_Opcode   = (insn >> 1) & 0x1F;

            // printf ("x_Opcode = 0x%x = %d xfl_Opcode = 0x%x = %d a_Opcode = 0x%x = %d \n",x_Opcode,x_Opcode,xfl_Opcode,xfl_Opcode,a_Opcode,a_Opcode);

            // Different parts of the instruction are used to identify what kind of instruction this is!
               if (  a_Opcode == 18 || (a_Opcode >= 20 && a_Opcode <= 31) )
                  {
                    instruction = decode_A_formInstruction();
                  }
                 else
                  {
                    if ( xfl_Opcode == 711 )
                       {
                         instruction = decode_XFL_formInstruction();
                       }
                      else
                       {
                         instruction = decode_X_formInstruction();
                       }
                  }

               ROSE_ASSERT(instruction != NULL);
               break;
             }

          default:
             {
            // The default case is now used for handling illegal instructions 
            // (during development it was for those not yet implemented).
               printf ("Primary opcode not handled yet: primaryOpcode = %d \n",primaryOpcode);
            // ROSE_ASSERT(false);
               throw BadInstruction();
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

  // Get bits 6-8, 3 bits as the BF opcode
     uint32_t liOpcode = (insn >> 2) & 0xFFFFFF;

  // Get bit 30, 1 bit as the reserved flag
     uint8_t aaOpcode = (insn >> 1) & 0x1;

  // Get bit 31, 1 bit as the reserved flag
     uint8_t lkOpcode = (insn >> 0) & 0x1;

#if DEBUG_OPCODES
     printf ("I-Form instruction opcode = 0x%x liOpcode = 0x%x aaOpcode = 0x%x lkOpcode = 0x%x \n",insn,liOpcode,aaOpcode,lkOpcode);
#endif

     switch(primaryOpcode)
        {
          case 0x12:
             {
            // The address should be computed for the knownSuccessorsReturn set, but not computed for use 
            // in the instuction (else we would be mixing the semantic analysis into the disassembly).
            // SgAsmExpression* targetAddress = new SgAsmDoubleWordValueExpression(liOpcode);

            // DQ (10/15/2008): Compute the address of the branch to restart the disassembly.
            // The address is LI || 0b00 sign-extended.
               uint32_t computedAddress = (liOpcode >= (1U << 23)) ? liOpcode - (1U << 24) : liOpcode;
               computedAddress <<= 2;

            // uint32_t targetAddr = p.ip + val_temp;
               uint32_t targetBranchAddress = computedAddress;

               if (aaOpcode == 0)
                  {
                 // In this case the address is the sum of LI || 0b00 sign-extended AND "the address of this instruction".
                    targetBranchAddress += p.ip;
                  }

               SgAsmQuadWordValueExpression* targetAddressExpression = makeBranchTarget(targetBranchAddress);
               ROSE_ASSERT(knownSuccessorsReturn->empty() == false);

               if (aaOpcode == 0)
                  {
                    if (lkOpcode == 0)
                       {
                         instruction = MAKE_INSN1(b,targetAddressExpression);
                       }
                      else
                       {
                         instruction = MAKE_INSN1(bl,targetAddressExpression);
                       }
                  }
                 else
                  {
                    if (lkOpcode == 0)
                       {
                         instruction = MAKE_INSN1(ba,targetAddressExpression);
                       }
                      else
                       {
                         instruction = MAKE_INSN1(bla,targetAddressExpression);
                       }
                  }

#if DEBUG_BRANCH_LOGIC
               printf ("Computation of targetAddressExpression = 0x%x \n",targetAddressExpression->get_value());
#endif
               break;
             }

          default:
             {
            // The default case is now used for handling illegal instructions 
            // (during development it was for those not yet implemented).
               printf ("Error: I-Form primaryOpcode = %d (illegal instruction) \n",primaryOpcode);
               ROSE_ASSERT(false);
             }
        }

     if (lkOpcode == 1)
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

#if DEBUG_OPCODES
     printf ("B-Form instruction opcode = 0x%x boOpcode = 0x%x biOpcode = 0x%x bdOpcode = 0x%x aaOpcode = 0x%x lkOpcode = 0x%x \n",insn,boOpcode,biOpcode,bdOpcode,aaOpcode,lkOpcode);
#endif

     switch(primaryOpcode)
        {
          case 0x10:
             {
            // Skip the registers for now!
               SgAsmExpression* BO = new SgAsmByteValueExpression(boOpcode);
               SgAsmExpression* BI = makeRegister(powerpc_regclass_gpr,biOpcode,powerpc_condreggranularity_bit);

            // DQ (10/15/2008): Compute the address of the branch to restart the disassembly.
            // The address is BD || 0b00 sign-extended.
               uint64_t computedAddress = (bdOpcode >= (1U << 13)) ? bdOpcode - (1U << 14) : bdOpcode;
               computedAddress <<= 2;

               uint32_t targetBranchAddress = computedAddress;

               if (aaOpcode == 0)
                  {
                 // In this case the address is the sum of BD || 0b00 sign-extended AND "the address of this instruction".
                    targetBranchAddress += p.ip;
                  }

               SgAsmQuadWordValueExpression* targetAddressExpression = makeBranchTarget(targetBranchAddress);
               ROSE_ASSERT(knownSuccessorsReturn->empty() == false);

               if (lkOpcode == 0)
                  {
                    if (aaOpcode == 0)
                       {
                         instruction = MAKE_INSN3(bc,BO,BI,targetAddressExpression);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(bca,BO,BI,targetAddressExpression);
                       }
                  }
                 else
                  {
                    if (aaOpcode == 0)
                       {
                         instruction = MAKE_INSN3(bcl,BO,BI,targetAddressExpression);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(bcla,BO,BI,targetAddressExpression);
                       }
                  }

               break;
             }

          default:
             {
            // The default case is now used for handling illegal instructions 
            // (during development it was for those not yet implemented).
               printf ("Error: B-Form primaryOpcode = %d not handled! \n",primaryOpcode);
               ROSE_ASSERT(false);
             }
        }

     if (lkOpcode == 1)
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

  // Get the bits 6-10, next 5 bits, as the secondary opcode: RT, RS, TO, FRT, FRS
     uint8_t levOpcode = (insn >> 5) & 0x7F;

  // Get bit 30, 1 bit as the reserved flag
     uint8_t constantOneOpcode = (insn >> 1) & 0x1;
     ROSE_ASSERT(constantOneOpcode == 1);

#if DEBUG_OPCODES
     printf ("SC-Form instruction opcode = 0x%x levOpcode = 0x%x constantOneOpcode = 0x%x \n",insn,levOpcode,constantOneOpcode);
#endif

     SgAsmExpression* LEV = new SgAsmWordValueExpression(levOpcode);
     instruction = MAKE_INSN1(sc,LEV);

     ROSE_ASSERT(instruction != NULL);
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
     uint8_t rtOpcode  = (insn >> 21) & 0x1F;
     uint8_t toOpcode  = rtOpcode;

  // Get the bits 11-15, next 5 bits, as the tertiary opcode
     uint8_t raOpcode = (insn >> 16) & 0x1F;

  // Get bits 6-8, 3 bits as the BF opcode
     uint8_t bfOpcode = (insn >> 21) & 0x8;

  // Get bit 9, 1 bit as the reserved flag
  // uint8_t reservedOpcode = (insn >> 24) & 0x1;

  // Get bit 10, 1 bit as the length flag
     uint8_t lengthOpcode = (insn >> 25) & 0x1;

  // Get the bits 16-31, next 16 bits, as the tertiary opcode
     uint8_t lastOpcode = (insn >> 0) & 0xFFFF;

#if DEBUG_OPCODES
  // printf ("D-Form instruction opcode = 0x%x secodaryOpcode = 0x%x bfOpcode = 0x%x reservedOpcode = 0x%x lengthOpcode = 0x%x raOpcode = 0x%x lastOpcode 0x%x \n",insn,secodaryOpcode,bfOpcode,reservedOpcode,lengthOpcode,raOpcode,lastOpcode);
     printf ("D-Form instruction opcode = 0x%x secodaryOpcode = 0x%x bfOpcode = 0x%x lengthOpcode = 0x%x raOpcode = 0x%x lastOpcode 0x%x \n",insn,secodaryOpcode,bfOpcode,lengthOpcode,raOpcode,lastOpcode);
#endif

     switch(primaryOpcode)
        {
       // 2
          case 0x2:
             {
               SgAsmExpression* TO = makeRegister(powerpc_regclass_gpr,toOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* SI = new SgAsmWordValueExpression(lastOpcode);
               instruction = MAKE_INSN3(tdi,TO,RA,SI);
               break;
             }

       // 3
          case 0x3:
             {
               SgAsmExpression* TO = makeRegister(powerpc_regclass_gpr,toOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* SI = new SgAsmWordValueExpression(lastOpcode);
               instruction = MAKE_INSN3(twi,TO,RA,SI);
               break;
             }

       // 4-6
          case 0x4:
          case 0x5:
          case 0x6: { /* illeagal instruction */ ROSE_ASSERT(false); break; }

       // 7
          case 0x7:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* SI = new SgAsmWordValueExpression(lastOpcode);
               instruction = MAKE_INSN3(mulli,RT,RA,SI);
               break;
             }

       // 8
          case 0x8:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* SI = new SgAsmWordValueExpression(lastOpcode);
               instruction = MAKE_INSN3(subfic,RT,RA,SI);
               break;
             }

       // 9
          case 0x9: { /* illeagal instruction */ ROSE_ASSERT(false); break; }

       // 10
          case 0x0A:
             {
               SgAsmExpression* BF = new SgAsmByteValueExpression(bfOpcode);
               SgAsmExpression* L  = new SgAsmByteValueExpression(lengthOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* UI = new SgAsmWordValueExpression(lastOpcode);
               instruction = MAKE_INSN4(cmpl,BF,L,RA,UI);
               break;
             }

       // 11
          case 0x0B:
             {
               SgAsmExpression* BF = new SgAsmByteValueExpression(bfOpcode);
               SgAsmExpression* L  = new SgAsmByteValueExpression(lengthOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* SI = new SgAsmWordValueExpression(lastOpcode);
               instruction = MAKE_INSN4(cmpi,BF,L,RA,SI);
               break;
             }

       // 12
          case 0x0C:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* SI = new SgAsmWordValueExpression(lastOpcode);
               instruction = MAKE_INSN3(addic,RT,RA,SI);
               break;
             }

       // 13
          case 0x0D:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* SI = new SgAsmWordValueExpression(lastOpcode);
               instruction = MAKE_INSN3(addic_record,RT,RA,SI);
               break;
             }

       // 14
          case 0x0E:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* SI = new SgAsmWordValueExpression(lastOpcode);
            // SgAsmExpression* addressExpr = makeAdd(RA,D);
            // SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
            // instruction = MAKE_INSN2(addi,RT,mr);
               instruction = MAKE_INSN3(addi,RT,RA,SI);
               break;
             }

       // 15
          case 0x0F:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* SI = new SgAsmWordValueExpression(lastOpcode);
               instruction = MAKE_INSN3(addis,RT,RA,SI);
               break;
             }

       // 24
          case 0x18:
             {
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* UI = new SgAsmWordValueExpression(lastOpcode);
               instruction = MAKE_INSN3(ori,RS,RA,UI);
               break;
             }

       // 25
          case 0x19:
             {
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* UI = new SgAsmWordValueExpression(lastOpcode);
               instruction = MAKE_INSN3(oris,RS,RA,UI);
               break;
             }

       // 26
          case 0x1A:
             {
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* UI = new SgAsmWordValueExpression(lastOpcode);
               instruction = MAKE_INSN3(xori,RS,RA,UI);
               break;
             }

       // 27
          case 0x1B:
             {
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* UI = new SgAsmWordValueExpression(lastOpcode);
               instruction = MAKE_INSN3(xoris,RS,RA,UI);
               break;
             }

       // 28
          case 0x1C:
             {
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* UI = new SgAsmWordValueExpression(lastOpcode);
               instruction = MAKE_INSN3(andi_record,RS,RA,UI);
               break;
             }

       // 29
          case 0x1D:
             {
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* UI = new SgAsmWordValueExpression(lastOpcode);
               instruction = MAKE_INSN3(andis_record,RS,RA,UI);
               break;
             }

       // 32
          case 0x20:
             {
            // The correct form of this instruction is "xxx RT,D(RA)", so maybe we need a more elaborate way to form "D(RA)" explicitly, perhaps as "(RA) + D".
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* D = new SgAsmWordValueExpression(lastOpcode);
               SgAsmExpression* addressExpr = makeAdd(RA,D);
               SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
               instruction = MAKE_INSN2(lwz,RT,mr);
               break;
             }

       // 33
          case 0x21:
             {
            // The correct form of this instruction is "xxx RT,D(RA)", so maybe we need a more elaborate way to form "D(RA)" explicitly, perhaps as "(RA) + D".
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* D = new SgAsmWordValueExpression(lastOpcode);
               SgAsmExpression* addressExpr = makeAdd(RA,D);
               SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
               instruction = MAKE_INSN2(lwzu,RT,mr);
               break;
             }

       // 34
          case 0x22:
             {
            // The correct form of this instruction is "xxx RT,D(RA)", so maybe we need a more elaborate way to form "D(RA)" explicitly, perhaps as "(RA) + D".
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* D = new SgAsmWordValueExpression(lastOpcode);
               SgAsmExpression* addressExpr = makeAdd(RA,D);
               SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
               instruction = MAKE_INSN2(lbz,RT,mr);
               break;
             }

       // 35
          case 0x23:
             {
            // The correct form of this instruction is "xxx RT,D(RA)", so maybe we need a more elaborate way to form "D(RA)" explicitly, perhaps as "(RA) + D".
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* D = new SgAsmWordValueExpression(lastOpcode);
               SgAsmExpression* addressExpr = makeAdd(RA,D);
               SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
               instruction = MAKE_INSN2(lbzu,RT,mr);
               break;
             }

       // 36
          case 0x24:
             {
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* D = new SgAsmWordValueExpression(lastOpcode);
               SgAsmExpression* addressExpr = makeAdd(RA,D);
               SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
               instruction = MAKE_INSN2(stw,RS,mr);
               break;
             }

       // 37
          case 0x25:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* D = new SgAsmWordValueExpression(lastOpcode);
               SgAsmExpression* addressExpr = makeAdd(RA,D);
               SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
               instruction = MAKE_INSN2(stwu,RT,mr);
               break;
             }

       // 38
          case 0x26:
             {
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* D = new SgAsmWordValueExpression(lastOpcode);
               SgAsmExpression* addressExpr = makeAdd(RA,D);
               SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
               instruction = MAKE_INSN2(stb,RS,mr);
               break;
             }

       // 39
          case 0x27:
             {
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* D = new SgAsmWordValueExpression(lastOpcode);
               SgAsmExpression* addressExpr = makeAdd(RA,D);
               SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
               instruction = MAKE_INSN2(stbu,RS,mr);
               break;
             }

       // 40
          case 0x28:
             {
            // The correct form of this instruction is "xxx RT,D(RA)", so maybe we need a more elaborate way to form "D(RA)" explicitly, perhaps as "(RA) + D".
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* D = new SgAsmWordValueExpression(lastOpcode);
               SgAsmExpression* addressExpr = makeAdd(RA,D);
               SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
               instruction = MAKE_INSN2(lhz,RT,mr);
               break;
             }

       // 41
          case 0x29:
             {
            // The correct form of this instruction is "xxx RT,D(RA)", so maybe we need a more elaborate way to form "D(RA)" explicitly, perhaps as "(RA) + D".
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* D = new SgAsmWordValueExpression(lastOpcode);
               SgAsmExpression* addressExpr = makeAdd(RA,D);
               SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
               instruction = MAKE_INSN2(lhzu,RT,mr);
               break;
             }

       // 42
          case 0x2A:
             {
            // The correct form of this instruction is "xxx RT,D(RA)", so maybe we need a more elaborate way to form "D(RA)" explicitly, perhaps as "(RA) + D".
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* D = new SgAsmWordValueExpression(lastOpcode);
               SgAsmExpression* addressExpr = makeAdd(RA,D);
               SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
               instruction = MAKE_INSN2(lha,RT,mr);
               break;
             }

       // 43
          case 0x2B:
             {
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* D = new SgAsmWordValueExpression(lastOpcode);
               SgAsmExpression* addressExpr = makeAdd(RA,D);
               SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
               instruction = MAKE_INSN2(sth,RS,mr);
               break;
             }

       // 44
          case 0x2C:
             {
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* D = new SgAsmWordValueExpression(lastOpcode);
               SgAsmExpression* addressExpr = makeAdd(RA,D);
               SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
               instruction = MAKE_INSN2(sth,RS,mr);
               break;
             }

       // 45
          case 0x2D:
             {
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* D = new SgAsmWordValueExpression(lastOpcode);
               SgAsmExpression* addressExpr = makeAdd(RA,D);
               SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
               instruction = MAKE_INSN2(sthu,RS,mr);
               break;
             }

       // 46
          case 0x2E:
             {
            // The correct form of this instruction is "xxx RT,D(RA)", so maybe we need a more elaborate way to form "D(RA)" explicitly, perhaps as "(RA) + D".
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* D = new SgAsmWordValueExpression(lastOpcode);
               SgAsmExpression* addressExpr = makeAdd(RA,D);
               SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
               instruction = MAKE_INSN2(lmw,RT,mr);
               break;
             }

       // 47
          case 0x2F:
             {
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* D = new SgAsmWordValueExpression(lastOpcode);
               SgAsmExpression* addressExpr = makeAdd(RA,D);
               SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
               instruction = MAKE_INSN2(stmw,RS,mr);
               break;
             }

       // 48
          case 0x30:
             {
               SgAsmExpression* FRT = makeRegister(powerpc_regclass_fpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* D = new SgAsmWordValueExpression(lastOpcode);
               SgAsmExpression* addressExpr = makeAdd(RA,D);
               SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
               instruction = MAKE_INSN2(lfs,FRT,mr);
               break;
             }

       // 49
          case 0x31:
             {
               SgAsmExpression* FRT = makeRegister(powerpc_regclass_fpr,secodaryOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* D = new SgAsmWordValueExpression(lastOpcode);
               SgAsmExpression* addressExpr = makeAdd(RA,D);
               SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
               instruction = MAKE_INSN2(lfsu,FRT,mr);
               break;
             }

       // 50
          case 0x32:
             {
            // The correct form of this instruction is "xxx FRS,D(RA)", so maybe we need a more elaborate way to form "D(RA)" explicitly, perhaps as "(RA) + D".
               SgAsmExpression* FRT = makeRegister(powerpc_regclass_fpr,secodaryOpcode);
               SgAsmExpression* RA  = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* D   = new SgAsmWordValueExpression(lastOpcode);
               SgAsmExpression* addressExpr = makeAdd(RA,D);
               SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
               instruction = MAKE_INSN2(lfd,FRT,mr);
               break;
             }

       // 52
          case 0x34:
             {
            // The correct form of this instruction is "xxx FRS,D(RA)", so maybe we need a more elaborate way to form "D(RA)" explicitly, perhaps as "(RA) + D".
               SgAsmExpression* FRS = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA  = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* D   = new SgAsmWordValueExpression(lastOpcode);
               SgAsmExpression* addressExpr = makeAdd(RA,D);
               SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
               instruction = MAKE_INSN2(stfs,FRS,mr);
               break;
             }

       // 53
          case 0x35:
             {
            // The correct form of this instruction is "xxx FRS,D(RA)", so maybe we need a more elaborate way to form "D(RA)" explicitly, perhaps as "(RA) + D".
               SgAsmExpression* FRS = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA  = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* D   = new SgAsmWordValueExpression(lastOpcode);
               SgAsmExpression* addressExpr = makeAdd(RA,D);
               SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
               instruction = MAKE_INSN2(stfsu,FRS,mr);
               break;
             }

       // 54
          case 0x36:
             {
            // The correct form of this instruction is "xxx FRS,D(RA)", so maybe we need a more elaborate way to form "D(RA)" explicitly, perhaps as "(RA) + D".
               SgAsmExpression* FRS = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA  = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* D   = new SgAsmWordValueExpression(lastOpcode);
               SgAsmExpression* addressExpr = makeAdd(RA,D);
               SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
               instruction = MAKE_INSN2(stfd,FRS,mr);
               break;
             }

       // 55
          case 0x37:
             {
            // The correct form of this instruction is "xxx FRS,D(RA)", so maybe we need a more elaborate way to form "D(RA)" explicitly, perhaps as "(RA) + D".
               SgAsmExpression* FRS = makeRegister(powerpc_regclass_gpr,secodaryOpcode);
               SgAsmExpression* RA  = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* D   = new SgAsmWordValueExpression(lastOpcode);
               SgAsmExpression* addressExpr = makeAdd(RA,D);
               SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr,NULL);
               instruction = MAKE_INSN2(stfdu,FRS,mr);
               break;
             }

          default:
             {
            // The default case is now used for handling illegal instructions 
            // (during development it was for those not yet implemented).
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

     printf ("DS-Form instructions not implemented yet, I have no examples so far! \n");

     ROSE_ASSERT(instruction != NULL);
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_X_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;

  // The primaryOpcode 
     uint8_t primaryOpcode = (insn >> 26) & 0x3F;

  // X-form instructions can be associated with only 2 primary opcodes.
     ROSE_ASSERT(primaryOpcode == 0x1F || primaryOpcode == 0x3F);

  // Get the bits 6-10, next 5 bits
     uint8_t rtOpcode  = (insn >> 21) & 0x1F;
     uint8_t rsOpcode  = rtOpcode;
     uint8_t btOpcode  = rtOpcode;

     uint8_t bfOpcode  = (rtOpcode >> 2) & 0x7;
     uint8_t lOpcode   = (rtOpcode >> 0) & 0x1;

  // Values currently commented out are expected to be used later!
  // uint8_t toOpcode  = rtOpcode;
     uint8_t frtOpcode = rtOpcode;
  // uint8_t frsOpcode = rtOpcode;
  // uint8_t boOpcode  = rtOpcode;

  // Values currently commented out are expected to be used later!
  // Get the bits 11-15, next 5 bits
     uint8_t raOpcode  = (insn >> 16) & 0x1F;
     uint8_t fraOpcode = raOpcode;

  // Get the bits 16-20, next 5 bits, as the secondary opcode: RT, RS, TO, FRT, FRS
     uint8_t rbOpcode  = (insn >> 11) & 0x1F;
  // uint8_t nbOpcode  = rbOpcode;
     uint8_t shOpcode  = rbOpcode;
     uint8_t frbOpcode = rbOpcode;

  // Get the bits 21-30, next 10 bits
     uint16_t xoOpcode = (insn >> 1) & 0x3FF;

  // Get bit 31, 1 bit as the link bit
     uint8_t rcOpcode = (insn >> 0) & 0x1;

#if DEBUG_OPCODES
     printf ("X-Form instruction opcode = 0x%x xoOpcode = 0x%x \n",insn,xoOpcode);
#endif

     switch(xoOpcode)
        {
       // 0
          case 0x0:
             {
               if (primaryOpcode == 0x1F)
                  {
                    SgAsmExpression* BF = makeRegister(powerpc_regclass_gpr,bfOpcode);
                    SgAsmExpression* L  = new SgAsmByteValueExpression(lOpcode);
                    SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
                    SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
                    instruction = MAKE_INSN4(cmp,BF,L,RA,RB);
                  }
                 else
                  {
                    ROSE_ASSERT(primaryOpcode == 0x3F);
                    SgAsmExpression* BF = makeRegister(powerpc_regclass_gpr,bfOpcode,powerpc_condreggranularity_bit);
                    SgAsmExpression* FRA = makeRegister(powerpc_regclass_fpr,fraOpcode);
                    SgAsmExpression* FRB = makeRegister(powerpc_regclass_fpr,frbOpcode);
                    instruction = MAKE_INSN3(fcmpu,BF,FRA,FRB);
                  }
               break;
             }

       // 21
          case 0x15:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(ldx,RT,RA,RB);
               break;
             }

       // 23
          case 0x17:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(lwzx,RT,RA,RB);
               break;
             }

       // 24
          case 0x18:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN3(slw,RA,RS,RB);
                  }
                 else
                  {
                    instruction = MAKE_INSN3(slw_record,RA,RS,RB);
                  }
               break;
             }

       // 26
          case 0x1A:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN2(cntlzw,RA,RS);
                  }
                 else
                  {
                    instruction = MAKE_INSN2(cntlzw_record,RA,RS);
                  }
               break;
             }

       // 27
          case 0x1B:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN3(sld,RA,RS,RB);
                  }
                 else
                  {
                    instruction = MAKE_INSN3(sld_record,RA,RS,RB);
                  }
               break;
             }

       // 28
          case 0x1C:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN3(and,RA,RS,RB);
                  }
                 else
                  {
                    instruction = MAKE_INSN3(and_record,RA,RS,RB);
                  }
               break;
             }

       // 32
          case 0x20:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* BF = makeRegister(powerpc_regclass_gpr,bfOpcode);
               SgAsmExpression* L  = new SgAsmByteValueExpression(lOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN4(cmpl,BF,L,RA,RB);
               break;
             }

       // 38
          case 0x26:
             {
               ROSE_ASSERT(primaryOpcode == 0x3F);
               SgAsmExpression* BT = new SgAsmByteValueExpression(btOpcode);
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN1(mtfsb1,BT);
                  }
                 else
                  {
                    instruction = MAKE_INSN1(mtfsb1_record,BT);
                  }
               break;
             }

       // 53
          case 0x35:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(ldux,RT,RA,RB);
               break;
             }

       // 55
          case 0x37:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(lwzux,RT,RA,RB);
               break;
             }

       // 60
          case 0x3C:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN3(andc,RA,RS,RB);
                  }
                 else
                  {
                    instruction = MAKE_INSN3(andc_record,RA,RS,RB);
                  }
               break;
             }

       // 70
          case 0x46:
             {
               ROSE_ASSERT(primaryOpcode == 0x3F);
               SgAsmExpression* BT = new SgAsmByteValueExpression(btOpcode);
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN1(mtfsb0,BT);
                  }
                 else
                  {
                    instruction = MAKE_INSN1(mtfsb0_record,BT);
                  }
               break;
             }

       // 83
          case 0x53:
             {
            // This is a privileged instruction (documented in Book III)!
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               instruction = MAKE_INSN1(mfmsr,RT);
               break;
             }

       // 87
          case 0x57:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(lbzx,RT,RA,RB);
               break;
             }

       // 119
          case 0x77:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(lbzux,RT,RA,RB);
               break;
             }

       // 124
          case 0x7C:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN3(nor,RA,RS,RB);
                  }
                 else
                  {
                    instruction = MAKE_INSN3(nor_record,RA,RS,RB);
                  }
               break;
             }

       // 149
          case 0x95:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(stdx,RS,RA,RB);
               break;
             }

       // 151
          case 0x97:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(stwx,RS,RA,RB);
               break;
             }

       // 181
          case 0xB5:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(stdux,RS,RA,RB);
               break;
             }

       // 183
          case 0xB7:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(stwux,RS,RA,RB);
               break;
             }

       // 188
          case 0xBC: // 188
          case 0xD3: // 211
             {
            // This is a illegal instruction which I will translate to NOP (in PowerPC: nop -> ori for now!

               printf ("Processing an illegal instruction (generating a NOP)! \n");
               ROSE_ASSERT(false);
#if 0
               SgAsmExpression* R0a = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* R0b = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* zero = new SgAsmByteValueExpression(0);
               instruction = MAKE_INSN3(ori,R0a,R0b,zero);
#endif
               break;
             }

       // 215
          case 0xD7:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(stbx,RS,RA,RB);
               break;
             }

       // 247
          case 0xF7:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(stbux,RS,RA,RB);
               break;
             }

       // 278
          case 0x116:
             {
            // This is an instruction from Book II (data cache block touch)
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN2(dcbt,RA,RB);
               break;
             }

       // 279
          case 0x117:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(lhzx,RT,RA,RB);
               break;
             }

       // 284
          case 0x11C:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN3(eqv,RA,RS,RB);
                  }
                 else
                  {
                    instruction = MAKE_INSN3(eqv_record,RA,RS,RB);
                  }
               break;
             }

       // 311
          case 0x137:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(lhzux,RT,RA,RB);
               break;
             }

       // 316
          case 0x13C:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN3(xor,RA,RS,RB);
                  }
                 else
                  {
                    instruction = MAKE_INSN3(xor_record,RA,RS,RB);
                  }
               break;
             }

       // 341
          case 0x155:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(lwax,RT,RA,RB);
               break;
             }

       // 343
          case 0x157:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(lhax,RT,RA,RB);
               break;
             }

       // 373
          case 0x175:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(lwaux,RT,RA,RB);
               break;
             }

       // 375
          case 0x177:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(lhaux,RT,RA,RB);
               break;
             }

       // 407
          case 0x197:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(sthx,RS,RA,RB);
               break;
             }

       // 412
          case 0x19C:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN3(orc,RA,RS,RB);
                  }
                 else
                  {
                    instruction = MAKE_INSN3(orc_record,RA,RS,RB);
                  }
               break;
             }

       // 439
          case 0x1B7:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(sthux,RS,RA,RB);
               break;
             }

       // 444
          case 0x1BC:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN3(or,RA,RS,RB);
                  }
                 else
                  {
                    instruction = MAKE_INSN3(or_record,RA,RS,RB);
                  }
               break;
             }

       // 476
          case 0x1DC:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN3(nand,RA,RS,RB);
                  }
                 else
                  {
                    instruction = MAKE_INSN3(nand_record,RA,RS,RB);
                  }
               break;
             }

       // 533
          case 0x215:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(lswx,RT,RA,RB);
               break;
             }

       // 534
          case 0x216:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(lwbrx,RT,RA,RB);
               break;
             }

       // 536
          case 0x218:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN3(srw,RA,RS,RB);
                  }
                 else
                  {
                    instruction = MAKE_INSN3(srw_record,RA,RS,RB);
                  }
               break;
             }

       // 539
          case 0x21B:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN3(srd,RA,RS,RB);
                  }
                 else
                  {
                    instruction = MAKE_INSN3(srd_record,RA,RS,RB);
                  }
               break;
             }

       // 583
          case 0x247:
             {
               ROSE_ASSERT(primaryOpcode == 0x3F);
               SgAsmExpression* FRT = makeRegister(powerpc_regclass_fpr,frtOpcode,powerpc_condreggranularity_whole);
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN1(mffs,FRT);
                  }
                 else
                  {
                    instruction = MAKE_INSN1(mffs_record,FRT);
                  }
               break;
             }

       // 597
          case 0x255:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(lswx,RT,RA,RB);
               break;
             }

       // 661
          case 0x295:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(stswx,RS,RA,RB);
               break;
             }

       // 662
          case 0x296:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(stwbrx,RS,RA,RB);
               break;
             }

       // 725
           case 0x2D5:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(stswi,RS,RA,RB);
               break;
             }

       // 790
          case 0x316:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(lhbrx,RT,RA,RB);
               break;
             }

       // 792
          case 0x318:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* SH = new SgAsmByteValueExpression(shOpcode);
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN3(srad,RA,RS,SH);
                  }
                 else
                  {
                    instruction = MAKE_INSN3(srad_record,RA,RS,SH);
                  }
               break;
             }

       // 794
          case 0x31A:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* SH = new SgAsmByteValueExpression(shOpcode);
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN3(sraw,RA,RS,SH);
                  }
                 else
                  {
                    instruction = MAKE_INSN3(sraw_record,RA,RS,SH);
                  }
               break;
             }

       // 824
          case 0x338:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* SH = new SgAsmByteValueExpression(shOpcode);
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN3(srawi,RA,RS,SH);
                  }
                 else
                  {
                    instruction = MAKE_INSN3(srawi_record,RA,RS,SH);
                  }
               break;
             }

       // 854
          case 0x356:
             {
            // This is a book II instruction "Enforce In-order Execution of I/O
               ROSE_ASSERT(primaryOpcode == 0x1F);
               instruction = MAKE_INSN0(eieio);
               break;
             }

       // 918
          case 0x396:
             {
               ROSE_ASSERT(primaryOpcode == 0x1F);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               instruction = MAKE_INSN3(sthbrx,RS,RA,RB);
               break;
             }

       // BGL specific instructions
          case 0x36: // 54
          case 0x56: // 86
          case 0x1CE: // 462
          case 0x1EE: // 494
          case 0x256: // 598
          case 0x257: // 599
          case 0x2D7: // 727
          case 0x3BA: // 954
          case 0x3BE: // 958
          case 0x3CE: // 974
          case 0x3EE: // 1006
             {
            // These appear to be BGL specific instructions (not a part of the public PowerPC instruction set documentation).
               ROSE_ASSERT(primaryOpcode == 0x1F);
               instruction = MAKE_INSN0(unknown_instruction);
               break;
             }

          default:
             {
            // The default case is now used for handling illegal instructions 
            // (during development it was for those not yet implemented).
               printf ("Error: X-Form xoOpcode = %d not handled! \n",xoOpcode);
               throw BadInstruction();
            // ROSE_ASSERT(false);
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

  // Get the bits 6-10, next 5 bits
     uint8_t btOpcode = (insn >> 21) & 0x1F;
     uint8_t boOpcode = btOpcode;
     uint8_t bfOpcode = btOpcode;

  // Get the bits 11-15, next 5 bits
     uint8_t baOpcode = (insn >> 16) & 0x1F;
     uint8_t biOpcode = baOpcode;

  // Get the bits 11-13, next 3 bits
     uint8_t bfaOpcode = (insn >> 18) & 0x7;

  // Get the bits 16-20, next 5 bits
     uint8_t bbOpcode = (insn >> 11) & 0x1F;

  // Get the bits 19-20, next 2 bits
     uint8_t bhOpcode = (insn >> 11) & 0x3;

  // Get the bits 21-30, next 10 bits
     uint16_t xoOpcode = (insn >> 1) & 0x3FF;

  // Get bit 31, 1 bit as the link bit
     uint8_t lkOpcode = (insn >> 0) & 0x1;

#if DEBUG_OPCODES
     printf ("XL-Form instruction opcode = 0x%x xoOpcode = 0x%x = %d btOpcode = 0x%x = %d baOpcode = 0x%x = %d bbOpcode = 0x%x = %d bhOpcode = 0x%x = %d lkOpcode = 0x%x \n",
          insn,xoOpcode,xoOpcode,btOpcode,btOpcode,baOpcode,baOpcode,bbOpcode,bbOpcode,bhOpcode,bhOpcode,lkOpcode);
#endif

     switch(xoOpcode)
        {
       // 0
          case 0x0:
             {
               SgAsmExpression* BF  = makeRegister(powerpc_regclass_gpr,bfOpcode,powerpc_condreggranularity_bit);
               SgAsmExpression* BFA = makeRegister(powerpc_regclass_gpr,bfaOpcode,powerpc_condreggranularity_bit);
               instruction = MAKE_INSN2(mcrf,BF,BFA);
               break;
             }

       // 16
          case 0x10:
             {
               SgAsmExpression* BO = new SgAsmByteValueExpression(boOpcode);
               SgAsmExpression* BI = makeRegister(powerpc_regclass_gpr,biOpcode,powerpc_condreggranularity_bit);
               SgAsmExpression* BH = new SgAsmByteValueExpression(bhOpcode);
               if (lkOpcode == 0)
                  {
                    instruction = MAKE_INSN3(bclr,BO,BI,BH);
                  }
                 else
                  {
                    instruction = MAKE_INSN3(bclrl,BO,BI,BH);

                    knownSuccessorsReturn->insert(p.ip + 4);
                  }

#if DEBUG_BRANCH_LOGIC
               printf ("***** Need to compute branch address (but it is indirect, via link register) ***** \n");
            // ROSE_ASSERT(false);
#endif
               break;
             }

       // 33
          case 0x21:
             {
               SgAsmExpression* BT = makeRegister(powerpc_regclass_gpr,btOpcode,powerpc_condreggranularity_bit);
               SgAsmExpression* BA = makeRegister(powerpc_regclass_gpr,baOpcode,powerpc_condreggranularity_bit);
               SgAsmExpression* BB = makeRegister(powerpc_regclass_gpr,bbOpcode,powerpc_condreggranularity_bit);
               instruction = MAKE_INSN3(crnor,BT,BA,BB);
               break;
             }

       // 129
          case 0x81:
             {
               SgAsmExpression* BT = makeRegister(powerpc_regclass_gpr,btOpcode,powerpc_condreggranularity_bit);
               SgAsmExpression* BA = makeRegister(powerpc_regclass_gpr,baOpcode,powerpc_condreggranularity_bit);
               SgAsmExpression* BB = makeRegister(powerpc_regclass_gpr,bbOpcode,powerpc_condreggranularity_bit);
               instruction = MAKE_INSN3(crandc,BT,BA,BB);
               break;
             }

       // 193
          case 0xC1:
             {
               SgAsmExpression* BT = makeRegister(powerpc_regclass_gpr,btOpcode,powerpc_condreggranularity_bit);
               SgAsmExpression* BA = makeRegister(powerpc_regclass_gpr,baOpcode,powerpc_condreggranularity_bit);
               SgAsmExpression* BB = makeRegister(powerpc_regclass_gpr,bbOpcode,powerpc_condreggranularity_bit);
               instruction = MAKE_INSN3(crxor,BT,BA,BB);
               break;
             }

       // 225
          case 0xE1:
             {
               SgAsmExpression* BT = makeRegister(powerpc_regclass_gpr,btOpcode,powerpc_condreggranularity_bit);
               SgAsmExpression* BA = makeRegister(powerpc_regclass_gpr,baOpcode,powerpc_condreggranularity_bit);
               SgAsmExpression* BB = makeRegister(powerpc_regclass_gpr,bbOpcode,powerpc_condreggranularity_bit);
               instruction = MAKE_INSN3(crnand,BT,BA,BB);
               break;
             }

       // 257
          case 0x101:
             {
               SgAsmExpression* BT = makeRegister(powerpc_regclass_gpr,btOpcode,powerpc_condreggranularity_bit);
               SgAsmExpression* BA = makeRegister(powerpc_regclass_gpr,baOpcode,powerpc_condreggranularity_bit);
               SgAsmExpression* BB = makeRegister(powerpc_regclass_gpr,bbOpcode,powerpc_condreggranularity_bit);
               instruction = MAKE_INSN3(crand,BT,BA,BB);
               break;
             }

       // 289
          case 0x121:
             {
               SgAsmExpression* BT = makeRegister(powerpc_regclass_gpr,btOpcode,powerpc_condreggranularity_bit);
               SgAsmExpression* BA = makeRegister(powerpc_regclass_gpr,baOpcode,powerpc_condreggranularity_bit);
               SgAsmExpression* BB = makeRegister(powerpc_regclass_gpr,bbOpcode,powerpc_condreggranularity_bit);
               instruction = MAKE_INSN3(creqv,BT,BA,BB);
               break;
             }

       // 417
          case 0x1A1:
             {
               SgAsmExpression* BT = makeRegister(powerpc_regclass_gpr,btOpcode,powerpc_condreggranularity_bit);
               SgAsmExpression* BA = makeRegister(powerpc_regclass_gpr,baOpcode,powerpc_condreggranularity_bit);
               SgAsmExpression* BB = makeRegister(powerpc_regclass_gpr,bbOpcode,powerpc_condreggranularity_bit);
               instruction = MAKE_INSN3(crorc,BT,BA,BB);
               break;
             }

       // 449
          case 0x1C1:
             {
               SgAsmExpression* BT = makeRegister(powerpc_regclass_gpr,btOpcode,powerpc_condreggranularity_bit);
               SgAsmExpression* BA = makeRegister(powerpc_regclass_gpr,baOpcode,powerpc_condreggranularity_bit);
               SgAsmExpression* BB = makeRegister(powerpc_regclass_gpr,bbOpcode,powerpc_condreggranularity_bit);
               instruction = MAKE_INSN3(cror,BT,BA,BB);
               break;
             }

       // 528
          case 0x210:
             {
               SgAsmExpression* BO = new SgAsmByteValueExpression(boOpcode);
               SgAsmExpression* BI = makeRegister(powerpc_regclass_gpr,biOpcode,powerpc_condreggranularity_bit);
               SgAsmExpression* BH = new SgAsmByteValueExpression(bhOpcode);
               if (lkOpcode == 0)
                  {
                    instruction = MAKE_INSN3(bcctr,BO,BI,BH);
                  }
                 else
                  {
                    instruction = MAKE_INSN3(bcctrl,BO,BI,BH);

                    knownSuccessorsReturn->insert(p.ip + 4);
                  }

#if DEBUG_BRANCH_LOGIC
               printf ("***** Need to compute branch address (but it is indirect, via link register) ***** \n");
            // ROSE_ASSERT(false);
#endif
               break;
             }

          default:
             {
            // The default case is now used for handling illegal instructions 
            // (during development it was for those not yet implemented).
               printf ("Error: XL-Form xoOpcode = %d not handled! \n",xoOpcode);
               ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_XFX_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;

  // The primaryOpcode 
     uint8_t primaryOpcode = (insn >> 26) & 0x3F;
     ROSE_ASSERT(primaryOpcode == 0x1F);

  // Get the bits 6-10, 5 bits wide
     uint8_t rtOpcode = (insn >> 21) & 0x1F;
     uint8_t rsOpcode = rtOpcode;

  // Get the bits 11-20, 10 bits wide
     uint16_t sprOpcode_temp = (insn >> 11) & 0x3FF;
     uint16_t sprOpcode = ((sprOpcode_temp >> 5) | (sprOpcode_temp << 5)) & 0x3FF;

  // Values currently commented out are expected to be used later!
  // uint16_t tbrOpcode = sprOpcode;

  // Get the bit 11, 1 bit wide
     uint8_t constantOpcode = (insn >> 20) & 0x1;

  // Values currently commented out are expected to be used later!
  // Get the bits 12-20, 8 bits wide
     uint8_t fxmOpcode = (insn >> 12) & 0xFF;

  // Get the bits 21-30, next 10 bits
     uint16_t xoOpcode = (insn >> 1) & 0x3FF;

#if DEBUG_OPCODES
     printf ("XFX-Form instruction opcode = 0x%x xoOpcode = 0x%x \n",insn,xoOpcode);
#endif

     switch(xoOpcode)
        {
       // 19
          case 0x13:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               if (constantOpcode == 0)
                  {
                    instruction = MAKE_INSN1(mfcr,RT);
                  }
                 else
                  {
                 // instruction = MAKE_INSN1(mfocr,RT);
                    instruction = MAKE_INSN1(mfcr,RT);

                 // See note on page 124 of User Instruction Set Architecture version 2.02
                    printf ("This mfocr instruction is an old form of the mfcr instruction ... \n");
                    ROSE_ASSERT(false);
                  }
               break;
             }

       // 144
          case 0x90:
             {
               SgAsmExpression* FXM = new SgAsmByteValueExpression(fxmOpcode);
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               if (constantOpcode == 0)
                  {
                    instruction = MAKE_INSN2(mtcrf,FXM,RS);
                  }
                 else
                  {
                 // instruction = MAKE_INSN2(mtocrf,FXM,RS);
                    instruction = MAKE_INSN2(mtcrf,FXM,RS);

                 // See note on page 124 of User Instruction Set Architecture version 2.02
                    printf ("This mtocrf instruction is an old form of the mtcrf instruction ... \n");
                    ROSE_ASSERT(false);
                  }
               break;
             }

       // 339
          case 0x153:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
            // SgAsmExpression* SPR = new SgAsmWordValueExpression(sprOpcode);
               SgAsmExpression* SPR = makeRegister(powerpc_regclass_spr,sprOpcode,powerpc_condreggranularity_whole);
               instruction = MAKE_INSN2(mfspr,RT,SPR);
               break;
             }

       // 467
          case 0x1d3:
             {
               SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);
               SgAsmExpression* SPR = makeRegister(powerpc_regclass_spr,sprOpcode,powerpc_condreggranularity_whole);
               instruction = MAKE_INSN2(mfspr,RS,SPR);
               break;
             }

          default:
             {
            // The default case is now used for handling illegal instructions 
            // (during development it was for those not yet implemented).
               printf ("Error: XFX-Form xoOpcode = %d not handled! \n",xoOpcode);
               ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_XFL_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;

  // The primaryOpcode 
     uint8_t primaryOpcode = (insn >> 26) & 0x3F;
     ROSE_ASSERT(primaryOpcode == 0x3F);

  // Get the bits 7-14, 8 bits wide
     uint8_t flmOpcode = (insn >> 17) & 0xFF;

  // Get the bits 16-20, 5 bits wide
     uint8_t frbOpcode = (insn >> 11) & 0x1F;

  // Get the bit 31, 1 bit wide
     uint8_t rcOpcode = (insn >> 0) & 0x1;

#if DEBUG_OPCODES
     printf ("XFL-Form instruction opcode = 0x%x flmOpcode = 0x%x = %d frbOpcode = 0x%x = %d \n",insn,flmOpcode,flmOpcode,frbOpcode,frbOpcode);
#endif

     SgAsmExpression* FLM = new SgAsmWordValueExpression(flmOpcode);
     SgAsmExpression* FRB = makeRegister(powerpc_regclass_gpr,frbOpcode);
     if (rcOpcode == 0)
        {
          instruction = MAKE_INSN2(mtfsf,FLM,FRB);
        }
       else
        {
          instruction = MAKE_INSN2(mtfsf_record,FLM,FRB);
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_XS_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;

     printf ("XS-Form instructions not implemented yet, I have no examples so far! \n");

     ROSE_ASSERT(instruction != NULL);
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_XO_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;

  // The primaryOpcode 
     uint8_t primaryOpcode = (insn >> 26) & 0x3F;
     ROSE_ASSERT(primaryOpcode == 0x1F);

  // Get the bits 6-10, 5 bits wide
     uint8_t rtOpcode = (insn >> 21) & 0x1F;

  // Get the bits 11-15, 5 bits wide
     uint8_t raOpcode = (insn >> 16) & 0x1F;

  // Get the bits 16-20, 5 bits wide
     uint8_t rbOpcode = (insn >> 11) & 0x1F;

  // Get the bit 21, 1 bit wide
     uint8_t oeOpcode = (insn >> 10) & 0x1;

  // Get the bits 21-30, next 10 bits
     uint16_t xoOpcode = (insn >> 1) & 0x1FF;

  // Get the bit 31, 1 bit wide
     uint8_t rcOpcode = (insn >> 0) & 0x1;

#if DEBUG_OPCODES
     printf ("XO-Form instruction opcode = 0x%x xoOpcode = 0x%x \n",insn,xoOpcode);
#endif

     switch(xoOpcode)
        {
       // 8
          case 0x8:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (oeOpcode == 0)
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(subfc,RT,RA,RB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(subfc_record,RT,RA,RB);
                       }
                  }
                 else
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(subfco,RT,RA,RB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(subfco_record,RT,RA,RB);
                       }
                  }
               break;
             }

       // 9
          case 0x9:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN3(mulhdu,RT,RA,RB);
                  }
                 else
                  {
                    instruction = MAKE_INSN3(mulhdu_record,RT,RA,RB);
                  }
               break;
             }

       // 10
          case 0xA:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (oeOpcode == 0)
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(addc,RT,RA,RB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(addc_record,RT,RA,RB);
                       }
                  }
                 else
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(addco,RT,RA,RB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(addco_record,RT,RA,RB);
                       }
                  }
               break;
             }

       // 11
          case 0xB:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN3(mulhwu,RT,RA,RB);
                  }
                 else
                  {
                    instruction = MAKE_INSN3(mulhwu_record,RT,RA,RB);
                  }
               break;
             }

       // 40
          case 0x28:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (oeOpcode == 0)
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(subf,RT,RA,RB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(subf_record,RT,RA,RB);
                       }
                  }
                 else
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(subfo,RT,RA,RB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(subfo_record,RT,RA,RB);
                       }
                  }
               break;
             }

       // 73
          case 0x49:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN3(mulhd,RT,RA,RB);
                  }
                 else
                  {
                    instruction = MAKE_INSN3(mulhd_record,RT,RA,RB);
                  }
               break;
             }

       // 75
          case 0x4B:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN3(mulhw,RT,RA,RB);
                  }
                 else
                  {
                    instruction = MAKE_INSN3(mulhw_record,RT,RA,RB);
                  }
               break;
             }

       // 104
          case 0x68:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               if (oeOpcode == 0)
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN2(neg,RT,RA);
                       }
                      else
                       {
                         instruction = MAKE_INSN2(neg_record,RT,RA);
                       }
                  }
                 else
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN2(nego,RT,RA);
                       }
                      else
                       {
                         instruction = MAKE_INSN2(nego_record,RT,RA);
                       }
                  }
               break;
             }

       // 136
          case 0x88:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (oeOpcode == 0)
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(adde,RT,RA,RB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(adde_record,RT,RA,RB);
                       }
                  }
                 else
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(addeo,RT,RA,RB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(addeo_record,RT,RA,RB);
                       }
                  }
               break;
             }

       // 138
          case 0x8A:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (oeOpcode == 0)
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(subfe,RT,RA,RB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(subfe_record,RT,RA,RB);
                       }
                  }
                 else
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(subfeo,RT,RA,RB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(subfeo_record,RT,RA,RB);
                       }
                  }
               break;
             }

       // 200
          case 0xC8:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               if (oeOpcode == 0)
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN2(subfze,RT,RA);
                       }
                      else
                       {
                         instruction = MAKE_INSN2(subfze_record,RT,RA);
                       }
                  }
                 else
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN2(subfzeo,RT,RA);
                       }
                      else
                       {
                         instruction = MAKE_INSN2(subfzeo_record,RT,RA);
                       }
                  }
               break;
             }

       // 202
          case 0xCA:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               if (oeOpcode == 0)
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN2(addze,RT,RA);
                       }
                      else
                       {
                         instruction = MAKE_INSN2(addze_record,RT,RA);
                       }
                  }
                 else
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN2(addzeo,RT,RA);
                       }
                      else
                       {
                         instruction = MAKE_INSN2(addzeo_record,RT,RA);
                       }
                  }
               break;
             }

       // 232
          case 0xE8:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               if (oeOpcode == 0)
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN2(subfme,RT,RA);
                       }
                      else
                       {
                         instruction = MAKE_INSN2(subfme_record,RT,RA);
                       }
                  }
                 else
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN2(subfmeo,RT,RA);
                       }
                      else
                       {
                         instruction = MAKE_INSN2(subfmeo_record,RT,RA);
                       }
                  }
               break;
             }

       // 234
          case 0xEA:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               if (oeOpcode == 0)
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN2(addme,RT,RA);
                       }
                      else
                       {
                         instruction = MAKE_INSN2(addme_record,RT,RA);
                       }
                  }
                 else
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN2(addmeo,RT,RA);
                       }
                      else
                       {
                         instruction = MAKE_INSN2(addmeo_record,RT,RA);
                       }
                  }
               break;
             }

       // 235
          case 0xEB:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (oeOpcode == 0)
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(mullw,RT,RA,RB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(mullw_record,RT,RA,RB);
                       }
                  }
                 else
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(mullwo,RT,RA,RB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(mullwo_record,RT,RA,RB);
                       }
                  }
               break;
             }

       // 266
          case 0x10A:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (oeOpcode == 0)
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(add,RT,RA,RB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(add_record,RT,RA,RB);
                       }
                  }
                 else
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(addo,RT,RA,RB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(addo_record,RT,RA,RB);
                       }
                  }
               break;
             }

       // 457
          case 0x1C9:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (oeOpcode == 0)
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(divdu,RT,RA,RB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(divdu_record,RT,RA,RB);
                       }
                  }
                 else
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(divduo,RT,RA,RB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(divduo_record,RT,RA,RB);
                       }
                  }
               break;
             }

       // 459
          case 0x1CB:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (oeOpcode == 0)
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(divwu,RT,RA,RB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(divwu_record,RT,RA,RB);
                       }
                  }
                 else
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(divwuo,RT,RA,RB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(divwuo_record,RT,RA,RB);
                       }
                  }
               break;
             }

       // 489
          case 0x1E9:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (oeOpcode == 0)
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(divd,RT,RA,RB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(divd_record,RT,RA,RB);
                       }
                  }
                 else
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(divdo,RT,RA,RB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(divdo_record,RT,RA,RB);
                       }
                  }
               break;
             }

       // 491
          case 0x1EB:
             {
               SgAsmExpression* RT = makeRegister(powerpc_regclass_gpr,rtOpcode);
               SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
               SgAsmExpression* RB = makeRegister(powerpc_regclass_gpr,rbOpcode);
               if (oeOpcode == 0)
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(divw,RT,RA,RB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(divw_record,RT,RA,RB);
                       }
                  }
                 else
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(divwo,RT,RA,RB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(divwo_record,RT,RA,RB);
                       }
                  }
               break;
             }

          default:
             {
            // The default case is now used for handling illegal instructions 
            // (during development it was for those not yet implemented).
               printf ("Error: XO-Form xoOpcode = %d not handled! \n",xoOpcode);
               ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_A_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;

  // The primaryOpcode
     uint8_t primaryOpcode = (insn >> 26) & 0x3F;

  // Get the bits 6-10, next 5 bits
     uint8_t frtOpcode  = (insn >> 21) & 0x1F;

  // Get the bits 11-15, next 5 bits
     uint8_t fraOpcode  = (insn >> 16) & 0x1F;

  // Get the bits 16-20, next 5 bits
     uint8_t frbOpcode  = (insn >> 11) & 0x1F;

  // Get the bits 21-25, next 5 bits
     uint8_t frcOpcode  = (insn >> 6) & 0x1F;

  // Get the bits 26-30, next 5 bits
     uint8_t xOpcode  = (insn >> 1) & 0x1F;

  // Get bit 31, 1 bit as the record bit
     uint8_t rcOpcode = (insn >> 0) & 0x1;

#if DEBUG_OPCODES
     printf ("A-Form instruction opcode = 0x%x frtOpcode = 0x%x fraOpcode = 0x%x frbOpcode = 0x%x frcOpcode = 0x%x xOpcode = 0x%x rcOpcode = 0x%x \n",insn,frtOpcode,fraOpcode,frbOpcode,frcOpcode,xOpcode,rcOpcode);
#endif

     SgAsmExpression* FRT = makeRegister(powerpc_regclass_gpr,frtOpcode);

     switch(xOpcode)
        {
       // 18
          case 0x12:
             {
               SgAsmExpression* FRA = makeRegister(powerpc_regclass_gpr,fraOpcode);
               SgAsmExpression* FRB = makeRegister(powerpc_regclass_gpr,frbOpcode);
               if (primaryOpcode == 0x3B /*59*/)
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(fdiv,FRT,FRA,FRB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(fdiv_record,FRT,FRA,FRB);
                       }
                  }
                 else
                  {
                    ROSE_ASSERT (primaryOpcode == 0x3F /*63*/);
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(fdivs,FRT,FRA,FRB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(fdivs_record,FRT,FRA,FRB);
                       }
                  }
               break;
             }

       // 20
          case 0x14:
             {
               SgAsmExpression* FRA = makeRegister(powerpc_regclass_gpr,fraOpcode);
               SgAsmExpression* FRB = makeRegister(powerpc_regclass_gpr,frbOpcode);
               if (primaryOpcode == 0x3B /*59*/)
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(fsub,FRT,FRA,FRB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(fsub_record,FRT,FRA,FRB);
                       }
                  }
                 else
                  {
                    ROSE_ASSERT (primaryOpcode == 0x3F /*63*/);
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(fsubs,FRT,FRA,FRB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(fsubs_record,FRT,FRA,FRB);
                       }
                  }
               break;
             }

       // 21
          case 0x15:
             {
               SgAsmExpression* FRA = makeRegister(powerpc_regclass_gpr,fraOpcode);
               SgAsmExpression* FRB = makeRegister(powerpc_regclass_gpr,frbOpcode);
               if (primaryOpcode == 0x3B /*59*/)
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(fadd,FRT,FRA,FRB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(fadd_record,FRT,FRA,FRB);
                       }
                  }
                 else
                  {
                    ROSE_ASSERT (primaryOpcode == 0x3F /*63*/);
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(fadds,FRT,FRA,FRB);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(fadds_record,FRT,FRA,FRB);
                       }
                  }
               break;
             }

       // 25
          case 0x19:
             {
               SgAsmExpression* FRA = makeRegister(powerpc_regclass_gpr,fraOpcode);
               SgAsmExpression* FRC = makeRegister(powerpc_regclass_gpr,frcOpcode);
               if (primaryOpcode == 0x3B /*59*/)
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(fmul,FRT,FRA,FRC);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(fmul_record,FRT,FRA,FRC);
                       }
                  }
                 else
                  {
                    ROSE_ASSERT (primaryOpcode == 0x3F /*63*/);
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN3(fmuls,FRT,FRA,FRC);
                       }
                      else
                       {
                         instruction = MAKE_INSN3(fmuls_record,FRT,FRA,FRC);
                       }
                  }
               break;
             }

       // 28
          case 0x1C:
             {
               SgAsmExpression* FRA = makeRegister(powerpc_regclass_gpr,fraOpcode);
               SgAsmExpression* FRB = makeRegister(powerpc_regclass_gpr,frbOpcode);
               SgAsmExpression* FRC = makeRegister(powerpc_regclass_gpr,frcOpcode);
               if (primaryOpcode == 0x3B /*59*/)
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN4(fmsub,FRT,FRA,FRC,FRB);
                       }
                      else
                       {
                         instruction = MAKE_INSN4(fmsub_record,FRT,FRA,FRC,FRB);
                       }
                  }
                 else
                  {
                    ROSE_ASSERT (primaryOpcode == 0x3F /*63*/);
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN4(fmsubs,FRT,FRA,FRC,FRB);
                       }
                      else
                       {
                         instruction = MAKE_INSN4(fmsubs_record,FRT,FRA,FRC,FRB);
                       }
                  }
               break;
             }

       // 29
          case 0x1D:
             {
               SgAsmExpression* FRA = makeRegister(powerpc_regclass_gpr,fraOpcode);
               SgAsmExpression* FRB = makeRegister(powerpc_regclass_gpr,frbOpcode);
               SgAsmExpression* FRC = makeRegister(powerpc_regclass_gpr,frcOpcode);
               if (primaryOpcode == 0x3B /*59*/)
                  {
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN4(fmadd,FRT,FRA,FRC,FRB);
                       }
                      else
                       {
                         instruction = MAKE_INSN4(fmadd_record,FRT,FRA,FRC,FRB);
                       }
                  }
                 else
                  {
                    ROSE_ASSERT (primaryOpcode == 0x3F /*63*/);
                    if (rcOpcode == 0)
                       {
                         instruction = MAKE_INSN4(fmadds,FRT,FRA,FRC,FRB);
                       }
                      else
                       {
                         instruction = MAKE_INSN4(fmadds_record,FRT,FRA,FRC,FRB);
                       }
                  }
               break;
             }

          default:
             {
            // The default case is now used for handling illegal instructions 
            // (during development it was for those not yet implemented).
               printf ("Error: A-Form primaryOpcode = %d (illegal instruction) \n",xOpcode);
               ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(instruction != NULL);
     return instruction;
   }

SgAsmPowerpcInstruction*
PowerpcDisassembler::SingleInstructionDisassembler::decode_M_formInstruction()
   {
     SgAsmPowerpcInstruction* instruction = NULL;

  // The primaryOpcode
     uint8_t primaryOpcode = (insn >> 26) & 0x3F;

  // Get the bits 6-10, next 5 bits
     uint8_t rsOpcode  = (insn >> 21) & 0x1F;

  // Get the bits 11-15, next 5 bits
     uint8_t raOpcode  = (insn >> 16) & 0x1F;

  // Get the bits 16-20, next 5 bits
     uint8_t rbOpcode  = (insn >> 11) & 0x1F;
     uint8_t shOpcode  = rbOpcode;

  // Get the bits 21-25, next 5 bits
     uint8_t mbOpcode  = (insn >> 6) & 0x1F;

  // Get the bits 26-30, next 5 bits
     uint8_t meOpcode  = (insn >> 1) & 0x1F;

  // Get bit 31, 1 bit as the record bit
     uint8_t rcOpcode = (insn >> 0) & 0x1;

#if DEBUG_OPCODES
     printf ("M-Form instruction opcode = 0x%x rsOpcode = 0x%x raOpcode = 0x%x rbOpcode = 0x%x mbOpcode = 0x%x meOpcode = 0x%x rcOpcode = 0x%x \n",insn,rsOpcode,raOpcode,rbOpcode,mbOpcode,meOpcode,rcOpcode);
#endif

     SgAsmExpression* RA = makeRegister(powerpc_regclass_gpr,raOpcode);
     SgAsmExpression* RS = makeRegister(powerpc_regclass_gpr,rsOpcode);

  // There should be a build function for these!
     SgAsmExpression* SH = new SgAsmByteValueExpression(shOpcode);
     SgAsmExpression* MB = new SgAsmByteValueExpression(mbOpcode);
     SgAsmExpression* ME = new SgAsmByteValueExpression(meOpcode);

     switch(primaryOpcode)
        {
       // 20
          case 0x14:
             {
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN5(rlwimi,RA,RS,SH,MB,ME);
                  }
                 else
                  {
                    instruction = MAKE_INSN5(rlwimi_record,RA,RS,SH,MB,ME);
                  }
               break;
             }

       // 21
          case 0x15:
             {
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN5(rlwinm,RA,RS,SH,MB,ME);
                  }
                 else
                  {
                    instruction = MAKE_INSN5(rlwinm_record,RA,RS,SH,MB,ME);
                  }
               break;
             }

       // 23
          case 0x17:
             {
               if (rcOpcode == 0)
                  {
                    instruction = MAKE_INSN5(rlwnm,RA,RS,SH,MB,ME);
                  }
                 else
                  {
                    instruction = MAKE_INSN5(rlwnm_record,RA,RS,SH,MB,ME);
                  }
               break;
             }

          default:
             {
            // The default case is now used for handling illegal instructions 
            // (during development it was for those not yet implemented).
            // There are only 3 legal M-form instructions, so everything else is an illegal instruction!
               printf ("Error: M-Form primaryOpcode = %d (illegal instruction) \n",primaryOpcode);
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

