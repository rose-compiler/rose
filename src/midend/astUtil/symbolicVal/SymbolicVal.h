
#ifndef SYMBOLIC_VAL
#define SYMBOLIC_VAL

#include <CountRefHandle.h>
#include <FunctionObject.h>
#include <AstInterface.h>
#include <assert.h>
#include <string>
#include <vector>
#include <map>

class SymbolicConst;
class SymbolicVar;
class SymbolicFunction;
class SymbolicExpr;
class SymbolicAstWrap;
class SymbolicVisitor
{
 protected:
  virtual void Default() {}
 public:
  virtual void VisitConst( const SymbolicConst &v) { Default(); }
  virtual void VisitVar( const SymbolicVar &v) { Default(); }
  virtual void VisitFunction( const SymbolicFunction &v) { Default(); }
  virtual void VisitAstWrap( const SymbolicAstWrap& v) { Default(); }
  virtual void VisitExpr( const SymbolicExpr& exp) { Default(); }
};

typedef enum {VAL_BASE = 0, VAL_CONST = 1, VAL_VAR = 2, VAL_AST = 4, 
              VAL_FUNCTION = 8, VAL_EXPR = 16}
      SymbolicValType;
typedef enum { SYMOP_NIL = 0, SYMOP_MULTIPLY=1, SYMOP_PLUS = 2,
               SYMOP_MIN=3, SYMOP_MAX=4, SYMOP_POW = 5} SymOpType;

class SymbolicValImpl 
{
 protected:
  virtual ~SymbolicValImpl() {}
 public:
  virtual std::string ToString() const { return ""; }
  virtual AstNodePtr ToAst() const { return 0; }

  virtual SymOpType GetOpType() const { return SYMOP_NIL; }
  virtual SymbolicValType GetValType() const { return VAL_BASE; }
  virtual std::string GetTypeName() const { return "unknown"; }
  virtual void Visit( SymbolicVisitor *op) const = 0;
  virtual AstNodePtr  CodeGen(AstInterface &fa) const = 0;
  virtual SymbolicValImpl* Clone() const = 0;

  virtual void Dump() const;

 friend class CountRefHandle<SymbolicValImpl>;
};

inline SymbolicValImpl* Clone(const SymbolicValImpl& that) 
  { return that.Clone(); }


class SymbolicConst : public SymbolicValImpl
{
  std::string val, type;
  int intval;

  SymbolicValImpl* Clone() const { return new SymbolicConst(*this); }
 public:
  SymbolicConst( int _val) ;
  SymbolicConst( std::string _val, std::string type);
  SymbolicConst( const SymbolicConst& that) 
       : val(that.val), type(that.type), intval(that.intval) {}
  ~SymbolicConst() {}

  bool operator == (const SymbolicConst& that) const 
    { return val == that.val && type == that.type; }
  virtual std::string ToString() const;
  virtual void Dump() const { std::cerr << val << "(" << type << ")"; }
  SymbolicValType GetValType() const { return VAL_CONST; }
  std::string GetTypeName() const { return type; }
  AstNodePtr  CodeGen(AstInterface &fa) const;
  void Visit( SymbolicVisitor *op) const { op->VisitConst(*this); }
//Boolean GetIntVal( int &result, int opt = 0) const
  int GetIntVal( int &result, int opt = 0) const
          { 
            if (type == "int") {
             result = intval; return true; 
            }
            return false;
          }
//Boolean GetConstVal( std::string& result, std::string& rtype) const 
  int GetConstVal( std::string& result, std::string& rtype) const 
          { 
             result = val; rtype = type; return true; 
          }
  std::string GetVal() const { return val; }
};

class SymbolicVar : public SymbolicValImpl
{
  std::string varname;
  AstNodePtr scope;
  SymbolicValImpl* Clone() const { return new SymbolicVar(varname, scope); }
 public:
#if 1
// DQ (3/23/2006): Put the implementation in the source file
  SymbolicVar( std::string name, const AstNodePtr& sc);
#else
  SymbolicVar( std::string name, const AstNodePtr& sc) : varname(name), scope(sc)
     {
       printf ("scope->class_name() = %s \n",scope->class_name().c_str());
     }
#endif
  SymbolicVar() : varname(""), scope(0) {}
  ~SymbolicVar() {}
  virtual std::string ToString() const;
  virtual void Dump() const { std::cerr << varname; }
  SymbolicValType GetValType() const { return VAL_VAR; }
  AstNodePtr  CodeGen( AstInterface &fa) const;

