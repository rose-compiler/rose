#include "rose.h"
#include "x86AssemblyToC.h"
#include <iostream>

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using X86Disassembler::sizeToType;
using X86Disassembler::sizeToPos;

SgType* asmTypeToCType(SgAsmType* ty) {
  switch (ty->variantT()) {
    case V_SgAsmTypeByte: return SgTypeUnsignedChar::createType();
    case V_SgAsmTypeWord: return SgTypeUnsignedShort::createType();
    case V_SgAsmTypeDoubleWord: return SgTypeUnsignedInt::createType();
    case V_SgAsmTypeQuadWord: return SgTypeUnsignedLongLong::createType();
    default: {cerr << "Unhandled type " << ty->class_name() << " in asmTypeToCType" << endl; abort();}
  }
}

SgType* asmTypeToCSignedType(SgAsmType* ty) {
  switch (ty->variantT()) {
    case V_SgAsmTypeByte: return SgTypeSignedChar::createType();
    case V_SgAsmTypeWord: return SgTypeShort::createType();
    case V_SgAsmTypeDoubleWord: return SgTypeInt::createType();
    case V_SgAsmTypeQuadWord: return SgTypeLongLong::createType();
    default: {cerr << "Unhandled type " << ty->class_name() << " in asmTypeToCSignedType" << endl; abort();}
  }
}

// This is not used -- casts are used instead
static SgExpression* buildMaskForType(SgAsmType* ty) {
  switch (ty->variantT()) {
    case V_SgAsmTypeByte: return buildUnsignedLongLongIntValHex(0xFF);
    case V_SgAsmTypeWord: return buildUnsignedLongLongIntValHex(0xFFFF);
    case V_SgAsmTypeDoubleWord: return buildUnsignedLongLongIntValHex(0xFFFFFFFFULL);
    case V_SgAsmTypeQuadWord: return buildUnsignedLongLongIntValHex(~0ULL);
    default: {cerr << "Unhandled type " << ty->class_name() << " in buildMaskForType" << endl; abort();}
  }
}

static SgExpression* buildSignBitMaskForType(SgAsmType* ty) {
  switch (ty->variantT()) {
    case V_SgAsmTypeByte: return buildUnsignedIntValHex(0x80);
    case V_SgAsmTypeWord: return buildUnsignedIntValHex(0x8000);
    case V_SgAsmTypeDoubleWord: return buildUnsignedLongValHex(0x80000000ULL);
    case V_SgAsmTypeQuadWord: return buildUnsignedLongLongIntValHex(0x8000000000000000ULL);
    default: {cerr << "Unhandled type " << ty->class_name() << " in buildSignBitMaskForType" << endl; abort();}
  }
}

static SgExpression* buildNotSignBitMaskForType(SgAsmType* ty) {
  switch (ty->variantT()) {
    case V_SgAsmTypeByte: return buildUnsignedLongLongIntValHex(0x7F);
    case V_SgAsmTypeWord: return buildUnsignedLongLongIntValHex(0x7FFF);
    case V_SgAsmTypeDoubleWord: return buildUnsignedLongLongIntValHex(0x7FFFFFFFULL);
    case V_SgAsmTypeQuadWord: return buildUnsignedLongLongIntValHex(0x7FFFFFFFFFFFFFFFULL);
    default: {cerr << "Unhandled type " << ty->class_name() << " in buildNotSignBitMaskForType" << endl; abort();}
  }
}

struct SingleInstructionTranslator {

  SgAsmx86Instruction* insn;
  X86AssemblyToCFramework* f;
  SgBasicBlock* bb;

  SingleInstructionTranslator(SgAsmx86Instruction* insn, X86AssemblyToCFramework* framework): insn(insn), f(framework), bb(buildBasicBlock()) {}

  inline void append(SgStatement* stmt, SgBasicBlock* blk = NULL) {appendStatement(stmt, blk ? blk : bb);}
  inline void append(SgExpression* expr, SgBasicBlock* blk = NULL) {append(buildExprStatement(expr), blk);}

  SgExpression* makeRead(SgAsmExpression* expr);
  SgStatement* makeWrite(SgAsmExpression* expr, SgExpression* data);
  SgVariableSymbol* cacheValue(SgAsmExpression* data, SgBasicBlock* blk = NULL);
  SgVariableSymbol* cacheValue(SgExpression* data, SgBasicBlock* blk = NULL);
  SgVariableSymbol* makeNewVariableInBlock(SgType* t, SgExpression* initialValue, SgBasicBlock* blk = NULL);

  void setFlagsBasedOnResult(SgVariableSymbol* resultSym, SgAsmType* resultType, bool includeZF, SgBasicBlock* blk = NULL);
  void setFlagsForSubtract(SgVariableSymbol* op1sym, SgVariableSymbol* op2sym, SgVariableSymbol* resultSym, SgAsmType* ty, SgBasicBlock* blk = NULL);

  SgVariableSymbol* makePop();
  void makePush(SgExpression* val);

  void translate();
};

void SingleInstructionTranslator::setFlagsBasedOnResult(SgVariableSymbol* resultSym, SgAsmType* resultType, bool includeZF, SgBasicBlock* blk)
   {
  // Returns a variable symbol representing the sign flag (used at some call sites)
  
     if (!blk) blk = bb;

     ROSE_ASSERT(resultSym != NULL);
     ROSE_ASSERT(resultType != NULL);

  // Now set the flags
     if (includeZF) {
       append(f->makeFlagWrite(x86flag_zf,
                 buildEqualityOp(
                    buildVarRefExp(resultSym),
                    buildUnsignedLongLongIntValHex(0))),
           blk);
     }

     // This form matches signed comparisons a bit better (for forms like test x, x; js foo to tell if x is negative)
     append(f->makeFlagWrite(x86flag_sf,
                             buildLessThanOp(
                               buildBitXorOp(
                                 buildVarRefExp(resultSym),
                                 buildSignBitMaskForType(
                                   resultType)),
                               buildSignBitMaskForType(resultType))),
            blk);

     append(f->makeFlagWrite(x86flag_pf,
            buildFunctionCallExp(
              f->getHelperFunction("parity"),
              buildExprListExp(buildVarRefExp(resultSym)))),
         blk);
   }

void SingleInstructionTranslator::setFlagsForSubtract(SgVariableSymbol* op1sym, SgVariableSymbol* op2sym, SgVariableSymbol* resultsym, SgAsmType* ty, SgBasicBlock* blk) {
  if (!blk) blk = bb;

  setFlagsBasedOnResult(resultsym, ty, false, blk);

  append(f->makeFlagWrite(x86flag_zf,
                          buildEqualityOp(
                            buildVarRefExp(op1sym),
                            buildVarRefExp(op2sym))),
         blk);

  append(f->makeFlagWrite(x86flag_cf,
                          buildLessThanOp(
                            buildVarRefExp(op1sym),
                            buildVarRefExp(op2sym))),
         blk);
  append(f->makeFlagWrite(x86flag_af,
                          buildLessThanOp(
                            buildBitAndOp(
                              buildVarRefExp(op1sym),
                              buildUnsignedLongLongIntValHex(0xF)),
                            buildBitAndOp(
                              buildVarRefExp(op2sym),
                              buildUnsignedLongLongIntValHex(0xF)))),
         blk);
  // This form computes SF^OF using a signed less-than comparison and then xors
  // it with SF to get OF
  append(f->makeFlagWrite(x86flag_of,
                          buildNotEqualOp(
                            f->makeFlagRead(x86flag_sf),
                            buildLessThanOp(
                              buildBitXorOp(
                                buildVarRefExp(op1sym),
                                buildSignBitMaskForType(ty)),
                              buildBitXorOp(
                                buildVarRefExp(op2sym),
                                buildSignBitMaskForType(ty))))),
         blk);
}

SgStatement* X86AssemblyToCFramework::convertInstruction(SgAsmx86Instruction* insn) {
  SingleInstructionTranslator trans(insn, this);
  trans.translate();
  return trans.bb;
}

static X86SegmentRegister getSegregFromMemoryReference(SgAsmMemoryReferenceExpression* mr) {
  X86SegmentRegister segreg = x86_segreg_none;
  SgAsmx86RegisterReferenceExpression* seg = isSgAsmx86RegisterReferenceExpression(mr->get_segment());
  if (seg) {
    ROSE_ASSERT (seg->get_register_class() == x86_regclass_segment);
    segreg = (X86SegmentRegister)(seg->get_register_number());
  } else {
    ROSE_ASSERT (!"Bad segment expr");
  }
  return segreg;
}

SgExpression* SingleInstructionTranslator::makeRead(SgAsmExpression* expr) {
  switch (expr->variantT()) {
    case V_SgAsmx86RegisterReferenceExpression: {
      SgAsmx86RegisterReferenceExpression* rr = isSgAsmx86RegisterReferenceExpression(expr);
      X86PositionInRegister pos = rr->get_position_in_register();
      X86RegisterClass cl = rr->get_register_class();
      int num = rr->get_register_number();
      return f->makeRegisterRead(cl, num, pos);
    }
    case V_SgAsmMemoryReferenceExpression: {
      SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(expr);
      SgAsmExpression* addr = mr->get_address();
      SgExpression* addrC = makeRead(addr);
      X86SegmentRegister segreg = getSegregFromMemoryReference(mr);
      return f->makeMemoryRead(segreg, addrC, mr->get_type());
    }
    case V_SgAsmBinaryAdd: {
      SgAsmBinaryAdd* ba = isSgAsmBinaryAdd(expr);
      return buildAddOp(makeRead(ba->get_lhs()), makeRead(ba->get_rhs()));
    }
    case V_SgAsmBinaryMultiply: {
      SgAsmBinaryMultiply* bm = isSgAsmBinaryMultiply(expr);
      return buildMultiplyOp(makeRead(bm->get_lhs()), makeRead(bm->get_rhs()));
    }
    case V_SgAsmByteValueExpression: {
      return buildUnsignedCharValHex(isSgAsmByteValueExpression(expr)->get_value());
    }
    case V_SgAsmWordValueExpression: {
      return buildUnsignedShortValHex(isSgAsmWordValueExpression(expr)->get_value());
    }
    case V_SgAsmDoubleWordValueExpression: {
      return buildUnsignedLongValHex(isSgAsmDoubleWordValueExpression(expr)->get_value());
    }
    case V_SgAsmQuadWordValueExpression: {
      return buildUnsignedLongLongIntValHex(isSgAsmQuadWordValueExpression(expr)->get_value());
    }
    default: cerr << "Unhandled expression " << expr->class_name() << " in makeRead" << endl; abort();
  }
}

SgStatement* SingleInstructionTranslator::makeWrite(SgAsmExpression* expr, SgExpression* data) {
  switch (expr->variantT()) {
    case V_SgAsmx86RegisterReferenceExpression: {
      SgAsmx86RegisterReferenceExpression* rr = isSgAsmx86RegisterReferenceExpression(expr);
      X86PositionInRegister pos = rr->get_position_in_register();
      X86RegisterClass cl = rr->get_register_class();
      int num = rr->get_register_number();
      return f->makeRegisterWrite(cl, num, pos, data);
    }
    case V_SgAsmMemoryReferenceExpression: {
      SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(expr);
      SgAsmExpression* addr = mr->get_address();
      SgExpression* addrC = makeRead(addr);
      X86SegmentRegister segreg = getSegregFromMemoryReference(mr);
      return f->makeMemoryWrite(segreg, addrC, data, mr->get_type());
    }
    default: cerr << "Unhandled expression " << expr->class_name() << " in makeWrite" << endl; abort();
  }
}

SgVariableSymbol* SingleInstructionTranslator::makeNewVariableInBlock(SgType* t, SgExpression* initialValue, SgBasicBlock* blk) {
  if (!blk) blk = bb;
  static int nameCounter = 0;
  string name = "temp" + StringUtility::numberToString(++nameCounter);
  SgModifierType* constT = new SgModifierType(t->stripType());
  constT->get_typeModifier().get_constVolatileModifier().setConst();
  SgVariableDeclaration* decl = buildVariableDeclaration(name, constT, buildAssignInitializer(initialValue), blk);
  appendStatement(decl, blk);
  return getFirstVarSym(decl);
}

static int sizeOfAsmType(SgAsmType* ty) {
  switch (ty->variantT()) {
    case V_SgAsmTypeByte: return 1;
    case V_SgAsmTypeWord: return 2;
    case V_SgAsmTypeDoubleWord: return 4;
    case V_SgAsmTypeQuadWord: return 8;
    case V_SgAsmTypeDoubleQuadWord: return 16;
    default: ROSE_ASSERT (!"sizeOfAsmType");
  }
}

