
#include <cctype>
#include <string>
#include "OperatorDescriptors.h"
#include "CommandOptions.h"
#include "AstInterface.h"

using namespace std;

DebugLog DebugOperatorDescriptor("-debugannot");
ReplaceParams::
ReplaceParams ( const ParameterDeclaration& decl, const AstInterface::AstNodeList& args,
                Map2Object<AstInterface*,AstNodePtr,AstNodePtr>* codegen)
{
  if (decl.get_params().size() < args.size()) {
    std::cerr << "Error: mismatching numbers of parameters and arguments in annotation." 
            << decl.get_params().size() << " vs " << args.size() << "\n";
    assert(false);
    throw MisMatchError();
  }
  int index = 0;
  for (AstInterface::AstNodeList::const_iterator p1 = args.begin();
       p1 != args.end(); ++p1, ++index) {
    AstNodePtr curAst = *p1;
    string curpar = decl.get_params()[index];
    SymbolicAstWrap curarg(curAst, codegen);
    parmap[curpar] = curarg;
    partypemap[curpar] = decl.get_param_types()[index];
    DebugOperatorDescriptor([&curpar, &curarg](){ return "Operator parameter " + curpar + "->" + curarg.toString(); });
  }
  if (decl.get_params().size() > args.size()) {
    std::cerr << "Error: mismatching numbers of parameters and arguments in annotation." 
            << decl.get_params().size() << " vs " << args.size() << "\n";
    assert(false);
    throw MisMatchError();
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
  DebugOperatorDescriptor([&varname](){ return "Cannot find argument for parameter: " + varname + ". Returning empty!"; });
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

// Returns signature for exp. Modifies argp with parameter values if exp is a function call.
std::string OperatorDeclaration::operator_signature( const AstNodePtr& exp, 
                                 AstInterface::AstNodeList* argp,
                                 AstInterface::AstTypeList* paramp) {
    DebugOperatorDescriptor([&exp](){ return "Creating operator signature:" + AstInterface::AstToString(exp); });
    std::string fname;
    AstNodePtr f;
    AstNodeType t;
    AstTypeList params;
    if (paramp == 0) paramp = &params;
    if (AstInterface::IsVarRef(exp,&t,&fname, 0, 0, /*use_globl_name=*/true) && AstInterface::IsFunctionType(t, paramp)) {
       if (argp != 0 && paramp->size() == 1) {
          argp->push_back(exp.get_ptr());
       }  
    }
    else if ((AstInterface::IsFunctionCall(exp, &f, argp, 0, paramp) && AstInterface::IsVarRef(f,0,&fname, 0, 0, /*use_globl_name=*/true)) || 
         AstInterface::IsFunctionDefinition(exp,&fname,argp,0,0, paramp, 0, /*use_globl_name=*/true)) { 
    } else {
      DebugOperatorDescriptor([&exp](){ return "Unexpected operator: not recognized:" + AstInterface::AstToString(exp) + ". Return empty name."; });
      return "";
    }
    // paramp is guaranteed to not be null (see above).
    if (argp != 0 && paramp->size() != argp->size()) {
      DebugOperatorDescriptor([&exp](){ return "Unexpected operator: not recognized:" + AstInterface::AstToString(exp) + ". Return empty name."; });
      return "";
    }
    return fname;
  }

OperatorDeclaration:: OperatorDeclaration(AstInterface& fa, AstNodePtr op_ast,
                                  AstInterface::AstNodeList* argp) {
    AstInterface::AstTypeList params;
    AstInterface::AstNodeList args;
    if (argp == 0) { argp = &args; }
    TypeDescriptor::get_name() = operator_signature(op_ast, argp, &params);
    if (TypeDescriptor::get_name() == "") {
        DebugOperatorDescriptor([&op_ast](){ return "Error: Unknown/inconsistent operation: " + AstInterface::AstToString(op_ast) + ". Generating empty declaration."; });
        return;
    } 
    AstInterface::AstNodeList::const_iterator p1 = argp->begin();
    AstInterface::AstTypeList::const_iterator p2 = params.begin(); 
    while (p2 != params.end() && p1 != argp->end()) {
       DebugOperatorDescriptor([&fa, &p1](){ return "Adding operator parameter:" + AstInterface::AstToString(*p1); });
       pars.add_param(fa.GetTypeName(*p2), AstInterface::GetVariableSignature(*p1));
       ++p1; ++p2;
    }
    assert(params.size() == pars.num_of_params());
    if (params.size() != argp->size()) {
       std::cerr << "Error: mismatching numbers of parameters and arguments in annotation." 
            << params.size() << " vs " << argp->size() << "\n";
        assert(false);
    }
}

//! Read in an operator (function) declaration: name + a list of parameter types and names)
OperatorDeclaration& OperatorDeclaration:: read ( istream& in )
   {
      // Signature is the full function name, possibly with several qualifiers
      std::string signiture = read_id(in);

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
         signiture = signiture + "_" + partype;
      }
      TypeDescriptor::get_name() = signiture;
      return *this;
   }

