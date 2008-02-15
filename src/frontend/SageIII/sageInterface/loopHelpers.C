#include "rose.h"
#include <vector>

using namespace std;

void findBreakStmtsHelper(SgStatement* code, vector<SgBreakStmt*>& breakStmts) {
  if (isSgWhileStmt(code) || isSgDoWhileStmt(code) || isSgForStatement(code) || isSgSwitchStatement(code) || isSgFortranDo(code)) {
    return;
  }
  if (isSgBreakStmt(code)) {
    breakStmts.push_back(isSgBreakStmt(code));
    return;
  }
  vector<SgNode*> children = code->get_traversalSuccessorContainer();
  for (unsigned int i = 0; i < children.size(); ++i) {
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
  if (isSgWhileStmt(code) || isSgDoWhileStmt(code) || isSgForStatement(code) || isSgFortranDo(code)) {
    return;
  }
  if (isSgContinueStmt(code)) {
    continueStmts.push_back(isSgContinueStmt(code));
    return;
  }
  vector<SgNode*> children = code->get_traversalSuccessorContainer();
  for (unsigned int i = 0; i < children.size(); ++i) {
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

  SgFunctionDefinition* getEnclosingProcedure(SgNode* n) {
    while (n && !isSgFunctionDefinition(n)) n = n->get_parent();
    ROSE_ASSERT (n);
    return isSgFunctionDefinition(n);
  }

  vector<SgGotoStatement*> findGotos(SgStatement* scope, SgLabelStatement* l) {
 // DQ (9/25/2007): Moved from std::list to std::vector uniformally in ROSE.
 // But we still need the copy since the return type is IR node specific.
    Rose_STL_Container<SgNode*> allGotos = NodeQuery::querySubTree(scope, V_SgGotoStatement);

    vector<SgGotoStatement*> result;
    for (Rose_STL_Container<SgNode*>::const_iterator i = allGotos.begin(); i != allGotos.end(); ++i) {
      if (isSgGotoStatement(*i)->get_label() == l) {
	result.push_back(isSgGotoStatement(*i));
      }
    }
    return result;
  }

  vector<SgGotoStatement*> findFortranGotos(SgStatement* scope, SgStatement* l) {
 // DQ (9/25/2007): Moved from std::list to std::vector uniformally in ROSE.
 // But we still need the copy since the return type is IR node specific.
    Rose_STL_Container<SgNode*> allGotos = NodeQuery::querySubTree(scope, V_SgGotoStatement);

    vector<SgGotoStatement*> result;
    for (Rose_STL_Container<SgNode*>::const_iterator i = allGotos.begin(); i != allGotos.end(); ++i) {
      SgLabelRefExp* lRef = isSgGotoStatement(*i)->get_label_expression();
      if (!lRef) continue;
      SgLabelSymbol* sym = lRef->get_symbol();
      ROSE_ASSERT(sym);
      if (sym->get_fortran_statement() == l) {
	result.push_back(isSgGotoStatement(*i));
      }
    }
    return result;
  }

  vector<SgReturnStmt*> findReturnStmts(SgStatement* scope) {
 // DQ (9/25/2007): Moved from std::list to std::vector uniformally in ROSE. 
 // But we still need the copy since the return type is IR node specific.
    Rose_STL_Container<SgNode*> returns = NodeQuery::querySubTree(scope, V_SgReturnStmt);

    vector<SgReturnStmt*> result;
    for (Rose_STL_Container<SgNode*>::const_iterator i = returns.begin(); i != returns.end(); ++i) {
      result.push_back(isSgReturnStmt(*i));
    }
    return result;
  }

  void getSwitchCasesHelper(SgStatement* top, vector<SgStatement*>& result) {
    ROSE_ASSERT (top);
    if (isSgSwitchStatement(top)) return; // Don't descend into nested switches
    if (isSgCaseOptionStmt(top) || isSgDefaultOptionStmt(top)) {
      result.push_back(top);
    }
    vector<SgNode*> children = top->get_traversalSuccessorContainer();
    for (unsigned int i = 0; i < children.size(); ++i) {
      if (isSgStatement(children[i])) {
	getSwitchCasesHelper(isSgStatement(children[i]), result);
      }
    }
  }

  vector<SgStatement*> getSwitchCases(SgSwitchStatement* sw) {
    vector<SgStatement*> result;
    getSwitchCasesHelper(sw->get_body(), result);
    return result;
  }

  SgSwitchStatement* findEnclosingSwitch(SgStatement* s) {
    while (s && !isSgSwitchStatement(s)) {
      s = isSgStatement(s->get_parent());
    }
    ROSE_ASSERT (s);
    return isSgSwitchStatement(s);
  }