static int sizeOfInsnSize(X86InstructionSize s) {
  switch (s) {
    case x86_insnsize_16: return 2;
    case x86_insnsize_32: return 4;
    case x86_insnsize_64: return 8;
    default: ROSE_ASSERT (!"sizeOfInsnSize");
  }
}

SgVariableSymbol* SingleInstructionTranslator::cacheValue(SgAsmExpression* data, SgBasicBlock* blk) {
  if (!blk) blk = bb;
  return makeNewVariableInBlock(asmTypeToCType(data->get_type()), makeRead(data), blk);
}

SgVariableSymbol* SingleInstructionTranslator::cacheValue(SgExpression* data, SgBasicBlock* blk) {
  if (!blk) blk = bb;
  return makeNewVariableInBlock(data->get_type(), data, blk);
}

SgVariableSymbol* SingleInstructionTranslator::makePop() {
  SgVariableSymbol* sym = 
    makeNewVariableInBlock(
      asmTypeToCType(sizeToType(insn->get_operandSize())),
      f->makeMemoryRead(
        x86_segreg_ss,
        f->makeRegisterRead(x86_regclass_gpr, x86_gpr_sp, x86_regpos_all),
        sizeToType(insn->get_operandSize())));
  append(
    f->makeRegisterWrite(x86_regclass_gpr, x86_gpr_sp, x86_regpos_all,
      buildAddOp(
        f->makeRegisterRead(x86_regclass_gpr, x86_gpr_sp, x86_regpos_all),
        buildIntVal(sizeOfInsnSize(insn->get_operandSize())))));
  return sym;
}

void SingleInstructionTranslator::makePush(SgExpression* val) {
  append(
    f->makeRegisterWrite(x86_regclass_gpr, x86_gpr_sp, x86_regpos_all,
      buildSubtractOp(
        f->makeRegisterRead(x86_regclass_gpr, x86_gpr_sp, x86_regpos_all),
        buildIntVal(sizeOfInsnSize(insn->get_operandSize())))));
  SgVariableSymbol* sym = cacheValue(val);
  append(
    f->makeMemoryWrite(
      x86_segreg_ss,
      f->makeRegisterRead(x86_regclass_gpr, x86_gpr_sp, x86_regpos_all),
      buildVarRefExp(sym),
      sizeToType(insn->get_operandSize())));
}

