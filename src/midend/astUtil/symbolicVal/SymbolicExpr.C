#include <general.h>

#include <SymbolicExpr.h>
#include <iostream>
#include <stdio.h>
#include <CommandOptions.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/8/2006): Since this is not used in a heade file it is OK here!
#define Boolean int

Boolean SymbolicTerm::CombineWith( const SymbolicTerm &that)
         { if (v == that.v) {
               time += that.time;
               return true;
           }
           return false;
         }

Boolean SymbolicTerm::operator == (const SymbolicTerm& that) const
     { return time == that.time && v == that.v; }
Boolean SymbolicTerm::operator == (const SymbolicVal &that) const
    { return time == 1 && v == that; }

class SymbolicTermMerge : public LatticeElemMerge<SymbolicTerm>
{
  OPApplicator& op;
 public:
  SymbolicTermMerge( OPApplicator& _op) : op(_op) {}
  Boolean IsTop( const SymbolicTerm& v)
   { return op.IsTop(v); }
  Boolean MergeElem(const SymbolicTerm& t1, const SymbolicTerm& t2,
                            SymbolicTerm& result)
        { return op.MergeElem(t1,t2,result); }
};


SymbolicVal SymbolicExpr::GetUnknownOpds() const
     { 
       SymbolicExpr* r = DistributeExpr( SYMOP_NIL, SymbolicVal());
       for (OpdIterator p = GetOpdIterator(); !p.ReachEnd(); ++p) {
           if (! p.Current().IsConstInt())
              r->AddOpd(*p);
       }
       return GetExprVal(r);
     }

Boolean SymbolicExpr :: GetConstOpd( int &val) const
{
  for (OpdIterator p = GetOpdIterator(); !p.ReachEnd(); ++p) {
     if ( p.Current().IsConstInt(&val)) {
         return true;
     }
  }
  return false;
}

unsigned SymbolicExpr::NumOfOpds() const
      { return (ConstPtr()==0)? 0 : ConstRef().NumOfOpds(); }

SymbolicTerm& SymbolicExpr :: FirstOpd() const
{
  return ConstRef().First();
}

SymbolicExpr::OpdIterator SymbolicExpr::GetOpdIterator() const
   { if (ConstPtr() == 0)
        return OpdIterator();
     return ConstRef().GetOpdIterator(); 
   }

void SymbolicExpr:: AddOpd( const SymbolicTerm& v, OPApplicator *op)
    {  if (UpdatePtr() == 0)
         CountRefHandle<SymbolicOperands>::Reset(new SymbolicOperands());
       UpdateRef().AddOpd( v, op);
    }

void SymbolicExpr:: AddOpd( const SymbolicVal& v, OPApplicator* op)
     { // if (v.IsNIL());
       AddOpd(Val2Term(v), op);
     }


Boolean SymbolicExpr:: operator == (const SymbolicExpr& that) const
    { if (GetOpType() == that.GetOpType() && 
          NumOfOpds() == that.NumOfOpds()) {
          OpdIterator iter1 = GetOpdIterator(); 
          for( ; !iter1.ReachEnd(); iter1.Advance()) {
            OpdIterator iter2 = that.GetOpdIterator(); 
            for ( ;  !iter2.ReachEnd(); iter2.Advance()) {
                if (iter1.Current() == iter2.Current())
                   break;
            }
            if (iter2.ReachEnd())
               break;
         }
         if (iter1.ReachEnd())
              return true;
      }
      return false;
   }


string SymbolicTerm :: ToString() const
{
  char buf[20];
  sprintf(buf, "%d",  time);

  if (IsConstInt()) {
       return buf; 
   }
   else { 
     if (time != 1)
       return v.ToString() + "(" + buf + ")"; 
     else
       return v.ToString();
   }
}

string SymbolicExpr :: ToString() const
{
  string r = GetOPName() + "(";
  bool first = true;
  for (OpdIterator iter = GetOpdIterator(); 
       !iter.ReachEnd(); iter.Advance()) {
    if (!first) r += ", ";
    first = false;
    r = r + iter.Current().ToString();
  }
  r = r + ")";
  return r;
}

void SymbolicOperands :: AddOpd( const SymbolicTerm& v, OPApplicator *op)
{   if (op != 0) {
        SymbolicTermMerge merge(*op);
        opds.AddElem( v, &merge); 
    }
    else
       opds.AddElem(v);
}

