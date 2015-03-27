#ifndef ROSE_X86INSTRUCTIONPROPERTIES_H
#define ROSE_X86INSTRUCTIONPROPERTIES_H

#include <vector>
#include <stdint.h>

class SgAsmX86Instruction;

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

#endif // ROSE_X86INSTRUCTIONPROPERTIES_H
