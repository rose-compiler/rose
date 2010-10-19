#include "rose.h"
#include "x86AssemblyToC.h"
#include "integerOps.h"
#include "generatedCOpts.h"
#include <boost/static_assert.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

using namespace std;
using namespace IntegerOps;
using namespace SageInterface;
using namespace SageBuilder;

static SgBasicBlock* bb; // Global location to append new statements

static size_t WordWithExpression_nameCounter = 0;

// This builds variable declarations and variable references for each symbolic value
// These are returnd and taken as arguments by the primative functions.
template <size_t Len>
struct WordWithExpression {
  private:
  SgVariableSymbol* sym;
  public:
  WordWithExpression(SgExpression* expr) {
    std::string name = "var" + boost::lexical_cast<std::string>(WordWithExpression_nameCounter);
    ++WordWithExpression_nameCounter;
    SgVariableDeclaration* decl = buildVariableDeclaration(name, SgTypeUnsignedLongLong::createType(), buildAssignInitializer(buildBitAndOp(expr, buildUnsignedLongLongIntValHex(SHL1<unsigned long long, Len>::value - 1))), bb);
    appendStatement(decl, bb);
    sym = getFirstVarSym(decl);
  }
  SgExpression* expr() const {return buildVarRefExp(sym);}
  private: BOOST_STATIC_ASSERT (Len <= 64); // FIXME handle longer operations
};

// Operations build IR nodes to represent the expressions.
struct X86CTranslationPolicy: public CTranslationPolicy {
  X86CTranslationPolicy(SgSourceFile* sourceFile, SgAsmGenericFile* asmFile);

  SgAsmGenericFile* asmFile;
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
  SgStatement* whileBody;
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
  WordWithExpression<1> undefined_() {return buildIntVal(0);}

  template <size_t Len>
  WordWithExpression<Len> and_(WordWithExpression<Len> a, WordWithExpression<Len> b) {
    return buildBitAndOp(a.expr(), b.expr());
  }

  template <size_t Len>
  WordWithExpression<Len> or_(WordWithExpression<Len> a, WordWithExpression<Len> b) {
    return buildBitOrOp(a.expr(), b.expr());
  }

  template <size_t Len>
  WordWithExpression<Len> xor_(WordWithExpression<Len> a, WordWithExpression<Len> b) {
    return buildBitXorOp(a.expr(), b.expr());
  }

  template <size_t Len>
  WordWithExpression<Len> invert(WordWithExpression<Len> a) {
    return buildBitXorOp(a.expr(), buildUnsignedLongLongIntValHex(GenMask<unsigned long long, Len>::value));
  }

