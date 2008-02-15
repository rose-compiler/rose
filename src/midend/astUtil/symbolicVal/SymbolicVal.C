#include <general.h>

#include <SymbolicVal.h>

#include <SymbolicMultiply.h>
#include <SymbolicPlus.h>
#include <SymbolicSelect.h>

#include <stdio.h>
#include <sstream>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/8/2006): Since this is not used in a heade file it is OK here!
#define Boolean int

void SymbolicValImpl :: Dump() const
{ cerr << ToString(); }


SymbolicVar::SymbolicVar( std::string name, const AstNodePtr& sc)
   : varname(name), scope(sc)
   {
#if 0
     printf ("In SymbolicVar: name = %s scope->class_name() = %s \n",name.c_str(),scope.get_ptr()->class_name().c_str());
     if (name == "imax")
        {
          printf ("imax found what is the scope! \n");
       // assert(false);
        }
#endif
   }

void SymbolicVal::Dump() const
{ cerr << ToString(); }

string SymbolicVal::ToString() const
{ 
   return (ConstPtr() != 0)? ConstRef().ToString() : "";
}

SymbolicVal ::SymbolicVal (int val)
  : CountRefHandle <SymbolicValImpl>( new SymbolicConst(val) ) {}


SymbolicConst::  SymbolicConst( int _val)
        : val ( ""), type("int"), intval( _val)
     { 
        char buf[20];
        sprintf( buf, "%d", _val);
        val = buf;
     }

SymbolicConst:: SymbolicConst( string _val, string t) : val(_val), type(t)
 {
    if (type == "int") {
      intval = atoi(_val.c_str());
    }
 }

string SymbolicConst :: ToString() const
{
     return val;
}

AstNodePtr  SymbolicConst :: CodeGen(AstInterface &fa) const
{
  if (type == "int")
    return fa.CreateConstInt(intval);
  else
    return fa.CreateConst( val, type);
}

string SymbolicVar :: ToString() const
{
 return varname;
}

AstNodePtr  SymbolicVar :: CodeGen( AstInterface &fa) const
{
  return fa.CreateVarRef(varname, scope);
}

AstNodePtr SymbolicAstWrap::CodeGen( AstInterface &fa) const
      { 
         if (codegen == 0)
             return fa.CopyAstTree(ast);
         else
             return (*codegen)(&fa, ast);
      }

string SymbolicAstWrap::ToString() const
{
   return AstInterface::AstToString(ast);
}

void SymbolicAstWrap::Dump() const
{
   AstInterface::DumpAst(ast);
   if (codegen != 0)
       cerr << "(codegen = " << codegen << ")\n";
}

Boolean SymbolicVar :: operator == (const SymbolicVar &that) const
{
  return varname == that.varname && ( scope == that.scope || scope == 0 || that.scope == 0);
}


string SymbolicFunction :: ToString() const
{
  string r = "(";
  if (args.size() == 0)
      r= "()";
  for (unsigned int i = 0; i + 1 < args.size(); ++i) {
      r = r + args[i].ToString() + ",";
   }
   r = r + args[args.size()-1].ToString() + ")";
   string r2 = op.ToString();
   return r2 + r;
}

Boolean SymbolicFunction:: operator == (const SymbolicFunction& that) const
{  
  if (args.size() != that.args.size())
    return false;
  if (op != that.op)
      return false;
  for (unsigned i = 0; i < args.size(); ++i) {
    if (args[i] != that.args[i])
      return false;
  } 
  return true;
}

AstNodePtr SymbolicFunction :: CodeGen( AstInterface &_fa) const
{
  AstInterface::AstNodeList l = _fa.CreateList();
  for (unsigned i = 0; i < args.size(); ++i) {
     SymbolicVal cur = args[i];
     AstNodePtr curast = cur.CodeGen(_fa); 
     _fa.ListAppend(l, curast);
  }
  AstNodePtr f = op.CodeGen(_fa);
  if (f != 0)
      return _fa.CreateFunctionCall( f, l);
  string opname = op.ToString();
  if (args.size() == 2) {
      return _fa.CreateBinaryOP( opname, l.front(), l.back());
  }
  else if (args.size() == 1) {
      return _fa.CreateUnaryOP( opname, l.front());
  }
  else
     assert(false);
}

AstNodePtr SymbolicSelect:: CodeGen(  AstInterface &fa ) const
   {
      int size = 0;
      AstInterface::AstNodeList list = fa.CreateList();
      for (OpdIterator iter = GetOpdIterator(); !iter.ReachEnd(); iter.Advance()) {
           AstNodePtr p = Term2Val(iter.Current()).CodeGen(fa);
           fa.ListAppend(list, p);
           ++size;
      }
      assert( size > 1);
      string func = (opt< 0)? "min" : "max";
      stringstream f, decl;
      f << func << size;
      decl << "int " << func << size << "(int a0";
      for (int i = 1; i < size; ++i)
          decl << ",int a" << i;
      decl << ")"; 
      if (size == 2) {
        decl << ((func == "min")? " { return (a0 < a1)? a0 : a1; }" : "{ return (a0 < a1)? a1 : a0; }");
      }
      else {
        decl << "{\n" << "  int res = a0;\n";
        for (int i = 1; i < size; ++i) {
           decl << ((func == "min")? "    if (res > a" : "     if (res < a");
           decl << i << ") res = a" << i << ";\n";
        }
        decl << " return res; \n  }";
      }
      return fa.CreateFunctionCall(f.str(), decl.str(), list);
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


string RelToString( CompareRel r)
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
    assert(false);
  }
}


