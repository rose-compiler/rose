
#include <cctype>
#include <string>
#include "OperatorDescriptors.h"

using namespace std;

extern bool DebugAnnot();
ReplaceParams::
ReplaceParams ( const ParamDescriptor& decl, AstInterface::AstNodeList& args,
                Map2Object<AstInterface*,AstNodePtr,AstNodePtr>* codegen)
{
  if (decl.size() != args.size()) {
    std::cerr << "Error: mismatching numbers of parameters and arguments in annotation." 
            << decl.size() << " vs " << args.size() << "\n";
    assert(false);
    throw MisMatchError();
  }
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
  if (DebugAnnot()) {
    std::cerr << "Error: Cannot find argument for parameter: " << varname << "\n";
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
  std::stringstream fname_stream;
  NameDescriptor::write(fname_stream, fname);
  if (!unique)
    for ( AstInterface::AstTypeList::const_iterator p = plist.begin();
          p != plist.end();  ++p) {
      AstNodeType t = *p;
      string name;
      fa.GetTypeInfo( t, &name);
      fname_stream << "_";
      NameDescriptor::write(fname_stream, name);
    }
  return fname_stream.str();
}

std::string OperatorDeclaration::operator_signature(AstInterface& fa, 
                                 const AstNodePtr& exp, 
                                 AstInterface::AstNodeList* argp,
                                 AstInterface::AstTypeList* paramp) {
    if (DebugAnnot()) {
      std::cerr << "Creating operator signature:" << fa.AstToString(exp) << "\n";
    }
    std::string fname;
    AstInterface::AstTypeList params;
    AstNodePtr f;
    AstNodeType t;
    if (fa.IsVarRef(exp,&t,&fname, 0, 0, true) && fa.IsFunctionType(t, &params)) {
       if (DebugAnnot()) {
          std::cerr << "Generating signature from function type for: " << fname << "\n";
       }
       assert(argp == 0);
    } else if (fa.IsFunctionCall(exp, &f, argp, 0, &params) && fa.IsVarRef(f,0,&fname)
       || fa.IsFunctionDefinition(exp,&fname,argp,0,0, &params)) { 
       if (DebugAnnot()) {
          std::cerr << "Generating signature from function call or definition for: " << fname << "\n";
       }
    } else {
      std::cerr << "Error: expecting a function but get: " << fa.AstToString(exp) << "\n";
      assert(0);
    }
    if (paramp != 0) {
      *paramp = params;
    }
    std::string sig = OperatorDeclaration::get_signiture(fa, fname, params);
    return sig;
  }

OperatorDeclaration:: OperatorDeclaration(AstInterface& fa, AstNodePtr op_ast) {
    AstInterface::AstTypeList params;
    AstInterface::AstNodeList args;
    signiture = operator_signature(fa, op_ast, &args, &params);
    assert(params.size() == args.size());
    AstInterface::AstNodeList::const_iterator p1 = args.begin();
    AstInterface::AstTypeList::const_iterator p2 = params.begin(); 
    while (p2 != params.end() && p1 != args.end()) {
       pars.add_param(AstInterface::GetTypeName(*p2), AstInterface::GetVarName(*p1));
       ++p1; ++p2;
    }
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

