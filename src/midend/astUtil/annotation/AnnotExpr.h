#ifndef ANNOT_EXPR_H
#define ANNOT_EXPR_H

#include <AnnotDescriptors.h>
#include <SymbolicVal.h>
#include <FunctionObject.h>
#include <stdlib.h>

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
  std::string ToString() const { std::stringstream out; write(out); return out.str(); }

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
  virtual SymbolicFunction* CloneFunction( const SymbolicVal& _op,
                                         const Arguments& v) const
         { return new SymbolicDotExp( v); }

  public:
   SymbolicDotExp(const Arguments& v)
    : SymbolicFunction(".", v)  { assert( v.size() == 2); }
   SymbolicDotExp( const SymbolicVal& obj, const SymbolicVal& field)
    : SymbolicFunction(".", obj, field) {}
   SymbolicDotExp( const SymbolicVal& obj, const std::string& fieldname)
    : SymbolicFunction(".", obj, SymbolicConst(fieldname, "field")) {}
   SymbolicDotExp( const SymbolicDotExp& that)
    : SymbolicFunction(that) {}

   virtual AstNodePtr CodeGen( AstInterface& fa) const;
};

class SymbolicSubscriptExp : public SymbolicFunction
{
  virtual SymbolicFunction* CloneFunction( const SymbolicVal& _op,
                                         const Arguments& v) const
         { return new SymbolicSubscriptExp( v); }

  public:
   SymbolicSubscriptExp(const Arguments& v)
    : SymbolicFunction("[]", v)  { assert( v.size() == 2); }
   SymbolicSubscriptExp( const SymbolicVal& array, const SymbolicVal& index)
    : SymbolicFunction(".", array, index) {}
   SymbolicSubscriptExp( const SymbolicSubscriptExp& that)
    : SymbolicFunction(that) {}

   virtual AstNodePtr CodeGen( AstInterface& fa) const;
};

class SymbolicExtendVar : public SymbolicFunction
{
  SymbolicValImpl* Clone() const
   { return new SymbolicExtendVar(*this); }
  virtual SymbolicFunction* CloneFunction( const SymbolicVal& _op,
                                         const Arguments& v) const
         { return new SymbolicExtendVar( v); }

  public:
   SymbolicExtendVar(const Arguments& v)
    : SymbolicFunction("$", v)  { assert( v.size() == 2); }
   SymbolicExtendVar( const std::string& varname, int  index)
    : SymbolicFunction("$", SymbolicVar(varname, 0), index) {}
   SymbolicExtendVar( const SymbolicExtendVar& that)
    : SymbolicFunction(that) {}

   virtual AstNodePtr CodeGen( AstInterface& fa) const;
   static std::string get_varname( std::string var, int index);
};


class ExtendibleParamDescriptor 
  : private SelectPair < SymbolicValDescriptor,
                         CollectPair<NameDescriptor, 
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
  std::string ToString() const { std::stringstream out; write(out); return out.str(); }

  bool read(std::istream& in);
  const SymbolicValDescriptor& get_param() const { return first; }
  SymbolicValDescriptor& get_param() { return first; }
  std::string get_param_name() const 
        { assert(first.get_val().GetValType() == VAL_VAR); return first.get_val().ToString(); }
  std::string get_extend_var() const { return second.first; }
  bool get_extension( int& lb, int& ub) const; 

  void replace_var( const std::string& varname, const SymbolicVal& val);
  void replace_val(MapObject<SymbolicVal, SymbolicVal>& repl);
};

class SymbolicParamListDescriptor 
  : public ContainerDescriptor< std::vector <ExtendibleParamDescriptor>, 
                             ExtendibleParamDescriptor,',','(', ')'>
{
 public:
  void replace_var( const std::string& varname, const SymbolicVal& val);
  void Dump() const;
};

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
  std::string ToString() const { std::stringstream out; write(out); return out.str(); }

  bool get_val( const SymbolicFunction::Arguments& argList, SymbolicVal& r) const;
  void replace_var( const std::string& varname, const SymbolicVal& val);
  void replace_val(MapObject<SymbolicVal, SymbolicVal>& repl);
};

class SymbolicFunctionDeclarationGroup
  : public ContainerDescriptor< std::list<SymbolicFunctionDeclaration>, 
                                SymbolicFunctionDeclaration, ',', '(', ')'>
{
  typedef ContainerDescriptor< std::list<SymbolicFunctionDeclaration>,
                                SymbolicFunctionDeclaration, ',', '(', ')'>
      BaseClass;
public:
 bool get_val( const SymbolicFunction::Arguments& argList, SymbolicVal& r) const;
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
