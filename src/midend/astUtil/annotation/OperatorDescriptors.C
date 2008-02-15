#include <general.h>
#include <OperatorDescriptors.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

extern bool DebugAnnot();
ReplaceParams:: 
ReplaceParams ( const ParamDescriptor& decl, AstInterface::AstNodeList& args,
                Map2Object<AstInterface*,AstNodePtr,AstNodePtr>* codegen)
{
  assert(decl.size() == args.size());
  int index = 0;
  for (AstInterface::AstNodeListIterator p1 = AstInterface::GetAstNodeListIterator(args);
       !p1.ReachEnd(); ++p1, ++index) {
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
  if (ast.get_ast() != 0) 
    cur = ast;
}

void ReplaceParams::operator()( SymbolicValDescriptor& v) 
{
  v.replace_val( *this);
}

string OperatorDeclaration:: get_signature( const AstNodePtr& opDef)
{
  AstNodePtr decl = AstInterface::GetFunctionDecl(opDef);
  if (decl == 0) {
     return "unknown";
  }
  string r;
  AstInterface::AstTypeList plist;
  AstInterface::IsFunctionDecl(decl, &r, 0, &plist);
  for ( AstInterface::AstTypeListIterator p 
           = AstInterface::GetAstTypeListIterator(plist);
        !p.ReachEnd(); ++p) {
    AstNodeType t = *p;
    string name;
    AstInterface::GetTypeInfo( t, &name); 
    r = r + "_" + name;
  }
  return r;
}

OperatorDeclaration& OperatorDeclaration:: read ( istream& in )
   {
      signature = read_id(in);

      string classname, funcname;

      char c = peek_ch(in);
      if (c == ':') {
        classname = signature;
        read_ch(in, ':');
        read_ch(in, ':');
        signature = signature + "::";
        c = peek_ch(in);
      }
      while ( in.good() && c != '(') {
         read_ch(in,c);
         signature.push_back(c);
         funcname.push_back(c);
         c = peek_ch(in);
      }

      if (signature.rfind(":operator") == signature.size() - 9) {
         signature += "()";
         read_ch(in,'(');
         read_ch(in,')');
      }


      int index = 0;
      if (classname != "" && classname != funcname) {
        index = 1;
        pars.add_param( signature, "this");
      }
      pars.read(in);
      for (unsigned i = index; i < pars.num_of_params(); ++i) {
         string partype = pars.get_param_type(i);
         signature = signature + "_" + partype;
      }
      return *this;
   }

void OperatorDeclaration:: write( ostream& out) const
   {
      out << get_signature();
      pars.write(out);
   }

bool OperatorSideEffectDescriptor::read( istream& in, const OperatorDeclaration& op)
{ 
  param_num = 0;
  if (BaseClass::read(in, op)) {
    for (unsigned int i = 0; i < decl.num_of_params(); ++i) {
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
      if (arg != 0) {
        collect( arg);
      }
      else {
        AstNodePtr var = fa.CreateVarRef(varname);
        collect( var);
      }
  }
}

// DQ (1/8/2006): force instantiation of this template so that the "read" member function will be available (required for g++ 4.0.2)
// template class ReadContainer<ParameterDeclaration, CollectPair<TypeDescriptor, NameDescriptor,0>, ',', '(', ')'>;

// template bool ReadContainer<ParameterDeclaration, CollectPair<TypeDescriptor, NameDescriptor, (char)0>, (char)44, (char)40, (char)41>::read(ParameterDeclaration&, std::basic_istream<char, std::char_traits<char> >&);
