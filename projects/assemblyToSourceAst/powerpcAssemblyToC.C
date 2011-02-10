#include "rose.h"
#include "powerpcInstructionSemantics.h"
#include "integerOps.h"
#include "generatedCOpts.h"
#include <boost/static_assert.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

using namespace std;
//using namespace IntegerOps;
using namespace SageInterface;
using namespace SageBuilder;

static SgBasicBlock* bb; // Global location to append new statements

static size_t WordWithExpression_nameCounter = 0;

static SgType* lengthToType(size_t Len) {
  if (Len <= 1) return SgTypeBool::createType();
  if (Len <= 8) return SgTypeUnsignedChar::createType();
  if (Len <= 16) return SgTypeUnsignedShort::createType();
  if (Len <= 32) return SgTypeUnsignedInt::createType();
  if (Len <= 64) return SgTypeUnsignedLongLong::createType();
  cerr << "lengthToType(" << Len << ")" << endl; abort();
}

static SgExpression* constantOfLength(size_t Len, uintmax_t c) {
  if (Len <= 1) return buildBoolValExp((bool)c);
  if (Len <= 32) return buildUnsignedIntValHex(c);
  if (Len <= 64) return buildUnsignedLongLongIntValHex(c);
  cerr << "constantOfLength(" << Len << ")" << endl; abort();
}

static size_t typeToLength(SgType* t) {
  switch (t->variantT()) {
    case V_SgTypeBool: return 1;
    case V_SgTypeUnsignedChar: return 8;
    case V_SgTypeUnsignedShort: return 16;
    case V_SgTypeUnsignedInt: return 32;
    case V_SgTypeInt: return 32;
    case V_SgTypeUnsignedLongLong: return 64;
    case V_SgTypedefType: return typeToLength(isSgTypedefType(t)->get_base_type());
    default: {cerr << "typeToLength: " << t->class_name() << endl; abort();}
  }
}

template <size_t Len>
struct WordWithExpression {
  private:
  SgVariableSymbol* sym;
  public:
  WordWithExpression(SgExpression* expr) {
    std::string name = "var" + boost::lexical_cast<std::string>(WordWithExpression_nameCounter);
    ++WordWithExpression_nameCounter;
    SgExpression* rhs = expr;
    if (typeToLength(expr->get_type()) < Len) { // Need to cast to a larger type
      rhs = buildCastExp(rhs, lengthToType(Len));
    } else if (typeToLength(expr->get_type()) > Len) { // Need to mask to a smaller number of bits
      rhs = buildBitAndOp(rhs, buildUnsignedLongLongIntValHex(IntegerOps::GenMask<uint64_t, Len>::value));
    }
    SgVariableDeclaration* decl = buildVariableDeclaration(name, rhs->get_type(), buildAssignInitializer(rhs), bb);
    appendStatement(decl, bb);
    sym = getFirstVarSym(decl);
  }
  SgExpression* expr() const {return buildVarRefExp(sym);}
  private: BOOST_STATIC_ASSERT (Len <= 64); // FIXME handle longer operations
};

struct powerpcCTranslationPolicy: public CTranslationPolicy {
  powerpcCTranslationPolicy(SgSourceFile* sourceFile, SgAsmGenericFile* asmFile);

  SgAsmGenericFile* asmFile;
  SgGlobal* globalScope;
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
  SgVariableSymbol* gprSym[32];
  SgVariableSymbol* crSym[8];
  SgVariableSymbol* ipSym;
// DQ (10/25/2008): Added Special Purpose Register support
  SgVariableSymbol* sprSym[1024];
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
  SgFunctionSymbol* systemCallSym;
  SgFunctionSymbol* readCRFieldsSym;
  SgBasicBlock* switchBody;
  SgStatement* whileBody;
  std::map<uint64_t, SgAsmBlock*> blocks;
  std::map<uint64_t, SgLabelStatement*> labelsForBlocks;
  std::set<uint64_t> externallyVisibleBlocks;

