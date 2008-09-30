#include "rose.h"
#include "x86AssemblyToC.h"
#include <boost/static_assert.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using X86Disassembler::sizeToType;
using X86Disassembler::sizeToPos;

SgBasicBlock* bb; // Global location to append new statements

static size_t WordWithExpression_nameCounter = 0;

template <size_t Len>
struct WordWithExpression {
  private:
  SgVariableSymbol* sym;
  public:
  WordWithExpression(SgExpression* expr) {
    std::string name = "var" + boost::lexical_cast<std::string>(WordWithExpression_nameCounter);
    ++WordWithExpression_nameCounter;
    SgVariableDeclaration* decl = buildVariableDeclaration(name, SgTypeUnsignedLongLong::createType(), buildAssignInitializer(buildBitAndOp(expr, buildUnsignedLongLongIntValHex((Len == 64 ? 0 : (1ULL << Len)) - 1))), bb);
    appendStatement(decl, bb);
    sym = getFirstVarSym(decl);
  }
  SgExpression* expr() const {return buildVarRefExp(sym);}
  private: BOOST_STATIC_ASSERT (Len <= 64); // FIXME handle longer operations
};

struct CTranslationPolicy {
  template <size_t Len>
  struct wordType {typedef WordWithExpression<Len> type;};

  CTranslationPolicy(SgSourceFile* sourceFile, SgAsmFile* asmFile);

  SgAsmFile* asmFile;
  SgGlobal* globalScope;
  SgFunctionSymbol* paritySym;
  SgFunctionSymbol* mulhi16Sym;
  SgFunctionSymbol* mulhi32Sym;
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
  // SgVariableSymbol* gprLowByteSym[16];
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
  std::map<uint64_t, SgAsmBlock*> blocks;
  std::map<uint64_t, SgLabelStatement*> labelsForBlocks;
  std::set<uint64_t> externallyVisibleBlocks;

  SgFunctionSymbol* addHelperFunction(const std::string& name, SgType* returnType, SgFunctionParameterList* params);

  template <size_t Len>
  WordWithExpression<Len> number(uint64_t n) {
    return buildUnsignedLongLongIntValHex(n);
  }

  template <size_t From, size_t To, size_t Len>
  WordWithExpression<To - From> extract(WordWithExpression<Len> a) {
    return (From == 0) ? a.expr() : buildRshiftOp(a.expr(), buildIntVal(From)); // Other bits will automatically be masked off
  }

  template <size_t Len1, size_t Len2>
  WordWithExpression<Len1 + Len2> concat(WordWithExpression<Len1> a, WordWithExpression<Len2> b) {
    // Concats a on LSB side of b
    return buildBitOrOp(a.expr(), buildLshiftOp(b.expr(), buildIntVal(Len1)));
  }

  WordWithExpression<1> true_() {return buildIntVal(1);}
  WordWithExpression<1> false_() {return buildIntVal(0);}

  template <size_t Len>
  WordWithExpression<Len> and_(WordWithExpression<Len> a, WordWithExpression<Len> b) {
    return buildBitAndOp(a.expr(), b.expr());
  }

  template <size_t Len>
  WordWithExpression<Len> or_(WordWithExpression<Len> a, WordWithExpression<Len> b) {
    return buildBitOrOp(a.expr(), b.expr());
  }

  template <size_t Len>
  WordWithExpression<Len> nor_(WordWithExpression<Len> a, WordWithExpression<Len> b) {
    return buildBitComplementOp(buildBitOrOp(a.expr(), b.expr()));
  }

  template <size_t Len>
  WordWithExpression<Len> xor_(WordWithExpression<Len> a, WordWithExpression<Len> b) {
    return buildBitXorOp(a.expr(), b.expr());
  }

  template <size_t Len>
  WordWithExpression<1> parity(WordWithExpression<Len> a) {
    return buildFunctionCallExp(paritySym, buildExprListExp(a.expr()));
  }

  template <size_t Len>
  WordWithExpression<Len> invert(WordWithExpression<Len> a) {
    return buildBitXorOp(a.expr(), buildUnsignedLongLongIntValHex((1ULL << Len) - 1));
  }

