#include <rose.h>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <interp_core.h>
#include "interp_smt.h"

#include <smtlib.h>

using namespace std;
using namespace Interp;
using namespace smtlib;
using namespace smtlib::QF_BV;

// TODO: find a better place for this function
static bool isUnsignedType(SgType *t)
   {
     switch (t->variantT())
        {
          case V_SgTypeUnsignedChar:
          case V_SgTypeUnsignedInt:
          case V_SgTypeUnsignedLongLong:
          case V_SgTypeUnsignedLong:
          case V_SgTypeUnsignedShort:
             return true;
          default:
             return false;
        }
   }

namespace Interp {
namespace smtBV {

Bits bvSgTypeBits(SgType *t)
   {
     switch (t->variantT())
        {
          case V_SgTypeBool: return bvSgTypeTraits<SgTypeBool>::bits;
          case V_SgTypeChar: return bvSgTypeTraits<SgTypeChar>::bits;
          case V_SgEnumType: return bvSgTypeTraits<SgEnumType>::bits;
          case V_SgTypeInt: return bvSgTypeTraits<SgTypeInt>::bits;
          case V_SgTypeLong: return bvSgTypeTraits<SgTypeLong>::bits;
          case V_SgTypeLongLong: return bvSgTypeTraits<SgTypeLongLong>::bits;
          case V_SgTypeShort: return bvSgTypeTraits<SgTypeShort>::bits;
          case V_SgTypeUnsignedChar: return bvSgTypeTraits<SgTypeUnsignedChar>::bits;
          case V_SgTypeUnsignedInt: return bvSgTypeTraits<SgTypeUnsignedInt>::bits;
          case V_SgTypeUnsignedLongLong: return bvSgTypeTraits<SgTypeUnsignedLongLong>::bits;
          case V_SgTypeUnsignedLong: return bvSgTypeTraits<SgTypeUnsignedLong>::bits;
          case V_SgTypeUnsignedShort: return bvSgTypeTraits<SgTypeUnsignedShort>::bits;
          default:
            throw InterpError("bvSgTypeBits: unrecognised type " + t->class_name());
        }
   }

bvbaseP BVValue::getBV() const
   {
     return v;
   }

bvbaseP BVValue::getBV(const_ValueP val, SgType *apt)
   {
     const BVValue *rhsBV = dynamic_cast<const BVValue *>(val->prim().get());
     if (rhsBV)
        {
          return rhsBV->v;
        }
     else
        {
          SgType *st = apt->stripTypedefsAndModifiers();
          switch (st->variantT())
             {
               case V_SgTypeChar:
                  return bvbaseP(new bvconst(uint8_t(val->prim()->getConcreteValueChar())));
               /*
               case V_SgTypeDouble:
               case V_SgTypeFloat:
               case V_SgTypeLongDouble:
                  return mpl->fromFloat(val->prim()->getConcreteValueDouble());
               case V_SgTypeBool:
               case V_SgTypeChar:
               case V_SgTypeInt:
               case V_SgTypeLong:
               case V_SgTypeLongLong:
               case V_SgTypeShort:
               case V_SgTypeUnsignedChar:
               case V_SgTypeUnsignedInt:
               case V_SgTypeUnsignedLongLong:
               case V_SgTypeUnsignedLong:
               case V_SgTypeUnsignedShort:
                  return mpl->fromInteger64(val->prim()->getConcreteValueLongLong());
                  */
               default:
                  throw InterpError("Deriving an algebraic value from a concrete value - cannot recognise type " + st->class_name());
             }
        }
   }

ValueP BVValue::evalBinOp(bvbinop_kind kind, const_ValueP rhs, SgType *lhsApt, SgType *rhsApt, bool isShift) const
   {
     if (!isValid || !rhs->valid()) return ValueP(new BVValue(getBits(), PTemp, owner));
     bvbaseP rhsBV = getBV(rhs, rhsApt);
     if (isShift) rhsBV = mkbvcast(zero_extend, v->bits(), rhsBV);
     return ValueP(new BVValue(mkbvbinop(kind, v, rhsBV), PTemp, owner));
   }

ValueP BVValue::evalUnOp(bvunop_kind kind, SgType *apt) const
   {
     if (!isValid) return ValueP(new BVValue(bvBits, PTemp, owner));
     return ValueP(new BVValue(mkbvunop(kind, v), PTemp, owner));
   }

ValueP BVValue::evalBinPred(bvbinpred_kind kind, const_ValueP rhs, SgType *lhsApt, SgType *rhsApt, bool negate) const
   {
     if (!isValid || !rhs->valid()) return ValueP(new BVValue(getBits(), PTemp, owner));
     bvbaseP rhsBV = getBV(rhs, rhsApt);
     predbaseP resultPred = mkbvbinpred(kind, v, rhsBV);
     if (negate) resultPred = mkprednot(resultPred);
     bvbaseP resultBV = mkbvite(resultPred,
                                bvbaseP(new bvconst(true)),
                                bvbaseP(new bvconst(false)));
     SgFile::outputLanguageOption_enum lang = owner->get_language();
     if (lang == SgFile::e_C_output_language)
          resultBV = mkbvextend(zero_extend, bvTypeTraits<int>::bits, resultBV);
     return ValueP(new BVValue(resultBV, PTemp, owner));
   }

BVValue::BVValue(bvbaseP v, Position pos, StackFrameP owner) : BasePrimValue(pos, owner, true), v(v)
{
  if (!v)
     {
       throw InterpError("A BVValue was created with a NULL argument");
     }
}

string BVValue::show() const
   {
     if (isValid)
          return v->show();
     else
          return "<<undefined>>";
   }

ValueP BVValue::primAssign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt)
   {
     if (rhs->valid())
        {
          isValid = true;
          v = getBV(rhs, rhsApt);
        }
     else
        {
          if (isValid)
               bvBits = v->bits();
          isValid = false;
        }
     return shared_from_this();
   }

size_t BVValue::forwardValidity() const { return 1; }

ValueP BVValue::evalAddOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const
   {
     return evalBinOp(bvadd, rhs, lhsApt, rhsApt);
   }

ValueP BVValue::evalSubtractOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const
   {
     return evalBinOp(bvsub, rhs, lhsApt, rhsApt);
   }

ValueP BVValue::evalMultiplyOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const
   {
     return evalBinOp(bvmul, rhs, lhsApt, rhsApt);
   }

ValueP BVValue::evalDivideOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const
   {
     return evalBinOp(isUnsignedType(lhsApt) ? bvudiv : bvsdiv, rhs, lhsApt, rhsApt);
   }

ValueP BVValue::evalModOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const
   {
     return evalBinOp(isUnsignedType(lhsApt) ? bvurem : bvsrem, rhs, lhsApt, rhsApt);
   }

ValueP BVValue::evalBitAndOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const
   {
     return evalBinOp(bvand, rhs, lhsApt, rhsApt);
   }

ValueP BVValue::evalBitOrOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const
   {
     return evalBinOp(bvor, rhs, lhsApt, rhsApt);
   }

ValueP BVValue::evalBitXorOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const
   {
     return evalBinOp(bvxor, rhs, lhsApt, rhsApt);
   }

ValueP BVValue::evalLshiftOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const
   {
     return evalBinOp(bvshl, rhs, lhsApt, rhsApt, true);
   }

ValueP BVValue::evalRshiftOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const
   {
     return evalBinOp(isUnsignedType(lhsApt) ? bvlshr : bvashr, rhs, lhsApt, rhsApt, true);
   }

ValueP BVValue::evalLessThanOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const
   {
     return evalBinPred(isUnsignedType(lhsApt) ? bvult : bvslt, rhs, lhsApt, rhsApt);
   }

ValueP BVValue::evalGreaterThanOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const
   {
     return evalBinPred(isUnsignedType(lhsApt) ? bvugt : bvsgt, rhs, lhsApt, rhsApt);
   }

ValueP BVValue::evalLessOrEqualOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const
   {
     return evalBinPred(isUnsignedType(lhsApt) ? bvule : bvsle, rhs, lhsApt, rhsApt);
   }

ValueP BVValue::evalGreaterOrEqualOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const
   {
     return evalBinPred(isUnsignedType(lhsApt) ? bvuge : bvsge, rhs, lhsApt, rhsApt);
   }

ValueP BVValue::evalEqualityOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const
   {
     return evalBinPred(bveq, rhs, lhsApt, rhsApt);
   }

ValueP BVValue::evalNotEqualOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const
   {
     return evalBinPred(bveq, rhs, lhsApt, rhsApt, true);
   }

ValueP BVValue::evalMinusOp(SgType *apt) const
   {
     return evalUnOp(bvneg, apt);
   }

ValueP BVValue::evalBitComplementOp(SgType *apt) const
   {
     return evalUnOp(bvnot, apt);
   }

bool BVValue::getConcreteValueBool() const
   {
     return getConcreteValue<bool>();
   }

char BVValue::getConcreteValueChar() const
   {
     return getConcreteValue<char>();
   }

double BVValue::getConcreteValueDouble() const
   {
     return getConcreteValue<double>();
   }

float BVValue::getConcreteValueFloat() const
   {
     return getConcreteValue<float>();
   }

int BVValue::getConcreteValueInt() const
   {
     return getConcreteValue<int>();
   }

long double BVValue::getConcreteValueLongDouble() const
   {
     return getConcreteValue<long double>();
   }

long int BVValue::getConcreteValueLong() const
   {
     return getConcreteValue<long int>();
   }

long long int BVValue::getConcreteValueLongLong() const
   {
     return getConcreteValue<long long int>();
   }

short BVValue::getConcreteValueShort() const
   {
     return getConcreteValue<short>();
   }

unsigned char BVValue::getConcreteValueUnsignedChar() const
   {
     return getConcreteValue<unsigned char>();
   }

unsigned int BVValue::getConcreteValueUnsignedInt() const
   {
     return getConcreteValue<unsigned int>();
   }

unsigned long long int BVValue::getConcreteValueUnsignedLongLong() const
   {
     return getConcreteValue<unsigned long long int>();
   }

unsigned long BVValue::getConcreteValueUnsignedLong() const
   {
     return getConcreteValue<unsigned long>();
   }

unsigned short BVValue::getConcreteValueUnsignedShort() const
   {
     return getConcreteValue<unsigned short>();
   }

ValueP BVValue::evalCastExp(ValueP fromVal, SgType *fromType, SgType *toType)
   {
     if (fromVal->valid())
        {
          toType = toType->stripTypedefsAndModifiers();
          bvbaseP fromBV = getBV(fromVal, fromType);
          v = mkbvcast(isUnsignedType(toType) ? zero_extend : sign_extend, bvSgTypeBits(toType), fromBV);
          isValid = true;
        }
     return shared_from_this();
   }

ValueP BVValue::evalPrefixPlusPlusOp(SgType *apt)
   {
     bvbaseP one (new bvconst(v->bits(), 1)), newBV;
     if (v->bits() == Bits1)
          newBV = one;
     else
          newBV = mkbvbinop(bvadd, v, one);
     ValueP newVal (new BVValue(newBV, PTemp, owner));
     assign(newVal, apt, apt);
     return shared_from_this();
   }

ValueP BVValue::evalPrefixMinusMinusOp(SgType *apt)
   {
     bvbaseP one (new bvconst(v->bits(), 1));
     bvbaseP newBV = mkbvbinop(bvsub, v, one);
     ValueP newVal (new BVValue(newBV, PTemp, owner));
     assign(newVal, apt, apt);
     return shared_from_this();
   }

Bits BVValue::getBits() const
   {
     if (isValid)
        {
          return v->bits();
        }
     else
        {
          return bvBits;
        }
   }

SgType *BVValue::defaultType() const
   {
     switch (getBits())
        {
          // TODO: make these types platform-independent
          case Bits1: return SgTypeBool::createType();
          case Bits8: return SgTypeUnsignedChar::createType();
          case Bits16: return SgTypeUnsignedShort::createType();
          case Bits32: return SgTypeUnsignedInt::createType();
          case Bits64: return SgTypeUnsignedLongLong::createType();
          default: throw InterpError("BVValue::defaultType: unexpected case");
        }
   }

string AssertFunctionValue::functionName() const { return "assert"; }

ValueP AssertFunctionValue::call(SgFunctionType *fnType, const vector<ValueP> &args) const
   {
     SMTInterpretation *interp = static_cast<SMTInterpretation *>(owner->interp());
     bool trace = interp->trace;
     ValueP arg = args[0]->prim();
     BVValue *bvArg = dynamic_cast<BVValue *>(arg.get());
     if (bvArg == NULL)
        {
          throw InterpError("Unable to assert a non-BVValue");
        }
     bvbaseP bv = bvArg->getBV();
     bvbaseP bvZero (new bvconst(bv->bits(), 0));
     predbaseP bvEqZero = mkbvbinpred(bveq, bv, bvZero);
     if (bvEqZero->isconst())
        {
          if (bvEqZero->getconst())
             {
               throw InterpError("Assertion failed! (statically)");
             }
          else
             {
               if (trace)
                    cout << "Assertion succeeded (statically)" << endl;
             }
          return ValueP();
        }

#if 0
     if (trace)
          cout << "About to call solver on " << bvEqZero->show() << endl;
#endif
     // solver_smtlib solver("/export/tmp.collingbourne2/binsrc/beaver-1.1-rc1-x86_64/bin/beaver -m --model-file=/dev/stdout");
     solver_smtlib solver(interp->smtSolver);
     solveresult sr = solver.solve(bvEqZero, interp->keepSolverInput);
     if (sr.kind == unsat)
        {
          if (trace)
               cout << "Assertion succeeded (proposition unsatisfiable)" << endl;
        }
     else if (sr.kind == sat)
        {
          throw InterpError("Assertion failed! (proposition satisfiable).  Model follows:\n" + sr.model);
        }
     return ValueP();
   }

string __mkbvvarFnValue::functionName() const { return "__mkbvvar"; }

ValueP __mkbvvarFnValue::call(SgFunctionType *fnType, const vector<ValueP> &args) const
   {
     ValueP valP = args[0], nameP = args[1];
     ValueP valPrim = valP->dereference()->prim();
     ValueP nameArr = nameP->dereference();

     BVValue *bvValPrim = dynamic_cast<BVValue *>(valPrim.get());
     if (bvValPrim == NULL)
        {
          throw InterpError("This type of value does not support holding a bvvar");
        }

     string name = valueToString(nameArr);
     bvvarP newVar (new bvvar(bvValPrim->getBits(), name));
     bvbaseP varName (new bvname(newVar));
     ValueP varVal (new BVValue(varName, PTemp, owner));
     bvValPrim->assign(varVal, bvValPrim->defaultType(), bvValPrim->defaultType());
     return ValueP();
   }

ValueP SMTStackFrame::newValue(SgType *t, Position pos, Context ctx)
   {
     t = t->stripTypedefsAndModifiers();
     switch (t->variantT())
        {
          case V_SgTypeBool:
          case V_SgTypeChar:
          case V_SgEnumType:
          case V_SgTypeInt:
          case V_SgTypeLong:
          case V_SgTypeLongLong:
          case V_SgTypeShort:
          case V_SgTypeUnsignedChar:
          case V_SgTypeUnsignedInt:
          case V_SgTypeUnsignedLongLong:
          case V_SgTypeUnsignedLong:
          case V_SgTypeUnsignedShort: return ValueP(new BVValue(bvSgTypeBits(t), pos, shared_from_this()));
          default: return StackFrame::newValue(t, pos, ctx);
        }
   }

StackFrameP SMTStackFrame::newStackFrame(SgFunctionSymbol *funSym, ValueP thisBinding)
   {
     return StackFrameP(new SMTStackFrame(static_cast<SMTInterpretation *>(interp()), funSym, thisBinding));
   }

ValueP SMTStackFrame::evalExpr(SgExpression *expr, bool arrPtrConv)
   {
     switch (expr->variantT())
        {
          case V_SgBoolValExp: return evalIntSymPrimExpr<SgBoolValExp>(expr);
          case V_SgCharVal: return evalIntSymPrimExpr<SgCharVal>(expr);
          case V_SgEnumVal: return evalIntSymPrimExpr<SgEnumVal>(expr);
          case V_SgIntVal: return evalIntSymPrimExpr<SgIntVal>(expr);
          case V_SgLongIntVal: return evalIntSymPrimExpr<SgLongIntVal>(expr);
          case V_SgLongLongIntVal: return evalIntSymPrimExpr<SgLongLongIntVal>(expr);
          case V_SgShortVal: return evalIntSymPrimExpr<SgShortVal>(expr);
          case V_SgUnsignedCharVal: return evalIntSymPrimExpr<SgUnsignedCharVal>(expr);
          case V_SgUnsignedIntVal: return evalIntSymPrimExpr<SgUnsignedIntVal>(expr);
          case V_SgUnsignedLongLongIntVal: return evalIntSymPrimExpr<SgUnsignedLongLongIntVal>(expr);
          case V_SgUnsignedLongVal: return evalIntSymPrimExpr<SgUnsignedLongVal>(expr);
          case V_SgUnsignedShortVal: return evalIntSymPrimExpr<SgUnsignedShortVal>(expr);
          default: return StackFrame::evalExpr(expr, arrPtrConv);
        }
   }

void SMTInterpretation::parseCommandLine(vector<string> &args)
   {
     Interpretation::parseCommandLine(args);
     CommandlineProcessing::isOptionWithParameter(args, "-interp:", "smtSolver", smtSolver, true);
     keepSolverInput = CommandlineProcessing::isOption(args, "-interp:", "keepSolverInput", true);
   }

void SMTInterpretation::prePrimAssign(ValueP lhs, const_ValueP rhs, SgType *lhsApt, SgType *rhsApt)
   {
     ValueP lhsPrim = lhs->prim();
     if (cpStack.size() > 0)
        {
          cpFrame_t &oldValues = cpStack.back();
          cpFrame_t::iterator oldValueI = oldValues.find(lhsPrim);
          if (oldValueI == oldValues.end())
             {
               ValueP oldValue = lhsPrim->owner->newValue(lhsApt, PTemp);
               oldValue->primAssign(lhs, lhsApt, lhsApt);
               oldValues[lhsPrim] = pair<SgType *, ValueP>(lhsApt, oldValue);
             }
        }
   }

/* The purpose of the ConditionalTransaction is to encapsulate all aspects of a conditional
   evaluation.  Namely, it is responsible for saving the state of the "true" and "false"
   branches and computing the meet of the two branches using the SMT conditional
   expression ite.
 
   To use: evaluate the two branches in between beginTrueBranch()/endTrueBranch() and
   beginFalseBranch()/endFalseBranch() (both branches are optional).  Then call commit()
   to substitute the meet of the two branches.
 */
class ConditionalTransaction
   {

