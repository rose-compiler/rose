#ifndef ROSE_X86_ASSEMBLY_TO_C_H
#define ROSE_X86_ASSEMBLY_TO_C_H

//#include "rose.h"
#include "x86InstructionSemantics.h"
#include <boost/static_assert.hpp>
#include <boost/lexical_cast.hpp>
#include "integerOps.h"
using namespace IntegerOps;

using namespace SageInterface;
using namespace SageBuilder;
#if 0
SgType* asmTypeToCType(SgAsmType* ty);
SgType* asmTypeToCSignedType(SgAsmType* ty);

struct X86AssemblyToCWithVariables {
  virtual SgStatement* convertInstruction(SgAsmX86Instruction* insn);
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
#include "generatedCOpts.h"
class SgBasicBlock;
#ifndef MAINFILE
extern SgBasicBlock* bb; // Global location to append new statements

extern size_t WordWithExpression_nameCounter;
#endif

template <size_t Len>
struct WordWithExpression {
  private:
  SgVariableSymbol* sym;
  public:
  WordWithExpression(SgExpression* expr) {
    std::string name = "var" + boost::lexical_cast<std::string>(WordWithExpression_nameCounter);
    ++WordWithExpression_nameCounter;
    SgVariableDeclaration* decl = buildVariableDeclaration(name, SgTypeUnsignedLongLong::createType(), buildAssignInitializer(buildBitAndOp(expr, buildUnsignedLongLongIntValHex(IntegerOps::SHL1<unsigned long long, Len>::value - 1))), bb);
    appendStatement(decl, bb);
    sym = getFirstVarSym(decl);
  }
  SgExpression* expr() const {return buildVarRefExp(sym);}
  private: BOOST_STATIC_ASSERT (Len <= 64); // FIXME handle longer operations
};


class CTranslationPolicy;
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
  static const size_t nflags = 32;
  SgVariableSymbol* flagsSym[nflags]; /* "eflags" register is 32 bits; Pentium4 architecture defines most bits up to bit 21*/
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
  const RegisterDictionary *regdict;

  X86CTranslationPolicy(): regdict(NULL) {}

  /** Returns the register dictionary. */
  const RegisterDictionary *get_register_dictionary() const {
      return regdict ? regdict : RegisterDictionary::dictionary_pentium4();
  }

  /** Sets the register dictionary. */
  void set_register_dictionary(const RegisterDictionary *regdict) {
      this->regdict = regdict;
  }

