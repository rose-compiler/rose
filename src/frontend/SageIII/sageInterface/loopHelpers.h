#ifndef LOOP_HELPERS_H
#define LOOP_HELPERS_H

// #include "rose.h"
// #include <vector>

#if 0
//! Find all break statements inside a particular statement, stopping at nested
//! loops or switch statements, which define their own contexts for break
//! statements.  The function will stop immediately if run on a loop or switch
//! statement.
std::vector<SgBreakStmt*> findBreakStmts(SgStatement* code);

//! Find all continue statements inside a particular statement, stopping at
//! nested loops, which define their own contexts for continue statements.  The
//! function will stop immediately if run on a loop.
std::vector<SgContinueStmt*> findContinueStmts(SgStatement* code);


std::vector<SgStatement*> getSwitchCases(SgSwitchStatement* sw);
std::vector<SgReturnStmt*> findReturnStmts(SgStatement* code);
SgFunctionDefinition* getEnclosingProcedure(SgNode* n);
std::vector<SgGotoStatement*> findGotos(SgStatement* scope, SgLabelStatement* l);
std::vector<SgGotoStatement*> findFortranGotos(SgStatement* scope, SgStatement* l);
SgSwitchStatement* findEnclosingSwitch(SgStatement* s);

#endif

template <typename List>
typename List::value_type indexList(const List& l, unsigned int i) {
    assert (i < l.size());
    typename List::const_iterator it = l.begin();
    std::advance(it, i);
    return *it;
  }

template <typename T>
inline std::vector<T> concat(std::vector<T> a, const std::vector<T>& b) {
    a.insert(a.end(), b.begin(), b.end());
    return a;
  }

#endif // LOOP_HELPERS_H