     StackFrameP sf;
     SMTInterpretation::cpFrame_t trueValues, falseValues;

     predbaseP pred;

     enum status_t {
          ST_NoBranchActive,
          ST_FalseBranchActive,
          ST_TrueBranchActive,
          ST_Committed,
     } status;

     SMTInterpretation *interp()
        {
          return static_cast<SMTInterpretation *>(sf->interp());
        }

     void beginBranch(status_t newStatus)
        {
          if (status != ST_NoBranchActive)
             {
               throw InterpError("Branch was begun while a branch was already active!");
             }
          interp()->cpStack.push_back(SMTInterpretation::cpFrame_t());
          status = newStatus;
        }

     void rollback(status_t expectStatus, SMTInterpretation::cpFrame_t &toFrame)
        {
          if (status != expectStatus)
             {
               throw InterpError("Branch was ended while no branch was active, or the wrong branch was active");
             }
          SMTInterpretation::cpFrame_t &curFrame = interp()->cpStack.back();
          for (SMTInterpretation::cpFrame_t::iterator i = curFrame.begin(); i != curFrame.end(); ++i)
             {
               ValueP newVal = sf->newValue(i->second.first, PTemp);
               newVal->primAssign(i->first, i->second.first, i->second.first);
               toFrame[i->first] = pair<SgType *, ValueP>(i->second.first, newVal);
               i->first->primAssign(i->second.second, i->second.first, i->second.first);
             }
          interp()->cpStack.pop_back();
          status = ST_NoBranchActive;
        }

