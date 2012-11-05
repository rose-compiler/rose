#include "LTL.h"
#include "LTLCheckerUnified.h"
#include "AType.h"
#include <stack>
#include <queue>
#include "CommandLineOptions.h"

// Copyright (c) 2012 Lawrence Livermore National Security, LLC.
// Produced at the Lawrence Livermore National Laboratory
// Written by Adrian Prantl <adrian@llnl.gov>.
//
// UCRL-CODE-155962.
// All rights reserved.
//
// This file is part of ROSE. For details, see http://www.rosecompiler.org/.
// Please read the COPYRIGHT file for Our Notice and for the BSD License.

using namespace CodeThorn;
using namespace LTL;
using namespace UnifiedLTL;

#define FOR_EACH_TRANSITION(TRANSITION)                              \
  for (TransitionGraph::const_iterator TRANSITION=transitionGraph.begin(); \
       TRANSITION != transitionGraph.end();				\
       ++TRANSITION)

#define FOR_EACH_ESTATE(STATE, LABEL)                                \
  Label LABEL=0;                                                     \
  for (EStateSet::const_iterator STATE=eStateSet.begin();            \
       STATE != eStateSet.end();                                     \
       ++STATE, ++LABEL)

#define FOR_EACH_STATE(STATE, LABEL)					\
  GraphTraits::vertex_iterator vi, vi_end;				\
  Label LABEL;								\
  const EState* STATE;							\
  for (tie(vi, vi_end) = vertices(g), LABEL=*vi, STATE=g[LABEL];	\
       vi != vi_end; ++vi,						\
       LABEL=(vi!=vi_end)?*vi:NULL, STATE=g[LABEL])			\

void add_edge_if_new1(Label src, Label tgt, BoostTransitionGraph& g) {
  boost::graph_traits<BoostTransitionGraph>::edge_descriptor _edge;
  bool b;
  tie(_edge, b) = edge(src, tgt, g);
  if (!b) add_edge(src, tgt, g);
}

void add_edge_if_new(LTLVertex src, LTLVertex tgt, LTLTransitionGraph& g) {
  boost::graph_traits<LTLTransitionGraph>::edge_descriptor _edge;
  bool b;
  tie(_edge, b) = edge(src, tgt, g);
  if (!b) add_edge(src, tgt, g);
  else {
    cerr<<"edge between"
	<<g[src].estate->toString()<<" and "
	<<g[src].estate->toString()<<" already existed."<<endl;
  }
}


#if 0
/**
 * DOT visualization of the LTL Checker result
 */
class Visualizer: public TopDownVisitor {
public:
  Visualizer(LTLProperties& p, Label state)
    : ltl_properties(p), label(state), n(1234567) {}
  LTLProperties& ltl_properties;
  Label label;
  stringstream s;
  int n; // consecutive labeling of dot node
  static const int shift = 16777216;

  struct Attr: public InheritedAttribute {
    Attr(int i) : dot_label(i) {}
    int dot_label;
  };
  static Attr* get(IAttr a) { return static_cast<Attr*>(a.get()); }
  static IAttr newAttr(int n)  { return IAttr((InheritedAttribute*)new Attr(n)); }
  int newNode(IAttr a) {
    int node = label*shift+n++;
    s<<node<<" -> "<<get(a)->dot_label<<" [color=limegreen, weight=2, style=dashed];\n  ";
    return node;
  }
  static string color(BoolLattice lval) {
    if (lval.isTrue())  return  "color=limegreen";
    if (lval.isFalse()) return  "color=crimson";
    if (lval.isTop())   return  "color=red";
    if (lval.isBot())   return  "color=gainsboro";
  }

