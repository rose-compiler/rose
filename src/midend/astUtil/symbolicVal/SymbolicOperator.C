#include "Matrix.h"
#include "SymbolicExpr.h"
#include "SymbolicPlus.h"
#include "SymbolicMultiply.h"
#include "SymbolicSelect.h"
#include "CommandOptions.h"
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

#define COMPARE_MAX  10
static int comparetime = 0;

CompareRel CompareValHelp(const SymbolicVal &v1, const SymbolicVal &v2,
                      MapObject<SymbolicVal,SymbolicBound>* f);

bool DebugOp()
{
  static int r = 0;
  if ( r == 0) {
    if (CmdOptions::GetInstance()->HasOption("-debugvalop"))
        r = 1;
     else
        r = -1;
  }
  return r == 1;
}


SymbolicVal ApplyBinOP( SymOpType t, const SymbolicVal &v1,
                               const SymbolicVal &v2)
{
  SymbolicVal r;
  switch (t) {
  case SYMOP_PLUS:
     {
      PlusApplicator op;
      r = ApplyBinOP(op, v1, v2);
      if (DebugOp())
         std::cerr << v1.toString() << " + " << v2.toString() << " = " << r.toString() << std::endl;
      return r;
     }
  case SYMOP_MULTIPLY:
    {
      MultiplyApplicator op;
      r = ApplyBinOP(op, v1, v2);
      if (DebugOp())
         std::cerr << v1.toString() << " * " << v2.toString() << " = " << r.toString() << std::endl;
      return r;
    }
  case SYMOP_MIN:
      r = Min(v1,v2);
      if (DebugOp())
         std::cerr << "Min( " << v1.toString() << " , " << v2.toString() << ") = " << r.toString() << std::endl;
      return r;
  case SYMOP_MAX: return Max(v1, v2);
      r = Max(v1,v2);
      if (DebugOp())
         std::cerr << "Max( " << v1.toString() << " , " << v2.toString() << ") = " << r.toString() << std::endl;
      return r;
  case SYMOP_POW:
     {
      int val2;
      int vu1, vd1;
      if (!v2.isConstInt(val2))
         assert(false);
      if (v1 == 1 || val2 == 1)
         r =  v1;
      else if (val2 == -1 && v1.isConstInt(vu1, vd1))
         r = new SymbolicConst(vd1, vu1);
      else
         r = new SymbolicPow(v1, val2);
      if (DebugOp())
         std::cerr << "Pow( " << v1.toString() << " , " << v2.toString() << ") = " << r.toString() << std::endl;
      return r;
     }
  default:
    assert(false);
  }

// DQ (12/10/2016): Added return statement, but because this was not specified, we want to make it an error to preserve the previous semantics.
// DQ (12/11/2016): It appears that printf() is not declared here on some versions of Linux (even RH).
   std::cerr << "Exiting at a previously undefined function return location. " << std::endl;
   assert(false);

// DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=return-type.
   return r;
}

SymbolicVal operator * (const SymbolicVal &v1, const SymbolicVal &v2)
         {  return ApplyBinOP(SYMOP_MULTIPLY, v1, v2); }
SymbolicVal operator / (const SymbolicVal &v1, const SymbolicVal &v2)
{
  SymbolicVal vd = ApplyBinOP(SYMOP_POW, v2, -1);
  SymbolicVal r = ApplyBinOP(SYMOP_MULTIPLY, v1, vd);
  return r;
}

int CountGT( CompareRel r)
  { return (r == REL_GT)? 1 : 0; }
int CountGE( CompareRel r)
  { return (r == REL_GE || r == REL_GT || r == REL_EQ)? 1 : 0; }
int CountLT( CompareRel r)
  { return (r == REL_LT)? 1 : 0; }
int CountLE( CompareRel r)
  { return (r == REL_LE || r == REL_LT || r == REL_EQ)? 1 : 0; }
int CountEQ( CompareRel r)
  { return (r == REL_EQ)? 1 : 0; }

bool DebugCompareVal()
{
  static int r = 0;
  if ( r == 0) {
    if (CmdOptions::GetInstance()->HasOption("-debugcompareval"))
        r = 1;
     else
        r = -1;
  }
  return r == 1;
}

