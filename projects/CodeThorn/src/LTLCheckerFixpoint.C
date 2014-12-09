
#include "sage3basic.h"

#include "LTL.h"
#include "LTLCheckerFixpoint.h"
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
using namespace FixpointLTL;

#define FOR_EACH_TRANSITION(TRANSITION)                     \
  for (TransitionGraph::const_iterator TRANSITION=transitionGraph.begin(); \
       TRANSITION != transitionGraph.end();                \
       ++TRANSITION)

#define FOR_EACH_ESTATE(STATE, LABEL)                     \
  Label LABEL=0;                                                     \
  for (EStateSet::const_iterator STATE=eStateSet.begin();         \
       STATE != eStateSet.end();                     \
       ++STATE, ++LABEL)

#define FOR_EACH_STATE(STATE, LABEL)                    \
  GraphTraits::vertex_iterator vi, vi_end;                \
  Label LABEL;                                \
  const EState* STATE;                            \
  for (tie(vi, vi_end) = vertices(g), LABEL=*vi, STATE=g[LABEL.getId()]; \
       vi != vi_end; ++vi,                        \
         LABEL=(vi!=vi_end)?*vi:NULL, STATE=g[LABEL.getId()])   \

#define props ltl_properties[label.getId()]


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
  // MS: removed smart pointer
  //  static Attr* get(IAttr a) { return static_cast<Attr*>(a.get()); }
  static Attr* get(IAttr a) { return static_cast<Attr*>(a); }
  static IAttr newAttr(int n)  { return IAttr((InheritedAttribute*)new Attr(n)); }

  int newNode(IAttr a) {
    int node = label.getId()*shift+n++;
    s<<node<<" -> "<<get(a)->dot_label<<" [color=limegreen, weight=2, style=dashed];\n  ";
    return node;
  }
  static string color(BoolLattice lval) {
    if (lval.isTrue())  return  "color=limegreen";
    if (lval.isFalse()) return  "color=crimson";
    if (lval.isTop())   return  "color=red";
    if (lval.isBot())   return  "color=gainsboro";
    throw "error";
  }

  IAttr visit(InputSymbol* expr, IAttr a)  {
    short e = expr->label;
    int node = newNode(a);
    s<<node<<" ["<<color(props[e])<<",label=\"Input "<<string(1, expr->c)
     <<" = "<<props[e]<<"\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(OutputSymbol* expr, IAttr a) {
    short e = expr->label;
    int node = newNode(a);
    s<<node<<" ["<<color(props[e])<<",label=\"Output "<<string(1, expr->c)
     <<" = "<<props[e]<<"\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(NegInputSymbol* expr, IAttr a)  {
    short e = expr->label;
    int node = newNode(a);
    s<<node<<" ["<<color(props[e])<<",label=\"¬Input "<<string(1, expr->c)
     <<" = "<<props[e]<<"\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(NegOutputSymbol* expr, IAttr a) {
    short e = expr->label;
    int node = newNode(a);
    s<<node<<" ["<<color(props[e])<<",label=\"¬Output "<<string(1, expr->c)
     <<" = "<<props[e]<<"\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(Not* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    int node = newNode(a);
    s<<node<<" ["<<color(props[e])<<",label=\""<<props[e]
     <<" = "<<"!"<<props[e1]<<"\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(Next* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    int node = newNode(a);
    s<<node<<" [shape=circle,"<<color(props[e])<<",label=\""<<"("<<props[e]
     <<" = "<<"X "<<props[e1]<<")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(Eventually* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    int node = newNode(a);
    s<<node<<" [shape=diamond,"<<color(props[e])<<",label=\""<<"("<<props[e]
     <<" = "<<"F "<<props[e1]<<")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(Globally* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    int node = newNode(a);
    s<<node<<" [shape=box,"<<color(props[e])<<",label=\""<<"("<<props[e]
     <<" = "<<"G "<<props[e1]<<")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(And* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    short e2 = expr->expr2->label;
    int node = newNode(a);
    s<<node<<" ["<<color(props[e])<<",label=\""<<"("<<props[e]
     <<" = "<<props[e1]<<" & "<<props[e2]<<")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(Or* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    short e2 = expr->expr2->label;
    int node = newNode(a);
    s<<node<<" ["<<color(props[e])<<",label=\""<<"("<<props[e]
     <<" = "<<props[e1]<<" | "<<props[e2]<<")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(Until* expr, IAttr a)    {
    short e = expr->label;
    short e1 = expr->expr1->label;
    short e2 = expr->expr2->label;
    int node = newNode(a);
    s<<node<<" ["<<color(props[e])<<",label=\""<<"("<<props[e]
     <<" = "<<props[e1]<<" U "<<props[e2]<<")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(WeakUntil* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    short e2 = expr->expr2->label;
    int node = newNode(a);
    s<<node<<" ["<<color(props[e])<<",label=\""<<"("<<props[e]
     <<" = "<<props[e1]<<" WU "<<props[e2]<<")\"];\n  ";
    return newAttr(node);
  }
  IAttr visit(Release* expr, IAttr a) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    short e2 = expr->expr2->label;
    int node = newNode(a);
    s<<node<<" ["<<color(props[e])<<",label=\""<<"("<<props[e]
     <<" = "<<props[e1]<<" R "<<props[e2]<<")\"];\n  ";
    return newAttr(node);
  }
};


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
class Verifier: public BottomUpVisitor {
  EStateSet& eStateSet;
  BoostTransitionGraph& g;
  deque<Label>& endpoints;
  Label start;
  // this map contains the label of every LTL expression
  map<const Expr*, Label> expr_label;

public:
  LTLProperties ltl_properties;

