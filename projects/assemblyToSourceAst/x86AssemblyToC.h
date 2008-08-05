#ifndef ROSE_X86_ASSEMBLY_TO_C_H
#define ROSE_X86_ASSEMBLY_TO_C_H

#include "rose.h"

SgType* asmTypeToCType(SgAsmType* ty);
SgType* asmTypeToCSignedType(SgAsmType* ty);

enum X86Flag { // These match the bit positions in rFLAGS, only the user-mode flags are in here
  x86flag_cf = 0,
  x86flag_pf = 2,
  x86flag_af = 4,
  x86flag_zf = 6,
  x86flag_sf = 7,
  x86flag_df = 10,
  x86flag_of = 11
};

enum X86Condition {
  x86cond_o,
  x86cond_no,
  x86cond_b,
  x86cond_ae,
  x86cond_e,
  x86cond_ne,
  x86cond_be,
  x86cond_a,
  x86cond_s,
  x86cond_ns,
  x86cond_pe,
  x86cond_po,
  x86cond_l,
  x86cond_ge,
  x86cond_le,
  x86cond_g
};

class X86AssemblyToCFramework {
  protected:
  // Subclasses must implement these
  virtual SgExpression* makeRegisterRead(X86RegisterClass cl, int num, X86PositionInRegister pos) = 0;
  virtual SgStatement* makeRegisterWrite(X86RegisterClass cl, int num, X86PositionInRegister pos, SgExpression* value) = 0;
  virtual SgExpression* makeMemoryRead(X86SegmentRegister segreg, SgExpression* address, SgAsmType* ty) = 0;
  virtual SgStatement* makeMemoryWrite(X86SegmentRegister segreg, SgExpression* address, SgExpression* data, SgAsmType* ty) = 0;
  virtual SgExpression* makeFlagRead(X86Flag flag) = 0;
  virtual SgExpression* makeCondition(X86Condition cond) = 0;
  virtual SgStatement* makeFlagWrite(X86Flag flag, SgExpression* value) = 0;
  virtual SgStatement* makeGotoNextInstruction(uint64_t currentAddr, uint64_t nextAddr) = 0;
  virtual SgStatement* makeJump(SgExpression* newAddr) = 0;
  virtual SgStatement* makeConditionalJump(uint64_t currentAddr, SgExpression* cond, uint64_t newAddr, uint64_t nextAddr) = 0;
  virtual SgFunctionSymbol* getHelperFunction(const std::string& name) = 0;

  public:
  virtual SgStatement* convertInstruction(SgAsmx86Instruction* insn);
  friend class SingleInstructionTranslator;
};

class X86AssemblyToCWithVariables: public X86AssemblyToCFramework {
  public: // For external access to symbols
  SgGlobal* globalScope;
  SgFunctionSymbol* paritySym;
  SgFunctionSymbol* mulhi64Sym;
  SgFunctionSymbol* imulhi16Sym;
  SgFunctionSymbol* imulhi32Sym;
  SgFunctionSymbol* imulhi64Sym;
  SgFunctionSymbol* div8Sym;
  SgFunctionSymbol* mod8Sym;
  SgFunctionSymbol* div16Sym;
  SgFunctionSymbol* mod16Sym;
  SgFunctionSymbol* div32Sym;
  SgFunctionSymbol* mod32Sym;
  SgFunctionSymbol* div64Sym;
  SgFunctionSymbol* mod64Sym;
  SgFunctionSymbol* idiv8Sym;
  SgFunctionSymbol* imod8Sym;
  SgFunctionSymbol* idiv16Sym;
  SgFunctionSymbol* imod16Sym;
  SgFunctionSymbol* idiv32Sym;
  SgFunctionSymbol* imod32Sym;
  SgFunctionSymbol* idiv64Sym;
  SgFunctionSymbol* imod64Sym;
  SgFunctionSymbol* bsrSym;
  SgFunctionSymbol* bsfSym;
  SgVariableSymbol* gprSym[16];
  SgVariableSymbol* gprLowByteSym[16];
  SgVariableSymbol* ipSym;
  SgVariableSymbol* flagsSym[16];
  SgVariableSymbol* sf_xor_ofSym;
  SgVariableSymbol* zf_or_cfSym;
  SgFunctionSymbol* memoryReadByteSym;
  SgFunctionSymbol* memoryReadWordSym;
  SgFunctionSymbol* memoryReadDWordSym;
  SgFunctionSymbol* memoryReadQWordSym;
  SgFunctionSymbol* memoryWriteByteSym;
  SgFunctionSymbol* memoryWriteWordSym;
  SgFunctionSymbol* memoryWriteDWordSym;
  SgFunctionSymbol* memoryWriteQWordSym;
  SgFunctionSymbol* abortSym;
  SgFunctionSymbol* interruptSym;
  SgFunctionSymbol* startingInstructionSym;
  SgBasicBlock* switchBody;
  SgBasicBlock* whileBody;
  std::map<uint64_t, SgAsmBlock*> blocks;
  std::map<uint64_t, SgLabelStatement*> labelsForBlocks;
  std::set<uint64_t> externallyVisibleBlocks;

  SgFunctionSymbol* addHelperFunction(const std::string& name, SgType* returnType, SgFunctionParameterList* params);

  protected:
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

  public:
  X86AssemblyToCWithVariables(SgFile* templateFile);
  SgBasicBlock* makeAllCode(SgAsmFile* asmFile, SgBasicBlock* appendTo);
  SgStatement* makeDispatchSwitch(SgAsmFile* f);
};

#endif // ROSE_X86_ASSEMBLY_TO_C_H
