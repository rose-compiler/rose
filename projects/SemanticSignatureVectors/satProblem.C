#include "satProblem.h"

void SatProblem::addInterface(const std::string& name, const std::vector<Lit>& values) {
  interfaceVariables.push_back(std::make_pair(name, values));
}

void SatProblem::addClause(const Clause& cl) {
  Clause newCl;
  for (size_t i = 0; i < cl.size(); ++i) {
    if (cl[i] == FALSE) {
    } else if (cl[i] == TRUE) {
      return;
    } else {
      newCl.push_back(cl[i]);
    }
  }
  if (newCl.empty()) {throw UnsatisfiableException();}
  std::sort(newCl.begin(), newCl.end(), absLess);
  newCl.erase(std::unique(newCl.begin(), newCl.end()), newCl.end());
  for (size_t i = 0; i < newCl.size(); ++i) {
    for (size_t j = 0; j < i; ++j) {
      if (newCl[i] == -newCl[j]) {
        return;
      }
    }
  }
  clauses.push_back(newCl);
}

void SatProblem::parse(FILE* f) {
  interfaceVariables.clear();
  numVariables = (size_t)(-1);
  while (true) {
    char* firstWord = NULL;
    fscanf(f, "%as", &firstWord);
    assert (firstWord);
    std::string fwString = firstWord;
    // fprintf(stderr, "firstWord = %s\n", firstWord);
    free(firstWord);
    if (fwString == "p") {
      break;
    } else if (fwString == "c") {
      const char* tag = NULL;
      size_t count = 0;
      fscanf(f, "%as %zu", &tag, &count);
      assert (tag);
      // fprintf(stderr, "tag=%s count=%zu\n", tag, count);
      std::vector<Lit> lits(count);
      for (size_t i = 0; i < count; ++i) {
        fscanf(f, "%d", &lits[i]);
      }
      interfaceVariables.push_back(std::make_pair(std::string(tag), lits));
    } else {
      assert (!"Bad first word");
    }
  }
  size_t nclauses;
  fscanf(f, " cnf %zu %zu\n", &numVariables, &nclauses);
  assert (numVariables != (size_t)(-1));
  // fprintf(stderr, "nvars=%zu nclauses=%zu\n", numVariables, nclauses);
  clauses.resize(nclauses);
  for (size_t i = 0; i < nclauses; ++i) {
    Clause& cl = clauses[i];
    while (true) {
      Lit lit;
      fscanf(f, "%d", &lit);
      if (lit == 0) {
        break;
      } else {
        cl.push_back(lit);
      }
    }
    std::sort(cl.begin(), cl.end(), absLess);
  }
  fprintf(stderr, "Starting with %zu var(s) and %zu clause(s)\n", numVariables, clauses.size());
}

void SatProblem::unparse(FILE* outfile) {
  fprintf(stderr, "Have %zu clauses before sorting\n", clauses.size());
  std::sort(clauses.begin(), clauses.end(), clauseLess);
  clauses.erase(std::unique(clauses.begin(), clauses.end()), clauses.end());
  fprintf(stderr, "Finished with %zu variables and %zu clauses\n", numVariables, clauses.size());
  for (size_t i = 0; i < interfaceVariables.size(); ++i) {
    fprintf(outfile, "c %s %zu", interfaceVariables[i].first.c_str(), interfaceVariables[i].second.size());
    for (size_t j = 0; j < interfaceVariables[i].second.size(); ++j) {
      fprintf(outfile, " %d", interfaceVariables[i].second[j]);
    }
    fputc('\n', outfile);
  }
  fprintf(outfile, "p cnf %zu %zu\n", numVariables, clauses.size());
  for (std::vector<Clause>::const_iterator i = clauses.begin(); i != clauses.end(); ++i) {
    const Clause& cl = *i;
    for (size_t j = 0; j < cl.size(); ++j) {
      fprintf(outfile, "%d ", cl[j]);
    }
    fputs("0\n", outfile);
  }
}

