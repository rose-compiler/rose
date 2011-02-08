#ifndef ANNOT_EXPR_H
#define ANNOT_EXPR_H

#include "AnnotDescriptors.h"
#include "SymbolicVal.h"
#include "FunctionObject.h"
#include <stdlib.h>
#include <vector>
//! SymbolicVal with I/O interface
class SymbolicValDescriptor
{
  SymbolicVal val;
  void set_val( const SymbolicVal &v);
 public:
  SymbolicValDescriptor( int i) : val(i) {}
  SymbolicValDescriptor( const SymbolicVal& that) { set_val(that); }
  SymbolicValDescriptor() { set_val( SymbolicVal()); }
  void operator = (const SymbolicVal& that)  { set_val(that); }

  bool read( std::istream& in);

  operator SymbolicVal() const { return val; }
  SymbolicVal get_val() const { return val; }
  SymbolicVal& get_val() { return val; }
  void write(std::ostream& out) const;
  void Dump() const;
  std::string toString() const { std::stringstream out; write(out); return out.str(); }

  bool is_top() const;
  bool is_bottom() const;
  static SymbolicConst get_bottom();
  static SymbolicConst get_top();

  bool merge( const SymbolicValDescriptor& that);
  void replace_var( const std::string& varname, const SymbolicVal& val);
  void replace_val( MapObject<SymbolicVal, SymbolicVal>& repl);
  bool find( const SymbolicVal& v);
};

class SymbolicDotExp : public SymbolicFunction
{
  public:
   SymbolicDotExp(const Arguments& v)
    : SymbolicFunction(AstInterface::BOP_DOT_ACCESS, ".", v)  { assert( v.size() == 2); }
   SymbolicDotExp( const SymbolicVal& obj, const SymbolicVal& field)
    : SymbolicFunction(AstInterface::BOP_DOT_ACCESS,".", obj, field) {}
   SymbolicDotExp( const SymbolicVal& obj, const std::string& fieldname)
    : SymbolicFunction(AstInterface::BOP_DOT_ACCESS,".", obj, SymbolicConst(fieldname, "field")) {}
   SymbolicDotExp( const SymbolicDotExp& that)
    : SymbolicFunction(that) {}

  virtual AstNodePtr CodeGen( AstInterface& fa) const;
  virtual SymbolicValImpl* Clone() const
         { return new SymbolicDotExp(*this); }
  virtual SymbolicFunction* cloneFunction(const Arguments& args) const
         { return new SymbolicDotExp(args); }
};

class SymbolicFunctionPtrCall : public SymbolicFunction
{
  public:
   SymbolicFunctionPtrCall(const Arguments& v)
    : SymbolicFunction(AstInterface::OP_NONE, "FunctionPtrCall", v) {} 
   SymbolicFunctionPtrCall( const SymbolicFunctionPtrCall& that)
    : SymbolicFunction(that) {}

  virtual AstNodePtr CodeGen( AstInterface& fa) const;
  virtual SymbolicValImpl* Clone() const
         { return new SymbolicFunctionPtrCall(*this); }
  virtual SymbolicFunction* cloneFunction(const Arguments& args) const
         { return new SymbolicFunctionPtrCall(args); }
};

class SymbolicExtendVar : public SymbolicFunction
{
  SymbolicValImpl* Clone() const
   { return new SymbolicExtendVar(*this); }
  virtual SymbolicFunction* cloneFunction(const Arguments& v) const
         { return new SymbolicExtendVar( v); }

  public:
   SymbolicExtendVar(const Arguments& v)
    : SymbolicFunction(AstInterface::OP_NONE, "$", v)  
       { assert( v.size() == 2); }
   SymbolicExtendVar( const std::string& varname, int  index)
    : SymbolicFunction(AstInterface::OP_NONE,"$", SymbolicVar(varname, AST_NULL), index) {}
   SymbolicExtendVar( const SymbolicExtendVar& that)
    : SymbolicFunction(that) {}

   virtual AstNodePtr CodeGen( AstInterface& fa) const;
   static std::string get_varname( std::string var, int index);
};

