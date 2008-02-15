#include "rose.h"
#include <vector>

//! Find all break statements inside a particular statement, stopping at nested
//! loops or switch statements, which define their own contexts for break
//! statements.  The function will stop immediately if run on a loop or switch
//! statement.
std::vector<SgBreakStmt*> findBreakStmts(SgStatement* code);



// ***** This function should replace the use of an identical function 
// in ROSE/src/midend/astInlining/replaceExpressionWithStatement.h
// Also remove the findContinueStmtsHelper() function.

//! Find all continue statements inside a particular statement, stopping at
//! nested loops, which define their own contexts for continue statements.  The
//! function will stop immediately if run on a loop.
std::vector<SgContinueStmt*> findContinueStmts(SgStatement* code);


/* 
   Additional code to extract from some of Jeremiah's work:

Everything from loopHelpers.h
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
  getScope
  removeVariableDeclaration
From src/midend/astInlining/inlinerSupport.C
  removeUnusedLabels
  containsVariableReference
  isDeclarationOf
  isPotentiallyModifiedDuringLifeOf
From src/midend/astInlining/replaceExpressionWithStatement.h:
  getRootOfExpression
  changeContinuesToGotos
  pushTestIntoBody
  addStepToLoopBody
  myStatementInsert (should be renamed or merged into another function)
  splitExpression
  convertInitializerIntoAssignment
  getStatementOfExpression
  replaceExpressionWithExpression (maybe)
From src/midend/astInlining/replaceExpressionWithStatement.C:
  getInitializerOfExpression
  addAllVariablesAsSymbols
Everything from src/midend/astInlining/typeTraits.h
Everything from src/midend/programTransformation/partialRedundancyElimination/expressionTreeEqual.h
From src/midend/programTransformation/partialRedundancyElimination/pre.h:
  anyOfListPotentiallyModifiedIn
  getSymbolsUsedInExpression
  PRE::getFunctionDefinition (there is a function doing the same thing in the CFG view code, and they should be merged)
From src/midend/programTransformation/finiteDifferencing/patternRewrite.h:
  replaceChild
From src/midend/programTransformation/finiteDifferencing/finiteDifferencing.h:
  moveForDeclaredVariables
From projects/AtermTranslation/atermTranslation.h:
  intToHex (both versions)
From cfgView.h (the new CFG code):
  getSwitchCases
  findReturnStmts
  getEnclosingProcedure (merge with version in PRE)
  findGotos
  findEnclosingSwitch
  indexList (needs to be fully in a header file)
  concat (maybe)
From constantPropagation.C (written when I was here last time, attached to this email):
  enum dead_code_kind
  DeadCodeFindingVisitor
  A function "void removeDeadCode(SgStatement* scope)" containing the relevant code from main in that file (i.e., lines 394 to 420), with "vis.funs[funnum]" replaced by "scope"

-- Jeremiah Willcock

*/
