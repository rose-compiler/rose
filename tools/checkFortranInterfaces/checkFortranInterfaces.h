// This program checks Fortran and C files and declares if they
// have matching interfaces based on functions arguments.
//

#ifndef CHECK_FORTRAN_INTERFACES_H_
#define CHECK_FORTRAN_INTERFACES_H_

#include "rose.h"

typedef std::map <std::string, SgFunctionDeclaration*> mapType;
typedef std::pair<std::string, SgFunctionDeclaration*> mapPair;

class InterfaceChecker
{
  public:

   SgType* fortran_to_c_type(SgType* f_type, SgExpression* kind_type, bool hasValue);
   bool compare_type_array(SgArrayType* f_array, SgArrayType* c_array);
   bool compare_type_class(SgClassType* f_class, SgClassType* c_class);
   bool compare_types(SgInitializedName* f_name, SgInitializedName* c_name);
   bool compare_decl(SgFunctionDeclaration* f_func_dec, SgFunctionDeclaration* c_func_dec);
   bool check_interface(mapType map_f, mapType map_c);
};

// Inherited attribute (see ROSE Tutorial (Chapter 9)).
class InheritedAttribute
{
  public:
   InheritedAttribute();
};


// Synthesized attribute (see ROSE Tutorial (Chapter 9)).
class SynthesizedAttribute
{
  public:
   SynthesizedAttribute();
};


class Traversal : public SgTopDownBottomUpProcessing<InheritedAttribute,SynthesizedAttribute>
{
  public:

   mapType map_c_or_cxx;
   mapType map_f;

   mapType getFortranMap() { return map_f       ; }
   mapType getCMap()       { return map_c_or_cxx; }

   Traversal();

   InheritedAttribute   evaluateInheritedAttribute   (SgNode* astNode, InheritedAttribute inheritedAttribute);
   SynthesizedAttribute evaluateSynthesizedAttribute (SgNode* astNode, InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList);
};

#endif
