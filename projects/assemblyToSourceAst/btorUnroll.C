#include "btorProblem.h"
#include <boost/lexical_cast.hpp>

using namespace std;

typedef BtorComputationPtr Comp;

typedef map<BtorComputation*, Comp> LatchMap;

enum RootHandling {rh_assume_false, rh_disjoin};

// Possible bug in Boolector: multiple roots are not allowed in combinational
// circuits, but are in sequential ones

static Comp translate(BtorProblem& p, Comp c, const LatchMap& latchMap, map<BtorComputation*, Comp>& variableMap, map<Comp, Comp>& translateMap, const string& tag) {
  BtorComputation* comp = c.p.get();
  if (translateMap.find(c) != translateMap.end()) {
    return translateMap[c];
  } else if (latchMap.find(comp) != latchMap.end()) { // The latch map must be checked before the variable map, as we don't replicate those variables that are also latches (but do for non-latches)
    LatchMap::const_iterator i = latchMap.find(comp);
    Comp c2 = i->second;
    if (c.inverted) c2 = c2.invert();
    translateMap[c] = c2;
    return c2;
  } else if (variableMap.find(comp) != variableMap.end()) {
    map<BtorComputation*, Comp>::const_iterator i = variableMap.find(comp);
    Comp c2 = i->second;
    if (c.inverted) c2 = c2.invert();
    translateMap[c] = c2;
    return c2;
  }
  if (comp->op == btor_op_var) { // New variable
    Comp newVar = p.build_var(comp->type.bitWidth, comp->variableName + tag);
    variableMap.insert(make_pair(comp, newVar));
    translateMap[c] = newVar;
    return newVar;
  } else if (comp->op == btor_op_array) { // New array
    Comp newVar = p.build_array(comp->type.bitWidth, comp->type.arraySize);
    variableMap.insert(make_pair(comp, newVar));
    translateMap[c] = newVar;
    return newVar;
  }
  vector<Comp> newOperands;
  for (size_t i = 0; i < comp->operands.size(); ++i) {
    newOperands.push_back(translate(p, comp->operands[i], latchMap, variableMap, translateMap, tag));
  }
  BtorComputation* newComp = new BtorComputation(comp->type.bitWidth, comp->op, newOperands, comp->immediates);
  Comp newC = newComp;
  if (c.inverted) newC = newC.invert();
  translateMap[c] = newC;
  return newC;
}

// Unroll one pass through the netlist
// Replicate var and array (done in translate()), and root constructs
// Change all references to latches using the oldLatchMap
// Produce newLatchMap by looking up the next values of all latches
static LatchMap unrollOneIteration(BtorProblem& p, const vector<Comp>& computations, const vector<pair<Comp, Comp> >& latches, const LatchMap& oldLatchMap, RootHandling rootHandling, Comp& rootsSoFar, const string& tag) {
  map<BtorComputation*, Comp> variableMap;
  LatchMap newLatchMap;
  map<Comp, Comp> translateMap;
  for (size_t i = 0; i < latches.size(); ++i) {
    assert (oldLatchMap.find(latches[i].first.p.get()) != oldLatchMap.end());
    newLatchMap[latches[i].first.p.get()] = translate(p, latches[i].second, oldLatchMap, variableMap, translateMap, tag);
  }
  for (size_t i = 0; i < computations.size(); ++i) {
    Comp comp = computations[i];
    if (comp.p->op == btor_op_root) {
      switch (rootHandling) {
        case rh_disjoin: {
          rootsSoFar = p.build_op_or(rootsSoFar, translate(p, comp.p->operands[0], oldLatchMap, variableMap, translateMap, tag));
          break;
        }
        case rh_assume_false: { // How do I do this (assume something is false)?
          assert (!"rh_assume_false not supported");
        }
        default: assert (false);
      }
    }
  }
  return newLatchMap;
}

int main(int argc, char** argv) {
  assert (argc == 3);
  uint unrollCount = boost::lexical_cast<uint>(argv[1]);
  string initialConditionTag = argv[2];
  BtorProblem p = BtorProblem::parse(stdin);
  BtorProblem p2;
  vector<pair<Comp, Comp> > latches;
  for (size_t i = 0; i < p.computations.size(); ++i) {
    if (p.computations[i].p->op == btor_op_next ||
        p.computations[i].p->op == btor_op_anext) {
      Comp current = p.computations[i].p->operands[0];
      Comp next = p.computations[i].p->operands[1];
      assert (!current.inverted);
      assert (!next.inverted);
      latches.push_back(make_pair(current, next));
    }
  }
  map<BtorComputation*, Comp> latchMap;
  if (initialConditionTag == "clear") {
    for (size_t i = 0; i < latches.size(); ++i) {
      if (latches[i].first.kind() == btor_type_bitvector) {
        latchMap[latches[i].first.p.get()] = p2.build_op_zero(latches[i].first.bitWidth());
      } else if (latches[i].first.kind() == btor_type_array) {
        latchMap[latches[i].first.p.get()] = p2.build_array(latches[i].first.bitWidth(), latches[i].first.arraySize());
      } else {
        assert (false);
      }
    }
  } else if (initialConditionTag == "arbitrary" || initialConditionTag == "induction") {
    for (size_t i = 0; i < latches.size(); ++i) {
      if (latches[i].first.kind() == btor_type_bitvector) {
        latchMap[latches[i].first.p.get()] = p2.build_var(latches[i].first.bitWidth(), "latch_" + boost::lexical_cast<string>(i));
      } else if (latches[i].first.kind() == btor_type_array) {
        latchMap[latches[i].first.p.get()] = p2.build_array(latches[i].first.bitWidth(), latches[i].first.arraySize());
      } else {
        assert (false);
      }
    }
  } else {
    fprintf(stderr, "Bad initial condition tag '%s'\n", initialConditionTag.c_str());
    abort();
  }
  Comp root = p2.build_op_zero(1);
  for (size_t i = 0; i < unrollCount; ++i) {
    RootHandling rootHandling = rh_disjoin;
    if (initialConditionTag == "induction" && i + 1 < unrollCount) {
      rootHandling = rh_assume_false;
    }
    latchMap = unrollOneIteration(p2, p.computations, latches, latchMap, rootHandling, root, "_iter" + boost::lexical_cast<string>(i));
  }
  p2.computations.push_back(p2.build_op_root(root));
  fprintf(stdout, "%s", p2.unparse().c_str());
  return 0;
}
