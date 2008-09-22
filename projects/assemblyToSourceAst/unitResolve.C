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

typedef int Var;
typedef int Lit; // DIMACS convention
typedef vector<Lit> Clause;

bool clauseLess(const Clause& a, const Clause& b) {
  Clause absa(a.size()), absb(b.size());
  for (size_t i = 0; i < a.size(); ++i) {
    absa[i] = abs(a[i]);
  }
  for (size_t i = 0; i < b.size(); ++i) {
    absb[i] = abs(b[i]);
  }
  if (absa < absb) return true;
  if (absa > absb) return false;
  return a < b;
}

int main(int argc, char** argv) {
  int nvars;
  size_t nclauses;
  scanf("p cnf %d %zu\n", &nvars, &nclauses);
  vector<Clause> clauses(nclauses);
  vector<bool> clausesToRemove(nclauses, false);
  for (size_t i = 0; i < clauses.size(); ++i) {
    Clause& cl = clauses[i];
    while (true) {
      Lit lit;
      scanf("%d", &lit);
      if (lit == 0) {
        break;
      } else {
        cl.push_back(lit);
      }
    }
  }

  fprintf(stderr, "Starting with %d var(s) and %zu clause(s)\n", nvars, nclauses);

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

  for (size_t i = 0; i < clauses.size(); ++i) {
    if (clausesToRemove[i]) continue;
    const Clause& cl = clauses[i];
    for (size_t j = 0; j < cl.size(); ++j) {
      literalUses[cl[j]].insert(i);
    }
  }

  for (size_t i = 0; i < clauses.size(); ++i) {
    if (clausesToRemove[i]) continue;
    const Clause& cl = clauses[i];
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
    Clause& cl = clauses[i];
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
  for (size_t i = 0; i < clauses.size(); ++i) {
    if (clausesToRemove[i] == false) {
      newClauses.push_back(clauses[i]);
    }
  }
  printf("p cnf %d %zu\n", nvars, newClauses.size());
  for (size_t i = 0; i < newClauses.size(); ++i) {
    const Clause& cl = newClauses[i];
    for (size_t j = 0; j < cl.size(); ++j) {
      printf("%d ", cl[j]);
    }
    printf("0\n");
  }

  return 0;
}
