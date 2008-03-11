#ifndef ROSE_X86DISASSEMBLER_H
#define ROSE_X86DISASSEMBLER_H

#include <stdint.h>
#include <vector>

namespace X86Disassembler {

  enum Size {size16, size32, size64};

  struct OverflowOfInstructionVector {};
  struct BadInstruction {};

  struct Parameters {
    uint64_t ip;
    Size insnSize;
    Parameters(uint64_t ip = 0, Size insnSize = size32): ip(ip), insnSize(insnSize) {}
  };

  SgAsmx86Instruction* disassemble(const Parameters& p, const std::vector<uint8_t>& insn, size_t positionInVector, std::vector<uint64_t>* knownSuccessorsReturn = 0); // *knownSuccessorsReturn will be appended to
  void disassembleFile(SgAsmFile* f);

};

#endif // ROSE_X86DISASSEMBLER_H
