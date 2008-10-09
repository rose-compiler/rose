#include <cassert>
#include <cstdio>
#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <bitset>
#include "satProblem.h"
#include <boost/smart_ptr.hpp>

using namespace std;

static const size_t maxVariableCountToSimplify = 64;
static const bool doPrint = true;

typedef bitset<maxVariableCountToSimplify> SimplificationClauseSetEntry;

struct SimplificationClauseSet {
  SimplificationClauseSetEntry positive;
  SimplificationClauseSetEntry negative;
  // Invariant: positive & negative is empty

  friend bool operator==(const SimplificationClauseSet& a, const SimplificationClauseSet& b) {
    return a.positive == b.positive && a.negative == b.negative;
  }
};

static inline bool aSubsumesB(const SimplificationClauseSet& a, const SimplificationClauseSet& b) {
  bool a_is_subset_of_b = (a.positive & b.positive) == a.positive && (a.negative & b.negative) == a.negative;
  return a_is_subset_of_b;
}

static inline bool resolve(const SimplificationClauseSet& a, const SimplificationClauseSet& b, SimplificationClauseSet& out) {
  SimplificationClauseSetEntry outPositive = a.positive | b.positive;
  SimplificationClauseSetEntry outNegative = a.negative | b.negative;
  SimplificationClauseSetEntry variablesToResolveOnMask = outPositive & outNegative;
  if (variablesToResolveOnMask.none()) return false;
  // From PowerPC Compiler Writer's Guide: check whether at most one bit of variablesToResolveOnMask is set
  unsigned long variablesToResolveOnMaskLong = variablesToResolveOnMask.to_ulong();
  if ((variablesToResolveOnMaskLong & (variablesToResolveOnMaskLong - 1)) != 0) return false;
  out.positive = outPositive & ~variablesToResolveOnMask;
  out.negative = outNegative & ~variablesToResolveOnMask;
  return true;
}

void printSCS(SimplificationClauseSet scs) {
  for (size_t j = 0; j < maxVariableCountToSimplify; ++j) {
    char c;
    if (scs.positive[j]) {
      if (scs.negative[j]) {
        c = 'X';
      } else {
        c = '+';
      }
    } else if (scs.negative[j]) {
      c = '-';
    } else {
      c = ' ';
    }
    fputc(c, stderr);
  }
  fputc('\n', stderr);
}

void printSignMatrix(const vector<SimplificationClauseSet>& signMatrix) {
  for (size_t i = 0; i < signMatrix.size(); ++i) {
    printSCS(signMatrix[i]);
  }
}