SymbolicVal Max( const SymbolicVal &v1, const SymbolicVal &v2,
                       MapObject<SymbolicVal, SymbolicBound>* f)
         {
            if (v1.IsNIL())
              return v2;
            if (v2.IsNIL())
              return v1;
            switch (CompareVal(v1,v2,f)) {
            case REL_NONE:
            case REL_UNKNOWN:
            case REL_NE:
               {
               SelectApplicator maxOp(1);
               return ApplyBinOP(maxOp,v1,v2);
              }
           case REL_EQ:
           case REL_LT:
           case REL_LE:
               return v2;
           case REL_GT:
           case REL_GE:
               return v1;
           default:
              assert(0);
           }
        }

SymbolicVal Min( const SymbolicVal &v1, const SymbolicVal &v2,
                       MapObject<SymbolicVal, SymbolicBound>* f)
         { if (v1.IsNIL())
              return v2;
           if (v2.IsNIL())
              return v1;
            switch (CompareVal(v1,v2,f)) {
            case REL_NONE:
            case REL_UNKNOWN:
            case REL_NE:
               {
               SelectApplicator minOp(-1);
               return ApplyBinOP(minOp,v1,v2);
               }
           case REL_EQ:
           case REL_LT:
           case REL_LE:
               return v1;
           case REL_GT:
           case REL_GE:
               return v2;
           default:
              assert(0);
           }
         }

class EQOperator : public SymbolicVisitor
{
 protected:
  bool result;
  virtual void VisitConst( const SymbolicConst &v) { result = false; }
  virtual void VisitVar( const SymbolicVar &v) { result = false; }
  virtual void VisitFunction( const SymbolicFunction &v) { result = false; }
  virtual void VisitExpr( const  SymbolicExpr& v) { result = false; }
  virtual void VisitAstWrap( const SymbolicAstWrap& v) { result = false; }
};

class ConstEQ : public EQOperator
{
  const SymbolicConst &c;
  void VisitConst( const SymbolicConst &v) { result = (c == v); }
 public:
  ConstEQ( const SymbolicConst &v) : c(v) {}
  bool operator ()(SymbolicVal v)
       { result = false; v.Visit(this); return result; }
};

class AstWrapEQ : public EQOperator
{
  const SymbolicAstWrap &c;
  void VisitAstWrap( const SymbolicAstWrap &v) { result = (c == v); }
 public:
  AstWrapEQ( const SymbolicAstWrap &v) : c(v) {}
  bool operator ()(SymbolicVal v)
       { result = false; v.Visit(this); return result; }
};

class VarEQ : public EQOperator
{
  const SymbolicVar &var;
  void VisitVar( const SymbolicVar &v) { result = (var == v); }
 public:
  VarEQ( const SymbolicVar &v) : var(v) {}
  bool operator ()(SymbolicVal v)
       { result = false; v.Visit(this); return result; }
};

class FunctionEQ : public EQOperator
{
  const SymbolicFunction &var;
  void VisitFunction( const SymbolicFunction &v) { result = (var == v); }
 public:
  FunctionEQ( const SymbolicFunction &v) : var(v) {}
  bool operator ()(SymbolicVal v)
       { result = false; v.Visit(this); return result; }
};

class ExprEQ : public EQOperator
{
  const SymbolicExpr &exp;
  void VisitExpr( const SymbolicExpr &v)
      { result = (exp == v); }
 public:
  ExprEQ( const SymbolicExpr &v) : exp(v) {}
  bool operator ()(SymbolicVal v)
       { result = false; v.Visit(this); return result; }
};

class ValEQ : public EQOperator
{
  SymbolicVal v2;
  void VisitConst( const SymbolicConst &v)  { result = ConstEQ(v)(v2); }
  void VisitVar( const SymbolicVar &v) { result = VarEQ(v)(v2); }
  void VisitFunction( const SymbolicFunction &v) { result = FunctionEQ(v)(v2); }
  void VisitExpr( const  SymbolicExpr& v) { result = ExprEQ(v)(v2); }
  void VisitAstWrap( const SymbolicAstWrap& v) { result = AstWrapEQ(v)(v2); }
 public:
  bool operator()(const SymbolicVal &_v1, const SymbolicVal &_v2)
   { result = false; v2 = _v2; _v1.Visit(this); return result; }
};

bool Equal(const SymbolicVal &v1, const SymbolicVal& v2)
     { return v1.IsSame(v2)? true : ValEQ()(v1,v2); }

