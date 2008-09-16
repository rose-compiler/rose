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

enum MinFlag {ZERO, ONE, DASH};

void minimizeCnf(uint64_t truthTable, const vector<Var>& edgeList, vector<Clause>& output) {
  // Quine-McCluskey algorithm from Wikipedia
  // Build DNF for negation of the function
  vector<set<vector<MinFlag> > > minterms(edgeList.size() + 1);
  for (unsigned long long mask = 1, combo = 0;
       combo < (1ULL << edgeList.size());
       mask <<= 2, ++combo) {
    if ((truthTable & mask) == 0) {
      size_t popCount = 0;
      vector<MinFlag> bitMasks(edgeList.size());
      for (size_t i = 0; i < edgeList.size(); ++i) {
        bitMasks[i] = (combo & (1 << i)) ? ONE : ZERO;
        if (bitMasks[i] == ONE) ++popCount;
      }
      minterms[popCount].insert(bitMasks);
    }
  }
  for (size_t pass = 0; pass < edgeList.size(); ++pass) {
    vector<set<vector<MinFlag> > > removedMinterms(edgeList.size() + 1);
    vector<set<vector<MinFlag> > > addedMinterms(edgeList.size() + 1);
    for (size_t pc = 0; pc < edgeList.size(); ++pc) {
      const set<vector<MinFlag> >& bucket0 = minterms[pc];
      const set<vector<MinFlag> >& bucket1 = minterms[pc + 1];
      for (set<vector<MinFlag> >::const_iterator i = bucket0.begin();
           i != bucket0.end(); ++i) {
        for (set<vector<MinFlag> >::const_iterator j = bucket1.begin();
             j != bucket1.end(); ++j) {
          const vector<MinFlag>& v0 = *i;
          const vector<MinFlag>& v1 = *j;
          vector<MinFlag> v2 = v0;
          size_t differences = 0;
          for (size_t k = 0; k < v0.size(); ++k) {
            if (v0[k] != v1[k]) {
              ++differences;
              v2[k] = DASH;
              if (differences >= 2) break;
            }
          }
          if (differences == 1) {
            addedMinterms[pc].insert(v2);
            removedMinterms[pc].insert(v0);
            removedMinterms[pc + 1].insert(v1);
          }
        }
      }
    }
    vector<set<vector<MinFlag> > > newMinterms(edgeList.size() + 1);
    for (size_t pc = 0; pc < minterms.size(); ++pc) {
      const set<vector<MinFlag> >& old = minterms[pc];
      const set<vector<MinFlag> >& removed = removedMinterms[pc];
      const set<vector<MinFlag> >& added = addedMinterms[pc];
      set<vector<MinFlag> >& newSet = newMinterms[pc];
      set_difference(old.begin(), old.end(), removed.begin(), removed.end(), inserter(newSet, newSet.end()));
      newSet.insert(added.begin(), added.end());
    }
    minterms = newMinterms;
  }
  // fprintf(stderr, "Minterm set for %" PRIX64 ":\n", truthTable);
  for (size_t pc = 0; pc < minterms.size(); ++pc) {
    for (set<vector<MinFlag> >::const_iterator i = minterms[pc].begin();
         i != minterms[pc].end(); ++i) {
      vector<Var> newClause;
      for (size_t j = 0; j < (*i).size(); ++j) {
        switch ((*i)[j]) {
          case ZERO: /* fputc('0', stderr); */ newClause.push_back(edgeList[j]); break;
          case ONE: /* fputc('1', stderr); */ newClause.push_back(-edgeList[j]); break;
          case DASH: /* fputc('-', stderr); */ break;
        }
      }
      // fputc('\n', stderr);
      output.push_back(newClause);
    }
  }
  // fprintf(stderr, "End of minterm list\n");
}

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

  bool changed = true;
  size_t passCount = 0;
  while (changed && passCount < 5) {
    changed = false;
    ++passCount;
    fprintf(stderr, "Pass %zu\n", passCount);

    map<Var, set<Var> > variableCoincidences;
    map<Var, vector<Clause> > variableUses;
    for (size_t i = 0; i < clauses.size(); ++i) {
      const Clause& cl = clauses[i];
      for (size_t j = 0; j < cl.size(); ++j) {
        Var varJ = abs(cl[j]);
        variableUses[varJ].push_back(cl);
        for (size_t k = 0; k < j; ++k) {
          Var varK = abs(cl[k]);
          variableCoincidences[varJ].insert(varK);
          variableCoincidences[varK].insert(varJ);
        }
      }
    }

    set<Var> variablesToRemove;
    size_t numRemovalFailed = 0;
    vector<Clause> newClauses; // Some new clauses may be added here to replace those simplified out
    for (map<Var, set<Var> >::const_iterator i = variableCoincidences.begin();
         i != variableCoincidences.end(); ++i) {
      if (i->second.size() > 5) continue;
      Var var = i->first;
      vector<Var> edgeList(i->second.begin(), i->second.end());
      // fprintf(stderr, "Variable %d has degree %zu\n", var, edgeList.size());
      uint64_t fullTruthTable = (1UL << (1 << (edgeList.size() + 1))) - 1;
      uint64_t truthTable = fullTruthTable;
      for (size_t j = 0; j < variableUses[var].size(); ++j) {
        const Clause& cl = variableUses[var][j];
        bool skipThisClause = true;
        for (size_t k = 0; k < cl.size(); ++k) {
          if (cl[k] == var || cl[k] == -var) {
            skipThisClause = false;
            break;
          }
        }
        if (skipThisClause) continue;
        uint64_t thisMask = 0ULL;
        for (size_t l = 0; l < cl.size(); ++l) {
          bool isNegated = (cl[l] < 0);
          size_t index = abs(cl[l]) == var ? 0 : (1 + find(edgeList.begin(), edgeList.end(), abs(cl[l])) - edgeList.begin());
          uint64_t thisMaskPart = 0ULL;
          switch (index) {
            case 0: thisMaskPart = 0xaaaaaaaaaaaaaaaaULL; break;
            case 1: thisMaskPart = 0xccccccccccccccccULL; break;
            case 2: thisMaskPart = 0xf0f0f0f0f0f0f0f0ULL; break;
            case 3: thisMaskPart = 0xff00ff00ff00ff00ULL; break;
            case 4: thisMaskPart = 0xffff0000ffff0000ULL; break;
            case 5: thisMaskPart = 0xffffffff00000000ULL; break;
            default: assert (!"Bad clause entry");
          }
          if (isNegated) thisMaskPart = ~thisMaskPart;
          thisMask |= thisMaskPart;
        }
        truthTable &= thisMask;
      }
      // existentially quantify the variable var (indexed by the LSB of the bit number within truthTable), and check if there are any false values left after this quantification
      bool anyFalse = (truthTable | (truthTable >> 1) | (fullTruthTable & 0xaaaaaaaaaaaaaaaaULL)) != fullTruthTable;
      uint64_t maskedTruthTable = (truthTable | (truthTable >> 1)) & 0x5555555555555555ULL;
      // fprintf(stderr, "anyFalse = %d\n", (int)anyFalse);
      if (anyFalse && edgeList.size() <= 5) {
        minimizeCnf(maskedTruthTable, edgeList, newClauses);
        variablesToRemove.insert(var);
      } else if (anyFalse) {
#if 0
        for (size_t j = 0; j < (1UL << edgeList.size()); ++j) { // Project out first variable
          fprintf(stderr, "Truth table entry %zx is %d/%d\n", j, (int)(truthTable >> (j << 1)) & 1, (int)(truthTable >> ((j << 1) | 1)) & 1);
        }
#endif
        ++numRemovalFailed;
      } else {
        variablesToRemove.insert(var);
      }
    }
    fprintf(stderr, "Removing %zu variable(s), failed to remove %zu\n", variablesToRemove.size(), numRemovalFailed);

    size_t clausesRemoved = 0;
    for (size_t i = 0; i < clauses.size(); ++i) {
      Clause& cl = clauses[i];
      bool remove = false;
      for (size_t j = 0; j < cl.size(); ++j) {
        if (variablesToRemove.find(abs(cl[j])) != variablesToRemove.end()) {
          remove = true;
          break;
        }
      }
      if (!remove) {
        changed = true;
        newClauses.push_back(cl);
      } else {
        ++clausesRemoved;
      }
    }

    clauses = newClauses;
    fprintf(stderr, "Removed %zu clause(s)\n", clausesRemoved);

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