void doAllMinimizations(vector<SimplificationClauseSet>& signMatrix, const vector<unsigned int>& variablesToProjectOut) {
  for (size_t i = 0; i < signMatrix.size(); ++i) { // size may be updated
    // fprintf(stderr, "signMatrix.size() == %zu\n", signMatrix.size());
    for (size_t j = 0; j < i; ++j) {
      SimplificationClauseSet res;
      bool didResolve = resolve(signMatrix[i], signMatrix[j], res);
      if (didResolve) {
#if 0
        if (doPrint) {
          fprintf(stderr, "Trying to add\n");
          printSCS(res);
          fprintf(stderr, "from\n");
          printSCS(signMatrix[i]);
          printSCS(signMatrix[j]);
        }
#endif
        bool doInsert = true;
        for (size_t k = 0; k < signMatrix.size(); ++k) {
          if (aSubsumesB(signMatrix[k], res)) {
            doInsert = false;
#if 0
            if (doPrint) {
              fprintf(stderr, "Subsumed by\n");
              printSCS(signMatrix[k]);
            }
#endif
            break;
          } else if (aSubsumesB(res, signMatrix[k])) {
            signMatrix[k] = res;
            doInsert = false;
#if 0
            if (doPrint) {
              fprintf(stderr, "Subsumes (replacing)\n");
              printSCS(signMatrix[k]);
            }
#endif
            break;
          }
        }
        if (doInsert) {
#if 0
          if (doPrint) {
            fprintf(stderr, "Adding\n");
            printSCS(res);
          }
#endif
          signMatrix.push_back(res);
        }
      }
    }
  }

  {
    vector<bool> keep(signMatrix.size(), true);
    for (size_t i = 0; i < signMatrix.size(); ++i) {
      if (keep[i] == false) continue;
      SimplificationClauseSet a = signMatrix[i];
      for (size_t j = 0; j < i; ++j) {
        if (keep[j] == false) continue;
        SimplificationClauseSet b = signMatrix[j];
        if (aSubsumesB(a, b)) {
          // Skip b
#if 0
          if (doPrint) {
            fprintf(stderr, "Removing\n");
            printSCS(b);
            fprintf(stderr, "Subsumed by\n");
            printSCS(a);
          }
#endif
          keep[j] = false;
        } else if (aSubsumesB(b, a)) {
          // Skip a
          // fprintf(stderr, "Removing a\n");
#if 0
          if (doPrint) {
            fprintf(stderr, "Removing\n");
            printSCS(a);
            fprintf(stderr, "Subsumed by\n");
            printSCS(b);
          }
#endif
          keep[i] = false;
          goto nextI;
        } else {}
      }
      nextI: ;
    }
    {
      size_t newSignMatrixSize = 0;
      for (size_t i = 0; i < signMatrix.size(); ++i) {
        if (keep[i]) {
          signMatrix[newSignMatrixSize++] = signMatrix[i];
        }
      }
      signMatrix.resize(newSignMatrixSize);
    }
  }

  // Project out (existentially quantify) variables from variablesToProjectOut;
  // this must be done before the filtering done next
  {
    size_t newSignMatrixSize = 0;
    for (size_t i = 0; i < signMatrix.size(); ++i) {
      for (size_t j = 0; j < variablesToProjectOut.size(); ++j) {
        unsigned int var = variablesToProjectOut[j];
        if (signMatrix[i].positive[var] || signMatrix[i].negative[var]) {
          goto clauseQuantifiedOut;
        }
      }
      signMatrix[newSignMatrixSize++] = signMatrix[i];
      clauseQuantifiedOut: ;
    }
    signMatrix.resize(newSignMatrixSize);
  }

  // Filter out redundant clauses (those that are resolvents of other kept clauses
  vector<bool> signMatrixKeep(signMatrix.size(), true);
  for (size_t i = 0; i < signMatrix.size(); ++i) {
    // Resolvents of non-kept clauses (i.e., those that are themselves redundant) are also redundant
    // if (!signMatrixKeep[i]) continue;
    SimplificationClauseSet cs1 = signMatrix[i];
    for (size_t j = 0; j < i; ++j) {
      // if (!signMatrixKeep[j]) continue;
      SimplificationClauseSet cs2 = signMatrix[j];
      SimplificationClauseSet res;
      bool resolutionSucceeded = resolve(cs1, cs2, res);
      if (!resolutionSucceeded) continue;
      for (size_t k = 0; k < signMatrix.size(); ++k) {
        if (res == signMatrix[k]) {
          signMatrixKeep[k] = false;
        }
      }
    }
  }

  size_t newSignMatrixSize = 0;
  for (size_t i = 0; i < signMatrix.size(); ++i) {
    if (signMatrixKeep[i]) {
      signMatrix[newSignMatrixSize++] = signMatrix[i];
    }
  }
  signMatrix.resize(newSignMatrixSize);
}