  SgFunctionSymbol* addHelperFunction(const std::string& name, SgType* returnType, SgFunctionParameterList* params);

  template <size_t Len>
  WordWithExpression<Len> number(uint64_t n) {
    return constantOfLength(Len, n);
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

  WordWithExpression<1> true_() {return buildBoolValExp(true);}
  WordWithExpression<1> false_() {return buildBoolValExp(false);}
  WordWithExpression<1> undefined_() {return buildBoolValExp(false);}

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
    if (Len == 1) {
      return buildNotOp(a.expr());
    } else {
      return buildBitXorOp(a.expr(), constantOfLength(Len, IntegerOps::GenMask<unsigned long long, Len>::value));
    }
  }

  template <size_t Len>
  WordWithExpression<Len> negate(WordWithExpression<Len> a) {
    return buildAddOp(buildBitXorOp(a.expr(), constantOfLength(Len, IntegerOps::GenMask<unsigned long long, Len>::value)), constantOfLength(Len, 1));
  }

  template <size_t Len>
  WordWithExpression<Len> ite(WordWithExpression<1> sel, WordWithExpression<Len> a, WordWithExpression<Len> b) {
    return buildConditionalExp(buildNotEqualOp(sel.expr(), buildIntVal(0)), a.expr(), b.expr());
  }

  template <size_t Len>
  WordWithExpression<1> equalToZero(WordWithExpression<Len> a) {
    return buildEqualityOp(a.expr(), constantOfLength(Len, 0));
  }

  template <size_t Len>
  WordWithExpression<Len> add(WordWithExpression<Len> a, WordWithExpression<Len> b) {
    return buildAddOp(a.expr(), b.expr());
  }

  template <size_t Len>
  WordWithExpression<Len> addWithCarries(WordWithExpression<Len> a, WordWithExpression<Len> b, WordWithExpression<1> carryIn, WordWithExpression<Len>& carries) {
    WordWithExpression<Len + 1> aExpanded = a.expr(); // Do casts
    WordWithExpression<Len + 1> bExpanded = b.expr(); // Do casts
    WordWithExpression<Len + 1> cExpanded = carryIn.expr(); // Do casts
    WordWithExpression<Len + 1> e = buildAddOp(aExpanded.expr(), buildAddOp(bExpanded.expr(), cExpanded.expr()));
    carries = buildRshiftOp(
                buildBitXorOp(buildBitXorOp(aExpanded.expr(), bExpanded.expr()), e.expr()),
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
               buildBitAndOp(a.expr(), constantOfLength(Len1, IntegerOps::SHL1<unsigned long long, Len1 - 1>::value)),
               buildBitComplementOp(buildRshiftOp(constantOfLength(Len1, IntegerOps::GenMask<unsigned long long, Len1>::value), b.expr())),
               constantOfLength(Len1, 0)));
  }

  template <size_t Len1, size_t Len2>
  WordWithExpression<Len1> generateMask(WordWithExpression<Len2> w) { // Set lowest w bits of result
    return buildConditionalExp(
             buildGreaterOrEqualOp(w.expr(), buildIntVal(Len1)),
             constantOfLength(Len1, IntegerOps::GenMask<unsigned long long, Len1>::value),
             buildSubtractOp(
               buildLshiftOp(constantOfLength(Len1, 1), w.expr()),
               constantOfLength(Len1, 1)));
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
    return buildBitOrOp(a.expr(), buildConditionalExp(buildNotEqualOp(buildBitAndOp(a.expr(), constantOfLength(To, IntegerOps::SHL1<uint64_t, From - 1>::value)), buildIntVal(0)), constantOfLength(To, IntegerOps::SHL1<uint64_t, To>::value - IntegerOps::SHL1<uint64_t, From>::value), constantOfLength(To, 0)));
  }

  template <size_t Len>
  WordWithExpression<Len> leastSignificantSetBit(WordWithExpression<Len> a) {
    return buildFunctionCallExp(bsfSym, buildExprListExp(a.expr()));
  }

  template <size_t Len>
  WordWithExpression<Len> mostSignificantSetBit(WordWithExpression<Len> a) {
    return buildFunctionCallExp(bsrSym, buildExprListExp(a.expr()));
  }

  WordWithExpression<4> readCRField(int fld) {
    return buildVarRefExp(crSym[fld]);
  }

  void writeCRField(int fld, WordWithExpression<4> value) {
    appendStatement(buildAssignStatement(buildVarRefExp(crSym[fld]), value.expr()), bb);
  }

  WordWithExpression<32> readCR() {
     return buildFunctionCallExp(readCRFieldsSym, buildExprListExp());
  }


  WordWithExpression<32> readGPR(int num) {
    return buildVarRefExp(gprSym[num]);
  }

  void writeGPR(int num, WordWithExpression<32> val) {
    appendStatement(buildExprStatement(buildAssignOp(buildVarRefExp(gprSym[num]), val.expr())), bb);
  }

  WordWithExpression<32> readIP() {
    return buildVarRefExp(ipSym);
  }

  void writeIP(WordWithExpression<32> val) {
    appendStatement(buildExprStatement(buildAssignOp(buildVarRefExp(ipSym), val.expr())), bb);
  }

