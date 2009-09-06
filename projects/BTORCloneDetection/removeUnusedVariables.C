#include "satProblem.h"
#include <cassert>
#include <cstdio>
#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

#define DO_PURE_LITERAL_ELIMINATION

using namespace std;

int main(int, char**) {
  SatProblem cnf;
  cnf.parse(stdin);

  set<Lit> usedLits;
  for (size_t i = 0; i < cnf.clauses.size(); ++i) {
    const Clause& cl = cnf.clauses[i];
    for (size_t j = 0; j < cl.size(); ++j) {
      usedLits.insert(cl[j]);
    }
  }

  set<Var> usedVars;
  set<Var> interfaceUsedVars;

  for (size_t i = 0; i < cnf.interfaceVariables.size(); ++i) {
    const InterfaceVariable& iv = cnf.interfaceVariables[i];
    for (size_t j = 0; j < iv.second.size(); ++j) {
      Lit oldLit = iv.second[j];
      if (oldLit != -oldLit) {
        interfaceUsedVars.insert(abs(oldLit));
      }
    }
  }
  usedVars.insert(interfaceUsedVars.begin(), interfaceUsedVars.end());

  for (set<Lit>::const_iterator i = usedLits.begin(); i != usedLits.end(); ++i) {
#ifdef DO_PURE_LITERAL_ELIMINATION
    if (*i < 0) continue;
    if (usedLits.find(-*i) != usedLits.end()) {
      usedVars.insert(*i);
    }
#else
    usedVars.insert(abs(*i));
#endif
  }

  map<Var, Var> varMap;
#ifdef DO_PURE_LITERAL_ELIMINATION
  for (set<Lit>::const_iterator i = usedLits.begin(); i != usedLits.end(); ++i) {
    if (usedLits.find(-*i) == usedLits.end() && interfaceUsedVars.find(abs(*i)) == interfaceUsedVars.end()) {
      varMap[abs(*i)] = ((*i < 0) ? FALSE : TRUE);
      usedVars.erase(abs(*i));
    }
  }
#endif

  size_t c = 1;
  for (set<Var>::const_iterator i = usedVars.begin(); i != usedVars.end(); ++i, ++c) {
    varMap[*i] = c;
#if 0
    if (varMap[*i] != c) {
      printf("c varmap %d %zu\n", *i, c);
    }
#endif
  }

  for (size_t i = 0; i < cnf.interfaceVariables.size(); ++i) {
    InterfaceVariable& iv = cnf.interfaceVariables[i];
    for (size_t j = 0; j < iv.second.size(); ++j) {
      Lit oldLit = iv.second[j];
      assert (oldLit == -oldLit || varMap[abs(oldLit)] != 0);
      Lit newLit = (oldLit < 0 ? invert(varMap[-oldLit]) : varMap[oldLit]);
      iv.second[j] = (oldLit == -oldLit ? oldLit : newLit);
    }
  }

  vector<Clause> oldClauses = cnf.clauses;
  cnf.clauses.clear();
  for (size_t i = 0; i < oldClauses.size(); ++i) {
    Clause cl;
    for (size_t j = 0; j < oldClauses[i].size(); ++j) {
      Lit oldLit = oldClauses[i][j];
      Lit newLit = (oldLit < 0 ? invert(varMap[-oldLit]) : varMap[oldLit]);
      // These two lines do pure literal elimination
      if (newLit == FALSE) continue;
      if (newLit == TRUE) goto skipClause;
      cl.push_back(newLit);
    }
    cnf.addClause(cl);
    skipClause: ;
  }

  cnf.numVariables = c;
  cnf.unparse(stdout);
  return 0;
}
