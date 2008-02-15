
#ifndef SYMBOLIC_PLUS_H
#define SYMBOLIC_PLUS_H

#include <SymbolicExpr.h>

class SymbolicPlus : public SymbolicExpr
{
  STD string GetOPName() const { return "+"; }
  virtual SymOpType GetTermOP() const { return SYMOP_MULTIPLY; }
 public:
  SymbolicPlus() : SymbolicExpr() {}
  SymbolicPlus( const SymbolicPlus& that) : SymbolicExpr(that) {}

  virtual SymOpType GetOpType() const { return SYMOP_PLUS; }
  virtual void ApplyOpd( const SymbolicVal & v) ;
  SymbolicExpr* DistributeExpr(SymOpType t, const SymbolicVal& that) const
          { return new SymbolicPlus(); }

  SymbolicExpr* CloneExpr() const { return new SymbolicPlus(*this);  }
  AstNodePtr CodeGenOP( AstInterface &fa, const AstNodePtr& a1, const AstNodePtr& a2) const;
};

class PlusApplicator : public OPApplicator
{
 public:
  SymOpType GetOpType() { return SYMOP_PLUS; }
  bool MergeConstInt( int vu1, int vd1, int vu2, int vd2, int& r1, int& r2) 
       { assert(vd1 == vd2);  //QY: not yet handle other case
         r1= vu1 + vu2; 
         r2 = vd1;
         return true;
       }
  SymbolicExpr* CreateExpr() { return new SymbolicPlus(); }
  bool IsTop(const SymbolicTerm& t) 
        { return IsZero(t) || t.IsTop(); }
  bool MergeElem(const SymbolicTerm& t1, const SymbolicTerm& t2,
                            SymbolicTerm& result)
    { 
      int valu1,vald1, valu2, vald2;
      if ( IsZero(t1)) {
        result = t2; return true;
      }
      else if (IsZero(t2)) {
        result = t1; return true;
      }
      else if (t1.IsConstInt(valu1, vald1) && t2.IsConstInt(valu2, vald2)) {
         int r1, r2;
         MergeConstInt(valu1, vald1, valu2, vald2, r1, r2);
         result = SymbolicTerm(r1, r2);
         return true;
      }
      else
        return OPApplicator::MergeElem(t1,t2,result); 
    } 
};

inline void SymbolicPlus:: ApplyOpd( const SymbolicVal &v)
{ PlusApplicator op; 
  AddOpd( v, &op);
}

#endif
