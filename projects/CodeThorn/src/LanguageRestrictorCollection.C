
#include "sage3basic.h"

#include "LanguageRestrictorCollection.h"

using namespace CodeThorn;

void LanguageRestrictorCppSubset1::initialize() {
  allowAstNodesRequiredForEmptyProgram();
  LanguageRestrictor::VariantSet vs;
  vs.insert(V_SgIntVal);
  vs.insert(V_SgAssignOp);
  vs.insert(V_SgCastExp);
  vs.insert(V_SgVarRefExp);
  vs.insert(V_SgExprStatement);
  vs.insert(V_SgIfStmt);
  vs.insert(V_SgWhileStmt);
  vs.insert(V_SgDoWhileStmt);
  
  vs.insert(V_SgForInitStatement);
  vs.insert(V_SgForStatement);
  
  vs.insert(V_SgBreakStmt);
  vs.insert(V_SgContinueStmt);
  vs.insert(V_SgGotoStatement);

  vs.insert(V_SgAndOp);
  vs.insert(V_SgOrOp);
  vs.insert(V_SgNotOp);
  vs.insert(V_SgNotEqualOp);
  vs.insert(V_SgEqualityOp);
  vs.insert(V_SgIntVal);
  vs.insert(V_SgVariableDeclaration);
  vs.insert(V_SgReturnStmt);
  vs.insert(V_SgAssignInitializer);
  vs.insert(V_SgBoolValExp);
  vs.insert(V_SgLabelStatement);
  vs.insert(V_SgNullStatement);
  vs.insert(V_SgConditionalExp); // TODO: case if inside expressions
  vs.insert(V_SgMinusOp);
  
  vs.insert(V_SgPlusPlusOp);
  vs.insert(V_SgMinusMinusOp);
  
  // inter-procedural
  vs.insert(V_SgFunctionCallExp);
  vs.insert(V_SgFunctionRefExp);
  vs.insert(V_SgExprListExp);

  //vs.insert(V_SgPragmaDeclaration);
  //vs.insert(V_SgPragma);
  
  // rers Problems
  vs.insert(V_SgTypedefDeclaration);
  vs.insert(V_SgClassDeclaration);
  vs.insert(V_SgClassDefinition);
  vs.insert(V_SgEnumDeclaration);
  vs.insert(V_SgStringVal);
  vs.insert(V_SgAddressOfOp);
  
  // rers Problems 10-13
  // arithmetic operators
  vs.insert(V_SgAddOp);
  vs.insert(V_SgSubtractOp);
  vs.insert(V_SgMultiplyOp);
  vs.insert(V_SgDivideOp);
  vs.insert(V_SgModOp);
  vs.insert(V_SgGreaterOrEqualOp);
  vs.insert(V_SgLessThanOp);
  vs.insert(V_SgGreaterThanOp);
  vs.insert(V_SgLessOrEqualOp);
  
  // temporary: is used inside asserts (on some systems), but no handled yet in general. It is ignored in assert, because __fail is called before.
  vs.insert(V_SgCommaOpExp);
  //vs.insert(V_SgNullExpression); TODO (e.g. return;)

  vs.insert(V_SgEnumVal);

  setAstNodeVariantSet(vs,true);
}

