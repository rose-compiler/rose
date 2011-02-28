#ifndef VALUE_ANNOT_H
#define VALUE_ANNOT_H

#include <AnnotExpr.h>
#include <OperatorAnnotation.h>
#include <CPPAstInterface.h>
#include <list>
//! The semantic information (values) for data members of a type. 
// Store the information into a map between the name of the data members and the expressions of their values
// e.g. class Index {has_value { stride = this.stride; base = this.base; length = this.length; } }
class HasValueDescriptor 
   : public MapDescriptor<StringDescriptor, SymbolicValDescriptor, ';', '{', '}'>
{
 public:
  typedef 
     MapDescriptor<StringDescriptor, SymbolicValDescriptor, ';', '{', '}'>
  BaseClass;
  bool read( std::istream& in)
    {
      return ReadContainer<HasValueDescriptor, 
                           CollectPair<StringDescriptor, SymbolicValDescriptor, '='>,
                           ';', '{','}'>::read(*this, in);   
    }
  bool merge (const HasValueDescriptor& that); 
  std::string toString() const;
  void replace_var( const std::string& name, const SymbolicVal& repl);
  void replace_val( MapObject<SymbolicVal, SymbolicVal>& repl); 
  // Check if there is a value expression for a data member named as 'name'.
  // If yes, store the symbolic value expression into 'r' when r!=0
  bool has_value( const std::string& name, SymbolicValDescriptor* r = 0) const;
};

//! Collecting annotations for "has_value", part of type/class annotations
class HasValueCollection : public TypeAnnotCollection< HasValueDescriptor>
{
   typedef TypeAnnotCollection< HasValueDescriptor > BaseClass;
  virtual bool read_annot_name( const std::string& annotName) const 
    { return annotName == "has_value"; }
 public:
  void Dump() const 
    { std::cerr << "has_value: \n"; BaseClass::Dump(); }
  std::string is_known_member_function( AstInterface& fa,
                       const SymbolicVal& exp, AstNodePtr* objp,
                       SymbolicFunction::Arguments* argsp, 
                        HasValueDescriptor* descp);
  std::string is_known_member_function( CPPAstInterface& fa, 
                        const AstNodePtr& exp, AstNodePtr* objp, 
                        AstInterface::AstNodeList* args,
                        HasValueDescriptor* desc);
};

typedef CollectPair< SymbolicValDescriptor, HasValueDescriptor, '='> RestrictValueDescriptor;
class RestrictValueOpDescriptor 
 : public  OPDescriptorTemp 
    <ContainerDescriptor< std::list <RestrictValueDescriptor>, RestrictValueDescriptor, ';', '{', '}'>  > 
{
  typedef  OPDescriptorTemp
    <ContainerDescriptor< std::list <RestrictValueDescriptor>, RestrictValueDescriptor, ';', '{', '}'>  >
    BaseClass;
 public:
  void replace_val( MapObject<SymbolicVal, SymbolicVal>& repl); 
};

class RestrictValueOpCollection
: public OperatorAnnotCollection<RestrictValueOpDescriptor>
{
  virtual bool read_annot_name( const std::string& annotName) const
    { return annotName == "restrict_value"; }
 public:
  void Dump() const
    {
      std::cerr << "restrict_value: \n";
      OperatorAnnotCollection<RestrictValueOpDescriptor>::Dump();
    }
};
//! Collector for both has_value and restrict_value annotations
class ValueAnnotation 
{
  HasValueCollection values;
  RestrictValueOpCollection valueRestrict;
  static ValueAnnotation* inst;
  ValueAnnotation() {}
 public:
  static ValueAnnotation* get_inst();
  void register_annot(); 
  void Dump() const;
  bool known_type( AstInterface& fa, const AstNodePtr& exp, HasValueDescriptor* d = 0);
  bool is_value_restrict_op( AstInterface& fa, const AstNodePtr& exp, 
                    Collect2Object< AstNodePtr, HasValueDescriptor>* desc = 0,
                    MapObject<SymbolicVal, SymbolicVal>* valMap = 0,
                    Map2Object<AstInterface*, AstNodePtr, AstNodePtr> *codegen = 0);
  bool is_access_value( CPPAstInterface& fa, const AstNodePtr& exp,
                        AstNodePtr* obj=0, std::string* name = 0,
                        AstInterface::AstNodeList* args=0,
                        HasValueDescriptor* desc = 0);
};

#endif

