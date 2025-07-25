#include "SymbolicVal.h"

#include "SymbolicMultiply.h"
#include "SymbolicPlus.h"
#include "BooleanOperators.h"
#include "SymbolicSelect.h"
#include <ROSE_ABORT.h>
#include <ROSE_ASSERT.h>
#include "CommandOptions.h"

#include <list>
#include <stdio.h>
#include <sstream>
using namespace std;
void SymbolicValImpl :: Dump() const
{ std::cerr << toString(); }

void SymbolicVal::Dump() const
{ std::cerr << toString(); }

std::string SymbolicVal::toString() const
{
   return (ConstPtr() != 0)? ConstRef().toString() : std::string("");
}

SymbolicVal ::SymbolicVal (int val)
  : CountRefHandle <SymbolicValImpl>( new SymbolicConst(val) ) {}


SymbolicConst::  SymbolicConst( int _val, int _d)
        : val (""), type(_d == 1? "int" : "fraction"), intval( _val), dval(_d)
     {
        char buf[40];
        if (_d == 1)
           snprintf(buf, sizeof(buf), "%d", _val);
        else
           snprintf(buf, sizeof(buf), "%d/%d", _val,_d);
        val = buf;
     }

SymbolicConst:: SymbolicConst( std::string _val, std::string t) : val(_val), type(t)
 {
    if (type == "int") {
      intval = atoi(_val.c_str());
      dval = 1;
    }
 }

std::string SymbolicConst :: toString() const
{
     return val;
}

AstNodePtr  SymbolicConst :: CodeGen(AstInterface &fa) const
{
  if (type == "int")
    return fa.CreateConstInt(intval);
  else
    return fa.CreateConstant( type, val);
}

std::string SymbolicVar :: toString() const
{
 return varname;
}

AstNodePtr  SymbolicVar :: CodeGen( AstInterface &fa) const
{
  if (exp_ != 0) {
     return fa.CopyAstTree(exp_);
  }
  return fa.CreateVarRef(varname, scope);
}

AstNodePtr SymbolicAstWrap::CodeGen( AstInterface &fa) const
      {
         if (codegen == 0)
             return fa.CopyAstTree(ast);
         else
             return (*codegen)(&fa, ast);
      }

std::string SymbolicAstWrap::toString() const
{
   return "AstWrap(" + AstInterface::AstToString(ast) + ")";
}

void SymbolicAstWrap::Dump() const
{
   std::cerr << "AstWrap: " << ast.get_ptr();
   if (codegen != 0)
       std::cerr << "(codegen = " << codegen << ")\n";
}

bool SymbolicVar :: operator == (const SymbolicVar &that) const
{
  return varname == that.varname && ( scope == that.scope || scope == AST_NULL || that.scope == AST_NULL);
}


std::string SymbolicFunction :: toString() const
{
  std::string r = "(";
  if (args.size() == 0)
      r= "()";
  for (const_iterator i = args.begin(); i != args.end(); ++i) {
      r = r + (*i).toString() + ",";
   }
   r[r.size()-1] = ')';
   return op.toString() + r;
}

bool SymbolicFunction:: operator == (const SymbolicFunction& that) const
{
  if (args.size() != that.args.size())
    return false;
  if (op != that.op)
      return false;
  for (const_iterator i = args.begin(), j = that.args.begin();
       i != args.end(); ++i,++j) {
    if (*i != *j)
      return false;
  }
  return true;
}

AstNodePtr SymbolicFunction:: CodeGen( AstInterface &_fa) const
{
  AstNodeList l;
  for (const_iterator i = args.begin(); i != args.end(); ++i) {
     SymbolicVal cur = *i;
     AstNodePtr curast = cur.CodeGen(_fa);
     l.push_back(curast.get_ptr());
  }
  if (t == AstInterface::OP_NONE) {
     return _fa.CreateFunctionCall( op.CodeGen(_fa), l.begin(), l.end());
  }
  else if (t == AstInterface::OP_ARRAY_ACCESS) {
        AstNodeList::const_iterator b = l.begin();
        AstNodePtr arr = *b;
        for (++b; b != l.end(); ++b) {
           arr =  _fa.CreateArrayAccess(arr, *b);
        }
        return arr;
     }
  else if (t == AstInterface::OP_ASSIGN && l.size() == 2) {
        return _fa.CreateAssignment(l.front(), l.back());
     }
  else if (l.size() == 2)
      return _fa.CreateBinaryOP( t, l.front(), l.back());
  else {
      assert(l.size() == 1);
      return _fa.CreateUnaryOP( t, l.front());
  }
}