  template <size_t Len>
  WordWithExpression<Len> ite(WordWithExpression<1> sel, WordWithExpression<Len> a, WordWithExpression<Len> b) {
    return buildConditionalExp(sel.expr(), a.expr(), b.expr());
  }

  template <size_t Len>
  WordWithExpression<1> equalToZero(WordWithExpression<Len> a) {
    return buildEqualityOp(a.expr(), buildIntVal(0));
  }

  template <size_t Len>
  WordWithExpression<1> equalToNegativeOne(WordWithExpression<Len> a) {
    return buildEqualityOp(a.expr(), buildUnsignedLongLongIntValHex((1ULL << Len) - 1));
  }

  template <size_t Len>
  WordWithExpression<1> notEqualToZero(WordWithExpression<Len> a) {
    return buildNotEqualOp(a.expr(), buildIntVal(0));
  }

  template <size_t Len>
  WordWithExpression<1> greaterOrEqual(WordWithExpression<Len> a, WordWithExpression<Len> b) {
    return buildGreaterOrEqualOp(a.expr(), b.expr());
  }

  template <size_t Len>
  WordWithExpression<Len> add(WordWithExpression<Len> a, WordWithExpression<Len> b) {
    return buildAddOp(a.expr(), b.expr());
  }

  template <size_t Len>
  WordWithExpression<Len> addWithCarries(WordWithExpression<Len> a, WordWithExpression<Len> b, WordWithExpression<1> carryIn, WordWithExpression<Len>& carries) {
    WordWithExpression<Len + 1> e = buildAddOp(a.expr(), buildAddOp(b.expr(), carryIn.expr()));
    carries = buildRshiftOp(
                buildBitXorOp(buildBitXorOp(a.expr(), b.expr()), e.expr()),
                buildIntVal(1));
    return extract<0, Len>(e);
  }

  template <size_t Len1, size_t Len2>
  WordWithExpression<Len1> shiftLeft(WordWithExpression<Len1> a, WordWithExpression<Len2> b) {
    return buildLshiftOp(a.expr(), b.expr());
  }

  template <size_t Len1, size_t Len2>
  WordWithExpression<Len1> shiftRight(WordWithExpression<Len1> a, WordWithExpression<Len2> b) {
    return buildRshiftOp(a.expr(), b.expr());
  }

  template <size_t Len1, size_t Len2>
  WordWithExpression<Len1> shiftRightArithmetic(WordWithExpression<Len1> a, WordWithExpression<Len2> b) {
    return buildBitOrOp(buildRshiftOp(a.expr(), b.expr()),
                        buildConditionalExp(buildNotEqualOp(buildBitAndOp(a.expr(), buildUnsignedLongLongIntValHex(1ULL << (Len1 - 1))),
                                                            buildIntVal(0)),
                                            buildBitComplementOp(buildRshiftOp(buildUnsignedLongLongIntValHex((1ULL << Len1) - 1),
                                                                               b.expr())),
                                            buildIntVal(0)));
  }

  template <size_t Len1, size_t Len2>
  WordWithExpression<Len1> rotateLeft(WordWithExpression<Len1> a, WordWithExpression<Len2> b) {
    return buildBitOrOp(buildLshiftOp(a.expr(), b.expr()), buildRshiftOp(a.expr(), buildSubtractOp(buildIntVal(Len1), b.expr())));
  }

  template <size_t Len1, size_t Len2>
  WordWithExpression<Len1> rotateRight(WordWithExpression<Len1> a, WordWithExpression<Len2> b) {
    return buildBitOrOp(buildRshiftOp(a.expr(), b.expr()), buildLshiftOp(a.expr(), buildSubtractOp(buildIntVal(Len1), b.expr())));
  }

  template <size_t Len1, size_t Len2>
  WordWithExpression<Len1 + Len2> unsignedMultiply(WordWithExpression<Len1> a, WordWithExpression<Len2> b) {
    return buildMultiplyOp(a.expr(), b.expr());
  }

  // FIXME
  template <size_t Len1, size_t Len2>
  WordWithExpression<Len1 + Len2> signedMultiply(WordWithExpression<Len1> a, WordWithExpression<Len2> b) {
    return buildMultiplyOp(a.expr(), b.expr());
  }

