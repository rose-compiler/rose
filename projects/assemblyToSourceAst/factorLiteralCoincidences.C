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

typedef int Var;
typedef int Lit; // DIMACS convention
typedef vector<Lit> Clause;

bool absLess(Lit a, Lit b) {
  Lit absa = abs(a);
  Lit absb = abs(b);
  if (absa < absb) return true;
  if (absa > absb) return false;
  return a < b;
}

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

namespace __gnu_cxx {
  template <>
  struct hash<std::pair<int, int> > {
    size_t operator()(const pair<int, int>& p) const {
      return (size_t)(p.first ^ (p.second << 1));
    }
  };
}

int main(int argc, char** argv) {
  int nvars;
  size_t nclauses;
  scanf("p cnf %d %zu\n", &nvars, &nclauses);
  vector<Clause> clauses(nclauses);
  for (size_t i = 0; i < clauses.size(); ++i) {
    Clause& cl = clauses[i];
    while (true) {
      Lit lit;
      scanf("%d", &lit);
      if (lit == 0) {
        break;
      } else {
        cl.push_back(lit);
      }
    }
  }

  fprintf(stderr, "Starting with %d var(s) and %zu clause(s)\n", nvars, nclauses);

  hash_map<pair<Lit, Lit>, size_t> coincidences;

  size_t maxCount = 0;
  for (size_t i = 0; i < clauses.size(); ++i) {
    const Clause& cl = clauses[i];
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

    Var newVar = ++nvars;
    for (size_t i = 0; i < clauses.size(); ++i) {
      Clause& cl = clauses[i];
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
      sort(newCl, newCl + clSize, absLess);
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
    sort(baseClause.begin(), baseClause.end(), absLess);
    clauses.push_back(baseClause);

    // fprintf(stderr, "Restarting loop with %d var(s) and %zu clause(s)\n", nvars, clauses.size());
  }

  sort(clauses.begin(), clauses.end(), clauseLess);
  clauses.erase(unique(clauses.begin(), clauses.end()), clauses.end());

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
