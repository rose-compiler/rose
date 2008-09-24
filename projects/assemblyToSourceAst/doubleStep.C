#include "satProblem.h"
#include <map>
#include <set>
#include <vector>
#include <cstdio>
#include <cassert>

using namespace std;

inline Lit biasLiteral(Lit a, size_t bias) {
  if (a == -a) {
    return a;
  } else if (a > 0) {
    return a + bias;
  } else {
    return a - bias;
  }
}

// FIXME: link up memory reads and writes between steps

int main(int, char**) {
  SatProblem cnf;
  cnf.parse(stdin);

  size_t origNVars = cnf.numVariables;

  cnf.numVariables *= 2;
  size_t oldNumClauses = cnf.clauses.size();
  for (size_t i = 0; i < oldNumClauses; ++i) {
    Clause cl = cnf.clauses[i];
    for (size_t j = 0; j < cl.size(); ++j) {
      cl[j] = biasLiteral(cl[j], origNVars);
    }
    cnf.clauses.push_back(cl);
  }

  map<string, vector<Lit> > interfaceVarsByName(cnf.interfaceVariables.begin(), cnf.interfaceVariables.end());

  for (size_t i = 0; i < cnf.interfaceVariables.size(); ++i) {
    InterfaceVariable& iv = cnf.interfaceVariables[i];
    if (iv.first.size() >= 4 && iv.first.substr(0, 4) == "out_") {
      vector<Lit>& litsOut = iv.second;
      const vector<Lit>& litsIn = interfaceVarsByName["in_" + iv.first.substr(4)];
      assert (litsOut.size() == litsIn.size());
      for (size_t j = 0; j < litsIn.size(); ++j) {
        Clause cl(2);
        cl[0] = -litsOut[j];
        cl[1] = litsIn[j];
        cnf.clauses.push_back(cl);
        cl[0] = litsOut[j];
        cl[1] = -litsIn[j];
        cnf.clauses.push_back(cl);
        litsOut[j] = biasLiteral(litsOut[j], origNVars);
      }
    }
  }

  cnf.unparse(stdout);
  return 0;
}