  template <size_t Len1, size_t Len2>
  WordWithExpression<Len1> unsignedDivide(WordWithExpression<Len1> a, WordWithExpression<Len2> b) {
    return buildDivideOp(a.expr(), b.expr());
  }

  template <size_t Len1, size_t Len2>
  WordWithExpression<Len2> unsignedModulo(WordWithExpression<Len1> a, WordWithExpression<Len2> b) {
    return buildModOp(a.expr(), b.expr());
  }

  // FIXME
  template <size_t Len1, size_t Len2>
  WordWithExpression<Len1> signedDivide(WordWithExpression<Len1> a, WordWithExpression<Len2> b) {
    return buildDivideOp(a.expr(), b.expr());
  }

  // FIXME
  template <size_t Len1, size_t Len2>
  WordWithExpression<Len2> signedModulo(WordWithExpression<Len1> a, WordWithExpression<Len2> b) {
    return buildModOp(a.expr(), b.expr());
  }

  template <size_t From, size_t To>
  WordWithExpression<To> signExtend(WordWithExpression<From> a) {
    return buildBitOrOp(a.expr(), buildConditionalExp(buildNotEqualOp(buildBitAndOp(a.expr(), buildUnsignedLongLongIntValHex(1ULL << (From - 1))), buildIntVal(0)), buildUnsignedLongLongIntValHex((To == 64 ? 0 : (1ULL << To)) - (1ULL << From)), buildIntVal(0)));
  }

  template <size_t Len>
  WordWithExpression<Len> leastSignificantSetBit(WordWithExpression<Len> a) {
    return buildFunctionCallExp(bsfSym, buildExprListExp(a.expr()));
  }

  template <size_t Len>
  WordWithExpression<Len> mostSignificantSetBit(WordWithExpression<Len> a) {
    return buildFunctionCallExp(bsrSym, buildExprListExp(a.expr()));
  }

  WordWithExpression<1> readFlag(X86Flag flag) {
    SgVariableSymbol* fl = flagsSym[flag];
    ROSE_ASSERT (fl);
    return buildVarRefExp(fl);
  }

  void writeFlag(X86Flag flag, WordWithExpression<1> value) {
    SgVariableSymbol* fl = flagsSym[flag];
    ROSE_ASSERT (fl);
    appendStatement(buildAssignStatement(buildVarRefExp(fl), value.expr()), bb);
  }

  WordWithExpression<32> readGPR(X86GeneralPurposeRegister num) {
    return buildVarRefExp(gprSym[num]);
  }

  void writeGPR(X86GeneralPurposeRegister num, WordWithExpression<32> val) {
    appendStatement(buildExprStatement(buildAssignOp(buildVarRefExp(gprSym[num]), val.expr())), bb);
  }

  WordWithExpression<32> readIP() {
    return buildVarRefExp(ipSym);
  }

  void writeIP(WordWithExpression<32> val) {
    appendStatement(buildExprStatement(buildAssignOp(buildVarRefExp(ipSym), val.expr())), bb);
  }

  template <size_t Len>
  WordWithExpression<Len> readMemory(X86SegmentRegister segreg, WordWithExpression<32> address) {
    SgFunctionSymbol* mrSym = NULL;
    switch (Len) {
      case 8: mrSym = memoryReadByteSym; break;
      case 16: mrSym = memoryReadWordSym; break;
      case 32: mrSym = memoryReadDWordSym; break;
      case 64: mrSym = memoryReadQWordSym; break;
      default: ROSE_ASSERT (false);
    }
    ROSE_ASSERT (mrSym);
    return buildFunctionCallExp(mrSym, buildExprListExp(address.expr()));
  }

  template <size_t Len>
  void writeMemory(X86SegmentRegister segreg, WordWithExpression<32> address, WordWithExpression<Len> data) {
    SgFunctionSymbol* mwSym = NULL;
    switch (Len) {
      case 8: mwSym = memoryWriteByteSym; break;
      case 16: mwSym = memoryWriteWordSym; break;
      case 32: mwSym = memoryWriteDWordSym; break;
      case 64: mwSym = memoryWriteQWordSym; break;
      default: ROSE_ASSERT (false);
    }
    ROSE_ASSERT (mwSym);
    appendStatement(buildExprStatement(buildFunctionCallExp(mwSym, buildExprListExp(address.expr(), data.expr()))), bb);
  }

