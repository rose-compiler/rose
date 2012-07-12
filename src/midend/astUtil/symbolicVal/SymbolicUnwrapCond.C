#include "SymbolicExpr.h"

struct VarRestr
{
  SymbolicVal coeff, result;
  CompareRel rel;
  VarRestr( const SymbolicVal &co, const SymbolicVal &res, CompareRel r)
   : coeff(co), result(res), rel(r) {}
  bool IsNIL() const { return rel == REL_UNKNOWN; }
};
class UnwrapCond : public SymbolicVisitor
{
  SymbolicVar pivot;
  SymbolicVal left, right;
  CompareRel rel;
  bool succ;

  void VisitVar( const SymbolicVar& v)
    { if (pivot == v) {
        left = 1;
        succ = true;
      }
    }
  void VisitExpr( const  SymbolicExpr& v) 
   { 
     SymbolicVal cur;
     SymbolicExpr::OpdIterator iter = v.GetOpdIterator();
     while ( !iter.ReachEnd()) {
         cur = v.Term2Val(iter.Current());
         if (FindVal( cur, pivot))
            break;
         ++iter;
     }
     if (iter.ReachEnd()) return;
     SymbolicTerm p = iter.Current(); 
     switch (v.GetOpType()) {
     case SYMOP_MULTIPLY:
          for (iter.Advance(); !iter.ReachEnd(); iter++) {
             if (FindVal( v.Term2Val(iter.Current()), pivot))
                break;
          }
          if (!iter.ReachEnd()) return;
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
