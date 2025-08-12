#ifndef SYMBOLIC_UNARY_OPERATORS
#define SYMBOLIC_UNARY_OPERATORS

#include "SymbolicExpr.h"

class SymbolicNot : public SymbolicExpr  
{  
  std::string GetOPName() const { return "!"; }  
  virtual SymOpType GetTermOP() const { return SYMOP_NOT; }
 public:  
  SymbolicNot() {}  
  SymbolicNot(const SymbolicNot& that) : SymbolicExpr(that) {}  
  
  SymbolicExpr* CloneExpr() const { return new SymbolicNot(*this); }  
  virtual SymOpType GetOpType() const { return SYMOP_NOT; }  
  virtual void ApplyOpd(const SymbolicVal & v);  
  SymbolicExpr* DistributeExpr(SymOpType, const SymbolicVal&) const {  
    return new SymbolicNot();  
  }
  AstNodePtr CodeGenOP(AstInterface&, const AstNodePtr&, const AstNodePtr&) const {
    ROSE_ABORT();
  }
};  
  
class NotApplicator : public OPApplicator  
{  
 public:  
  virtual SymOpType GetOpType() { return SYMOP_NOT; }

  bool MergeConstInt(int vu1, int vd1, int vu2, int vd2, int& r1, int& r2)
  {
      // For unary expression, only use vu1 and vd1 (ignore vu2, vd2)
      // If operand is 0 (false), result is 1 (true)  
      if (vu1 == 0) {
          r1 = 1; 
          r2 = 1;
      }
      else {
          r1 = 0;
          r2 = 1;
      }
      return true;
  }
    
  SymbolicExpr* CreateExpr() { return new SymbolicNot(); }  
    
  bool IsTop(const SymbolicTerm& t)   
    { return t.IsTop(); }

  bool MergeElem(const SymbolicTerm& t1, const SymbolicTerm& t2, SymbolicTerm& result)
  {
    int valu=0, vald=0;
    if (t1.IsConstInt(valu, vald)) {
        int r1, r2;
        MergeConstInt(valu, vald, 0, 1, r1, r2);
        result = SymbolicTerm(r1, r2);
        return true;
    }
    else if (IsZero(t1)) {
        result = SymbolicTerm(1, 1);
        return true;
    }
    else if (IsOne(t1)) {
        result = SymbolicTerm(0, 1);
        return true;
    }
    else {
        return OPApplicator::MergeElem(t1, t2, result);
    }
  }
};  
  
inline void SymbolicNot::ApplyOpd(const SymbolicVal &v)  
{   
  NotApplicator op;   
  AddOpd(v, &op);
}

#endif //SYMBOLIC_UNARY_OPERATORS