void SingleInstructionTranslator::translate() {
  // cerr << "Working on instruction: " << unparseInstructionWithAddress(insn) << endl;
  SgAsmOperandList* ol = insn->get_operandList();
  ROSE_ASSERT(ol);
  const vector<SgAsmExpression*>& operands = ol->get_operands();
  bool jumpToNextInstruction = true;
  uint64_t nextIP = insn->get_address() + insn->get_raw_bytes().size();
  switch (insn->get_kind()) {

    case x86_mov: {
      ROSE_ASSERT (operands.size() == 2);
      append(makeWrite(operands[0], makeRead(operands[1])));
      break;
    }

    case x86_movzx:
      {
        ROSE_ASSERT (operands.size() == 2);
     // Zero extension is handled automatically by the cast to the register type.
        append(makeWrite(operands[0], makeRead(operands[1])));
        break;
      }

    case x86_movsx:
      {
        ROSE_ASSERT (operands.size() == 2);
        switch (operands[0]->get_type()->variantT()) {
          case V_SgAsmTypeWord: {
            ROSE_ASSERT (isSgAsmTypeByte(operands[1]));
            SgVariableSymbol* temp = cacheValue(operands[1]);
            append(makeWrite(operands[0],
                             buildConditionalExp(
                               buildLessThanOp(
                                 buildBitXorOp(
                                   buildVarRefExp(temp),
                                   buildUnsignedLongValHex(0x80)),
                                 buildUnsignedLongValHex(0x80)),
                               buildBitOrOp(buildVarRefExp(temp),
                                            buildUnsignedLongValHex(0xFF00)),
                               buildVarRefExp(temp))));
            break;
          }
          case V_SgAsmTypeDoubleWord: {
            switch (operands[1]->get_type()->variantT()) {
              case V_SgAsmTypeByte: {
                SgVariableSymbol* temp = cacheValue(operands[1]);
                append(makeWrite(operands[0],
                                 buildConditionalExp(
                                   buildLessThanOp(
                                     buildBitXorOp(
                                       buildVarRefExp(temp),
                                       buildUnsignedLongValHex(0x80)),
                                     buildUnsignedLongValHex(0x80)),
                                   buildBitOrOp(
                                     buildVarRefExp(temp),
                                     buildUnsignedLongValHex(0xFFFFFF00UL)),
                                   buildVarRefExp(temp))));
                break;
              }
              case V_SgAsmTypeWord: {
                SgVariableSymbol* temp = cacheValue(operands[1]);
                append(makeWrite(operands[0],
                                 buildConditionalExp(
                                   buildLessThanOp(
                                     buildBitXorOp(
                                       buildVarRefExp(temp),
                                       buildUnsignedLongValHex(0x8000)),
                                     buildUnsignedLongValHex(0x8000)),
                                   buildBitOrOp(
                                     buildVarRefExp(temp),
                                     buildUnsignedLongValHex(0xFFFF0000UL)),
                                   buildVarRefExp(temp))));
                break;
              }
              default: ROSE_ASSERT (false);
            }
            break;
          }
          default: ROSE_ASSERT (false);
        }
        break;
      }

    case x86_xchg: {
      ROSE_ASSERT (operands.size() == 2);
      SgVariableSymbol* op1sym = cacheValue(makeRead(operands[0]));
      append(makeWrite(operands[0], makeRead(operands[1])));
      append(makeWrite(operands[1], buildVarRefExp(op1sym)));
      break;
    }

    case x86_lea: {
      ROSE_ASSERT (operands.size() == 2);

      SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(operands[1]);
      ROSE_ASSERT(mr != NULL);

      append(makeWrite(operands[0], makeRead(mr->get_address())));
      break;
    }

    case x86_sub: {
      ROSE_ASSERT (operands.size() == 2);
      SgVariableSymbol* op1sym = cacheValue(operands[0]);
      SgVariableSymbol* op2sym = cacheValue(operands[1]);
      SgVariableSymbol* resultsym =
        makeNewVariableInBlock(
          asmTypeToCType(operands[0]->get_type()),
          buildSubtractOp(
            buildVarRefExp(op1sym),
            buildVarRefExp(op2sym)));

      setFlagsForSubtract(op1sym, op2sym, resultsym, operands[0]->get_type());
      append(makeWrite(operands[0], buildVarRefExp(resultsym)));
      break;
    }

    case x86_sbb: {
      ROSE_ASSERT (operands.size() == 2);
      SgVariableSymbol* op1sym = cacheValue(operands[0]);
      SgVariableSymbol* op2sym = cacheValue(operands[1]);
      SgVariableSymbol* resultsym =
        makeNewVariableInBlock(
          asmTypeToCType(operands[0]->get_type()),
          buildSubtractOp(
            buildVarRefExp(op1sym),
            buildAddOp(
              buildVarRefExp(op2sym),
              buildConditionalExp(
                f->makeFlagRead(x86flag_cf),
                buildIntValHex(1),
                buildIntValHex(0)))));

      SgAsmType* ty = operands[0]->get_type();

      setFlagsBasedOnResult(resultsym, ty, true);

      append(f->makeFlagWrite(x86flag_af,
                              buildLessThanOp(
                                buildBitAndOp(
                                  buildVarRefExp(op1sym),
                                  buildUnsignedLongLongIntValHex(0xF)),
                                buildAddOp(
                                  buildBitAndOp(
                                    buildVarRefExp(op2sym),
                                    buildUnsignedLongLongIntValHex(0xF)),
                                  buildConditionalExp(
                                    f->makeFlagRead(x86flag_cf),
                                    buildIntValHex(1),
                                    buildIntValHex(0))))));
      append(f->makeFlagWrite(x86flag_of,
                              buildNotEqualOp(
                                buildBitAndOp(
                                  buildBitAndOp(
                                    buildBitXorOp( // op1 sign != op2 sign
                                      buildVarRefExp(op1sym),
                                      buildVarRefExp(op2sym)),
                                    buildBitXorOp( // op1 sign != result sign
                                      buildVarRefExp(op1sym),
                                      buildVarRefExp(resultsym))),
                                  buildSignBitMaskForType(ty)),
                                buildUnsignedLongLongIntValHex(0))));
      append(makeWrite(operands[0], buildVarRefExp(resultsym)));
      append(f->makeFlagWrite(x86flag_cf,
                              buildOrOp(
                                buildLessThanOp(
                                  buildVarRefExp(op1sym),
                                  buildVarRefExp(op2sym)),
                                buildAndOp(
                                  f->makeFlagRead(x86flag_cf),
                                  buildEqualityOp(
                                    buildVarRefExp(op1sym),
                                    buildVarRefExp(op2sym))))));
      break;
    }

 // Same as SUB but skips output of result
    case x86_cmp: {
      ROSE_ASSERT (operands.size() == 2);
      SgVariableSymbol* op1sym = cacheValue(operands[0]);
      SgVariableSymbol* op2sym = cacheValue(operands[1]);
      SgVariableSymbol* resultsym =
        makeNewVariableInBlock(
          asmTypeToCType(operands[0]->get_type()),
          buildSubtractOp(
            buildVarRefExp(op1sym),
            buildVarRefExp(op2sym)));

      setFlagsForSubtract(op1sym, op2sym, resultsym, operands[0]->get_type());
      break;
    }

    case x86_repe_cmpsb: {
      ROSE_ASSERT (operands.size() == 0);
      SgBasicBlock* bbForCxNotZero = buildBasicBlock();

      X86SegmentRegister segForSrc = insn->get_segmentOverride();
      append(
        buildWhileStmt(
          buildNotEqualOp(f->makeRegisterRead(x86_regclass_gpr, x86_gpr_cx, x86_regpos_all), buildUnsignedLongLongIntValHex(0)),
          buildBasicBlock(
            f->makeRegisterWrite(x86_regclass_gpr, x86_gpr_cx, x86_regpos_all, buildSubtractOp(f->makeRegisterRead(x86_regclass_gpr, x86_gpr_cx, x86_regpos_all), buildUnsignedLongLongIntValHex(1))),
            buildIfStmt(
              buildNotEqualOp(
                f->makeMemoryRead(segForSrc, f->makeRegisterRead(x86_regclass_gpr, x86_gpr_si, x86_regpos_all), SgAsmTypeByte::createType()),
                f->makeMemoryRead(x86_segreg_es, f->makeRegisterRead(x86_regclass_gpr, x86_gpr_di, x86_regpos_all), SgAsmTypeByte::createType())),
              buildBasicBlock(buildBreakStmt()),
              buildBasicBlock()),
            f->makeRegisterWrite(x86_regclass_gpr, x86_gpr_si, x86_regpos_all, buildAddOp(f->makeRegisterRead(x86_regclass_gpr, x86_gpr_si, x86_regpos_all), buildConditionalExp(f->makeFlagRead(x86flag_df), buildUnsignedLongLongIntValHex(-1), buildUnsignedLongLongIntValHex(1)))),
            f->makeRegisterWrite(x86_regclass_gpr, x86_gpr_di, x86_regpos_all, buildAddOp(f->makeRegisterRead(x86_regclass_gpr, x86_gpr_di, x86_regpos_all), buildConditionalExp(f->makeFlagRead(x86flag_df), buildUnsignedLongLongIntValHex(-1), buildUnsignedLongLongIntValHex(1))))
            )),
        bbForCxNotZero);

      SgVariableSymbol* op1sym = cacheValue(f->makeMemoryRead(segForSrc, f->makeRegisterRead(x86_regclass_gpr, x86_gpr_si, x86_regpos_all), SgAsmTypeByte::createType()), bbForCxNotZero);
      SgVariableSymbol* op2sym = cacheValue(f->makeMemoryRead(x86_segreg_es, f->makeRegisterRead(x86_regclass_gpr, x86_gpr_di, x86_regpos_all), SgAsmTypeByte::createType()), bbForCxNotZero);
      SgVariableSymbol* resultsym =
        makeNewVariableInBlock(
          SgTypeUnsignedChar::createType(),
          buildBitAndOp(
            buildSubtractOp(
              buildVarRefExp(op1sym),
              buildVarRefExp(op2sym)),
            buildUnsignedLongLongIntValHex(0xFF)),
          bbForCxNotZero);

      setFlagsForSubtract(op1sym, op2sym, resultsym, SgAsmTypeByte::createType(), bbForCxNotZero);

      append(buildIfStmt(
               buildNotEqualOp(f->makeRegisterRead(x86_regclass_gpr, x86_gpr_cx, x86_regpos_all), buildUnsignedLongLongIntValHex(0)),
               bbForCxNotZero,
               buildBasicBlock()));
      break;
    }

    case x86_repne_scasb: {
      ROSE_ASSERT (operands.size() == 0);
      SgBasicBlock* bbForCxNotZero = buildBasicBlock();

      append(
        buildWhileStmt(
          buildNotEqualOp(f->makeRegisterRead(x86_regclass_gpr, x86_gpr_cx, x86_regpos_all), buildUnsignedLongLongIntValHex(0)),
          buildBasicBlock(
            f->makeRegisterWrite(x86_regclass_gpr, x86_gpr_cx, x86_regpos_all, buildSubtractOp(f->makeRegisterRead(x86_regclass_gpr, x86_gpr_cx, x86_regpos_all), buildIntVal(1))),
            buildIfStmt(
              buildEqualityOp(
                f->makeMemoryRead(x86_segreg_es, f->makeRegisterRead(x86_regclass_gpr, x86_gpr_di, x86_regpos_all), SgAsmTypeByte::createType()),
                makeRead(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_low_byte))),
              buildBasicBlock(buildBreakStmt()),
              buildBasicBlock()),
            f->makeRegisterWrite(x86_regclass_gpr, x86_gpr_di, x86_regpos_all, buildAddOp(f->makeRegisterRead(x86_regclass_gpr, x86_gpr_di, x86_regpos_all), buildConditionalExp(f->makeFlagRead(x86flag_df), buildIntVal(-1), buildIntVal(1))))
            )),
        bbForCxNotZero);

      SgVariableSymbol* op1sym = cacheValue(f->makeRegisterRead(x86_regclass_gpr, x86_gpr_ax, x86_regpos_low_byte), bbForCxNotZero);
      SgVariableSymbol* op2sym = cacheValue(f->makeMemoryRead(x86_segreg_es, f->makeRegisterRead(x86_regclass_gpr, x86_gpr_di, x86_regpos_all), SgAsmTypeByte::createType()), bbForCxNotZero);
      SgVariableSymbol* resultsym =
        makeNewVariableInBlock(
          SgTypeUnsignedChar::createType(),
          buildBitAndOp(
            buildSubtractOp(
              buildVarRefExp(op1sym),
              buildVarRefExp(op2sym)),
            buildUnsignedLongLongIntValHex(0xFF)),
          bbForCxNotZero);

      setFlagsForSubtract(op1sym, op2sym, resultsym, SgAsmTypeByte::createType(), bbForCxNotZero);

      append(buildIfStmt(
               buildNotEqualOp(f->makeRegisterRead(x86_regclass_gpr, x86_gpr_cx, x86_regpos_all), buildIntVal(0)),
               bbForCxNotZero,
               buildBasicBlock()));
      break;
    }

    case x86_lodsb: {
      X86SegmentRegister segForSrc = insn->get_segmentOverride();
      SgVariableSymbol* op1sym = cacheValue(f->makeMemoryRead(segForSrc, f->makeRegisterRead(x86_regclass_gpr, x86_gpr_si, x86_regpos_all), SgAsmTypeByte::createType()));
      append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_low_byte), buildVarRefExp(op1sym)));
      append(
          f->makeRegisterWrite(x86_regclass_gpr, x86_gpr_si, x86_regpos_all, buildAddOp(f->makeRegisterRead(x86_regclass_gpr, x86_gpr_si, x86_regpos_all), buildConditionalExp(f->makeFlagRead(x86flag_df), buildIntVal(-1), buildIntVal(1)))));
      break;
    }

    case x86_lodsd: {
      X86SegmentRegister segForSrc = insn->get_segmentOverride();
      SgVariableSymbol* op1sym = cacheValue(f->makeMemoryRead(segForSrc, f->makeRegisterRead(x86_regclass_gpr, x86_gpr_si, x86_regpos_all), SgAsmTypeDoubleWord::createType()));
      append(f->makeRegisterWrite(x86_regclass_gpr, x86_gpr_ax, x86_regpos_dword, buildVarRefExp(op1sym)));
      append(
          f->makeRegisterWrite(x86_regclass_gpr, x86_gpr_si, x86_regpos_all, buildAddOp(f->makeRegisterRead(x86_regclass_gpr, x86_gpr_si, x86_regpos_all), buildConditionalExp(f->makeFlagRead(x86flag_df), buildIntVal(-1), buildIntVal(1)))));
      break;
    }

    case x86_add:
       {
         ROSE_ASSERT (operands.size() == 2);
         SgVariableSymbol* op1sym = cacheValue(operands[0]);
         SgVariableSymbol* op2sym = cacheValue(operands[1]);
         SgVariableSymbol* resultsym =
           makeNewVariableInBlock(
             asmTypeToCType(operands[0]->get_type()),
             buildAddOp(
               buildVarRefExp(op1sym),
               buildVarRefExp(op2sym)));

         setFlagsBasedOnResult(resultsym, operands[0]->get_type(), true);

         append(f->makeFlagWrite(x86flag_cf,
                              buildLessThanOp(
                                buildBitComplementOp(buildVarRefExp(op1sym)),
                                buildVarRefExp(op2sym))));

   // Auxiliary carry (for decimal arithmetic) (carry out of the first 4 bits)
      append(f->makeFlagWrite(x86flag_af,
                              buildGreaterOrEqualOp(
                                 buildAddOp(
                                    buildBitAndOp(
                                       buildVarRefExp(op1sym),
                                       buildIntValHex(0xF)),
                                    buildBitAndOp(
                                       buildVarRefExp(op2sym),
                                       buildIntValHex(0xF))), 
                                 buildIntValHex(0x10))));

   // Overflow is determined by different sign from inputs if they have the same sign      
   // This version uses some tricks to be a bit faster
   // s^o = (a ^ (signMask - 1)) >= (b ^ signMask)

      append(f->makeFlagWrite(x86flag_of,
                              buildNotEqualOp(
                                f->makeFlagRead(x86flag_sf),
                                buildGreaterOrEqualOp(
                                  buildBitXorOp(
                                    buildVarRefExp(op1sym),
                                    buildNotSignBitMaskForType(operands[0]->get_type())),
                                  buildBitXorOp(
                                    buildVarRefExp(op2sym),
                                    buildSignBitMaskForType(operands[0]->get_type()))))));
         append(makeWrite(operands[0], buildVarRefExp(resultsym)));

         break;
       }

    case x86_adc:
       {
         ROSE_ASSERT (operands.size() == 2);
         SgVariableSymbol* op1sym = cacheValue(operands[0]);
         SgVariableSymbol* op2sym = cacheValue(operands[1]);
         SgVariableSymbol* resultsym =
           makeNewVariableInBlock(
             asmTypeToCType(operands[0]->get_type()),
             buildAddOp(
               buildVarRefExp(op1sym),
               buildAddOp(
                 buildVarRefExp(op2sym),
                 buildConditionalExp(
                   f->makeFlagRead(x86flag_cf),
                   buildIntValHex(1),
                   buildIntValHex(0)))));

         setFlagsBasedOnResult(resultsym, operands[0]->get_type(), true);

   // Auxiliary carry (for decimal arithmetic) (carry out of the first 4 bits)
      append(f->makeFlagWrite(x86flag_af,
                              buildGreaterOrEqualOp(
                                buildAddOp(
                                  buildAddOp(
                                    buildBitAndOp(
                                      buildVarRefExp(op1sym),
                                      buildIntValHex(0xF)),
                                    buildBitAndOp(
                                      buildVarRefExp(op2sym),
                                      buildIntValHex(0xF))), 
                                  buildConditionalExp(
                                    f->makeFlagRead(x86flag_cf),
                                    buildIntValHex(1),
                                    buildIntValHex(0))),
                                 buildIntValHex(0x10))));

   // Overflow is determined by different sign from inputs if they have the same sign      
   // This version uses some tricks to be a bit faster
      append(f->makeFlagWrite(x86flag_of,
                              buildNotEqualOp(
                                buildBitAndOp(
                                  buildBitAndOp(
                                    buildBitComplementOp(
                                      buildBitXorOp(
                                        buildVarRefExp(op1sym),
                                        buildVarRefExp(op2sym))),
                                    buildBitXorOp(
                                      buildVarRefExp(op1sym),
                                      buildVarRefExp(resultsym))),
                                  buildSignBitMaskForType(
                                    operands[0]->get_type())),
                                buildIntValHex(0))));

      append(makeWrite(operands[0], buildVarRefExp(resultsym)));

      append(f->makeFlagWrite(x86flag_cf,
                              buildOrOp(
                                buildLessThanOp(
                                  buildBitComplementOp(buildVarRefExp(op1sym)),
                                  buildVarRefExp(op2sym)),
                                buildAndOp(
                                  f->makeFlagRead(x86flag_cf) /* cf before */,
                                  f->makeFlagRead(x86flag_zf) /* zf after */
                                ))));

      break;
       }

    case x86_xor:
       {
         ROSE_ASSERT (operands.size() == 2);
         SgVariableSymbol* op1sym = cacheValue(operands[0]);
         SgVariableSymbol* op2sym = cacheValue(operands[1]);
         SgVariableSymbol* resultsym =
           makeNewVariableInBlock(
             asmTypeToCType(operands[0]->get_type()),
             buildBitXorOp(
               buildVarRefExp(op1sym),
               buildVarRefExp(op2sym)));

         setFlagsBasedOnResult(resultsym, operands[0]->get_type(), false);

         append(f->makeFlagWrite(x86flag_zf,
                                 buildEqualityOp(
                                   buildVarRefExp(op1sym),
                                   buildVarRefExp(op2sym))));

         append(f->makeFlagWrite(x86flag_cf, buildBoolValExp(false)));

      // Auxiliary carry (for decimal arithmetic) (carry out of the first 4 bits)
      // Marked as undefined, we will set it to false (Bochs emulator sets this to false).
         append(f->makeFlagWrite(x86flag_af, buildBoolValExp(false)));

      // Overflow is marked as being false.
         append(f->makeFlagWrite(x86flag_of, buildBoolValExp(false)));

         append(makeWrite(operands[0], buildVarRefExp(resultsym)));

         break;
       }

    case x86_or:
       {
         ROSE_ASSERT (operands.size() == 2);
         SgVariableSymbol* op1sym = cacheValue(operands[0]);
         SgVariableSymbol* op2sym = cacheValue(operands[1]);
         SgVariableSymbol* resultsym =
           makeNewVariableInBlock(
             asmTypeToCType(operands[0]->get_type()),
             buildBitOrOp(
               buildVarRefExp(op1sym),
               buildVarRefExp(op2sym)));

         setFlagsBasedOnResult(resultsym, operands[0]->get_type(), true);

         append(f->makeFlagWrite(x86flag_cf, buildBoolValExp(false)));

      // Auxiliary carry (for decimal arithmetic) (carry out of the first 4 bits)
      // Marked as undefined, we will set it to false (Bochs emulator sets this to false).
         append(f->makeFlagWrite(x86flag_af, buildBoolValExp(false)));

      // Overflow is marked as being false.
         append(f->makeFlagWrite(x86flag_of, buildBoolValExp(false)));

         append(makeWrite(operands[0], buildVarRefExp(resultsym)));

         break;
       }

    case x86_and:
       {
         ROSE_ASSERT (operands.size() == 2);
         SgVariableSymbol* op1sym = cacheValue(operands[0]);
         SgVariableSymbol* op2sym = cacheValue(operands[1]);
         SgVariableSymbol* resultsym =
           makeNewVariableInBlock(
             asmTypeToCType(operands[0]->get_type()),
             buildBitAndOp(
               buildVarRefExp(op1sym),
               buildVarRefExp(op2sym)));

         setFlagsBasedOnResult(resultsym, operands[0]->get_type(), true);

         append(f->makeFlagWrite(x86flag_cf, buildBoolValExp(false)));

      // Auxiliary carry (for decimal arithmetic) (carry out of the first 4 bits)
      // Marked as undefined, we will set it to false (Bochs emulator sets this to false).
         append(f->makeFlagWrite(x86flag_af, buildBoolValExp(false)));

      // Overflow is marked as being false.
         append(f->makeFlagWrite(x86flag_of, buildBoolValExp(false)));

         append(makeWrite(operands[0], buildVarRefExp(resultsym)));

         break;
       }

 // Same as AND but skips output of result
    case x86_test:
       {
         ROSE_ASSERT (operands.size() == 2);
         SgVariableSymbol* op1sym = cacheValue(operands[0]);
         SgVariableSymbol* op2sym = cacheValue(operands[1]);
         SgVariableSymbol* resultsym =
           makeNewVariableInBlock(
             asmTypeToCType(operands[0]->get_type()),
             buildBitAndOp(
               buildVarRefExp(op1sym),
               buildVarRefExp(op2sym)));

         setFlagsBasedOnResult(resultsym, operands[0]->get_type(), true);

         append(f->makeFlagWrite(x86flag_cf, buildBoolValExp(false)));

      // Auxiliary carry (for decimal arithmetic) (carry out of the first 4 bits)
      // Marked as undefined, we will set it to false (Bochs emulator sets this to false).
         append(f->makeFlagWrite(x86flag_af, buildBoolValExp(false)));

      // Overflow is marked as being false.
         append(f->makeFlagWrite(x86flag_of, buildBoolValExp(false)));

         break;
       }

    case x86_not:
       {
         ROSE_ASSERT (operands.size() == 1);
         SgVariableSymbol* op1sym = cacheValue(operands[0]);
         SgVariableSymbol* resultsym =
           makeNewVariableInBlock(
             asmTypeToCType(operands[0]->get_type()),
             buildBitComplementOp(buildVarRefExp(op1sym)));

         setFlagsBasedOnResult(resultsym, operands[0]->get_type(), true);

         append(f->makeFlagWrite(x86flag_cf, buildBoolValExp(false)));

      // Auxiliary carry (for decimal arithmetic) (carry out of the first 4 bits)
      // Marked as undefined, we will set it to false (Bochs emulator sets this to false).
         append(f->makeFlagWrite(x86flag_af, buildBoolValExp(false)));

      // Overflow is marked as being false.
         append(f->makeFlagWrite(x86flag_of, buildBoolValExp(false)));

         append(makeWrite(operands[0], buildVarRefExp(resultsym)));

         break;
       }

    case x86_neg:
       {
         ROSE_ASSERT (operands.size() == 1);
         SgVariableSymbol* op1sym = cacheValue(operands[0]);
         SgVariableSymbol* subtractOpSym = cacheValue(buildIntValHex(0));
         SgVariableSymbol* resultsym =
           makeNewVariableInBlock(
             asmTypeToCType(operands[0]->get_type()),
             buildMinusOp(buildVarRefExp(op1sym)));

         setFlagsBasedOnResult(resultsym, operands[0]->get_type(), false);

         append(f->makeFlagWrite(x86flag_zf, buildEqualityOp(buildVarRefExp(op1sym), buildIntValHex(0))));
         append(f->makeFlagWrite(x86flag_cf, buildEqualityOp(buildVarRefExp(op1sym), buildIntValHex(0))));
         append(f->makeFlagWrite(x86flag_of, buildEqualityOp(buildVarRefExp(op1sym), buildSignBitMaskForType(operands[0]->get_type()))));
         append(f->makeFlagWrite(x86flag_af, buildEqualityOp(buildBitAndOp(buildVarRefExp(op1sym), buildIntValHex(0xF)), buildIntValHex(0))));

         append(makeWrite(operands[0], buildVarRefExp(resultsym)));

         break;
       }

    case x86_cwd: {
      ROSE_ASSERT (operands.size() == 0);
      append(
          makeWrite(
            new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_dx, x86_regpos_word),
            buildConditionalExp(
              buildNotEqualOp(
                buildBitAndOp(
                  makeRead(
                    new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_word)),
                  buildSignBitMaskForType(SgAsmTypeWord::createType())),
                buildIntValHex(0)),
              buildIntValHex(0xFFFF),
              buildIntValHex(0))));
      break;
    }

    case x86_cdq: {
      ROSE_ASSERT (operands.size() == 0);
      append(
          makeWrite(
            new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_dx, x86_regpos_dword),
            buildConditionalExp(
              buildNotEqualOp(
                buildBitAndOp(
                  makeRead(
                    new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_dword)),
                  buildSignBitMaskForType(SgAsmTypeDoubleWord::createType())),
                buildIntValHex(0)),
              buildIntValHex(0xFFFFFFFFUL),
              buildIntValHex(0))));
      break;
    }

    case x86_shl:
       {
         ROSE_ASSERT (operands.size() == 2);
         SgVariableSymbol* op1sym = cacheValue(operands[0]);
         SgVariableSymbol* op2sym = cacheValue(operands[1]);

      // Compute the shift count depending if this is for 32 bit or 64 bit
         SgVariableSymbol* shiftCountSym = cacheValue(buildBitAndOp(buildVarRefExp(op2sym),buildIntValHex(insn->get_operandSize() == x86_insnsize_64 ? 0x3F : 0x1F)));

         SgVariableSymbol* resultsym =
           makeNewVariableInBlock(
             asmTypeToCType(operands[0]->get_type()),
               buildLshiftOp(
                 buildVarRefExp(op1sym),
                 buildVarRefExp(shiftCountSym)));

         append(makeWrite(operands[0], buildVarRefExp(resultsym)));

         SgBasicBlock* thenBasicBlock = buildBasicBlock();

         setFlagsBasedOnResult(resultsym, operands[0]->get_type(), true, thenBasicBlock);
      // CF is set to ((op1 << (shiftCount-1)) AND (signMask for op1 type)) != 0
      // appendStatement(f->makeFlagWrite(x86flag_cf, buildBoolValExp(false)),thenBasicBlock);
         appendStatement(f->makeFlagWrite(x86flag_cf, buildNotEqualOp(buildBitAndOp(buildLshiftOp(buildVarRefExp(op1sym),
                                                                                                  buildSubtractOp(buildVarRefExp(shiftCountSym),buildIntValHex(1))),
                                                                                    buildSignBitMaskForType(operands[0]->get_type())),
                                                                      buildIntValHex(0))),
                         thenBasicBlock);

      // Auxiliary carry (for decimal arithmetic) (carry out of the first 4 bits)
      // Marked as undefined, we will set it to false (Bochs emulator sets this to false).
         appendStatement(f->makeFlagWrite(x86flag_af, buildBoolValExp(false)),thenBasicBlock);

      // Overflow is marked as: shiftCount == 1 ? CF xor SF : undefined (false)
      // appendStatement(f->makeFlagWrite(x86flag_of, buildBoolValExp(false)),thenBasicBlock);
         appendStatement(f->makeFlagWrite(x86flag_of, buildConditionalExp(buildEqualityOp(buildVarRefExp(shiftCountSym),
                                                                                          buildIntValHex(1)),
                                                                          buildNotEqualOp(f->makeFlagRead(x86flag_cf),f->makeFlagRead(x86flag_sf)),
                                                                          buildBoolValExp(false))),
                         thenBasicBlock);

         append(buildIfStmt(buildNotEqualOp(buildVarRefExp(shiftCountSym),buildIntValHex(0)),thenBasicBlock,buildBasicBlock()));

         break;
       }

    case x86_shr:
       {
         ROSE_ASSERT (operands.size() == 2);
         SgVariableSymbol* op1sym = cacheValue(operands[0]);
         SgVariableSymbol* op2sym = cacheValue(operands[1]);

      // Compute the shift count depending if this is for 32 bit or 64 bit
         SgVariableSymbol* shiftCountSym = cacheValue(buildBitAndOp(buildVarRefExp(op2sym),buildIntValHex(insn->get_operandSize() == x86_insnsize_64 ? 0x3F : 0x1F)));

         SgVariableSymbol* resultsym =
           makeNewVariableInBlock(
             asmTypeToCType(operands[0]->get_type()),
               buildRshiftOp(
                 buildVarRefExp(op1sym),
                 buildVarRefExp(shiftCountSym)));

         append(makeWrite(operands[0], buildVarRefExp(resultsym)));

         SgBasicBlock* thenBasicBlock = buildBasicBlock();

         setFlagsBasedOnResult(resultsym, operands[0]->get_type(), true, thenBasicBlock);
      // CF is set to ((op1 >> (shiftCount-1)) AND (1)) != 0
         appendStatement(f->makeFlagWrite(x86flag_cf, buildNotEqualOp(buildBitAndOp(buildRshiftOp(buildVarRefExp(op1sym),
                                                                                                  buildSubtractOp(buildVarRefExp(shiftCountSym),buildIntValHex(1))),
                                                                                    buildIntValHex(1)),
                                                                      buildIntValHex(0))),
                         thenBasicBlock);

      // Auxiliary carry (for decimal arithmetic) (carry out of the first 4 bits)
      // Marked as undefined, we will set it to false (Bochs emulator sets this to false).
         appendStatement(f->makeFlagWrite(x86flag_af, buildBoolValExp(false)),thenBasicBlock);

      // Overflow is marked as: shiftCount == 1 ? (sign bit of op1sym != 0) : undefined (false)
      // appendStatement(f->makeFlagWrite(x86flag_of, buildBoolValExp(false)),thenBasicBlock);
         appendStatement(f->makeFlagWrite(x86flag_of, buildConditionalExp(buildEqualityOp(buildVarRefExp(shiftCountSym),
                                                                                          buildIntValHex(1)),
                                                                          buildNotEqualOp(buildBitAndOp(buildVarRefExp(op1sym),
                                                                                                        buildSignBitMaskForType(operands[0]->get_type())),
                                                                                          buildIntValHex(0)),
                                                                          buildBoolValExp(false))),
                         thenBasicBlock);

         append(buildIfStmt(buildNotEqualOp(buildVarRefExp(shiftCountSym),buildIntValHex(0)),thenBasicBlock,buildBasicBlock()));

         break;
       }

    case x86_sar:
       {
         ROSE_ASSERT (operands.size() == 2);
         SgVariableSymbol* op1sym = cacheValue(operands[0]);
         SgVariableSymbol* op2sym = cacheValue(operands[1]);

      // Compute the shift count depending if this is for 32 bit or 64 bit
         SgVariableSymbol* shiftCountSym = cacheValue(buildBitAndOp(buildVarRefExp(op2sym),buildIntValHex(insn->get_operandSize() == x86_insnsize_64 ? 0x3F : 0x1F)));

         SgVariableSymbol* resultsym =
           makeNewVariableInBlock(
             asmTypeToCType(operands[0]->get_type()),
               buildRshiftOp(
                 buildCastExp(
                   buildVarRefExp(op1sym),
                   asmTypeToCSignedType(operands[0]->get_type())),
                 buildVarRefExp(shiftCountSym)));

         append(makeWrite(operands[0], buildVarRefExp(resultsym)));

         SgBasicBlock* thenBasicBlock = buildBasicBlock();

         setFlagsBasedOnResult(resultsym, operands[0]->get_type(), true, thenBasicBlock);
      // CF is set to ((op1 >> (shiftCount-1)) AND (1)) != 0
         appendStatement(
           f->makeFlagWrite(
             x86flag_cf,
             buildNotEqualOp(
               buildBitAndOp(
                 buildRshiftOp(
                   buildCastExp(
                     buildVarRefExp(op1sym),
                     asmTypeToCSignedType(operands[0]->get_type())),
                   buildSubtractOp(
                     buildVarRefExp(shiftCountSym),
                     buildIntValHex(1))),
                 buildIntValHex(1)),
               buildIntValHex(0))),
           thenBasicBlock);

      // Auxiliary carry (for decimal arithmetic) (carry out of the first 4 bits)
      // Marked as undefined, we will set it to false (Bochs emulator sets this to false).
         appendStatement(f->makeFlagWrite(x86flag_af, buildBoolValExp(false)),thenBasicBlock);

      // Overflow is marked as: shiftCount == 1 ? 0 : undefined (false)
         appendStatement(f->makeFlagWrite(x86flag_of, buildBoolValExp(false)),
                         thenBasicBlock);

         append(buildIfStmt(buildNotEqualOp(buildVarRefExp(shiftCountSym),buildIntValHex(0)),thenBasicBlock,buildBasicBlock()));

         break;
       }

    case x86_shrd:
       {
         ROSE_ASSERT (operands.size() == 3);
         SgVariableSymbol* op1sym = cacheValue(operands[0]);
         SgVariableSymbol* op2sym = cacheValue(operands[1]);
         SgVariableSymbol* op3sym = cacheValue(operands[2]);

      // Compute the shift count depending if this is for 32 bit or 64 bit
         SgVariableSymbol* shiftCountSym = cacheValue(buildBitAndOp(buildVarRefExp(op3sym),buildIntValHex(insn->get_operandSize() == x86_insnsize_64 ? 0x3F : 0x1F)));

         SgVariableSymbol* resultsym =
           makeNewVariableInBlock(
             asmTypeToCType(operands[0]->get_type()),
               buildBitOrOp(
                 buildRshiftOp(
                   buildVarRefExp(op1sym),
                   buildVarRefExp(shiftCountSym)),
                 buildLshiftOp(
                   buildVarRefExp(op2sym),
                   buildSubtractOp(
                     buildIntValHex(sizeOfAsmType(operands[0]->get_type()) * 8),
                     buildVarRefExp(shiftCountSym)))));

         append(makeWrite(operands[0], buildVarRefExp(resultsym)));

         SgBasicBlock* thenBasicBlock = buildBasicBlock();

         setFlagsBasedOnResult(resultsym, operands[0]->get_type(), true, thenBasicBlock);
      // CF is set to ((op1 >> (shiftCount-1)) AND (1)) != 0
         appendStatement(f->makeFlagWrite(x86flag_cf, buildNotEqualOp(buildBitAndOp(buildRshiftOp(buildVarRefExp(op1sym),
                                                                                                  buildSubtractOp(buildVarRefExp(shiftCountSym),buildIntValHex(1))),
                                                                                    buildIntValHex(1)),
                                                                      buildIntValHex(0))),
                         thenBasicBlock);

      // Auxiliary carry (for decimal arithmetic) (carry out of the first 4 bits)
      // Marked as undefined, we will set it to false (Bochs emulator sets this to false).
         appendStatement(f->makeFlagWrite(x86flag_af, buildBoolValExp(false)),thenBasicBlock);

      // Overflow is marked as: shiftCount == 1 ? (sign bit of op1sym != sign bit of resultsym) : undefined (false)
         appendStatement(
           f->makeFlagWrite(
             x86flag_of,
             buildConditionalExp(
               buildEqualityOp(
                 buildVarRefExp(shiftCountSym),
                 buildIntValHex(1)),
               buildNotEqualOp(buildBitAndOp(
                   buildBitXorOp(
                     buildVarRefExp(op1sym),
                     buildVarRefExp(resultsym)),
                   buildSignBitMaskForType(operands[0]->get_type())),
                 buildIntValHex(0)),
               buildBoolValExp(false))),
           thenBasicBlock);

         append(buildIfStmt(buildNotEqualOp(buildVarRefExp(shiftCountSym),buildIntValHex(0)),thenBasicBlock,buildBasicBlock()));

         break;
       }

    case x86_ror:
       {
         ROSE_ASSERT (operands.size() == 2);
         SgVariableSymbol* op1sym = cacheValue(operands[0]);
         SgVariableSymbol* op2sym = cacheValue(operands[1]);

      // Compute the shift count depending if this is for 32 bit or 64 bit
         SgVariableSymbol* shiftCountSym = cacheValue(buildBitAndOp(buildVarRefExp(op2sym),buildIntValHex(insn->get_operandSize() == x86_insnsize_64 ? 0x3F : 0x1F)));

         SgVariableSymbol* resultsym =
           makeNewVariableInBlock(
             asmTypeToCType(operands[0]->get_type()),
               buildBitOrOp(
                 buildRshiftOp(
                   buildVarRefExp(op1sym),
                   buildVarRefExp(shiftCountSym)),
                 buildLshiftOp(
                   buildVarRefExp(op1sym),
                   buildSubtractOp(
                     buildIntValHex(sizeOfAsmType(operands[0]->get_type()) * 8),
                     buildVarRefExp(shiftCountSym)))));

         append(makeWrite(operands[0], buildVarRefExp(resultsym)));

         SgBasicBlock* thenBasicBlock = buildBasicBlock();

      // CF is set to ((op1 >> (shiftCount-1)) AND (1)) != 0
         appendStatement(
             f->makeFlagWrite(
               x86flag_cf,
               buildNotEqualOp(
                 buildBitAndOp(
                   buildRshiftOp(
                     buildVarRefExp(op1sym),
                     buildSubtractOp(
                       buildVarRefExp(shiftCountSym),
                       buildIntValHex(1))),
                   buildIntValHex(1)),
                 buildIntValHex(0))),
             thenBasicBlock);

      // Overflow is marked as: shiftCount == 1 ? (sign bit of op1sym != sign bit of resultsym) : undefined (false)
         appendStatement(
           f->makeFlagWrite(
             x86flag_of,
             buildConditionalExp(
               buildEqualityOp(
                 buildVarRefExp(shiftCountSym),
                 buildIntValHex(1)),
               buildNotEqualOp(buildBitAndOp(
                   buildBitXorOp(
                     buildVarRefExp(op1sym),
                     buildVarRefExp(resultsym)),
                   buildSignBitMaskForType(operands[0]->get_type())),
                 buildIntValHex(0)),
               buildBoolValExp(false))),
           thenBasicBlock);

         append(buildIfStmt(buildNotEqualOp(buildVarRefExp(shiftCountSym),buildIntValHex(0)),thenBasicBlock,buildBasicBlock()));

         break;
       }

    case x86_bsr: {
      ROSE_ASSERT (operands.size() == 2);
      SgVariableSymbol* inputsym = cacheValue(operands[1]);
      append(f->makeFlagWrite(x86flag_of, buildBoolValExp(false))); // undef
      append(f->makeFlagWrite(x86flag_sf, buildBoolValExp(false))); // undef
      append(f->makeFlagWrite(x86flag_af, buildBoolValExp(false))); // undef
      append(f->makeFlagWrite(x86flag_pf, buildBoolValExp(false))); // undef
      append(f->makeFlagWrite(x86flag_cf, buildBoolValExp(false))); // undef
      append(
        buildIfStmt(
          buildEqualityOp(buildVarRefExp(inputsym), buildIntValHex(0)),
          buildBasicBlock(
            f->makeFlagWrite(x86flag_zf, buildBoolValExp(true))),
          buildBasicBlock(
            f->makeFlagWrite(x86flag_zf, buildBoolValExp(false)),
            makeWrite(operands[0], buildFunctionCallExp(f->getHelperFunction("bsr"), buildExprListExp(buildVarRefExp(inputsym)))))));
      break;
    }

    case x86_bsf: {
      ROSE_ASSERT (operands.size() == 2);
      SgVariableSymbol* inputsym = cacheValue(operands[1]);
      append(f->makeFlagWrite(x86flag_of, buildBoolValExp(false))); // undef
      append(f->makeFlagWrite(x86flag_sf, buildBoolValExp(false))); // undef
      append(f->makeFlagWrite(x86flag_af, buildBoolValExp(false))); // undef
      append(f->makeFlagWrite(x86flag_pf, buildBoolValExp(false))); // undef
      append(f->makeFlagWrite(x86flag_cf, buildBoolValExp(false))); // undef
      append(
        buildIfStmt(
          buildEqualityOp(buildVarRefExp(inputsym), buildIntValHex(0)),
          buildBasicBlock(
            f->makeFlagWrite(x86flag_zf, buildBoolValExp(true))),
          buildBasicBlock(
            f->makeFlagWrite(x86flag_zf, buildBoolValExp(false)),
            makeWrite(operands[0], buildFunctionCallExp(f->getHelperFunction("bsf"), buildExprListExp(buildVarRefExp(inputsym)))))));
      break;
    }

    case x86_inc:
       {
         ROSE_ASSERT (operands.size() == 1);
         SgVariableSymbol* op1sym = cacheValue(operands[0]);
         SgVariableSymbol* resultsym =
           makeNewVariableInBlock(
             asmTypeToCType(operands[0]->get_type()),
             buildAddOp(
               buildVarRefExp(op1sym),
               buildIntValHex(1)));

         setFlagsBasedOnResult(resultsym, operands[0]->get_type(), true);

      // Auxiliary carry (for decimal arithmetic) (carry out of the first 4 bits)
         append(f->makeFlagWrite(x86flag_af, buildEqualityOp(buildBitAndOp(buildVarRefExp(resultsym),buildIntValHex(0x0F)),buildIntValHex(0))));

      // Overflow is computed based on if the sign bit changed.
         append(f->makeFlagWrite(x86flag_of, buildEqualityOp(buildVarRefExp(resultsym),buildSignBitMaskForType(operands[0]->get_type()))));

         append(makeWrite(operands[0], buildVarRefExp(resultsym)));

         break;
       }

    case x86_dec:
       {
      // Same code as for x86_inc, except using buildSubtractOp() instead of buildAddOp()
         ROSE_ASSERT (operands.size() == 1);
         SgVariableSymbol* op1sym = cacheValue(operands[0]);
         SgVariableSymbol* resultsym =
           makeNewVariableInBlock(
             asmTypeToCType(operands[0]->get_type()),
             buildSubtractOp(
               buildVarRefExp(op1sym),
               buildIntValHex(1)));

         setFlagsBasedOnResult(resultsym, operands[0]->get_type(), true);

      // Auxiliary carry (for decimal arithmetic) (carry out of the first 4 bits)
         append(f->makeFlagWrite(x86flag_af, buildEqualityOp(buildBitAndOp(buildVarRefExp(resultsym),buildIntValHex(0x0F)),buildIntValHex(0x0F))));

      // Overflow is computed based on if the sign bit changed.
         append(f->makeFlagWrite(x86flag_of, buildEqualityOp(buildVarRefExp(resultsym),buildNotSignBitMaskForType(operands[0]->get_type()))));

         append(makeWrite(operands[0], buildVarRefExp(resultsym)));

         break;
       }

    case x86_push: {
      ROSE_ASSERT (operands.size() == 1);
      makePush(makeRead(operands[0]));
      break;
    }

    case x86_pop: {
      ROSE_ASSERT (operands.size() == 1);
      SgVariableSymbol* sym = makePop();
      append(makeWrite(operands[0], buildVarRefExp(sym)));
      break;
    }

    case x86_jmp: {
      ROSE_ASSERT (operands.size() == 1);
      jumpToNextInstruction = false;
      append(f->makeJump(makeRead(operands[0])));
      break;
    }