  Verifier(EStateSet& ess, BoostTransitionGraph& btg, deque<Label>& exits,
       Label start_label, Label max_label, short expr_size)
    : eStateSet(ess), g(btg), endpoints(exits), start(start_label) {
    // reserve a result map for each label
    // it maps an analysis result to each sub-expression of the ltl formula
    ltl_properties.resize(max_label.getId());
    for (Label i=0; i<max_label; i++)
      ltl_properties[i.getId()].resize(expr_size);
  }

  /**
   * perform a forward-directed fixpoint iteration over all states
   *
   * \param INIT  initial value of each node
   * \param JOIN  join operator
   * \param TRANSFER  dataflow equation
   *
   *
   * FIXME: rewrite this as a template
   */
# define fixpoint(INIT, JOIN, TRANSFER) {                \
    FOR_EACH_STATE(state, label)                    \
      props[e] = INIT;                            \
                                    \
    queue<Label> workset;                        \
    workset.push(start);                        \
                                    \
    while (!workset.empty()) {                        \
      Label label = workset.front(); workset.pop();            \
      /*cerr<<"Visiting state "<<label<<endl;*/                \
      const EState* state = g[label];                    \
      assert(state);                            \
                                    \
      /* Merge result of incoming edges */                \
      BoolLattice joined_preds = Bot();                    \
      /* for each predecessor */                    \
      GraphTraits::in_edge_iterator in_i, in_end;            \
      for (tie(in_i, in_end) = in_edges(label, g); in_i != in_end; ++in_i) { \
    Label pred = source(*in_i, g);                    \
    BoolLattice pred_prop = ltl_properties[pred][e];        \
    /*cerr<<"  pred: "<<pred<<" = "<<pred_prop<<endl;*/        \
                                    \
    joined_preds = (joined_preds JOIN pred_prop);            \
      }                                    \
                                    \
      /* Calculate property for this node */                \
      /*assert(props.find(e->expr1) != props.end());*/            \
      BoolLattice old_val = props[e];                    \
      props[e] = (old_val JOIN ( TRANSFER ));                \
      /* cerr<<"  "<<label<<" <- "<<props[e]<<endl; */            \
      bool no_fixpoint = (old_val == INIT || old_val != props[e]);    \
      if (no_fixpoint) {                        \
    /* for each successor */                    \
    GraphTraits::out_edge_iterator out_i, out_end;            \
    for (tie(out_i, out_end) = out_edges(label, g); out_i != out_end; ++out_i) { \
      Label succ = target(*out_i, g);                \
      /*cerr<<"  succ: "<<succ<<endl;*/                \
      workset.push(succ);                    \
    }                                \
      } else {                                \
    /*cerr<<"FIX!"<<endl;*/                        \
      }                                    \
    }                                    \
  }

