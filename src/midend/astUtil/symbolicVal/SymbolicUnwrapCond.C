
#include "SymbolicExpr.h"
#include <stdlib.h>
struct VarRestr
{
  SymbolicVal coeff, result;
  CompareRel rel;
  VarRestr( const SymbolicVal &co, const SymbolicVal &res, CompareRel r)
   : coeff(co), result(res), rel(r) {}
  bool IsNIL() const { return rel == REL_UNKNOWN; }
};

// Evaluate conditional symbolic expression, similar to a=b?1:0
class UnwrapCond : public SymbolicVisitor
{
  SymbolicVar pivot;
  SymbolicVal left, right;
  CompareRel rel;
  bool succ;
  // unwrap condition based on a symbolic value
  void VisitVar( const SymbolicVar& v)
    { 
      if (pivot == v) {
        left = 1;
        succ = true;
      }
    }
  // apply on a symbolic expression  
  void VisitExpr( const  SymbolicExpr& v) 
  { 
    //SymbolicVal cur = left; // so that when left is overwritten v is still valid
    SymbolicVal cur;
    SymbolicExpr::OpdIterator iter = v.GetOpdIterator(); // Operand iterator
    while ( !iter.ReachEnd()) {
      cur = v.Term2Val(iter.Current());
      if (FindVal( cur, pivot))
        break;
      ++iter;
    }
    if (iter.ReachEnd()) return;
    SymbolicTerm p = iter.Current(); // Store the matched operand containing pivot into p
    switch (v.GetOpType()) {
      case SYMOP_MULTIPLY:
        for (iter.Advance(); !iter.ReachEnd(); iter++) {
          if (FindVal( v.Term2Val(iter.Current()), pivot))
            break;
        }
        if (!iter.ReachEnd()) return;
        // this assertion is wrong since p might be a unary expression with its internal operand ==pivot
        // I have handled SgCastExp cases in SymbolicValGenerator::GetSymbolicVal() 
        // I keep this assertion here to expose other unhandled cases, Liao, 11/20/2008
        //printf("debug: before assertion ...");
        // Turned off this assertion to allow more complex condition expressions like b*b-1 and so on
        // Liao, 5/24/2010
        // assert(p == pivot); 	   
        left = 1;
        for (iter=v.GetOpdIterator(); !iter.ReachEnd(); iter.Advance()) {
          if (iter.Current() != p )
            left = left * v.Term2Val(iter.Current());
        }
        succ = true;
        break;
      case SYMOP_PLUS:
        for (iter.Reset(); iter.Current() != p; iter++) 
          right = right - v.Term2Val(iter.Current());
        cur.Visit(this);
        if (!succ) 
          return;
        for (iter.Advance(); !iter.ReachEnd(); ++iter) {
          SymbolicVal curv = v.Term2Val(iter.Current());
          if (FindVal(curv, pivot)) {
            SymbolicVal leftsave = left;
            curv.Visit(this);
            if (!succ)
              return;
            left = left + leftsave;
          } 
          else 
            right = right - curv;
        }
        break;
      case SYMOP_MAX:
        if (rel==REL_LT || rel==REL_EQ || rel==REL_LE) {
          cur.Visit(this);
        }
        break;
      case SYMOP_MIN:
        if (rel==REL_GT || rel==REL_EQ || rel==REL_GE) { 
          cur.Visit(this);
        }
        else if (rel == REL_LE || rel == REL_LT) {
          for (iter.Reset(); !iter.ReachEnd(); iter++) {
            if (p != iter.Current()) 
              right = Min(right,v.Term2Val(*iter));
          }
          cur.Visit(this);
        }
        break;
      default:
        assert(false);
    } 
  }
 public:
  UnwrapCond( const SymbolicVar &var) : pivot(var) {}
  VarRestr operator()( const SymbolicCond &cond)
   { 
     left = cond.GetLeftVal();
     right = cond.GetRightVal();
     rel = cond.GetRelType();
     succ = false;

     bool b1 = FindVal(left, pivot), b2 = FindVal(right, pivot);
     if (!b1 && !b2) 
        rel = REL_NONE;
     else {
        if (!b1 && b2) {
           SymbolicVal tmp = left;
           left = right;
           right = tmp;
           rel = Reverse(rel);
        }
        else if (b1 && b2) {
           left = left - right;
           right = 0;
        } 
        SymbolicVal tmp = left; // need to create a temporary because left may be re-assigned, causing memory error
        tmp.Visit(this); 
        if (!succ)
           rel = REL_UNKNOWN;
     }
     return VarRestr( left, right, rel);
   }
};

SymbolicVal UnwrapVarCond( const SymbolicCond& cond, const SymbolicVar &pivot, SymbolicBound& pivotBound)
{
  VarRestr res = UnwrapCond(pivot)(cond);
  SymbolicVal coeff = res.coeff;
  switch (res.rel) {
  case REL_EQ: pivotBound.Intersect( SymbolicBound(res.result, res.result)); break;
  case REL_LT: pivotBound.Intersect( SymbolicBound(SymbolicVal(), res.result-1)); break;
  case REL_GT: pivotBound.Intersect( SymbolicBound(res.result+1, SymbolicVal())); break;
  case REL_LE: pivotBound.Intersect( SymbolicBound(SymbolicVal(), res.result)); break;
  case REL_GE: pivotBound.Intersect( SymbolicBound( res.result, SymbolicVal())); break;
  case REL_NONE:
       return 0;
  default:
      return SymbolicVal();
  }
  return coeff;
}
