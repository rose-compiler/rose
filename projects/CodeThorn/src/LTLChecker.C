#include "LTLChecker.h"
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

//  Written 2012 by Adrian Prantl <adrian@llnl.gov>.

using namespace LTL;
using namespace boost;

enum e_BoolLattice { 
  bot   = -1,
  FALSE =  0, 
  TRUE  =  1 
};
typedef enum e_BoolLattice BoolLattice;
typedef vector< map<const LTL::Expr*, int> > LTLProperties;
typedef adjacency_list<vecS, vecS, bidirectionalS, const EState*> BoostTransitionGraph;
typedef graph_traits<BoostTransitionGraph> GraphTraits;

#define FOR_EACH_TRANSITION(TRANSITION)				     \
  for (TransitionGraph::const_iterator TRANSITION=transitionGraph.begin(); \
       TRANSITION != transitionGraph.end();			     \
       ++TRANSITION)						     

#define FOR_EACH_STATE(STATE, LABEL)			             \
  Label LABEL=0;                                                     \
  for (EStateSet::const_iterator STATE=eStateSet.begin();     	     \
       STATE != eStateSet.end();				     \
       ++STATE, ++LABEL)						     
#define props ltl_properties[label]


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
    s<<node<<" -> "<<get(a)->dot_label<<" [color=green];\n  "; 
    return node;
  }

  IAttr visit(const InputSymbol* e, IAttr a)  {
    int node = newNode(a);
    s<<node<<" [label=\""<<props[e]<<"\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(const OutputSymbol* e, IAttr a) {
    int node = newNode(a);
    s<<node<<" [label=\""<<props[e]<<"\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(const Not* e, IAttr a) {
    int node = newNode(a);
    s<<node<<" [label=\""<<props[e]<<" = "<<"!"<<props[e->expr]<<"\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(const Next* e, IAttr a) {
    int node = newNode(a);
    s<<node<<" [label=\""<<"("<<props[e]<<" = "<<"X"<<props[e->expr]<<")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(const Eventually* e, IAttr a) {
    int node = newNode(a);
    s<<node<<" [label=\""<<"("<<props[e]<<" = "<<"F"<<props[e->expr]<<")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(const Globally* e, IAttr a) {
    int node = newNode(a);
    s<<node<<" [label=\""<<"("<<props[e]<<" = "<<"G"<<props[e->expr]<<")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(const And* e, IAttr a) {
    int node = newNode(a);
    s<<node<<" [label=\""<<"("<<props[e]<<"="<<props[e->expr1]<<" & "<<props[e->expr2]<<")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(const Or* e, IAttr a) {
    int node = newNode(a);
    s<<node<<" [label=\""<<"("<<props[e]<<"="<<props[e->expr1]<<" | "<<props[e->expr2]<<")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(const Until* e, IAttr a)	{
    int node = newNode(a);
    s<<node<<" [label=\""<<"("<<props[e]<<"="<<props[e->expr1]<<" U "<<props[e->expr2]<<")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(const WeakUntil* e, IAttr a) {
    int node = newNode(a);
    s<<node<<" [label=\""<<"("<<props[e]<<"="<<props[e->expr1]<<" WU "<<props[e->expr2]<<")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(const Release* e, IAttr a) {
    int node = newNode(a);
    s<<node<<" [label=\""<<"("<<props[e]<<"="<<props[e->expr1]<<" R "<<props[e->expr2]<<")\"];\n  ";
    return newAttr(node);
  }
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
class Verifier: public BottomUpVisitor {
  EStateSet& eStateSet;
  BoostTransitionGraph& g;
  Label start;
public:
  LTLProperties ltl_properties;

  Verifier(EStateSet& ess, BoostTransitionGraph& btg, Label start_label, int max_label) 
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

