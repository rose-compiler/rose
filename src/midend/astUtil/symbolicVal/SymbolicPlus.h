
#ifndef SYMBOLIC_PLUS_H
#define SYMBOLIC_PLUS_H

#include <SymbolicExpr.h>

class SymbolicPlus : public SymbolicExpr
{
  std::string GetOPName() const { return "+"; }
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
  int MergeConstInt( int v1, int v2) { return v1 + v2; }
  SymbolicExpr* CreateExpr() { return new SymbolicPlus(); }
//Boolean IsTop(const SymbolicTerm& t) 
  int IsTop(const SymbolicTerm& t) 
        { return IsZero(t) || t.IsTop(); }
//Boolean MergeElem(const SymbolicTerm& t1, const SymbolicTerm& t2,
  int MergeElem(const SymbolicTerm& t1, const SymbolicTerm& t2,
                            SymbolicTerm& result)
    { 
      int val1, val2;
      if ( IsZero(t1)) {
        result = t2; return true;
      }
      else if (IsZero(t2)) {
        result = t1; return true;
      }
      else if (t1.IsConstInt(&val1) && t2.IsConstInt(&val2)) {
        result = SymbolicTerm(val1 + val2); return true;
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
