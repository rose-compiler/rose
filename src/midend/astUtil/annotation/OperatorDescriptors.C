

#include "OperatorDescriptors.h"
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */
#include <string.h>

using namespace std;

extern bool DebugAnnot();
ReplaceParams::
ReplaceParams ( const ParamDescriptor& decl, AstInterface::AstNodeList& args,
                Map2Object<AstInterface*,AstNodePtr,AstNodePtr>* codegen)
{
  assert(decl.size() == args.size());
  int index = 0;
  for (AstInterface::AstNodeList::iterator p1 = args.begin();
       p1 != args.end(); ++p1, ++index) {
    AstNodePtr curAst = *p1;
    string curpar = decl[index];
    SymbolicAstWrap curarg(curAst, codegen);
    parmap[curpar] = curarg;
  }
}

SymbolicVal ReplaceParams::operator()( const SymbolicVal& v)
{
  cur = SymbolicVal();
  v.Visit(this);
  return cur;
}

SymbolicAstWrap ReplaceParams:: find( const string& varname)
{
  map<string,SymbolicAstWrap>::const_iterator p = parmap.find(varname);
  if (p != parmap.end()) {
    return (*p).second;
  }
  return SymbolicAstWrap();
}

void ReplaceParams:: VisitVar( const SymbolicVar &var)
{
  string varname = var.GetVarName();
  SymbolicAstWrap ast = find(varname);
  if (ast.get_ast() != AST_NULL)
    cur = ast;
}

void ReplaceParams::operator()( SymbolicValDescriptor& v)
{
  v.replace_val( *this);
}

bool OperatorDeclaration::unique = false;
//! Get a unique string name for a type, similar to qualified names in C++
string OperatorDeclaration::
get_signiture( AstInterface& fa, const std::string& fname,
                                    const AstInterface::AstTypeList& plist)
{
  std::string r = fname;
  if (!unique)
    for ( AstInterface::AstTypeList::const_iterator p = plist.begin();
          p != plist.end();  ++p) {
      AstNodeType t = *p;
      string name;
      fa.GetTypeInfo( t, &name);
      r = r + "_" + name;
    }
  return r;
}
//! Read in an operator (function) declaration: name + a list of parameter types and names)
OperatorDeclaration& OperatorDeclaration:: read ( istream& in )
   {
      // Signature is the full function name, possibly with several qualifiers
      signiture = read_id(in);

      string classname, funcname;

      char c = peek_ch(in);
      if (c == ':') {
        classname = signiture;
        read_ch(in, ':');
        read_ch(in, ':');
        signiture = signiture + "::";
        c = peek_ch(in);
      }
      // Plus other special characters in the operator's name, such as <=, *,~
      while ( in.good() && c != '(') {
         read_ch(in,c);
         signiture.push_back(c);
         funcname.push_back(c);
         c = peek_ch(in);
      }
      // Append () for "::operator()" ?
      const char* opstart = strrchr(signiture.c_str(), ':');
      if (opstart != 0 && string(opstart+1) == "operator") {
         signiture = signiture + "()";
         read_ch(in,'(');
         read_ch(in,')');
      }

      // Read in the parameter declaration: a list of (type, name)
      int index = 0;
      if (classname != "" && classname != funcname) {
        index = 1;
        pars.add_param( signiture, "this");
      }
      pars.read(in);


      for (unsigned i = index; i < pars.num_of_params(); ++i) {
         string partype = pars.get_param_type(i);
         if (!unique)
            signiture = signiture + "_" + partype;
      }
      return *this;
   }

void OperatorDeclaration:: write( ostream& out) const
   {
      out << get_signiture();
      pars.write(out);
   }

bool OperatorSideEffectDescriptor::read( istream& in, const OperatorDeclaration& op)
{
  param_num = 0;
  if (BaseClass::read(in, op)) {
    for (size_t i = 0; i < decl.num_of_params(); ++i) {
      if (contain_parameter(i))
        ++param_num;
    }
    return true;
  }
  return false;
}

void OperatorSideEffectDescriptor::
get_side_effect( AstInterface& fa,
                 AstInterface::AstNodeList& args, CollectObject< AstNodePtr >& collect)
{
  ReplaceParams paramMap( get_param_decl().get_params(), args);
  for (OperatorSideEffectDescriptor::const_iterator p = begin(); p != end(); ++p) {
      string varname = *p;
      AstNodePtr arg = paramMap.find(varname).get_ast();
      if (arg != AST_NULL) {  // if it is one of the function arguments, collect it
        collect( arg);
      }
      else { // otherwise, it is a global variable, create a reference to it.
        AstNodePtr var = fa.CreateVarRef(varname);
        collect( var);
      }
  }
}

// DQ (1/8/2006): force instantiation of this template so that the "read" member function will be available (required for g++ 4.0.2)
// template class ReadContainer<ParameterDeclaration, CollectPair<TypeDescriptor, NameDescriptor,0>, ',', '(', ')'>;

// template bool ReadContainer<ParameterDeclaration, CollectPair<TypeDescriptor, NameDescriptor, (char)0>, (char)44, (char)40, (char)41>::read(ParameterDeclaration&, std::basic_istream<char, std::char_traits<char> >&);