class CompareOperator : public SymbolicVisitor
{
  MapObject<SymbolicVal,SymbolicBound>* func;
 protected:
  MapObject<SymbolicVal,SymbolicBound>* GetFunc() { return func; }
  CompareRel result;
  virtual void VisitConst( const SymbolicConst &v) { result = REL_UNKNOWN; }
  virtual void VisitVar( const SymbolicVar &v) { result = REL_UNKNOWN; }
  virtual void VisitFunction( const SymbolicFunction &v) { result = REL_UNKNOWN; }
  virtual void VisitExpr( const  SymbolicExpr& v) { result = REL_UNKNOWN; }
  void VisitAstWrap( const SymbolicAstWrap& v) { result = REL_UNKNOWN; }

  void Default1( const SymbolicVal &v1, const SymbolicVal &v2)
  {
    if (v1.IsSame(v2))
      result = REL_EQ;
    else if (v1.IsNIL() || v2.IsNIL())
       result = REL_UNKNOWN;
    else if (v1 == v2)
       result = REL_EQ;
    else
       result = REL_UNKNOWN;
  }

   void Default0( const SymbolicVal &v1, const SymbolicVal &v2)
   {
     Default1(v1,v2);
     if (DebugCompareVal())
         std::cerr << " in CompareOperator::Default \n";
     if (result == REL_UNKNOWN) {
       int tmp = comparetime;
       SymbolicVal diff = v1 - v2;
       comparetime = tmp;
       if (diff.GetValType() == VAL_CONST) {
          int diffval = atoi( diff.toString().c_str());
          if (diffval  < 0)
              result = REL_LT;
          else if (diffval > 0)
              result = REL_GT;
          else
              result = REL_EQ;
      }
      else if (func != 0) {
         int tmp = comparetime;
         SymbolicBound b1 = GetValBound(v1,*func), b2 = GetValBound(v2,*func);
         comparetime = tmp;
         CompareRel ge1 = (b1.lb != v1)? CompareValHelp(b1.lb,v2,func) : REL_UNKNOWN;
         CompareRel le2 = (b2.ub != v2)? CompareValHelp(b2.ub,v1,func) : REL_UNKNOWN;
         CompareRel le1 = (b1.ub != v1)? CompareValHelp(b1.ub,v2,func) : REL_UNKNOWN;
         CompareRel ge2 = (b2.lb != v2)? CompareValHelp(b2.lb,v1,func) : REL_UNKNOWN;
         if (CountGT(ge1) || CountLT(le2))
              result = REL_GT;
         else if (CountGE(ge1) || CountLE(le2))
              result = REL_GE;
         else if (CountLT(le1) || CountGT(ge2))
              result = REL_LT;
         else if (CountLE(le1) || CountGE(ge2))
              result = REL_LE;
     }
    }
   }

  CompareOperator(MapObject<SymbolicVal,SymbolicBound>* _func = 0) : func(_func) {}
};

void ExprTermCompare( const SymbolicExpr &e1, const SymbolicVal& v2,
                       Matrix<CompareRel> &result,
                       MapObject<SymbolicVal,SymbolicBound>* f)
{
   if (DebugCompareVal())
       std::cerr << " in ExprTermCompare1 \n";
   int i = 0;
   for (SymbolicExpr::OpdIterator p1 = e1.GetOpdIterator();
        !p1.ReachEnd(); ++p1,++i) {
      SymbolicVal v1 = e1.Term2Val(p1.Current());
      int j = 0;
      result(i,j) = CompareValHelp(v1, v2, f);
   }
}
void ExprTermCompare( const SymbolicExpr &e1, const SymbolicExpr &e2,
                       Matrix<CompareRel> &result,
                       MapObject<SymbolicVal,SymbolicBound>* f)
{
   if (DebugCompareVal())
       std::cerr << " in ExprTermCompare2 \n";
   int i = 0;
   for (SymbolicExpr::OpdIterator p1 = e1.GetOpdIterator();
        !p1.ReachEnd(); ++p1,++i) {
      SymbolicVal v1 = e1.Term2Val(p1.Current());
      int j = 0;
      for (SymbolicExpr::OpdIterator p2 = e2.GetOpdIterator();
              !p2.ReachEnd(); ++p2,++j) {
         result(i,j) = CompareValHelp(v1, e2.Term2Val(p2.Current()),f);
      }
   }
}