  IAttr visit(InputSymbol* expr, IAttr a)  {
    short e = expr->label;
    int node = newNode(a);
    s<<node<<" ["<</*color(props[e])<<*/",label=\"Input "<<string(1, expr->c)
     <<" = "<</*props[e]<<*/"\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(OutputSymbol* expr, IAttr a) {
    short e = expr->label;
    int node = newNode(a);
    s<<node<<" ["<</*color(props[e])<<*/",label=\"Output "<<string(1, expr->c)
     <<" = "<</*props[e]<<*/"\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(NegInputSymbol* expr, IAttr a)  {
    short e = expr->label;
    int node = newNode(a);
    s<<node<<" ["<</*color(props[e])<<*/",label=\"¬Input "<<string(1, expr->c)
     <<" = "<</*props[e]<<*/"\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(NegOutputSymbol* expr, IAttr a) {
    short e = expr->label;
    int node = newNode(a);
    s<<node<<" ["<</*color(props[e])<<*/",label=\"¬Output "<<string(1, expr->c)
     <<" = "<</*props[e]<<*/"\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(Not* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    int node = newNode(a);
    s<<node<<" ["<</*color(props[e])<<*/",label=\""/*<<props[e]*/
     <<" = "<<"!"<</*color(props[e1])<<*/"\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(Next* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    int node = newNode(a);
    s<<node<<" [shape=circle,"<</*color(props[e])<<*/",label=\""<<"("/*<<props[e]*/
     <<" = "<<"X "<</*color(props[e1])<<*/")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(Eventually* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    int node = newNode(a);
    s<<node<<" [shape=diamond,"<</*color(props[e])<<*/",label=\""<<"("/*<<props[e]*/
     <<" = "<<"F "<</*color(props[e1])<<*/")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(Globally* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    int node = newNode(a);
    s<<node<<" [shape=box,"<</*color(props[e])<<*/",label=\""<<"("/*<<props[e]*/
     <<" = "<<"G "<</*color(props[e1])<<*/")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(And* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    short e2 = expr->expr2->label;
    int node = newNode(a);
    s<<node<<" ["<</*color(props[e])<<*/",label=\""<<"("/*<<props[e]*/
     <<" = "<</*color(props[e1])<<*/" & "<</*color(props[e2])<<*/")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(Or* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    short e2 = expr->expr2->label;
    int node = newNode(a);
    s<<node<<" ["<</*color(props[e])<<*/",label=\""<<"("/*<<props[e]*/
     <<" = "<</*color(props[e1])<<*/" | "<</*color(props[e2])<<*/")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(Until* expr, IAttr a)	{
    short e = expr->label;
    short e1 = expr->expr1->label;
    short e2 = expr->expr2->label;
    int node = newNode(a);
    s<<node<<" ["<</*color(props[e])<<*/",label=\""<<"("/*<<props[e]*/
     <<" = "<</*color(props[e1])<<*/" U "<</*color(props[e2])<<*/")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(WeakUntil* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    short e2 = expr->expr2->label;
    int node = newNode(a);
    s<<node<<" ["<</*color(props[e])<<*/",label=\""<<"("/*<<props[e]*/
     <<" = "<</*color(props[e1])<<*/" WU "<</*color(props[e2])<<*/")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(Release* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    short e2 = expr->expr2->label;
    int node = newNode(a);
    s<<node<<" ["<</*color(props[e])<<*/",label=\""<<"("/*<<props[e]*/
     <<" = "<</*color(props[e1])<<*/" R "<</*color(props[e2])<<*/")\"];\n  ";
    return newAttr(node);
  }
};
#endif

ostream& UnifiedLTL::operator<<(ostream& os, const LTLState& s) {
  os << "LTLState(" << s.estate->toString() << ", [ ";
  for (vector<BoolLattice>::const_iterator i = s.valstack.begin();
       i != s.valstack.end(); ++i) {
    os<<*i<<" ";
  }
  os<<"])";
  return os;
}


/**
 * We perform the verification of the LTL formula by traversing it
 * bottom-up and verifying all the properties as we move upwards.
 *   This requires up to N fixpoint iterations over the transition
 * graph, where N is the number of sub-expressions in f.
 *
 * We distinguish two kinds of LTL operators:
 * * constant(?) operators (!, &, |, iX, oX, G)
 * * monotone(?) operators (X, F, E, WU, U), these require a
 *   fixpoint iteration over the entire transition graph.
 *
 * TODO: Are there better names for these classes?
 * JOINS should have the same information
 */
class UVerifier: public BottomUpVisitor {
  EStateSet& eStateSet;
  deque<const EState*> endpoints;
  Label start;
  // this map contains the label of every LTL expression
  map<const Expr*, Label> expr_label;

public:
  LTLStateTransitionGraph stg;

  UVerifier(EStateSet& ess, BoostTransitionGraph& g,
	   Label start_label, Label max_label, short expr_size)
    : eStateSet(ess), start(start_label) {
    // reserve a result map for each label
    // it maps an analysis result to each sub-expression of the ltl formula

    // Create the LTLStateTransitionGraph
    GraphTraits::vertex_iterator vi, vi_end;
    for (tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi) {
      LTLState state(g[*vi], Bot());
      LTLVertex v = add_vertex(stg.g);
      stg.vertex[state] = v;
      stg.g[v] = state;
    }

    GraphTraits::edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
      Label src = source(*ei, g);
      Label tgt = target(*ei, g);
      add_edge(stg.vertex[LTLState(g[src], Bot())], 
	       stg.vertex[LTLState(g[tgt], Bot())], 
	       stg.g);
    }

    // keep track of all endpoints
    LTLGraphTraits::vertex_iterator lvi, lvi_end;
    for (tie(lvi, lvi_end) = vertices(stg.g); lvi != lvi_end; ++lvi) {
      if (out_degree(*lvi, stg.g) == 0)
	endpoints.push_back(stg.g[*lvi].estate);
    }

  }


# define NOP do{} while(0)
# define AND &&
  // Apologies for this. Really, can a macro be any more dirty!?
