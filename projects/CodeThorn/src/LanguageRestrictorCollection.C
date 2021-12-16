
#include "sage3basic.h"

#include "LanguageRestrictorCollection.h"

using namespace CodeThorn;

void LanguageRestrictorRers::initialize() {
  allowAstNodesRequiredForEmptyProgram();
  LanguageRestrictor::VariantSet vs={
  V_SgIntVal,
  V_SgAssignOp,
  V_SgCastExp,
  V_SgVarRefExp,
  V_SgExprStatement,
  V_SgIfStmt,
  V_SgWhileStmt,
  V_SgDoWhileStmt,
  
  V_SgForInitStatement,
  V_SgForStatement,
  
  V_SgBreakStmt,
  V_SgContinueStmt,
  V_SgGotoStatement,

  V_SgAndOp,
  V_SgOrOp,
  V_SgNotOp,
  V_SgNotEqualOp,
  V_SgEqualityOp,
  V_SgIntVal,
  V_SgVariableDeclaration,
  V_SgReturnStmt,
  V_SgAssignInitializer,
  V_SgBoolValExp,
  V_SgLabelStatement,
  V_SgNullStatement,
  V_SgConditionalExp,
  V_SgMinusOp,
  
  V_SgPlusPlusOp,
  V_SgMinusMinusOp,
  
  // inter-procedural
  V_SgFunctionCallExp,
  V_SgFunctionRefExp,
  V_SgExprListExp,

  //V_SgPragmaDeclaration,
  //V_SgPragma,
  
  // rers Problems
  V_SgTypedefDeclaration,
  V_SgClassDeclaration,
  V_SgClassDefinition,
  V_SgEnumDeclaration,
  V_SgStringVal,
  V_SgAddressOfOp,
  
  // rers Problems 10-13
  // arithmetic operators
  V_SgAddOp,
  V_SgSubtractOp,
  V_SgMultiplyOp,
  V_SgDivideOp,
  V_SgModOp,
  V_SgGreaterOrEqualOp,
  V_SgLessThanOp,
  V_SgGreaterThanOp,
  V_SgLessOrEqualOp,
  
  // is used inside asserts
  V_SgCommaOpExp,

  V_SgNullExpression,
  V_SgEnumVal,
  V_SgSizeOfOp,

  V_SgStatementExpression, // GNU extension
  };
  
  setAstNodeVariantSet(vs,true);
}

void LanguageRestrictorC::initialize() {
  // generated with 'thorn3 --print-variant-set program.c'
  LanguageRestrictor::VariantSet vs
    ={
    V_SgAddOp,
    V_SgAddressOfOp,
    V_SgAggregateInitializer,
    V_SgAndAssignOp,
    V_SgAndOp,
    V_SgArrowExp,
    V_SgAsmOp,
    V_SgAsmStmt,
    V_SgAssignInitializer,
    V_SgAssignOp,
    V_SgBasicBlock,
    V_SgBitAndOp,
    V_SgBitComplementOp,
    V_SgBitOrOp,
    V_SgBitXorOp,
    V_SgBreakStmt,
    V_SgCaseOptionStmt,
    V_SgCastExp,
    V_SgCharVal,
    V_SgClassDeclaration,
    V_SgClassDefinition,
    V_SgCommaOpExp,
    V_SgConditionalExp,
    V_SgContinueStmt,
    V_SgDefaultOptionStmt,
    V_SgDesignatedInitializer,
    V_SgDivAssignOp,
    V_SgDivideOp,
    V_SgDoWhileStmt,
    V_SgDotExp,
    V_SgDoubleVal,
    V_SgEnumDeclaration,
    V_SgEnumVal,
    V_SgEqualityOp,
    V_SgExprListExp,
    V_SgExprStatement,
    V_SgFileList,
    V_SgForInitStatement,
    V_SgForStatement,
    V_SgFunctionCallExp,
    V_SgFunctionDeclaration,
    V_SgFunctionDefinition,
    V_SgFunctionParameterList,
    V_SgFunctionRefExp,
    V_SgGlobal,
    V_SgGotoStatement,
    V_SgGreaterOrEqualOp,
    V_SgGreaterThanOp,
    V_SgIfStmt,
    V_SgInitializedName,
    V_SgIntVal,
    V_SgIorAssignOp,
    V_SgLabelStatement,
    V_SgLessOrEqualOp,
    V_SgLessThanOp,
    V_SgLongIntVal,
    V_SgLshiftAssignOp,
    V_SgLshiftOp,
    V_SgMinusAssignOp,
    V_SgMinusMinusOp,
    V_SgMinusOp,
    V_SgModAssignOp,
    V_SgModOp,
    V_SgMultAssignOp,
    V_SgMultiplyOp,
    V_SgNotEqualOp,
    V_SgNotOp,
    V_SgNullExpression,
    V_SgNullStatement,
    V_SgOrOp,
    V_SgPlusAssignOp,
    V_SgPlusPlusOp,
    V_SgPntrArrRefExp,
    V_SgPointerDerefExp,
    V_SgProject,
    V_SgReturnStmt,
    V_SgRshiftAssignOp,
    V_SgRshiftOp,
    V_SgShortVal,
    V_SgSizeOfOp,
    V_SgSourceFile,
    V_SgStatementExpression,
    V_SgStringVal,
    V_SgSubtractOp,
    V_SgSwitchStatement,
    V_SgTypedefDeclaration,
    V_SgVarRefExp,
    V_SgVariableDeclaration,
    V_SgWhileStmt,
    V_SgXorAssignOp,
  };
  setAstNodeVariantSet(vs,true);
}
