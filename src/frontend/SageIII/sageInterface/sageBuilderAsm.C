// tps (01/14/2010) : Switching from rose.h to sage3
#include "sage3basic.h"

// DQ (5/1/2010): This is required to support the function: SgAsmInstruction* SageBuilderAsm::buildMultibyteNopInstruction(int n)
#include "InstructionEnumsX86.h"


using namespace std;

SgAsmx86Instruction* SageBuilderAsm::buildx86Instruction( X86InstructionKind kind )
   {
  // These are the default values used for the construction of new instructions.
     rose_addr_t address            = 0;
     string mnemonic           = "";
     X86InstructionSize baseSize    = x86_insnsize_none;
     X86InstructionSize operandSize = x86_insnsize_none; 
     X86InstructionSize addressSize = x86_insnsize_none;

     SgAsmx86Instruction* instruction = new SgAsmx86Instruction(address,mnemonic,kind,baseSize,operandSize,addressSize);

  // This should not have been set yet.
     ROSE_ASSERT(instruction->get_operandList() == NULL);

  // All instructions are required to have a valid SgAsmOperandList pointer.
     instruction->set_operandList(new SgAsmOperandList ());

  // Set the parent in the SgAsmOperandList
     instruction->get_operandList()->set_parent(instruction);

     return instruction;
   }

SgAsmx86Instruction* SageBuilderAsm::buildx86Instruction( X86InstructionKind kind, SgAsmExpression* operand )
   {
     SgAsmx86Instruction* instruction = buildx86Instruction(kind);
     appendOperand(instruction,operand);

     return instruction;
   }

SgAsmx86Instruction* SageBuilderAsm::buildx86Instruction( X86InstructionKind kind, SgAsmExpression* lhs, SgAsmExpression* rhs  )
   {
     SgAsmx86Instruction* instruction = buildx86Instruction(kind);
     appendOperand(instruction,lhs);
     appendOperand(instruction,rhs);

     return instruction;
   }

SgAsmDoubleWordValueExpression*
SageBuilderAsm::buildAsmDWordValue(uint32_t val)
   {
  // This build function is just a wrapper (for now) to the "make" functions of which there are only a few available.
     return SageBuilderAsm::makeDWordValue(val);
   }

SgAsmByteValueExpression*
SageBuilderAsm::buildAsmByteValue(uint8_t val)
   {
  // This build function is just a wrapper (for now) to the "make" functions of which there are only a few available.
     return SageBuilderAsm::makeByteValue(val);
   }

SgAsmWordValueExpression*
SageBuilderAsm::buildAsmWordValue(uint16_t val)
   {
  // This build function is just a wrapper (for now) to the "make" functions of which there are only a few available.
     return SageBuilderAsm::makeWordValue(val);
   }

SgAsmBinaryAdd*
SageBuilderAsm::buildAsmAddExpression(SgAsmExpression* lhs, SgAsmExpression* rhs)
   {
  // This build function is just a wrapper (for now) to the "make" functions of which there are only a few available.
     return SageBuilderAsm::makeAdd(lhs,rhs);
   }


SgAsmMemoryReferenceExpression*
SageBuilderAsm::buildAsmMemoryReferenceExpression(SgAsmExpression* addr, SgAsmExpression* segment, SgAsmType* t)
   {
     return SageBuilderAsm::makeMemoryReference(addr,segment,t);
   }


SgAsmTypeByte*
SageBuilderAsm::buildAsmTypeByte()
   {
     return new SgAsmTypeByte();
   }

SgAsmTypeWord*
SageBuilderAsm::buildAsmTypeWord()
   {
     return new SgAsmTypeWord();
   }

SgAsmTypeDoubleWord*
SageBuilderAsm::buildAsmTypeDoubleWord()
   {
     return new SgAsmTypeDoubleWord();
   }

SgAsmTypeQuadWord*
SageBuilderAsm::buildAsmTypeQuadWord()
   {
     return new SgAsmTypeQuadWord();
   }

SgAsmTypeSingleFloat*
SageBuilderAsm::buildAsmTypeSingleFloat()
   {
     return new SgAsmTypeSingleFloat();
   }

SgAsmTypeDoubleFloat*
SageBuilderAsm::buildAsmTypeDoubleFloat()
   {
     return new SgAsmTypeDoubleFloat();
   }

SgAsmType128bitFloat*
SageBuilderAsm::buildAsmType128bitFloat()
   {
     return new SgAsmType128bitFloat();
   }

SgAsmType80bitFloat*
SageBuilderAsm::buildAsmType80bitFloat()
   {
     return new SgAsmType80bitFloat();
   }

SgAsmTypeDoubleQuadWord*
SageBuilderAsm::buildAsmTypeDoubleQuadWord()
   {
     return new SgAsmTypeDoubleQuadWord();
   }

SgAsmTypeVector*
SageBuilderAsm::buildAsmTypeVector()
   {
     return new SgAsmTypeVector();
   }