#define DO_CONDITIONAL_JUMP(name) \
    case x86_j##name: { \
      ROSE_ASSERT (operands.size() == 1); \
      jumpToNextInstruction = false; \
      ROSE_ASSERT (isSgAsmQuadWordValueExpression(operands[0])); \
      append(f->makeConditionalJump(insn->get_address(), f->makeCondition(x86cond_##name), isSgAsmQuadWordValueExpression(operands[0])->get_value(), nextIP)); \
      break; \
    }

#define DO_CONDITIONAL_JUMP_CXZ(name) \
    case x86_j##name: { \
      ROSE_ASSERT (operands.size() == 1); \
      jumpToNextInstruction = false; \
      ROSE_ASSERT (isSgAsmQuadWordValueExpression(operands[0])); \
      append(f->makeConditionalJump(insn->get_address(), CONDITION_##name, isSgAsmQuadWordValueExpression(operands[0])->get_value(), nextIP)); \
      break; \
    }

#define CONDITION_cxz buildEqualityOp(makeRead(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_cx, x86_regpos_word)), buildIntValHex(0))
#define CONDITION_ecxz buildEqualityOp(makeRead(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_cx, x86_regpos_dword)), buildIntValHex(0))
#define CONDITION_rcxz buildEqualityOp(makeRead(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_cx, x86_regpos_qword)), buildUnsignedLongLongIntValHex(0))

    DO_CONDITIONAL_JUMP(e)
    DO_CONDITIONAL_JUMP(ne)
    DO_CONDITIONAL_JUMP(s)
    DO_CONDITIONAL_JUMP(ns)
    DO_CONDITIONAL_JUMP(le)
    DO_CONDITIONAL_JUMP(g)
    DO_CONDITIONAL_JUMP(l)
    DO_CONDITIONAL_JUMP(ge)
    DO_CONDITIONAL_JUMP(a)
    DO_CONDITIONAL_JUMP(be)
    DO_CONDITIONAL_JUMP(b)
    DO_CONDITIONAL_JUMP(ae)
    DO_CONDITIONAL_JUMP(o)
    DO_CONDITIONAL_JUMP(no)
    DO_CONDITIONAL_JUMP(pe)
    DO_CONDITIONAL_JUMP(po)
    DO_CONDITIONAL_JUMP_CXZ(cxz)
    DO_CONDITIONAL_JUMP_CXZ(ecxz)
    DO_CONDITIONAL_JUMP_CXZ(rcxz)

#undef DO_CONDITIONAL_JUMP
#undef DO_CONDITIONAL_JUMP_CXZ

    case x86_call: {
      ROSE_ASSERT (operands.size() == 1);
      makePush(buildUnsignedLongLongIntValHex(nextIP));
      jumpToNextInstruction = false;
      append(f->makeJump(makeRead(operands[0])));
      break;
    }

    case x86_ret: {
      ROSE_ASSERT (operands.size() <= 1);
      SgVariableSymbol* newIP = makePop();
      jumpToNextInstruction = false;
      if (operands.size() == 1) {
        append(f->makeRegisterWrite(x86_regclass_gpr, x86_gpr_sp, x86_regpos_all,
                 buildAddOp(
                   f->makeRegisterRead(x86_regclass_gpr, x86_gpr_sp, x86_regpos_all),
                   makeRead(operands[0]))));
      }
      append(f->makeJump(buildVarRefExp(newIP)));
      break;
    }

    case x86_leave: {
      ROSE_ASSERT (operands.empty());
      append(
        makeWrite(
          new SgAsmx86RegisterReferenceExpression(
                x86_regclass_gpr,
                x86_gpr_sp,
             // using baseSize to stand for the stackAddressSize, we need 
             // something in the framework to determine the stackAddressSize
                sizeToPos(insn->get_baseSize())),
          f->makeRegisterRead(x86_regclass_gpr, x86_gpr_bp, x86_regpos_all)));
      SgVariableSymbol* newBP = makePop();
      append(
        makeWrite(
          new SgAsmx86RegisterReferenceExpression(
                x86_regclass_gpr,
                x86_gpr_bp,
                sizeToPos(insn->get_operandSize())),
          buildVarRefExp(newBP)));
      break;
    }

    case x86_lds:
    case x86_les:
    case x86_lfs:
    case x86_lgs:
    case x86_lss: {
      ROSE_ASSERT (operands.size() == 2);
      SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(operands[1]);
      ROSE_ASSERT (mr);
      SgAsmExpression* addr = mr->get_address();
      SgExpression* addrC = makeRead(addr);
      X86SegmentRegister segreg = getSegregFromMemoryReference(mr);
      SgVariableSymbol* addrResult = cacheValue(f->makeMemoryRead(segreg, addrC, operands[0]->get_type()));
      SgVariableSymbol* segResult = cacheValue(f->makeMemoryRead(segreg, buildAddOp(addrC, buildIntValHex(sizeOfAsmType(operands[0]->get_type()))), SgAsmTypeWord::createType()));
      append(makeWrite(operands[0], buildVarRefExp(addrResult)));
      X86SegmentRegister segregToWrite = x86_segreg_ds;
      switch (insn->get_kind()) {
        case x86_lds: segregToWrite = x86_segreg_ds; break;
        case x86_les: segregToWrite = x86_segreg_es; break;
        case x86_lfs: segregToWrite = x86_segreg_fs; break;
        case x86_lgs: segregToWrite = x86_segreg_gs; break;
        case x86_lss: segregToWrite = x86_segreg_ss; break;
        default: ROSE_ASSERT (false);
      }
      append(f->makeRegisterWrite(x86_regclass_segment, segregToWrite, x86_regpos_all, buildVarRefExp(segResult)));
      break;
    }

     case x86_mul: {
       ROSE_ASSERT (operands.size() == 1);
       SgVariableSymbol* argSym = cacheValue(makeRead(operands[0]));
       append(f->makeFlagWrite(x86flag_sf, buildBoolValExp(false))); // undefined
       append(f->makeFlagWrite(x86flag_zf, buildBoolValExp(false))); // undefined
       append(f->makeFlagWrite(x86flag_af, buildBoolValExp(false))); // undefined
       append(f->makeFlagWrite(x86flag_pf, buildBoolValExp(false))); // undefined
       switch (operands[0]->get_type()->variantT()) {
         case V_SgAsmTypeByte: {
           SgVariableSymbol* alSym = cacheValue(makeRead(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_low_byte)));
           SgVariableSymbol* resultSym = cacheValue(buildMultiplyOp(buildCastExp(buildVarRefExp(alSym), SgTypeUnsignedInt::createType()), buildVarRefExp(argSym)));
           append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_word), buildVarRefExp(resultSym)));
           append(f->makeFlagWrite(x86flag_cf, buildGreaterOrEqualOp(buildVarRefExp(resultSym), buildUnsignedLongLongIntValHex(0x100))));
           append(f->makeFlagWrite(x86flag_of, buildGreaterOrEqualOp(buildVarRefExp(resultSym), buildUnsignedLongLongIntValHex(0x100))));
           break;
         }
         case V_SgAsmTypeWord: {
           SgVariableSymbol* axSym = cacheValue(makeRead(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_word)));
           SgVariableSymbol* resultSym = cacheValue(buildMultiplyOp(buildCastExp(buildVarRefExp(axSym), SgTypeUnsignedInt::createType()), buildVarRefExp(argSym)));
           append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_word), buildBitAndOp(buildVarRefExp(resultSym), buildUnsignedLongLongIntValHex(0xFFFFU))));
           append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_dx, x86_regpos_word), buildRshiftOp(buildVarRefExp(resultSym), buildIntValHex(16))));
           append(f->makeFlagWrite(x86flag_cf, buildGreaterOrEqualOp(buildVarRefExp(resultSym), buildUnsignedLongLongIntValHex(0x10000))));
           append(f->makeFlagWrite(x86flag_of, buildGreaterOrEqualOp(buildVarRefExp(resultSym), buildUnsignedLongLongIntValHex(0x10000))));
           break;
         }
         case V_SgAsmTypeDoubleWord: {
           SgVariableSymbol* eaxSym = cacheValue(makeRead(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_dword)));
           SgVariableSymbol* resultSym = cacheValue(buildMultiplyOp(buildCastExp(buildVarRefExp(eaxSym), SgTypeUnsignedLongLong::createType()), buildVarRefExp(argSym)));
           append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_dword), buildBitAndOp(buildVarRefExp(resultSym), buildUnsignedLongLongIntValHex(0xFFFFFFFFULL))));
           append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_dx, x86_regpos_dword), buildRshiftOp(buildVarRefExp(resultSym), buildIntValHex(32))));
           append(f->makeFlagWrite(x86flag_cf, buildGreaterOrEqualOp(buildVarRefExp(resultSym), buildUnsignedLongLongIntValHex(0x100000000ULL))));
           append(f->makeFlagWrite(x86flag_of, buildGreaterOrEqualOp(buildVarRefExp(resultSym), buildUnsignedLongLongIntValHex(0x100000000ULL))));
           break;
         }
         case V_SgAsmTypeQuadWord: {
           SgVariableSymbol* raxSym = cacheValue(makeRead(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_qword)));
           SgVariableSymbol* resultLowSym = cacheValue(buildMultiplyOp(buildVarRefExp(raxSym), buildVarRefExp(argSym)));
           SgVariableSymbol* resultHighSym = cacheValue(buildFunctionCallExp(f->getHelperFunction("mulhi64"), buildExprListExp(buildVarRefExp(raxSym), buildVarRefExp(argSym))));
           append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_qword), buildVarRefExp(resultLowSym)));
           append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_dx, x86_regpos_dword), buildVarRefExp(resultHighSym)));
           append(f->makeFlagWrite(x86flag_cf, buildNotEqualOp(buildVarRefExp(resultHighSym), buildIntValHex(0))));
           append(f->makeFlagWrite(x86flag_of, buildNotEqualOp(buildVarRefExp(resultHighSym), buildIntValHex(0))));
           break;
         }
         default: ROSE_ASSERT (false);
       }
       break;
     }

     case x86_imul: {
       append(f->makeFlagWrite(x86flag_sf, buildBoolValExp(false))); // undefined
       append(f->makeFlagWrite(x86flag_zf, buildBoolValExp(false))); // undefined
       append(f->makeFlagWrite(x86flag_af, buildBoolValExp(false))); // undefined
       append(f->makeFlagWrite(x86flag_pf, buildBoolValExp(false))); // undefined
       SgAsmExpression* asmOp1 = NULL;
       SgAsmExpression* asmOp2 = NULL;
       SgAsmExpression* asmResultLow = NULL;
       SgAsmExpression* asmResultHigh = NULL;
       switch (operands.size()) {
         case 1: {
           asmOp2 = operands[0];
           switch (operands[0]->get_type()->variantT()) {
             case V_SgAsmTypeByte:
             asmOp1 = new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_low_byte);
             asmResultLow = new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_low_byte);
             asmResultHigh = new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_high_byte);
             break;

             case V_SgAsmTypeWord:
             asmOp1 = new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_word);
             asmResultLow = new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_word);
             asmResultHigh = new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_dx, x86_regpos_word);
             break;

             case V_SgAsmTypeDoubleWord:
             asmOp1 = new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_dword);
             asmResultLow = new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_dword);
             asmResultHigh = new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_dx, x86_regpos_dword);
             break;

             case V_SgAsmTypeQuadWord:
             asmOp1 = new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_qword);
             asmResultLow = new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_qword);
             asmResultHigh = new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_dx, x86_regpos_qword);
             break;
           }
           break;
         }

         case 2: {
           asmOp1 = operands[0];
           asmOp2 = operands[1];
           asmResultLow = operands[0];
           asmResultHigh = NULL;
           break;
         }

         case 3: {
           asmOp1 = operands[1];
           asmOp2 = operands[2];
           asmResultLow = operands[0];
           asmResultHigh = NULL;
           break;
         }

         default: ROSE_ASSERT (false);
      }
      ROSE_ASSERT (asmOp1);
      ROSE_ASSERT (asmOp2);
      ROSE_ASSERT (asmResultLow); // asmResultHigh may be NULL

      SgVariableSymbol* op1 = cacheValue(makeRead(asmOp1));
      SgVariableSymbol* op2 = cacheValue(makeRead(asmOp2));

      SgExpression* cfAndOfExpr = NULL;
      switch (operands[0]->get_type()->variantT()) {
        case V_SgAsmTypeByte: {
          SgVariableSymbol* resultSym = cacheValue(buildCastExp(buildMultiplyOp(buildCastExp(buildCastExp(buildVarRefExp(op1), SgTypeSignedChar::createType()), SgTypeShort::createType()), buildCastExp(buildVarRefExp(op2), SgTypeSignedChar::createType())), SgTypeUnsignedShort::createType()));
          append(makeWrite(asmResultLow, buildCastExp(buildVarRefExp(resultSym), SgTypeUnsignedChar::createType())));
          if (asmResultHigh) {
            append(makeWrite(asmResultHigh, buildRshiftOp(buildVarRefExp(resultSym), buildIntValHex(8))));
          }
          // || op seems correct, but this is now an unsigned comparison and so && is actually required
          cfAndOfExpr = buildAndOp(buildGreaterThanOp(buildVarRefExp(resultSym), buildUnsignedLongLongIntValHex(0x007F)), buildLessThanOp(buildVarRefExp(resultSym), buildUnsignedLongLongIntValHex(0xFF80)));
          break;
        }
        case V_SgAsmTypeWord: {
          SgVariableSymbol* resultLowSym = cacheValue(buildMultiplyOp(buildVarRefExp(op1), buildVarRefExp(op2)));
          // imulhi16 must return an UNSIGNED result
          SgVariableSymbol* resultHighSym = cacheValue(buildFunctionCallExp(f->getHelperFunction("imulhi16"), buildExprListExp(buildVarRefExp(op1), buildVarRefExp(op2))));
          append(makeWrite(asmResultLow, buildVarRefExp(resultLowSym)));
          if (asmResultHigh) {
            append(makeWrite(asmResultHigh, buildVarRefExp(resultHighSym)));
          }
          cfAndOfExpr = buildNotEqualOp(buildMinusOp(buildVarRefExp(resultHighSym)), buildRshiftOp(buildVarRefExp(resultLowSym), buildIntValHex(15)));
          break;
        }
        case V_SgAsmTypeDoubleWord: {
          SgVariableSymbol* resultLowSym = cacheValue(buildMultiplyOp(buildVarRefExp(op1), buildVarRefExp(op2)));
          // imulhi32 must return an UNSIGNED result
          SgVariableSymbol* resultHighSym = cacheValue(buildFunctionCallExp(f->getHelperFunction("imulhi32"), buildExprListExp(buildVarRefExp(op1), buildVarRefExp(op2))));
          append(makeWrite(asmResultLow, buildVarRefExp(resultLowSym)));
          if (asmResultHigh) {
            append(makeWrite(asmResultHigh, buildVarRefExp(resultHighSym)));
          }
          cfAndOfExpr = buildNotEqualOp(buildMinusOp(buildVarRefExp(resultHighSym)), buildRshiftOp(buildVarRefExp(resultLowSym), buildIntValHex(31)));
          break;
        }
        case V_SgAsmTypeQuadWord: {
          SgVariableSymbol* resultLowSym = cacheValue(buildMultiplyOp(buildVarRefExp(op1), buildVarRefExp(op2)));
          // imulhi64 must return an UNSIGNED result
          SgVariableSymbol* resultHighSym = cacheValue(buildFunctionCallExp(f->getHelperFunction("imulhi64"), buildExprListExp(buildVarRefExp(op1), buildVarRefExp(op2))));
          append(makeWrite(asmResultLow, buildVarRefExp(resultLowSym)));
          if (asmResultHigh) {
            append(makeWrite(asmResultHigh, buildVarRefExp(resultHighSym)));
          }
          cfAndOfExpr = buildNotEqualOp(buildMinusOp(buildVarRefExp(resultHighSym)), buildRshiftOp(buildVarRefExp(resultLowSym), buildIntValHex(63)));
          break;
        }
        default: ROSE_ASSERT (false);
      }
      ROSE_ASSERT (cfAndOfExpr);
      append(f->makeFlagWrite(x86flag_cf, copyExpression(cfAndOfExpr)));
      append(f->makeFlagWrite(x86flag_of, cfAndOfExpr));
      break;
    }

     case x86_div: {
       ROSE_ASSERT (operands.size() == 1);
       SgVariableSymbol* argSym = cacheValue(makeRead(operands[0]));
       append(f->makeFlagWrite(x86flag_sf, buildBoolValExp(false))); // undefined
       append(f->makeFlagWrite(x86flag_zf, buildBoolValExp(false))); // undefined
       append(f->makeFlagWrite(x86flag_af, buildBoolValExp(false))); // undefined
       append(f->makeFlagWrite(x86flag_pf, buildBoolValExp(false))); // undefined
       append(f->makeFlagWrite(x86flag_cf, buildBoolValExp(false))); // undefined
       append(f->makeFlagWrite(x86flag_of, buildBoolValExp(false))); // undefined
       switch (operands[0]->get_type()->variantT()) {
         case V_SgAsmTypeByte: {
           SgVariableSymbol* axSym = cacheValue(makeRead(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_word)));
           append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_low_byte), buildFunctionCallExp(f->getHelperFunction("div8"), buildExprListExp(buildVarRefExp(axSym), buildVarRefExp(argSym)))));
           append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_high_byte), buildFunctionCallExp(f->getHelperFunction("mod8"), buildExprListExp(buildVarRefExp(axSym), buildVarRefExp(argSym)))));
           break;
         }
         case V_SgAsmTypeWord: {
           SgVariableSymbol* axSym = cacheValue(makeRead(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_word)));
           SgVariableSymbol* dxSym = cacheValue(makeRead(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_dx, x86_regpos_word)));
           append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_word), buildFunctionCallExp(f->getHelperFunction("div16"), buildExprListExp(buildVarRefExp(dxSym), buildVarRefExp(axSym), buildVarRefExp(argSym)))));
           append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_dx, x86_regpos_word), buildFunctionCallExp(f->getHelperFunction("mod16"), buildExprListExp(buildVarRefExp(dxSym), buildVarRefExp(axSym), buildVarRefExp(argSym)))));
           break;
         }
         case V_SgAsmTypeDoubleWord: {
           SgVariableSymbol* eaxSym = cacheValue(makeRead(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_dword)));
           SgVariableSymbol* edxSym = cacheValue(makeRead(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_dx, x86_regpos_dword)));
           append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_dword), buildFunctionCallExp(f->getHelperFunction("div32"), buildExprListExp(buildVarRefExp(edxSym), buildVarRefExp(eaxSym), buildVarRefExp(argSym)))));
           append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_dx, x86_regpos_dword), buildFunctionCallExp(f->getHelperFunction("mod32"), buildExprListExp(buildVarRefExp(edxSym), buildVarRefExp(eaxSym), buildVarRefExp(argSym)))));
           break;
         }
         case V_SgAsmTypeQuadWord: {
           SgVariableSymbol* raxSym = cacheValue(makeRead(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_qword)));
           SgVariableSymbol* rdxSym = cacheValue(makeRead(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_dx, x86_regpos_qword)));
           SgVariableSymbol* quotientSym = cacheValue(buildFunctionCallExp(f->getHelperFunction("div64"), buildExprListExp(buildVarRefExp(rdxSym), buildVarRefExp(raxSym), buildVarRefExp(argSym))));
           SgVariableSymbol* remainderSym = cacheValue(buildFunctionCallExp(f->getHelperFunction("mod64"), buildExprListExp(buildVarRefExp(rdxSym), buildVarRefExp(raxSym), buildVarRefExp(argSym))));
           append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_qword), buildVarRefExp(quotientSym)));
           append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_dx, x86_regpos_qword), buildVarRefExp(remainderSym)));
           break;
         }
         default: ROSE_ASSERT (false);
       }
       break;
     }

     case x86_idiv: {
       ROSE_ASSERT (operands.size() == 1);
       SgVariableSymbol* argSym = cacheValue(makeRead(operands[0]));
       append(f->makeFlagWrite(x86flag_sf, buildBoolValExp(false))); // undefined
       append(f->makeFlagWrite(x86flag_zf, buildBoolValExp(false))); // undefined
       append(f->makeFlagWrite(x86flag_af, buildBoolValExp(false))); // undefined
       append(f->makeFlagWrite(x86flag_pf, buildBoolValExp(false))); // undefined
       append(f->makeFlagWrite(x86flag_cf, buildBoolValExp(false))); // undefined
       append(f->makeFlagWrite(x86flag_of, buildBoolValExp(false))); // undefined
       switch (operands[0]->get_type()->variantT()) {
         case V_SgAsmTypeByte: {
           SgVariableSymbol* axSym = cacheValue(makeRead(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_word)));
           append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_low_byte), buildFunctionCallExp(f->getHelperFunction("idiv8"), buildExprListExp(buildVarRefExp(axSym), buildVarRefExp(argSym)))));
           append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_high_byte), buildFunctionCallExp(f->getHelperFunction("imod8"), buildExprListExp(buildVarRefExp(axSym), buildVarRefExp(argSym)))));
           break;
         }
         case V_SgAsmTypeWord: {
           SgVariableSymbol* axSym = cacheValue(makeRead(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_word)));
           SgVariableSymbol* dxSym = cacheValue(makeRead(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_dx, x86_regpos_word)));
           append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_word), buildFunctionCallExp(f->getHelperFunction("idiv16"), buildExprListExp(buildVarRefExp(dxSym), buildVarRefExp(axSym), buildVarRefExp(argSym)))));
           append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_dx, x86_regpos_word), buildFunctionCallExp(f->getHelperFunction("imod16"), buildExprListExp(buildVarRefExp(dxSym), buildVarRefExp(axSym), buildVarRefExp(argSym)))));
           break;
         }
         case V_SgAsmTypeDoubleWord: {
           SgVariableSymbol* eaxSym = cacheValue(makeRead(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_dword)));
           SgVariableSymbol* edxSym = cacheValue(makeRead(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_dx, x86_regpos_dword)));
           append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_dword), buildFunctionCallExp(f->getHelperFunction("idiv32"), buildExprListExp(buildVarRefExp(edxSym), buildVarRefExp(eaxSym), buildVarRefExp(argSym)))));
           append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_dx, x86_regpos_dword), buildFunctionCallExp(f->getHelperFunction("imod32"), buildExprListExp(buildVarRefExp(edxSym), buildVarRefExp(eaxSym), buildVarRefExp(argSym)))));
           break;
         }
         case V_SgAsmTypeQuadWord: {
           SgVariableSymbol* raxSym = cacheValue(makeRead(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_qword)));
           SgVariableSymbol* rdxSym = cacheValue(makeRead(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_dx, x86_regpos_qword)));
           SgVariableSymbol* quotientSym = cacheValue(buildFunctionCallExp(f->getHelperFunction("idiv64"), buildExprListExp(buildVarRefExp(rdxSym), buildVarRefExp(raxSym), buildVarRefExp(argSym))));
           SgVariableSymbol* remainderSym = cacheValue(buildFunctionCallExp(f->getHelperFunction("imod64"), buildExprListExp(buildVarRefExp(rdxSym), buildVarRefExp(raxSym), buildVarRefExp(argSym))));
           append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_ax, x86_regpos_qword), buildVarRefExp(quotientSym)));
           append(makeWrite(new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, x86_gpr_dx, x86_regpos_qword), buildVarRefExp(remainderSym)));
           break;
         }
         default: ROSE_ASSERT (false);
       }
       break;
     }

