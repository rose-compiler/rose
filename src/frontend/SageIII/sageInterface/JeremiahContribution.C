#include "rose.h"
#include <vector>

using namespace std;

void findBreakStmtsHelper(SgStatement* code, vector<SgBreakStmt*>& breakStmts) {
  if (isSgWhileStmt(code) || isSgDoWhileStmt(code) || isSgForStatement(code) || isSgSwitchStatement(code)) {
    return;
  }
  if (isSgBreakStmt(code)) {
    breakStmts.push_back(isSgBreakStmt(code));
    return;
  }
  vector<SgNode*> children = code->get_traversalSuccessorContainer();
  for (int i = 0; i < children.size(); ++i) {
    if (isSgStatement(children[i])) {
      findBreakStmtsHelper(isSgStatement(children[i]), breakStmts);
    }
  }
}

vector<SgBreakStmt*> findBreakStmts(SgStatement* code) {
  // Run this on the body of a loop or switch, because it stops at any
  // construct which defines a new scope for break statements
  vector<SgBreakStmt*> result;
  findBreakStmtsHelper(code, result);
  return result;
}

void findContinueStmtsHelper(SgStatement* code, vector<SgContinueStmt*>& continueStmts) {
  if (isSgWhileStmt(code) || isSgDoWhileStmt(code) || isSgForStatement(code)) {
    return;
  }
  if (isSgContinueStmt(code)) {
    continueStmts.push_back(isSgContinueStmt(code));
    return;
  }
  vector<SgNode*> children = code->get_traversalSuccessorContainer();
  for (int i = 0; i < children.size(); ++i) {
    if (isSgStatement(children[i])) {
      findContinueStmtsHelper(isSgStatement(children[i]), continueStmts);
    }
  }
}

vector<SgContinueStmt*> findContinueStmts(SgStatement* code) {
  // Run this on the body of a loop, because it stops at any construct which
  // defines a new scope for continue statements
  vector<SgContinueStmt*> result;
  findContinueStmtsHelper(code, result);
  return result;
}

//! Find the variables directly in a scope, excluding those in nested
//! scopes.  This is done by searching the scope directly, so it should
//! only be used inside a function.  It might be worth fixing this up to
//! work in all contexts.
vector<SgInitializedName*> findVariablesDirectlyInScope(SgStatement* sc) {
  // This can't be done with a traversal because it needs to avoid
  // entering inner scope statements
  vector<SgInitializedName*> variablesInScope;
  vector<SgNode*> succs = sc->get_traversalSuccessorContainer();
  for (int i = succs.size() - 1; i > -1; --i) {
    if (isSgVariableDeclaration(succs[i])) {
      SgVariableDeclaration* vd = isSgVariableDeclaration(succs[i]);
      const SgInitializedNamePtrList& vars = vd->get_variables();
      variablesInScope.insert(variablesInScope.end(), vars.rbegin(), vars.rend());
    } else if (isSgForInitStatement(succs[i])) {
      vector<SgInitializedName*> initVars = findVariablesDirectlyInScope(isSgForInitStatement(succs[i]));
      variablesInScope.insert(variablesInScope.end(), initVars.begin(), initVars.end());
    }
  }
  return variablesInScope;
} 

