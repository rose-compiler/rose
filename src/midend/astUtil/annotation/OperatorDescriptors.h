
#ifndef ___OPERATION_DESCRIPTORS_H
#define ___OPERATION_DESCRIPTORS_H

#include "AnnotExpr.h"
#include "FunctionObject.h"


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
  std::map<std::string, TypeDescriptor> partypemap;
  SymbolicVal cur;
public:
  class MisMatchError {};
  ReplaceParams(const ParameterDeclaration& decl,
                const AstInterface::AstNodeList& args, 
                Map2Object<AstInterface*,AstNodePtr,AstNodePtr>* codegen = 0);
  void add( const std::string& par, const AstNodePtr& arg,
                Map2Object<AstInterface*,AstNodePtr,AstNodePtr>* codegen = 0)
        { parmap[par] = SymbolicAstWrap(arg, codegen); }
  void operator() (SymbolicValDescriptor& result);
  SymbolicAstWrap find( const std::string& parname);
};
//! Representation for an operator (function)
class OperatorDeclaration : public TypeDescriptor {
  ParameterDeclaration pars; // parameter names and types
public:
  OperatorDeclaration() {}
  OperatorDeclaration(AstInterface& fa, AstNodePtr op_ast, AstInterface::AstNodeList* argp= 0);
  
  std::string get_signiture () const { return TypeDescriptor::get_string(); }
  static std::string operator_signature(const AstNodePtr& exp, 
                                 AstInterface::AstNodeList* argp= 0,
                                 AstInterface::AstTypeList* paramp = 0); 
  const ParameterDeclaration& get_param_info() const { return pars; }
  OperatorDeclaration& read( std::istream& in);
};

template <class BaseClass>
class OPDescriptorTemp : public BaseClass
{
 protected:
  ParameterDeclaration decl;
  bool has_unknown_ = false;
 public:
  const ParameterDeclaration& get_param_decl() const { return decl; }
  void set_param_decl(const ParameterDeclaration& d) { decl = d; }

  bool get_has_unknown() const { return has_unknown_; }
  void set_has_unknown(bool hasit = true) { has_unknown_ = hasit; }
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
  ReplaceParams GenReplaceParams(const AstInterface::AstNodeList& args, const AstNodePtr& exp,
              Map2Object<AstInterface*, AstNodePtr, AstNodePtr>* astcodegen =0) {
    ReplaceParams paramMap( get_param_decl(), args, astcodegen);
    return paramMap;
  }
  void replace_val(MapObject<SymbolicVal, SymbolicVal>&) {}
};
//! a set of names separated by ',', begin with '(', end with ')'
typedef  SetDescriptor<NameDescriptor,',','(',')'> NameGroup; 
//! A container of std::list<> of elment of NameGroup type
typedef OPDescriptorTemp
         <ContainerDescriptor<std::list<NameGroup>, NameGroup,',','{','}'> > 
OperatorAliasDescriptor;

class OperatorSideEffectDescriptor
: public OPDescriptorTemp <SetDescriptor<SymbolicValDescriptor, ',','{','}'> >              
{
  typedef OPDescriptorTemp <SetDescriptor<SymbolicValDescriptor, ',','{','}'> >
    BaseClass;
 public:
  OperatorSideEffectDescriptor() {}
  using BaseClass::read;
  typedef SymbolicValDescriptor BaseDescriptor;
  typedef SetDescriptor<BaseDescriptor, ',','{','}'>::const_iterator const_iterator;
  using SetDescriptor<BaseDescriptor, ',','{','}'>::begin;
  using SetDescriptor<BaseDescriptor, ',','{','}'>::end;

  // Whether the given parameter is part of the side effect description.
  bool contains_parameter_in_annotation(int index) const
    { 
      BaseDescriptor n (SymbolicVar(decl.get_param(index), AST_NULL));
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
  template <class CollectObject>
  void collect(AstInterface& fa, AstInterface::AstNodeList& args,
                       CollectObject& collect_f,
                       Map2Object<AstInterface*, AstNodePtr, AstNodePtr>* astcodegen =0) {
    ReplaceParams paramMap = BaseClass::GenReplaceParams(args, astcodegen);
    for (const_iterator p = begin(); p != BaseClass::end(); ++p) {
      BaseDescriptor exp = (*p), arg = exp;
      arg.replace_val(paramMap);
      collect_f( arg.get_val().CodeGen(fa));
    }
    if (get_has_unknown()) {
       collect_f(AstNodePtr(AstNodePtr::SpecialAstTypes::UNKNOWN_AST)); 
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