  void hlt() {
    appendStatement(buildExprStatement(buildFunctionCallExp(abortSym, buildExprListExp())), bb);
  }

  void interrupt(uint8_t num) {
    appendStatement(buildExprStatement(buildFunctionCallExp(interruptSym, buildExprListExp(buildIntVal(num)))), bb);
  }

  WordWithExpression<64> rdtsc() {
    return buildUnsignedLongLongIntValHex(0); // FIXME
  }

  void startBlock(uint64_t addr) {
    bb = buildBasicBlock();
    appendStatement(buildCaseOptionStmt(buildUnsignedLongLongIntValHex(addr), bb), switchBody);
  }

  void finishBlock(uint64_t addr) {
    appendStatement(buildContinueStmt(), bb);
  }

  void startInstruction(SgAsmInstruction* insn) {
    appendStatement(buildPragmaDeclaration(unparseInstructionWithAddress(insn), bb), bb);
  }

  void finishInstruction(SgAsmInstruction* insn) {}
};

static int sizeOfInsnSize(X86InstructionSize s) {
  switch (s) {
    case x86_insnsize_16: return 2;
    case x86_insnsize_32: return 4;
    case x86_insnsize_64: return 8;
    default: ROSE_ASSERT (!"sizeOfInsnSize");
  }
}

SgFunctionSymbol* CTranslationPolicy::addHelperFunction(const std::string& name, SgType* returnType, SgFunctionParameterList* params) {
  SgFunctionDeclaration* decl = buildNondefiningFunctionDeclaration(name, returnType, params, globalScope);
  appendStatement(decl, globalScope);
  SgFunctionSymbol* sym = globalScope->lookup_function_symbol(name);
  ROSE_ASSERT (sym);
  return sym;
}