# define LUB ).lub(
# define GLB ).glb(



  LTLVertex add_state_if_new(LTLState new_state, //queue<LTLVertex>& workset,
			     LTLStateTransitionGraph& stg) {
    LTLStateMap::iterator i = stg.vertex.find(new_state);
    if (i == stg.vertex.end()) {
      LTLVertex v = add_vertex(stg.g);
      stg.vertex[new_state] = v;
      stg.g[v] = new_state;
      //workset.push(v);
      cerr<<"** added new node "<<new_state<<endl;
      return v;
    } else {
      // if we have identical valstacks and different vals this should rather be an update!      
      LTLVertex v = (*i).second;
      LTLState s = stg.g[v];
      s.val = s.val.lub(new_state.val);
      stg.g[v] = s;
      // don't need to update vertex[], because val is not used by operator<
      cerr<<"** merged states "<<new_state<<endl;
      return v;
    }
  }

  /**
   * perform a backward-directed fixpoint iteration over all states
   *
   * since our programs do not necessarily have exit nodes, we start
   * the working set with all exit nodes AND all nodes where START == true.
   *
   * \param JOIN  join operator
   * \param TRANSFER  dataflow equation
   *
   * transfer(a, next)
   */
  template<class TransferFunctor>
  void analyze(LTLStateTransitionGraph& stg, TransferFunctor transfer, short nargs)  {
    LTLWorklist worklist;//(endpoints);

    cerr<<"analyze() initializing..."<<endl;
    LTLGraphTraits::vertex_iterator vi, vi_end;
    for (tie(vi, vi_end) = vertices(stg.g); vi != vi_end; ++vi) {
      // push bot to the top of the LTL stack and update the vertex map
      
      LTLState state = stg.g[*vi];
      stg.vertex.erase(state);
      state.val = Bot();
      stg.g[*vi] = state;
      stg.vertex[state] = *vi;
      //assert(state.valstack.back()

      // start at all fixpoints
      //LTLState s = transfer(state, Bot());
      //if (!s.valstack.back().isBot()) {
      worklist.push(stg.vertex[state]);
	//}
    }

    if (worklist.empty()) {
      cerr<<"** WARNING: empty worklist!"<<endl;
    }

    while (!worklist.empty()) {
      LTLVertex v = worklist.front(); worklist.pop();
      cerr<<"Visiting state "<<stg.g[v].estate<<endl;

      /* for each successor */
      LTLWorklist succs;
      LTLGraphTraits::out_edge_iterator out_i, out_end;
      for (tie(out_i, out_end) = out_edges(v, stg.g); out_i != out_end; ++out_i) {
	LTLVertex succ = target(*out_i, stg.g);
	// store it in a local worklist because we will invalidate the
	// iterator by adding/removing edges
	succs.push(succ);
      }
      while (!succs.empty()) {
	LTLVertex succ = succs.front(); succs.pop();
	cerr<<"  succ: "<<succ; 
	BoolLattice succ_val = stg.g[succ].top();
	cerr<<" = "<<succ_val<<endl;

	// Always make a new state v', so we can calculate precise
	// results for both paths.  If a v' and v'' should later turn
	// out to be identical, they will automatically be merged.
	LTLState old_state = stg.g[v];
	LTLState new_state = transfer(old_state, succ_val);
	bool fixpoint = ( (new_state == old_state) && (new_state.val == old_state.val));

	if (fixpoint) {
	  cerr<<"reached fixpoint!"<<endl;
	} else {
	  // create a new state in lieu of  the old state, and cut off the old state
	  LTLVertex v_prime = add_state_if_new(new_state, stg);
	  //stg.vertex[updated_state] = v;
	  //stg.g[v] = updated_state;
	  //cerr<<old_state<< " <--> "<<new_state<<endl;
	  assert (v_prime != v || (new_state.val != old_state.val));
	  add_edge_if_new(v_prime, succ, stg.g);

	  // for each predecessor
	  LTLGraphTraits::in_edge_iterator in_i, in_end;
	  for (tie(in_i, in_end) = in_edges(v, stg.g); in_i != in_end; ++in_i) {
	    LTLVertex pred = source(*in_i, stg.g);
	    
	    cerr<<" succ(really pred): "<<stg.g[pred].estate<<" = .."<<endl;
	    add_edge_if_new(pred, v_prime, stg.g);
	    if (v_prime != v) remove_edge(v, succ, stg.g);
	    worklist.push(pred);
	  }
	}
      }
    }

    cerr<<"storing results"<<endl;
    for (tie(vi, vi_end) = vertices(stg.g); vi != vi_end; ++vi) {
      LTLState state = stg.g[*vi];
      stg.vertex.erase(state);
      for (int i = 0; i<nargs; ++i)
	state.valstack.pop_back(); 
      cerr<<"==="<<state.val<<endl;
      state.push(state.val);
      stg.g[*vi] = state;
      stg.vertex[state] = *vi;
    }
  }

  struct AllEdges { bool operator() (LTLEdge e) { return true; } };


  ///tell if a state is an exit node
  bool isEndpoint(const EState* es) const {
    return find(endpoints.begin(), endpoints.end(), es) != endpoints.end();
  }

  /// verify that two constraints are consistent, ie. not true and false
  static bool consistent(BoolLattice a, BoolLattice b) {
    if ((a.isTrue() && b.isFalse()) || (b.isTrue() && a.isFalse())) return false;
    else return true;
  }

  /// convert an integer 1..26 to an ASCII char value
  static inline int rersChar(int c) {
    return c+'A'-1;
  }

  static void updateInputVar(const EState* estate, set<VariableId>& input_vars) {
    if (estate->io.op == InputOutput::STDIN_VAR) {
      input_vars.insert(estate->io.var);
    }
  }

  /// return True iff that state is an Oc operation
  static BoolLattice isInputState(const EState* estate,
				  set<VariableId>& input_vars,
				  int c, BoolLattice joined_preds) {
    updateInputVar(estate, input_vars);
    if (input_vars.empty())
      return Bot();

    BoolLattice r = BoolLattice(Top()) || joined_preds;
    assert(estate);
    assert(estate->constraints());
    ConstraintSet constraints = *estate->constraints();
    for (set<VariableId>::const_iterator ivar = input_vars.begin();
	 ivar != input_vars.end();
	 ++ivar) {
      // main input variable
      BoolLattice r1 = is_eq(constraints, *ivar, c);
      assert(consistent(r, r1));
      r = r1;
    }
    return r;
  }

  static BoolLattice is_eq(const ConstraintSet& constraints,
			   const VariableId& v,
			   int c) {
    ListOfAValue l = constraints.getEqVarConst(v);
    for (ListOfAValue::iterator lval = l.begin(); lval != l.end(); ++lval) {
      //cerr<<endl<<"ivar == "<<v.variableName()<<endl;
      //cerr<<constraints.toString()<<endl;
      if (lval->isConstInt()) {
	// A=1, B=2
	//cerr<<(char)c<<" == "<<(char)(rersChar(lval->getIntValue()))<<"? "
	//    <<(bool)(c == rersChar(lval->getIntValue()))<<endl;
	return c == rersChar(lval->getIntValue());
      }
    }
    // In ConstIntLattice, Top means ALL values
    return Top(); // Bool Top, however, means UNKNOWN
  }

  // Implementation status: IN PROGRESS
  // NOTE: This is extremely taylored to the RERS challenge benchmarks.
  struct TransferI {
    set<VariableId> input_vars;
    TransferI(const UVerifier& v, const InputSymbol* e) : verifier(v), expr(e) {}
    const UVerifier& verifier;
    const InputSymbol* expr;
    LTLState operator() (const LTLState& s, BoolLattice joined_succs ) { 
      LTLState newstate(s);
      BoolLattice old_val = s.val; 
      BoolLattice new_val = 
	isInputState(s.estate, input_vars, expr->c, joined_succs);
      assert(new_val.lub(old_val) == new_val); // only move up in the lattice!
      newstate.val = new_val;
      cerr<<"  I(old="<<old_val<<", succ="<<joined_succs<<") = "<<new_val<<endl;
      return newstate;
    }
  }; 
  void visit(const InputSymbol* expr) {
    analyze(stg, TransferI(*this, expr), 0);
  }

  /// return True iff that state is an !Ic operation
  static BoolLattice isNegInputState(const EState* estate,
				     set<VariableId>& input_vars,
				     int c, BoolLattice joined_preds) {
    updateInputVar(estate, input_vars);
    if (input_vars.empty())
      return Bot();

    BoolLattice r = joined_preds.isBot() ? Top() : joined_preds;
    assert(estate);
    assert(estate->constraints());
    ConstraintSet constraints = *estate->constraints();
    for (set<VariableId>::const_iterator ivar = input_vars.begin();
		 ivar != input_vars.end();
		 ++ivar) {
      // This will really only work with one input variable (that one may be aliased, though)
      BoolLattice r1 = is_neq(constraints, *ivar, c);
      assert(consistent(r, r1));
      r = r1;
    }
    return r;
  }

  static BoolLattice is_neq(const ConstraintSet& constraints,
			    const VariableId& v,
			    int c) {
    // var == c
    ListOfAValue l = constraints.getEqVarConst(v);
    for (ListOfAValue::iterator lval = l.begin(); lval != l.end(); ++lval) {
      if (lval->isConstInt()) {
	// A=1, B=2
	return c != rersChar(lval->getIntValue());
      }
    }
    // var != c
    l = constraints.getNeqVarConst(v);
    for (ListOfAValue::iterator lval = l.begin(); lval != l.end(); ++lval) {
      if (lval->isConstInt())
	if (c == rersChar(lval->getIntValue()))
	  return true;
    }

    // In ConstIntLattice, Top means ALL values
    return Top();   // Bool Top, however, means UNKNOWN
  }

  // Implementation status: IN PROGRESS
  // NOTE: This is extremely taylored to the RERS challenge benchmarks.
  struct TransferNI {
    set<VariableId> input_vars;
    TransferNI(const UVerifier& v, const InputSymbol* e) : verifier(v), expr(e) {}
    const UVerifier& verifier;
    const InputSymbol* expr;
    LTLState operator() (const LTLState& s, BoolLattice joined_succs ) { 
      LTLState newstate(s);
      BoolLattice old_val = s.val; 
      BoolLattice new_val = 
	isNegInputState(s.estate, input_vars, expr->c, joined_succs);
      assert(new_val.lub(old_val) == new_val); // only move up in the lattice!
      newstate.val = new_val;
      cerr<<"  NI(old="<<old_val<<", succ="<<joined_succs<<") = "<<new_val<<endl;
      return newstate;
    }
  }; 
  void visit(const NegInputSymbol* expr) {
    analyze(stg, TransferNI(*this, expr), 0);
  }

  /// return True iff that state is an Oc operation
  static BoolLattice isOutputState(const EState* estate, int c, bool endpoint,
				   BoolLattice joined_succs) {
    switch (estate->io.op) {
    case InputOutput::STDOUT_CONST: {
      const AType::ConstIntLattice& lval = estate->io.val;
      //cerr<<lval.toString()<<endl;
      assert(lval.isConstInt());
      // U=21, Z=26
      return c == rersChar(lval.getIntValue());
    }
    case InputOutput::STDOUT_VAR: {
      const PState& prop_state = *estate->pstate();
      //cerr<<estate->toString()<<endl;
      //cerr<<prop_state.varValueToString(estate->io.var)<<" lval="<<lval.toString()<<endl;
      assert(prop_state.varIsConst(estate->io.var));
      AValue aval = const_cast<PState&>(prop_state)[estate->io.var].getValue();
      //cerr<<aval<<endl;
      return c == rersChar(aval.getIntValue());
    }
    default:
      // Make sure that dead ends with no I/O show up as false
      if (endpoint)
	return false;

      return joined_succs;
    }
  }

  /**
   * Caveat: Although the LTL semantics say so, we can't start on
   * the start node. Maybe at the first I/O node?
   *
   * Think about ``oA U oB''.
   *
   * PLEASE NOTE: We therefore define oX to be true until oY occurs
   *   * if there is no output, we return false
   *
   * Implementation status: DONE
   */
  struct TransferO {
    TransferO(const UVerifier& v, const OutputSymbol* e) : verifier(v), expr(e) {}
    const UVerifier& verifier;
    const OutputSymbol* expr;
    LTLState operator() (const LTLState& s, BoolLattice joined_succs ) const { 
      LTLState newstate(s);
      BoolLattice old_val = s.val; 
      BoolLattice new_val = isOutputState(s.estate, expr->c, 
					  verifier.isEndpoint(s.estate), joined_succs);
      assert(new_val.lub(old_val) == new_val); // only move up in the lattice!
      newstate.val = new_val;
      cerr<<"  O(old="<<old_val<<", succ="<<joined_succs<<") = "<<new_val<<endl;
      return newstate;
    }
  }; 
  void visit(const OutputSymbol* expr) {
    analyze(stg, TransferO(*this, expr), 0);
  }
 

  /// return True iff that state is a !Oc operation
  static BoolLattice isNegOutputState(const EState* estate, int c, bool endpoint,
				      BoolLattice joined_succs) {
    switch (estate->io.op) {
    case InputOutput::STDOUT_CONST: {
      const AType::ConstIntLattice& lval = estate->io.val;
      //cerr<<lval.toString()<<endl;
      assert(lval.isConstInt());
      // U=21, Z=26
      return c != rersChar(lval.getIntValue());
    }
    case InputOutput::STDOUT_VAR: {
      // is there an output != c constraint?

      // var != c
      ListOfAValue l = estate->constraints()->getNeqVarConst(estate->io.var);
      for (ListOfAValue::iterator lval = l.begin(); lval != l.end(); ++lval) {
	if (lval->isConstInt())
	  if (c == rersChar(lval->getIntValue()))
	    return true;
      }

      // output == c constraint?
      const PState& prop_state = *estate->pstate();
      assert(prop_state.varIsConst(estate->io.var));
      AValue aval = const_cast<PState&>(prop_state)[estate->io.var].getValue();
      return c != rersChar(aval.getIntValue());
    }
    default:
      if (endpoint)
	return true;

      return joined_succs;
    }
  }

  /**
   * Negated version of OutputSymbol
   *
   * Implementation status: DONE
   */
  struct TransferNO {
    TransferNO(const UVerifier& v, const OutputSymbol* e) : verifier(v), expr(e) {}
    const UVerifier& verifier;
    const OutputSymbol* expr;
    LTLState operator() (const LTLState& s, BoolLattice joined_succs ) const { 
      LTLState newstate(s);
      BoolLattice old_val = s.val; 
      BoolLattice new_val = 
	isNegOutputState(s.estate, expr->c, verifier.isEndpoint(s.estate), joined_succs);
      assert(new_val.lub(old_val) == new_val); // only move up in the lattice!
      newstate.val = new_val;
      cerr<<"  !O(old="<<old_val<<", succ="<<joined_succs<<") = "<<new_val<<endl;
      return newstate;
    }
  }; 
  void visit(const NegOutputSymbol* expr) {
    analyze(stg, TransferNO(*this, expr), 0);
  }

  /**
   * NOT
   *
   * Implementation status: DONE
   */
  struct TransferNot {
    LTLState operator() (const LTLState& s, BoolLattice joined_succs ) const { 
      LTLState newstate(s);
      BoolLattice old_val = s.val;
      BoolLattice e1 = s.top();
      BoolLattice new_val = old_val.lub(!e1);
      assert(new_val.lub(old_val) == new_val); // only move up in the lattice!
      newstate.val = new_val;
      cerr<<"  G(old="<<old_val<<", e1="<<e1<<", succ="<<joined_succs<<") = "<<new_val<<endl;
      return newstate;
    } 
  };
  void visit(const Not* expr) {
    analyze(stg, TransferNot(), 1);
  }

  /**
   * X φ (next): φ has to hold after the next step
   *
   * I'm interpreting Next as follows
   *
   *  a     N A is true at a.
   *  |\
   * Ab Ac
   *
   * We simply join the information (A) from all successors.
   *
   * Implementation status: DONE
   */
  void visit(const Next* expr) {
    short e = expr->label;
    short e1 = expr->expr1->label;

    //FOR_EACH_STATE(state, label) {
    //  BoolLattice joined_succs = Bot();
    //
    //  /* for each successor */
    //  GraphTraits::out_edge_iterator out_i, out_end;
    //  for (tie(out_i, out_end) = out_edges(label, g); out_i != out_end; ++out_i) {
    //    Label succ = target(*out_i, g);
    //    joined_succs = (joined_succs LUB ltl_properties[label][e1]);
    //  }
    //  props[e] = joined_succs;
    //}
  }

  /**
   * F φ (eventually): φ has to hold at some point in the future (or now)
   *
   *
   * I'm interpreting Eventually to be a backward problem
   *
   *  a     if p(b) then F p(a) and F p(b) but not F p(c)
   *  |\
   *  b c
   *
   * propagate the information that the event occured up each path
   *
   * Implementation status: DONE
   */
  struct TransferF {
    LTLState operator() (const LTLState& s, BoolLattice joined_succs ) const { 
      LTLState newstate(s);
      BoolLattice old_val = s.val;
      BoolLattice e1 = s.top();
      BoolLattice new_val = old_val.lub(e1 || joined_succs);
      assert(new_val.lub(old_val) == new_val); // only move up in the lattice!
      newstate.val = new_val;
      cerr<<"  F(old="<<old_val<<", e1="<<e1<<", succ="<<joined_succs<<") = "<<new_val<<endl;
      return newstate;
    } 
  };
  void visit(const Eventually* expr) {
    analyze(stg, TransferF(), 1);

    //bw_fixpoint(/* init */      Bot(),
    //            ///* start */     !props[e1].isBot(),
    //            /* join */      LUB,
    //            /* transfer  */ props[e1] || joined_succs,
    //            /* debug */     NOP //cerr<<props[e1]<<" || "<<joined_succs<<endl;
    //            );
  }

  /**
   * G φ (globally): φ has to hold always (including now)
   *
   * I'm interpreting this as all states following the current one,
   * ignoring the past.
   *
   * True, iff for each state we have TRUE
   * Implementation status: DONE
   */

  struct TransferG {
    LTLState operator() (const LTLState& s, BoolLattice joined_succs ) const { 
      LTLState newstate(s);
      BoolLattice old_val = s.val;
      BoolLattice e1 = s.top();
      BoolLattice new_val = old_val.lub(e1 && joined_succs);
      assert(new_val.lub(old_val) == new_val); // only move up in the lattice!
      newstate.val = new_val;
      cerr<<"  G(old="<<old_val<<", e1="<<e1<<", succ="<<joined_succs<<") = "<<new_val<<endl;
      return newstate;
    } 
  };
  void visit(const Globally* expr) {
    analyze(stg, TransferG(), 1);

//    if (expr->quantified) {
//      bw_fixpoint(/* init */      Bot(),
//                  /* transfer  */ props[e1] && joined_succs,
//                  /* debug */     NOP //cerr<<props[e1]<<" && "<<joined_succs<<endl;
//                  );
//    } else {
//      bw_fixpoint(/* init */      Bot(),
//                ///* start */     !props[e1].isBot(),
//                /* join */      AND, // this allows for improved precision in the case of an All-quantified G node, which is, by, default, any outermost G node
//                /* transfer  */ props[e1] && joined_succs,
//                /* debug */     NOP //cerr<<props[e1]<<" && "<<joined_succs<<endl;
//                );
//    };
  }

  // Implementation status: DONE
 struct TransferAnd {
    LTLState operator() (const LTLState& s, BoolLattice joined_succs ) const { 
      LTLState newstate(s);
      BoolLattice old_val = s.val;
      BoolLattice e1 = s.over();
      BoolLattice e2 = s.top();
      BoolLattice new_val = old_val.lub(e1 && e2);
      assert(new_val.lub(old_val) == new_val); // only move up in the lattice!
      newstate.val = new_val;
      cerr<<"  And(old="<<old_val
	  <<", e1="<<e1<<", e2="<<e1
	  <<", succ="<<joined_succs<<") = "<<new_val<<endl;
      return newstate;
    } 
  };
  void visit(const And* expr) {
    analyze(stg, TransferAnd(), 2);
    //  FOR_EACH_STATE(state, label)
    //  props[e] = props[e1] && props[e2];
  }

  // Implementation status: DONE
 struct TransferOr {
    LTLState operator() (const LTLState& s, BoolLattice joined_succs ) const { 
      LTLState newstate(s);
      BoolLattice old_val = s.val;
      BoolLattice e1 = s.over();
      BoolLattice e2 = s.top();
      BoolLattice new_val = old_val.lub(e1 || e2);
      assert(new_val.lub(old_val) == new_val); // only move up in the lattice!
      newstate.val = new_val;
      cerr<<"  Or(old="<<old_val
	  <<", e1="<<e1<<", e2="<<e1
	  <<", succ="<<joined_succs<<") = "<<new_val<<endl;
      return newstate;
    } 
  };
  void visit(const Or* expr) {
    analyze(stg, TransferOr(), 2);

    //FOR_EACH_STATE(state, label)
    //  props[e] = props[e1] || props[e2];
  }

  /**
   * φ U ψ (until): φ has to hold until ψ holds (which eventually occurs)
   *
   * A holds until B occurs
   *
   * I'm interpreting UNTIL as follows:
   *
   *  a
   *  |\
   * Ab \    A U B is valid at b and c, e = ⊥
   *  | Ad
   * Bc
   *  |
   *  e
   *
   * Implementation status: DONE
   */
 struct TransferU {
    LTLState operator() (const LTLState& s, BoolLattice joined_succs ) const { 
      LTLState newstate(s);
      BoolLattice old_val = s.val;
      BoolLattice e1 = s.over();
      BoolLattice e2 = s.top();
      BoolLattice new_val = old_val.lub(e2 || (e1 && joined_succs));
      assert(new_val.lub(old_val) == new_val); // only move up in the lattice!
      newstate.val = new_val;
      cerr<<"  Until(old="<<old_val
	  <<", e1="<<e1<<", e2="<<e1
	  <<", succ="<<joined_succs<<") = "<<new_val<<endl;
      return newstate;
    } 
  };
  void visit(const Until* expr) {
    analyze(stg, TransferU(), 2);
//    bw_fixpoint(/* init */      Bot(),
//                ///* start */     (!props[e1].isBot() || !props[e2].isBot()),
//                /* join */      LUB,
//                /* transfer */  (props[e2] || (props[e1] && joined_succs)) || false,
//                /* debug */     NOP
//                //cerr<<label<<": "<<((props[e2] || (props[e1] && joined_succs)) || false).toString()<<" == "<<props[e2]<<" || ("<<props[e1]<<" && "<<joined_succs<<")"<<endl
//                );
  }

  /**
   * φ WU ψ (weak until): φ has to hold until ψ holds (which does not necessarily occur)
   *
   * A holds until B occurs, which may never happen
   *
   * I'm interpreting WEAK UNITL as follows:
   *
   *  a
   *  |\
   * Ab \    A WU B is valid at b and c and f
   *  | Ad
   * Ac  |\
   *     e Bf
   *
   * Implementation status: DONE
   */
  void visit(const WeakUntil* expr) {
    assert(false);
  }

  /**
   * φ R ψ (release): φ has to hold until ψ held in the previous step.
   *
   * If !B occurs, A happens before it.
   *
   * According to the unwinding property, R seems to be the dual operator of U.
   * Implementation status: DONE
   */
 struct TransferR {
    LTLState operator() (const LTLState& s, BoolLattice joined_succs ) const { 
      LTLState newstate(s);
      BoolLattice old_val = s.val;
      BoolLattice e1 = s.over();
      BoolLattice e2 = s.top();
      BoolLattice new_val = old_val.lub(e2 && (e1 || joined_succs));
      assert(new_val.lub(old_val) == new_val); // only move up in the lattice!
      newstate.val = new_val;
      cerr<<"  Until(old="<<old_val
	  <<", e1="<<e1<<", e2="<<e1
	  <<", succ="<<joined_succs<<") = "<<new_val<<endl;
      return newstate;
    } 
  };
  void visit(const Release* expr) {
    analyze(stg, TransferR(), 2);

   // bw_fixpoint(/* init */      Bot(),
   //             ///* start */     (!props[e1].isBot() || !props[e2].isBot()),
   //             /* join */      LUB,
   //             /* transfer */  props[e2] && (props[e1] || joined_succs),
   //             /* debug */     NOP
   //             );
  }
};


