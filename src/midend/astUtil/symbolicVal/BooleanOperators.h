#ifndef SYMBOLIC_BOOLEAN_OPERATORS
#define SYMBOLIC_BOOLEAN_OPERATORS

#include "SymbolicExpr.h"

class SymbolicAnd : public SymbolicExpr  
{  
  std::string GetOPName() const { return "&&"; }  
  virtual SymOpType GetTermOP() const { return SYMOP_AND; }
 public:  
  SymbolicAnd() {}  
  SymbolicAnd(const SymbolicAnd& that) : SymbolicExpr(that) {}  
  
  SymbolicExpr* CloneExpr() const { return new SymbolicAnd(*this); }  
  virtual SymOpType GetOpType() const { return SYMOP_AND; }  
  virtual void ApplyOpd(const SymbolicVal & v);  
  SymbolicExpr* DistributeExpr(SymOpType, const SymbolicVal&) const {  
    return new SymbolicAnd();  
  }  
  AstNodePtr CodeGenOP( AstInterface &fa, const AstNodePtr& a1, const AstNodePtr& a2) const;
};  
  
class AndApplicator : public OPApplicator
{
 public:
  virtual SymOpType GetOpType() { return SYMOP_AND; }

  bool MergeConstInt(int vu1, int /*vd1*/, int vu2, int /*vd2*/, int& r1, int& r2)
  {   
    // If either operand is 0 (false), result is 0 (false)  
    if (vu1 == 0 || vu2 == 0) {   
      r1 = 0;
      r2 = 1;
      return true;  
    }  
    // If both are non-zero (true), result is true
    else {   
      r1 = 1;
      r2 = 1;
      return true;  
    }  
  }  
    
  SymbolicExpr* CreateExpr() { return new SymbolicAnd(); }  
    
  bool IsTop(const SymbolicTerm& t)   
    { return IsOne(t) || t.IsTop(); }
    
  bool MergeElem(const SymbolicTerm& t1, const SymbolicTerm& t2,  
                 SymbolicTerm& result)  
  {   
    int valu1=0, vald1=0, valu2=0, vald2=0;  
      
    // If either is 0 (false), result is false  
    if (IsZero(t1) || IsZero(t2)) {  
      result = SymbolicTerm(0, 1);
      return true;  
    }  
    // If either t1 or t2 is non-zero (true), return the other term  
    else if (IsTop(t1)) {  
      result = t2;  
      return true;  
    }  
    else if (IsTop(t2)) {  
      result = t1;  
      return true;  
    }  
    // If both are constants, compute the result  
    else if (t1.IsConstInt(valu1, vald1) && t2.IsConstInt(valu2, vald2)) {  
      int r1, r2;  
      MergeConstInt(valu1, vald1, valu2, vald2, r1, r2);  
      result = SymbolicTerm(r1, r2);  
      return true;  
    }
    else {
      return OPApplicator::MergeElem(t1, t2, result);
    }
  }   
};  
  
inline void SymbolicAnd::ApplyOpd(const SymbolicVal &v)  
{   
  AndApplicator op;   
  AddOpd(v, &op);  
}  

class SymbolicEq : public SymbolicExpr  
{  
  std::string GetOPName() const { return "=="; }  
  virtual SymOpType GetTermOP() const { return SYMOP_EQ; }
 public:  
  SymbolicEq() {}  
  SymbolicEq(const SymbolicEq& that) : SymbolicExpr(that) {}  
  
  SymbolicExpr* CloneExpr() const { return new SymbolicEq(*this); }  
  virtual SymOpType GetOpType() const { return SYMOP_EQ; }  
  virtual void ApplyOpd(const SymbolicVal & v);  
  SymbolicExpr* DistributeExpr(SymOpType, const SymbolicVal&) const {  
    return new SymbolicEq();  
  }  
  AstNodePtr CodeGenOP( AstInterface &fa, const AstNodePtr& a1, const AstNodePtr& a2) const;
};  
  
class EqualApplicator : public OPApplicator  
{  
 public:  
  virtual SymOpType GetOpType() { return SYMOP_EQ; }

  bool MergeConstInt(int vu1, int vd1, int vu2, int vd2, int& r1, int& r2)   
  {   
    // If values are equal, result is 1 (true)
    r1 = (vu1 * vd2 == vu2 * vd1);
    r2 = 1;
    return true;
  }  
    
  SymbolicExpr* CreateExpr() { return new SymbolicEq(); }  
    
  bool IsTop(const SymbolicTerm& t)   
    { return t.IsTop(); }
    
  bool MergeElem(const SymbolicTerm& t1, const SymbolicTerm& t2,  
                 SymbolicTerm& result)  
  {   
    int valu1=0, vald1=0, valu2=0, vald2=0;  
      
    if(t1.IsConstInt(valu1, vald1) && t2.IsConstInt(valu2, vald2)){
      int r1, r2;
      MergeConstInt(valu1, vald1, valu2, vald2, r1, r2);
      result = SymbolicTerm(r1, r2);
      return true;
    }
    else if(t1 == t2){
      result = SymbolicTerm(1, 1);
      return true;
    }
    else {
      return OPApplicator::MergeElem(t1, t2, result);
    }
  }   
};  
  
inline void SymbolicEq::ApplyOpd(const SymbolicVal &v)  
{   
  EqualApplicator op;   
  AddOpd(v, &op);  
}  

#endif //SYMBOLIC_BOOLEAN_OPERATORS