SymbolicVal GetExprVal( SymbolicExpr *r)
   {
      int num = r->NumOfOpds();
      if (num == 1) {
         SymbolicVal result = r->Term2Val(r->FirstOpd());
         delete r;
         return result;
      }
      else if (num == 0) {
         switch (r->GetOpType()) {
         case SYMOP_PLUS: return 0;
         case SYMOP_MULTIPLY: return 1;
         default: 
              cerr << "non-recognized expression type: " << r->ToString() << endl;
              assert(false);
         }
      }
      return r;
   }

class OPHelpVisitor : public SymbolicVisitor
{
 protected:
  OPApplicator& op;
  SymbolicVal result, that;

  void DefaultOP( const SymbolicVal& v1, const SymbolicVal &v2) 
     { SymbolicExpr *r = op.CreateExpr();
            r->AddOpd(v1,&op); r->AddOpd(v2, &op);
            result = GetExprVal(r); }

  virtual void VisitExpr( const SymbolicExpr& v)
       {
          SymOpType t1 = op.GetOpType(), t2 = v.GetOpType() ;
          if (t1 > t2)
             Default();
          else if (t1 < t2) {
            SymbolicExpr* r = v.DistributeExpr(t1, that);
            if (r == 0) 
               Default();
            else {
              for (SymbolicExpr::OpdIterator iter = v.GetOpdIterator();
                   !iter.ReachEnd(); iter.Advance()) {
                 SymbolicVal tmp = ApplyOP(v.Term2Val(iter.Current()));
                 r->ApplyOpd( tmp );
              }
              result = GetExprVal(r);
            }
          }
          else {
            assert(false);
          }
       }
 public:
  OPHelpVisitor(OPApplicator& _op, const SymbolicVal& _that) 
    :  op(_op), that(_that) {}
   OPHelpVisitor(OPApplicator& _op) : op(_op) {}

  virtual SymbolicVal ApplyOP( const SymbolicVal& v)
    { result = SymbolicVal();
      v.Visit(this); return result; }
};

class IntVisitor : public OPHelpVisitor
{
  int val;
  SymbolicVal v2;

  void Default() { DefaultOP(val, v2); }
  virtual void VisitConst( const SymbolicConst &v) 
               {
                   int val1;
                   if (v.GetIntVal(val1)) 
                      result = op.MergeConstInt(val, val1);
                   else 
                      Default();
               }
  virtual void VisitExpr( const SymbolicExpr& v)
       {
          if (op.GetOpType() == v.GetOpType())  {
             SymbolicExpr *r = v.CloneExpr();
             r->AddOpd( SymbolicTerm(val), &op);
             result = GetExprVal(r);
          }
          else
             OPHelpVisitor::VisitExpr(v);
       }

 public: 
  IntVisitor( const SymbolicVal& v1, int _val, OPApplicator &_op) 
    : OPHelpVisitor( _op, v1), val(_val) {}
  SymbolicVal ApplyOP( const SymbolicVal& v)
    { v2 = v; return OPHelpVisitor::ApplyOP(v); }

};	

class TermVisitor : public OPHelpVisitor
{
  SymbolicVal v2;

  void Default() { DefaultOP(that, v2); }

  void VisitConst( const SymbolicConst &v)
          {
              int val;
              if (v.GetIntVal(val)) 
                 result = IntVisitor( v2, val,op).ApplyOP(that); 
              else 
                 Default();
          }
  void VisitExpr( const SymbolicExpr& v)
   {
      if (op.GetOpType() == v.GetOpType()) {
         SymbolicExpr *r = v.CloneExpr();
         r->AddOpd( v.Val2Term(that), &op );
         result = GetExprVal(r);
      }
      else
         OPHelpVisitor::VisitExpr(v);
   }

 public:
  TermVisitor( const SymbolicVal &_v1, OPApplicator& _op) 
    : OPHelpVisitor(_op, _v1) {}

 SymbolicVal ApplyOP( const SymbolicVal& v)
    { v2 = v; return OPHelpVisitor::ApplyOP(v); }
}; 

class CombineVisitor : public OPHelpVisitor
{
  const SymbolicExpr &exp;
  SymbolicVal v2;

