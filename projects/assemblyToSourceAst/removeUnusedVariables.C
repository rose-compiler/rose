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

#undef DO_PURE_LITERAL_ELIMINATION

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

#ifndef DO_PURE_LITERAL_ELIMINATION
  for (size_t i = 0; i < cnf.interfaceVariables.size(); ++i) {
    InterfaceVariable& iv = cnf.interfaceVariables[i];
    for (size_t j = 0; j < iv.second.size(); ++j) {
      Lit oldLit = iv.second[j];
      if (oldLit != -oldLit) {
        usedVars.insert(abs(oldLit));
      }
    }
  }
#endif

  map<Var, Var> varMap;
  size_t c = 1;
  for (set<Var>::const_iterator i = usedVars.begin(); i != usedVars.end(); ++i, ++c) {
    varMap[*i] = c;
#if 0
    if (varMap[*i] != c) {
      printf("c varmap %d %zu\n", *i, c);
    }
#endif
  }
#ifdef DO_PURE_LITERAL_ELIMINATION
  for (set<Lit>::const_iterator i = usedLits.begin(); i != usedLits.end(); ++i) {
    if (usedLits.find(-*i) == usedLits.end()) {
      varMap[abs(*i)] = ((*i < 0) ? FALSE : TRUE);
    }
  }
#endif

  for (size_t i = 0; i < cnf.interfaceVariables.size(); ++i) {
    InterfaceVariable& iv = cnf.interfaceVariables[i];
    for (size_t j = 0; j < iv.second.size(); ++j) {
      Lit oldLit = iv.second[j];
      Lit newLit = (oldLit < 0 ? invert(varMap[-oldLit]) : varMap[oldLit]);
      iv.second[j] = newLit;
    }
  }

  vector<Clause> newClauses;
  for (size_t i = 0; i < cnf.clauses.size(); ++i) {
    Clause cl;
    for (size_t j = 0; j < cnf.clauses[i].size(); ++j) {
      Lit oldLit = cnf.clauses[i][j];
      Lit newLit = (oldLit < 0 ? invert(varMap[-oldLit]) : varMap[oldLit]);
      // These two lines do pure literal elimination
      if (newLit == FALSE) continue;
      if (newLit == TRUE) goto skipClause;
      cl.push_back(newLit);
    }
    newClauses.push_back(cl);
    skipClause: ;
  }

  cnf.clauses = newClauses;
  cnf.numVariables = c;

  cnf.unparse(stdout);

  return 0;
}