AstNodePtr SymbolicSelect:: CodeGen(  AstInterface &fa ) const
   {
      int size = 0;
      AstInterface::AstNodeList list;
      for (OpdIterator iter = GetOpdIterator(); !iter.ReachEnd(); iter.Advance()) {
           AstNodePtr p = Term2Val(iter.Current()).CodeGen(fa);
           list.push_back(p.get_ptr());
           ++size;
      }
      assert( size > 1);
      std::string func = (opt< 0)? "min" : "max";

      return fa.CreateFunctionCall(func, list.begin(), list.end());
   }

void SymbolicBound::
Union( const SymbolicBound& b2, MapObject<SymbolicVal, SymbolicBound>* f)
{
  lb = Min(lb, b2.lb, f);
  ub = Max(ub, b2.ub, f);
}

void SymbolicBound::
Intersect( const SymbolicBound& b2, MapObject<SymbolicVal, SymbolicBound>* f)
{
  lb = Max(lb, b2.lb, f);
  ub = Min(ub, b2.ub, f);
}

void SymbolicBound::
ReplaceVars(  MapObject<SymbolicVal,SymbolicBound>& f)
   {
          lb = GetValLB( lb, f) ;
          ub = GetValUB( ub, f) ;
   }


std::string RelToString( CompareRel r)
{
  switch (r) {
  case REL_EQ: return "==";
  case REL_NE: return "!=";
  case REL_LT: return "<";
  case REL_GT: return ">";
  case REL_LE: return "<=";
  case REL_GE: return ">=";
  case REL_UNKNOWN: return "?";
  default:
    ROSE_ABORT();
  }
}


std::string SymbolicCond :: toString() const
{
  std::string r = val1.toString() + RelToString(GetRelType()) + val2.toString();
  return r;
}

void SymbolicCond:: Dump() const
{
  std::cerr << toString();
}

AstNodePtr  SymbolicCond :: CodeGen(AstInterface &fa) const
{
  switch (GetRelType()) {
  case REL_EQ:
     return fa.CreateBinaryOP(AstInterface::BOP_EQ, val1.CodeGen(fa), val2.CodeGen(fa));
  case REL_NE:
     return fa.CreateBinaryOP( AstInterface::BOP_NE,
                               val1.CodeGen(fa), val2.CodeGen(fa));
  case REL_LT:
    return fa.CreateBinaryOP( AstInterface::BOP_LT,
                               val1.CodeGen(fa), val2.CodeGen(fa));
  case REL_LE:
     return fa.CreateBinaryOP( AstInterface::BOP_LE,
                               val1.CodeGen(fa), val2.CodeGen(fa));
  case REL_GT:
     return fa.CreateBinaryOP( AstInterface::BOP_GT,
                               val1.CodeGen(fa), val2.CodeGen(fa));
  case REL_GE:
     return fa.CreateBinaryOP( AstInterface::BOP_GE,
                               val1.CodeGen(fa), val2.CodeGen(fa));
  default:
     ROSE_ABORT();
  }
}

AstNodePtr SymbolicMultiply::
CodeGenOP( AstInterface &fa, const AstNodePtr& a1, const AstNodePtr& a2) const
  {
    int val = 0;
    if (fa.IsConstInt(a1, &val) && val == -1)
        return fa.CreateUnaryOP(AstInterface::UOP_MINUS, a2);
    else if (fa.IsConstInt(a2, &val) && val == -1)
        return fa.CreateUnaryOP(AstInterface::UOP_MINUS, a1);
    return fa.CreateBinaryOP(AstInterface::BOP_TIMES, a1, a2);
  }

