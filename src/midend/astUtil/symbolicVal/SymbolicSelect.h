#ifndef SYMBOLIC_SELECT
#define SYMBOLIC_SELECT

#include <SymbolicExpr.h>
#include <const.h>
#include <iostream>

class SymbolicSelect : public SymbolicExpr
{
  int opt;
  std::string GetOPName() const { return ((opt < 0)? "Min" : "Max"); }
  virtual SymOpType GetTermOP() const { return SYMOP_NIL; }
 public:
  SymbolicSelect(int t) : opt(t) {}
  SymbolicSelect( const SymbolicSelect& that) 
     : SymbolicExpr(that), opt(that.opt) {}

  SymbolicExpr* CloneExpr() const { return new SymbolicSelect(*this);  }

  virtual SymOpType GetOpType() const 
        { return (opt < 0)? SYMOP_MIN : SYMOP_MAX; }
  virtual void ApplyOpd( const SymbolicVal &v);

  SymbolicExpr* DistributeExpr(SymOpType t, const SymbolicVal& v) const
   {  
      if (t!= SYMOP_MULTIPLY || v >= 0)
            return new SymbolicSelect(opt);
      else  if (v <= 0)
            return new SymbolicSelect(opt * (-1));
      else
         return 0;
   }


   AstNodePtr CodeGenOP( AstInterface &fa, const AstNodePtr& a1, const AstNodePtr& a2) const
   {  assert(false);  return AstNodePtr(0); }
   AstNodePtr CodeGen(  AstInterface &fa ) const;
};

class SelectApplicator : public OPApplicator
{
   int opt; // opt == -1: min ; 1 : max;
 protected:
   virtual CompareRel Compare(const SymbolicVal& v1, const SymbolicVal& v2)
     { return ::CompareVal(v1,v2); }

// virtual Boolean IsTop( const SymbolicTerm& v)
   virtual int IsTop( const SymbolicTerm& v)
     { 
       int val;
       return OPApplicator::IsTop(v) || (v.IsConstInt(&val) && val*opt == NEG_INFTY);
     } 

// Boolean SelectMerge(const SymbolicVal& v1, const SymbolicVal& v2,
   int SelectMerge(const SymbolicVal& v1, const SymbolicVal& v2,
                         SymbolicVal& result)
    {
      switch (Compare(v1,v2)) {
        case REL_EQ:
        case REL_LT:
        case REL_LE:
                result = (opt < 0)? v1 : v2; return true;
        case REL_GT:
        case REL_GE:
                result = (opt < 0)? v2 : v1; return true;
        default:
            return false;
      }
   }
 public:
  SelectApplicator( int t) : opt(t) {}
  virtual SymOpType GetOpType() { return (opt < 0)? SYMOP_MIN : SYMOP_MAX; }
  SymbolicExpr* CreateExpr() { return  new SymbolicSelect(opt); }
  int MergeConstInt( int v1, int v2)
      { return ((v1-v2) * opt < 0)? v2 : v1; }
//Boolean MergeElem(const SymbolicTerm& t1, const SymbolicTerm& t2,
  int MergeElem(const SymbolicTerm& t1, const SymbolicTerm& t2,
                                  SymbolicTerm& result)
    { SymbolicVal r;
      SymbolicSelect e(opt);
      if (SelectMerge(e.Term2Val(t1), e.Term2Val(t2), r)) {
          result = SymbolicTerm(1,r);
          return true;
       }
       return false;
     }
};

class SelectApplicatorWithBound : public SelectApplicator
{
  MapObject<SymbolicVal, SymbolicBound>& func;
 protected:
//Boolean GetLB( const SymbolicVal& v, int& result);
  int GetLB( const SymbolicVal& v, int& result);
//Boolean GetUB( const SymbolicVal& v, int& result);
  int GetUB( const SymbolicVal& v, int& result);
  CompareRel Compare(const SymbolicVal& v1, const SymbolicVal& v2)
        { return CompareVal(v1,v2, &func); }
 public:
  SelectApplicatorWithBound(  MapObject<SymbolicVal, SymbolicBound>& f, int t)
    : SelectApplicator(t), func(f) {}
};

inline void SymbolicSelect:: ApplyOpd( const SymbolicVal &v)
{ SelectApplicator op(opt);
  AddOpd( v, &op);
}


#endif