UChecker::UChecker(EStateSet& ess, TransitionGraph& _tg)
  : transitionGraph(_tg),
    eStateSet(ess)
{
  // Build our own customized Transition graph
  int i = 0;
  map<const EState*, Label> estate_label;
  FOR_EACH_ESTATE(state, l1) {
    estate_label[&(*state)] = i++;
  }

  BoostTransitionGraph full_graph(ess.size());

  FOR_EACH_TRANSITION(t) {
    Label src = estate_label[((*t).source)];
    Label tgt = estate_label[((*t).target)];
    add_edge(src, tgt, full_graph);
    full_graph[src] = (*t).source;
    full_graph[tgt] = (*t).target;
    //cerr<<src<<"("<<t->source<<") -- "<<tgt<<"("<<t->target<<")"<<endl;
    assert(full_graph[src]);
    assert(full_graph[tgt]);
  }
  //start = estate_label[transitionGraph.begin()->source];
  start = estate_label[transitionGraph.getStartTransition().source];
#if 0
  cout << "DEBUG: START"<<transitionGraph.begin()->source
	   <<", news: "<<transitionGraph.getStartTransition().source
	   <<", newt: "<<transitionGraph.getStartTransition().target
	   <<endl;
#endif

#if 1
  // Optimization
  start = collapse_transition_graph(full_graph, g);
#else
  g = full_graph;
#endif

  //FOR_EACH_STATE(state, label) {
  //  cerr<<label<<": "<<state->toString()<<endl;
  //}
}

