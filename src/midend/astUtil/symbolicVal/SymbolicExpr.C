#include "SymbolicExpr.h"
#include <iostream>
#include <sstream>
#include <stdio.h>
#include "CommandOptions.h"

bool SymbolicTerm::CombineWith( const SymbolicTerm &that)
         { if (v == that.v) {
               if (time2 == that.time2)
                   time1 += that.time1;
               else 
                   assert(false); // QY: a case not yet handled
               return true;
           }
           return false;
         }

bool SymbolicTerm::operator == (const SymbolicTerm& that) const
     { return time1 == that.time1 && time2 == that.time2 && v == that.v; }
bool SymbolicTerm::operator == (const SymbolicVal &that) const
    { return time1 == 1 && time2 == 1 && v == that; }

class SymbolicTermMerge : public LatticeElemMerge<SymbolicTerm>
{
  OPApplicator& op;
 public:
  SymbolicTermMerge( OPApplicator& _op) : op(_op) {}
  bool IsTop( const SymbolicTerm& v)
   { return op.IsTop(v); }
  bool MergeElem(const SymbolicTerm& t1, const SymbolicTerm& t2,
                            SymbolicTerm& result)
        { return op.MergeElem(t1,t2,result); }
};


SymbolicVal SymbolicExpr::GetUnknownOpds() const
     { 
       SymbolicExpr* r = DistributeExpr( SYMOP_NIL, SymbolicVal());
       for (OpdIterator p = GetOpdIterator(); !p.ReachEnd(); ++p) {
           if (! p.Current().IsConst())
              r->AddOpd(*p);
       }
       return GetExprVal(r);
     }

bool SymbolicExpr :: GetConstOpd( int &val1, int& val2) const
{
  for (OpdIterator p = GetOpdIterator(); !p.ReachEnd(); ++p) {
     if ( p.Current().IsConstInt(val1, val2)) {
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
     {
       v.IsNIL();
       AddOpd(Val2Term(v), op);
     }


bool SymbolicExpr:: operator == (const SymbolicExpr& that) const
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


std::string SymbolicTerm :: toString() const
{
  std::stringstream out;

  if (v.IsNIL()) {
    out << time1;
    if (time2 != 1)
        out << "/" << time2;    
   }
   else { 
     out << v.toString();
     if (time1 != 1 || time2 != 1) {
       out << "(";
       if (time1 != 1)
         out << time1;
       if (time2 != 1)
         out << "/" << time2;
       out << ")";
     }
   }
  return out.str();
}

std::string SymbolicExpr :: toString() const
{
  std::string r = "(";
  bool begin = true;
  for (OpdIterator iter = GetOpdIterator(); 
       !iter.ReachEnd(); iter.Advance())  
  {
    if (!begin) { r = r + GetOPName(); }
    else begin = false;
    r = r + iter.Current().toString();
  }
/*QY: prefix output
  std::string r = GetOPName() + "(";
  for (OpdIterator iter = GetOpdIterator(); 
       !iter.ReachEnd(); iter.Advance())  
    r = r + iter.Current().toString();
*/
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
              std::cerr << "non-recognized expression type: " << r->toString() << std::endl;
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
  int valu, vald;
  SymbolicVal v2;

  void Default() { DefaultOP(SymbolicConst(valu,vald), v2); }
  virtual void VisitConst( const SymbolicConst &v) 
               {
                   int valu1, vald1;
                   if (v.GetIntVal(valu1, vald1)) {
                      int r1, r2;
                      op.MergeConstInt(valu, vald, valu1, vald1, r1, r2);
                      result = SymbolicConst(r1, r2);
                   }
                   else 
                      Default();
               }
  virtual void VisitExpr( const SymbolicExpr& v)
       {
          if (op.GetOpType() == v.GetOpType())  {
             SymbolicExpr *r = v.CloneExpr();
             r->AddOpd( SymbolicTerm(valu,vald), &op);
             result = GetExprVal(r);
          }
          else
             OPHelpVisitor::VisitExpr(v);
       }

 public: 
  IntVisitor( const SymbolicVal& v1, int _valu, int _vald, OPApplicator &_op) 
    : OPHelpVisitor( _op, v1), valu(_valu), vald(_vald) {}
  SymbolicVal ApplyOP( const SymbolicVal& v)
    { v2 = v; return OPHelpVisitor::ApplyOP(v); }

};

class TermVisitor : public OPHelpVisitor
{
  SymbolicVal v2;

  void Default() { DefaultOP(that, v2); }

  void VisitConst( const SymbolicConst &v)
          {
              int valu, vald;
              if (v.GetIntVal(valu,vald)) 
                 result = IntVisitor( v2, valu,vald,op).ApplyOP(that); 
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
       int valu, vald;
       if (v.GetIntVal(valu, vald)) {
          SymbolicExpr* r = exp.CloneExpr();
          r->AddOpd( SymbolicTerm(valu, vald), &op);
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
          int valu, vald;
          if (v.GetIntVal(valu, vald))
             result = IntVisitor(v2, valu, vald, op).ApplyOP(that); 
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
          int valu, vald;
          if (v.GetIntVal(valu, vald)) {
            IntVisitor intOp(v1,valu, vald,op);
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

