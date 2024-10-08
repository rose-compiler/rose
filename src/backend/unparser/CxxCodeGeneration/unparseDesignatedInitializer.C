
#include "sage3basic.h"
#include "unparser.h"
#include "rose_config.h"

#define DEBUG__unparseDesignatedInitializer 0

void Unparse_ExprStmt::unparseDesignatedInitializer(SgExpression * expr, SgUnparse_Info & info) {
  SgDesignatedInitializer* design_init = isSgDesignatedInitializer(expr);
#if DEBUG__unparseDesignatedInitializer
  printf ("Enter unparseDesignatedInitializer()\n");
  printf ("  design_init = %p = %s\n", design_init, design_init->class_name().c_str());
#endif
  ROSE_ASSERT(design_init->get_designatorList()->get_expressions().empty() == false);

  SgExpression *  designator  = design_init->get_designatorList()->get_expressions()[0];
  SgInitializer * initializer = design_init->get_memberInit();
#if DEBUG__unparseDesignatedInitializer
  printf ("  designator = %p = %s\n", designator, designator ? designator->class_name().c_str() : "");
  printf ("  initializer = %p = %s\n", initializer, initializer ? initializer->class_name().c_str() : "");
#endif
  SgVarRefExp * varRefExp = isSgVarRefExp(designator);

  bool isDataMemberDesignator   = (varRefExp != NULL);
  bool isArrayElementDesignator = (isSgUnsignedLongVal(designator) != NULL);
  bool isCastDesignator         = (isSgCastExp(designator) != NULL);
  bool isAggregateInitializer   = (isSgAggregateInitializer(designator) != NULL);
  bool isAssignInitializer      = (isSgAssignInitializer(initializer) != NULL);

  bool isInUnion = false;
  if (isDataMemberDesignator == true) {
    SgVariableSymbol*   variableSymbol   = isSgVariableSymbol(varRefExp->get_symbol());
    SgClassDefinition*  classDefinition  = isSgClassDefinition(variableSymbol->get_declaration()->get_scope());
    SgClassDeclaration* classDeclaration = NULL;
    if (classDefinition != NULL) {
      classDeclaration = classDefinition->get_declaration();
    }

    isInUnion = (classDeclaration != NULL && classDeclaration->get_class_type() == SgClassDeclaration::e_union);
    if (isInUnion == true) {
      bool isInFunctionCallArgument = SageInterface::getEnclosingNode<SgFunctionCallExp>(design_init);
      if (isInFunctionCallArgument == false) {
        isInUnion = false;
      }
    }

    if (isInUnion == false) {
      curprint (".");
      unparseVarRef(designator, info);
    }
  } else {
    if (isArrayElementDesignator == true) {
      curprint ("[");
      unparseValue(designator, info);
      curprint ("]");
      isArrayElementDesignator = true;
    } else {
      if (isCastDesignator == true || isAggregateInitializer == true) {
        printf ("WARNING: designator might be an inappropriate expression (expected SgVarRefExp or SgUnsignedLongVal, but this case might be OK): designator = %p = %s \n",designator,designator->class_name().c_str());
      } else {
        printf ("ERROR: designator is an inappropriate expression (should be SgVarRefExp or SgUnsignedLongVal): designator = %p = %s \n",designator,designator->class_name().c_str());
        ROSE_ABORT();
      }
    }
  }

  SgAggregateInitializer* aggregateInitializer = isSgAggregateInitializer(initializer);
  bool need_explicit_braces_in_aggregateInitializer = (aggregateInitializer != NULL && aggregateInitializer->get_need_explicit_braces());

  bool need_explicit_braces = (need_explicit_braces_in_aggregateInitializer == false);

  if (need_explicit_braces == true && isAssignInitializer == true) {
    SgAssignInitializer* assignInitializer = isSgAssignInitializer(initializer);
    ASSERT_not_null(assignInitializer);
    SgValueExp*       valueExp       = isSgValueExp(assignInitializer->get_operand());
    SgCastExp*        castExp        = isSgCastExp(assignInitializer->get_operand());
    SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(assignInitializer->get_operand());

    ASSERT_not_null(assignInitializer->get_operand());
    ASSERT_not_null(assignInitializer->get_operand()->get_type());

    SgClassType* classType = isSgClassType( assignInitializer->get_operand()->get_type()->stripType(SgType::STRIP_MODIFIER_TYPE | SgType::STRIP_REFERENCE_TYPE | SgType::STRIP_RVALUE_REFERENCE_TYPE | SgType::STRIP_TYPEDEF_TYPE) );
    bool isClassType = (classType != NULL);

    if (valueExp != NULL || castExp != NULL || functionRefExp != NULL || isClassType == true) {
      need_explicit_braces = false;
    }
  }

  if (!isInUnion) {
    curprint (" = ");
    if (need_explicit_braces) {
      curprint ("{");
    }
  }

  unparseExpression(initializer, info);

  if (!isInUnion) {
    if (need_explicit_braces) {
      curprint ("}");
    }
  }
#if DEBUG__unparseDesignatedInitializer
  printf ("Leave unparseDesignatedInitializer()\n");
#endif
}
