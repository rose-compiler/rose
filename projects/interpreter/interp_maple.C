#include <rose.h>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <interp_core.h>
#include <interp_maple.h>

#include <maple++.h>

using namespace std;
using namespace Interp;
using namespace MaplePP;

namespace Interp {
namespace maple {

Maple *mpl = NULL;

Algeb AlgebValue::getAlgeb(const_ValueP val, SgType *apt)
   {
     const AlgebValue *rhsAlg = dynamic_cast<const AlgebValue *>(val->prim().get());
     if (rhsAlg)
        {
          return rhsAlg->v;
        }
     else
        {
          SgType *st = apt->stripTypedefsAndModifiers();
          switch (st->variantT())
             {
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
               default:
                  throw InterpError("Deriving an algebraic value from a concrete value - cannot recognise type " + st->class_name());
             }
        }
   }

AlgebValue::AlgebValue(Algeb v, Position pos, StackFrameP owner) : BasePrimValue(pos, owner, true), v(v)
{
  if (!v)
     {
       throw InterpError("An AlgebValue was created with a NULL argument");
     }
}

string AlgebValue::show() const
   {
     if (isValid)
          return v.show();
     else
          return "<<undefined>>";
   }

ValueP AlgebValue::primAssign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt)
   {
     if (rhs->valid())
        {
          isValid = true;
          v = getAlgeb(rhs, rhsApt);
        }
     else
        {
          isValid = false;
        }
     return shared_from_this();
   }

size_t AlgebValue::forwardValidity() const { return 1; }

#define DEFINE_ALGEB_BINOP_IMPL(op,opname,opassignname) \
ValueP AlgebValue::eval##opname(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const \
   { \
     Algeb result = mpl->binOp(v, #op, getAlgeb(rhs, rhsApt)); \
     return ValueP(new AlgebValue(result, PTemp, owner)); \
   }

FOREACH_BINARY_PRIMOP(DEFINE_ALGEB_BINOP_IMPL)

ValueP AlgebValue::evalBitAndOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const
   {
     vector<Algeb> args;
     args.push_back(v);
     args.push_back(getAlgeb(rhs, rhsApt));
     Algeb result = mpl->toFunction(mpl->evalStatement("Bits:-And:"), args);
     return ValueP(new AlgebValue(result, PTemp, owner));
   }

ValueP SymStackFrame::newValue(SgType *t, Position pos, Context ctx)
   {
     t = t->stripTypedefsAndModifiers();
     switch (t->variantT())
        {
          case V_SgTypeBool:
          case V_SgTypeChar:
          case V_SgTypeDouble:
          case V_SgTypeFloat:
          case V_SgTypeInt:
          case V_SgTypeLongDouble:
          case V_SgTypeLong:
          case V_SgTypeLongLong:
          case V_SgTypeShort:
          case V_SgTypeUnsignedChar:
          case V_SgTypeUnsignedInt:
          case V_SgTypeUnsignedLongLong:
          case V_SgTypeUnsignedLong:
          case V_SgTypeUnsignedShort: return ValueP(new AlgebValue(pos, shared_from_this()));
          default: return StackFrame::newValue(t, pos, ctx);
        }
   }

StackFrameP SymStackFrame::newStackFrame(SgFunctionSymbol *funSym, ValueP thisBinding)
   {
     return StackFrameP(new SymStackFrame(interp(), funSym, thisBinding));
   }

ValueP SymStackFrame::evalExpr(SgExpression *expr, bool arrPtrConv)
   {
     switch (expr->variantT())
        {
          case V_SgBoolValExp: return evalIntSymPrimExpr<SgBoolValExp>(expr);
          case V_SgCharVal: return evalIntSymPrimExpr<SgCharVal>(expr);
          case V_SgDoubleVal: return evalFloatSymPrimExpr<SgDoubleVal>(expr);
          case V_SgFloatVal: return evalFloatSymPrimExpr<SgFloatVal>(expr);
          case V_SgIntVal: return evalIntSymPrimExpr<SgIntVal>(expr);
          case V_SgLongDoubleVal: return evalFloatSymPrimExpr<SgLongDoubleVal>(expr);
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

/*
int main(int argc, char **argv)
   {
     try
        {
          Maple maple = vector<string>();
          mpl = &maple;

          SgProject *prj = frontend(argc, argv);
          SgSourceFile *file = isSgSourceFile((*prj)[0]);
          ROSE_ASSERT(file != NULL);
          SgGlobal *global = file->get_globalScope();
          SgFunctionSymbol *testSym = global->lookup_function_symbol("test");

          Interpretation interp;
          StackFrameP head(new SymStackFrame(&interp, testSym));
          ValueP x (new AlgebValue(mpl->mkName("x", false), PTemp, head));
       // ValueP x (new AlgebValue(mpl->fromInteger64(1), PTemp, head));
          ValueP rv = head->interpFunction(vector<ValueP>(1, x));
          cout << "Returned " << (rv.get() ? rv->show() : "<<nothing>>") << endl;
          maple.evalStatement("gc():");
        }
     catch (MapleError &me)
        {
          cerr << "Caught a Maple error: " << me.err << endl;
          return 1;
        }
   }
*/

}
}
