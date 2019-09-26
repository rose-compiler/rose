#ifndef ROSE_BinaryAnalysis_X86InstructionProperties_H
#define ROSE_BinaryAnalysis_X86InstructionProperties_H

#include <vector>
#include <stdint.h>

class SgAsmX86Instruction;

namespace Rose {
namespace BinaryAnalysis {

// FIXME[Robb Matzke 2019-03-09]: these should have been members of SgAsmX86Instruction
bool x86InstructionIsConditionalFlagControlTransfer(SgAsmX86Instruction* inst);
bool x86InstructionIsConditionalFlagDataTransfer(SgAsmX86Instruction* inst);
bool x86InstructionIsConditionalControlTransfer(SgAsmX86Instruction* inst);
bool x86InstructionIsConditionalDataTransfer(SgAsmX86Instruction* inst);
bool x86InstructionIsPrivileged(SgAsmX86Instruction*);
bool x86InstructionIsFloatingPoint(SgAsmX86Instruction*);

bool x86InstructionIsConditionalFlagBitAndByte(SgAsmX86Instruction* inst);

bool x86InstructionIsControlTransfer(SgAsmX86Instruction* inst);
bool x86InstructionIsUnconditionalBranch(SgAsmX86Instruction* inst);
bool x86InstructionIsConditionalBranch(SgAsmX86Instruction* inst);
bool x86InstructionIsDataTransfer(SgAsmX86Instruction* inst);

const char* gprToString(X86GeneralPurposeRegister n);
const char* segregToString(X86SegmentRegister n);
const char* flagToString(X86Flag n);

} // namespace
} // namespace

#endif