  /**
   * perform a backward-directed fixpoint iteration over all states
   *
   * since our programs do not necessarily have exit nodes, we start
   * the working set with all nodes where START == true.
   *
   * FIXME: rewrite this as a template
   */
# define bw_fixpoint(INIT, START, JOIN, CALC) {				\
    stack<Label> workset;						\
    FOR_EACH_STATE(state, label) {					\
      props[e] = INIT;							\
      if (START) workset.push(label);					\
    }									\
    									\
    while (!workset.empty()) {						\
      Label label = workset.top(); workset.pop();			\
      /*cerr<<"Visiting state "<<label<<endl;*/				\
      const EState* state = g[label];					\
      assert(state);							\
      									\
      /* Merge result of incoming edges */				\
      int merged_succs = TRUE;						\
      bool has_succs = false;						\
      /* for each successor */						\
      GraphTraits::out_edge_iterator out_i, out_end;			\
      for (tie(out_i, out_end) = out_edges(label, g); out_i != out_end; ++out_i) { \
	Label succ = target(*out_i, g);					\
	int succ_prop = ltl_properties[succ][e];			\
	/*cerr<<"  succ: "<<succ<<" = "<<succ_prop<<endl;*/			\
									\
	merged_succs = merged_succs JOIN succ_prop;			\
	has_succs = true;						\
      }									\
      if (!has_succs)							\
	merged_succs = FALSE;						\
      									\
      /* Calculate property for this node */				\
      /*assert(props.find(e->expr) != props.end());*/			\
      int old_val = props[e];						\
      									\
      props[e] = CALC;							\
      									\
      /*cerr<<"  "<<label<<" <- "<<props[e]<<" was: "<<old_val<<endl;*/	\
      bool no_fixpoint = (old_val == bot || old_val != props[e]);	\
      if (no_fixpoint) {						\
	/*cerr<<"NO FIX!"<<endl;*/						\
	/* for each predecessor */					\
	GraphTraits::in_edge_iterator in_i, in_end;			\
	for (tie(in_i, in_end) = in_edges(label, g); in_i != in_end; ++in_i) { \
	  Label pred = source(*in_i, g);				\
	  /*cerr<<"  pred: "<<pred<<endl;*/					\
	  workset.push(pred);						\
	}								\
      } else {								\
	/*cerr<<"FIX!"<<endl;*/						\
      }									\
    }									\
  }



  // Implementation status: TODO
  void visit(const InputSymbol* e) {
    FOR_EACH_STATE(state, label) {
      char c;
      switch (state->io.op) {
      case InputOutput::IN_VAR: {
	//record input_vars = 
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

  // Implementation status: DONE
  void visit(const OutputSymbol* e) {
    FOR_EACH_STATE(state, label) {
      char c;
      switch (state->io.op) {
      case InputOutput::OUT_CONST: {
	const AType::ConstIntLattice& lval = state->io.val;
	cerr<<lval.toString()<<endl;
	assert(lval.isConstInt());
        c = lval.getIntValue()+'A';
	break;
      }
      case InputOutput::OUT_VAR: {
      	//const AType::ConstIntLattice& lval = 
      	//  state->constraints.varConstIntLatticeValue(state->io.var);
	const State& prop_state = *state->state;
	assert(prop_state.varIsConst(state->io.var));
	AValue aval = const_cast<State&>(prop_state)[state->io.var];
	cerr<<aval<<endl;
      	c = aval+'A';
      	break;
      }
      default:
	c = EOF;
      }
      if (c != EOF) cerr<<"output: "<<c<< endl;
      props[e] = (c == e->c);
    }
  }

  // Implementation status: DONE
  void visit(const Not* e) {
    FOR_EACH_STATE(state, label) 
      props[e] = !props[e->expr];
  }

  // Implementation status: TODO
  void visit(const Next* e) { assert(false); }

  // Implementation status: TODO
  void visit(const Eventually* e) {
    // I'm interpreting Eventually to be a backward problem
    //  
    //  a     if e(b) then F e(a) and F e(b) but not F e(c)
    //  |\
    //  b c
    //
    // propagate the information that the event occured up each path
    bw_fixpoint(bot,                           // init
		props[e->expr] /*== true*/,    // start
		&&,                            // join
		props[e->expr] || merged_succs // calc
		);
    FOR_EACH_STATE(state, label) 
      if (props[e] == bot) props[e] = false;
  }

  // Implementation status: DONE
  void visit(const Globally* e) {
    int global = TRUE;
    FOR_EACH_STATE(state, label) 
      global = global && props[e->expr];

    { // propagate the global result to all states
      FOR_EACH_STATE(state, label) 
	props[e] = global; 
    }
  }

  // Implementation status: DONE
  void visit(const And* e) {
    FOR_EACH_STATE(state, label)
      props[e] = props[e->expr1] && props[e->expr2];
  }

  // Implementation status: DONE
  void visit(const Or* e) {
    FOR_EACH_STATE(state, label)
      props[e] = props[e->expr1] || props[e->expr2];
  }

  // Implementation status: TODO
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

  // Implementation status: TODO
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

  // Implementation status: TODO
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
  FOR_EACH_STATE(state, l1) {
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
  
  Verifier v(eStateSet, g, estate_label[transitionGraph.begin()->source], N);
  const Expr& e = f;
  e.accept(v);

  // generate dot output for debugging
  stringstream s;
  s<<"digraph G {\n";
  FOR_EACH_TRANSITION(t) {
    s<<"node[shape=rectangle, color=gray, style=filled];\n  ";
    s<<estate_label[t->source]<<" -> "<<estate_label[t->target]<<";\n";
  }
  FOR_EACH_STATE(state, l) {
    Visualizer viz(v.ltl_properties, l);
    e.accept(viz, Visualizer::newAttr(l));
    s<<l<<" [label=\""<<l<<":"<< state->toString() <<"\"] ;\n";
    s<<"subgraph ltl_"<<l<<" {\n";
    s<<"  node[shape=rectangle, color=green, style=filled];\n  ";
    s<<viz.s.str();
    s<<"}\n";
  }
  s<<"}\n";

  std::ofstream myfile;
  myfile.open("ltl_output.dot", std::ios::out);
  myfile << s.str();
  myfile.close();
  cout<<"generated ltl_output.dot."<<endl;

  // use result at start node as return value, 
  // I hope this is always correct
  FOR_EACH_STATE(state, label) 
    return v.props[&e];
}