     public:
     ConditionalTransaction(StackFrameP sf, predbaseP pred) : sf(sf), pred(pred), status(ST_NoBranchActive) {}

     void commit()
        {
       // foreach val in trueValues U falseValues: set val to (ite pred trueValue falseValue)
          if (status != ST_NoBranchActive)
             {
               throw InterpError("Commit was called twice or while a branch was already active!");
             }
          for (SMTInterpretation::cpFrame_t::iterator ti = trueValues.begin(), fi = falseValues.begin(); ti != trueValues.end() || fi != falseValues.end();)
             {
               ValueP val, tv, fv;
               SgType *t;
               if (ti != trueValues.end() && (fi == falseValues.end() || ti->first < fi->first))
                  {
                    val = ti->first;
                    t = ti->second.first;
                    tv = ti->second.second;
                    fv = val;
                    ++ti;
                  }
               else if (ti != trueValues.end() && fi != falseValues.end() && ti->first == fi->first)
                  {
                    val = ti->first;
                    t = ti->second.first;
                    tv = ti->second.second;
                    fv = fi->second.second;
                    ++ti; ++fi;
                  }
               else if (fi != falseValues.end() && (ti == trueValues.end() || fi->first < ti->first))
                  {
                    val = fi->first;
                    t = fi->second.first;
                    tv = val;
                    fv = fi->second.second;
                    ++fi;
                  }
               else
                  {
                    throw InterpError("Something weird happened in SymIfBlockStackFrame::commit");
                  }
               BVValue *tvBVV = dynamic_cast<BVValue *>(tv.get());
               BVValue *fvBVV = dynamic_cast<BVValue *>(fv.get());
               if (tvBVV == NULL || fvBVV == NULL)
                  {
                    throw InterpError("Unable to meet non-integral values (such as pointers)");
                  }
               bvbaseP tvBV = tvBVV->getBV(), fvBV = fvBVV->getBV();
#if 0
               cout << "pred = " << pred->show() << endl;
               cout << "tvBV = " << tvBV->show() << endl;
               cout << "fvBV = " << fvBV->show() << endl;
#endif
               ValueP newVal (new BVValue(mkbvite(pred, tvBV, fvBV), PTemp, val->owner));
               val->primAssign(newVal, t, t);
             }
          status = ST_Committed;
        }

