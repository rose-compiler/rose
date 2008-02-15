#ifndef SYMBOLIC_EXPR
#define SYMBOLIC_EXPR

#include <SymbolicVal.h>
#include <LatticeElemList.h>
#include <iostream>


SymbolicVal ApplyBinOP( SymOpType t, const SymbolicVal &v1,
                        const SymbolicVal &v2);
class SymbolicTerm
{
  int time;
  SymbolicVal v;
 public:
  SymbolicTerm( int t, const SymbolicVal& _v) : time(t), v( _v) { }
  SymbolicTerm(int t = 0) : time(t) {}
  ~SymbolicTerm() {}
  SymbolicTerm& operator = (const SymbolicTerm& that)
     { time = that.time; v = that.v; return *this; }

  std::string ToString() const;
//Boolean IsConstInt( int *val = 0) const 
  int IsConstInt( int *val = 0) const 
      { if (val != 0) *val = time; 
        return v.IsNIL(); }
  SymbolicVal GetVal( SymOpType op)  const
          { int val;
            if (IsConstInt(&val)) return val;
            else if ( time == 1)
               return v;
            return ApplyBinOP( op, v, time);
          }


//Boolean IsTop() const { return  !v.IsNIL() && time == 0; } 
  int IsTop() const { return  !v.IsNIL() && time == 0; } 
//Boolean CombineWith( const SymbolicTerm &that)  ;
  int CombineWith( const SymbolicTerm &that)  ;
//Boolean operator == (const SymbolicTerm& that) const;
  int operator == (const SymbolicTerm& that) const;
//Boolean operator == (const SymbolicVal &that) const;
  int operator == (const SymbolicVal &that) const;
 friend class SymbolicExpr;
};

//inline Boolean IsZero( const SymbolicTerm& t)
inline int IsZero( const SymbolicTerm& t)
      { int val; 
        return t.IsConstInt(&val) && val == 0; }
// inline Boolean IsOne( const SymbolicTerm& t)
inline int IsOne( const SymbolicTerm& t)
      { int val;
        return t.IsConstInt(&val) && val == 1; }
// inline Boolean operator != (const SymbolicTerm& v1, const SymbolicTerm& v2)
inline int operator != (const SymbolicTerm& v1, const SymbolicTerm& v2)
{  return !(v1 == v2); }

class OPApplicator;
class SymbolicOperands
{
  LatticeElemList<SymbolicTerm> opds;
 public:
  typedef LatticeElemList<SymbolicTerm>::Iterator OpdIterator;

  SymbolicOperands() {}
  SymbolicOperands( const SymbolicOperands& that)
    { for (OpdIterator iter(that.opds); !iter.ReachEnd(); iter.Advance())
         opds.AddElem(iter.Current());
    }

  void AddOpd( const SymbolicTerm& v, OPApplicator *op = 0) ;
  unsigned NumOfOpds() const { return opds.NumberOfEntries(); }
  OpdIterator GetOpdIterator() const { return OpdIterator(opds); }
  SymbolicTerm& First() const { return opds.First()->GetEntry(); }
  std::string ToString() const;

  SymbolicOperands* Clone() const { return new SymbolicOperands(*this); }
};