# define NOP do{} while(0)
# define AND &&
  // Apologies for this. Really, can a macro be any more dirty!?
# define LUB ).lub(
# define GLB ).glb(


  /**
   * perform a backward-directed fixpoint iteration over all states
   *
   * since our programs do not necessarily have exit nodes, we start
   * the working set with all exit nodes AND all nodes where START == true.
   *
   * \param INIT  initial value of each node
   * \param JOIN  join operator
   * \param TRANSFER  dataflow equation
   *
   * FIXME: rewrite this as a template
   */
# define bw_fixpoint(INIT, JOIN, TRANSFER, DEBUG) {            \
    /* ensure that INIT is the neutral element */            \
    assert((BoolLattice(true)  JOIN INIT) == true);            \
    assert((BoolLattice(false) JOIN INIT) == false);            \
    assert((BoolLattice(Bot()) JOIN INIT) == Bot());            \
    assert((BoolLattice(Top()) JOIN INIT) == Top());            \
                                    \
                                    \
    queue<Label> workset(endpoints);                    \
    FOR_EACH_STATE(state, label) {                    \
      props[e] = INIT;                            \
    }                                    \
                                    \
    while (!workset.empty()) {                        \
      Label label = workset.front(); workset.pop();            \
      /*cerr<<"Visiting state "<<label<<endl;*/                \
      const EState* state = g[label];                    \
      assert(state);                            \
                                    \
      /* Merge result of incoming edges */                \
      /* init to neutral element in lattice */                \
      BoolLattice joined_succs = INIT;                    \
      /* for each successor */                        \
      GraphTraits::out_edge_iterator out_i, out_end;            \
      for (tie(out_i, out_end) = out_edges(label, g); out_i != out_end; ++out_i) { \
    Label succ = target(*out_i, g);                    \
    BoolLattice succ_prop = ltl_properties[succ][e];        \
    /*cerr<<"  succ: "<<succ<<" = "<<succ_prop<<endl;*/        \
    joined_succs = (joined_succs JOIN succ_prop);            \
      }                                    \
                                    \
      /* Calculate property for this node */                \
      /*assert(props.find(e->expr1) != props.end());*/            \
      BoolLattice old_val = props[e];                    \
                                    \
      props[e] = (old_val JOIN TRANSFER);                \
                                    \
      DEBUG;                                \
                                    \
      /*cerr<<"  "<<label<<" <- "<<props[e]<<" was: "<<old_val<<endl;*/    \
      bool no_fixpoint = (old_val != props[e]);                \
      if (no_fixpoint) {                        \
    /*cerr<<"NO FIX!"<<endl;*/                    \
    /* for each predecessor */                    \
    GraphTraits::in_edge_iterator in_i, in_end;            \
    for (tie(in_i, in_end) = in_edges(label, g); in_i != in_end; ++in_i) { \
      Label pred = source(*in_i, g);                \
      /*cerr<<"  pred: "<<pred<<endl;*/                \
      workset.push(pred);                    \
    }                                \
      } else {                                \
    /*cerr<<"FIX!"<<endl;*/                        \
      }                                    \
    }                                    \
  }

  /// tell if a state is an exit node
  bool isEndpoint(Label l) {
    for (deque<Label>::iterator i = endpoints.begin();
     i != endpoints.end(); ++i)
      if (*i == l) return true;
    return false;
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

  static void updateInputVar(const EState* estate, VariableIdMapping::VariableIdSet& input_vars) {
    if (estate->io.op == InputOutput::STDIN_VAR) {
      input_vars.insert(estate->io.var);
    }
  }

  /// return True iff that state is an Oc operation
  static BoolLattice isInputState(const EState* estate, 
                  VariableIdMapping::VariableIdSet& input_vars,
                  int c, BoolLattice joined_preds) {
    updateInputVar(estate, input_vars);
    if (input_vars.empty())
      return Bot();

    BoolLattice r = BoolLattice(Top()) || joined_preds;
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
  void visit(const InputSymbol* expr) {
    short e = expr->label;
    VariableIdMapping::VariableIdSet input_vars;

    fixpoint(Bot(),                                                 // init
         &&,                                                    // join
         isInputState(state, input_vars, expr->c, joined_preds) // transfer
         );

  }

  /// return True iff that state is an !Ic operation
  static BoolLattice isNegInputState(const EState* estate, 
                     VariableIdMapping::VariableIdSet& input_vars,
                     int c, BoolLattice joined_preds) {
    updateInputVar(estate, input_vars);
    if (input_vars.empty())
      return Bot();

    BoolLattice r = joined_preds.isBot() ? Top() : joined_preds;
    assert(estate);
    assert(estate->constraints());
    ConstraintSet constraints = *estate->constraints();
    for (VariableIdMapping::VariableIdSet::const_iterator ivar = input_vars.begin();
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
  void visit(const NegInputSymbol* expr) {
    short e = expr->label;
    VariableIdMapping::VariableIdSet input_vars;

    fixpoint(Bot(),                                                 // init
         &&,                                                    // join
         isNegInputState(state, input_vars, expr->c, joined_preds) // transfer
         );
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
  void visit(const OutputSymbol* expr) {
    short e = expr->label;
    bw_fixpoint(/* init */     Bot(),
        ///* start */    state->io.op != InputOutput::NONE,
        /* join */     LUB,
        /* transfer */ isOutputState(state, expr->c, isEndpoint(label), joined_succs),
        /* debug */    NOP/*cerr<<"out("<<string(1,expr->c)<<")"<<endl*/);
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
  void visit(const NegOutputSymbol* expr) {
    short e = expr->label;
    bw_fixpoint(/* init */     Bot(),
        ///* start */    state->io.op != InputOutput::NONE,
        /* join */     LUB,
        /* transfer */ isNegOutputState(state, expr->c, isEndpoint(label), joined_succs),
        /* debug */    NOP/*cerr<<"neg_out("<<string(1,expr->c)<<")"<<endl*/);
  }

  /**
   * NOT
   *
   * Implementation status: DONE
   */
  void visit(const Not* expr) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    FOR_EACH_STATE(state, label)
      props[e] = !props[e1];
  }

  /**
   * X φ (next): φ has to hold after the next step
   *
   * I'm interpreting Next as follows
   *
   *  a        N A is true at a.
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

    FOR_EACH_STATE(state, label) {
      BoolLattice joined_succs = Bot();

      /* for each successor */
      GraphTraits::out_edge_iterator out_i, out_end;
      for (tie(out_i, out_end) = out_edges(label, g); out_i != out_end; ++out_i) {
    //Label succ = target(*out_i, g);
    joined_succs = (joined_succs LUB ltl_properties[label][e1]);
      }
      props[e] = joined_succs;
    }
  }

  /**
   * F φ (eventually): φ has to hold at some point in the future (or now)
   *
   *
   * I'm interpreting Eventually to be a backward problem
   *
   *  a        if p(b) then F p(a) and F p(b) but not F p(c)
   *  |\
   *  b c
   *
   * propagate the information that the event occured up each path
   *
   * Implementation status: DONE
   */
  void visit(const Eventually* expr) {
    short e = expr->label;
    short e1 = expr->expr1->label;

    bw_fixpoint(/* init */      Bot(),
        ///* start */     !props[e1].isBot(),
        /* join */      LUB,
        /* transfer  */ props[e1] || joined_succs,
        /* debug */     NOP //cerr<<props[e1]<<" || "<<joined_succs<<endl;
        );
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
  void visit(const Globally* expr) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    
    if (expr->quantified) {
      bw_fixpoint(/* init */      Bot(),
          ///* start */     !props[e1].isBot(),
          /* join */      LUB,
          /* transfer  */ props[e1] && joined_succs,
          /* debug */     NOP //cerr<<props[e1]<<" && "<<joined_succs<<endl;
          );
    } else {
      bw_fixpoint(/* init */      Bot(),
          ///* start */     !props[e1].isBot(),
          /* join */      AND, // this allows for improved precision in the case of an All-quantified G node, which is, by, default, any outermost G node
          /* transfer  */ props[e1] && joined_succs,
          /* debug */     NOP //cerr<<props[e1]<<" && "<<joined_succs<<endl;
          );
    };
  }

  // Implementation status: DONE
  void visit(const And* expr) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    short e2 = expr->expr2->label;

    FOR_EACH_STATE(state, label)
      props[e] = props[e1] && props[e2];
  }

  // Implementation status: DONE
  void visit(const Or* expr) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    short e2 = expr->expr2->label;

    FOR_EACH_STATE(state, label)
      props[e] = props[e1] || props[e2];
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
    short e = expr->label;
    short e1 = expr->expr1->label;
    short e2 = expr->expr2->label;

    bw_fixpoint(/* init */      Bot(),
        ///* start */     (!props[e1].isBot() || !props[e2].isBot()),
        /* join */      LUB,
        /* transfer */  (props[e2] || (props[e1] && joined_succs)) || false,
        /* debug */     NOP
        //cerr<<label<<": "<<((props[e2] || (props[e1] && joined_succs)) || false).toString()<<" == "<<props[e2]<<" || ("<<props[e1]<<" && "<<joined_succs<<")"<<endl
        );
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
   * Ac     |\
   *     e Bf
   *
   * Implementation status: DONE
   */
  void visit(const WeakUntil* expr) {
    assert(false);
    
    short e = expr->label;
    short e1 = expr->expr1->label;
    short e2 = expr->expr2->label;

    bw_fixpoint(/* init */    Bot(),
        ///* start */    !props[e2].isBot(),
        /* join */    LUB,
        /* transfer */    props[e2] || (props[e1] && joined_succs),
        /* debug */     NOP
        );

    //FOR_EACH_STATE(state, label)
    //  if (props[e].isBot()) props[e] = false;
  }

  /**
   * φ R ψ (release): φ has to hold until ψ held in the previous step.
   *
   * If !B occurs, A happens before it.
   *
   * According to the unwinding property, R seems to be the dual operator of U.
   * Implementation status: DONE
   */
  void visit(const Release* expr) {
    short e = expr->label;
    short e1 = expr->expr1->label;
    short e2 = expr->expr2->label;

    bw_fixpoint(/* init */      Bot(),
        ///* start */     (!props[e1].isBot() || !props[e2].isBot()),
        /* join */      LUB,
        /* transfer */  props[e2] && (props[e1] || joined_succs),
        /* debug */     NOP
        );
  }
};


Checker::Checker(EStateSet& ess, TransitionGraph& _tg)
  : transitionGraph(_tg),
    eStateSet(ess)
{
 // Build our own customized Transition graph
  int i = 0;
  map<const EState*, Label> estate_label;
  FOR_EACH_ESTATE(state, l1) {
    estate_label[*state] = i++;
  }
  BoostTransitionGraph full_graph(ess.size());

  FOR_EACH_TRANSITION(t) {
    Label src = estate_label[(*t)->source];
    Label tgt = estate_label[(*t)->target];
    add_edge(src, tgt, full_graph);
    full_graph[src] = (*t)->source;
    full_graph[tgt] = (*t)->target;
    //cerr<<src<<"("<<t->source<<") -- "<<tgt<<"("<<t->target<<")"<<endl;
    assert(full_graph[src]);
    assert(full_graph[tgt]);
  }
  start = estate_label[transitionGraph.getStartTransition().source];
  if(boolOptions["post-collapse-stg"]) {
    // Optimization
    start = collapse_transition_graph(full_graph, g);
  } else {
    g = full_graph;
  }
  FOR_EACH_STATE(state, label) {
    //if (out_degree(label, g) == 0) {
     // endpoints.push_back(label);
      //cerr<<label<<endl;
    //}
    bool endstate = true;
    GraphTraits::out_edge_iterator out_i, out_end;
    for (tie(out_i, out_end) = out_edges(label, g); out_i != out_end; ++out_i) {
      Label succ = target(*out_i, g);
      endstate = endstate && (succ==label);
    }
    if (endstate) 
      endpoints.push_back(label);
  }
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
Label Checker::collapse_transition_graph(BoostTransitionGraph& g, 
                     BoostTransitionGraph& reduced) const {
  Label n = 0;
  vector<Label> renumbered(num_vertices(g));

  FOR_EACH_STATE(state, label) {
    //cerr<<label<<endl;
    assert(g[label.getId()]);
    if (( in_degree(label, g) >= 1) && // keep start
    (out_degree(label, g) >= 0) && // DO NOT keep exits
        (g[label.getId()]->io.op == InputOutput::NONE ||
         g[label.getId()]->io.op == InputOutput::FAILED_ASSERT)) {
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
      renumbered[label.getId()] = n++;
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
    add_edge(renumbered[src.getId()], renumbered[tgt.getId()], reduced);
    //cerr<<renumbered[src]<<" -> "<<renumbered[tgt]<<";"<<endl;
    reduced[renumbered[src.getId()]] = g[src.getId()];
    reduced[renumbered[tgt.getId()]] = g[tgt.getId()];
  }
  //cerr<<"}"<<endl;

  //cerr<<"## done "<<endl<<endl;
  cerr<<"Number of EStates: "<<num_vertices(g)<<endl;
  cerr<<"Number of LTLStates: "<<num_vertices(reduced)<<endl;

  return renumbered[start.getId()];
}


BoolLattice
Checker::verify(const Formula& f)
{
  // Verify!
  Verifier v(eStateSet, g, endpoints, start.getId(), num_vertices(g), f.size());
  const Expr& e = f;
  e.accept(v);

  // Visualization:
  // generate dot output for debugging
  // TODO: implement these flags as cmdline options
  bool ltl_output_dot = boolOptions["ltl-output-dot"];//  true;
  bool show_derivation = boolOptions["ltl-show-derivation"];//  true;
  bool show_node_detail = boolOptions["ltl-show-node-detail"];//  true;
  bool collapsed_graph = boolOptions["ltl-collapsed-graph"];//  false

  if (ltl_output_dot) {
    stringstream s;
    s<<"digraph G {\n";
    s<<"node[shape=rectangle, color=lightsteelblue, style=filled];\n  ";


    GraphTraits::edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
      Label src = source(*ei, g);
      Label tgt = target(*ei, g);
      switch (g[src]->io.op) {
      case InputOutput::STDIN_VAR:
    s<<src<<" [shape=rectangle, color=gold, style=filled];\n  ";
    break;
      case InputOutput::STDOUT_VAR:
      case InputOutput::STDOUT_CONST:
    s<<src<<" [shape=rectangle, color=indigo, style=filled];\n  ";
    break;
      default: break;
      }
      s<<src<<" -> "<<tgt<<";\n";
    }
    FOR_EACH_STATE(state, l) {
      Visualizer viz(v.ltl_properties, l);
      const_cast<Expr&>(e).accept(viz, Visualizer::newAttr(l));
      s<<l<<" [label=\""<<l;
      if (show_node_detail) {
    s<<":";
    if (collapsed_graph) {

    } else s<<state->toString();
      }
      s<<"\"] ;\n";
      s<<"subgraph ltl_"<<l<<" {\n";
      s<<"  node[shape=rectangle, style=filled];\n  ";
      if (show_derivation) s<<viz.s.str();
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
  }

  // Skip over states until we reach any i/o state
  // I hope this is always correct.
  //
  // We need to skip over the first couple of non-IO states because
  // the result will be non-sentical (=⊥) most of the time anyway.
  //
  // We do this by doing a BFS and returning the result at the first I/O node.
  // CAVEAT: This will fail horribly on non-RERS inputs.
  queue<Label> workset;
  workset.push(start);
  while (!workset.empty()) {
    Label label = workset.front(); workset.pop();
    const EState* state = g[label];

    // Return the first result at an I/O node
    // FIXME: not always correct, obviously. See comment above.
    switch (state->io.op) {
    case InputOutput::STDOUT_CONST:
    case InputOutput::STDOUT_VAR:
    case InputOutput::STDERR_VAR:
    case InputOutput::STDERR_CONST:
    case InputOutput::STDIN_VAR: {
      //cerr<<label<<": "<<v.ltl_properties[label][e.label]<<endl;
      BoolLattice result = v.ltl_properties[label][e.label];
      //assert(!result.isBot());
      return result;
    }
    default: ;
    }

    /* add each successor to workset */
    GraphTraits::out_edge_iterator out_i, out_end;
    for (tie(out_i, out_end) = out_edges(label, g); out_i != out_end; ++out_i) {
      Label succ = target(*out_i, g);
      workset.push(succ);
    }
  }
  return Top();
}
