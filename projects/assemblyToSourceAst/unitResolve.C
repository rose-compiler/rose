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

  for (size_t i = 1; i < argc; ++i) {
    Lit l = atoi(argv[i]);
    units[l] = true;
    units[-l] = false;
  }

  bool changed = true;
  while (changed) {
    changed = false;
#if 0
    map<Lit, set<size_t> > literalUses;
#endif

    map<Lit, set<Lit> > binaryClauses;

    for (size_t i = 0; i < clauses.size(); ++i) {
      const Clause& cl = clauses[i];
#if 0
      for (size_t j = 0; j < cl.size(); ++j) {
        literalUses[cl[j]].insert(i);
      }
#endif
      if (cl.size() == 1) {
        units[cl[0]] = true;
        units[-cl[0]] = false;
      }
      if (cl.size() == 2) {
        binaryClauses[cl[0]].insert(cl[1]);
        binaryClauses[cl[1]].insert(cl[0]);
      }
    }

    size_t unitsAddedFromBinary = 0;
    for (map<Lit, set<Lit> >::const_iterator i = binaryClauses.begin();
         i != binaryClauses.end(); ++i) {
      const set<Lit>& ls = i->second;
      for (set<Lit>::const_iterator j = ls.begin(); j != ls.end(); ++j) {
        if (ls.find(-*j) != ls.end()) {
          units[i->first] = true;
          units[-i->first] = false;
          ++unitsAddedFromBinary;
          changed = true;
          break;
        }
      }
    }
    fprintf(stderr, "Added %zu unit(s) from binary resolution\n", unitsAddedFromBinary);

    vector<Clause> newClauses;
    size_t changedClauses = 0;
    for (size_t i = 0; i < clauses.size(); ++i) {
      const Clause& cl = clauses[i];
      Clause newCl;
      bool thisClauseChanged = false;
      for (size_t j = 0; j < cl.size(); ++j) {
        map<Lit, bool>::const_iterator k = units.find(cl[j]);
        if (k != units.end()) {
          thisClauseChanged = true;
          bool val = k->second;
          if (val) {
            goto skipThisClause;
          } else {
            continue; // skip literal
          }
        } else {
          newCl.push_back(cl[j]);
        }
      }
      newClauses.push_back(newCl);
      skipThisClause:
      if (thisClauseChanged) ++changedClauses;
    }

    if (changedClauses != 0) {
      changed = true;
      clauses = newClauses;
    }

    fprintf(stderr, "Units so far = %zu, changed this pass = %zu\n", units.size() / 2, changedClauses);
  }

  printf("p cnf %d %zu\n", nvars, clauses.size());
  for (size_t i = 0; i < clauses.size(); ++i) {
    const Clause& cl = clauses[i];
    for (size_t j = 0; j < cl.size(); ++j) {
      printf("%d ", cl[j]);
    }
    printf("0\n");
  }

  return 0;
}