void minimizeCnf(const vector<Clause>& input, vector<Clause>& output, const vector<Var>& variablesToProjectOut) {
  vector<Var> variables;
  for (size_t i = 0; i < input.size(); ++i) {
    const Clause& cl = input[i];
    for (size_t j = 0; j < cl.size(); ++j) {
      variables.push_back(abs(cl[j]));
    }
  }
  sort(variables.begin(), variables.end());
  variables.erase(unique(variables.begin(), variables.end()), variables.end());
#if 0
  if (doPrint) {
    fprintf(stderr, "variables =");
    for (size_t i = 0; i < variables.size(); ++i) {
      fprintf(stderr, " %d", variables[i]);
    }
    fprintf(stderr, "\n");
  }
#endif

  assert (variables.size() <= maxVariableCountToSimplify);

  vector<SimplificationClauseSet> temp(input.size());
  for (size_t i = 0; i < input.size(); ++i) {
    const Clause& cl = input[i];
    SimplificationClauseSet& sc = temp[i];
    sc.positive.reset();
    sc.negative.reset();
    for (size_t j = 0; j < cl.size(); ++j) {
      Lit l = cl[j];
      pair<vector<Var>::const_iterator, vector<Var>::const_iterator> range = equal_range(variables.begin(), variables.end(), abs(l));
      assert (range.first != range.second);
      size_t idx = range.first - variables.begin();
      if (l < 0) {
        sc.negative.set(idx);
      } else {
        sc.positive.set(idx);
      }
    }
  }

#if 1
  vector<SimplificationClauseSet> oldTemp = temp;
#endif

  vector<unsigned int> variablesToProjectOutInGroup(variablesToProjectOut.size());
  for (size_t i = 0; i < variablesToProjectOut.size(); ++i) {
    variablesToProjectOutInGroup[i] = find(variables.begin(), variables.end(), variablesToProjectOut[i]) - variables.begin();
  }
  doAllMinimizations(temp, variablesToProjectOutInGroup);

#if 1
  if (doPrint && temp != oldTemp) {
    fprintf(stderr, "In:\n");
    printSignMatrix(oldTemp);
    fprintf(stderr, "Out:\n");
    printSignMatrix(temp);
  }
#endif

  for (size_t i = 0; i < temp.size(); ++i) {
    Clause cl;
    for (size_t j = 0; j < variables.size(); ++j) {
      if (temp[i].positive.test(j)) {
        cl.push_back(variables[j]);
      } else if (temp[i].negative.test(j)) {
        cl.push_back(-variables[j]);
      }
    }
    output.push_back(cl);
  }
}

