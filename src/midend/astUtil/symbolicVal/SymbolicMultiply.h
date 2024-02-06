#ifndef SYMBOLIC_MULTIPLY
#define SYMBOLIC_MULTIPLY

#include "SymbolicExpr.h"

class SymbolicMultiply : public SymbolicExpr
{
  std::string GetOPName() const { return "*"; }
  virtual SymOpType GetTermOP() const {  return SYMOP_POW; }
 public:
  SymbolicMultiply() {}
  SymbolicMultiply( const SymbolicMultiply& that) 
     : SymbolicExpr(that) {}

  SymbolicExpr* CloneExpr() const { return new SymbolicMultiply(*this);  }
  SymbolicExpr* DistributeExpr(SymOpType t, const SymbolicVal&) const {
      return (t < SYMOP_MULTIPLY)? new SymbolicMultiply() : 0;
  }

  virtual SymOpType GetOpType() const { return SYMOP_MULTIPLY; }
  virtual void ApplyOpd( const SymbolicVal &v);

  AstNodePtr CodeGenOP( AstInterface &fa, const AstNodePtr& a1, const AstNodePtr& a2) const;
};

class MultiplyApplicator : public OPApplicator
{
 public:
  virtual SymOpType GetOpType() { return SYMOP_MULTIPLY; }

  bool MergeConstInt( int vu1, int vd1, int vu2, int vd2, int& r1, int& r2)
          { 
            r1 =  vu1 * vu2; 
            r2 = vd1 * vd2;
            int r3 = r1 / r2;
            if (r3 * r2 == r1) {
                r1 = r3;
                r2 = 1;
            }    
            return true;
          }
  SymbolicExpr* CreateExpr() { return new SymbolicMultiply(); } 
  bool IsTop( const SymbolicTerm& t)
    { return IsOne(t) || t.IsTop(); }
  bool MergeElem(const SymbolicTerm& t1, const SymbolicTerm& t2,
                            SymbolicTerm& result)
   { 
     int valu1=0,vald1=0, valu2=0, vald2=0;
     if (IsZero( t1) || IsZero( t2)) {
        result = SymbolicTerm(0,1); return true;
     }
     else if (IsOne(t1)) {
        result = t2; return true;
     }
     else if (IsOne(t2)) {
        result = t1; return true;
     }
      else if (t1.IsConstInt(valu1, vald1) && t2.IsConstInt(valu2, vald2)) {
        int r1, r2;
        MergeConstInt(valu1, vald1, valu2, vald2, r1, r2);
        result = SymbolicTerm(r1, r2); return true;
      }
     else
        return OPApplicator::MergeElem(t1,t2,result);
   }
};

inline void SymbolicMultiply:: ApplyOpd( const SymbolicVal &v)
{ MultiplyApplicator op;
  AddOpd( v, &op);
}


#endif
