// -*- C++ -*-
#ifndef __LTLCHECKERUNIFIED_H__
#define __LTLCHECKERUNIFIED_H__

// Copyright (c) 2012 Lawrence Livermore National Security, LLC.
// Produced at the Lawrence Livermore National Laboratory
// Written by Adrian Prantl <adrian@llnl.gov>.
//
// UCRL-CODE-155962.
// All rights reserved.
//
// This file is part of ROSE. For details, see http://www.rosecompiler.org/.
// Please read the COPYRIGHT file for Our Notice and for the BSD License.

#include "StateRepresentations.h"
#include "AType.h"
#include <map>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "LTL.h"
//using LTL::Formula;

namespace CodeThorn {

namespace UnifiedLTL {
  using namespace boost;
  using namespace std;
  using namespace AType;

  typedef adjacency_list<vecS, vecS, bidirectionalS, const EState*> BoostTransitionGraph;
  typedef graph_traits<BoostTransitionGraph> GraphTraits;
  typedef GraphTraits::vertex_descriptor Vertex;

  /**
   * This is pretty cool: We are solving the LTL formula as if it were
   * written in a postfix notation and store the intermediate results
   * on a stack. It's a little like the Forth execution model.
   * For equality comparisons we are only using the top of
   * stack, because we do not care about the history.
   */
  class LTLState {
  public:
    const EState* estate;
    stack<BoolLattice> valstack;

    LTLState() : estate(NULL) { }
    LTLState(const EState* s, stack<BoolLattice> v) : estate(s), valstack(v) {}
    LTLState(const EState* s, BoolLattice top) : estate(s){ valstack.push(top); }
    LTLState(const LTLState& copy) :estate(copy.estate), valstack(copy.valstack) {}
    bool operator==(LTLState& other) const { 
      return (estate == other.estate) &&
	( !valstack.empty()
	  ? valstack.top() == other.valstack.top()
	  : true );
    }
    bool operator<(LTLState other) const { 
      if (estate == other.estate) {
	return ( !valstack.empty()
		 ? valstack.top() < other.valstack.top()
		 : false );
      } else {
	return estate < other.estate;
      }
    }
  };

  typedef adjacency_list<listS, listS, bidirectionalS, LTLState> LTLTransitionGraph;
  typedef graph_traits<LTLTransitionGraph> LTLGraphTraits;
  typedef LTLGraphTraits::vertex_descriptor LTLVertex;
  typedef LTLGraphTraits::edge_descriptor LTLEdge;
  typedef map<LTLState, LTLVertex> LTLStateMap;

  class LTLStateTransitionGraph {
  public:
    LTLStateTransitionGraph() {}
    LTLStateTransitionGraph(LTLStateMap m, LTLTransitionGraph tg) 
      : vertex(m), g(tg) {}
    LTLStateMap vertex;
    LTLTransitionGraph g;
  };

  typedef queue<LTLVertex> LTLWorklist;

  /**
   * A dataflow-based checker for LTL formulae.
   * \date 2012
   * \author Adrian Prantl
   */
  class Checker {
  public:
    Checker(EStateSet& ess, TransitionGraph& g);
    /// verify the LTL formula f
    BoolLattice verify(const CodeThorn::LTL::Formula& f);

    Label collapse_transition_graph(BoostTransitionGraph &g, BoostTransitionGraph &reduced) const;

  protected:
    Label start;
    BoostTransitionGraph g;
    TransitionGraph& transitionGraph;
    EStateSet& eStateSet;
  };

} // end of namespace LTL

} // end of namespace CodeThorn

#endif