  /** Finds a register by name. */
  const RegisterDescriptor& findRegister(const std::string &regname, size_t nbits=0) {
      const RegisterDescriptor *reg = get_register_dictionary()->lookup(regname);
      if (!reg) {
          std::ostringstream ss;
          ss <<"Invalid register: \"" <<regname <<"\"";
          abort();
      }
      if (nbits>0 && reg->get_nbits()!=nbits) {
          std::ostringstream ss;
          ss <<"Invalid " <<nbits <<"-bit register: \"" <<regname <<"\" is "
             <<reg->get_nbits() <<" " <<(1==reg->get_nbits()?"byte":"bytes");
          abort();
      }
      return *reg;
  }

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
               buildBitAndOp(a.expr(), buildUnsignedLongLongIntValHex(IntegerOps::SHL1<unsigned long long, Len1 - 1>::value)),
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
    return buildBitOrOp(a.expr(), buildConditionalExp(buildNotEqualOp(buildBitAndOp(a.expr(), buildUnsignedLongLongIntValHex(IntegerOps::SHL1<unsigned long long, (From - 1)>::value)), buildIntVal(0)), buildUnsignedLongLongIntValHex(IntegerOps::SHL1<unsigned long long, To>::value - IntegerOps::SHL1<unsigned long long, From>::value), buildIntVal(0)));
  }

  template<size_t From, size_t To>
  WordWithExpression<To> unsignedExtend(WordWithExpression<From> a) {
      return buildBitAndOp(a.expr(),
                           buildUnsignedLongLongIntValHex(SHL1<unsigned long long, To>::value));
  }

  template <size_t Len>
  WordWithExpression<Len> leastSignificantSetBit(WordWithExpression<Len> a) {
    return buildFunctionCallExp(bsfSym, buildExprListExp(a.expr()));
  }

  template <size_t Len>
  WordWithExpression<Len> mostSignificantSetBit(WordWithExpression<Len> a) {
    return buildFunctionCallExp(bsrSym, buildExprListExp(a.expr()));
  }

  /** Reads one of the bit flags from the Pentium4 "eflags" register. Each bit is represented separately rather than one
    *  value for the entire register. */
  WordWithExpression<1> readFlag(X86Flag flag) {
    SgVariableSymbol* fl = flagsSym[flag];
    ROSE_ASSERT (fl);
    return buildVarRefExp(fl);
  }

  /** Sets or clears one of the bit flags from the Pentium4 "eflags" register. Each bit is represented separately rather than
   *  one value for the entire register. */
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

  void sysenter() {} // FIXME

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
  /** Reads from a named register. */
  template<size_t Len/*bits*/>
  WordWithExpression<Len> readRegister(const char *regname) {
      return readRegister<Len>(findRegister(regname, Len));
  }

  /** Writes to a named register. */
  template<size_t Len/*bits*/>
  void writeRegister(const char *regname, const WordWithExpression<Len> &value) {
      writeRegister<Len>(findRegister(regname, Len), value);
  }

  /** Generic register read. */
  template<size_t Len>
  WordWithExpression<Len> readRegister(const RegisterDescriptor &reg) {
      switch (Len) {
          case 1:
              // Only FLAGS/EFLAGS bits have a size of one.  Other registers cannot be accessed at this granularity.
              assert(reg.get_major()==x86_regclass_flags);
              assert(reg.get_nbits()==1);
              return unsignedExtend<1, Len>(readFlag((X86Flag)reg.get_offset()));

          case 8:
              // Only general-purpose registers can be accessed at a byte granularity, and we can access only the low-order
              // byte or the next higher byte.  For instance, "al" and "ah" registers.
              assert(reg.get_major()==x86_regclass_gpr);
              assert(reg.get_nbits()==8);
              switch (reg.get_offset()) {
                  case 0:
                      return extract<0, Len>(readGPR((X86GeneralPurposeRegister)reg.get_minor()));
                  case 8:
                      return extract<8, 8+Len>(readGPR((X86GeneralPurposeRegister)reg.get_minor()));
                  default:
                      assert(false);
                      abort();
              }

          case 16:
              assert(reg.get_nbits()==16);
              assert(reg.get_offset()==0);
              switch (reg.get_major()) {
                  case x86_regclass_segment:
                      return unsignedExtend<16, Len>(readSegreg((X86SegmentRegister)reg.get_minor()));
                  case x86_regclass_gpr:
                      return extract<0, Len>(readGPR((X86GeneralPurposeRegister)reg.get_minor()));
                  case x86_regclass_flags:
                      return unsignedExtend<16, Len>(concat(readFlag((X86Flag)0),
                                                     concat(readFlag((X86Flag)1),
                                                     concat(readFlag((X86Flag)2),
                                                     concat(readFlag((X86Flag)3),
                                                     concat(readFlag((X86Flag)4),
                                                     concat(readFlag((X86Flag)5),
                                                     concat(readFlag((X86Flag)6),
                                                     concat(readFlag((X86Flag)7),
                                                     concat(readFlag((X86Flag)8),
                                                     concat(readFlag((X86Flag)9),
                                                     concat(readFlag((X86Flag)10),
                                                     concat(readFlag((X86Flag)11),
                                                     concat(readFlag((X86Flag)12),
                                                     concat(readFlag((X86Flag)13),
                                                     concat(readFlag((X86Flag)14),
                                                            readFlag((X86Flag)15)))))))))))))))));
                  default:
                      assert(false);
                      abort();
              }

          case 32:
              assert(reg.get_offset()==0);
              switch (reg.get_major()) {
                  case x86_regclass_gpr:
                      return unsignedExtend<32, Len>(readGPR((X86GeneralPurposeRegister)reg.get_minor()));
                  case x86_regclass_ip:
                      return unsignedExtend<32, Len>(readIP());
                  case x86_regclass_segment:
                      return unsignedExtend<16, Len>(readSegreg((X86SegmentRegister)reg.get_minor()));
                  case x86_regclass_flags: {
                      return unsignedExtend<32, Len>(concat(readRegister<16>("flags"), // no-op sign extension
                                                     concat(readFlag((X86Flag)16),
                                                     concat(readFlag((X86Flag)17),
                                                     concat(readFlag((X86Flag)18),
                                                     concat(readFlag((X86Flag)19),
                                                     concat(readFlag((X86Flag)20),
                                                     concat(readFlag((X86Flag)21),
                                                     concat(readFlag((X86Flag)22),
                                                     concat(readFlag((X86Flag)23),
                                                     concat(readFlag((X86Flag)24),
                                                     concat(readFlag((X86Flag)25),
                                                     concat(readFlag((X86Flag)26),
                                                     concat(readFlag((X86Flag)27),
                                                     concat(readFlag((X86Flag)28),
                                                     concat(readFlag((X86Flag)29),
                                                     concat(readFlag((X86Flag)30),
                                                            readFlag((X86Flag)31))))))))))))))))));
                  }
                  default:
                      assert(false);
                      abort();
              }

          default:
              assert(false);
              abort();
      }
  }

  /** Generic register write. */
  template<size_t Len>
  void writeRegister(const RegisterDescriptor &reg, const WordWithExpression<Len> &value) {
      switch (Len) {
          case 1:
              assert(reg.get_major()==x86_regclass_flags);
              assert(reg.get_nbits()==1);
              writeFlag((X86Flag)reg.get_offset(), unsignedExtend<Len, 1>(value));
              break;

          case 8:
              // Only general purpose registers can be accessed at byte granularity, and only for offsets 0 and 8.
              assert(reg.get_major()==x86_regclass_gpr);
              assert(reg.get_nbits()==8);
              switch (reg.get_offset()) {
                  case 0:
                      writeGPR((X86GeneralPurposeRegister)reg.get_minor(),
                               concat(signExtend<Len, 8>(value),
                                      extract<8, 32>(readGPR((X86GeneralPurposeRegister)reg.get_minor()))));
                      break;
                  case 8:
                      writeGPR((X86GeneralPurposeRegister)reg.get_minor(),
                               concat(extract<0, 8>(readGPR((X86GeneralPurposeRegister)reg.get_minor())),
                                      concat(unsignedExtend<Len, 8>(value),
                                             extract<16, 32>(readGPR((X86GeneralPurposeRegister)reg.get_minor())))));
                      break;
                  default:
                      assert(false);
                      abort();
              }
              break;

          case 16:
              assert(reg.get_nbits()==16);
              assert(reg.get_offset()==0);
              switch (reg.get_major()) {
                  case x86_regclass_segment:
                      writeSegreg((X86SegmentRegister)reg.get_minor(), unsignedExtend<Len, 16>(value));
                      break;
                  case x86_regclass_gpr:
                      writeGPR((X86GeneralPurposeRegister)reg.get_minor(),
                               concat(unsignedExtend<Len, 16>(value),
                                      extract<16, 32>(readGPR((X86GeneralPurposeRegister)reg.get_minor()))));
                      break;
                  case x86_regclass_flags:
                      writeFlag((X86Flag)0,  extract<0,  1 >(value));
                      writeFlag((X86Flag)1,  extract<1,  2 >(value));
                      writeFlag((X86Flag)2,  extract<2,  3 >(value));
                      writeFlag((X86Flag)3,  extract<3,  4 >(value));
                      writeFlag((X86Flag)4,  extract<4,  5 >(value));
                      writeFlag((X86Flag)5,  extract<5,  6 >(value));
                      writeFlag((X86Flag)6,  extract<6,  7 >(value));
                      writeFlag((X86Flag)7,  extract<7,  8 >(value));
                      writeFlag((X86Flag)8,  extract<8,  9 >(value));
                      writeFlag((X86Flag)9,  extract<9,  10>(value));
                      writeFlag((X86Flag)10, extract<10, 11>(value));
                      writeFlag((X86Flag)11, extract<11, 12>(value));
                      writeFlag((X86Flag)12, extract<12, 13>(value));
                      writeFlag((X86Flag)13, extract<13, 14>(value));
                      writeFlag((X86Flag)14, extract<14, 15>(value));
                      writeFlag((X86Flag)15, extract<15, 16>(value));
                      break;
                  default:
                      assert(false);
                      abort();
              }
              break;

          case 32:
              assert(reg.get_offset()==0);
              switch (reg.get_major()) {
                  case x86_regclass_gpr:
                      writeGPR((X86GeneralPurposeRegister)reg.get_minor(), signExtend<Len, 32>(value));
                      break;
                  case x86_regclass_ip:
                      writeIP(unsignedExtend<Len, 32>(value));
                      break;
                  case x86_regclass_flags:
                      assert(reg.get_nbits()==32);
                      writeRegister<16>("flags", unsignedExtend<Len, 16>(value));
                      writeFlag((X86Flag)16, extract<16, 17>(value));
                      writeFlag((X86Flag)17, extract<17, 18>(value));
                      writeFlag((X86Flag)18, extract<18, 19>(value));
                      writeFlag((X86Flag)19, extract<19, 20>(value));
                      writeFlag((X86Flag)20, extract<20, 21>(value));
                      writeFlag((X86Flag)21, extract<21, 22>(value));
                      writeFlag((X86Flag)22, extract<22, 23>(value));
                      writeFlag((X86Flag)23, extract<23, 24>(value));
                      writeFlag((X86Flag)24, extract<24, 25>(value));
                      writeFlag((X86Flag)25, extract<25, 26>(value));
                      writeFlag((X86Flag)26, extract<26, 27>(value));
                      writeFlag((X86Flag)27, extract<27, 28>(value));
                      writeFlag((X86Flag)28, extract<28, 29>(value));
                      writeFlag((X86Flag)29, extract<29, 30>(value));
                      writeFlag((X86Flag)30, extract<30, 31>(value));
                      writeFlag((X86Flag)31, extract<31, 32>(value));
                      break;
                  default:
                      assert(false);
                      abort();
              }
              break;

          default:
              assert(false);
              abort();
      }
  }
};

#if 1

class X86CTranslationPolicy;
std::set<SgInitializedName*> computeLiveVars(SgStatement* stmt, const X86CTranslationPolicy& conv, std::map<SgLabelStatement*, std::set<SgInitializedName*> >& liveVarsForLabels, std::set<SgInitializedName*> currentLiveVars, bool actuallyRemove);

void removeDeadStores(SgBasicBlock* switchBody, const X86CTranslationPolicy& conv);
#endif

#endif // ROSE_X86_ASSEMBLY_TO_C_H