SgAsmInstruction*
SageBuilderAsm::buildMultibyteNopInstruction(int n)
   {
  // DQ (5/1/2010): Support for building multi-byte NOP instructions.
  // this is x86 specific which is OK for now.
  // SgAsmInstruction* instruction = NULL;

     ROSE_ASSERT(n > 0);
     ROSE_ASSERT(n <= 9);

     uint64_t ip                             = 0; /* Virtual address for start of instruction */
     std::string mnemonic                    = "nop";
     X86InstructionKind kind                 = x86_nop;
     X86InstructionSize insnSize             = x86_insnsize_32; /* Default size of instructions, based on architecture; see init() */
     X86InstructionSize effectiveOperandSize = x86_insnsize_32;
     X86InstructionSize effectiveAddressSize = x86_insnsize_32;

     bool lock                               = false;
     X86RepeatPrefix repeatPrefix            = x86_repeat_none;
     X86SegmentRegister segOverride          = x86_segreg_none;
     bool branchPredictionEnabled            = false;
     X86BranchPrediction branchPrediction    = x86_branch_prediction_none;

  // SgUnsignedCharList insnbuf(0x90);
     SgUnsignedCharList insnbuf;
     size_t insnbufat                        = size_t(n);  /* Index of next byte to be read from or write to insnbuf */

     SgAsmx86Instruction *instruction = NULL;

     instruction = new SgAsmx86Instruction(ip, mnemonic, kind, insnSize, effectiveOperandSize, effectiveAddressSize);
     ROSE_ASSERT(instruction != NULL);

  // Here we are building a simpler version of multi-byte nop using repeated prefixes.
     for (int i = 1; i < n; i++)
        {
          insnbuf.push_back(0x66);
        }

     insnbuf.push_back(0x90);

#if 0
  // This switch will implement proper multi-byte NOPs (not implemented yet).
     switch(n)
        {
          case 1:
             {
            // instruction->set_raw_bytes(SgUnsignedCharList(&(insnbuf[0]), &(insnbuf[0])+insnbufat));
               insnbuf.push_front(0x66);
               break;
             }

       // case 2: instruction = makeInstruction(x86_nop, "nop", modrm); break;

          default:
             {
               printf ("Error: SageBuilderAsm::buildMultibyteNopInstruction(n=%d) not supported \n",n);
               ROSE_ASSERT(false);
             }
        }
#endif

     instruction->set_raw_bytes(SgUnsignedCharList(&(insnbuf[0]), &(insnbuf[0])+insnbufat));
     ROSE_ASSERT(instruction != NULL);

     instruction->set_lockPrefix(lock);
     instruction->set_repeatPrefix(repeatPrefix);

     instruction->set_segmentOverride(segOverride);
     if (branchPredictionEnabled)
          instruction->set_branchPrediction(branchPrediction);

     SgAsmOperandList *operands = new SgAsmOperandList();
     instruction->set_operandList(operands);
     operands->set_parent(instruction);

     return instruction;
   }



// DQ (4/26/2010): This functions should be depricated so that we can
// change the names to be consistant with the rest of the AST build interface.
namespace SageBuilderAsm {

  SgAsmByteValueExpression* makeByteValue(uint8_t val) {
    SgAsmByteValueExpression* v = new SgAsmByteValueExpression(val);
    ROSE_ASSERT (v);
    return v;
  }

  SgAsmWordValueExpression* makeWordValue(uint16_t val) {
    SgAsmWordValueExpression* v = new SgAsmWordValueExpression(val);
    ROSE_ASSERT (v);
    return v;
  }

  SgAsmDoubleWordValueExpression* makeDWordValue(uint32_t val) {
    SgAsmDoubleWordValueExpression* v = new SgAsmDoubleWordValueExpression(val);
    ROSE_ASSERT (v);
    return v;
  }

  SgAsmQuadWordValueExpression* makeQWordValue(uint64_t val) {
    SgAsmQuadWordValueExpression* v = new SgAsmQuadWordValueExpression(val);
    ROSE_ASSERT (v);
    return v;
  }

  SgAsmMemoryReferenceExpression* makeMemoryReference(SgAsmExpression* addr, SgAsmExpression* segment, SgAsmType* t) {
    SgAsmMemoryReferenceExpression* r = new SgAsmMemoryReferenceExpression(addr);
    addr->set_parent(r);
    if (segment) {
      r->set_segment(segment);
      segment->set_parent(r);
    }
    if (t) r->set_type(t);
    return r;
  }

  SgAsmBinaryAdd* makeAdd(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinaryAdd* a = new SgAsmBinaryAdd(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  SgAsmBinarySubtract* makeSubtract(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinarySubtract* a = new SgAsmBinarySubtract(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  SgAsmBinaryAddPreupdate* makeAddPreupdate(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinaryAddPreupdate* a = new SgAsmBinaryAddPreupdate(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  SgAsmBinarySubtractPreupdate* makeSubtractPreupdate(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinarySubtractPreupdate* a = new SgAsmBinarySubtractPreupdate(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  SgAsmBinaryAddPostupdate* makeAddPostupdate(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinaryAddPostupdate* a = new SgAsmBinaryAddPostupdate(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  SgAsmBinarySubtractPostupdate* makeSubtractPostupdate(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinarySubtractPostupdate* a = new SgAsmBinarySubtractPostupdate(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  SgAsmBinaryMultiply* makeMul(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinaryMultiply* a = new SgAsmBinaryMultiply(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  SgAsmBinaryLsl* makeLsl(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinaryLsl* a = new SgAsmBinaryLsl(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  SgAsmBinaryLsr* makeLsr(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinaryLsr* a = new SgAsmBinaryLsr(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  SgAsmBinaryAsr* makeAsr(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinaryAsr* a = new SgAsmBinaryAsr(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  SgAsmBinaryRor* makeRor(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinaryRor* a = new SgAsmBinaryRor(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  SgAsmUnaryRrx* makeRrx(SgAsmExpression* lhs) {
    SgAsmUnaryRrx* a = new SgAsmUnaryRrx(lhs);
    lhs->set_parent(a);
    return a;
  }

  SgAsmUnaryArmSpecialRegisterList* makeArmSpecialRegisterList(SgAsmExpression* lhs) {
    SgAsmUnaryArmSpecialRegisterList* a = new SgAsmUnaryArmSpecialRegisterList(lhs);
    lhs->set_parent(a);
    return a;
  }

  SgAsmExprListExp* makeExprListExp() {
    return new SgAsmExprListExp();
  }

}


