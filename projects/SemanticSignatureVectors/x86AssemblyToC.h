#ifndef ROSE_X86_ASSEMBLY_TO_C_H
#define ROSE_X86_ASSEMBLY_TO_C_H

//#include "rose.h"
#include "x86InstructionSemantics.h"

#if 0
SgType* asmTypeToCType(SgAsmType* ty);
SgType* asmTypeToCSignedType(SgAsmType* ty);

struct X86AssemblyToCWithVariables {
  virtual SgStatement* convertInstruction(SgAsmx86Instruction* insn);
  SgExpression* makeRegisterRead(X86RegisterClass cl, int num, X86PositionInRegister pos);
  SgStatement* makeRegisterWrite(X86RegisterClass cl, int num, X86PositionInRegister pos, SgExpression* value);
  SgExpression* makeMemoryRead(X86SegmentRegister segreg, SgExpression* address, SgAsmType* ty);
  SgStatement* makeMemoryWrite(X86SegmentRegister segreg, SgExpression* address, SgExpression* data, SgAsmType* ty);
  SgExpression* makeFlagRead(X86Flag flag);
  SgExpression* makeCondition(X86Condition cond);
  SgStatement* makeFlagWrite(X86Flag flag, SgExpression* value);
  SgStatement* makeGotoNextInstruction(uint64_t currentAddr, uint64_t nextAddr);
  SgStatement* makeJump(SgExpression* newAddr);
  SgStatement* makeConditionalJump(uint64_t currentAddr, SgExpression* cond, uint64_t newAddr, uint64_t nextAddr);
  SgFunctionSymbol* getHelperFunction(const std::string& name);
  SgStatement* makeDispatchSwitch(SgExpression* ipExpr);

  public:
  SgBasicBlock* makeAllCode(SgBasicBlock* appendTo);
};
#endif

#endif // ROSE_X86_ASSEMBLY_TO_C_H