/**
 * Optimization: remove all nodes that are non-I/O and not relevant to
 * the control flow
 *
 * Creates reduced_eStateSet
 */
Label UChecker::collapse_transition_graph(BoostTransitionGraph& g, 
					  BoostTransitionGraph& reduced) const {
  Label n = 0;
  Label renumbered[num_vertices(g)];

  FOR_EACH_STATE(state, label) {
    //cerr<<label<<endl;
    assert(g[label]);
    if (( in_degree(label, g) >= 1) && // keep start
	(out_degree(label, g) >= 0) && // DO NOT keep exits
	(g[label]->io.op == InputOutput::NONE)) {
      //cerr<<"-- removing "<<label <<endl;//g[label]->toString()<<endl;

      // patch pred <--> succ
      GraphTraits::in_edge_iterator in_i, in_end;			
      for (tie(in_i, in_end) = in_edges(label, g); in_i != in_end; ++in_i) { 
	Label pred = source(*in_i, g);

	GraphTraits::out_edge_iterator out_i, out_end;
	for (tie(out_i, out_end) = out_edges(label, g); out_i != out_end; ++out_i) {
	  Label succ = target(*out_i, g);
	
	  //cerr<<"-- connecting "<<pred<<" and "<<succ<<endl;
	  add_edge(pred, succ, g);
	}
      }
      // remove state
      clear_vertex(label, g);
      // but don't remove_vertex(label, g), since we don't want the
      // boost graph to reassign numerical labels!
    } else {
      //cerr<<"-- keeping "<<label<<": "<<g[label]->toString()<<endl;
      renumbered[label] = n++;
      add_vertex(reduced);
    }
    //cerr<<"-- done "<<endl<<endl;
  }

  // Build a copy of the graph without the orphaned states
  //cerr<<"digraph g {"<<endl;
  GraphTraits::edge_iterator ei, ei_end;
  for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
    Label src = source(*ei, g);
    Label tgt = target(*ei, g);
    add_edge_if_new1(renumbered[src], renumbered[tgt], reduced);
    cerr<<renumbered[src]<<" -> "<<renumbered[tgt]<<";"<<endl;
    reduced[renumbered[src]] = g[src];
    reduced[renumbered[tgt]] = g[tgt];
  }
  //cerr<<"}"<<endl;

  //cerr<<"## done "<<endl<<endl;
  return renumbered[start];
}