void SatProblem::identify(Lit a, Lit b) {
  if (absLess(b, a)) std::swap(a, b);
  LitList(2) key;
  key[0] = a;
  key[1] = b;
  __gnu_cxx::hash_map<LitList(2), Lit>::const_iterator it = condEquivCSE.find(key);
  if (it != condEquivCSE.end()) {
    // Add a unit clause for the existing condition
    addClause(single(it->second));
  } else {
    LitList(2) cl;
    cl[0] = invert(a);
    cl[1] = b;
    addClause(cl);
    cl[0] = a;
    cl[1] = invert(b);
    addClause(cl);
    condEquivCSE[key] = TRUE; // In case there is a conditional equivalence added later, ignore it
  }
}

void SatProblem::condEquivalence(Lit sel, Lit a, Lit b) {
  if (absLess(b, a)) std::swap(a, b);
  LitList(2) key;
  key[0] = a;
  key[1] = b;
  __gnu_cxx::hash_map<LitList(2), Lit>::const_iterator it = condEquivCSE.find(key);
  if (it != condEquivCSE.end()) {
    LitList(2) cl;
    cl[0] = invert(sel);
    cl[1] = it->second;
    addClause(cl); // Add implication from new sel to the existing condition
  } else {
    Lit totalSel = newVar();
    condEquivCSE[key] = totalSel;
    LitList(2) cl1;
    cl1[0] = invert(sel);
    cl1[1] = totalSel;
    addClause(cl1);
    LitList(3) cl;
    cl[0] = invert(totalSel);
    cl[1] = invert(a);
    cl[2] = b;
    addClause(cl);
    cl[1] = a;
    cl[2] = invert(b);
    addClause(cl);
  }
}

Lit SatProblem::mux(Lit sel, Lit ifTrue, Lit ifFalse) {
  if (sel == FALSE) {
    return ifFalse;
  } else if (sel == TRUE) {
    return ifTrue;
  } else if (ifTrue == ifFalse) {
    return ifTrue;
  } else if (ifTrue == TRUE && ifFalse == FALSE) {
    return sel;
  } else if (ifTrue == FALSE && ifFalse == TRUE) {
    return invert(sel);
  } else {
    if (sel < 0) {
      sel = invert(sel);
      std::swap(ifTrue, ifFalse);
    }
    LitList(3) cseLookup;
    cseLookup[0] = sel;
    cseLookup[1] = ifTrue;
    cseLookup[2] = ifFalse;
    __gnu_cxx::hash_map<LitList(3), Lit>::const_iterator it = muxCSE.find(cseLookup);
    if (it != muxCSE.end()) return it->second;
    Lit output = newVar();
    condEquivalence(sel, ifTrue, output);
    condEquivalence(invert(sel), ifFalse, output);
    muxCSE.insert(std::make_pair(cseLookup, output));
    return output;
  }
}

Lit SatProblem::andAcross(const std::vector<Lit>& a) {
  std::vector<Lit> newA;
  for (size_t i = 0; i < a.size(); ++i) {
    if (a[i] == FALSE) {
      return FALSE;
    } else if (a[i] == TRUE) {
    } else {
      newA.push_back(a[i]);
    }
  }
  if (newA.size() == 1) {
    return newA[0];
  }
  if (newA.empty()) return TRUE;
  std::sort(newA.begin(), newA.end(), absLess);
  __gnu_cxx::hash_map<std::vector<Lit>, Lit>::const_iterator it = andGateCSE.find(newA);
  if (it != andGateCSE.end()) return it->second;
  Lit output = newVar();
  for (size_t i = 0; i < newA.size(); ++i) {
    LitList(2) cl1;
    cl1[0] = invert(output);
    cl1[1] = newA[i];
    addClause(cl1);
  }
  std::vector<Lit> cl2;
  for (size_t i = 0; i < newA.size(); ++i) {
    cl2.push_back(invert(newA[i]));
  }
  cl2.push_back(output);
  addClause(cl2);
  andGateCSE.insert(std::make_pair(newA, output));
  return output;
}