int main(int argc, char** argv) {
  assert (argc == 2);
  size_t variableLimit = (size_t)atoi(argv[1]);
  assert (variableLimit <= maxVariableCountToSimplify);
  SatProblem cnf;
  cnf.parse(stdin);
  set<Var> frozenVars;
  vector<bool> liveClauses(cnf.clauses.size(), true);
  for (size_t i = 0; i < cnf.interfaceVariables.size(); ++i) {
    const vector<Lit>& lits = cnf.interfaceVariables[i].second;
    for (size_t j = 0; j < lits.size(); ++j) {
      Lit l = lits[j];
      if (l != -l) {
        frozenVars.insert(abs(l));
      }
    }
  }

  bool changed = true;
  size_t passCount = 0;
  set<Var> variablesToCheck;
  for (size_t i = 1; i <= cnf.numVariables; ++i) {
    variablesToCheck.insert(i);
  }

  while (changed && passCount < 50) {
    changed = false;
    ++passCount;
    fprintf(stderr, "Pass %zu\n", passCount);

    std::vector<std::vector<Var> > variableCoincidences(cnf.numVariables + 1);
    std::vector<std::vector<size_t> > variableUses(cnf.numVariables + 1);
    for (size_t i = 0; i < cnf.clauses.size(); ++i) {
      if (!liveClauses[i]) continue;
      const Clause& cl = cnf.clauses[i];
      for (size_t j = 0; j < cl.size(); ++j) {
        Var varJ = abs(cl[j]);
        variableUses[varJ].push_back(i);
        for (size_t k = 0; k < j; ++k) {
          Var varK = abs(cl[k]);
          variableCoincidences[varJ].push_back(varK);
          variableCoincidences[varK].push_back(varJ);
        }
      }
    }
    fprintf(stderr, "Finished making tables\n");

    size_t numRemoved = 0;
    size_t numRemovalFailed = 0;
    vector<Clause> newClauses; // Some new clauses may be added here to replace those simplified out
    set<Var> newVariablesToCheck;
    fprintf(stderr, "Checking %zu variable(s)\n", variablesToCheck.size());
    for (set<Var>::const_iterator i = variablesToCheck.begin(); i != variablesToCheck.end(); ++i) {
      Var var = *i;
      if (var % 1000 == 0) fprintf(stderr, "Var %d\n", var);
      vector<Var>& edgeList = variableCoincidences[*i];
      sort(edgeList.begin(), edgeList.end());
      edgeList.erase(unique(edgeList.begin(), edgeList.end()), edgeList.end());
      if (edgeList.size() > variableLimit) continue;
      vector<Var> varsToExamine = edgeList;
      varsToExamine.insert(varsToExamine.begin(), var);
      // fprintf(stderr, "Variable %d has degree %zu\n", var, edgeList.size());
      set<size_t> clausesForThisVariable;
      vector<Clause> oldClausesTemp;
      vector<size_t> clausesToCheck = variableUses[var];
      for (size_t j = 0; j < edgeList.size(); ++j) {
        clausesToCheck.insert(clausesToCheck.end(), variableUses[edgeList[j]].begin(), variableUses[edgeList[j]].end());
      }
      sort(clausesToCheck.begin(), clausesToCheck.end());
      clausesToCheck.erase(unique(clausesToCheck.begin(), clausesToCheck.end()), clausesToCheck.end());
      for (vector<size_t>::const_iterator j = clausesToCheck.begin(); j != clausesToCheck.end(); ++j) {
        const Clause& cl = cnf.clauses[*j];
        // We now process any clause that contains only variables that are in edgeList, not just those that actually contain the current variable
        bool skipThisClause = !liveClauses[*j];
        for (size_t k = 0; !skipThisClause && k < cl.size(); ++k) {
          if (!binary_search(edgeList.begin(), edgeList.end(), abs(cl[k])) &&
              abs(cl[k]) != var) {
            skipThisClause = true;
            break;
          }
        }
        if (skipThisClause) continue;
        clausesForThisVariable.insert(*j);
        oldClausesTemp.push_back(cl);
      }
      std::vector<Clause> newClausesTemp;
#if 0
      if (find(varsToExamine.begin(), varsToExamine.end(), 131) != varsToExamine.end()) {
        fprintf(stderr, "varsToExamine =");
        for (size_t i = 0; i < varsToExamine.size(); ++i) {
          fprintf(stderr, " %d", varsToExamine[i]);
        }
        fprintf(stderr, "\n");
      }
#endif
      vector<Var> variablesToProjectOut;
      if (frozenVars.find(var) == frozenVars.end()) {
        variablesToProjectOut.push_back(var);
      }
      minimizeCnf(oldClausesTemp, newClausesTemp, variablesToProjectOut);
      int oldScore = 0;
      for (size_t x = 0; x < oldClausesTemp.size(); ++x) {
        oldScore += (int)oldClausesTemp[x].size() - 1;
        if (oldClausesTemp[x].size() == 1) --oldScore;
      }
      int newScore = 0;
      for (size_t x = 0; x < newClausesTemp.size(); ++x) {
        newScore += (int)newClausesTemp[x].size() - 1;
        if (newClausesTemp[x].size() == 1) --newScore;
      }
      bool reduced = (newScore < oldScore);
      if (reduced) {
        newClauses.insert(newClauses.end(), newClausesTemp.begin(), newClausesTemp.end());
        for (set<size_t>::const_iterator x = clausesForThisVariable.begin(); x != clausesForThisVariable.end(); ++x) {
          if (liveClauses[*x]) {
            ++numRemoved;
            liveClauses[*x] = false;
          }
        }
        newVariablesToCheck.insert(edgeList.begin(), edgeList.end());
      } else {
        ++numRemovalFailed;
      }
    }

    for (size_t i = 0; i < newClauses.size(); ++i) {
      cnf.addClause(newClauses[i]);
    }
    liveClauses.resize(cnf.clauses.size(), true);

    fprintf(stderr, "Removing %zu clause(s), adding %zu, failed to remove %zu\n", numRemoved, newClauses.size(), numRemovalFailed);

    if (numRemoved != 0 || newClauses.size() != 0) {
      changed = true;
      variablesToCheck = newVariablesToCheck;
    }

    fprintf(stderr, "Removed %zu clause(s), now have %zu\n", numRemoved, cnf.clauses.size());

  }

  std::vector<Clause> finalClauses;
  for (size_t i = 0; i < cnf.clauses.size(); ++i) {
    assert (i < liveClauses.size());
    if (liveClauses[i]) finalClauses.push_back(cnf.clauses[i]);
  }
  cnf.clauses = finalClauses;
  cnf.unparse(stdout);

  return 0;
}
