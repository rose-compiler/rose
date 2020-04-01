#ifndef TRANSFORMATION_OPERATORS_H
#define TRANSFORMATION_OPERATORS_H

class SgNode;
class SgFunctionDefinition;
class SgExpression;

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "UDAstAttribute.h"

namespace CodeThorn {

class TransformationOperators {
 public:
  //! Creates a copy of function definition originalFunDef and returns a pointer to the copy.
  //! The name of the new function is 'prefix'+originalName+'postfix'. Note that this function also properly creates a new
  //! declaration of the function.
  static SgFunctionDefinition* createAdaptedFunctionDefinitionCopy(SgFunctionDefinition* originalFunDef, std::string prefix, std::string suffix);
  //! computes the list of all SgExpressions in the AST that are using variables of varId
  //! e.g. for the program "x=y; z=z+y; yp=&y; z+=*yp;" returns 3 SgExpression pointers for varid of y, and 2 for the varid of yp
  static std::list<SgExpression*> varRefExpOfVar(SgNode* root, CodeThorn::VariableId varId, CodeThorn::VariableIdMapping* variableIdMapping );
  //! Requires the AST to be annotated with UDAstAttribute. Based on this information it replaces the annotated expression with the rhs of its definition. This tranformation is only performed if there is exactly one definition (otherwise it would be wrong).
  //! This function does NOT take into account yet whether the rhs is an available expression (TODO: also check for AEAttribute) - if both attributes are available the transformation can be applied automatically.
  static void substituteUsesWithAvailableExpRhsOfDef(std::string udAttributeName, SgNode* root, CodeThorn::Labeler* labeler, CodeThorn::VariableIdMapping* variableIdMapping);
  
};
}

#endif
