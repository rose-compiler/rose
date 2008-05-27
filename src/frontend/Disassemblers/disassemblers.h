#ifndef ROSE_DISASSEMBLERS_H
#define ROSE_DISASSEMBLERS_H

#include <stdint.h>
#include <vector>

struct OverflowOfInstructionVector {};
struct BadInstruction {};

namespace X86Disassembler {

  struct Parameters {
    uint64_t ip;
    X86InstructionSize insnSize;
    Parameters(uint64_t ip = 0, X86InstructionSize insnSize = x86_insnsize_32): ip(ip), insnSize(insnSize) {}
  };

  SgAsmx86Instruction* disassemble(const Parameters& p, const std::vector<uint8_t>& insn, size_t positionInVector, std::vector<uint64_t>* knownSuccessorsReturn = 0); // *knownSuccessorsReturn will be appended to
  void disassembleFile(SgAsmFile* f);

  inline SgAsmType* sizeToType(X86InstructionSize s) {
    switch (s) {
      case x86_insnsize_none: return NULL;
      case x86_insnsize_16: return SgAsmTypeWord::createType();
      case x86_insnsize_32: return SgAsmTypeDoubleWord::createType();
      case x86_insnsize_64: return SgAsmTypeQuadWord::createType();
      default: ROSE_ASSERT (false);
    }
  }

  inline X86PositionInRegister sizeToPos(X86InstructionSize s) {
    switch (s) {
      case x86_insnsize_none: return x86_regpos_all;
      case x86_insnsize_16: return x86_regpos_word;
      case x86_insnsize_32: return x86_regpos_dword;
      case x86_insnsize_64: return x86_regpos_qword;
      default: ROSE_ASSERT (false);
    }
  }

}

namespace ArmDisassembler {

  struct Parameters {
    uint32_t ip;
    bool decodeUnconditionalInstructions;
    Parameters(uint32_t ip = 0, bool decodeUnconditionalInstructions = true): ip(ip), decodeUnconditionalInstructions(decodeUnconditionalInstructions) {}
  };

  SgAsmArmRegisterReferenceExpression* makeRegister(uint8_t reg);
  SgAsmArmInstruction* disassemble(const Parameters& p, uint32_t insn, std::vector<uint64_t>* knownSuccessorsReturn = 0); // *knownSuccessorsReturn will be appended to
  void disassembleFile(SgAsmFile* f);

}

#endif // ROSE_DISASSEMBLERS_H
