#include "LTLChecker.h"
#include "AType.h"
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

//  Written 2012 by Adrian Prantl <adrian@llnl.gov>.

using namespace LTL;
using namespace boost;
using namespace std;
using namespace AType;

typedef vector< map<const Expr*, BoolLattice> > LTLProperties;
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
  static string color(BoolLattice green) {
    return green.isTrue() ? "color=green" : "color=red";
  }

  IAttr visit(const InputSymbol* e, IAttr a)  {
    int node = newNode(a);
    s<<node<<" ["<<color(props[e])<<",label=\"Input "<<string(1, e->c)
     <<" = "<<props[e]<<"\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(const OutputSymbol* e, IAttr a) {
    int node = newNode(a);
    s<<node<<" ["<<color(props[e])<<",label=\"Output "<<string(1, e->c)
     <<" = "<<props[e]<<"\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(const Not* e, IAttr a) {
    int node = newNode(a);
    s<<node<<" ["<<color(props[e])<<",label=\""<<props[e]
     <<" = "<<"!"<<props[e->expr]<<"\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(const Next* e, IAttr a) {
    int node = newNode(a);
    s<<node<<" ["<<color(props[e])<<",label=\""<<"("<<props[e]
     <<" = "<<"X "<<props[e->expr]<<")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(const Eventually* e, IAttr a) {
    int node = newNode(a);
    s<<node<<" ["<<color(props[e])<<",label=\""<<"("<<props[e]
     <<" = "<<"F "<<props[e->expr]<<")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(const Globally* e, IAttr a) {
    int node = newNode(a);
    s<<node<<" ["<<color(props[e])<<",label=\""<<"("<<props[e]
     <<" = "<<"G "<<props[e->expr]<<")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(const And* e, IAttr a) {
    int node = newNode(a);
    s<<node<<" ["<<color(props[e])<<",label=\""<<"("<<props[e]
     <<" = "<<props[e->expr1]<<" & "<<props[e->expr2]<<")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(const Or* e, IAttr a) {
    int node = newNode(a);
    s<<node<<" ["<<color(props[e])<<",label=\""<<"("<<props[e]
     <<" = "<<props[e->expr1]<<" | "<<props[e->expr2]<<")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(const Until* e, IAttr a)	{
    int node = newNode(a);
    s<<node<<" ["<<color(props[e])<<",label=\""<<"("<<props[e]
     <<" = "<<props[e->expr1]<<" U "<<props[e->expr2]<<")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(const WeakUntil* e, IAttr a) {
    int node = newNode(a);
    s<<node<<" ["<<color(props[e])<<",label=\""<<"("<<props[e]
     <<" = "<<props[e->expr1]<<" WU "<<props[e->expr2]<<")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(const Release* e, IAttr a) {
    int node = newNode(a);
    s<<node<<" ["<<color(props[e])<<",label=\""<<"("<<props[e]
     <<" = "<<props[e->expr1]<<" R "<<props[e->expr2]<<")\"];\n  ";
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
 * JOINS should have the same information
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
# define fixpoint(INIT, JOIN, CALC) {					\
    FOR_EACH_STATE(state, label)					\
      props[e] = INIT;							\
    									\
    stack<Label> workset;						\
    workset.push(start);						\
    									\
    while (!workset.empty()) {						\
      Label label = workset.top(); workset.pop();			\
      /*cerr<<"Visiting state "<<label<<endl;*/				\
      const EState* state = g[label];					\
      assert(state);							\
      									\
      /* Merge result of incoming edges */				\
      BoolLattice joined_preds = true;					\
      bool has_preds = false;						\
      /* for each predecessor */					\
      GraphTraits::in_edge_iterator in_i, in_end;			\
      for (tie(in_i, in_end) = in_edges(label, g); in_i != in_end; ++in_i) { \
	Label pred = source(*in_i, g);					\
	BoolLattice pred_prop = ltl_properties[pred][e];		\
	/*cerr<<"  pred: "<<pred<<" = "<<pred_prop<<endl;*/		\
									\
	joined_preds = joined_preds JOIN pred_prop;			\
	has_preds = true;						\
      }									\
      if (!has_preds) joined_preds = false;				\
      									\
      /* Calculate property for this node */				\
      /*assert(props.find(e->expr) != props.end());*/			\
      BoolLattice old_val = props[e];					\
      props[e] = old_val JOIN ( CALC );					\
      /* cerr<<"  "<<label<<" <- "<<props[e]<<endl; */			\
      bool no_fixpoint = (old_val == INIT || old_val != props[e]);	\
      if (no_fixpoint) {						\
	/* for each successor */					\
	GraphTraits::out_edge_iterator out_i, out_end;			\
	for (tie(out_i, out_end) = out_edges(label, g); out_i != out_end; ++out_i) { \
	  Label succ = target(*out_i, g);				\
	  /*cerr<<"  succ: "<<succ<<endl;*/				\
	  workset.push(succ);						\
	}								\
      } else {								\
	/*cerr<<"FIX!"<<endl;*/						\
      }									\
    }									\
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
      BoolLattice joined_succs = true;					\
      bool has_succs = false;						\
      /* for each successor */						\
      GraphTraits::out_edge_iterator out_i, out_end;			\
      for (tie(out_i, out_end) = out_edges(label, g); out_i != out_end; ++out_i) { \
	Label succ = target(*out_i, g);					\
	BoolLattice succ_prop = ltl_properties[succ][e];		\
	/*cerr<<"  succ: "<<succ<<" = "<<succ_prop<<endl;*/		\
									\
	joined_succs = joined_succs JOIN succ_prop;			\
	has_succs = true;						\
      }									\
      if (!has_succs)							\
	joined_succs = false;						\
      									\
      /* Calculate property for this node */				\
      /*assert(props.find(e->expr) != props.end());*/			\
      BoolLattice old_val = props[e];					\
      									\
      props[e] = old_val JOIN ( CALC );					\
      									\
      /*cerr<<"  "<<label<<" <- "<<props[e]<<" was: "<<old_val<<endl;*/	\
      bool no_fixpoint = (old_val.isBot() || old_val != props[e]);	\
      if (no_fixpoint) {						\
	/*cerr<<"NO FIX!"<<endl;*/					\
	/* for each predecessor */					\
	GraphTraits::in_edge_iterator in_i, in_end;			\
	for (tie(in_i, in_end) = in_edges(label, g); in_i != in_end; ++in_i) { \
	  Label pred = source(*in_i, g);				\
	  /*cerr<<"  pred: "<<pred<<endl;*/				\
	  workset.push(pred);						\
	}								\
      } else {								\
	/*cerr<<"FIX!"<<endl;*/						\
      }									\
    }									\
  }


  static void updateInputVar(const EState* estate, const VariableId** v) {
    assert(v);
    if (estate->io.op == InputOutput::IN_VAR) {
      *v = &estate->io.var;
    }
  }

  /// return True iff that state is an Oc operation
  static BoolLattice isInputState(const EState* estate, const VariableId** v, 
				  int c, BoolLattice joined_preds) {
    assert(v);
    updateInputVar(estate, v);
    if (*v == NULL) 
      return false;
    assert(*v);

    const ConstIntLattice& lval = 
      estate->constraints.varConstIntLatticeValue(**v);
    //cerr<<endl<<"ivar == "<<(*v)->variableName()<<endl;
    //cerr<<estate->constraints.toString()<<endl;
    //cerr<<lval.toString()<<endl;
    if (lval.isConstInt()) {
      //cerr<<(bool)(c == lval.getIntValue()+'A')<<endl;
      return c == lval.getIntValue()+'A';
    }
    else
      return BoolLattice(Top()) || joined_preds;
  }

  // Implementation status: IN PROGRESS
  // NOTE: Assumes there is only one input variable
  void visit(const InputSymbol* e) {
    const VariableId* input_var = NULL;

    fixpoint(Bot(),                                              // init
	     &&,                                                 // join
	     isInputState(state, &input_var, e->c, joined_preds) // calc
	     );


    //FOR_EACH_STATE(state, label) 
    //  if (props[e].isBot()) props[e] = false;
  }

  /// return True iff that state is an Oc operation
  static BoolLattice isOutputState(const EState* estate, int c) {
    switch (estate->io.op) {
    case InputOutput::OUT_CONST: {
      const ConstIntLattice& lval = estate->io.val;
      cerr<<lval.toString()<<endl;
      assert(lval.isConstInt());
      return c == lval.getIntValue()+'A';
    }
    case InputOutput::OUT_VAR: {
      const State& prop_state = *estate->state;
      assert(prop_state.varIsConst(estate->io.var));
      AValue aval = const_cast<State&>(prop_state)[estate->io.var];
      //cerr<<aval<<endl;
      return c == aval+'A';
    }
    default:
      return false;
    }
  }

  // Implementation status: DONE
  void visit(const OutputSymbol* e) {
    // Caveat: Although the LTL semantics say so, we can't start on
    // the start node. Maybe at the first I/O node? 
    //
    // Think about ``oA U oB''.
    //
    // PLEASE NOTE: We therefore define oX to be true until oY occurs
    //

    // propagate the O predicate until we reach the next O predicate
    fixpoint(Bot(),                                     // init
	     &&,                                        // join
	     isOutputState(state, e->c) || joined_preds // calc
	     );
  }

  // Implementation status: DONE
  void visit(const Not* e) {
    FOR_EACH_STATE(state, label) 
      props[e] = !props[e->expr];
  }

  // Implementation status: TODO
  void visit(const Next* e) { assert(false); }

  /**
   * I'm interpreting Eventually to be a backward problem
   *  
   *  a	    if p(b) then F p(a) and F p(b) but not F p(c)
   *  |\
   *  b c
   *
   * propagate the information that the event occured up each path
   *
   * Implementation status: DONE
   */
  void visit(const Eventually* e) {
    bw_fixpoint(Bot(),                         // init
		props[e->expr].isTrue(),       // start
		&&,                            // join
		props[e->expr] || joined_succs // calc
		);
    // FOR_EACH_STATE(state, label) 
    //   if (props[e] == BOT) props[e] = false;
  }

  /**
   * True, iff for each state we have TOP or TRUE
   *
   * Implementation status: DONE
   */
  void visit(const Globally* e) {
    BoolLattice global = true;
    FOR_EACH_STATE(state, label) {
      global = global && props[e->expr];
      // TOP and TRUE are seen as valid
      if (global.isFalse()) {
	cerr<<"global failed at "<<label<<endl;
	break;
      }
    }
    // propagate the global result to all states
    {  FOR_EACH_STATE(state, label) 
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

  /**
   * A holds until B occurs
   *
   * I'm interpreting UNITL as follows:
   *
   *  a
   *  |\
   * Ab \    A U B is valid at b and c
   *  | Ad
   * Bc 
   *
   * Implementation status: DONE
   */
  void visit(const Until* e) {
    bw_fixpoint(Bot(),                          // init
		props[e->expr2].isTrue(),       // start
		&&,                             // join
		props[e->expr2] || (props[e->expr1] && joined_succs) // calc
		);

    // FOR_EACH_STATE(state, label) 
    //  if (props[e] == BOT) props[e] = false;
  }

  /**
   * A holds until B occurs, which may never happen
   *
   * I'm interpreting WEAK UNITL as follows:
   *
   *  a
   *  |\
   * Ab \    A WU B is valid at b and c and f
   *  | Ad
   * Ac	 |\
   *	 e Bf
   *
   * Implementation status: TESTING
   */
  void visit(const WeakUntil* e) {
    bw_fixpoint(props[e->expr1].isTrue() 
		? BoolLattice(true)
		: BoolLattice(Bot()),                  // init
		props[e->expr1].isTrue() || props[e->expr2].isTrue(), // start
		&&,                                    // join
		props[e->expr2] || (props[e->expr1] && joined_succs) // calc
		);

    // FOR_EACH_STATE(state, label) 
    //   if (props[e] == BOT) props[e] = false;
  }

  /**
   * If !B occurs, A happens before it.
   *
   * Implementation status: DONE?, BUT UNSURE ABOUT SEMANTICS
   */
  void visit(const Release* e) {
    fixpoint(Bot(),  // init
	     &&,                                     // join
	     ((props[e->expr1] && props[e->expr2]) || // A&B  or
	      (!props[e->expr2] && joined_preds))     // !A & B@pred
	     );
  }
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
  s<<"node[shape=rectangle, color=gray, style=filled];\n  ";
  FOR_EACH_TRANSITION(t) {
    switch (t->source->io.op) {
    case InputOutput::IN_VAR:
      s<<estate_label[t->source]<<" [shape=rectangle, color=yellow, style=filled];\n  ";
      break;
    case InputOutput::OUT_VAR:
    case InputOutput::OUT_CONST:
      s<<estate_label[t->source]<<" [shape=rectangle, color=blue, style=filled];\n  ";
      break;
    default: break;
    }
    s<<estate_label[t->source]<<" -> "<<estate_label[t->target]<<";\n";
  }
  FOR_EACH_STATE(state, l) {
    Visualizer viz(v.ltl_properties, l);
    e.accept(viz, Visualizer::newAttr(l));
    s<<l<<" [label=\""<<l<<":"<< state->toString() <<"\"] ;\n";
    s<<"subgraph ltl_"<<l<<" {\n";
    s<<"  node[shape=rectangle, style=filled];\n  ";
    s<<viz.s.str();
    s<<"}\n";
  }
  s<<"}\n";

  ofstream myfile;
  stringstream fname;
  static int n = 1;
  fname << "ltl_output_" << n++ << ".dot";
  myfile.open(fname.str().c_str(), ios::out);
  myfile << s.str();
  myfile.close();
  cout<<"generated "<<fname.str()<<"."<<endl;

  // use result at start node as return value, 
  // I hope this is always correct
  FOR_EACH_STATE(state, label) 
    return v.props[&e].isTrue();
}