#define DO_SET(name) \
    case x86_set##name: { \
      ROSE_ASSERT (operands.size() == 1); \
      append(makeWrite(operands[0], buildConditionalExp(f->makeCondition(x86cond_##name), buildIntValHex(1), buildIntValHex(0)))); \
      break; \
    }

    DO_SET(e)
    DO_SET(ne)
    DO_SET(s)
    DO_SET(ns)
    DO_SET(le)
    DO_SET(g)
    DO_SET(l)
    DO_SET(ge)
    DO_SET(a)
    DO_SET(be)
    DO_SET(b)
    DO_SET(ae)
    DO_SET(o)
    DO_SET(no)
    DO_SET(pe)
    DO_SET(po)

#undef DO_SET

#define DO_CMOV(name) \
    case x86_cmov##name: { \
      ROSE_ASSERT (operands.size() == 2); \
      append(buildIfStmt( \
               f->makeCondition(x86cond_##name), \
               buildBasicBlock(makeWrite(operands[0], makeRead(operands[1]))),  \
               buildBasicBlock())); \
      break; \
    }

    DO_CMOV(e)
    DO_CMOV(ne)
    DO_CMOV(s)
    DO_CMOV(ns)
    DO_CMOV(le)
    DO_CMOV(g)
    DO_CMOV(l)
    DO_CMOV(ge)
    DO_CMOV(a)
    DO_CMOV(be)
    DO_CMOV(b)
    DO_CMOV(ae)
    DO_CMOV(o)
    DO_CMOV(no)
    DO_CMOV(pe)
    DO_CMOV(po)

#undef DO_CMOV

    case x86_nop: {
      ROSE_ASSERT (operands.size() == 0);
      break;
    }

    case x86_cld: {
      ROSE_ASSERT (operands.size() == 0);
      append(f->makeFlagWrite(x86flag_df, buildBoolValExp(false)));
      break;
    }

    case x86_std: {
      ROSE_ASSERT (operands.size() == 0);
      append(f->makeFlagWrite(x86flag_df, buildBoolValExp(true)));
      break;
    }

    case x86_hlt:
       {
         ROSE_ASSERT (operands.size() == 0);
         append(buildExprStatement(buildFunctionCallExp(f->getHelperFunction("abort"), buildExprListExp())));
         break;
       }

    case x86_int:
       {
         ROSE_ASSERT (operands.size() == 1);
         append(buildExprStatement(buildFunctionCallExp(f->getHelperFunction("interrupt"), buildExprListExp(makeRead(operands[0])))));
         break;
       }

    default: {
      cerr << "Unhandled x86 instruction kind " << toString(insn->get_kind()) << " in convertInstruction" << endl;

   // Comment out to force unknown instructions to fail.
   // break;
      abort();
    }
  }
  if (jumpToNextInstruction) {
    append(f->makeGotoNextInstruction(insn->get_address(), nextIP));
  }
}

SgFunctionSymbol* X86AssemblyToCWithVariables::addHelperFunction(const std::string& name, SgType* returnType, SgFunctionParameterList* params) {
  SgFunctionDeclaration* decl = buildNondefiningFunctionDeclaration(name, returnType, params, globalScope);
  appendStatement(decl, globalScope);
  SgFunctionSymbol* sym = globalScope->lookup_function_symbol(name);
  ROSE_ASSERT (sym);
  return sym;
}

X86AssemblyToCWithVariables::X86AssemblyToCWithVariables(SgFile* f): globalScope(NULL) {
  ROSE_ASSERT (f);
  ROSE_ASSERT (f->get_globalScope());
  globalScope = f->get_globalScope();
#define LOOKUP_FUNC(name) \
  do {name##Sym = globalScope->lookup_function_symbol(#name); ROSE_ASSERT (name##Sym);} while (0)
  LOOKUP_FUNC(parity);
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
  gprLowByteSym[0] = globalScope->lookup_variable_symbol("al"); ROSE_ASSERT (gprLowByteSym[0]);
  gprLowByteSym[1] = globalScope->lookup_variable_symbol("cl"); ROSE_ASSERT (gprLowByteSym[1]);
  gprLowByteSym[2] = globalScope->lookup_variable_symbol("dl"); ROSE_ASSERT (gprLowByteSym[2]);
  gprLowByteSym[3] = globalScope->lookup_variable_symbol("bl"); ROSE_ASSERT (gprLowByteSym[3]);
  gprLowByteSym[4] = globalScope->lookup_variable_symbol("spl"); ROSE_ASSERT (gprLowByteSym[4]);
  gprLowByteSym[5] = globalScope->lookup_variable_symbol("bpl"); ROSE_ASSERT (gprLowByteSym[5]);
  gprLowByteSym[6] = globalScope->lookup_variable_symbol("sil"); ROSE_ASSERT (gprLowByteSym[6]);
  gprLowByteSym[7] = globalScope->lookup_variable_symbol("dil"); ROSE_ASSERT (gprLowByteSym[7]);
  gprLowByteSym[8] = globalScope->lookup_variable_symbol("r8b"); ROSE_ASSERT (gprLowByteSym[8]);
  gprLowByteSym[9] = globalScope->lookup_variable_symbol("r9b"); ROSE_ASSERT (gprLowByteSym[9]);
  gprLowByteSym[10] = globalScope->lookup_variable_symbol("r10b"); ROSE_ASSERT (gprLowByteSym[10]);
  gprLowByteSym[11] = globalScope->lookup_variable_symbol("r11b"); ROSE_ASSERT (gprLowByteSym[11]);
  gprLowByteSym[12] = globalScope->lookup_variable_symbol("r12b"); ROSE_ASSERT (gprLowByteSym[12]);
  gprLowByteSym[13] = globalScope->lookup_variable_symbol("r13b"); ROSE_ASSERT (gprLowByteSym[13]);
  gprLowByteSym[14] = globalScope->lookup_variable_symbol("r14b"); ROSE_ASSERT (gprLowByteSym[14]);
  gprLowByteSym[15] = globalScope->lookup_variable_symbol("r15b"); ROSE_ASSERT (gprLowByteSym[15]);
  ipSym = globalScope->lookup_variable_symbol("ip"); ROSE_ASSERT (ipSym);
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
  sf_xor_ofSym = globalScope->lookup_variable_symbol("sf_xor_of"); ROSE_ASSERT (sf_xor_ofSym);
  zf_or_cfSym = globalScope->lookup_variable_symbol("zf_or_cf"); ROSE_ASSERT (zf_or_cfSym);
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

SgExpression* X86AssemblyToCWithVariables::makeRegisterRead(X86RegisterClass cl, int num, X86PositionInRegister pos) {
  switch (cl) {
    case x86_regclass_gpr: {
      if (pos == x86_regpos_all) pos = x86_regpos_dword;
      ROSE_ASSERT (pos != x86_regpos_qword);
      switch (pos) {
        case x86_regpos_dword: return buildVarRefExp(gprSym[num]);
        case x86_regpos_word: return buildBitAndOp(buildVarRefExp(gprSym[num]), buildUnsignedIntValHex(0xFFFF));
        case x86_regpos_low_byte: return buildVarRefExp(gprLowByteSym[num]);
        case x86_regpos_high_byte: return buildBitAndOp(buildRshiftOp(buildVarRefExp(gprSym[num]), buildIntVal(8)), buildUnsignedIntValHex(0xFF));
        default: ROSE_ASSERT (false);
      }
    }
    case x86_regclass_segment: {
      return buildIntVal(0); // FIXME
    }
    default: ROSE_ASSERT (!"Bad reg class");
  }
}

SgStatement* X86AssemblyToCWithVariables::makeRegisterWrite(X86RegisterClass cl, int num, X86PositionInRegister pos, SgExpression* value) {
  SgBasicBlock* bb = buildBasicBlock();
  switch (cl) {
    case x86_regclass_gpr: {
      if (pos == x86_regpos_all) pos = x86_regpos_dword;
      ROSE_ASSERT (pos != x86_regpos_qword);
      SgExpression* rawRef = buildVarRefExp(gprSym[num]);
      switch (pos) {
        case x86_regpos_dword: appendStatement(buildAssignStatement(rawRef, value), bb); appendStatement(buildAssignStatement(buildVarRefExp(gprLowByteSym[num]), buildBitAndOp(copyExpression(value), buildUnsignedIntValHex(0xFFU))), bb); break;
        case x86_regpos_word: appendStatement(buildAssignStatement(rawRef, buildBitOrOp(buildBitAndOp(copyExpression(rawRef), buildUnsignedIntValHex(0xFFFF0000U)), buildBitAndOp(value, buildUnsignedIntValHex(0xFFFFU)))), bb); appendStatement(buildAssignStatement(buildVarRefExp(gprLowByteSym[num]), buildBitAndOp(copyExpression(value), buildUnsignedIntValHex(0xFFU))), bb); break;
        case x86_regpos_low_byte: appendStatement(buildAssignStatement(rawRef, buildBitOrOp(buildBitAndOp(copyExpression(rawRef), buildUnsignedIntValHex(0xFFFFFF00U)), buildBitAndOp(value, buildUnsignedIntValHex(0xFFU)))), bb); appendStatement(buildAssignStatement(buildVarRefExp(gprLowByteSym[num]), copyExpression(value)), bb); break;
        case x86_regpos_high_byte: appendStatement(buildAssignStatement(rawRef, buildBitOrOp(buildBitAndOp(copyExpression(rawRef), buildUnsignedIntValHex(0xFFFF00FFU)), buildLshiftOp(buildBitAndOp(value, buildUnsignedIntValHex(0xFFU)), buildIntVal(8)))), bb); /* No change to low byte */ break;
        default: ROSE_ASSERT (false);
      }
      return bb;
    }
    case x86_regclass_segment: {
      return bb; // FIXME
    }
    default: ROSE_ASSERT (!"Bad reg class");
  }
}

SgExpression* X86AssemblyToCWithVariables::makeMemoryRead(X86SegmentRegister segreg, SgExpression* address, SgAsmType* ty) {
  SgFunctionSymbol* mrSym = NULL;
  switch (ty->variantT()) {
    case V_SgAsmTypeByte: mrSym = memoryReadByteSym; break;
    case V_SgAsmTypeWord: mrSym = memoryReadWordSym; break;
    case V_SgAsmTypeDoubleWord: mrSym = memoryReadDWordSym; break;
    case V_SgAsmTypeQuadWord: mrSym = memoryReadQWordSym; break;
    default: ROSE_ASSERT (false);
  }
  ROSE_ASSERT (mrSym);
  return buildFunctionCallExp(mrSym, buildExprListExp(address));
}

SgStatement* X86AssemblyToCWithVariables::makeMemoryWrite(X86SegmentRegister segreg, SgExpression* address, SgExpression* data, SgAsmType* ty) {
  SgFunctionSymbol* mwSym = NULL;
  switch (ty->variantT()) {
    case V_SgAsmTypeByte: mwSym = memoryWriteByteSym; break;
    case V_SgAsmTypeWord: mwSym = memoryWriteWordSym; break;
    case V_SgAsmTypeDoubleWord: mwSym = memoryWriteDWordSym; break;
    case V_SgAsmTypeQuadWord: mwSym = memoryWriteQWordSym; break;
    default: ROSE_ASSERT (false);
  }
  ROSE_ASSERT (mwSym);
  return buildExprStatement(buildFunctionCallExp(mwSym, buildExprListExp(address, data)));
}

SgExpression* X86AssemblyToCWithVariables::makeFlagRead(X86Flag flag) {
  SgVariableSymbol* fl = flagsSym[flag];
  ROSE_ASSERT (fl);
  return buildVarRefExp(fl);
}

SgExpression* X86AssemblyToCWithVariables::makeCondition(X86Condition cond) {
  switch (cond) {
    case x86cond_o: return makeFlagRead(x86flag_of);
    case x86cond_no: return buildNotOp(makeFlagRead(x86flag_of));
    case x86cond_b: return makeFlagRead(x86flag_cf);
    case x86cond_ae: return buildNotOp(makeFlagRead(x86flag_cf));
    case x86cond_e: return makeFlagRead(x86flag_zf);
    case x86cond_ne: return buildNotOp(makeFlagRead(x86flag_zf));
    case x86cond_be: return buildVarRefExp(zf_or_cfSym);
    case x86cond_a: return buildNotOp(buildVarRefExp(zf_or_cfSym));
    case x86cond_s: return makeFlagRead(x86flag_sf);
    case x86cond_ns: return buildNotOp(makeFlagRead(x86flag_sf));
    case x86cond_pe: return makeFlagRead(x86flag_pf);
    case x86cond_po: return buildNotOp(makeFlagRead(x86flag_pf));
    case x86cond_l: return buildVarRefExp(sf_xor_ofSym);
    case x86cond_ge: return buildNotOp(buildVarRefExp(sf_xor_ofSym));
    case x86cond_le: return buildOrOp(makeCondition(x86cond_l), makeCondition(x86cond_e));
    case x86cond_g: return buildAndOp(makeCondition(x86cond_ge), makeCondition(x86cond_ne));
    default: ROSE_ASSERT (false);
  }
}

SgStatement* X86AssemblyToCWithVariables::makeFlagWrite(X86Flag flag, SgExpression* value) {
  SgVariableSymbol* fl = flagsSym[flag];
  ROSE_ASSERT (fl);
  SgBasicBlock* bb = buildBasicBlock();
  appendStatement(buildAssignStatement(buildVarRefExp(fl), value), bb);
  if (flag == x86flag_of && isSgNotEqualOp(value) && isSgVarRefExp(isSgNotEqualOp(value)->get_lhs_operand()) && isSgVarRefExp(isSgNotEqualOp(value)->get_lhs_operand())->get_symbol()->get_declaration() == flagsSym[x86flag_sf]->get_declaration()) {
    // This is a hack to get simpler expressions for signed comparisons
    appendStatement(buildAssignStatement(buildVarRefExp(sf_xor_ofSym), copyExpression(isSgNotEqualOp(value)->get_rhs_operand())), bb);
  } else if (flag == x86flag_sf || flag == x86flag_of) {
    appendStatement(buildAssignStatement(buildVarRefExp(sf_xor_ofSym), buildNotEqualOp(makeFlagRead(x86flag_sf), makeFlagRead(x86flag_of))), bb);
  // } else if (flag == x86flag_cf && isSgLessThanOp(value)) { FIXME: this is broken now
    // // This is a hack to get simpler expressions for unsigned comparisons
    // appendStatement(buildAssignStatement(buildVarRefExp(zf_or_cfSym), buildLessOrEqualOp(copyExpression(isSgLessThanOp(value)->get_lhs_operand()), copyExpression(isSgLessThanOp(value)->get_rhs_operand()))), bb);
  } else if (flag == x86flag_zf || flag == x86flag_cf) {
    appendStatement(buildAssignStatement(buildVarRefExp(zf_or_cfSym), buildOrOp(makeFlagRead(x86flag_zf), makeFlagRead(x86flag_cf))), bb);
  }
  return bb;
}

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
  return buildBasicBlock(
           buildAssignStatement(buildVarRefExp(ipSym), newAddr),
           buildContinueStmt());
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

SgBasicBlock* X86AssemblyToCWithVariables::makeAllCode(SgAsmFile* f, SgBasicBlock* appendTo) {
  SgBasicBlock* body = appendTo;
  switchBody = buildBasicBlock();
  SgSwitchStatement* sw = buildSwitchStatement(buildVarRefExp(ipSym), switchBody);
  appendStatement(
    buildAssignStatement(
      buildVarRefExp(ipSym),
      buildUnsignedLongLongIntValHex(
        f->get_associated_entry_point())),
    body);
  appendStatement(
    buildAssignStatement(
      buildVarRefExp(flagsSym[x86flag_df]),
      buildBoolValExp(false)),
    body);
  whileBody = buildBasicBlock();
  appendStatement(
    buildWhileStmt(
      buildBoolValExp(true),
      whileBody),
    body);
  appendStatement(
    buildExprStatement(
      buildFunctionCallExp(
        startingInstructionSym,
        buildExprListExp())),
    whileBody);
  appendStatement(sw, whileBody);
  vector<SgNode*> asmBlocks = NodeQuery::querySubTree(f, V_SgAsmBlock);
  for (size_t i = 0; i < asmBlocks.size(); ++i) {
    SgAsmBlock* bb = isSgAsmBlock(asmBlocks[i]);
    blocks.insert(std::make_pair(bb->get_address(), bb));
    SgLabelStatement* ls = buildLabelStatement("label_" + StringUtility::intToHex(bb->get_address()), buildBasicBlock(), whileBody);
    labelsForBlocks.insert(std::make_pair(bb->get_address(), ls));
    appendStatement(ls, whileBody);
    if (bb->get_externallyVisible()) {
      appendStatement(
        buildCaseOptionStmt(
          buildUnsignedLongLongIntValHex(bb->get_address()),
          buildBasicBlock(buildGotoStatement(ls))),
        switchBody);
      externallyVisibleBlocks.insert(bb->get_address());
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
  for (size_t i = 0; i < asmBlocks.size(); ++i) {
    SgAsmBlock* bb = isSgAsmBlock(asmBlocks[i]);
    const SgAsmStatementPtrList& stmts = bb->get_statementList();
    ROSE_ASSERT (!stmts.empty());
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
    flattenBlocks(caseBody);
    insertStatementAfter(labelsForBlocks[addr], caseBody);
  }
  return body;
}