CTranslationPolicy::CTranslationPolicy(SgSourceFile* f, SgAsmFile* asmFile): asmFile(asmFile), globalScope(NULL) {
  ROSE_ASSERT (f);
  ROSE_ASSERT (f->get_globalScope());
  globalScope = f->get_globalScope();
#define LOOKUP_FUNC(name) \
  do {name##Sym = globalScope->lookup_function_symbol(#name); ROSE_ASSERT (name##Sym);} while (0)
  LOOKUP_FUNC(parity);
  LOOKUP_FUNC(mulhi16);
  LOOKUP_FUNC(mulhi32);
  LOOKUP_FUNC(mulhi64);
  LOOKUP_FUNC(imulhi16);
  LOOKUP_FUNC(imulhi32);
  LOOKUP_FUNC(imulhi64);
  LOOKUP_FUNC(div8);
  LOOKUP_FUNC(mod8);
  LOOKUP_FUNC(div16);
  LOOKUP_FUNC(mod16);
  LOOKUP_FUNC(div32);
  LOOKUP_FUNC(mod32);
  LOOKUP_FUNC(div64);
  LOOKUP_FUNC(mod64);
  LOOKUP_FUNC(idiv8);
  LOOKUP_FUNC(imod8);
  LOOKUP_FUNC(idiv16);
  LOOKUP_FUNC(imod16);
  LOOKUP_FUNC(idiv32);
  LOOKUP_FUNC(imod32);
  LOOKUP_FUNC(idiv64);
  LOOKUP_FUNC(imod64);
  LOOKUP_FUNC(bsr);
  LOOKUP_FUNC(bsf);
  gprSym[0] = globalScope->lookup_variable_symbol("rax"); ROSE_ASSERT (gprSym[0]);
  gprSym[1] = globalScope->lookup_variable_symbol("rcx"); ROSE_ASSERT (gprSym[1]);
  gprSym[2] = globalScope->lookup_variable_symbol("rdx"); ROSE_ASSERT (gprSym[2]);
  gprSym[3] = globalScope->lookup_variable_symbol("rbx"); ROSE_ASSERT (gprSym[3]);
  gprSym[4] = globalScope->lookup_variable_symbol("rsp"); ROSE_ASSERT (gprSym[4]);
  gprSym[5] = globalScope->lookup_variable_symbol("rbp"); ROSE_ASSERT (gprSym[5]);
  gprSym[6] = globalScope->lookup_variable_symbol("rsi"); ROSE_ASSERT (gprSym[6]);
  gprSym[7] = globalScope->lookup_variable_symbol("rdi"); ROSE_ASSERT (gprSym[7]);
  gprSym[8] = globalScope->lookup_variable_symbol("r8"); ROSE_ASSERT (gprSym[8]);
  gprSym[9] = globalScope->lookup_variable_symbol("r9"); ROSE_ASSERT (gprSym[9]);
  gprSym[10] = globalScope->lookup_variable_symbol("r10"); ROSE_ASSERT (gprSym[10]);
  gprSym[11] = globalScope->lookup_variable_symbol("r11"); ROSE_ASSERT (gprSym[11]);
  gprSym[12] = globalScope->lookup_variable_symbol("r12"); ROSE_ASSERT (gprSym[12]);
  gprSym[13] = globalScope->lookup_variable_symbol("r13"); ROSE_ASSERT (gprSym[13]);
  gprSym[14] = globalScope->lookup_variable_symbol("r14"); ROSE_ASSERT (gprSym[14]);
  gprSym[15] = globalScope->lookup_variable_symbol("r15"); ROSE_ASSERT (gprSym[15]);
  flagsSym[0] = globalScope->lookup_variable_symbol("cf"); ROSE_ASSERT (flagsSym[0]);
  flagsSym[1] = NULL;
  flagsSym[2] = globalScope->lookup_variable_symbol("pf"); ROSE_ASSERT (flagsSym[2]);
  flagsSym[3] = NULL;
  flagsSym[4] = globalScope->lookup_variable_symbol("af"); ROSE_ASSERT (flagsSym[4]);
  flagsSym[5] = NULL;
  flagsSym[6] = globalScope->lookup_variable_symbol("zf"); ROSE_ASSERT (flagsSym[6]);
  flagsSym[7] = globalScope->lookup_variable_symbol("sf"); ROSE_ASSERT (flagsSym[7]);
  flagsSym[8] = NULL;
  flagsSym[9] = NULL;
  flagsSym[10] = globalScope->lookup_variable_symbol("df"); ROSE_ASSERT (flagsSym[10]);
  flagsSym[11] = globalScope->lookup_variable_symbol("of"); ROSE_ASSERT (flagsSym[11]);
  flagsSym[12] = NULL;
  flagsSym[13] = NULL;
  flagsSym[14] = NULL;
  flagsSym[15] = NULL;
  ipSym = globalScope->lookup_variable_symbol("ip"); ROSE_ASSERT (ipSym);
  LOOKUP_FUNC(memoryReadByte);
  LOOKUP_FUNC(memoryReadWord);
  LOOKUP_FUNC(memoryReadDWord);
  LOOKUP_FUNC(memoryReadQWord);
  LOOKUP_FUNC(memoryWriteByte);
  LOOKUP_FUNC(memoryWriteWord);
  LOOKUP_FUNC(memoryWriteDWord);
  LOOKUP_FUNC(memoryWriteQWord);
  LOOKUP_FUNC(abort);
  LOOKUP_FUNC(interrupt);
  LOOKUP_FUNC(startingInstruction);
#undef LOOKUP_FUNC
}

#if 0
SgStatement* X86AssemblyToCWithVariables::makeGotoNextInstruction(uint64_t currentAddr, uint64_t nextIP) {
#if 1
  map<uint64_t, SgAsmBlock*>::const_iterator i = blocks.find(nextIP);
  if (i != blocks.end()) { // Start of new block
    return makeJump(buildUnsignedLongLongIntValHex(nextIP));
  } else {
    return buildBasicBlock(); // Fall through in switch statement
  }
#endif
  // FIXME: What if we are falling off the end of the set of instructions disassembled
  // return buildBasicBlock(); // Always fall through
}

SgStatement* X86AssemblyToCWithVariables::makeJump(SgExpression* newAddr) {
  return makeDispatchSwitch(newAddr);
}

SgStatement* X86AssemblyToCWithVariables::makeConditionalJump(uint64_t currentAddr, SgExpression* cond, uint64_t newAddr, uint64_t nextIP) {
  return buildIfStmt(
           cond,
           buildBasicBlock(makeJump(buildUnsignedLongLongIntValHex(newAddr))),
           buildBasicBlock(makeJump(buildUnsignedLongLongIntValHex(nextIP))));
}

SgFunctionSymbol* X86AssemblyToCWithVariables::getHelperFunction(const string& name) {
#define DO_NAME(n) do {if (name == #n) return n##Sym;} while (0)
  DO_NAME(parity);
  DO_NAME(mulhi16);
  DO_NAME(mulhi32);
  DO_NAME(mulhi64);
  DO_NAME(imulhi16);
  DO_NAME(imulhi32);
  DO_NAME(imulhi64);
  DO_NAME(div8);
  DO_NAME(mod8);
  DO_NAME(div16);
  DO_NAME(mod16);
  DO_NAME(div32);
  DO_NAME(mod32);
  DO_NAME(div64);
  DO_NAME(mod64);
  DO_NAME(idiv8);
  DO_NAME(imod8);
  DO_NAME(idiv16);
  DO_NAME(imod16);
  DO_NAME(idiv32);
  DO_NAME(imod32);
  DO_NAME(idiv64);
  DO_NAME(imod64);
  DO_NAME(bsr);
  DO_NAME(bsf);
  DO_NAME(abort);
  DO_NAME(interrupt);
  cerr << "Bad name " << name << endl;
  abort();
}

SgStatement* X86AssemblyToCWithVariables::makeDispatchSwitch(SgExpression* ipExpr) {
  if (isSgValueExp(ipExpr)) { // Special case for constants
    uint64_t addr = getIntegerConstantValue(isSgValueExp(ipExpr));
    map<uint64_t, SgLabelStatement*>::const_iterator it = labelsForBlocks.find(addr);
    ROSE_ASSERT (it != labelsForBlocks.end());
    SgLabelStatement* ls = it->second;
    ROSE_ASSERT (ls);
    return buildGotoStatement(ls);
  }
  SgBasicBlock* switchBody = buildBasicBlock();
  SgSwitchStatement* sw = buildSwitchStatement(ipExpr, switchBody);
  vector<SgNode*> asmBlocks = NodeQuery::querySubTree(asmFile, V_SgAsmBlock);
  for (size_t i = 0; i < asmBlocks.size(); ++i) {
    SgAsmBlock* bb = isSgAsmBlock(asmBlocks[i]);
    map<uint64_t, SgLabelStatement*>::const_iterator it = labelsForBlocks.find(bb->get_address());
    ROSE_ASSERT (it != labelsForBlocks.end());
    SgLabelStatement* ls = it->second;
    ROSE_ASSERT (ls);
    if (bb->get_externallyVisible()) {
      appendStatement(
        buildCaseOptionStmt(
          buildUnsignedLongLongIntValHex(bb->get_address()),
          buildBasicBlock(buildGotoStatement(ls))),
        switchBody);
    }
  }
  appendStatement(
    buildDefaultOptionStmt(
      buildBasicBlock(
        buildExprStatement(
          buildFunctionCallExp(
            abortSym,
            buildExprListExp())))),
    switchBody);
  return sw;
}

SgBasicBlock* X86AssemblyToCWithVariables::makeAllCode(SgBasicBlock* appendTo) {
  SgBasicBlock* body = appendTo;
  whileBody = body;
#if 0
  appendStatement(
    buildWhileStmt(
      buildBoolValExp(true),
      whileBody),
    body);
  appendStatement(whileBody, body);
#endif
  vector<SgNode*> asmBlocks = NodeQuery::querySubTree(asmFile, V_SgAsmBlock);
  for (size_t i = 0; i < asmBlocks.size(); ++i) {
    SgAsmBlock* bb = isSgAsmBlock(asmBlocks[i]);
    blocks.insert(std::make_pair(bb->get_address(), bb));
    SgLabelStatement* ls = buildLabelStatement("label_" + StringUtility::intToHex(bb->get_address()), buildBasicBlock(), whileBody);
    labelsForBlocks.insert(std::make_pair(bb->get_address(), ls));
    appendStatement(ls, whileBody);
    if (bb->get_externallyVisible()) {
      externallyVisibleBlocks.insert(bb->get_address());
    }
  }

  SgStatement* sw = makeDispatchSwitch(buildUnsignedLongLongIntValHex(asmFile->get_interpretations()[0]->get_header()->get_entry_rva()));

  prependStatement(sw, whileBody);
  prependStatement(
    buildAssignStatement(
      buildVarRefExp(flagsSym[x86flag_df]),
      buildBoolValExp(false)),
    body);
#if 0
  prependStatement(
    buildExprStatement(
      buildFunctionCallExp(
        startingInstructionSym,
        buildExprListExp())),
    whileBody);
#endif
  for (size_t i = 0; i < asmBlocks.size(); ++i) {
    SgAsmBlock* bb = isSgAsmBlock(asmBlocks[i]);
    const SgAsmStatementPtrList& stmts = bb->get_statementList();
    if (stmts.empty()) continue;
    if (isSgAsmBlock(stmts.front())) continue; // This is an enclosing block that doesn't have any instructions
    uint64_t addr = stmts.front()->get_address();
    SgBasicBlock* caseBody = buildBasicBlock();
    while (true) { // Keep processing basic blocks until there is a control transfer (i.e., append blocks that are only split because they can be entered from elsewhere)
      const SgAsmStatementPtrList& stmts = bb->get_statementList();
      ROSE_ASSERT (!stmts.empty());
      for (size_t j = 0; j < stmts.size(); ++j) {
        SgAsmx86Instruction* insn = isSgAsmx86Instruction(stmts[j]);
        ROSE_ASSERT (insn);
        appendStatement(buildPragmaDeclaration(unparseInstructionWithAddress(insn), caseBody), caseBody);
        appendStatement(convertInstruction(isSgAsmx86Instruction(stmts[j])), caseBody);
      }
      SgAsmx86Instruction* lastInsn = isSgAsmx86Instruction(stmts.back());
      if (false /* !x86InstructionIsControlTransfer(lastInsn) */ ) {
        map<uint64_t, SgAsmBlock*>::const_iterator it = blocks.find(lastInsn->get_address() + lastInsn->get_raw_bytes().size());
        if (it == blocks.end()) break;
        bb = it->second;
      } else {
        break;
      }
    }
    // flattenBlocks(caseBody);
    insertStatementAfter(labelsForBlocks[addr], caseBody);
  }
  return body;
}
#endif

int main(int argc, char** argv) {
  SgProject* proj = frontend(argc, argv);
  ROSE_ASSERT (proj);
  SgSourceFile* newFile = isSgSourceFile(proj->get_fileList().front());
  ROSE_ASSERT(newFile != NULL);
  SgGlobal* g = newFile->get_globalScope();
  ROSE_ASSERT (g);
  SgFunctionDeclaration* decl = buildDefiningFunctionDeclaration("run", SgTypeVoid::createType(), buildFunctionParameterList(), g);
  appendStatement(decl, g);
  vector<SgNode*> asmFiles = NodeQuery::querySubTree(proj, V_SgAsmFile);
  ROSE_ASSERT (asmFiles.size() == 1);
  SgBasicBlock* body = decl->get_definition()->get_body();
  CTranslationPolicy policy(newFile, isSgAsmFile(asmFiles[0]));
  policy.switchBody = buildBasicBlock();
  SgSwitchStatement* sw = buildSwitchStatement(buildVarRefExp(policy.ipSym), policy.switchBody);
  SgWhileStmt* whileStmt = buildWhileStmt(buildBoolValExp(true), sw);
  appendStatement(whileStmt, body);
  X86InstructionSemantics<CTranslationPolicy> t(policy);
  vector<SgNode*> blocks = NodeQuery::querySubTree(asmFiles[0], V_SgAsmBlock);
  for (size_t i = 0; i < blocks.size(); ++i) {
    SgAsmBlock* b = isSgAsmBlock(blocks[i]);
    ROSE_ASSERT (b);
    t.processBlock(b);
  }
  proj->get_fileList().erase(proj->get_fileList().end() - 1); // Remove binary file before calling backend
  AstTests::runAllTests(proj);
  return backend(proj);
}
