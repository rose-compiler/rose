#include <interp_core.h>
#include <maple++.h>

namespace Interp {
namespace maple {

extern MaplePP::Maple *mpl;

class AlgebValue : public BasePrimValue
   {
     MaplePP::Algeb v;

     static MaplePP::Algeb getAlgeb(const_ValueP val, SgType *apt);

     public:
     AlgebValue(Position pos, StackFrameP owner) : BasePrimValue(pos, owner, false) {}
     AlgebValue(MaplePP::Algeb v, Position pos, StackFrameP owner);

     std::string show() const;

     ValueP primAssign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt);

     size_t forwardValidity() const;

#define DECLARE_ALGEB_BINOP_IMPL(op,opname,opassignname) \
     ValueP eval##opname(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const;

     FOREACH_BINARY_PRIMOP(DECLARE_ALGEB_BINOP_IMPL)

#undef DECLARE_ALGEB_BINOP_IMPL

     ValueP evalBitAndOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const;

   };

class SymStackFrame : public StackFrame
   {
     public:

          SymStackFrame(Interpretation *currentInterp, SgFunctionSymbol *funSym, ValueP thisBinding = ValueP()) : StackFrame(currentInterp, funSym, thisBinding) {}

          ValueP newValue(SgType *t, Position pos, Context ctx);

          template <class SgValExprT>
          ValueP evalIntSymPrimExpr(SgExpression *expr)
             {
               SgValExprT *pe = dynamic_cast<SgValExprT *>(expr);
               return ValueP(new AlgebValue(mpl->fromInteger64(pe->get_value()), PTemp, shared_from_this()));
             }

          template <class SgValExprT>
          ValueP evalFloatSymPrimExpr(SgExpression *expr)
             {
               SgValExprT *pe = dynamic_cast<SgValExprT *>(expr);
               return ValueP(new AlgebValue(mpl->fromFloat(pe->get_value()), PTemp, shared_from_this()));
             }

          StackFrameP newStackFrame(SgFunctionSymbol *funSym, ValueP thisBinding);
          ValueP evalExpr(SgExpression *expr, bool arrPtrConv = true);

   };

}
}
