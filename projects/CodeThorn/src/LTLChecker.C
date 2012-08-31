#include "LTLChecker.h"
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

using namespace ltl;
using namespace boost;

enum e_BoolLattice { 
  bot   = -1,
  FALSE =  0, 
  TRUE  =  1 
};
typedef enum e_BoolLattice BoolLattice;
typedef vector< map<const ltl::Expr*, int> > LTLProperties;
typedef adjacency_list<vecS, vecS, bidirectionalS, const EState*> BoostTransitionGraph;
typedef graph_traits<BoostTransitionGraph> GraphTraits;

#define FOR_EACH_TRANSITION(TRANSITION)				     \
  for (TransitionGraph::iterator TRANSITION=transitionGraph.begin(); \
       TRANSITION != transitionGraph.end();			     \
       ++TRANSITION)						     

#define FOR_EACH_STATE(STATE, LABEL)			             \
  Label LABEL=0;                                                     \
  for (EStateSet::iterator STATE=eStateSet.begin();     	     \
       STATE != eStateSet.end();				     \
       ++STATE, ++LABEL)						     
#define props ltl_properties[label]

class Visualizer: public BottomUpVisitor {
public: 
  Visualizer(LTLProperties& p, Label state)
    : ltl_properties(p), label(state) {}
  LTLProperties& ltl_properties;
  Label label;
  stringstream s;

  void visit(const InputSymbol* e)  {s<<props[e];}
  void visit(const OutputSymbol* e) {s<<props[e];}
  void visit(const Not* e)	    {s<<"("<<props[e]<<" = "<<"!"<<props[e->expr]<<")";}
  void visit(const Next* e)	    {s<<"("<<props[e]<<" = "<<"X"<<props[e->expr]<<")";}
  void visit(const Eventually* e)   {s<<"("<<props[e]<<" = "<<"F"<<props[e->expr]<<")";}
  void visit(const Globally* e)	    {s<<"("<<props[e]<<" = "<<"G"<<props[e->expr]<<")";}
  void visit(const And* e)      {s<<"("<<props[e]<<"="<<props[e->expr1]<<" & "<<props[e->expr2]<<")";}
  void visit(const Or* e)	{s<<"("<<props[e]<<"="<<props[e->expr1]<<" | "<<props[e->expr2]<<")";}
  void visit(const Until* e)	{s<<"("<<props[e]<<"="<<props[e->expr1]<<" U "<<props[e->expr2]<<")";}
  void visit(const WeakUntil* e){s<<"("<<props[e]<<"="<<props[e->expr1]<<" WU "<<props[e->expr2]<<")";}
  void visit(const Release* e)	{s<<"("<<props[e]<<"="<<props[e->expr1]<<" R "<<props[e->expr2]<<")";}
};

