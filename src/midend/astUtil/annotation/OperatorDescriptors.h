
#ifndef ___OPERATION_DESCRIPTORS_H
#define ___OPERATION_DESCRIPTORS_H

#include "TypeAnnotation.h"
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
  SymbolicVal cur;
public:
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
  
  std::string get_signiture () const { return signiture; }
  static std::string get_signiture( AstInterface& fa, const std::string& fname,
                                    const AstInterface::AstTypeList& params);
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
  void replace_val( MapObject<SymbolicVal, SymbolicVal>& repl) {}

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
  int param_num;
  typedef OPDescriptorTemp <SetDescriptor<NameDescriptor, ',','{','}'> >
    BaseClass;
 public:
  OperatorSideEffectDescriptor() : param_num(0) {}
  bool contain_parameter(int index) const
    { 
      NameDescriptor n (decl.get_param(index));
      return find(n) != end();  
    }
  bool contain_global() const { return size() - param_num > 0; }
  void get_side_effect(AstInterface& fa, AstInterface::AstNodeList& args,
                       CollectObject<AstNodePtr>& collect);
  bool read( std::istream& in, const OperatorDeclaration& op);
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