  std::string GetVarName() const { return varname; }
  AstNodePtr GetVarScope() const  { return scope; }
//virtual Boolean GetVarName( std::string& r) const 
  virtual int GetVarName( std::string& r) const 
         { r = varname; return true; } 
//Boolean operator == (const SymbolicVar &that) const ;
  int operator == (const SymbolicVar &that) const ;
  void Visit( SymbolicVisitor *op) const { op->VisitVar(*this); }
};

class SymbolicAstWrap : public SymbolicValImpl
{
  AstNodePtr ast;
  Map2Object <AstInterface*, AstNodePtr, AstNodePtr>* codegen;

  SymbolicValImpl* Clone() const { return new SymbolicAstWrap(*this); }
 public:
  SymbolicAstWrap( const AstNodePtr& _ast, 
                   Map2Object <AstInterface*, AstNodePtr, AstNodePtr>* cg = 0) 
    : ast(_ast), codegen(cg) {}
  SymbolicAstWrap() : ast(0), codegen(0) {}
  SymbolicAstWrap( const SymbolicAstWrap& that)
    : SymbolicValImpl(that), ast(that.ast), codegen(that.codegen) {}

  ~SymbolicAstWrap() {}
  virtual void Dump() const;
  virtual std::string ToString() const;
  virtual AstNodePtr ToAst() const { return ast; }

  SymbolicValType GetValType() const { return VAL_AST; }
  virtual std::string GetTypeName() const { return "astwrap"; }
  AstNodePtr CodeGen( AstInterface &fa) const ;

  void Visit( SymbolicVisitor *op) const { op->VisitAstWrap(*this); }
//Boolean operator == (const SymbolicAstWrap& that) const
  int operator == (const SymbolicAstWrap& that) const
     { return ast == that.ast; }
  const AstNodePtr& get_ast() const { return ast; }
};

class SymbolicVal : public CountRefHandle <SymbolicValImpl>
{
 public:
  SymbolicVal() {}
  SymbolicVal( SymbolicValImpl* _impl)
    : CountRefHandle <SymbolicValImpl>(_impl) {}
  SymbolicVal( const SymbolicValImpl& _impl)
    : CountRefHandle <SymbolicValImpl>(_impl) {}
  SymbolicVal (int val) ;
  SymbolicVal( const SymbolicVal& that)
   : CountRefHandle <SymbolicValImpl>(that) {}
  SymbolicVal& operator = (const SymbolicVal& that)
   { CountRefHandle <SymbolicValImpl>:: operator = (that); return *this; }

//Boolean IsNIL() const { return ConstPtr() == 0; }
  int IsNIL() const { return ConstPtr() == 0; }
//Boolean IsSame( const SymbolicVal& that) const
  int IsSame( const SymbolicVal& that) const
     { return ConstPtr() == that.ConstPtr(); }
//Boolean IsSame( const SymbolicValImpl& impl) const
  int IsSame( const SymbolicValImpl& impl) const
     { return ConstPtr() == &impl; }
  AstNodePtr CodeGen(AstInterface &fa) const
           { return (ConstPtr()==0)? AstNodePtr(0) : ConstRef().CodeGen(fa); }
  void Visit( SymbolicVisitor *op) const 
         { if (ConstPtr()!=0) 
                 ConstRef().Visit(op); }
  void Dump() const;
  std::string ToString() const;
  bool ToInt(int &val) const 
   { 
     if (ConstPtr() != 0 && ConstRef().GetTypeName() == "int" && 
         ConstRef().GetValType() == VAL_CONST) {
         val = atoi(ConstRef().ToString().c_str());
         return true;
      }
     return false;
   }
                 
  virtual AstNodePtr ToAst() const 
      { return (ConstPtr()== 0)? 0: ConstRef().ToAst(); }
  SymbolicValType GetValType() const 
      { return (ConstPtr()== 0)? VAL_BASE : ConstRef().GetValType(); }
  virtual SymOpType GetOpType() const  
     { return  (ConstPtr()== 0)? SYMOP_NIL : ConstRef().GetOpType(); }
  std::string GetTypeName() const 
    { return (ConstPtr()== 0)? "NIL" : ConstRef().GetTypeName(); }
};

class SymbolicFunction : public SymbolicValImpl
{
  SymbolicVal op;
  std::vector<SymbolicVal> args;

  SymbolicValImpl* Clone() const { return CloneFunction(op, args); }
 public:
  typedef std::vector<SymbolicVal> Arguments;
  SymbolicFunction() {}
  SymbolicFunction( const std::string& _op, const Arguments& v ) 
    : args(v) { op = new SymbolicConst(_op, "function"); }
  SymbolicFunction( const std::string& _op, const SymbolicVal& v1, const SymbolicVal& v2)
     { 
        op = new SymbolicConst(_op, "function");
        args.push_back(v1); args.push_back(v2);
      }
  SymbolicFunction( const SymbolicVal& _op, const Arguments& v)
    : op(_op), args(v) {}
  SymbolicFunction( const SymbolicFunction& that)
    : SymbolicValImpl(that), op(that.op), args(that.args) {}