     void beginTrueBranch()
        {
          beginBranch(ST_TrueBranchActive);
        }

     void beginFalseBranch()
        {
          beginBranch(ST_FalseBranchActive);
        }

     void endTrueBranch()
        {
          rollback(ST_TrueBranchActive, trueValues);
        }

     void endFalseBranch()
        {
          rollback(ST_FalseBranchActive, falseValues);
        }

     ~ConditionalTransaction()
        {
          if (status != ST_Committed)
             {
               cerr << "An active ConditionalTransaction was destroyed!" << endl;
             }
        }

   };

struct SMTStackFrame::SymIfBlockStackFrame : BlockStackFrame
   {
     SymIfBlockStackFrame(BlockStackFrameP up, StackFrameP sf, SgIfStmt *scope) : BlockStackFrame(up, sf, scope), doneFalseBody(false) {}

     SMTInterpretation::cpFrame_t trueValues, falseValues;
     bool doneFalseBody;
     boost::shared_ptr<ConditionalTransaction> condTrans;

     void setPred(predbaseP pred)
        {
          condTrans = boost::shared_ptr<ConditionalTransaction>(new ConditionalTransaction(sf, pred));
        }

     SgStatement *next()
        {
          if (condTrans)
             {
               if (!doneFalseBody)
                  {
                    condTrans->endTrueBranch();
                    SgIfStmt *ifStmt = static_cast<SgIfStmt *>(scope);
                    if (ifStmt->get_false_body())
                       {
                         condTrans->beginFalseBranch();
                         doneFalseBody = true;
                         return ifStmt->get_false_body();
                       }
                    else
                       {
                         condTrans->commit();
                         return NULL;
                       }
                  }
               else
                  {
                    condTrans->endFalseBranch();
                    condTrans->commit();
                    return NULL;
                  }
             }
          else
             {
               return NULL;
             }
        }

   };

void SMTStackFrame::evalIfStmt(SgIfStmt *ifStmt, BlockStackFrameP &curFrame)
   {
     SymIfBlockStackFrame *ifFrame = new SymIfBlockStackFrame(curFrame, shared_from_this(), ifStmt);
     curFrame = BlockStackFrameP(ifFrame);
     ValueP cond = evalStmtAsBool(ifStmt->get_conditional(), curFrame->scopeVars);
     ValueP condPrim = cond->prim();
     if (BVValue *bvVal = dynamic_cast<BVValue *>(condPrim.get()))
        {
          bvbaseP bv = bvVal->getBV();
          bvbaseP bvZero (new bvconst(bv->bits(), 0));
          predbaseP bvEqZero = mkbvbinpred(bveq, bv, bvZero);
          predbaseP bvNeZero = mkprednot(bvEqZero);
          if (bvNeZero->isconst())
             {
               if (bvNeZero->getconst())
                  {
                    evalStmt(ifStmt->get_true_body(), curFrame);
                  }
               else if (ifStmt->get_false_body())
                  {
                    evalStmt(ifStmt->get_false_body(), curFrame);
                  }
             }
          else
             {
               ifFrame->setPred(bvNeZero);
               ifFrame->condTrans->beginTrueBranch();
               evalStmt(ifStmt->get_true_body(), curFrame);
             }
        }
     else
        {
          if (cond->getConcreteValueInt())
             {
               evalStmt(ifStmt->get_true_body(), curFrame);
             }
          else if (ifStmt->get_false_body())
             {
               evalStmt(ifStmt->get_false_body(), curFrame);
             }
        }
   }

ValueP SMTStackFrame::evalShortCircuitExp(SgExpression *condExp, SgExpression *trueExp, SgExpression *falseExp)
   {
     ValueP cond = evalExpr(condExp);
     ValueP condPrim = cond->prim();
     if (BVValue *bvVal = dynamic_cast<BVValue *>(condPrim.get()))
        {
          bvbaseP bv = bvVal->getBV();
          bvbaseP bvZero (new bvconst(bv->bits(), 0));
          predbaseP bvEqZero = mkbvbinpred(bveq, bv, bvZero);
          predbaseP bvNeZero = mkprednot(bvEqZero);
          if (bvNeZero->isconst())
             {
               if (bvNeZero->getconst())
                  {
                    return trueExp ? evalExpr(trueExp) : cond;
                  }
               else
                  {
                    return falseExp ? evalExpr(falseExp) : cond;
                  }
             }
          else
             {
               ConditionalTransaction ct(shared_from_this(), bvNeZero);
               SgType *resType = trueExp ? trueExp->get_type() : falseExp->get_type();
               ValueP result = newValue(resType, PTemp);
               ct.beginTrueBranch();
               ValueP trueExpVal;
               if (trueExp)
                  {
                    trueExpVal = evalExpr(trueExp);
                  }
               else
                  {
                    bvbaseP bvResOne (new bvconst(bvSgTypeBits(resType), 1));
                    trueExpVal = ValueP(new BVValue(bvResOne, PTemp, shared_from_this()));
                  }
               result->assign(trueExpVal, resType, resType);
               ct.endTrueBranch();
               ct.beginFalseBranch();
               ValueP falseExpVal;
               if (falseExp)
                  {
                    falseExpVal = evalExpr(falseExp);
                  }
               else
                  {
                    bvbaseP bvResZero (new bvconst(bvSgTypeBits(resType), 0));
                    falseExpVal = ValueP(new BVValue(bvResZero, PTemp, shared_from_this()));
                  }
               result->assign(falseExpVal, resType, resType);
               ct.endFalseBranch();
               ct.commit();
               return result;
             }
        }
     else
        {
          if (condPrim->getConcreteValueInt() != 0)
             {
               return trueExp ? evalExpr(trueExp) : cond;
             }
          else
             {
               return falseExp ? evalExpr(falseExp) : cond;
             }
        }
   }

ValueP SMTStackFrame::evalConditionalExp(SgConditionalExp *condExp)
   {
     return evalShortCircuitExp(condExp->get_conditional_exp(), condExp->get_true_exp(), condExp->get_false_exp());
   }

ValueP SMTStackFrame::evalAndOp(SgExpression *lhs, SgExpression *rhs)
   {
     return evalShortCircuitExp(lhs, rhs, NULL);
   }

ValueP SMTStackFrame::evalOrOp(SgExpression *lhs, SgExpression *rhs)
   {
     return evalShortCircuitExp(lhs, NULL, rhs);
   }

void SMTInterpretation::registerBuiltinFns(builtins_t &builtins) const
   {
     Interpretation::registerBuiltinFns(builtins);
     builtins["::__mkbvvar"] = ValueP(new __mkbvvarFnValue(PGlob, StackFrameP()));
     builtins["::assert"] = ValueP(new AssertFunctionValue(PGlob, StackFrameP()));
   }

/*
int main(int argc, char **argv)
   {
     SgProject *prj = frontend(argc, argv);
     SgSourceFile *file = isSgSourceFile((*prj)[0]);
     ROSE_ASSERT(file != NULL);
     SgGlobal *global = file->get_globalScope();
     SgFunctionSymbol *testSym = global->lookup_function_symbol("test");

     Interpretation interp;
     StackFrameP head(new SMTStackFrame(&interp, testSym));
     ValueP x (new BVValue(bvbaseP(new bvname(Bits32, "x")), PTemp, head));
     ValueP rv = head->interpFunction(vector<ValueP>(1, x));
     cout << "Returned " << (rv.get() ? rv->show() : "<<nothing>>") << endl;
   }
*/

}
}
