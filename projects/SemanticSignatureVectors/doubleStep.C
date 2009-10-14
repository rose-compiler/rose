#include "rose.h"
#include "satProblem.h"
#include "x86AssemblyToNetlist.h"
#include <map>
#include <set>
#include <vector>
#include <cstdio>
#include <cassert>

using namespace std;

int main(int, char**) {
  SatProblem cnf;
  cnf.parse(stdin);

  size_t origNVars = cnf.numVariables;

  cnf.numVariables *= 2;
  size_t oldNumClauses = cnf.clauses.size();
  for (size_t i = 0; i < oldNumClauses; ++i) {
    Clause cl = biasLiterals(cnf.clauses[i], origNVars);
    cnf.clauses.push_back(cl);
  }

  map<string, vector<Lit> > interfaceVarsByName(cnf.interfaceVariables.begin(), cnf.interfaceVariables.end());
  vector<vector<Lit> > memoryWrites;
  vector<vector<Lit> > memoryReads;

  for (size_t i = 0; i < cnf.interfaceVariables.size(); ++i) {
    InterfaceVariable& iv = cnf.interfaceVariables[i];
    if (iv.first.size() >= 12 && iv.first.substr(0, 12) == "memoryWrite_") {
      size_t idx = boost::lexical_cast<size_t>(iv.first.substr(12));
      if (memoryWrites.size() <= idx) memoryWrites.resize(idx + 1);
      memoryWrites[idx] = iv.second;
    }
    if (iv.first.size() >= 11 && iv.first.substr(0, 11) == "memoryRead_") {
      size_t idx = boost::lexical_cast<size_t>(iv.first.substr(11));
      if (memoryReads.size() <= idx) memoryReads.resize(idx + 1);
      memoryReads[idx] = iv.second;
    }
  }

  // Match up registers and flags
  for (size_t i = 0; i < cnf.interfaceVariables.size(); ++i) {
    InterfaceVariable& iv = cnf.interfaceVariables[i];
    if (iv.first.size() >= 4 && iv.first.substr(0, 4) == "out_") {
      vector<Lit>& litsOut = iv.second;
      const vector<Lit>& litsIn = interfaceVarsByName["in_" + iv.first.substr(4)];
      assert (litsOut.size() == litsIn.size());
      for (size_t j = 0; j < litsIn.size(); ++j) {
        cnf.identify(biasLiteral(litsIn[j], origNVars), litsOut[j]);
        litsOut[j] = biasLiteral(litsOut[j], origNVars);
      }
    }
  }

  // Match up memory accesses
  // Keep all old memory writes, and add another copy of them with biased labels
  for (size_t i = 0; i < memoryWrites.size(); ++i) {
    cnf.interfaceVariables.push_back(make_pair("memoryWrite_" + boost::lexical_cast<string>(i + memoryWrites.size()), biasLiterals(memoryWrites[i], origNVars)));
  }
  // Try to match new copy of reads with old writes, but add them to the read list otherwise
  for (size_t i = 0; i < memoryReads.size(); ++i) {
    MemoryAccess rd = biasLiterals(memoryReads[i], origNVars);
    LitList(8) rdData = rd.data;
    // Create new variables for before the whole set of writes
    rd.data = cnf.newVars<8>();
    cnf.interfaceVariables.push_back(make_pair("memoryRead_" + boost::lexical_cast<string>(i + memoryReads.size()), rd.vec()));
    // Scan from earliest to latest as newer writes overwrite older ones
    for (size_t j = 0; j < memoryWrites.size(); ++j) {
      MemoryAccess wr = memoryWrites[j];
      rd.data = cnf.ite(cnf.andGate(cnf.andGate(rd.cond, wr.cond), cnf.equal(rd.addr, wr.addr)), wr.data, rd.data);
    }
    // Map the result variables to the result of the read
    for (size_t j = 0; j < 8; ++j) {
      cnf.identify(rd.data[j], rdData[j]);
    }
  }

  cnf.unparse(stdout);
  return 0;
}