BoolLattice
UChecker::verify(const Formula& f)
{
  // Verify!
  UVerifier v(eStateSet, g, start, num_vertices(g), f.size());
  const Expr& e = f;
  e.accept(v);

  // Visualization:
  bool ltl_output_dot = boolOptions["ltl-output-dot"];//  true;
  bool show_derivation = boolOptions["ltl-show-derivation"];//  true;
  bool show_node_detail = boolOptions["ltl-show-node-detail"];//  true;
  bool collapsed_graph = boolOptions["ltl-collapsed-graph"];//  false

  if (ltl_output_dot) {
    stringstream s;
    s<<"digraph G {\n";
    s<<"node[shape=rectangle, color=lightsteelblue, style=filled];\n  ";


    LTLGraphTraits::edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = edges(v.stg.g); ei != ei_end; ++ei) {
      s<<"\""<<source(*ei, v.stg.g)<<"\" -> \""<<target(*ei, v.stg.g)<<"\";\n";
    }

    LTLGraphTraits::vertex_iterator vi, vi_end;
    for (tie(vi, vi_end) = vertices(v.stg.g); vi != vi_end; ++vi) {
      LTLState state = v.stg.g[*vi];
      s<<"\""<<*vi<<"\" [label=\""<<state<<"\"";
      switch (state.estate->io.op) {
      case InputOutput::STDIN_VAR:
	s<<"shape=rectangle, color=gold, style=filled";
	break;
      case InputOutput::STDOUT_VAR:
      case InputOutput::STDOUT_CONST:
	s<<"shape=rectangle, color=indigo, style=filled";
	break;
      default: break;
      }
      s<<"];\n  ";
    }
    // FOR_EACH_STATE(state, l) {
    //   Visualizer viz(v.ltl_properties, l);
    //   const_cast<Expr&>(e).accept(viz, Visualizer::newAttr(l));
    //   s<<l<<" [label=\""<<l;
    //   if (show_node_detail) {
    // 	s<<":";
    // 	if (collapsed_graph) {

    // 	} else s<<state->toString();
    //   }
    //   s<<"\"] ;\n";
    //   s<<"subgraph ltl_"<<l<<" {\n";
    //   s<<"  node[shape=rectangle, style=filled];\n  ";
    //   if (show_derivation) s<<viz.s.str();
    //   s<<"}\n";
    // }
    s<<"}\n";

    ofstream myfile;
    stringstream fname;
    static int n = 1;
    fname << "ltl_output_" << n++ << ".dot";
    myfile.open(fname.str().c_str(), ios::out);
    myfile << s.str();
    myfile.close();
    cout<<"generated "<<fname.str()<<"."<<endl;
  }



  // Find the disjunction of all start states; we are looking for
  // all-quantified LTL formulae only
  BoolLattice b = Bot();
  LTLGraphTraits::vertex_iterator lvi, lvi_end;
  for (tie(lvi, lvi_end) = vertices(v.stg.g); lvi != lvi_end; ++lvi) {
    LTLState s = v.stg.g[*lvi];
    if (in_degree(*lvi, v.stg.g) == 0) {
      cerr<<"Value at START = "<<s.valstack.back()<<endl;
      b = b && s.valstack.back();
    }
  }
  return b;
}