  template <size_t Len>
  WordWithExpression<Len> negate(WordWithExpression<Len> a) {
    return buildAddOp(buildBitXorOp(a.expr(), buildUnsignedLongLongIntValHex(GenMask<unsigned long long, Len>::value)), buildIntVal(1));
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
  WordWithExpression<Len1> rotateLeft(WordWithExpression<Len1> a, WordWithExpression<Len2> b) {
    return buildBitOrOp(buildLshiftOp(a.expr(), b.expr()), buildRshiftOp(a.expr(), buildModOp(buildSubtractOp(buildIntVal(Len1), b.expr()), buildIntVal(Len1))));
  }

  template <size_t Len1, size_t Len2>
  WordWithExpression<Len1> rotateRight(WordWithExpression<Len1> a, WordWithExpression<Len2> b) {
    return buildBitOrOp(buildRshiftOp(a.expr(), b.expr()), buildLshiftOp(a.expr(), buildModOp(buildSubtractOp(buildIntVal(Len1), b.expr()), buildIntVal(Len1))));
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
    return buildBitOrOp(
             buildRshiftOp(a.expr(), b.expr()),
             buildConditionalExp(
               buildBitAndOp(a.expr(), buildUnsignedLongLongIntValHex(SHL1<unsigned long long, Len1 - 1>::value)),
               buildBitComplementOp(buildRshiftOp(buildUnsignedLongLongIntValHex(GenMask<unsigned long long, Len1>::value), b.expr())),
               buildUnsignedLongLongIntValHex(0)));
  }

  template <size_t Len1, size_t Len2>
  WordWithExpression<Len1> generateMask(WordWithExpression<Len2> w) { // Set lowest w bits of result
    return buildConditionalExp(
             buildGreaterOrEqualOp(w.expr(), buildIntVal(Len1)),
             buildUnsignedLongLongIntValHex(GenMask<unsigned long long, Len1>::value),
             buildSubtractOp(
               buildLshiftOp(buildUnsignedLongLongIntValHex(1), w.expr()),
               buildIntVal(1)));
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
    return buildBitOrOp(a.expr(), buildConditionalExp(buildNotEqualOp(buildBitAndOp(a.expr(), buildUnsignedLongLongIntValHex(SHL1<unsigned long long, (From - 1)>::value)), buildIntVal(0)), buildUnsignedLongLongIntValHex(SHL1<unsigned long long, To>::value - SHL1<unsigned long long, From>::value), buildIntVal(0)));
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

  WordWithExpression<16> readSegreg(X86SegmentRegister sr) {
    return buildIntValHex(0x2B); // FIXME
  }

  void writeSegreg(X86SegmentRegister sr, WordWithExpression<16> val) {
    // FIXME
  }

  WordWithExpression<32> readIP() {
    return buildVarRefExp(ipSym);
  }

  void writeIP(WordWithExpression<32> val) {
    appendStatement(buildExprStatement(buildAssignOp(buildVarRefExp(ipSym), val.expr())), bb);
  }

  template <size_t Len>
  WordWithExpression<Len> readMemory(X86SegmentRegister segreg, WordWithExpression<32> address, WordWithExpression<1> cond) {
    SgFunctionSymbol* mrSym = NULL;
    switch (Len) {
      case 8: mrSym = memoryReadByteSym; break;
      case 16: mrSym = memoryReadWordSym; break;
      case 32: mrSym = memoryReadDWordSym; break;
      case 64: mrSym = memoryReadQWordSym; break;
      default: ROSE_ASSERT (false);
    }
    ROSE_ASSERT (mrSym);
    return buildConditionalExp(cond.expr(), buildFunctionCallExp(mrSym, buildExprListExp(address.expr())), buildIntVal(0));
  }

  template <size_t Len>
  void writeMemory(X86SegmentRegister segreg, WordWithExpression<32> address, WordWithExpression<Len> data,
                   WordWithExpression<1> cond) {
      SgFunctionSymbol* mwSym = NULL;
      switch (Len) {
        case 8: mwSym = memoryWriteByteSym; break;
        case 16: mwSym = memoryWriteWordSym; break;
        case 32: mwSym = memoryWriteDWordSym; break;
        case 64: mwSym = memoryWriteQWordSym; break;
        default: ROSE_ASSERT (false);
      }
      ROSE_ASSERT (mwSym);
      appendStatement(buildIfStmt(buildNotEqualOp(cond.expr(), buildIntVal(0)),
                                  buildExprStatement(buildFunctionCallExp(mwSym, buildExprListExp(address.expr(),
                                                                                                  data.expr()))),
                                  NULL),
                      bb);
  }
  template <size_t Len>
  void writeMemory(X86SegmentRegister segreg, WordWithExpression<32> address, WordWithExpression<Len> data,
                   WordWithExpression<32> repeat, WordWithExpression<1> cond) {
      writeMemory(segreg, address, data, cond);
  }

// Thee might exist so that static analysis can be used to generate more about the indirect jump.
  WordWithExpression<32> filterIndirectJumpTarget(const WordWithExpression<32>& addr) {
    return addr;
  }

  WordWithExpression<32> filterCallTarget(const WordWithExpression<32>& addr) {
    return addr;
  }

  WordWithExpression<32> filterReturnTarget(const WordWithExpression<32>& addr) {
    return addr;
  }

  void hlt() {
    appendStatement(buildExprStatement(buildFunctionCallExp(abortSym, buildExprListExp())), bb);
  }

  void interrupt(uint8_t num) {
    appendStatement(buildExprStatement(buildFunctionCallExp(interruptSym, buildExprListExp(buildIntVal(num)))), bb);
  }

  void sysenter() {
  }
    
  WordWithExpression<64> rdtsc() {
    return buildUnsignedLongLongIntValHex(0); // FIXME
  }

  void startBlock(uint64_t addr) {
  }

  void finishBlock(uint64_t addr) {
  }

  void startInstruction(SgAsmInstruction* insn) {
    bb = buildBasicBlock();
    appendStatement(buildCaseOptionStmt(buildUnsignedLongLongIntValHex(insn->get_address()), bb), switchBody);
    appendStatement(buildPragmaDeclaration(unparseInstructionWithAddress(insn), bb), bb);
  }

  void finishInstruction(SgAsmInstruction* insn) {
    appendStatement(buildContinueStmt(), bb);
  }

  virtual bool isMemoryWrite(SgFunctionRefExp* func) const {
    return (
        func->get_symbol()->get_declaration() == memoryWriteByteSym->get_declaration() ||
        func->get_symbol()->get_declaration() == memoryWriteWordSym->get_declaration() ||
        func->get_symbol()->get_declaration() == memoryWriteDWordSym->get_declaration() ||
        func->get_symbol()->get_declaration() == memoryWriteQWordSym->get_declaration());
  }
  virtual bool isMemoryRead(SgFunctionRefExp* func) const {
    return (
        func->get_symbol()->get_declaration() == memoryReadByteSym->get_declaration() ||
        func->get_symbol()->get_declaration() == memoryReadWordSym->get_declaration() ||
        func->get_symbol()->get_declaration() == memoryReadDWordSym->get_declaration() ||
        func->get_symbol()->get_declaration() == memoryReadQWordSym->get_declaration());
  }
  virtual bool isVolatileOperation(SgExpression* e) const {
    return (
        isSgFunctionCallExp(e) &&
        isSgFunctionRefExp(isSgFunctionCallExp(e)->get_function()) &&
        isSgFunctionRefExp(isSgFunctionCallExp(e)->get_function())->get_symbol()->get_declaration() == interruptSym->get_declaration());
  }
  virtual SgStatement* getWhileBody() const {return whileBody;}
  virtual SgBasicBlock* getSwitchBody() const {return switchBody;}
  virtual SgVariableSymbol* getIPSymbol() const {return ipSym;}
  virtual const std::map<uint64_t, SgLabelStatement*>& getLabelsForBlocks() const {return labelsForBlocks;}
  virtual const std::set<uint64_t>& getExternallyVisibleBlocks() const {return externallyVisibleBlocks;}
};

#if 0 // Unused
static int sizeOfInsnSize(X86InstructionSize s) {
  switch (s) {
    case x86_insnsize_16: return 2;
    case x86_insnsize_32: return 4;
    case x86_insnsize_64: return 8;
    default: ROSE_ASSERT (!"sizeOfInsnSize");
  }
}
#endif

SgFunctionSymbol* X86CTranslationPolicy::addHelperFunction(const std::string& name, SgType* returnType, SgFunctionParameterList* params) {
  SgFunctionDeclaration* decl = buildNondefiningFunctionDeclaration(name, returnType, params, globalScope);
  appendStatement(decl, globalScope);
  SgFunctionSymbol* sym = globalScope->lookup_function_symbol(name);
  ROSE_ASSERT (sym);
  return sym;
}

X86CTranslationPolicy::X86CTranslationPolicy(SgSourceFile* f, SgAsmGenericFile* asmFile): asmFile(asmFile), globalScope(NULL) {
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

int main(int argc, char** argv) {
  SgProject* proj = frontend(argc, argv);
  ROSE_ASSERT (proj);
  SgSourceFile* newFile = isSgSourceFile(proj->get_fileList().front());
  ROSE_ASSERT(newFile != NULL);
  SgGlobal* g = newFile->get_globalScope();
  ROSE_ASSERT (g);
  SgFunctionDeclaration* decl = buildDefiningFunctionDeclaration("run", SgTypeVoid::createType(), buildFunctionParameterList(), g);
  appendStatement(decl, g);
  vector<SgNode*> asmFiles = NodeQuery::querySubTree(proj, V_SgAsmGenericFile);
  ROSE_ASSERT (asmFiles.size() == 1);
  SgBasicBlock* body = decl->get_definition()->get_body();
  X86CTranslationPolicy policy(newFile, isSgAsmGenericFile(asmFiles[0]));
  policy.switchBody = buildBasicBlock();
  SgSwitchStatement* sw = buildSwitchStatement(buildVarRefExp(policy.ipSym), policy.switchBody);
  SgWhileStmt* whileStmt = buildWhileStmt(buildBoolValExp(true), sw);
  appendStatement(whileStmt, body);
  policy.whileBody = sw;
  X86InstructionSemantics<X86CTranslationPolicy, WordWithExpression> t(policy);
  vector<SgNode*> instructions = NodeQuery::querySubTree(asmFiles[0], V_SgAsmx86Instruction);
  for (size_t i = 0; i < instructions.size(); ++i) {
    SgAsmx86Instruction* insn = isSgAsmx86Instruction(instructions[i]);
    ROSE_ASSERT (insn);
    try {
        t.processInstruction(insn);
    } catch (const X86InstructionSemantics<X86CTranslationPolicy, WordWithExpression>::Exception &e) {
        fprintf(stderr, "%s: %s\n", e.mesg.c_str(), unparseInstructionWithAddress(e.insn).c_str());
    }
  }
  proj->get_fileList().erase(proj->get_fileList().end() - 1); // Remove binary file before calling backend
  AstTests::runAllTests(proj);
  return backend(proj);
}
