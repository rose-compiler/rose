#include "satProblem.h"
#include <set>
#include <map>
#include <vector>
#include <iostream>
#include <ext/hash_map>

// Binary clause simplifier -- like 2-SIMPLIFY

using namespace std;
using namespace __gnu_cxx;

hash_map<Lit, vector<Lit> > implications;
hash_map<Lit, Lit> equivalences;
hash_set<Lit> done;

Lit normalize(Lit a) {
  while (true) {
    hash_map<Lit, Lit>::const_iterator i = equivalences.find(a);
    if (i == equivalences.end()) break;
    a = i->second;
  }
  return a;
}

void addEquivalence(Lit a, Lit b) {
  a = normalize(a);
  b = normalize(b);
  if (a == b) return;
  if (a == invert(b)) {
    cerr << "System is inconsistent" << endl;
    abort();
  }
  if (a == TRUE || a == FALSE) { // Have constants on RHS
    std::swap(a, b);
  }
  equivalences[a] = b;
}

void explore(Lit l, vector<Lit>& stack) {
  if (find(stack.begin(), stack.end(), l) != stack.end()) {
    // Found cycle
    // cerr << "Found cycle from " << l << endl;
    for (vector<Lit>::const_iterator i = find(stack.begin(), stack.end(), l);
         ++i != stack.end(); ) {
      addEquivalence(l, *i);
      addEquivalence(invert(l), invert(*i));
      // cerr << ".. containing " << *i << endl;
    }
    // cerr << "End of cycle" << endl;
  } else if (find(stack.begin(), stack.end(), invert(l)) != stack.end()) {
    // We have not(l) -> l, so l is true
    addEquivalence(normalize(l), TRUE);
    addEquivalence(normalize(invert(l)), FALSE);
    // cerr << "Found known true literal " << l << endl;
  } else if (done.find(l) != done.end()) {
    // Nothing
  } else if (implications.find(l) == implications.end()) {
    // cerr << "Found pure literal " << l << endl;
  } else {
    done.insert(l);
    stack.push_back(l);
    for (size_t i = 0; i < implications[l].size(); ++i) {
      explore(implications[l][i], stack);
    }
    stack.pop_back();
  }
}

int main(int argc, char** argv) {
  SatProblem cnf;
  cnf.parse(stdin);

  size_t count = 0;

  while (count < 100) {
    cerr << "count = " << count << endl;

    equivalences.clear();
    implications.clear();
    for (size_t i = 0; i < cnf.clauses.size(); ++i) {
      if (cnf.clauses[i].size() == 0) {
        abort();
      } else if (cnf.clauses[i].size() == 1) {
        addEquivalence(cnf.clauses[i][0], TRUE);
        addEquivalence(invert(cnf.clauses[i][0]), FALSE);
      } else if (cnf.clauses[i].size() == 2) {
        implications[invert(cnf.clauses[i][0])].push_back(cnf.clauses[i][1]);
        implications[invert(cnf.clauses[i][1])].push_back(cnf.clauses[i][0]);
      } else {
        // Do nothing
      }
    }

    done.clear();
    for (hash_map<Lit, vector<Lit> >::const_iterator i = implications.begin();
         i != implications.end(); ++i) {
      vector<Lit> stack;
      // cerr << "Exploring " << i->first << endl;
      explore(i->first, stack);
    }

    vector<Clause> oldClauses = cnf.clauses;
    cnf.clauses.clear();

    for (size_t i = 0; i < oldClauses.size(); ++i) {
      Clause cl = oldClauses[i];
      for (size_t j = 0; j < cl.size(); ++j) {
        cl[j] = normalize(cl[j]);
      }
      cnf.addClause(cl);
    }

    for (size_t i = 0; i < cnf.interfaceVariables.size(); ++i) {
      InterfaceVariable& iv = cnf.interfaceVariables[i];
      for (size_t j = 0; j < iv.second.size(); ++j) {
        Lit oldLit = iv.second[j];
        Lit newLit = normalize(oldLit);
        iv.second[j] = newLit;
      }
    }

    if (equivalences.empty()) break; // No changes

    ++count;
  }

  cnf.unparse(stdout);

  return 0;
}