  ~SymbolicFunction() {}
  virtual std::string ToString() const;
  SymbolicValType GetValType() const { return VAL_FUNCTION; }
  AstNodePtr CodeGen( AstInterface &fa) const;
  virtual SymbolicFunction* CloneFunction( const SymbolicVal& _op, 
                                         const Arguments& v) const
         { return new SymbolicFunction( _op, v); }

  void Visit( SymbolicVisitor *op) const { op->VisitFunction(*this); }
//Boolean operator == (const SymbolicFunction& that) const;
  int operator == (const SymbolicFunction& that) const;
  SymbolicVal GetOp() const { return op; }
  Arguments GetArgs() const { return args; }
  unsigned NumOfArgs() const { return args.size(); }
  SymbolicVal GetArg(int index) const { return args[index]; }
  virtual SymbolicVal GetUnknownOpds() const 
      { return SymbolicVal(); }
//virtual Boolean GetConstOpd(int &val) const { return false; }
  virtual int GetConstOpd(int &val) const { return false; }
};

class SymbolicPow : public SymbolicFunction
{
 public:
  SymbolicPow( const SymbolicVal& v, int e ) 
    : SymbolicFunction("pow", v, e)
    { }
  SymbolicPow( const SymbolicPow& that)
    : SymbolicFunction(that) {}
  virtual SymOpType GetOpType() const { return SYMOP_POW; }
  virtual SymbolicVal GetUnknownOpds() const { return GetArg(0); }
//virtual Boolean GetConstOpd(int &val) const { return GetArg(1).ToInt(val); }
  virtual int GetConstOpd(int &val) const { return GetArg(1).ToInt(val); }
};

class ToSymbolicFunction : public SymbolicVisitor
{
  SymbolicFunction* result;  
  bool succ;
  virtual void VisitFunction( const SymbolicFunction &v) { *result = v;  succ = true; }
 public:
  bool operator() (const SymbolicVal& v, SymbolicFunction& r)
  {
     result = &r;
     succ = false;
     v.Visit(this); 
     return succ;
  }
};

class SymbolicValGenerator
{
 public:
 static SymbolicVal GetSymbolicVal( AstInterface &fa, const AstNodePtr& exp);
};

typedef enum {REL_NONE = 0, REL_EQ = 1, REL_LT = 2, REL_LE = 3,
              REL_GT = 4, REL_GE = 5, REL_NE = 6, REL_UNKNOWN = 8} CompareRel;
CompareRel Reverse( CompareRel rel);
std::string RelToString( CompareRel r);
// Boolean operator == (const SymbolicVal &v1, const SymbolicVal& v2);
int operator == (const SymbolicVal &v1, const SymbolicVal& v2);
// Boolean operator != (const SymbolicVal &v1, const SymbolicVal& v2);
int operator != (const SymbolicVal &v1, const SymbolicVal& v2);
// Boolean operator <= (const SymbolicVal &v1, const SymbolicVal &v2);
int operator <= (const SymbolicVal &v1, const SymbolicVal &v2);
// Boolean operator >= (const SymbolicVal &v1, const SymbolicVal& v2);
int operator >= (const SymbolicVal &v1, const SymbolicVal& v2);
// Boolean operator < (const SymbolicVal &v1, const SymbolicVal& v2);
int operator < (const SymbolicVal &v1, const SymbolicVal& v2);
// Boolean operator > (const SymbolicVal &v1, const SymbolicVal& v2);
int operator > (const SymbolicVal &v1, const SymbolicVal& v2);

class SymbolicCond
{
  SymbolicVal val1, val2;
  CompareRel t;
 public:
  SymbolicCond() { t = REL_UNKNOWN; }
  SymbolicCond( const CompareRel _t, const SymbolicVal &_val1,
                const SymbolicVal &_val2)
      : val1(_val1), val2(_val2), t(_t) {}
  SymbolicCond( const SymbolicCond& that)
    : val1(that.val1), val2(that.val2), t(that.t) {}
  SymbolicCond& operator = (const SymbolicCond& that)
    { t = that.t; val1 = that.val1; val2 = that.val2;  return *this; }
  ~SymbolicCond() {}

  SymbolicVal  GetLeftVal() const { return val1; }
  SymbolicVal  GetRightVal() const { return val2; }
  CompareRel GetRelType() const { return t; }

//Boolean IsNIL() { return t == REL_UNKNOWN; }
  int IsNIL() { return t == REL_UNKNOWN; }

