#include "satProblem.h"

#include <cassert>
#include <cstdio>
#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <ext/hash_map>
#include <ext/hash_set>

using namespace std;
using namespace __gnu_cxx;

namespace __gnu_cxx {
  template <>
  struct hash<std::pair<int, int> > {
    size_t operator()(const pair<int, int>& p) const {
      return (size_t)(p.first ^ (p.second << 1));
    }
  };
}

int main(int argc, char** argv) {
  SatProblem cnf;
  cnf.parse(stdin);

  hash_map<pair<Lit, Lit>, size_t> coincidences;

  size_t maxCount = 0;
  for (size_t i = 0; i < cnf.clauses.size(); ++i) {
    const Clause& cl = cnf.clauses[i];
    for (size_t j = 0; j < cl.size(); ++j) {
      for (size_t k = 0; k < cl.size(); ++k) {
        Lit l1 = cl[j], l2 = cl[k];
        if (l1 >= l2) continue;
        size_t& count = coincidences[make_pair(l1, l2)];
        ++count;
        if (count > maxCount) maxCount = count;
      }
    }
  }

  vector<hash_set<pair<Lit, Lit> > > coincidencesT(maxCount + 1);

  for (hash_map<pair<Lit, Lit>, size_t>::const_iterator i = coincidences.begin();
       i != coincidences.end(); ++i) {
    if (i->second >= 2) coincidencesT[i->second].insert(i->first);
  }

  size_t maxFilledCountGuess = maxCount; // Must be an overestimate

  const size_t sizeLimit = 3; // Keep this many duplicates

  while (true) {
    assert (maxFilledCountGuess <= maxCount);
    vector<hash_set<pair<Lit, Lit> > >::const_iterator ci = coincidencesT.begin() + maxFilledCountGuess + 1;
    while (ci != coincidencesT.begin()) {
      --ci;
      if (!ci->empty()) break;
    }
    assert (ci != coincidencesT.begin());
    size_t biggestCoincidenceSet = ci - coincidencesT.begin();
    maxFilledCountGuess = biggestCoincidenceSet;
    pair<Lit, Lit> biggestCoincidence = *ci->begin();
    if (biggestCoincidenceSet <= sizeLimit) break;

    fprintf(stderr, "Found coincidence (%d, %d) with count %zu\n", biggestCoincidence.first, biggestCoincidence.second, biggestCoincidenceSet);

    Var newVar = cnf.newVar();
    for (size_t i = 0; i < cnf.clauses.size(); ++i) {
      Clause& cl = cnf.clauses[i];
      int hasCoincidence = 0;
      size_t clSize = cl.size();
      Lit newCl[clSize];
      for (size_t j = 0; j < clSize; ++j) {
        Lit l = cl[j];
        if (l == biggestCoincidence.first) {
          ++hasCoincidence;
          newCl[j] = newVar;
        } else if (l == biggestCoincidence.second) {
          ++hasCoincidence;
          newCl[j] = 0;
        } else {
          newCl[j] = l;
        }
      }
      if (hasCoincidence != 2) continue;
      sort(&newCl[0], &newCl[clSize], absLess);
      assert (*newCl == 0);
      for (size_t j = 0; j < clSize; ++j) {
        Lit l1 = cl[j];
        for (size_t k = 0; k < clSize; ++k) {
          Lit l2 = cl[k];
          if (l1 >= l2) continue;
          size_t& count = coincidences[make_pair(l1, l2)];
          size_t oldCount = count;
          --count;
          assert (oldCount <= maxCount);
          assert (count <= maxCount);
          if (oldCount > sizeLimit) coincidencesT[oldCount].erase(make_pair(l1, l2));
          if (count > sizeLimit) coincidencesT[count].insert(make_pair(l1, l2));
          if (count == 0) coincidences.erase(make_pair(l1, l2));
        }
      }
      for (size_t j = 1; j < clSize; ++j) {
        Lit l1 = newCl[j];
        for (size_t k = 1; k < clSize; ++k) {
          Lit l2 = newCl[k];
          if (l1 >= l2) continue;
          size_t& count = coincidences[make_pair(l1, l2)];
          size_t oldCount = count;
          ++count;
          assert (oldCount <= maxCount);
          assert (count <= maxCount);
          if (oldCount > sizeLimit) coincidencesT[oldCount].erase(make_pair(l1, l2));
          if (count > sizeLimit) coincidencesT[count].insert(make_pair(l1, l2));
          if (count > maxFilledCountGuess) {
            maxFilledCountGuess = count;
          }
        }
      }
      copy(newCl + 1, newCl + clSize, cl.begin());
      cl.pop_back();
    }
    Clause baseClause(3);
    baseClause[0] = -newVar;
    baseClause[1] = biggestCoincidence.first;
    baseClause[2] = biggestCoincidence.second;
    ++coincidences[biggestCoincidence];
    cnf.clauses.push_back(baseClause);

    // fprintf(stderr, "Restarting loop with %d var(s) and %zu clause(s)\n", nvars, clauses.size());
  }

  cnf.unparse(stdout);
  return 0;
}
