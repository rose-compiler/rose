
#include <vector>

/* 
   Additional code to extract from some of Jeremiah's work:
From src/midend/astInlining/inlinerSupport.h:
  isPotentiallyModified
  hasAddressTaken
  flattenBlocks
  isMemberVariable
  renameVariables
  rebindVariableAndLabelReferences
  fixReturnStatements (maybe)
  changeAllMembersToPublic
  myRemoveStatement (probably rename)
  removeVariableDeclaration
From src/midend/astInlining/inlinerSupport.C
  containsVariableReference
  isDeclarationOf
  isPotentiallyModifiedDuringLifeOf
From src/midend/astInlining/replaceExpressionWithStatement.h:
  getRootOfExpression
  changeContinuesToGotos
  pushTestIntoBody
  addStepToLoopBody
  myStatementInsert (should be renamed or merged into another function)
  convertInitializerIntoAssignment
  getStatementOfExpression
  replaceExpressionWithExpression (maybe)
From src/midend/astInlining/replaceExpressionWithStatement.C:
  getInitializerOfExpression
  addAllVariablesAsSymbols
Everything from src/midend/programTransformation/partialRedundancyElimination/expressionTreeEqual.h
From src/midend/programTransformation/partialRedundancyElimination/pre.h:
  anyOfListPotentiallyModifiedIn
  getSymbolsUsedInExpression
  PRE::getFunctionDefinition (there is a function doing the same thing in the CFG view code, and they should be merged)
From src/midend/programTransformation/finiteDifferencing/patternRewrite.h:
  replaceChild
From src/midend/programTransformation/finiteDifferencing/finiteDifferencing.h:
  moveForDeclaredVariables
From constantPropagation.C (written when I was here last time, attached to this email):
  enum dead_code_kind
  DeadCodeFindingVisitor
  A function "void removeDeadCode(SgStatement* scope)" containing the relevant code from main in that file (i.e., lines 394 to 420), with "vis.funs[funnum]" replaced by "scope"

-- Jeremiah Willcock

*/
