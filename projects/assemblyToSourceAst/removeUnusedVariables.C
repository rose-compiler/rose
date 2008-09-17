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

int main(int, char**) {
  int nvars;
  size_t nclauses;
  scanf("p cnf %d %zu\n", &nvars, &nclauses);
  vector<Clause> clauses(nclauses);
  for (size_t i = 0; i < clauses.size(); ++i) {
    while (true) {
      Lit lit;
      scanf("%d", &lit);
      if (lit == 0) {
        break;
      } else {
        clauses[i].push_back(lit);
      }
    }
  }

  fprintf(stderr, "Starting with %d var(s) and %zu clause(s)\n", nvars, nclauses);

  set<Var> usedVars;
  for (size_t i = 0; i < clauses.size(); ++i) {
    const Clause& cl = clauses[i];
    for (size_t j = 0; j < cl.size(); ++j) {
      usedVars.insert(abs(cl[j]));
    }
  }

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

  printf("p cnf %zu %zu\n", c - 1, clauses.size());
  for (size_t i = 0; i < clauses.size(); ++i) {
    const Clause& cl = clauses[i];
    for (size_t j = 0; j < cl.size(); ++j) {
      printf("%d ", (cl[j] < 0 ? -varMap[-cl[j]] : varMap[cl[j]]));
    }
    printf("0\n");
  }

  return 0;
}
