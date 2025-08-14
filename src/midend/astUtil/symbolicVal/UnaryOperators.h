#ifndef SYMBOLIC_UNARY_OPERATORS
#define SYMBOLIC_UNARY_OPERATORS

#include "SymbolicExpr.h"

class UnaryOperator : public SymbolicExpr
{
  protected:
    UnaryOperator() {}
    UnaryOperator( const UnaryOperator& that) : SymbolicExpr() {}
  public:
    AstNodePtr CodeGen(AstInterface &fa) const override
    {
      OpdIterator iter = GetOpdIterator();
      AstNodePtr operand = Term2Val(iter.Current()).CodeGen(fa);
      return CodeGenUnaryOP(fa, operand);
    }
    
    std::string toString() const override
    {
      std::string r = GetOPName() + "(";
      for (OpdIterator iter = GetOpdIterator(); !iter.ReachEnd(); iter.Advance()){
        r = r + iter.Current().toString();
      }
      r = r + ")";
      return r;
    }
    
    virtual AstNodePtr CodeGenUnaryOP(AstInterface &fa, const AstNodePtr& operand) const = 0;
    
    AstNodePtr CodeGenOP(AstInterface &fa, const AstNodePtr& a1, const AstNodePtr& a2) const override
    {
      ROSE_ABORT();
    }
    
    SymbolicExpr* DistributeExpr(SymOpType op, const SymbolicVal& that) const override
    {
      return CloneExpr();
    }
};

class SymbolicNot : public UnaryOperator  
{  
  std::string GetOPName() const override { return "!"; }  
  SymOpType GetTermOP() const override { return SYMOP_NOT; }
 public:  
  SymbolicNot() {}  
  SymbolicNot(const SymbolicNot& that) : UnaryOperator(that) {}  
  
  SymbolicExpr* CloneExpr() const override { return new SymbolicNot(*this); }  
  SymOpType GetOpType() const { return SYMOP_NOT; }  
  void ApplyOpd(const SymbolicVal & v) override;  
  SymbolicExpr* DistributeExpr(SymOpType, const SymbolicVal&) const {  
    return new SymbolicNot();  
  }
  AstNodePtr CodeGenUnaryOP(AstInterface& fa, const AstNodePtr& operand) const override {
    return fa.CreateUnaryOP(AstInterface::UOP_NOT, operand);
  }
};  

class UnaryOPApplicator : public OPApplicator
{
  public:
    bool MergeConstInt(int vu1, int vd1, int vu2, int vd2, int& r1, int& r2) override final
    {
      return MergeConstIntUnary(vu1, vd1, r1, r2);
    }

    bool MergeElem(const SymbolicTerm& t1, const SymbolicTerm& t2, SymbolicTerm& result) override final
    {
      return MergeElemUnary(t1, result);
    }

    virtual bool MergeConstIntUnary(int vu, int vd, int& r1, int& r2) = 0;
    virtual bool MergeElemUnary(const SymbolicTerm& t1, SymbolicTerm& result) = 0;
};

class NotApplicator : public UnaryOPApplicator  
{  
 public:  
  SymOpType GetOpType() override { return SYMOP_NOT; }

  bool MergeConstIntUnary(int vu, int vd, int& r1, int& r2) override
  {
      // If operand is 0 (false), result is 1 (true)  
      if (vu == 0) {
          r1 = 1; 
          r2 = 1;
      }
      else {
          r1 = 0;
          r2 = 1;
      }
      return true;
  }
    
  SymbolicExpr* CreateExpr() override { return new SymbolicNot(); }  
    
  bool IsTop(const SymbolicTerm& t) override  
    { return t.IsTop(); }

  bool MergeElemUnary(const SymbolicTerm& t1, SymbolicTerm& result) override
  {
    int valu=0, vald=0;
    if (t1.IsConstInt(valu, vald)) {
        int r1, r2;
        MergeConstIntUnary(valu, vald, r1, r2);
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
        return false;
    }
  }
};  
  
inline void SymbolicNot::ApplyOpd(const SymbolicVal &v)  
{   
  NotApplicator op;   
  AddOpd(v, &op);
}

#endif //SYMBOLIC_UNARY_OPERATORS
