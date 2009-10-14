#include "satProblem.h"
#include <set>
#include <map>

using namespace std;

int main(int argc, char** argv) {
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

  map<Lit, vector<Clause> > clausesPerLit; // Each clause in RHS has had the LHS literal removed from it
  for (size_t i = 0; i < cnf.clauses.size(); ++i) {
    const Clause& cl = cnf.clauses[i];
    for (size_t j = 0; j < cl.size(); ++j) {
      Clause others;
      for (size_t k = 0; k < cl.size(); ++k) {
        if (cl[k] != cl[j]) others.push_back(cl[k]);
      }
      clausesPerLit[cl[j]].push_back(others);
    }
  }

  map<vector<Clause>, vector<Lit> > litsWithEachCombo; // LHS of map is sorted by clauseLess and each clause in it is sorted by absLess
  for (map<Lit, vector<Clause> >::const_iterator i = clausesPerLit.begin();
       i != clausesPerLit.end(); ++i) {
    vector<Clause> cls = i->second;
    for (size_t j = 0; j < cls.size(); ++j) {
      sort(cls[j].begin(), cls[j].end(), absLess);
    }
    sort(cls.begin(), cls.end(), clauseLess);
    cls.erase(unique(cls.begin(), cls.end()), cls.end());
    litsWithEachCombo[cls].push_back(i->first);
  }

  map<Lit, Lit> litMap;
  for (map<vector<Clause>, vector<Lit> >::const_iterator i = litsWithEachCombo.begin();
       i != litsWithEachCombo.end(); ++i) {
    vector<Lit> lits = i->second;
    if (lits.size() <= 1) continue;
    // If there is a frozen var in lits, move it to the front
    if (frozenVars.find(abs(lits[0])) == frozenVars.end()) {
      for (size_t j = 1; j < lits.size(); ++j) {
        if (frozenVars.find(abs(lits[j])) != frozenVars.end()) {
          swap(lits[0], lits[j]);
          break;
        }
      }
    }
    for (size_t j = 1; j < lits.size(); ++j) {
      if (frozenVars.find(abs(lits[j])) != frozenVars.end()) continue;
      fprintf(stderr, "Literal %d is redundant with %d\n", lits[j], lits[0]);
      litMap[lits[j]] = lits[0];
      litMap[-lits[j]] = -lits[0];
    }
  }

  for (size_t i = 0; i < cnf.interfaceVariables.size(); ++i) {
    InterfaceVariable& iv = cnf.interfaceVariables[i];
    for (size_t j = 0; j < iv.second.size(); ++j) {
      Lit oldLit = iv.second[j];
      map<Lit, Lit>::const_iterator k = litMap.find(oldLit);
      if (k == litMap.end()) continue;
      iv.second[j] = k->second;
    }
  }

  vector<Clause> newClauses;
  for (size_t i = 0; i < cnf.clauses.size(); ++i) {
    Clause cl;
    for (size_t j = 0; j < cnf.clauses[i].size(); ++j) {
      Lit oldLit = cnf.clauses[i][j];
      map<Lit, Lit>::const_iterator k = litMap.find(oldLit);
      Lit newLit = (k == litMap.end() ? oldLit : k->second);
      cl.push_back(newLit);
    }
    newClauses.push_back(cl);
  }

  cnf.clauses = newClauses;
  cnf.unparse(stdout);

  return 0;
}
