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

enum SubsumptionStatus {
  A_SUBSUMES_B, B_SUBSUMES_A, A_AND_B_ARE_EQUAL, INCOMPARABLE
};

SubsumptionStatus checkSubsumption(const vector<signed char>& a, const vector<signed char>& b) {
  assert (a.size() == b.size());
  bool a_is_subset_of_b = true, b_is_subset_of_a = true;
  for (size_t i = 0; i < a.size(); ++i) {
    b_is_subset_of_a = b_is_subset_of_a && !(b[i] == 0 && a[i] != 0);
    a_is_subset_of_b = a_is_subset_of_b && !(a[i] == 0 && b[i] != 0);
    if (!b_is_subset_of_a && !a_is_subset_of_b) return INCOMPARABLE;
  }
  if (a_is_subset_of_b) {
    return b_is_subset_of_a ? A_AND_B_ARE_EQUAL : A_SUBSUMES_B;
  } else {
    return b_is_subset_of_a ? B_SUBSUMES_A : INCOMPARABLE;
  }
}

bool resolve(const vector<signed char>& a, const vector<signed char>& b, vector<signed char>& out) {
  out.clear();
  out.resize(a.size(), 0);
  assert (a.size() == b.size());
  unsigned int matches = 0;
  for (size_t i = 0; i < a.size(); ++i) {
    static const signed char outputs[9] = {-1, -1, 0, -1, 0, 1, 0, 1, 1};
    static const unsigned char addToMatch[9] = {0, 0, 1, 0, 0, 0, 1, 0, 0};
    unsigned int idx = a[i] * 3 + b[i] + 4;
    out[i] = outputs[idx];
    matches += (unsigned int)addToMatch[idx];
  }
  if (matches != 1) return false;
  return true;
}

void doAllMinimizations(vector<vector<signed char> >& signMatrix) {
  for (size_t i = 0; i < signMatrix.size(); ++i) { // size may be updated
    // fprintf(stderr, "signMatrix.size() == %zu\n", signMatrix.size());
    for (size_t j = 0; j < i; ++j) {
      vector<signed char> res;
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

  set<vector<signed char> > worklist(signMatrix.begin(), signMatrix.end());
  while (!worklist.empty()) {
    // fprintf(stderr, "Worklist size = %zu\n", worklist.size());
    vector<signed char> a = *worklist.begin();
    worklist.erase(worklist.begin());
    vector<bool> keep(signMatrix.size(), true);
    for (size_t j = 0; j < signMatrix.size(); ++j) {
      if (keep[j] == false) continue;
      const vector<signed char>& b = signMatrix[j];
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
      vector<vector<signed char> > newSignMatrix;
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

  vector<vector<signed char> > temp(input.size());
  for (size_t i = 0; i < input.size(); ++i) {
    const Clause& cl = input[i];
    vector<signed char> sc(variables.size(), 0);
    for (size_t j = 0; j < cl.size(); ++j) {
      Lit l = cl[j];
      size_t idx = find(variables.begin(), variables.end(), abs(l)) - variables.begin();
      sc[idx] = (l < 0 ? -1 : 1);
    }
    temp[i] = sc;
  }

  doAllMinimizations(temp);

  for (size_t i = 0; i < temp.size(); ++i) {
    Clause cl;
    for (size_t j = 0; j < variables.size(); ++j) {
      if (temp[i][j] != 0) {
        cl.push_back((temp[i][j] < 0) ? -variables[j] : variables[j]);
      }
    }
    output.push_back(cl);
  }
}

int main(int, char**) {
  int nvars;
  size_t nclauses;
  scanf("p cnf %d %zu\n", &nvars, &nclauses);
  vector<Clause> clauses(nclauses);
  vector<bool> liveClauses(nclauses, true);
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
  set<Var> variablesToCheck;
  for (size_t i = 1; i <= (size_t)nvars; ++i) {
    variablesToCheck.insert(i);
  }

  while (changed && passCount < 50) {
    changed = false;
    ++passCount;
    fprintf(stderr, "Pass %zu\n", passCount);

    vector<vector<Var> > variableCoincidences(nvars + 1);
    vector<vector<size_t> > variableUses(nvars + 1);
    for (size_t i = 0; i < clauses.size(); ++i) {
      if (!liveClauses[i]) continue;
      const Clause& cl = clauses[i];
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
      if (edgeList.size() > 60) continue;
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
        const Clause& cl = clauses[*j];
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
      vector<Clause> newClausesTemp;
      vector<Clause> newClausesTemp2;
      minimizeCnf(oldClausesTemp, newClausesTemp2);
      for (size_t x = 0; x < newClausesTemp2.size(); ++x) {
        if (find(newClausesTemp2[x].begin(), newClausesTemp2[x].end(), var) == newClausesTemp2[x].end() &&
            find(newClausesTemp2[x].begin(), newClausesTemp2[x].end(), -var) == newClausesTemp2[x].end()) {
          newClausesTemp.push_back(newClausesTemp2[x]);
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

    clauses.insert(clauses.end(), newClauses.begin(), newClauses.end());
    liveClauses.resize(clauses.size(), true);

    fprintf(stderr, "Removing %zu clause(s), adding %zu, failed to remove %zu\n", numRemoved, newClauses.size(), numRemovalFailed);

    if (numRemoved != 0 || newClauses.size() != 0) {
      changed = true;
      variablesToCheck = newVariablesToCheck;
    }

    fprintf(stderr, "Removed %zu clause(s), now have %zu\n", numRemoved, clauses.size());

  }

  vector<Clause> finalClauses;
  for (size_t i = 0; i < clauses.size(); ++i) {
    assert (i < liveClauses.size());
    if (liveClauses[i]) finalClauses.push_back(clauses[i]);
  }
  sort(finalClauses.begin(), finalClauses.end(), clauseLess);
  finalClauses.erase(unique(finalClauses.begin(), finalClauses.end()), finalClauses.end());

  fprintf(stderr, "Finished with %d variables and %zu clauses\n", nvars, finalClauses.size());

  printf("p cnf %d %zu\n", nvars, finalClauses.size());
  for (size_t i = 0; i < finalClauses.size(); ++i) {
    const Clause& cl = finalClauses[i];
    for (size_t j = 0; j < cl.size(); ++j) {
      printf("%d ", cl[j]);
    }
    printf("0\n");
  }

  return 0;
}
