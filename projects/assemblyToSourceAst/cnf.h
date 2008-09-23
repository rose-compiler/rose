#ifndef CNF_H
#define CNF_H

#include <vector>
#include <string>
#include <utility>
#include <algorithm>
#include <stdio.h>
#include <cassert>
#include <math.h>

typedef int Var;
typedef int Lit; // DIMACS convention
typedef std::vector<Lit> Clause;
const Lit TRUE = (int)0x80000000;
const Lit FALSE = 0;
typedef std::vector<Clause> ClauseList;
typedef std::pair<std::string, std::vector<Lit> > InterfaceVariable;
typedef std::vector<InterfaceVariable> InterfaceVariableList;

inline Lit inv(Lit l) {
  if (l == -l) { // TRUE or FALSE
    return l ^ (Lit)(0x80000000);
  } else {
    return -l;
  }
}

inline bool absLess(Lit a, Lit b) {
  Lit absa = abs(a);
  Lit absb = abs(b);
  if (absa < absb) return true;
  if (absa > absb) return false;
  return a < b;
}

static bool clauseLess(const Clause& a, const Clause& b) {
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

struct CNF {
  ClauseList clauses;
  InterfaceVariableList interfaceVariables;
  size_t nvars;

  void parse(FILE* f) {
    clauses.clear();
    interfaceVariables.clear();
    nvars = (size_t)(-1);
    while (true) {
      char* firstWord = NULL;
      fscanf(f, "%as", &firstWord);
      assert (firstWord);
      std::string fwString = firstWord;
      // fprintf(stderr, "firstWord = %s\n", firstWord);
      free(firstWord);
      if (fwString == "p") {
        break;
      } else if (fwString == "c") {
        const char* tag = NULL;
        size_t count = 0;
        fscanf(f, "%as %zu", &tag, &count);
        assert (tag);
        // fprintf(stderr, "tag=%s count=%zu\n", tag, count);
        std::vector<Lit> lits(count);
        for (size_t i = 0; i < count; ++i) {
          fscanf(f, "%d", &lits[i]);
        }
        interfaceVariables.push_back(std::make_pair(std::string(tag), lits));
      } else {
        assert (!"Bad first word");
      }
    }
    size_t nclauses;
    fscanf(f, " cnf %zu %zu\n", &nvars, &nclauses);
    assert (nvars != (size_t)(-1));
    // fprintf(stderr, "nvars=%zu nclauses=%zu\n", nvars, nclauses);
    clauses.resize(nclauses);
    for (size_t i = 0; i < clauses.size(); ++i) {
      Clause& cl = clauses[i];
      while (true) {
        Lit lit;
        fscanf(f, "%d", &lit);
        if (lit == 0) {
          break;
        } else {
          cl.push_back(lit);
        }
      }
    }
  }

  void unparse(FILE* f) {
    sort(clauses.begin(), clauses.end(), clauseLess);
    clauses.erase(std::unique(clauses.begin(), clauses.end()), clauses.end());
    for (size_t i = 0; i < interfaceVariables.size(); ++i) {
      fprintf(f, "c %s %zu", interfaceVariables[i].first.c_str(), interfaceVariables[i].second.size());
      for (size_t j = 0; j < interfaceVariables[i].second.size(); ++j) {
        fprintf(f, " %d", interfaceVariables[i].second[j]);
      }
      fputc('\n', f);
    }
    fprintf(f, "p cnf %zu %zu\n", nvars, clauses.size());
    for (size_t i = 0; i < clauses.size(); ++i) {
      const Clause& cl = clauses[i];
      for (size_t j = 0; j < cl.size(); ++j) {
        fprintf(f, "%d ", cl[j]);
      }
      fprintf(f, "0\n");
    }
  }

};

#endif // CNF_H
