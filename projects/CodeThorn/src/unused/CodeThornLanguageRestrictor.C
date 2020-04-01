#include "sage3basic.h"
#include "CodeThornLanguageRestrictor.h"

void CodeThorn::CodeThornLanguageRestrictor::initialize() {
  LanguageRestrictorCppSubset1::initialize();
  // RERS 2013 (required for some system headers)
  setAstNodeVariant(V_SgBitOrOp, true);
  setAstNodeVariant(V_SgBitAndOp, true);
  setAstNodeVariant(V_SgBitComplementOp, true);
  setAstNodeVariant(V_SgRshiftOp, true);
  setAstNodeVariant(V_SgLshiftOp, true);
  setAstNodeVariant(V_SgAggregateInitializer, true);
  setAstNodeVariant(V_SgNullExpression, true);
  // Polyhedral test codes and RERS 2015
  setAstNodeVariant(V_SgPlusAssignOp, true);
  setAstNodeVariant(V_SgMinusAssignOp, true);
  setAstNodeVariant(V_SgMultAssignOp, true);
  setAstNodeVariant(V_SgDivAssignOp, true);
  setAstNodeVariant(V_SgPntrArrRefExp, true);
  setAstNodeVariant(V_SgPragmaDeclaration, true);
  setAstNodeVariant(V_SgPragma, true);

  // floating point types
  setAstNodeVariant(V_SgFloatVal, true);
  setAstNodeVariant(V_SgDoubleVal, true);
  setAstNodeVariant(V_SgLongDoubleVal, true);

  // all accepted number types (restricted to int range)
  setAstNodeVariant(V_SgEnumVal, true);
  setAstNodeVariant(V_SgCharVal, true);
  setAstNodeVariant(V_SgUnsignedCharVal, true);
  setAstNodeVariant(V_SgBoolValExp, true);
  setAstNodeVariant(V_SgShortVal, true);
  setAstNodeVariant(V_SgIntVal, true);
  setAstNodeVariant(V_SgLongIntVal, true);
  setAstNodeVariant(V_SgLongLongIntVal, true);
  setAstNodeVariant(V_SgUnsignedShortVal, true);
  setAstNodeVariant(V_SgUnsignedIntVal, true);
  setAstNodeVariant(V_SgUnsignedLongVal, true);
  setAstNodeVariant(V_SgUnsignedLongLongIntVal, true);

  setAstNodeVariant(V_SgComplexVal, true);
  setAstNodeVariant(V_SgNullptrValExp, true);
  setAstNodeVariant(V_SgStringVal, true);

  //more general test codes
  setAstNodeVariant(V_SgPointerDerefExp, true);
  setAstNodeVariant(V_SgNullExpression, true);
  setAstNodeVariant(V_SgSizeOfOp,true);

}