// DQ (10/25/2008): Added Special Purpose Register support
  WordWithExpression<32> readSPR(int num) {
    ROSE_ASSERT(sprSym[num] != NULL);
    return buildVarRefExp(sprSym[num]);
  }

// DQ (10/25/2008): Added Special Purpose Register support (we need to restict this to 1,8, and 9 for user modes and support a number of others for other modes).
  void writeSPR(int num, WordWithExpression<32> val) {
    ROSE_ASSERT(sprSym[num] != NULL);
    appendStatement(buildExprStatement(buildAssignOp(buildVarRefExp(sprSym[num]), val.expr())), bb);
  }

  template <size_t Len>
  WordWithExpression<Len> readMemory(WordWithExpression<32> address, WordWithExpression<1> cond) {
    SgFunctionSymbol* mrSym = NULL;
    switch (Len) {
      case 8: mrSym = memoryReadByteSym; break;
      case 16: mrSym = memoryReadWordSym; break;
      case 32: mrSym = memoryReadDWordSym; break;
      case 64: mrSym = memoryReadQWordSym; break;
      default: ROSE_ASSERT (false);
    }
    ROSE_ASSERT (mrSym);
    return buildConditionalExp(buildNotEqualOp(cond.expr(), buildIntVal(0)), buildFunctionCallExp(mrSym, buildExprListExp(address.expr())), buildIntVal(0));
  }

  template <size_t Len>
  void writeMemory(WordWithExpression<32> address, WordWithExpression<Len> data, WordWithExpression<1> cond) {
    SgFunctionSymbol* mwSym = NULL;
    switch (Len) {
      case 8: mwSym = memoryWriteByteSym; break;
      case 16: mwSym = memoryWriteWordSym; break;
      case 32: mwSym = memoryWriteDWordSym; break;
      case 64: mwSym = memoryWriteQWordSym; break;
      default: ROSE_ASSERT (false);
    }
    ROSE_ASSERT (mwSym);
    appendStatement(buildIfStmt(buildNotEqualOp(cond.expr(), buildIntVal(0)), buildExprStatement(buildFunctionCallExp(mwSym, buildExprListExp(address.expr(), data.expr()))), NULL), bb);
  }

  void startBlock(uint64_t addr) {
  }

  void finishBlock(uint64_t addr) {
  }

  void startInstruction(SgAsmInstruction* insn) {
    bb = buildBasicBlock();
    appendStatement(buildCaseOptionStmt(buildUnsignedIntValHex(insn->get_address()), bb), switchBody);
    appendStatement(buildPragmaDeclaration(unparseInstructionWithAddress(insn), bb), bb);
    SgLabelStatement* label = buildLabelStatement("addr_" + StringUtility::intToHex(insn->get_address()), buildBasicBlock());
    appendStatement(label, bb);
    labelsForBlocks[insn->get_address()] = label;
  }

  void finishInstruction(SgAsmInstruction* insn) {
    appendStatement(buildContinueStmt(), bb);
  }

  void systemCall(uint8_t num) {
    appendStatement(buildExprStatement(buildFunctionCallExp(systemCallSym, buildExprListExp(buildIntVal(num)))), bb);
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
        isSgFunctionRefExp(isSgFunctionCallExp(e)->get_function())->get_symbol()->get_declaration() == systemCallSym->get_declaration());
  }

    void undefinedInstruction(SgAsmPowerpcInstruction*) {
        fprintf(stderr, "Bad instruction\n");
        ROSE_ASSERT(false);
        abort();
    }

  virtual SgStatement* getWhileBody() const {return whileBody;}
  virtual SgBasicBlock* getSwitchBody() const {return switchBody;}
  virtual SgVariableSymbol* getIPSymbol() const {return ipSym;}
  virtual const std::map<uint64_t, SgLabelStatement*>& getLabelsForBlocks() const {return labelsForBlocks;}
  virtual const std::set<uint64_t>& getExternallyVisibleBlocks() const {return externallyVisibleBlocks;}
};

