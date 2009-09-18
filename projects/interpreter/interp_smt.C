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

ValueP BVValue::evalBinOp(bvbinop_kind kind, const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const
   {
     if (!valid || !rhs->valid) return ValueP(new BVValue(PTemp, owner));
     bvbaseP rhsBV = getBV(rhs, rhsApt);
     return ValueP(new BVValue(mkbvbinop(kind, v, rhsBV), PTemp, owner));
   }

ValueP BVValue::evalBinPred(bvbinpred_kind kind, const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const
   {
     if (!valid || !rhs->valid) return ValueP(new BVValue(PTemp, owner));
     bvbaseP rhsBV = getBV(rhs, rhsApt);
     predbaseP resultPred = mkbvbinpred(kind, v, rhsBV);
     bvbaseP resultBV = mkpred2bv(resultPred);
     SgFile::outputLanguageOption_enum lang = owner->get_language();
     if (lang == SgFile::e_C_output_language)
          resultBV = mkbvextend(zero_extend, bvTypeTraits<int>::bits, resultBV);
     return ValueP(new BVValue(resultBV, PTemp, owner));
   }

BVValue::BVValue(bvbaseP v, Position pos, StackFrameP owner) : Value(pos, owner, true), v(v)
{
  if (!v)
     {
       throw InterpError("A BVValue was created with a NULL argument");
     }
}

string BVValue::show() const
   {
     if (valid)
          return v->show();
     else
          return "<<undefined>>";
   }

ValueP BVValue::primAssign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt)
   {
     if (rhs->valid)
        {
          valid = true;
          v = getBV(rhs, rhsApt);
        }
     else
        {
          valid = false;
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
     if (fromVal->valid)
        {
          toType = toType->stripTypedefsAndModifiers();
          bvbaseP fromBV = getBV(fromVal, fromType);
          v = mkbvcast(isUnsignedType(toType) ? zero_extend : sign_extend, bvSgTypeBits(toType), fromBV);
          valid = true;
        }
     return shared_from_this();
   }

string AssertFunctionValue::show() const { return "<<built-in function assert>>"; }

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
     bvbaseP bv = bvArg->v;
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

     if (trace)
          cout << "About to call solver on " << bvEqZero->show() << endl;
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
          throw InterpError("Assertion failed! (proposition satisfiable).  Model follows:" + sr.model);
        }
     return ValueP();
   }

ValueP AssertFunctionValue::primAssign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt)
   {
     throw InterpError("Cannot assign to a function!");
   }

size_t AssertFunctionValue::forwardValidity() const { return 1; }

ValueP SMTStackFrame::newValue(SgType *t, Position pos, bool isParam)
   {
     t = t->stripTypedefsAndModifiers();
     switch (t->variantT())
        {
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
          case V_SgTypeUnsignedShort: return ValueP(new BVValue(pos, shared_from_this()));
          default: return StackFrame::newValue(t, pos, isParam);
        }
   }

StackFrameP SMTStackFrame::newStackFrame(SgFunctionSymbol *funSym, ValueP thisBinding)
   {
     return StackFrameP(new SMTStackFrame(static_cast<SMTInterpretation *>(interp()), funSym, thisBinding));
   }

ValueP SMTStackFrame::evalExpr(SgExpression *expr)
   {
     switch (expr->variantT())
        {
          case V_SgBoolValExp: return evalIntSymPrimExpr<SgBoolValExp>(expr);
          case V_SgCharVal: return evalIntSymPrimExpr<SgCharVal>(expr);
          case V_SgIntVal: return evalIntSymPrimExpr<SgIntVal>(expr);
          case V_SgLongIntVal: return evalIntSymPrimExpr<SgLongIntVal>(expr);
          case V_SgLongLongIntVal: return evalIntSymPrimExpr<SgLongLongIntVal>(expr);
          case V_SgShortVal: return evalIntSymPrimExpr<SgShortVal>(expr);
          case V_SgUnsignedCharVal: return evalIntSymPrimExpr<SgUnsignedCharVal>(expr);
          case V_SgUnsignedIntVal: return evalIntSymPrimExpr<SgUnsignedIntVal>(expr);
          case V_SgUnsignedLongLongIntVal: return evalIntSymPrimExpr<SgUnsignedLongLongIntVal>(expr);
          case V_SgUnsignedLongVal: return evalIntSymPrimExpr<SgUnsignedLongVal>(expr);
          case V_SgUnsignedShortVal: return evalIntSymPrimExpr<SgUnsignedShortVal>(expr);
          default: return StackFrame::evalExpr(expr);
        }
   }

ValueP SMTStackFrame::evalFunctionRefExp(SgFunctionSymbol *sym)
   {
     if (sym->get_name() == "assert")
          return ValueP(new AssertFunctionValue(PTemp, shared_from_this()));

     return StackFrame::evalFunctionRefExp(sym);
   }

void SMTInterpretation::parseCommandLine(vector<string> &args)
   {
     Interpretation::parseCommandLine(args);
     CommandlineProcessing::isOptionWithParameter(args, "-interp:", "smtSolver", smtSolver, true);
     keepSolverInput = CommandlineProcessing::isOption(args, "-interp:", "keepSolverInput", true);
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
