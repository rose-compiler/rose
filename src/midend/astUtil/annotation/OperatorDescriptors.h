
#ifndef ___OPERATION_DESCRIPTORS_H
#define ___OPERATION_DESCRIPTORS_H

#include "AnnotExpr.h"
#include "FunctionObject.h"

bool DebugAnnot();
//!Paramter names
typedef ContainerDescriptor<std::vector<NameDescriptor>, NameDescriptor,',','(',')'>
    ParamDescriptor;
//!Parameter types    
typedef ContainerDescriptor<std::vector<TypeDescriptor>, TypeDescriptor,',','(',')'>
    ParamTypeDescriptor;
//! Representation for function parameter names and types.
class ParameterDeclaration
{
  ParamDescriptor pars;
  ParamTypeDescriptor parTypes;
public:
  std::string get_param(int i) const { return pars[i]; }
  std::string get_param_type( int i) const { return parTypes[i]; }
  unsigned num_of_params() const { return pars.size(); }

  const ParamDescriptor& get_params() const { return pars; }
  const ParamTypeDescriptor& get_param_types() const { return parTypes; }

  void add_param( const std::string& type, const std::string& name) 
    { 
      pars.push_back( NameDescriptor(name));
      parTypes.push_back( TypeDescriptor(type));
    }

  void push_back( const SelectPair<TypeDescriptor, NameDescriptor, 0, 0>& cur)
   { add_param( cur.first, cur.second); }
  bool read( std::istream& in) 
   { 
      return ReadContainer<ParameterDeclaration, 
                           CollectPair<TypeDescriptor, NameDescriptor,0>,
                           ',', '(', ')'>::read(*this, in);
   }
  void write( std::ostream& out) const { pars.write(out); }
  void Dump() const { write(std::cerr); }
  std::string ToString() const { std::stringstream out; write(out); return out.str(); }
};

class ReplaceParams 
  : public MapObject<SymbolicVal, SymbolicVal>, public SymbolicVisitor
{
  virtual void VisitVar( const SymbolicVar &v);
  virtual SymbolicVal operator()( const SymbolicVal& v); 

  std::map<std::string, SymbolicAstWrap> parmap;
  SymbolicVal cur;
public:
  class MisMatchError {};
  ReplaceParams() {}
  ReplaceParams(const ParamDescriptor& decl,
                AstInterface::AstNodeList& args, 
                Map2Object<AstInterface*,AstNodePtr,AstNodePtr>* codegen = 0);
  void add( const std::string& par, const AstNodePtr& arg,
                Map2Object<AstInterface*,AstNodePtr,AstNodePtr>* codegen = 0)
        { parmap[par] = SymbolicAstWrap(arg, codegen); }
  void operator() (SymbolicValDescriptor& result);
  SymbolicAstWrap find( const std::string& parname);
};
//! Representation for an operator (function)
class OperatorDeclaration {
  std::string signiture; //operator name
  ParameterDeclaration pars; // parameter names and types
  static bool unique;
public:
  OperatorDeclaration() : signiture("") {}
  OperatorDeclaration(AstInterface& fa, AstNodePtr op_ast);
  
  std::string get_signiture () const { return signiture; }
  static std::string get_signiture( AstInterface& fa, const std::string& fname,
                                    const AstInterface::AstTypeList& params);
  static std::string operator_signature(AstInterface& fa, 
                                 const AstNodePtr& exp, 
                                 AstInterface::AstNodeList* argp= 0,
                                 AstInterface::AstTypeList* paramp = 0); 
  static void set_unique() { unique = true; }
  
  const ParameterDeclaration& get_param_info() const { return pars; }
  
  OperatorDeclaration& read( std::istream& in);
  void write( std::ostream& out) const;
  void Dump() const { write(std::cerr); }
  std::string ToString() const { std::stringstream out; write(out); return out.str(); }
};

template <class BaseClass>
class OPDescriptorTemp : public BaseClass
{
 protected:
  ParameterDeclaration decl;
 public:
  const ParameterDeclaration& get_param_decl() const { return decl; }
  void set_param_decl(const ParameterDeclaration& d) { decl = d; }
  void replace_val(MapObject<SymbolicVal, SymbolicVal>&) {}

  bool read( std::istream& in, const OperatorDeclaration& op)
    {
      decl = op.get_param_info();
      return BaseClass::read(in);
    }
   void write( std::ostream& out) const
   { 
     decl.write(out); 
     out << ":"; 
     BaseClass::write(out);
   }
  void Dump() const { write(std::cerr); }
  void collect(AstInterface& fa, AstInterface::AstNodeList& args,
                       CollectObject<AstNodePtr>& collect_f) { }
};
//! a set of names separated by ',', begin with '(', end with ')'
typedef  SetDescriptor<NameDescriptor,',','(',')'> NameGroup; 
//! A container of std::list<> of elment of NameGroup type
typedef OPDescriptorTemp
         <ContainerDescriptor<std::list<NameGroup>, NameGroup,',','{','}'> > 
OperatorAliasDescriptor;

class OperatorSideEffectDescriptor
: public OPDescriptorTemp <SetDescriptor<NameDescriptor, ',','{','}'> >              
{
  typedef OPDescriptorTemp <SetDescriptor<NameDescriptor, ',','{','}'> >
    BaseClass;
 public:
  OperatorSideEffectDescriptor() {}
  using BaseClass::read;

  // Whether the given parameter is part of the side effect description.
  bool contains_parameter_in_annotation(int index) const
    { 
      NameDescriptor n (decl.get_param(index));
      return find(n) != end();  
    }
  bool contains_global_variables_in_annotation() const {
      size_t param_num = 0;
      for (size_t i = 0; i < decl.num_of_params(); ++i) {
         if (contains_parameter_in_annotation(i))
         ++param_num;
      }
      // Returns true if annotation contains more than function parameters.
      return size() > param_num;
    }
  void collect(AstInterface& fa, AstInterface::AstNodeList& args,
                       CollectObject<AstNodePtr>& collect_f) {
    ReplaceParams paramMap( get_param_decl().get_params(), args);
    for (const_iterator p = begin(); p != end(); ++p) {
      std::string varname = *p;
      if (DebugAnnot()) {
        std::cerr << "Side effect Descriptor collecting " << varname << "\n";
      }
      AstNodePtr arg = paramMap.find(varname).get_ast();
      if (arg != AST_NULL) {  // if it is one of the function arguments, collect it
        if (DebugAnnot()) {
           std::cerr << "Side effect Descriptor varname " << varname << " is a parameter\n";
        }
        collect_f( arg);
      }
      else { // otherwise, it is a global variable, create a reference to it.
        if (DebugAnnot()) {
           std::cerr << "Side effect Descriptor varname " << varname << " is a global variable\n";
        }
        AstNodePtr var = fa.CreateVarRef(varname);
        collect_f( var);
      }
    }
  }
};

class OperatorInlineDescriptor 
  : public OPDescriptorTemp
          < CloseDescriptor< SymbolicValDescriptor, '{', '}'> >
{
  typedef OPDescriptorTemp
          < CloseDescriptor< SymbolicValDescriptor, '{', '}'> > 
   BaseClass;
 public:
   void replace_val( MapObject<SymbolicVal, SymbolicVal>& repl)
    {
       CloseDescriptor< SymbolicValDescriptor, '{', '}'>::replace_val(repl);
    }
};


#endif






