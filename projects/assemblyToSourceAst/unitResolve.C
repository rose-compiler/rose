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

using namespace std;

int main(int argc, char** argv) {
  SatProblem cnf;
  cnf.parse(stdin);
  vector<bool> clausesToRemove(cnf.clauses.size(), false);

  map<Lit, bool> units;

  map<Lit, set<size_t> > literalUses;
  map<Lit, set<Lit> > binaryClauses;
  set<size_t> worklist;

#define ADD_UNIT(lit) do { \
  units.insert(make_pair((lit), true)); \
  units.insert(make_pair(-(lit), false)); \
  worklist.insert(literalUses[(lit)].begin(), literalUses[(lit)].end()); \
  worklist.insert(literalUses[-(lit)].begin(), literalUses[-(lit)].end()); \
  /* fprintf(stderr, "Added unit %d\n", (lit)); */ \
} while (0)

  for (size_t i = 0; i < cnf.clauses.size(); ++i) {
    if (clausesToRemove[i]) continue;
    const Clause& cl = cnf.clauses[i];
    for (size_t j = 0; j < cl.size(); ++j) {
      literalUses[cl[j]].insert(i);
    }
  }

  for (size_t i = 0; i < cnf.clauses.size(); ++i) {
    if (clausesToRemove[i]) continue;
    const Clause& cl = cnf.clauses[i];
    if (cl.size() == 1) {
      ADD_UNIT(cl[0]);
    }
    if (cl.size() == 2) {
      binaryClauses[cl[0]].insert(cl[1]);
      binaryClauses[cl[1]].insert(cl[0]);
    }
  }

  size_t unitsAddedFromBinary = 0;
  for (map<Lit, set<Lit> >::iterator i = binaryClauses.begin();
      i != binaryClauses.end(); ++i) {
    const set<Lit>& ls = i->second;
    for (set<Lit>::const_iterator j = ls.begin(); j != ls.end(); ++j) {
      if (ls.find(-*j) != ls.end() && units.find(i->first) == units.end()) {
        ADD_UNIT(i->first);
        ++unitsAddedFromBinary;
        break;
      }
    }
  }
  fprintf(stderr, "Added %zu unit(s) from binary resolution\n", unitsAddedFromBinary);

  fprintf(stderr, "Units so far = %zu\n", units.size() / 2);

  while (!worklist.empty()) {
    size_t i = *worklist.begin();
    worklist.erase(worklist.begin());

    if (clausesToRemove[i] == true) continue;
    Clause& cl = cnf.clauses[i];
    Clause newCl;
    bool thisClauseChanged = false;
    for (size_t j = 0; j < cl.size(); ++j) {
      literalUses[cl[j]].erase(i);
    }
    for (size_t j = 0; j < cl.size(); ++j) {
      map<Lit, bool>::const_iterator k = units.find(cl[j]);
      if (k != units.end()) {
        binaryClauses[cl[j]].erase(i);
        thisClauseChanged = true;
        bool val = k->second;
        if (val) {
          clausesToRemove[i] = true;
          goto skipThisClause;
        } else {
          continue; // skip literal
        }
      } else {
        newCl.push_back(cl[j]);
        literalUses[cl[j]].insert(i);
      }
    }
    cl = newCl;
    if (cl.size() == 1) {
      ADD_UNIT(cl[0]);
    }
    if (cl.size() == 2) {
      binaryClauses[cl[0]].insert(cl[1]);
      binaryClauses[cl[1]].insert(cl[0]);
      if (binaryClauses[cl[0]].find(-cl[1]) != binaryClauses[cl[0]].end()) {
        ADD_UNIT(cl[0]);
      }
      if (binaryClauses[cl[1]].find(-cl[0]) != binaryClauses[cl[1]].end()) {
        ADD_UNIT(cl[1]);
      }
    }
    skipThisClause:
    ;
  }

  fprintf(stderr, "Units at end = %zu\n", units.size() / 2);

  vector<Clause> newClauses;
  // newClauses.reserve(clauses.size());
  for (size_t i = 0; i < cnf.clauses.size(); ++i) {
    if (clausesToRemove[i] == false) {
      newClauses.push_back(cnf.clauses[i]);
    }
  }
  cnf.clauses = newClauses;

  for (size_t i = 0; i < cnf.interfaceVariables.size(); ++i) {
    InterfaceVariable& iv = cnf.interfaceVariables[i];
    for (size_t j = 0; j < iv.second.size(); ++j) {
      map<Lit, bool>::const_iterator it = units.find(iv.second[j]);
      if (it == units.end()) continue;
      iv.second[j] = it->second ? TRUE : FALSE;
    }
  }

  cnf.unparse(stdout);
  return 0;
}