string SymbolicCond :: ToString() const
{
  string r = val1.ToString() + RelToString(GetRelType()) + val2.ToString();
  return r;
}

void SymbolicCond:: Dump() const
{
  cerr << ToString();
}

AstNodePtr  SymbolicCond :: CodeGen(AstInterface &fa) const
{
  switch (GetRelType()) {
  case REL_EQ:
     return fa.CreateRelEQ( val1.CodeGen(fa), val2.CodeGen(fa));
  case REL_NE:
     return fa.CreateRelNE( val1.CodeGen(fa), val2.CodeGen(fa));
  case REL_LT:
    return fa.CreateRelLT( val1.CodeGen(fa), val2.CodeGen(fa));
  case REL_LE:
     return fa.CreateRelLE( val1.CodeGen(fa), val2.CodeGen(fa));
  case REL_GT:
     return fa.CreateRelGT( val1.CodeGen(fa), val2.CodeGen(fa));
  case REL_GE:
     return fa.CreateRelGE( val1.CodeGen(fa), val2.CodeGen(fa));
  default:
     assert(false);
  }
}

AstNodePtr SymbolicMultiply::
CodeGenOP( AstInterface &fa, const AstNodePtr& a1, const AstNodePtr& a2) const
  { 
    int val = 0;
    if (fa.IsConstInt(a1, &val) && val == -1)
        return fa.CreateUnaryOP("-", a2);
    else if (fa.IsConstInt(a2, &val) && val == -1)
        return fa.CreateUnaryOP("-", a1);
    return fa.CreateBinaryTimes( a1, a2); 
  }

AstNodePtr SymbolicPlus::
CodeGenOP( AstInterface &fa, const AstNodePtr& a1, const AstNodePtr& a2) const
 { 
    AstNodePtr opd;
    if (fa.IsUnaryMinus(a2, &opd)) {
      fa.ReplaceAst(opd, fa.CreateConstInt(0));
      return fa.CreateBinaryMinus(a1, opd);
    }
    else if (fa.IsUnaryMinus(a1, &opd)) {
      fa.ReplaceAst(opd, fa.CreateConstInt(0));
      return fa.CreateBinaryMinus(a2, opd);
    }
    return fa.CreateBinaryPlus( a1, a2); 
 }

SymbolicVal SymbolicValGenerator ::
GetSymbolicVal( AstInterface &fa, const AstNodePtr& exp)
{
  string name;
  AstNodePtr scope;
  int val = 0;
  AstNodePtr s1, s2;
  AstInterface::AstNodeList l;

// printf ("exp = %p = %s \n",exp.get_ptr(),exp.get_ptr()->class_name().c_str());

  if (fa.IsVarRef(exp, 0, &name, &scope)) {
  // assert(scope.get_ptr() != NULL);
  // printf ("scope = %p = %s \n",scope.get_ptr(),scope.get_ptr()->class_name().c_str());
     return new SymbolicVar( name, scope );
  }
  else if (fa.IsConstInt(exp, &val)) {
     return new SymbolicConst( val );
  }
  else if (fa.IsBinaryTimes(exp, &s1, &s2)) {
     SymbolicVal v1 = GetSymbolicVal( fa, s1 ), v2 = GetSymbolicVal(fa, s2);
     return v1 * v2;
  }
  else if (fa.IsBinaryPlus(exp, &s1, &s2)) {
    SymbolicVal v1 = GetSymbolicVal( fa, s1 );
    SymbolicVal v2 = GetSymbolicVal( fa, s2 );
    return v1 +  v2;
  }
  else if (fa.IsBinaryMinus(exp, &s1, &s2)) {
    SymbolicVal v1 = GetSymbolicVal( fa, s1 );
    SymbolicVal v2 = GetSymbolicVal( fa, s2 );
    return v1 - v2;
  }
  else if (fa.IsUnaryMinus(exp, &s1)) {
    SymbolicVal v = GetSymbolicVal( fa, s1);
    return (-1) * v; 
  }
  else if (fa.IsFunctionCall(exp, &s1, &l)) {
     AstInterface::AstNodeListIterator p = fa.GetAstNodeListIterator(l); 

     string name = fa.GetFunctionName(s1);
     Boolean ismin =  (name == "min") || (name == "MIN");
     Boolean ismax = ( name == "max") || ( name == "MAX");
     if (ismin || ismax) {
       AstNodePtr s = *p;
       SymbolicVal v = GetSymbolicVal( fa, s );  
       for ( ++p; !p.ReachEnd(); ++p ) {
           s = *p;
           v = (ismin)? Min(v, GetSymbolicVal(fa, s)) 
                        : Max(v, GetSymbolicVal(fa, s));
       }
       return v;
     }
     SymbolicFunction::Arguments args;
     for ( ; !p.ReachEnd(); ++p) { 
        SymbolicVal cur = GetSymbolicVal(fa, *p);
        args.push_back( cur );
     }
     return new SymbolicFunction( GetSymbolicVal(fa, s1), args);
  } 
  else if (fa.IsBinaryOp( exp, &s1, &s2, &name) && name != "")
     return new SymbolicFunction( name, GetSymbolicVal(fa, s1), GetSymbolicVal(fa, s2));
  else
    return new SymbolicAstWrap(exp);
}



