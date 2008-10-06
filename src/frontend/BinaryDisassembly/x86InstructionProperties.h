#ifndef ROSE_X86INSTRUCTIONPROPERTIES_H
#define ROSE_X86INSTRUCTIONPROPERTIES_H

#include <vector>
#include <stdint.h>

class SgAsmx86Instruction;

bool x86InstructionIsConditionalFlagControlTransfer(SgAsmx86Instruction* inst);
bool x86InstructionIsConditionalFlagDataTransfer(SgAsmx86Instruction* inst);
bool x86InstructionIsConditionalControlTransfer(SgAsmx86Instruction* inst);
bool x86InstructionIsConditionalDataTransfer(SgAsmx86Instruction* inst);

bool x86InstructionIsConditionalFlagBitAndByte(SgAsmx86Instruction* inst);

bool x86InstructionIsControlTransfer(SgAsmx86Instruction* inst);
bool x86InstructionIsUnconditionalBranch(SgAsmx86Instruction* inst);
bool x86InstructionIsConditionalBranch(SgAsmx86Instruction* inst);
bool x86InstructionIsDataTransfer(SgAsmx86Instruction* inst);
bool x86GetKnownBranchTarget(SgAsmx86Instruction* insn, uint64_t& addr);
// SgAsmx86Instruction* x86GetInstructionDestination(SgAsmx86Instruction* inst); // Returns non-fallthrough destination
// std::vector<SgAsmx86Instruction*> x86GetInstructionOutEdges(SgAsmx86Instruction* inst); // Returns all possible targets and fallthrough

const char* regclassToString(X86RegisterClass n);
const char* gprToString(X86GeneralPurposeRegister n);
const char* segregToString(X86SegmentRegister n);
const char* flagToString(X86Flag n);

#endif // ROSE_X86INSTRUCTIONPROPERTIES_H