//!  a parameter with a named range of values. parameter:range_name:lower_bound:upper_bound
// e.g:  _size:dim:1:dimension
class ExtendibleParamDescriptor 
  : private SelectPair < SymbolicValDescriptor, // parameter itself 
                         CollectPair<NameDescriptor, // a range's name followed by its lower and upper bound
                                     CollectPair<SymbolicValDescriptor,
                                                 SymbolicValDescriptor, ':'>,
                                     ':'>,
                         ':',0>
{
 typedef SelectPair < SymbolicValDescriptor,
                         CollectPair<NameDescriptor,
                                     CollectPair<SymbolicValDescriptor,
                                                 SymbolicValDescriptor, ':'>,
                                     ':'>,
                         ':', 0>
          BaseClass;
 SymbolicValDescriptor& get_ext_lb() { return second.second.first; }
 const SymbolicValDescriptor& get_ext_lb() const { return second.second.first; }
 SymbolicValDescriptor& get_ext_ub() { return second.second.second; }
 const SymbolicValDescriptor& get_ext_ub() const { return second.second.second; }
 
 public:
  ExtendibleParamDescriptor() {}
  ExtendibleParamDescriptor( const SymbolicValDescriptor& p)
       { first = p; }
  void write(std::ostream& out) const { BaseClass::write(out); }
  void Dump() const;
  std::string toString() const { std::stringstream out; write(out); return out.str(); }

  bool read(std::istream& in);
  const SymbolicValDescriptor& get_param() const { return first; }
  SymbolicValDescriptor& get_param() { return first; }
  std::string get_param_name() const 
        { assert(first.get_val().GetValType() == VAL_VAR); return first.get_val().toString(); }
  //! Get the name for the range        
  std::string get_extend_var() const { return second.first; }
  //! Get the lower bound and upper bound of the parameter
  bool get_extension( int& lb, int& ub) const; 

  void replace_var( const std::string& varname, const SymbolicVal& val);
  void replace_val(MapObject<SymbolicVal, SymbolicVal>& repl);
};
//! A list of parameter descriptors, separated by ',', enclosed in '(..)'
// Each parameter has a range of values
class SymbolicParamListDescriptor 
  : public ContainerDescriptor< std::vector <ExtendibleParamDescriptor>, 
                             ExtendibleParamDescriptor,',','(', ')'>
{
 public:
  void replace_var( const std::string& varname, const SymbolicVal& val);
  void Dump() const;
};
//! A symbolic function declaration with 
// a parameter list 'SymbolicParamListDescriptor' and a name (??) 'SymbolicValDescriptor',
// separated by ':' in their string format
// Note the reversed order for function name and parameter list
class SymbolicFunctionDeclaration
 : private CollectPair<SymbolicParamListDescriptor,SymbolicValDescriptor,':'>
{
  typedef CollectPair<SymbolicParamListDescriptor,SymbolicValDescriptor,':'> BaseClass;
 public:
  SymbolicFunctionDeclaration( const SymbolicParamListDescriptor& par, 
                            const SymbolicValDescriptor& val)
              { first = par; second = val; }
  SymbolicFunctionDeclaration( const ExtendibleParamDescriptor& par,
                            const SymbolicValDescriptor& val)
              { first.push_back(par); second = val; }

  void write( std::ostream& out) const
        {  BaseClass::write(out); }
  void Dump() const;
  std::string toString() const { std::stringstream out; write(out); return out.str(); }

  bool get_val( const std::vector<SymbolicVal>& args, SymbolicVal& r) const;
  void replace_var( const std::string& varname, const SymbolicVal& val);
  void replace_val(MapObject<SymbolicVal, SymbolicVal>& repl);
};
//! A list of symbolic function declarations, separated by ',', enclosed in '(..)'
class SymbolicFunctionDeclarationGroup
  : public ContainerDescriptor< std::list<SymbolicFunctionDeclaration>, 
                                SymbolicFunctionDeclaration, ',', '(', ')'>
{
  typedef ContainerDescriptor< std::list<SymbolicFunctionDeclaration>,
                                SymbolicFunctionDeclaration, ',', '(', ')'>
      BaseClass;
public:
 bool get_val( const std::vector<SymbolicVal>& argList, SymbolicVal& r) const;
 bool get_val( AstInterface& fa, AstInterface::AstNodeList& argList, 
                  AstNodePtr& r) const;
 void replace_var( const std::string& varname, const SymbolicVal& val);
 void replace_val( MapObject<SymbolicVal, SymbolicVal>& repl);
 void Dump() const;
};

class DefineVariableDescriptor 
  : public CollectPair< CollectPair<TypeDescriptor, ExtendibleParamDescriptor,0>, 
                        SymbolicValDescriptor, '='>
{
  typedef CollectPair< CollectPair<TypeDescriptor, ExtendibleParamDescriptor,0>,
                        SymbolicValDescriptor, '='> BaseClass;
 public:
  std::string get_var_type() const { return first.first; }
  const ExtendibleParamDescriptor& get_var() const { return first.second; }
  ExtendibleParamDescriptor& get_var() { return first.second; }
  SymbolicVal get_var_init() const { return second; }
  void replace_var( const std::string& varname, const SymbolicVal& val);
  void replace_val( MapObject<SymbolicVal, SymbolicVal>& repl);
  void Dump() const;
};
// string_name(parameterList)=symbolicVal
// e.g: elem(i:dim:1:dimension) = this(i$dim)
class ReadSymbolicFunctionDeclaration 
 : public CollectPair< SelectPair <StringDescriptor, 
                                 SymbolicParamListDescriptor,0,'('>,
                     SymbolicValDescriptor, '='> 
{
  public:
   std::string get_name() const { return first.first; }
   SymbolicFunctionDeclaration get_function() const 
     { return SymbolicFunctionDeclaration( first.second, second); }
};

#endif