unsigned CountSrcRel( Matrix<CompareRel> r, int (*Count)(CompareRel r))
   {
      unsigned c1 = 0;
      for (size_t i1 = 0; i1 < r.rows(); ++i1)  {
         for ( size_t i2 = 0; i2 < r.cols(); ++i2) {
            if ( Count( r(i1,i2) ) ) {
               ++c1;
               break;
            }
         }
      }
      return c1;
   }

unsigned CountSinkRel( Matrix<CompareRel> r, int (*Count)(CompareRel r))
   {
      unsigned c1 = 0;
      for (size_t i1 = 0; i1 < r.cols(); ++i1)  {
         for ( size_t i2 = 0; i2 < r.rows(); ++i2) {
            if ( Count( r(i2, i1) )) {
               ++c1;
               break;
            }
         }
      }
      return c1;
   }

class SelectCompare  : public CompareOperator
{
  SymbolicVal v1, v2;
  const SymbolicExpr& e1;

  void Default0()
   {
      Matrix<CompareRel> rel(e1.NumOfOpds(),1,0);
      ExprTermCompare(e1,v2,rel,GetFunc());
      MatchCompare(rel,1);

   }
  void MatchCompare(Matrix<CompareRel> &rel, size_t c2)
  {
          SymOpType t1 = e1.GetOpType();
                  if ( t1 == SYMOP_MAX) {
                     if (CountSrcRel( rel, CountLT) == e1.NumOfOpds())
                         result = REL_LT;
                     else if (CountSrcRel( rel, CountLE) == e1.NumOfOpds())
                         result = REL_LE;
                     else if ( CountSinkRel( rel, CountGT) == c2)
                         result = REL_GT;
                     else if ( CountSinkRel( rel, CountGE) == c2)
                         result = REL_GE;
                  }
                  else if (t1 == SYMOP_MIN) {
                     if (CountSrcRel( rel, CountGT) == e1.NumOfOpds())
                         result= REL_GT;
                     else if (CountSrcRel( rel, CountGE) == e1.NumOfOpds())
                         result= REL_GE;
                     else if ( CountSinkRel( rel, CountLT) == c2)
                         result = REL_LT;
                     else if ( CountSinkRel( rel, CountLE) == c2)
                         result = REL_LE;
                  }
  }


  virtual void VisitConst( const SymbolicConst &v) { Default0(); }
  virtual void VisitAstWrap( const SymbolicAstWrap &v) { Default0(); }
  virtual void VisitVar( const SymbolicVar &v) { Default0(); }
  virtual void VisitFunction( const SymbolicFunction &v) { Default0(); }
  virtual void VisitExpr( const  SymbolicExpr& e2)
           {  SymOpType t1 = e1.GetOpType(), t2 = e2.GetOpType();
              unsigned c1 = e1.NumOfOpds(), c2 = e2.NumOfOpds();
              if (t2 == SYMOP_MULTIPLY || t2 == SYMOP_PLUS )
                     Default0();
              else if (t1 == t2) {
                  Matrix<CompareRel> rel(c1,c2,0);
                  ExprTermCompare(e1,e2,rel,GetFunc());
                  MatchCompare(rel,c2);
              }
              else if (t1 == SYMOP_MIN && t2 == SYMOP_MAX) {
                 size_t le = 0, lt = 0, ge = 0, gt = 0;
                 SymbolicExpr::OpdIterator p2 = e2.GetOpdIterator();
                 for ( ; !p2.ReachEnd(); ++p2) {
                     CompareRel r = CompareValHelp(v1, e2.Term2Val(p2.Current()),GetFunc());
                     if (CountLE(r))
                         ++le;
                     if (CountGE(r))
                         ++ge;
                     if (CountLT(r))
                         ++lt;
                     if (CountGT(r))
                         ++gt;
                 }
                 if (gt == e2.NumOfOpds())
                      result = REL_GT;
                 else if (lt > 0)
                       result = REL_LT;
                 else if (ge == e2.NumOfOpds())
                      result = REL_GE;
                 else if (le > 0)
                       result = REL_LE;
              }
              else if (t1 == SYMOP_MAX && t2 == SYMOP_MIN)
                  result = Reverse( SelectCompare(v2, e2, GetFunc())(v1) );
              else
                  assert(false);
           }