AstNodePtr SymbolicPlus::
CodeGenOP( AstInterface &fa, const AstNodePtr& a1, const AstNodePtr& a2) const
 {
    AstNodePtr opd;
    AstInterface::OperatorEnum opr;
    if (fa.IsUnaryOp(a2, &opr, &opd) && opr == AstInterface::UOP_MINUS) {
      return fa.CreateBinaryOP(AstInterface::BOP_MINUS,a1, fa.CopyAstTree(opd));
    }
    else if (fa.IsUnaryOp(a1, &opr, &opd) && opr == AstInterface::UOP_MINUS) {
      return fa.CreateBinaryOP(AstInterface::BOP_MINUS,a2, fa.CopyAstTree(opd));
    }
    return fa.CreateBinaryOP(AstInterface::BOP_PLUS, a1, a2);
 }

AstNodePtr SymbolicAnd::  
CodeGenOP(AstInterface &fa, const AstNodePtr& a1, const AstNodePtr& a2) const  
{  
   return fa.CreateBinaryOP(AstInterface::BOP_AND, a1, a2);  
} 

AstNodePtr SymbolicEq::  
CodeGenOP(AstInterface &fa, const AstNodePtr& a1, const AstNodePtr& a2) const  
{  
   return fa.CreateBinaryOP(AstInterface::BOP_EQ, a1, a2);  
}  

SymbolicVal SymbolicValGenerator::
GetSymbolicVal( const std::string& sig) {
  DebugLog debugval("-debugsym");
  if (sig == "_NULL_") {
      return SymbolicVal();
  } else if (sig == "_UNKNOWN_") {
     return SymbolicVal(new SymbolicValImpl()); 
  } else if (sig[0] == '*') {
     debugval([&sig]() { return "creating pointer deref:" + sig; });
     auto r = GetSymbolicVal(sig.substr(1, sig.size()-1));
     return new SymbolicFunction( AstInterface::UOP_DEREF, "*", r);
 
  } else {
     debugval([&sig]() { return "creating variable:" + sig; });
     return SymbolicVal(new SymbolicVar(sig, NULL));
  }
}

bool SymbolicValGenerator::
IsFortranLoop(AstInterface& fa, const AstNodePtr& s, SymbolicVar* ivar ,
        SymbolicVal* lb , SymbolicVal* ub, SymbolicVal* step, AstNodePtr* body)
{
  AstNodePtr ivarast, lbast, ubast, stepast, ivarscope;
  if (!fa.IsFortranLoop(s, &ivarast, &lbast, &ubast, &stepast, body))
      return false;
  std::string varname;
  if (! fa.IsVarRef(ivarast, 0, &varname, &ivarscope)) {
         return false;
  }
  if (ivar != 0)
     *ivar = SymbolicVar(varname, ivarscope, ivarast);
  if (lb != 0)
     *lb = SymbolicValGenerator::GetSymbolicVal(fa,lbast);
  if (ub != 0)
     *ub = SymbolicValGenerator::GetSymbolicVal(fa,ubast);
  if (step != 0) {
     if (stepast != AST_NULL)
       *step = SymbolicValGenerator::GetSymbolicVal(fa,stepast);
     else
       *step = SymbolicVal(1);
  }
  return true;
}