  void Dump() const;
  std::string ToString () const;
  AstNodePtr  CodeGen(AstInterface &fa) const;
};

SymbolicVal operator + (const SymbolicVal &v1, const SymbolicVal &v2);
SymbolicVal operator - (const SymbolicVal &v);
SymbolicVal operator - (const SymbolicVal &v1, const SymbolicVal &v2);
SymbolicVal operator * (const SymbolicVal &v1, const SymbolicVal &v2);
SymbolicVal operator / (const SymbolicVal &v1, const SymbolicVal &v2);
//Boolean HasFraction(const SymbolicVal& v, SymbolicVal* integral = 0, SymbolicVal* frac = 0);
int HasFraction(const SymbolicVal& v, SymbolicVal* integral = 0, SymbolicVal* frac = 0);

struct SymbolicBound{
  SymbolicVal lb, ub;
  SymbolicBound() {}
  SymbolicBound( const SymbolicVal &l, const SymbolicVal &u): lb(l),ub(u) {}
  std::string ToString() const 
      { return "(" + lb.ToString() + "," + ub.ToString() + ")"; }
  SymbolicBound operator - (const SymbolicVal& v) const
   { return SymbolicBound( lb - v, ub - v); } 
  SymbolicBound operator + (const SymbolicVal& v) const
   { return SymbolicBound( lb + v, ub + v); } 
  void Union( const SymbolicBound& b2, 
	      MapObject<SymbolicVal, SymbolicBound>* f = 0);
  void Intersect( const SymbolicBound& b2, 
		  MapObject<SymbolicVal, SymbolicBound>* f = 0);  
  void ReplaceVars(  MapObject<SymbolicVal,SymbolicBound>& f); 
};
//Boolean operator ==( const SymbolicBound& b1, const SymbolicBound& b2);
int operator ==( const SymbolicBound& b1, const SymbolicBound& b2);
//Boolean operator != (const SymbolicBound &b1, const SymbolicBound& b2);
int operator != (const SymbolicBound &b1, const SymbolicBound& b2);
SymbolicBound& operator &= ( SymbolicBound& b1, const SymbolicBound& b2);
SymbolicBound& operator |= (SymbolicBound& b1, const SymbolicBound& b2);
SymbolicBound GetValBound(SymbolicVal val, MapObject<SymbolicVal, SymbolicBound>& f);
SymbolicVal GetValLB(const SymbolicVal& val, MapObject<SymbolicVal, SymbolicBound>& f);
SymbolicVal GetValUB(const SymbolicVal& val, MapObject<SymbolicVal, SymbolicBound>& f);

CompareRel CompareVal( const SymbolicVal &v1, const SymbolicVal &v2, 
                       MapObject<SymbolicVal, SymbolicBound>* f = 0);
SymbolicVal Max(const SymbolicVal &v1, const SymbolicVal &v2,
                       MapObject<SymbolicVal, SymbolicBound>* f = 0);
SymbolicVal Min(const SymbolicVal &v1, const SymbolicVal &v2,
                       MapObject<SymbolicVal, SymbolicBound>* f = 0);
SymbolicVal UnwrapVarCond( const SymbolicCond& valCond,
                         const SymbolicVar &pivot, SymbolicBound& pivotBound ); //return pivot coefficient

//Boolean FindVal( const SymbolicVal &v, const SymbolicVal &sub);
int FindVal( const SymbolicVal &v, const SymbolicVal &sub);
SymbolicVal ReplaceVal( const SymbolicVal &v, const SymbolicVal &sub, const SymbolicVal& newval);
SymbolicVal ReplaceVal( const SymbolicVal &v, MapObject<SymbolicVal, SymbolicVal>& valmap);

class AstTreeReplaceVar : public ProcessAstNode
{
   SymbolicVar oldvar;
   SymbolicVal val;
  public:
   AstTreeReplaceVar( const SymbolicVar& ov, const SymbolicVal &nv)
       : oldvar(ov), val(nv) {}
// Boolean Traverse( AstInterface &fa, const AstNodePtr& r,
   int Traverse( AstInterface &fa, const AstNodePtr& r,
                     AstInterface::TraversalVisitType t)
     {
        std::string varname;
        if (t == AstInterface::PostVisit && fa.IsVarRef(r, 0, &varname)) {
          if (varname == oldvar.GetVarName()) {
            AstNodePtr r1 = val.CodeGen(fa);
            fa.ReplaceAst(r,r1);
          }
        }
        return true;
     }
   void operator() ( AstInterface &fa, const AstNodePtr& root)
       { ReadAstTraverse( fa, root, *this, AstInterface::PostOrder); }
};

#endif