 public:
  SelectCompare( const SymbolicVal& _v1, const SymbolicExpr& _e1,
                 MapObject<SymbolicVal,SymbolicBound>* _func = 0)
     : CompareOperator(_func), v1(_v1), e1(_e1) {}
  CompareRel operator() ( const SymbolicVal &_v2)
  { result = REL_UNKNOWN; v2 = _v2;
    v2.Visit(this); return result; }
};

class ValCompare  : public CompareOperator
{
  SymbolicVal v1, v2;
  int index;

  void Default0() {
      if (index == 1) {
         index = 2;
         v2.Visit(this);
      }
      else {
        CompareOperator::Default0(v1,v2);
      }
  }
  void VisitConst( const SymbolicConst &v) { Default0(); }
  void VisitVar( const SymbolicVar &v) { Default0(); }
  void VisitFunction( const SymbolicFunction &v) { Default0(); }
  void VisitExpr( const  SymbolicExpr& v)
          { switch (v.GetOpType()) {
             case SYMOP_MULTIPLY:
             case SYMOP_PLUS:
                   Default0(); break;
             case SYMOP_MIN:
             case SYMOP_MAX:
                   Default1(v1,v2);
                   if (result == REL_UNKNOWN)
                      result = (index == 1)? SelectCompare(v1,v,GetFunc())(v2)
                                  : Reverse(SelectCompare(v2,v,GetFunc())(v1));
                      break;
             default:
                   assert(false);
            }
          }

 public:
  ValCompare( MapObject<SymbolicVal,SymbolicBound>* _func = 0) : CompareOperator(_func) {}
  CompareRel operator() ( const SymbolicVal& _v1, const SymbolicVal &_v2)
  {
    result = REL_UNKNOWN; v1= _v1; v2 = _v2; index = 1;
    v1.Visit(this);
    return result;
  }
};

CompareRel CompareVal(const SymbolicVal &v1, const SymbolicVal &v2,
                      MapObject<SymbolicVal,SymbolicBound>* f)
   {
     if ( v1.IsNIL() && v2.IsNIL()) return REL_UNKNOWN;
     if (DebugCompareVal())
         std::cerr << "comparing " << v1.toString() << " with " << v2.toString() << " under " <<  f << std::endl;
     comparetime = 0;
     return CompareValHelp(v1,v2,f);
   }

CompareRel CompareValHelp(const SymbolicVal &v1, const SymbolicVal &v2,
                      MapObject<SymbolicVal,SymbolicBound>* f)
{
    CompareRel r = REL_UNKNOWN;
    if (++comparetime < COMPARE_MAX)
        r = ValCompare(f)(v1,v2);
    if (DebugCompareVal())
         std::cerr << v1.toString() << RelToString(r) << v2.toString() << " under " << f << std::endl;
     return r;
   }

CompareRel Reverse( CompareRel rel)
{
  switch (rel) {
   case REL_NONE:
   case REL_UNKNOWN:
   case REL_EQ:
   case REL_NE:
        return rel;
   case REL_LT:
        return REL_GT;
   case REL_LE:
        return REL_GE;
   case REL_GT:
        return REL_LT;
   case REL_GE:
        return REL_LE;
   default:
      assert(0);
  }
}

bool operator < (const SymbolicVal &v1, const SymbolicVal& v2)
{ return CompareVal(v1,v2) == REL_LT; }

bool operator > (const SymbolicVal &v1, const SymbolicVal& v2)
{ return CompareVal(v1,v2) == REL_GT; }


bool operator ==( const SymbolicBound& b1, const SymbolicBound& b2)
     { return b1.lb == b2.lb && b1.ub == b2.ub; }
bool operator != (const SymbolicBound &b1, const SymbolicBound& b2)
     { return b1.lb != b2.lb || b1.ub != b2.ub; }
SymbolicBound& operator &= ( SymbolicBound& b1, const SymbolicBound& b2)
    { b1.Intersect(b2); return b1; }
SymbolicBound& operator |= (SymbolicBound& b1, const SymbolicBound& b2)
    { b1.Union(b2); return b1; }

