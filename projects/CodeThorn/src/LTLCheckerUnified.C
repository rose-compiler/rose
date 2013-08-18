#include "sage3basic.h"

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

// really expensive debug option -- prints a dot file at every iteration
// use the following BASH code to join everything into one PDF:
/* $ for f in $(ls -1 -v --color=never ltl_anim_*.dot); do    \
       dot -Tpdf $f >$f.pdf; \
     done; \
     pdfjoin $(ls -1 -v --color=never ltl_anim_*.pdf) --outfile ltl_anim-joined.pdf \
     && xpdf ltl_anim-joined.pdf
*/

//#define SHOW_WORKLIST_GROWTH
//#define REDUCE_DEBUG
//#define ANIM_OUTPUT
#ifdef ANIM_OUTPUT
#define ANIM_START 0
#define ANIM_END 300
static int anim_i = 0;
#endif

////////////////////////////////////////////////////////////////////////
// Convenience loop macros for dealing with boost graphs.
// Hopefully they increase the readability of the algorithms.
////////////////////////////////////////////////////////////////////////
#define FOR_EACH_TRANSITION(TRANSITION)                              \
  for (TransitionGraph::const_iterator TRANSITION=transitionGraph.begin(); \
       TRANSITION != transitionGraph.end();                \
       ++TRANSITION)

#define FOR_EACH_ESTATE(STATE, LABEL)                                \
  Label LABEL=0;                                                     \
  for (EStateSet::const_iterator STATE=eStateSet.begin();            \
       STATE != eStateSet.end();                                     \
       ++STATE, ++LABEL)

#define FOR_EACH_STATE(STATE, LABEL)                    \
  GraphTraits::vertex_iterator vi, vi_end;                \
  Label LABEL;                                \
  const EState* STATE;                            \
  for (tie(vi, vi_end) = vertices(g), LABEL=*vi, STATE=g[LABEL];    \
       vi != vi_end; ++vi,                        \
       LABEL=(vi!=vi_end)?*vi:NULL, STATE=g[LABEL])            \

#define END_FOR }}

