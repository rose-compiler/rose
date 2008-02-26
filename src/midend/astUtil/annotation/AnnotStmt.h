#ifndef ANNOT_STMT_H
#define ANNOT_STMT_H

#include "AnnotExpr.h"

class VariableDefinitionDescriptor 
  : public CollectPair< CollectPair<TypeDescriptor, NameDescriptor,0>, 
                        SymbolicValDescriptor, '='>
{
  typedef CollectPair< CollectPair<TypeDescriptor, NameDescriptor,0>,
                        SymbolicValDescriptor, '='> BaseClass;
 public:
  string get_var_type() const { return first.first; }
  string get_var_name() const { return first.second; }
  SymbolicVal get_var_init() const { return second; }
  VariableDefinitionDescriptor& read( istream& in)
   {  BaseClass::read(in); return *this; }
};

#endif