SymbolicVal SymbolicValGenerator ::
GetSymbolicVal( AstInterface &fa, const AstNodePtr& exp)
{
  std::string name;
  AstNodePtr scope;
  int val = 0;
  AstNodePtr s1, s2;
  AstInterface::AstNodeList l;
  AstInterface::OperatorEnum opr = (AstInterface::OperatorEnum)0;

  if (fa.IsConstInt(exp, &val)) {
     return new SymbolicConst( val );
  }
  else if (fa.IsBinaryOp(exp, &opr, &s1, &s2)) {
     SymbolicVal v1 = GetSymbolicVal( fa, s1 ), v2 = GetSymbolicVal(fa, s2);
     switch (opr) {
     case AstInterface::BOP_TIMES:
         return v1 * v2;
     case AstInterface::BOP_PLUS:
         return v1 + v2;
     case AstInterface::BOP_MINUS:
         return v1 - v2;
     case AstInterface::BOP_MOD:
         return new SymbolicFunction(opr, "%", v1, v2);
     case AstInterface::BOP_DOT_ACCESS:
     case AstInterface::BOP_ARROW_ACCESS:
         return new SymbolicAstWrap(exp);
     case AstInterface::BOP_DIVIDE:
        return new SymbolicFunction( opr, "/", v1,v2);
     case AstInterface::BOP_EQ:
        return new SymbolicFunction( opr, "==", v1,v2);
     case AstInterface::BOP_LE:
        return new SymbolicFunction( opr, "<=", v1,v2);
     case AstInterface::BOP_LT:
        return new SymbolicFunction( opr, "<", v1,v2);
     case AstInterface::BOP_NE:
        return new SymbolicFunction( opr, "!=", v1,v2);
     case AstInterface::BOP_GT:
        return new SymbolicFunction( opr, ">", v1,v2);
     case AstInterface::BOP_GE:
        return new SymbolicFunction( opr, ">=", v1,v2);
     case AstInterface::BOP_AND:
        return new SymbolicFunction( opr, "&&", v1,v2);
     case AstInterface::BOP_OR:
        return new SymbolicFunction( opr, "||", v1,v2);
     case AstInterface::BOP_BIT_RSHIFT:
        return new SymbolicFunction( opr, ">>", v1,v2);
     case AstInterface::BOP_BIT_LSHIFT:
        return new SymbolicFunction( opr, "<<", v1,v2);
     default:
     {
         cerr<<"Error in SymbolicValGenerator::GetSymbolicVal(): unhandled type of binary operator "<< AstInterface::toString(opr) <<endl;
        ROSE_ABORT();
     }
     }
  }
  else if (fa.IsUnaryOp(exp, &opr, &s1)) {
    SymbolicVal v = GetSymbolicVal( fa, s1);
    switch (opr) {
    case AstInterface::UOP_MINUS:
        return (-1) * v;
    case AstInterface::UOP_ADDR:
        return new SymbolicFunction( opr, "&", v);
    case AstInterface::UOP_DEREF:
        return new SymbolicFunction( opr, "*", v);
    case AstInterface::UOP_ALLOCATE:
        return new SymbolicFunction( opr, "new", v);
    case AstInterface::UOP_NOT:
        return new SymbolicFunction( opr, "!", v);
    case AstInterface::UOP_CAST:
        return v;
    case AstInterface::UOP_DECR1:
        return new SymbolicFunction( opr, "--", v);
    case AstInterface::UOP_INCR1:
        return new SymbolicFunction( opr, "++", v);
    default:
       std::cerr << "Cannot handle " << AstInterface::AstToString(exp) << ":" << opr << "\n";
       ROSE_ABORT();
     }
  }
  else if (fa.IsFunctionCall(exp, &s1, &l) || fa.IsArrayAccess(exp, &s1, &l)) {
     bool ismin = fa.IsMin(s1), ismax = fa.IsMax(s1);
     AstInterface::AstNodeList::const_iterator p = l.begin();
     if (ismin || ismax) {
       AstNodePtr s = *p;
       SymbolicVal v = GetSymbolicVal( fa, s );
       for ( ++p; p != l.end(); ++p ) {
           s = *p;
           v = (ismin)? Min(v, GetSymbolicVal(fa, s))
                        : Max(v, GetSymbolicVal(fa, s));
       }
       return v;
     }
     if (fa.IsVarRef(s1, 0, &name)) {
        SymbolicFunction::Arguments args;
        for ( ; p != l.end(); ++p) {
           SymbolicVal cur = GetSymbolicVal(fa, *p);
           args.push_back( cur );
        }
        return new SymbolicFunction( AstInterface::OP_NONE, new SymbolicAstWrap(s1), args);
     }
  }
  else if (fa.IsVarRef(exp, 0, &name, &scope)) {
     return new SymbolicVar( name, scope, exp );
  }
  return new SymbolicAstWrap(exp);
}

