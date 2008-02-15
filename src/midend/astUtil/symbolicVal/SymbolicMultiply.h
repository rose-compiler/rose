#ifndef SYMBOLIC_MULTIPLY
#define SYMBOLIC_MULTIPLY

#include <SymbolicExpr.h>

class SymbolicMultiply : public SymbolicExpr
{
  std::string GetOPName() const { return "*"; }
  virtual SymOpType GetTermOP() const {  return SYMOP_POW; }
 public:
  SymbolicMultiply() {}
  SymbolicMultiply( const SymbolicMultiply& that) 
     : SymbolicExpr(that) {}

  SymbolicExpr* CloneExpr() const { return new SymbolicMultiply(*this);  }
  SymbolicExpr* DistributeExpr(SymOpType t, const SymbolicVal& that) const
         { return (t < SYMOP_MULTIPLY)? new SymbolicMultiply() : 0; }

  virtual SymOpType GetOpType() const { return SYMOP_MULTIPLY; }
  virtual void ApplyOpd( const SymbolicVal &v);

  AstNodePtr CodeGenOP( AstInterface &fa, const AstNodePtr& a1, const AstNodePtr& a2) const;
};

class MultiplyApplicator : public OPApplicator
{
 public:
  virtual SymOpType GetOpType() { return SYMOP_MULTIPLY; }

  int MergeConstInt( int v1, int v2)
          { return v1 * v2; }
  SymbolicExpr* CreateExpr() { return new SymbolicMultiply(); } 
//Boolean IsTop( const SymbolicTerm& t)
  int IsTop( const SymbolicTerm& t)
    { return IsOne(t) || t.IsTop(); }
//Boolean MergeElem(const SymbolicTerm& t1, const SymbolicTerm& t2,
  int MergeElem(const SymbolicTerm& t1, const SymbolicTerm& t2,
                            SymbolicTerm& result)
   { 
     int val1, val2;
     if (IsZero( t1) || IsZero( t2)) {
        result = 0; return true;
     }
     else if (IsOne(t1)) {
        result = t2; return true;
     }
     else if (IsOne(t2)) {
        result = t1; return true;
     }
      else if (t1.IsConstInt(&val1) && t2.IsConstInt(&val2)) {
        result = SymbolicTerm(val1 * val2); return true;
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