#define FOR_EACH_VERTEX(V, G) {                        \
  LTLGraphTraits::vertex_iterator i, i_end;            \
  for (tie(i, i_end) = vertices(G); i != i_end; ++i) {    \
    LTLVertex V = *i;

#define FOR_EACH_PREDECESSOR(PRED, V, G) {                \
  LTLGraphTraits::in_edge_iterator in_i, in_end;            \
  for (tie(in_i, in_end) = in_edges(V, G); in_i != in_end; ++in_i ) { \
    LTLVertex PRED = source(*in_i, G);

#define FOR_EACH_SUCCESSOR(SUCC, V, G) {                \
  LTLGraphTraits::out_edge_iterator out_i, out_end;            \
  for (tie(out_i, out_end) = out_edges(V, G); out_i != out_end; ++out_i ) { \
    LTLVertex SUCC = target(*out_i, G);


/**
 * is there an edge v->v?
 */
template<class Vertex, class Graph>
bool selfcycle(Vertex v, Graph& g) {
  LTLGraphTraits::out_edge_iterator out_i, out_end;
  for (tie(out_i, out_end) = out_edges(v, g); out_i != out_end; ++out_i) {
    LTLVertex succ = target(*out_i, g);
    if (succ == v)
      return true;

    //    LTLGraphTraits::in_edge_iterator in_i, in_end;
    //for (tie(in_i, in_end) = in_edges(v, g); in_i != in_end; ++in_i) {
    //  LTLVertex pred = source(*in_i, g);
    //  if (pred == succ)
    //	return true;
    //}
  }

  return false;
}

/**
 * \return true if out_degree is 0 or the only outgoing edge is a loop.
 */
template<class Vertex, class Graph>
bool is_leaf(Vertex v, Graph& g) {
  int outd = out_degree(v, g);
  int ind = in_degree(v, g);
  return outd==0 || (outd==1 && ind==1 && selfcycle(v, g));
}


/**
 * \return all predecessors of v
 */
static
LTLWorklist predecessors(const LTLVertex& v, const LTLTransitionGraph& g) {
  LTLWorklist preds;
  FOR_EACH_PREDECESSOR(pred, v, g) {
    preds.push(pred);
  } END_FOR
  return preds;
}



/**
 * DOT visualization of the LTL Checker result
 */
class UVisualizer: public TopDownVisitor {
public:
  UVisualizer(LTLState& st, int l)
    : state(st), label(l), n(65536) {}
  LTLState& state;
  stringstream s;
  int label;
  int n; // consecutive labeling of dot node
  static const int shift = 24;

  struct Attr: public InheritedAttribute {
    Attr(int i) : dot_label(i) {}
    int dot_label;
  };
  // MS: removed smart pointer
  //static Attr* get(IAttr a) { return static_cast<Attr*>(a.get()); }
  static Attr* get(IAttr a) { return static_cast<Attr*>(a); }
  static IAttr newAttr(int n)  { return IAttr((InheritedAttribute*)new Attr(n)); }
  int newNode(IAttr a) {
    int node = (label<<shift)+n++;
    s<<node<<" -> "<<get(a)->dot_label<<" [color=limegreen, weight=2, style=dashed];\n    ";
    return node;
  }
  static string color(BoolLattice lval) {
    if (lval.isTrue())  return  "color=palegreen";
    if (lval.isFalse()) return  "color=salmon";
    if (lval.isTop())   return  "color=azure";
    if (lval.isBot())   return  "color=gainsboro";
    throw "error";
  }

  IAttr visit(InputSymbol* expr, IAttr a)  {
    short e = expr->label;
    int node = newNode(a);
    s<<node<<" ["<<color(state.valstack[e])<<",label=\"Input "<<string(1, expr->c)
     <<" = "<<state.valstack[e]<<"\"];\n    ";
    return newAttr(node);
  }
  IAttr visit(OutputSymbol* expr, IAttr a) {
    short e = expr->label;
    int node = newNode(a);
    s<<node<<" ["<<color(state.valstack[e])<<",label=\"Output "<<string(1, expr->c)
     <<" = "<<state.valstack[e]<<"\"];\n    ";
    return newAttr(node);
  }
  IAttr visit(NegInputSymbol* expr, IAttr a)  {
    short e = expr->label;
    int node = newNode(a);
    s<<node<<" ["<<color(state.valstack[e])<<",label=\"¬Input "<<string(1, expr->c)
     <<" = "<<state.valstack[e]<<"\"];\n    ";
    return newAttr(node);
  }
  IAttr visit(NegOutputSymbol* expr, IAttr a) {
    short e = expr->label;
    int node = newNode(a);
    s<<node<<" ["<<color(state.valstack[e])<<",label=\"¬Output "<<string(1, expr->c)
     <<" = "<<state.valstack[e]<<"\"];\n    ";
    return newAttr(node);
  }
  IAttr visit(Not* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    //cerr<<"e="<<e<<endl;
    //cerr<<"e1="<<e1<<endl;
    //cerr<<"state.valstack[e]="<<state.valstack[e]<<endl;
    //cerr<<"state.valstack[e1]="<<state.valstack[e1]<<endl;
    int node = newNode(a);
    s<<node<<" ["<<color(state.valstack[e])<<",label=\""<<state.valstack[e]
     <<" = "<<"!"<<state.valstack[e1]<<"\"];\n    ";
    return newAttr(node);
  }
  IAttr visit(Next* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    int node = newNode(a);
    s<<node<<" [shape=circle,"<<color(state.valstack[e])<<",label=\""<<"("<<state.valstack[e]
     <<" = "<<"X "<<state.valstack[e1]<<")\"];\n    ";
    return newAttr(node);
  }
  IAttr visit(Eventually* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    int node = newNode(a);
    s<<node<<" [shape=diamond,"<<color(state.valstack[e])<<",label=\""<<"("<<state.valstack[e]
     <<" = "<<"F "<<state.valstack[e1]<<")\"];\n    ";
    return newAttr(node);
  }
  IAttr visit(Globally* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    int node = newNode(a);
    s<<node<<" [shape=box,"<<color(state.valstack[e])<<",label=\""<<"("<<state.valstack[e]
     <<" = "<<"G "<<state.valstack[e1]<<")\"];\n    ";
    return newAttr(node);
  }
  IAttr visit(And* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    short e2 = expr->expr2->label;
    int node = newNode(a);
    s<<node<<" ["<<color(state.valstack[e])<<",label=\""<<"("<<state.valstack[e]
     <<" = "<<state.valstack[e1]<<" & "<<state.valstack[e2]<<")\"];\n    ";
    return newAttr(node);
  }
  IAttr visit(Or* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    short e2 = expr->expr2->label;
    int node = newNode(a);
    s<<node<<" ["<<color(state.valstack[e])<<",label=\""<<"("<<state.valstack[e]
     <<" = "<<state.valstack[e1]<<" | "<<state.valstack[e2]<<")\"];\n    ";
    return newAttr(node);
  }
  IAttr visit(Until* expr, IAttr a)    {
    short e = expr->label;
    short e1 = expr->expr1->label;
    short e2 = expr->expr2->label;
    int node = newNode(a);
    s<<node<<" ["<<color(state.valstack[e])<<",label=\""<<"("<<state.valstack[e]
     <<" = "<<state.valstack[e1]<<" U "<<state.valstack[e2]<<")\"];\n    ";
    return newAttr(node);
  }
  IAttr visit(WeakUntil* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    short e2 = expr->expr2->label;
    int node = newNode(a);
    s<<node<<" ["<<color(state.valstack[e])<<",label=\""<<"("<<state.valstack[e]
     <<" = "<<state.valstack[e1]<<" WU "<<state.valstack[e2]<<")\"];\n    ";
    return newAttr(node);
  }
  IAttr visit(Release* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    short e2 = expr->expr2->label;
    int node = newNode(a);
    s<<node<<" ["<<color(state.valstack[e])<<",label=\""<<"("<<state.valstack[e]
     <<" = "<<state.valstack[e1]<<" R "<<state.valstack[e2]<<")\"];\n    ";
    return newAttr(node);
  }
};

ostream& UnifiedLTL::operator<<(ostream& os, const LTLState& s) {
  os << "LTLState(" << s.estate->toString() <<": "<<", [ ";
  for (vector<BoolLattice>::const_iterator i = s.valstack.begin();
       i != s.valstack.end(); ++i) {
    os<<*i<<" ";
  }
  os<<"])";
  return os;
}

string UnifiedLTL::LTLState::toHTML() const {
  stringstream s;
  s << estate->toHTML();
  s << "LTLState(" << " [ ";
  for (vector<BoolLattice>::const_iterator i = valstack.begin();
       i != valstack.end(); ++i) {
    s<<*i<<" ";
  }
  s<<"]) <BR />";
  return s.str();
}

vector< pair<int, int> > focus_history;

struct VertexSorter {
  const LTLStateTransitionGraph& stg;
  VertexSorter(const LTLStateTransitionGraph& g) : stg(g) {}
  bool operator() (LTLVertex a, LTLVertex b) {
    return stg.g[a].estate->label() < stg.g[b].estate->label();
  }
};

string visualize(const LTLStateTransitionGraph& stg, const Expr& e,
	 LTLVertex focus0=NULL, LTLVertex focus1=NULL, LTLVertex focus2=NULL) {
  bool show_derivation = boolOptions["ltl-show-derivation"];
  stringstream s;
  s<<"digraph G {\n";
  s<<"  node[shape=rectangle, color=lightsteelblue, style=filled];\n  ";

  vector<LTLVertex> sorted_vs;
  map<LTLVertex, int> label;
  int l = 0;
  FOR_EACH_VERTEX(vi, stg.g) {
    label[vi] = ++l;
    sorted_vs.push_back(vi);
  } END_FOR;
  sort(sorted_vs.begin(), sorted_vs.end(), VertexSorter(stg));

  const EState* last_estate = NULL;
  int last_label = 0;
  for (size_t i=0; i<sorted_vs.size(); i++) {
    // group them by similar estates
    LTLVertex vi = sorted_vs[i];
    LTLState state = stg.g[vi];
    l = label[vi];
    if (i>0 && state.estate->label() == last_estate->label()) {
      //s<<"  "<<l<<"->"<<last_label<<" [style=\"invis\",weight=99];\n";
    }
    last_estate = state.estate;
    last_label = l;

    s<<"    "<<l<<" [label=<\n"<<state.toHTML()<<"\n>, ";
    if (focus0 && focus0 == vi)
      s<<"shape=rectangle, color=chartreuse, style=filled";
    else
    if (focus1 && focus1 == vi)
      s<<"shape=rectangle, color=honeydew, style=filled";
    else
    if (focus2 && focus2 == vi)
      s<<"shape=rectangle, color=gray, style=filled";
    else
    switch (state.estate->io.op) {
    case InputOutput::STDIN_VAR:
      s<<"shape=rectangle, color=gold, style=filled";
      break;
    case InputOutput::STDOUT_VAR:
    case InputOutput::STDOUT_CONST:
      s<<"shape=rectangle, color=slateblue, style=filled";
      break;
    default: break;
    }
    s<<"];\n  ";

    // LTL visualization
    UVisualizer viz(state, l);
    s<<"subgraph ltl_"<<l<<" {\n";
    s<<"    node[shape=rectangle, style=filled];\n  ";
    if (show_derivation) {
      assert(state.valstack.size() == ltl_label);
      const_cast<Expr&>(e).accept(viz, UVisualizer::newAttr(l));
      s<<viz.s.str();
    }
    s<<"  }\n";
  }

  // draw the cut-off edges
  for (size_t i=0; i<focus_history.size();i++)
    s<<focus_history[i].first<<" -> "<<focus_history[i].second<<" [color=gray, style=dotted, constraint=false];\n  ";

  if (focus1 && focus2) {
    focus_history.push_back(make_pair(label[focus1], label[focus2]));
    s<<label[focus1]<<" -> "<<label[focus2]<<" [color=salmon, style=dashed];\n  ";
  }

  LTLGraphTraits::edge_iterator ei, ei_end;
  for (tie(ei, ei_end) = edges(stg.g); ei != ei_end; ++ei) {
    s<<label[source(*ei, stg.g)]<<" -> "<<label[target(*ei, stg.g)]<<";\n";
  }

  s<<"}\n";

  return s.str();
}




/**
 * We perform the verification of the LTL formula by traversing it
 * bottom-up and verifying all the properties as we move upwards.
 *   This requires up to N fixpoint iterations over the transition
 * graph, where N is the number of sub-expressions in f.
 *
 * TODO: Are there better names for these classes?
 * JOINS should have the same information
 */
class UVerifier {
  EStateSet& eStateSet;
  deque<const EState*> endpoints;
  Label start;
  unsigned short progress;
  const Formula& f;

  /// To avoid recomputing I/O LTL-leafs, we keep track of common subexpressions
  vector< pair<LTL::NodeType, char> > stack_contents;
  VariableIdMapping::VariableIdSet input_vars;

public:
  LTLStateTransitionGraph stg;

  UVerifier(EStateSet& ess, BoostTransitionGraph& g,
	Label start_label, Label max_label, const Formula& _f)
    : eStateSet(ess), start(start_label), progress(0), f(_f) {
    // reserve a result map for each label
    // it maps an analysis result to each sub-expression of the ltl formula

    // Create the LTLStateTransitionGraph
    GraphTraits::vertex_iterator vi, vi_end;
    for (tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi) {
      LTLState state(g[*vi], f.size());
      LTLVertex v = add_vertex(stg.g);
      stg.vertex[state] = v;
      stg.g[v] = state;
    }

    GraphTraits::edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
      Label src = source(*ei, g);
      Label tgt = target(*ei, g);
      add_edge(stg.vertex[LTLState(g[src], f.size())],
	   stg.vertex[LTLState(g[tgt], f.size())],
	   stg.g);
    }

    // keep track of all endpoints
    LTLGraphTraits::vertex_iterator lvi, lvi_end;
    for (tie(lvi, lvi_end) = vertices(stg.g); lvi != lvi_end; ++lvi) {
      if (out_degree(*lvi, stg.g) == 0)
    endpoints.push_back(stg.g[*lvi].estate);
    }

  }

  static void updateInputVar(const EState* estate, VariableIdMapping::VariableIdSet& input_vars) {
    if (estate->io.op == InputOutput::STDIN_VAR) {
      input_vars.insert(estate->io.var);
    }
  }


  /**
   * Look for an LTLVertex of new_state and return it,
   * otherwise create a new LTLVertex and add it to worklist
   */
  LTLVertex add_state_if_new(LTLState new_state,
		 LTLStateTransitionGraph& stg,
		 LTLWorklist& worklist) {
    LTLVertex v;
    LTLStateMap::iterator i = stg.vertex.find(new_state);
    if (i == stg.vertex.end()) {
#ifdef EXTRA_ASSERTS
      for (i=stg.vertex.begin(); i != stg.vertex.end(); ++i) {
	LTLState s = (*i).first;
	assert(! (s == new_state));
      }
      FOR_EACH_VERTEX(x, stg.g)
	assert(!(stg.g[x] == new_state));
      END_FOR;
#endif
      // Create a new vertex
      v = add_vertex(stg.g);
      stg.vertex[new_state] = v;
      stg.g[v] = new_state;
      worklist.push(v);
      //cerr<<"** added new node "<<v<<": "<<new_state<<endl;
    } else {
      v = (*i).second;

#ifdef MERGE_TOP_STATES
      if (new_state.val.isTrue() || new_state.val.isFalse()) {
	// If there is both A and !A, create a single A=Top instead
	LTLState neg_state = new_state;
	neg_state.val = !new_state.val;
	LTLStateMap::iterator j = stg.vertex.find(neg_state);
	if (j != stg.vertex.end()) {
	  LTLState top_state = new_state;
	  top_state.val = Top();
	  v = add_state_if_new(top_state, stg, worklist);
	  
	  FOR_EACH_PREDECESSOR(pred, (*i).second, stg.g)
	    add_edge(pred, v, stg.g);
	  END_FOR;
	  FOR_EACH_SUCCESSOR(succ, (*i).second, stg.g)
	    add_edge(v, succ, stg.g);
	  END_FOR;
	  clear_vertex((*i).second, stg.g);

	  FOR_EACH_PREDECESSOR(pred, (*j).second, stg.g)
	    add_edge(pred, v, stg.g);
	  END_FOR;
	  FOR_EACH_SUCCESSOR(succ, (*j).second, stg.g)
	    add_edge(v, succ, stg.g);
	  END_FOR;
	  clear_vertex((*j).second, stg.g);

	  //cerr<<"** 3-way merged states "<<top_state<<endl;
	}
      }
#endif
      //cerr<<"** merged states "<<new_state<<endl;
    }
    return v;
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
  void analyze(bool verbose)  {
    LTLWorklist worklist;
    unordered_set<LTLVertex> endpoints;
    boost::unordered_set< pair<LTLVertex, LTLVertex> > dead_edges;

    // dummy exit state with all lattice values set to bot
    LTLState end_state(NULL, f.size());

    //cerr<<"\n-------------------------------------------"<<endl;
    //cerr<<"analyze() initializing..."<<endl;
    stg.vertex.clear();
    FOR_EACH_VERTEX(v, stg.g) {
      // push bot to the LTL stack and update the vertex map
      LTLState state = stg.g[v];
      stg.g[v] = state;
      //cerr<<v<<": "<<state<<", "<<v<<endl;
      //size_t old = stg.vertex.size();
      stg.vertex[state] = v;
      //assert(stg.vertex.size()==old+1);

      if (out_degree(v, stg.g) == 0) {
	//cerr<<"registered endpoint "<<stg.g[v]<<endl;
	endpoints.insert(v);
      }

      // scan the entire STG for input vars
      updateInputVar(state.estate, input_vars);

      worklist.push(stg.vertex[state]);
    } END_FOR;

    //cerr<<stg.vertex.size()<<endl;
    //cerr<<num_vertices(stg.g)<<endl;
    assert(stg.vertex.size() == num_vertices(stg.g));

    if (worklist.empty()) {
      cerr<<"** WARNING: empty worklist!"<<endl;
    }
    //cerr<<endl;
    while (!worklist.empty()) {
#ifdef SHOW_WORKLIST_GROWTH
      cerr<<worklist.size()<<"\r";
#endif
      LTLVertex succ = worklist.pop();

      // Endpoint handling
      if (is_leaf(succ, stg.g)) {
	if (endpoints.count(succ) == 0) {
	  //cerr<<"orphan: "<<succ<<","<<stg.g[succ]<<endl;
	  // Orphan: did it become an orphan because it was replaced by a new node?
	  FOR_EACH_PREDECESSOR(pred, succ, stg.g) {
	    FOR_EACH_SUCCESSOR(s, pred, stg.g)
	      worklist.push(s);
	    END_FOR;
	    // should be redundant: worklist.push(pred);
	  } END_FOR;
	  // Cut it off.
	  clear_vertex(succ, stg.g);
	} else {
	  //cerr<<"hit a real endpoint: "<<succ<<endl;
	  // Real endpoint (ie. exit/assert)
	  // Endpoints always receive a strong update, because there is no ambiguity
	  LTLState old_state = stg.g[succ];
	  LTLState new_state = transfer(old_state, end_state, verbose, true);
	  stg.g[succ] = new_state;
	  stg.vertex[new_state] = succ;
	}
      }

      // Store all successors in the temporary list vs because our
      // actions destroy the iterator
      LTLWorklist vs = predecessors(succ, stg.g);

      // for each predecessor
      while (!vs.empty()) {
	LTLVertex v = vs.pop();

	//if (stg.g[v].estate->label() == 612)
	//   verbose = true;

	if (verbose) {
	  cerr<<"\n** Visiting state "<<v<<","<<stg.g[v]<<endl;
	  cerr<<"  in_degree = "<< in_degree(v, stg.g)<<endl;
	  cerr<<" out_degree = "<<out_degree(v, stg.g)<<endl;
	}

	// Always make a new state v', so we can calculate precise
	// results for both paths.  If a v' and v'' should later turn
	// out to be identical, they will automatically be merged.
	LTLState old_state = stg.g[v];
	LTLState new_state = transfer(old_state, stg.g[succ], verbose, false);
	bool fixpoint = (new_state == old_state);

	if (fixpoint) {

	  if (verbose) cerr<<"reached fixpoint!"<<endl;
	  assert(stg.g[v].valstack == new_state.valstack);

	} /*else if (old_state.top().isBot()) {

	  // Performance shortcut: perform a strong update.
	  // This is legal because we ignore old_val in the transfer function anyway.
	  // Benefit: it reduces the total number of nodes
	  stg.vertex[new_state] = v;
	  stg.g[v] = new_state;
	  worklist.push(v);

	  } */else {

	  // create a new state in lieu of the old state, and cut off the old state
	  // if a new v' is created, it is put into the worklist automatically
	  LTLVertex v_prime = add_state_if_new(new_state, stg, worklist);
	  if (verbose && v != v_prime) {
	    cerr<<"  OLD: "<<old_state<< "\n  NEW: "<<new_state<<endl;
	    cerr<<"  v: "<<v<<","<<stg.g[v]<<" = "<<stg.g[v].top()<<endl;
	  }
	  //assert (v_prime != v || (new_state.val != old_state.val));

	  // Add edge from new state to successor
	  if (succ == v) {
	    // self-cycle
	    add_edge(v_prime, v_prime, stg.g);
	    //cerr<<v_prime<<" -> "<<v_prime<<endl;
	  } else {
	    add_edge(v_prime, succ, stg.g);
	    //cerr<<v_prime<<" -> "<<succ<<endl;

	    // Cut off the old state
	    remove_edge(v, succ, stg.g);
	    dead_edges.insert(make_pair(v, succ));

	    // Add edge from predecessor to new state
	    FOR_EACH_PREDECESSOR(pred, v, stg.g) {
	      if (pred == v) add_edge(v_prime, v_prime, stg.g);
	      else {
		// if we marked it as dead, a previous iteration
		// already proved this connection to be a dead end and
		// hooked that node up to the real successor. If we
		// don't perform this check we might get into an
		// infinite loop.
		if (dead_edges.find(make_pair(pred, v_prime)) == dead_edges.end())
		  add_edge(pred, v_prime, stg.g);
	      }
	    } END_FOR;

	    // add all remaining successors of the old v to the
	    // worklist to force v to be recomputed
	    FOR_EACH_SUCCESSOR(v_succ, v, stg.g)
	      worklist.push(v_succ);
	    END_FOR;
	    // should be redundant: worklist.push(v);

#ifdef ANIM_OUTPUT
	    if (anim_i++ > ANIM_START) {
	      ofstream animfile;
	      stringstream fname;
	      fname << "ltl_anim_" << setw(3) << setfill('0') << anim_i << ".dot";
	      animfile.open(fname.str().c_str(), ios::out);
	      const Expr& e = f;
	      animfile << visualize(stg, e, v_prime, v, succ);
	      animfile.close();
	      cout<<"generated "<<fname.str()<<"."<<endl;
	      if (anim_i >= ANIM_END) exit(2);
	    }
#endif
	  }
#ifdef REDUCE_DEBUG
	  static int iteration=0;
	  if (++iteration == 500000) exit(2);
#endif
	}
      }
    }
    /*
    // Store results on the stack
    stg.vertex.clear();
    FOR_EACH_VERTEX(v, stg.g) {
      LTLState state = stg.g[v];
      // Pop arguments
      for (int i = 0; i<nargs; ++i)
    state.valstack.pop_back();

      // Push result
      state.push(state.val);
      state.debug.push_back(state.val);

      // Does this state coincide with an existing?
      LTLStateMap::iterator i = stg.vertex.find(state);
      if (i != stg.vertex.end()) {
    // merge them
    FOR_EACH_PREDECESSOR(pred, v, stg.g)
      add_edge(pred, (*i).second, stg.g);
    END_FOR;
    FOR_EACH_SUCCESSOR(succ, v, stg.g)
      add_edge((*i).second, succ, stg.g);
    END_FOR;
    clear_vertex(v, stg.g);
    const_cast<LTLState&>((*i).first).merge_debug_info(state);
    //cerr<<"STORE: merged "<<state<<"; "<<v<<" and "<<(*i).second<<endl;
      } else {
    stg.g[v] = state;
    stg.vertex[state] = v;
      }

    } END_FOR;
    */

    // This is a purely cosmetic change, since these states are unreachable.
    FOR_EACH_VERTEX(v, stg.g)
      if (is_leaf(v, stg.g) && !(endpoints.count(v) == 0))
 	clear_vertex(v, stg.g);
    END_FOR;

    // remove orphaned vertices
    LTLWorklist orphans;
    FOR_EACH_VERTEX(v, stg.g)
      if (degree(v, stg.g) == 0) orphans.push(v);
    END_FOR;
    while (!orphans.empty()) {
      LTLVertex v = orphans.pop();
      remove_vertex(v, stg.g);
    }
  }

  /// walk through the LTL expression and calculate a new state for a given successor state
  inline LTLState transfer(const LTLState& v, const LTLState& succ, bool debug, bool endpoint) {
    LTLVisitor lv(v, succ, debug, endpoint, input_vars);
    const Expr& e = f;
    e.accept(lv);
    return lv.result;
  }

  void show_progress(const Expr& expr) {
    cout<<setw(16)<<expr.id;
  }

  ///tell if a state is an exit node
  bool isEndpoint(const EState* es) const {
    return find(endpoints.begin(), endpoints.end(), es) != endpoints.end();
  }

  struct LTLVisitor: public BottomUpVisitor {
    const LTLState& s;
    const LTLState& succ;
    const VariableIdMapping::VariableIdSet& input_vars;
    bool verbose;
    bool endpoint;
    LTLState result;
    LTLVisitor(const LTLState& _s, const LTLState& _succ,
	   bool _verbose, bool _endpoint,
	   const VariableIdMapping::VariableIdSet& _input_vars)
      : s(_s), succ(_succ),
    input_vars(_input_vars),
    verbose(_verbose), endpoint(_endpoint),
    result(_s) {}

    /// verify that two constraints are consistent, ie. not true and false
    static bool consistent(BoolLattice a, BoolLattice b) {
      if ((a.isTrue() && b.isFalse()) || (b.isTrue() && a.isFalse())) return false;
      else return true;
    }

    /// convert an integer 1..26 to an ASCII char value
    static inline char rersChar(char c) {
      return c+'A'-1;
    }

    /// return True iff that state is an Ic operation
    static BoolLattice isInputState(const EState* estate,
		    const VariableIdMapping::VariableIdSet& input_vars,
		    char c, BoolLattice succ_val) {
      if (input_vars.empty())
	return Bot();

      BoolLattice r = Bot();
      assert(estate);
      assert(estate->constraints());
      ConstraintSet constraints = *estate->constraints();
      for (VariableIdMapping::VariableIdSet::const_iterator ivar = input_vars.begin();
	ivar != input_vars.end();
	++ivar) {
	// main input variable
	BoolLattice r1 = is_eq(constraints, *ivar, c);
	assert(consistent(r, r1));
	r = r1;
      }

      if (r.isBot())
	return succ_val;
      else
	return r;
    }

    static BoolLattice is_eq(const ConstraintSet& constraints,
		   const VariableId& v,
		   char c) {
      // var == c
      ListOfAValue l = constraints.getEqVarConst(v);
      for (ListOfAValue::iterator lval = l.begin(); lval != l.end(); ++lval) {
	if (lval->isConstInt()) {
	  // A=1, B=2
	  return c == rersChar(lval->getIntValue());
	}
      }
      // var != c
      l = constraints.getNeqVarConst(v);
      for (ListOfAValue::iterator lval = l.begin(); lval != l.end(); ++lval) {
	if (lval->isConstInt()) {
	  if (c == rersChar(lval->getIntValue()))
	    return false;
	}
      }

      // In ConstIntLattice, Top means ALL values
      return Top();   // Bool Top, however, means UNKNOWN
    }


    // NOTE: This is extremely taylored to the RERS challenge benchmarks.
    void visit(const InputSymbol* expr) {
      BoolLattice succ_val = succ.valstack[expr->label];
      BoolLattice old_val  = result.valstack[expr->label];
      BoolLattice new_val = isInputState(s.estate, input_vars, expr->c, succ_val);
      if (verbose) cerr<<"  I(old="<<old_val<<", succ="<<succ_val<<") = "<<new_val<<endl;
      result.valstack[expr->label] = new_val;
    }

    /// return True iff that state is an !Ic operation
    static BoolLattice isNegInputState(const EState* estate,
		       const VariableIdMapping::VariableIdSet& input_vars,
		       char c, BoolLattice succ_val) {
      if (input_vars.empty())
	return Bot();

      BoolLattice r = Bot();

      //cerr<<"succ_val = "<<succ_val<<endl;
      assert(estate);
      assert(estate->constraints());
      ConstraintSet constraints = *estate->constraints();
      for (VariableIdMapping::VariableIdSet::const_iterator ivar = input_vars.begin();
	    ivar != input_vars.end();
	    ++ivar) {
	// This will really only work with one input variable (that one may be aliased, though)
	BoolLattice r1 = !is_eq(constraints, *ivar, c);
	//cerr<<"r = "<<r<<endl;
	//cerr<<"r1 = "<<r1<<endl;
	assert(consistent(r, r1));
	r = r1;
      }

      if (r.isBot())
 	return succ_val;
      else
	return r;
    }

    // NOTE: This is extremely taylored to the RERS challenge benchmarks.
    void visit(const NegInputSymbol* expr) {
      BoolLattice succ_val = succ.valstack[expr->label];
      BoolLattice old_val  = result.valstack[expr->label];
      BoolLattice new_val = isNegInputState(s.estate, input_vars, expr->c, succ_val);
      //assert(new_val.lub(old_val) == new_val); // only move up in the lattice!
      if (verbose) cerr<<"  ¬I(old="<<old_val<<", succ="<<succ_val<<") = "<<new_val<<endl;
      result.valstack[expr->label] = new_val;
    }

    /// return True iff that state is an Oc operation
    static BoolLattice isOutputState(const EState* estate, char c, bool endpoint,
		     BoolLattice succ_val) {
      //cerr<<estate->io.toString()<<endl;
      switch (estate->io.op) {
      case InputOutput::STDOUT_CONST: {
	const AType::ConstIntLattice& lval = estate->io.val;
	//cerr<<lval.toString()<<endl;
	assert(lval.isConstInt());
	// U=21, Z=26
	return c == rersChar(lval.getIntValue());
      }
      case InputOutput::STDOUT_VAR: {
	// output == c constraint?
	const PState& prop_state = *estate->pstate();
	//cerr<<estate->toString()<<endl;
	//cerr<<prop_state.varValueToString(estate->io.var)<<" lval="<<lval.toString()<<endl;
	if (prop_state.varIsConst(estate->io.var)) {
	  AValue aval = const_cast<PState&>(prop_state)[estate->io.var].getValue();
	  //cerr<<aval<<endl;
	  return c == rersChar(aval.getIntValue());
	}

	// Is there an output != c constraint?
	// var != c
	ListOfAValue l = estate->constraints()->getNeqVarConst(estate->io.var);
	for (ListOfAValue::iterator lval = l.begin(); lval != l.end(); ++lval) {
	  if (lval->isConstInt())
	    if (c == rersChar(lval->getIntValue()))
	      return false;
	}

      }
      default:
    return false;
	// Make sure that dead ends with no I/O show up as false
	if (endpoint)
	  return false;

	return succ_val;
      }
    }

    /**
     */
    void visit(const OutputSymbol* expr) {
      BoolLattice succ_val = succ.valstack[expr->label];
      BoolLattice old_val  = result.valstack[expr->label];
      BoolLattice new_val  = isOutputState(s.estate, expr->c, endpoint, succ_val);
      result.valstack[expr->label] = new_val;
      //cerr<<"  O(old="<<old_val<<", succ="<<succ_val<<") = "<<new_val<<endl;

    }

    /**
     * Negated version of OutputSymbol
     *
     */
    void visit(const NegOutputSymbol* expr) {
      BoolLattice succ_val = succ.valstack[expr->label];
      BoolLattice old_val  = result.valstack[expr->label];
      BoolLattice new_val  = !isOutputState(s.estate, expr->c, endpoint, succ_val);
      if (verbose) cerr<<"  ¬O(old="<<old_val<<", succ="<<succ_val<<") = "<<new_val<<endl;
      result.valstack[expr->label] = new_val;
    }

    /**
     * NOT
     */
    void visit(const Not* expr) {
      BoolLattice succ_val = succ.valstack[expr->label];
      BoolLattice old_val  = result.valstack[expr->label];
      BoolLattice e1       = result.valstack[expr->expr1->label];
      BoolLattice new_val  = /*old_val.lub(*/!e1/*)*/;
      if (verbose) cerr<<"  G(old="<<old_val<<", e1="<<e1<<", succ="<<succ_val<<") = "<<new_val<<endl;
      result.valstack[expr->label] = new_val;
    }

    /**
     * X φ (next): φ has to hold after the next step
     *
     */
    void visit(const Next* expr) {
      BoolLattice succ_val = succ.valstack[expr->label];
      BoolLattice old_val  = result.valstack[expr->label];
      BoolLattice e1       = result.valstack[expr->expr1->label];
      BoolLattice new_val  = old_val.lub(succ_val);
      if (verbose) cerr<<"  X(old="<<old_val<<", e1="<<e1<<", succ="<<succ_val<<") = "<<new_val<<endl;
      result.valstack[expr->label] = new_val;
    }

    /**
     * F φ (eventually): φ has to hold at some point in the future (or now)
     *
     */
    void visit(const Eventually* expr) {
      BoolLattice succ_val = succ.valstack[expr->label];
      BoolLattice old_val  = result.valstack[expr->label];
      BoolLattice e1       = result.valstack[expr->expr1->label];
      BoolLattice new_val  = /*old_val ||*/ e1 || succ_val;
      if (verbose) cerr<<"  F(old="<<old_val<<", e1="<<e1<<", succ="<<succ_val<<") = "<<new_val<<endl;
      result.valstack[expr->label] = new_val;
    }

    /**
     * G φ (globally): φ has to hold always (including now)
     *
     * I'm interpreting this as all states following the current one,
     * ignoring the past.
     *
     * True, iff for each state we have TRUE
     */

    void visit(const Globally* expr) {
      BoolLattice succ_val = succ.valstack[expr->label];
      BoolLattice old_val  = result.valstack[expr->label];
      BoolLattice e1       = result.valstack[expr->expr1->label];
      // TODO: I'm not sure about the correct way to combine old_val with the new one
      // And my current intuition is that it is safe to ignore it, since it
      // will be propagated back to this node, if we have a loop, anyway.
      BoolLattice new_val = /*old_val &&*/ e1 && succ_val;
      if (verbose) cerr<<"  G(old="<<old_val<<", e1="<<e1<<", succ="<<succ_val<<") = "<<new_val<<endl;
      result.valstack[expr->label] = new_val;
    }

    void visit(const And* expr) {
      BoolLattice succ_val = succ.valstack[expr->label];
      BoolLattice old_val  = result.valstack[expr->label];
      BoolLattice e1       = result.valstack[expr->expr1->label];
      BoolLattice e2       = result.valstack[expr->expr2->label];
      BoolLattice new_val = /*old_val &&*/ e1 && e2;
      if (verbose) cerr<<"  And(old="<<old_val
		  <<", e1="<<e1<<", e2="<<e1
		  <<", succ="<<succ_val<<") = "<<new_val<<endl;
      result.valstack[expr->label] = new_val;
    }

    // Implementation status: DONE
    void visit(const Or* expr) {
      BoolLattice succ_val = succ.valstack[expr->label];
      BoolLattice old_val  = result.valstack[expr->label];
      BoolLattice e1       = result.valstack[expr->expr1->label];
      BoolLattice e2       = result.valstack[expr->expr2->label];
      BoolLattice new_val = /*old_val ||*/ e1 || e2;
      if (verbose) cerr<<"  Or(old="<<old_val
		<<", e1="<<e1<<", e2="<<e1
		<<", succ="<<succ_val<<") = "<<new_val<<endl;
      result.valstack[expr->label] = new_val;
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
    void visit(const Until* expr) {
      BoolLattice succ_val = succ.valstack[expr->label];
      BoolLattice old_val  = result.valstack[expr->label];
      BoolLattice e1       = result.valstack[expr->expr1->label];
      BoolLattice e2       = result.valstack[expr->expr2->label];
      BoolLattice new_val = /*old_val &&*/ (e2 || (e1 && succ_val));
      if (verbose) cerr<<"  Until(old="<<old_val
		<<", e1="<<e1<<", e2="<<e1
		<<", succ="<<succ_val<<") = "<<new_val<<endl;
      result.valstack[expr->label] = new_val;
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
     * According to the unwinding properties, R seems to be the dual operator of U.
     */
    void visit(const Release* expr) {
      BoolLattice succ_val = succ.valstack[expr->label];
      BoolLattice old_val  = result.valstack[expr->label];
      BoolLattice e1       = result.valstack[expr->expr1->label];
      BoolLattice e2       = result.valstack[expr->expr2->label];
      BoolLattice new_val = /*old_val ||*/ (e2 && (e1 || succ_val));
      if (verbose) cerr<<"  Release(old="<<old_val
		<<", e1="<<e1<<", e2="<<e1
		<<", succ="<<succ_val<<") = "<<new_val<<endl;
      result.valstack[expr->label] = new_val;
    };
  };
};


UChecker::UChecker(EStateSet& ess, TransitionGraph& _tg)
  : transitionGraph(_tg),
    eStateSet(ess)
{
  // Build our own customized Transition graph

  cerr<<"Building boost state transition graph... "<<flush;
  int i = 0;
  boost::unordered_map<const EState*, Label> estate_label;
  FOR_EACH_ESTATE(state, l1) {
    estate_label[&(*state)] = i++;
  }
  //cerr<<" finished labeling "<<flush;

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
  cerr<<"done"<<endl;
  //start = estate_label[transitionGraph.begin()->source];
  Transition st = transitionGraph.getStartTransition();
  start = estate_label[st.source];

  // Optimization
  if(option_debug_mode==200) {
    cout << "DEBUG: START"<<(*transitionGraph.begin()).source
     <<", news: "<<transitionGraph.getStartTransition().source
     <<", newt: "<<transitionGraph.getStartTransition().target
     <<endl;
  }

  if(boolOptions["post-collapse-stg"]) {
    // Optimization
    start = collapse_transition_graph(full_graph, g);

  } else {
    g = full_graph;
  }

  if(option_debug_mode==201) {
    FOR_EACH_STATE(state, label) {
      cerr<<"DEBUG: "<<label<<": "<<state->toString()<<endl;
    }
  }
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
  vector<Label> renumbered(num_vertices(g));

  FOR_EACH_STATE(state, label) {
    //cerr<<label<<endl;
    assert(g[label]);
    if (( in_degree(label, g) >= 1) && // keep start
	(out_degree(label, g) >= 0) && // DO NOT keep exits
	(g[label]->io.op == InputOutput::NONE ||
	 g[label]->io.op == InputOutput::FAILED_ASSERT)) {
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
    add_edge(renumbered[src], renumbered[tgt], reduced);
    //cerr<<renumbered[src]<<" -> "<<renumbered[tgt]<<";"<<endl;
    reduced[renumbered[src]] = g[src];
    reduced[renumbered[tgt]] = g[tgt];
  }
  //cerr<<"}"<<endl;

  //cerr<<"## done "<<endl<<endl;
  cerr<<"Number of EStates: "<<num_vertices(g)<<endl;
  cerr<<"Number of LTLStates: "<<num_vertices(reduced)<<endl;

  return renumbered[start];
}


BoolLattice
UChecker::verify(const Formula& f)
{
  // Verify!
  const Expr& e = f;
  UVerifier v(eStateSet, g, start, num_vertices(g), f);
  v.analyze(boolOptions["ltl-verbose"]);

  // Visualization:
  bool ltl_output_dot = boolOptions["ltl-output-dot"];//  true;
  //bool show_node_detail = boolOptions["ltl-show-node-detail"];//  true;
  //bool collapsed_graph = boolOptions["ltl-collapsed-graph"];//  false

  if (ltl_output_dot) {
    ofstream myfile;
    stringstream fname;
    static int n = 1;
    fname << "ltl_output_" << n++ << ".dot";
    myfile.open(fname.str().c_str(), ios::out);
    myfile << visualize(v.stg, e);
    myfile.close();
    cout<<"generated "<<fname.str()<<"."<<endl;
  }

  // Find the disjunction of all start states; we are looking for
  // all-quantified LTL formulae only
  BoolLattice b = Bot();
  FOR_EACH_VERTEX(lv, v.stg.g) {
    LTLState s = v.stg.g[lv];
    if (in_degree(lv, v.stg.g) == 0) {
      //cerr<<"Value at START = "<<s.top()<<endl;
      b = b && s.top();
    }
  } END_FOR
  cerr<<"Number of LTL states: "<<num_vertices(v.stg.g)<<endl;
  return b;
}