Checker::Checker(EStateSet& ess, TransitionGraph& g)
  : transitionGraph(g),
    eStateSet(ess)
{
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
 * TODO: Are there better anmes for these classes?
 */
class Verifyer: public BottomUpVisitor {
  EStateSet& eStateSet;
  BoostTransitionGraph& g;
  Label start;
public:
  LTLProperties ltl_properties;

  Verifyer(EStateSet& ess, BoostTransitionGraph& btg, Label start_label, int max_label) 
    : eStateSet(ess), g(btg), start(start_label) {
    // reserve a result map for each label
    // it maps an analysis result to each sub-expression of the ltl formula
    ltl_properties.resize(max_label);
    
  }

  /**
   * perform a forward-directed fixpoint iteration over all states
   *
   * FIXME: rewrite this as a template
   */
# define fixpoint(INIT, JOIN, CALC) {						      \
    FOR_EACH_STATE(state, label)						      \
      props[e] = INIT;								      \
										      \
    stack<Label> workset;							      \
    workset.push(start);							      \
										      \
    while (!workset.empty()) {							      \
      Label label = workset.top(); workset.pop();				      \
      /*cerr<<"Visiting state "<<label<<endl;*/					      \
      const EState* state = g[label];						      \
      assert(state);								      \
										      \
      /* Merge result of incoming edges */					      \
      int merged_preds = TRUE;							      \
      bool has_preds = false;							      \
      /* for each predecessor */						      \
      GraphTraits::in_edge_iterator in_i, in_end;				      \
      for (tie(in_i, in_end) = in_edges(label, g); in_i != in_end; ++in_i) {	      \
	Label pred = source(*in_i, g);						      \
	int pred_prop = ltl_properties[pred][e];				      \
	/*cerr<<"  pred: "<<pred<<" = "<<pred_prop<<endl;*/			      \
										      \
	merged_preds = merged_preds JOIN pred_prop;				      \
	has_preds = true;							      \
      }										      \
      if (!has_preds)								      \
	merged_preds = FALSE;							      \
										      \
      /* Calculate property for this node */					      \
      /*assert(props.find(e->expr) != props.end());*/			              \
      int old_val = props[e];							      \
      CALC;								              \
      /* cerr<<"  "<<label<<" <- "<<props[e]<<endl; */				      \
      bool no_fixpoint = (old_val == INIT || old_val != props[e]);		      \
      if (no_fixpoint) {							      \
	/* for each successor */						      \
	GraphTraits::out_edge_iterator out_i, out_end;				      \
	for (tie(out_i, out_end) = out_edges(label, g); out_i != out_end; ++out_i) {  \
	  Label succ = target(*out_i, g);					      \
	  /*cerr<<"  succ: "<<succ<<endl;*/					      \
	  workset.push(succ);							      \
	}									      \
      } else {									      \
	/*cerr<<"FIX!"<<endl;*/							      \
      }										      \
    }										      \
  }


  void visit(const InputSymbol* e) {
    FOR_EACH_STATE(state, label) {
      char c;
      switch (state->io.op) {
      case InputOutput::IN_VAR: {
	const AType::ConstIntLattice& lval = 
	  state->constraints.varConstIntLatticeValue(state->io.var);
	assert(lval.isConstInt());
	c = lval.getIntValue()+'A';
	// FIXME: this is WRONG. We should re-run the contraint propagation with that value!?
	break;
      }
      default:
	c = EOF;
      }
      if (c != EOF) cerr<<"input: "<<c<< endl;
      props[e] = (c == e->c);
    }
  }

  void visit(const OutputSymbol* e) {
    FOR_EACH_STATE(state, label) {
      char c;
      switch (state->io.op) {
      case InputOutput::OUT_CONST: {
	const AType::ConstIntLattice& lval = state->io.val;
	assert(lval.isConstInt());
        c = lval.getIntValue()+'A';
	break;
      }
      case InputOutput::OUT_VAR: {
	const AType::ConstIntLattice& lval = 
	  state->constraints.varConstIntLatticeValue(state->io.var);
	assert(lval.isConstInt());
	c = lval.getIntValue()+'A';
	break;
      }
      default:
	c = EOF;
      }
      if (c != EOF) cerr<<"output: "<<c<< endl;
      props[e] = (c == e->c);
    }
  }

  void visit(const Not* e) {
    FOR_EACH_STATE(state, label) 
      props[e] = !props[e->expr];
  }

  void visit(const Next* e) { assert(false); }
  void visit(const Eventually* e) {
    fixpoint(bot, // init
	     &&,  // join
	     props[e] = props[e->expr] || merged_preds // calc
	     );
    cerr<<"FIXME: fetch the result from the leaf nodes and store it"<< endl;
  }
  void visit(const Globally* e) {
    int global = TRUE;
    FOR_EACH_STATE(state, label) 
      global = global && props[e->expr];

    { // propagate the global result to all states
      FOR_EACH_STATE(state, label) 
	props[e] = global; 
    }
  }

  void visit(const And* e) {
    FOR_EACH_STATE(state, label)
      props[e] = props[e->expr1] && props[e->expr2];
  }

  void visit(const Or* e) {
    FOR_EACH_STATE(state, label)
      props[e] = props[e->expr1] || props[e->expr2];
  }

  void visit(const Until* e) {
    // A holds until B occurs
    //
    // Caveat: Although the LTL semantics say so, we can't start on
    // the start node. Maybe at the first I/O node? 
    //
    // Think about ``oA U oB''.

    // First find out if there exists an uninterrupted chain of As
    fixpoint(bot, // init
	     &&,  // join
	     props[e] = props[e->expr1] || merged_preds // calc
	     );

    // FIXME: this is not yet correct!
    FOR_EACH_STATE(state, label)
      props[e] = props[e] ^ props[e->expr2];
  }
  void visit(const WeakUntil* e) {
    // A holds until B occurs, but B may never occur
    //
    // Caveat: see Until.

    // First find out if there exists an uninterrupted chain of As
    fixpoint(bot, // init
	     &&,  // join
	     props[e] = props[e->expr1] || merged_preds // calc
	     );

    // FIXME: this is not yet correct either!
    FOR_EACH_STATE(state, label)
      props[e] = props[e] || (!props[e] && props[e->expr2]);
  }
  void visit(const Release* e) {
    // If !B occurs, A happens before it.
    //
    // Caveat: see Until.
    FOR_EACH_STATE(state, label)
      props[e] = bot;
    // TODO
  }


  //void visit(const Next* e)	    { assert(false&&"not implemented"); }
  //void visit(const Eventually* e) { /*witness[e] = 0;*/ }
  //void visit(const Until* e)	    { /*until_occurred[e] = false;*/ }
  //void visit(const WeakUntil* e)  { /*until_occurred[e] = false;*/ }
  //void visit(const Release* e)    { /*released[e] = false;*/ }
};




bool
Checker::verify(const Formula& f)
{
  // Build our own customized Transition graph
  int N = eStateSet.size();
  int i = 0;
  map<const EState*, Label> estate_label;
  FOR_EACH_STATE(state, label) {
    estate_label[&(*state)] = i++;
  }

  BoostTransitionGraph g(N);
  FOR_EACH_TRANSITION(t) {
    Label src = estate_label[&(*t->source)];
    Label tgt = estate_label[&(*t->target)];
    add_edge(src, tgt, g);
    g[src] = t->source;
    g[tgt] = t->target;
  }
  
  Verifyer v(eStateSet, g, estate_label[transitionGraph.begin()->source], N);
  const Expr& e = f;
  e.accept(v);

  // generate dot output for debugging
  cout<<"generating visualization..."<<endl;
  stringstream s;
  s<<"digraph G {\n";
  FOR_EACH_TRANSITION(t) {
    s<<estate_label[t->source]<<" -> "<<estate_label[t->target]<<";\n";
  }
  FOR_EACH_STATE(state, l) {
    Visualizer viz(v.ltl_properties, l);
    e.accept(viz);
    s<<l<<" [label=\""<<l<<":"<< state->toString() <<"\"] ;\n";
    s<<l<<" -> ltl_"<< l <<";\n";
    s<<"ltl_"<<l<<" [label=\""<< viz.s.str() <<"\", shape=rectangle, color=yellow, style=filled] ;\n";
  }
  s<<"}\n";

  std::ofstream myfile;
  myfile.open("ltl_output.dot", std::ios::out);
  myfile << s.str();
  myfile.close();

}

