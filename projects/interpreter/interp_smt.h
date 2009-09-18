#ifndef _INTERP_SMT_H
#define _INTERP_SMT_H

#include <sstream>

#include <interp_core.h>
#include <smtlib.h>

namespace Interp {
namespace smtBV {

template <typename SgTypeT>
struct bvSgTypeTraits {};

template <>
struct bvSgTypeTraits<SgTypeBool> : smtlib::QF_BV::bvTypeTraits<bool> {};

template <>
struct bvSgTypeTraits<SgTypeChar> : smtlib::QF_BV::bvTypeTraits<char> {};

template <>
struct bvSgTypeTraits<SgTypeInt> : smtlib::QF_BV::bvTypeTraits<int> {};

template <>
struct bvSgTypeTraits<SgTypeLong> : smtlib::QF_BV::bvTypeTraits<long> {};

template <>
struct bvSgTypeTraits<SgTypeLongLong> : smtlib::QF_BV::bvTypeTraits<long long> {};

template <>
struct bvSgTypeTraits<SgTypeShort> : smtlib::QF_BV::bvTypeTraits<short> {};

template <>
struct bvSgTypeTraits<SgTypeUnsignedChar> : smtlib::QF_BV::bvTypeTraits<unsigned char> {};

template <>
struct bvSgTypeTraits<SgTypeUnsignedInt> : smtlib::QF_BV::bvTypeTraits<unsigned int> {};

template <>
struct bvSgTypeTraits<SgTypeUnsignedLongLong> : smtlib::QF_BV::bvTypeTraits<unsigned long long> {};

template <>
struct bvSgTypeTraits<SgTypeUnsignedLong> : smtlib::QF_BV::bvTypeTraits<unsigned long> {};

template <>
struct bvSgTypeTraits<SgTypeUnsignedShort> : smtlib::QF_BV::bvTypeTraits<unsigned short> {};

template <typename SgValueT>
struct bvSgValueTraits : smtlib::QF_BV::bvTypeTraits<void, sizeof(((SgValueT *)NULL)->get_value())> {};

template <>
struct bvSgValueTraits<SgBoolValExp> : smtlib::QF_BV::bvTypeTraits<bool> {};

smtlib::QF_BV::Bits bvSgTypeBits(SgType *t);

class BVValue : public Value
   {
     friend class AssertFunctionValue;

     smtlib::QF_BV::bvbaseP v;

     static smtlib::QF_BV::bvbaseP getBV(const_ValueP val, SgType *apt);

     ValueP evalBinOp(smtlib::QF_BV::bvbinop_kind kind, const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const;
     ValueP evalBinPred(smtlib::QF_BV::bvbinpred_kind kind, const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const;

     public:
     BVValue(Position pos, StackFrameP owner) : Value(pos, owner, false) {}
     BVValue(smtlib::QF_BV::bvbaseP v, Position pos, StackFrameP owner);

     std::string show() const;

     ValueP primAssign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt);
     size_t forwardValidity() const;

     ValueP evalAddOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const;
     ValueP evalSubtractOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const;
     ValueP evalMultiplyOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const;
     ValueP evalDivideOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const;
     ValueP evalLessThanOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const;
     ValueP evalGreaterThanOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const;
     ValueP evalLessOrEqualOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const;
     ValueP evalGreaterOrEqualOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const;

     template <typename intT>
     intT getConcreteValue() const
        {
          if (!valid)
             {
               throw InterpError("Attempt to access an undefined value");
             }
          switch (v->bits())
             {
               case smtlib::QF_BV::Bits1: return intT(v->const1());
               case smtlib::QF_BV::Bits8: return intT(v->const8());
               case smtlib::QF_BV::Bits16: return intT(v->const16());
               case smtlib::QF_BV::Bits32: return intT(v->const32());
               case smtlib::QF_BV::Bits64: return intT(v->const64());
               default:
                 std::stringstream ss;
                 ss << "Unrecognised bitcount " << v->bits();
                 throw InterpError(ss.str());
             }
        }

     bool getConcreteValueBool() const;
     char getConcreteValueChar() const;
     double getConcreteValueDouble() const;
     float getConcreteValueFloat() const;
     int getConcreteValueInt() const;
     long double getConcreteValueLongDouble() const;
     long int getConcreteValueLong() const;
     long long int getConcreteValueLongLong() const;
     short getConcreteValueShort() const;
     unsigned char getConcreteValueUnsignedChar() const;
     unsigned int getConcreteValueUnsignedInt() const;
     unsigned long long int getConcreteValueUnsignedLongLong() const;
     unsigned long getConcreteValueUnsignedLong() const;
     unsigned short getConcreteValueUnsignedShort() const;

     ValueP evalCastExp(ValueP fromVal, SgType *fromType, SgType *toType);

   };

class AssertFunctionValue : public Value
   {
     public:
     AssertFunctionValue(Position pos, StackFrameP owner) : Value(pos, owner, true) {}

     std::string show() const;

     ValueP call(SgFunctionType *fnType, const std::vector<ValueP> &args) const;
     ValueP primAssign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt);
     size_t forwardValidity() const;

   };

class SMTInterpretation : public Interpretation
     {

       public:
       std::string smtSolver;
       bool keepSolverInput;
       void parseCommandLine(std::vector<std::string> &args);

     };

class SMTStackFrame : public StackFrame
   {
     public:

          SMTStackFrame(SMTInterpretation *currentInterp, SgFunctionSymbol *funSym, ValueP thisBinding = ValueP()) : StackFrame(currentInterp, funSym, thisBinding) {}

          ValueP newValue(SgType *t, Position pos, bool isParam);

          template <class SgValExprT>
          ValueP evalIntSymPrimExpr(SgExpression *expr)
             {
               typedef bvSgValueTraits<SgValExprT> valT;
               SgValExprT *pe = dynamic_cast<SgValExprT *>(expr);
               smtlib::QF_BV::bvbaseP bvConst (new smtlib::QF_BV::bvconst(typename valT::canon_type(pe->get_value())));
               return ValueP(new BVValue(bvConst, PTemp, shared_from_this()));
             }

          StackFrameP newStackFrame(SgFunctionSymbol *funSym, ValueP thisBinding);

          ValueP evalExpr(SgExpression *expr);

          ValueP evalFunctionRefExp(SgFunctionSymbol *sym);

   };

}
}

#endif
