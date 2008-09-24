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

using namespace std;

static const size_t maxVariableCountToSimplify = 64;

struct SimplificationClauseSet {
  bitset<maxVariableCountToSimplify> positive;
  bitset<maxVariableCountToSimplify> negative;
  // Invariant: positive & negative is empty

  friend bool operator==(const SimplificationClauseSet& a, const SimplificationClauseSet& b) {
    return a.positive == b.positive && a.negative == b.negative;
  }
};

enum SubsumptionStatus {
  A_SUBSUMES_B, B_SUBSUMES_A, A_AND_B_ARE_EQUAL, INCOMPARABLE
};

SubsumptionStatus checkSubsumption(const SimplificationClauseSet& a, const SimplificationClauseSet& b) {
  bool a_is_subset_of_b = (a.positive & b.positive) == a.positive && (a.negative & b.negative) == a.negative;
  bool b_is_subset_of_a = (a.positive & b.positive) == b.positive && (a.negative & b.negative) == b.negative;
  if (a_is_subset_of_b) {
    return b_is_subset_of_a ? A_AND_B_ARE_EQUAL : A_SUBSUMES_B;
  } else {
    return b_is_subset_of_a ? B_SUBSUMES_A : INCOMPARABLE;
  }
}

bool resolve(const SimplificationClauseSet& a, const SimplificationClauseSet& b, SimplificationClauseSet& out) {
  out.positive = a.positive | b.positive;
  out.negative = a.negative | b.negative;
  bitset<maxVariableCountToSimplify> variablesToResolveOnMask = out.positive & out.negative;
  if (variablesToResolveOnMask.count() != 1) return false;
  out.positive &= ~variablesToResolveOnMask;
  out.negative &= ~variablesToResolveOnMask;
  return true;
}

void doAllMinimizations(vector<SimplificationClauseSet>& signMatrix) {
  for (size_t i = 0; i < signMatrix.size(); ++i) { // size may be updated
    // fprintf(stderr, "signMatrix.size() == %zu\n", signMatrix.size());
    for (size_t j = 0; j < i; ++j) {
      SimplificationClauseSet res;
      bool didResolve = resolve(signMatrix[i], signMatrix[j], res);
      if (didResolve) {
        bool doInsert = true;
        for (size_t k = 0; k < signMatrix.size(); ++k) {
          SubsumptionStatus ss = checkSubsumption(signMatrix[k], res);
          if (ss == A_SUBSUMES_B || ss == A_AND_B_ARE_EQUAL) {
            doInsert = false;
            break;
          } else if (ss == B_SUBSUMES_A) {
            signMatrix[k] = res;
            doInsert = false;
            break;
          }
        }
        if (doInsert) {
          signMatrix.push_back(res);
        }
      }
    }
  }

  vector<SimplificationClauseSet> worklist(signMatrix.begin(), signMatrix.end());
  while (!worklist.empty()) {
    // fprintf(stderr, "Worklist size = %zu\n", worklist.size());
    SimplificationClauseSet a = worklist.back();
    worklist.pop_back();
    vector<bool> keep(signMatrix.size(), true);
    for (size_t j = 0; j < signMatrix.size(); ++j) {
      if (keep[j] == false) continue;
      const SimplificationClauseSet& b = signMatrix[j];
      switch (checkSubsumption(a, b)) {
        case A_SUBSUMES_B:
        case A_AND_B_ARE_EQUAL: {
          // Skip b
          // fprintf(stderr, "Removing %zu\n", j);
          keep[j] = false;
          goto nextJ;
        }
        case B_SUBSUMES_A: {
          // Skip a
          // fprintf(stderr, "Removing a\n");
          goto nextWorklistEntry;
        }
        case INCOMPARABLE: {
          break;
        }
      }
      nextJ: ;
    }
    {
      vector<SimplificationClauseSet> newSignMatrix;
      for (size_t i = 0; i < signMatrix.size(); ++i) {
        if (keep[i]) {
          newSignMatrix.push_back(signMatrix[i]);
        }
      }
      signMatrix.swap(newSignMatrix);
      signMatrix.push_back(a);
    }
    nextWorklistEntry: ;
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

  vector<SimplificationClauseSet> signMatrix2;
  for (size_t i = 0; i < signMatrix.size(); ++i) {
    if (signMatrixKeep[i]) {
      signMatrix2.push_back(signMatrix[i]);
    }
  }

  signMatrix.swap(signMatrix2);

#if 0
  fprintf(stderr, "Out:\n");
  for (size_t i = 0; i < signMatrix.size(); ++i) {
    for (size_t j = 0; j < signMatrix[i].size(); ++j) {
      fputc((signMatrix[i][j] == 0 ? ' ' : signMatrix[i][j] == 1 ? '+' : '-'), stderr);
    }
    fputc('\n', stderr);
  }
#endif
}

void minimizeCnf(const vector<Clause>& input, vector<Clause>& output) {
  vector<Var> variables;
  for (size_t i = 0; i < input.size(); ++i) {
    const Clause& cl = input[i];
    for (size_t j = 0; j < cl.size(); ++j) {
      variables.push_back(abs(cl[j]));
    }
  }
  sort(variables.begin(), variables.end());
  variables.erase(unique(variables.begin(), variables.end()), variables.end());

  assert (variables.size() <= maxVariableCountToSimplify);

  vector<SimplificationClauseSet> temp(input.size());
  for (size_t i = 0; i < input.size(); ++i) {
    const Clause& cl = input[i];
    SimplificationClauseSet& sc = temp[i];
    sc.positive.reset();
    sc.negative.reset();
    for (size_t j = 0; j < cl.size(); ++j) {
      Lit l = cl[j];
      size_t idx = find(variables.begin(), variables.end(), abs(l)) - variables.begin();
      if (l < 0) {
        sc.negative.set(idx);
      } else {
        sc.positive.set(idx);
      }
    }
  }

  doAllMinimizations(temp);

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
        for (size_t k = 0; k < cl.size(); ++k) {
          if (find(varsToExamine.begin(), varsToExamine.end(), abs(cl[k])) == varsToExamine.end()) {
            skipThisClause = true;
            break;
          }
        }
        if (skipThisClause) continue;
        clausesForThisVariable.insert(*j);
        oldClausesTemp.push_back(cl);
      }
      std::vector<Clause> newClausesTemp;
      std::vector<Clause> newClausesTemp2;
      minimizeCnf(oldClausesTemp, newClausesTemp2);
      if (frozenVars.find(var) != frozenVars.end()) {
        newClausesTemp = newClausesTemp2;
      } else {
        // Project out this variable
        for (size_t x = 0; x < newClausesTemp2.size(); ++x) {
          if (find(newClausesTemp2[x].begin(), newClausesTemp2[x].end(), var) == newClausesTemp2[x].end() &&
              find(newClausesTemp2[x].begin(), newClausesTemp2[x].end(), -var) == newClausesTemp2[x].end()) {
            newClausesTemp.push_back(newClausesTemp2[x]);
          }
        }
      }
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

    cnf.clauses.insert(cnf.clauses.end(), newClauses.begin(), newClauses.end());
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