SgFunctionSymbol* powerpcCTranslationPolicy::addHelperFunction(const std::string& name, SgType* returnType, SgFunctionParameterList* params) {
  SgFunctionDeclaration* decl = buildNondefiningFunctionDeclaration(name, returnType, params, globalScope);
  appendStatement(decl, globalScope);
  SgFunctionSymbol* sym = globalScope->lookup_function_symbol(name);
  ROSE_ASSERT (sym);
  return sym;
}

powerpcCTranslationPolicy::powerpcCTranslationPolicy(SgSourceFile* f, SgAsmGenericFile* asmFile): asmFile(asmFile), globalScope(NULL) {
  ROSE_ASSERT (f);
  ROSE_ASSERT (f->get_globalScope());
  globalScope = f->get_globalScope();

// DQ (10/25/2008): This is a macro idiom to use a do ... while loop to force ";" at the end of each call, avoid dangling else, etc.
#define LOOKUP_FUNC(name) \
  do {name##Sym = globalScope->lookup_function_symbol(#name); ROSE_ASSERT (name##Sym);} while (0)

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
  for (size_t i = 0; i < 32; ++i) {
    gprSym[i]  = globalScope->lookup_variable_symbol("gpr" + boost::lexical_cast<string>(i)); ROSE_ASSERT (gprSym[i]);
  }
  for (size_t i = 0; i < 8; ++i) {
    crSym[i]  = globalScope->lookup_variable_symbol("cr" + boost::lexical_cast<string>(i)); ROSE_ASSERT (crSym[i]);
  }
  ipSym  = globalScope->lookup_variable_symbol("ip"); ROSE_ASSERT (ipSym);
  for (size_t i = 0; i < 1024; ++i) sprSym[i] = NULL;
  sprSym[powerpc_spr_xer] = globalScope->lookup_variable_symbol("xer"); ROSE_ASSERT (sprSym[powerpc_spr_xer]);
  sprSym[powerpc_spr_lr] = globalScope->lookup_variable_symbol("lr"); ROSE_ASSERT (sprSym[powerpc_spr_lr]);
  sprSym[powerpc_spr_ctr] = globalScope->lookup_variable_symbol("ctr"); ROSE_ASSERT (sprSym[powerpc_spr_ctr]);
  LOOKUP_FUNC(memoryReadByte);
  LOOKUP_FUNC(memoryReadWord);
  LOOKUP_FUNC(memoryReadDWord);
  LOOKUP_FUNC(memoryReadQWord);
  LOOKUP_FUNC(memoryWriteByte);
  LOOKUP_FUNC(memoryWriteWord);
  LOOKUP_FUNC(memoryWriteDWord);
  LOOKUP_FUNC(memoryWriteQWord);
  LOOKUP_FUNC(abort);
  LOOKUP_FUNC(startingInstruction);
  LOOKUP_FUNC(systemCall);
  LOOKUP_FUNC(readCRFields);
#undef LOOKUP_FUNC
}

int main(int argc, char** argv) {
  SgProject* proj = frontend(argc, argv);
  ROSE_ASSERT (proj);

// The source file is a C++ source file template/example with a collection
// of variable declarations that predefine the support for grp, spr, etc.
  SgSourceFile* newFile = isSgSourceFile(proj->get_fileList().front());
  ROSE_ASSERT(newFile != NULL);

// This is the existing global scope with required declarations prebuilt...
  SgGlobal* g = newFile->get_globalScope();
  ROSE_ASSERT (g);

// Adding a function to have the emulation execute, the function body will be generated 
// to be an emulation of the binary.
  SgFunctionDeclaration* decl = buildDefiningFunctionDeclaration("run", SgTypeVoid::createType(), buildFunctionParameterList(), g);
  appendStatement(decl, g);
  vector<SgNode*> asmFiles = NodeQuery::querySubTree(proj, V_SgAsmGenericFile);
  ROSE_ASSERT (asmFiles.size() == 1);
  SgBasicBlock* body = decl->get_definition()->get_body();

// Build the policy object which contains the details of the translation of the disassembled instructions
  powerpcCTranslationPolicy policy(newFile, isSgAsmGenericFile(asmFiles[0]));
  policy.switchBody = buildBasicBlock();
  SgSwitchStatement* sw = buildSwitchStatement(buildVarRefExp(policy.ipSym), policy.switchBody);
  SgWhileStmt* whileStmt = buildWhileStmt(buildBoolValExp(true), sw);
  appendStatement(whileStmt, body);
  policy.whileBody = sw;

  PowerpcInstructionSemantics<powerpcCTranslationPolicy, WordWithExpression> t(policy);

  vector<SgNode*> instructions = NodeQuery::querySubTree(asmFiles[0], V_SgAsmPowerpcInstruction);
  for (size_t i = 0; i < instructions.size(); ++i) {
    SgAsmPowerpcInstruction* insn = isSgAsmPowerpcInstruction(instructions[i]);
    ROSE_ASSERT (insn);

 // Convert to C code
    t.processInstruction(insn);
  }

  proj->get_fileList().erase(proj->get_fileList().end() - 1); // Remove binary file before calling backend

  set<SgFunctionDeclaration*> safeFunctions;
  safeFunctions.insert(policy.memoryReadByteSym->get_declaration());
  safeFunctions.insert(policy.memoryReadWordSym->get_declaration());
  safeFunctions.insert(policy.memoryReadDWordSym->get_declaration());
  safeFunctions.insert(policy.memoryReadQWordSym->get_declaration());

#if 0
  cerr << "addDirectJumpsToSwitchCases" << endl;
  addDirectJumpsToSwitchCases(policy);
#endif
  cerr << "plugInAllConstVarDefs 1" << endl;
  plugInAllConstVarDefs(policy.switchBody, policy);
  cerr << "simplifyAllExpressions 1" << endl;
  simplifyAllExpressions(policy.switchBody);
  cerr << "removeIfConstants 1" << endl;
  removeIfConstants(policy.switchBody);
  cerr << "removeUnusedVariables 1" << endl;
  removeUnusedVariables(policy.switchBody, safeFunctions);
  cerr << "plugInAllConstVarDefs 2" << endl;
  plugInAllConstVarDefs(policy.switchBody, policy);
  cerr << "simplifyAllExpressions 2" << endl;
  simplifyAllExpressions(policy.switchBody);
  cerr << "removeIfConstants 2" << endl;
  removeIfConstants(policy.switchBody);
  cerr << "removeUnusedVariables 2" << endl;
  removeUnusedVariables(policy.switchBody, safeFunctions);

// Run the standard ROSE consistancy tests on the generated source file
  cerr << "testing" << endl;
  AstTests::runAllTests(proj);

// Generate the source code and call the backend compiler.
  cerr << "unparsing" << endl;
  return backend(proj);
}