  virtual void Default()
   { SymbolicExpr* r = exp.CloneExpr();
     r->AddOpd( exp.Val2Term(v2), &op ); 
     result = GetExprVal(r); }
  virtual void VisitConst( const SymbolicConst &v)
   { 
       int val;
       if (v.GetIntVal(val)) {
          SymbolicExpr* r = exp.CloneExpr();
          r->AddOpd( SymbolicTerm(val), &op);
          result = GetExprVal(r); 
       }
       else
          Default();
 
   }
  virtual void VisitExpr( const SymbolicExpr& v)
   {  
      if (v.GetOpType() == op.GetOpType()) {
         SymbolicExpr* r = exp.CloneExpr();
         for (SymbolicExpr::OpdIterator iter = v.GetOpdIterator();
              !iter.ReachEnd(); iter.Advance())
              r->AddOpd( iter.Current(), &op);
         result = GetExprVal(r);
      }
      else
        OPHelpVisitor::VisitExpr(v);
   }  
 public:
  CombineVisitor( const SymbolicVal &_v1, const SymbolicExpr &_exp,
                  OPApplicator& _op)
    : OPHelpVisitor( _op,_v1), exp(_exp) {}
  SymbolicVal ApplyOP( const SymbolicVal& v)
    { v2 = v; return OPHelpVisitor::ApplyOP(v); }
};

class DistributeVisitor : private OPHelpVisitor
{
  const SymbolicExpr &exp;
  SymbolicVal v2;

  void Default()
   { result = TermVisitor(v2,op).ApplyOP(that); }
  void VisitConst( const SymbolicConst &v) 
      { 
          int val;
          if (v.GetIntVal(val))
             result = IntVisitor(v2, val, op).ApplyOP(that); 
          else
             Default();
      }
  void VisitExpr( const  SymbolicExpr& v)
      { if (v.GetOpType() == exp.GetOpType())  {
           SymbolicExpr* r = exp.DistributeExpr(op.GetOpType(), v2);
           if (r == 0) 
              DefaultOP(that,v2);
           else {
             for (SymbolicExpr::OpdIterator iter1 = exp.GetOpdIterator();
                  !iter1.ReachEnd(); iter1.Advance()) {
               SymbolicVal tmp = exp.Term2Val(iter1.Current());
               for (SymbolicExpr::OpdIterator iter2 = v.GetOpdIterator();
                    !iter2.ReachEnd(); iter2.Advance()) {
                 r->ApplyOpd( ApplyBinOP(op, tmp, v.Term2Val(iter2.Current())));
               }
             }
             result = GetExprVal(r);
           }
        }
        else if (v.GetOpType() < exp.GetOpType()) {
          if (v.GetOpType() < op.GetOpType())
              Default();
          else if (v.GetOpType() == op.GetOpType())
              result = CombineVisitor(v2,v,op).ApplyOP(that);
          else 
             result = DistributeVisitor(v2, v, op).ApplyOP(that);
        }
        else 
           OPHelpVisitor::VisitExpr(v);
      }
 public:
  DistributeVisitor( const SymbolicVal& v, const SymbolicExpr& _exp, 
                     OPApplicator& _op) : OPHelpVisitor(_op, v), exp(_exp) {}
  SymbolicVal ApplyOP( const SymbolicVal &_v2)
   { v2 = _v2; return OPHelpVisitor::ApplyOP(_v2); }
};

class OPVisitor  : public SymbolicVisitor
{
  SymbolicVal v1, v2, result;
  OPApplicator& op;
   void Default()
   { result = TermVisitor(v1,op).ApplyOP(v2); }
  void VisitConst( const SymbolicConst &v)
      { 
          int val;
          if (v.GetIntVal(val)) {
            IntVisitor intOp(v1,val,op);
            result = intOp.ApplyOP(v2); 
          }
          else
             Default();
      }

  void VisitExpr( const  SymbolicExpr& v)
      { if (op.GetOpType() == v.GetOpType())
           result = CombineVisitor(v1, v,op).ApplyOP(v2);
        else if (op.GetOpType() < v.GetOpType()) 
           result = DistributeVisitor(v1,v,op).ApplyOP(v2);
        else
           Default();
      }
 public:
  OPVisitor( OPApplicator& _op) : op(_op) {}
  SymbolicVal operator()( const SymbolicVal &_v1, const SymbolicVal &_v2)
   { v2 = _v2; v1 = _v1;  result = SymbolicVal();
     v1.Visit(this); return result; }
};


SymbolicVal ApplyBinOP( OPApplicator& op,
                        const SymbolicVal &v1, const SymbolicVal &v2)
{
  OPVisitor tmp(op);
  return tmp(v1,v2);
}