class SplitFraction : public SymbolicVisitor
{
  SymbolicVal *inp, *frp;
  bool hasfrac;
 public:
  virtual void Default(const SymbolicVal& v)
   { if (inp != 0) *inp = v; }
  virtual void VisitConst( const SymbolicConst &v)
   {
     std::string t = v.GetTypeName();
     if (t == "int") {
          if (inp != 0) *inp = v;
          hasfrac=false;
     }
     else if (t == "fraction") {
        if (frp != 0) *frp = v;
        hasfrac = true;
     }
   }
  virtual void VisitFunction( const SymbolicFunction &v)
   {
     bool _hasfrac = hasfrac;
     std::string op = v.GetOp();
     if (op == "pow" && v.last_arg() < 0) {
         _hasfrac = true;
         if (frp != 0) *frp = v;
     }
     else {
         SymbolicFunction::const_iterator p = v.args_begin();
         for (  ; p != v.args_end(); ++p) {
            if (operator()(*p, inp, frp)) {
                _hasfrac = true;
                 break;
            }
         }
         if (p == v.args_end()) {
            if (inp != 0) *inp = v;
         }
         else {
            if (inp == 0 && frp == 0)
                 return;
            SymbolicFunction::Arguments inargs, frargs;
            SymbolicFunction::const_iterator q = v.args_begin();
            for ( ; q != p ; ++q) {
               if (inp != 0)
                  inargs.push_back(*q);
               if (frp != 0)
                  frargs.push_back(*q);
            }
            if (inp != 0)
               inargs.push_back(*inp);
            if (frp != 0)
               frargs.push_back(*frp);
            for (++q; q != v.args_end(); ++q) {
              SymbolicVal cur = *q;
              if (operator()(v, inp, frp)) {
                if (inp != 0)
                   inargs.push_back(*inp);
                if (frp != 0)
                   frargs.push_back(*frp);
              }
              else {
                if (inp != 0)
                   inargs.push_back(cur);
                if (frp != 0)
                   frargs.push_back(cur);
              }
            }
            if (inp != 0)
                *inp = v.cloneFunction(inargs);
            if (frp != 0)
                *frp = v.cloneFunction(frargs);
         }
     }
     hasfrac = _hasfrac;
   }
  virtual void VisitExpr( const  SymbolicExpr& v)
   {
      SymbolicVal in1, fr1;
      bool _hasfrac = hasfrac;
      SymbolicExpr::OpdIterator opds = v.GetOpdIterator();
      for ( ; !opds.ReachEnd(); ++opds) {
         if (operator()(v.Term2Val(*opds), inp, frp)) {
               _hasfrac = true;
                 break;
         }
      }
      if (opds.ReachEnd()) {
         if (inp != 0) *inp = v;
      }
      else {
          if (inp == 0 && frp == 0)
              return;
          SymbolicExpr* inv = (inp == 0)? 0 : v.DistributeExpr(SYMOP_NIL, SymbolicVal());
          SymbolicExpr* frv = (frp == 0)? 0 : v.DistributeExpr(SYMOP_NIL, SymbolicVal());
          SymbolicExpr::OpdIterator opd1 = v.GetOpdIterator();
          for ( ; opd1 != opds ; ++opd1) {
              if (inv != 0)
                  inv->AddOpd(*opd1);
              if (frv != 0)
                  frv->AddOpd(*opd1);
            }
           if (inv != 0)
               inv->ApplyOpd(*inp);
           if (frv != 0)
               frv->ApplyOpd(*frp);
           for (++opd1; !opd1.ReachEnd(); ++opd1) {
              SymbolicVal cur = v.Term2Val(*opd1);
              if (operator()(cur, inp, frp)) {
                 if (inv != 0)
                     inv->ApplyOpd(*inp);
                 if (frv != 0)
                     frv->ApplyOpd(*frp);
              }
              else {
                 if (inv != 0)
                     inv->AddOpd(*opd1);
                 if (frv != 0)
                     frv->AddOpd(*opd1);
              }
           }
           if (inp != 0)
                *inp = GetExprVal(inv);
           if (frp != 0)
                *frp = GetExprVal(frv);
     }
     hasfrac = _hasfrac;
   }

  bool operator()(const SymbolicVal& v, SymbolicVal* i, SymbolicVal* f)
   {
      inp = i;
      frp = f;
      if (inp != 0) *inp = 0;
      if (frp != 0) *frp = 0;
      hasfrac = false;
      v.Visit(this);
      return hasfrac;
   }
};
// extract fraction from v, save integral/rac if the last two pointers are not NULL
bool HasFraction(const SymbolicVal& v, SymbolicVal* integral,
                    SymbolicVal* frac)
{
   return SplitFraction()(v, integral, frac);
}