class SymbolicExpr : public SymbolicValImpl, 
                     public CountRefHandle<SymbolicOperands>
{
  void operator = (const SymbolicExpr& that) {}

 protected:
  SymbolicExpr() {}
  SymbolicExpr( const SymbolicExpr& that)
    : SymbolicValImpl(that), CountRefHandle<SymbolicOperands>(that) {}

  virtual SymOpType GetTermOP() const = 0;
 public:
  SymbolicTerm Val2Term(const SymbolicVal& v) const;
  SymbolicVal Term2Val( const SymbolicTerm& tm) const
          { return tm.GetVal(GetTermOP()); }

  typedef LatticeElemList<SymbolicTerm>::Iterator OpdIterator;
  void AddOpd( const SymbolicTerm& v, OPApplicator* op = 0);
  void AddOpd( const SymbolicVal& v, OPApplicator* op = 0);

  virtual void ApplyOpd(  const SymbolicVal& v ) = 0;
  virtual SymbolicExpr* DistributeExpr(SymOpType t, const SymbolicVal& that) const = 0;
  void Visit( SymbolicVisitor* op) const { op->VisitExpr(*this); }
  SymbolicValImpl* Clone() const { return CloneExpr(); }
  std::string ToString() const;
  SymbolicValType GetType() const { return VAL_EXPR; }
  void push_back( const SymbolicVal& v) { ApplyOpd(v); }
  virtual std::string GetOPName () const = 0;

  virtual SymOpType GetOpType() const = 0;
  virtual SymbolicExpr* CloneExpr() const  = 0;

  virtual AstNodePtr CodeGenOP( AstInterface &fa, 
                               const AstNodePtr& a1, const AstNodePtr& a2) const =0;

  virtual AstNodePtr CodeGen( AstInterface &fa) const
  {
   OpdIterator iter = GetOpdIterator();
   AstNodePtr l = Term2Val(iter.Current()).CodeGen(fa);
   for (iter.Advance(); !iter.ReachEnd(); iter.Advance())
      l = CodeGenOP(fa, l, Term2Val(iter.Current()).CodeGen(fa));
   return l;
  }

  SymbolicVal GetUnknownOpds() const;
//Boolean GetConstOpd(int &val) const;
  int GetConstOpd(int &val) const;
  unsigned NumOfOpds() const ;
  OpdIterator GetOpdIterator() const;
  SymbolicTerm& FirstOpd() const;

//Boolean operator == (const SymbolicExpr& that) const;
  int operator == (const SymbolicExpr& that) const;
};

class ValTermVisitor : public SymbolicVisitor
{
  SymbolicTerm result;
  SymOpType expOP;
  void VisitConst( const SymbolicConst& e) 
       { int v = 0;
         if (e.GetIntVal(v))
             result = SymbolicTerm(v); 
       }
  void VisitFunction( const SymbolicFunction& v)
   {
     int val;
     if (v.GetOpType() == expOP &&
         v.GetConstOpd(val)) {
         result = SymbolicTerm(val, v.GetUnknownOpds());
     }
   }
  void VisitExpr( const SymbolicExpr& v)
   {
     int val;
     if (v.GetOpType() == expOP &&
         v.GetConstOpd(val)) {
         result = SymbolicTerm(val, v.GetUnknownOpds());
     }
   }
  public:
    ValTermVisitor( SymOpType t) : expOP(t) {}
    SymbolicTerm operator()( const SymbolicVal& v)
     { result = SymbolicTerm(1,v);
       v.Visit(this);  return result; }
};

class OPApplicator 
{
 public:
  virtual SymOpType GetOpType() = 0;

  virtual SymbolicExpr *CreateExpr() = 0;
  virtual int MergeConstInt(int v1, int v2) = 0;
//virtual Boolean IsTop( const SymbolicTerm& v)
  virtual int IsTop( const SymbolicTerm& v)
   { return v.IsTop(); }
//virtual Boolean MergeElem(const SymbolicTerm& t1, const SymbolicTerm& t2,
  virtual int MergeElem(const SymbolicTerm& t1, const SymbolicTerm& t2,
                            SymbolicTerm& result)
        { result = t1;
          return result.CombineWith(t2); }
};

inline SymbolicTerm SymbolicExpr::
Val2Term(const SymbolicVal& v) const
          { return ValTermVisitor(GetTermOP())(v); }

SymbolicVal ApplyBinOP( OPApplicator& op, 
                        const SymbolicVal &_v1, const SymbolicVal &_v2);
SymbolicVal GetExprVal( SymbolicExpr *exp); 
   // delete exp if return value if different from exp
#endif
