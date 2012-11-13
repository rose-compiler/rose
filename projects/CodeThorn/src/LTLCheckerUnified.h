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
   */
  struct LTLState {
    const EState* estate;
    vector<BoolLattice> valstack;
    BoolLattice val;  /// result of the current iteration, will become the next top of stack
    vector<BoolLattice> debug; // stores all intermediate results for the dot output

    LTLState() : estate(NULL), val(Bot()) { valstack.push_back(Bot()); }
    //LTLState(const EState* s, vector<BoolLattice> v) : estate(s), valstack(v) {}
    LTLState(const EState* s, BoolLattice top) : estate(s), val(top) { }
    
    BoolLattice top()  const { return valstack.back(); }
    BoolLattice over() const { return valstack[valstack.size()-2]; }
    BoolLattice pop()  { 
      BoolLattice val = valstack.back(); 
      valstack.pop_back(); 
      return val; 
    }
    void push(BoolLattice val) { valstack.push_back(val); }

    //LTLState(const LTLState& copy) :estate(copy.estate), valstack(copy.valstack) {}
    bool operator==(LTLState& other) const { 
      //cerr<<"?  "<<*this<<"\n== "<<other<<"\n=> "<< (estate == other.estate)<<" && "<<(valstack == other.valstack)<<endl;
      return (estate == other.estate) && (valstack == other.valstack) && (val == other.val);
    }
    bool operator<(LTLState other) const { 
      if (estate == other.estate) {
	if (val == other.val)
	  return valstack < other.valstack;
	else 
	  return val < other.val;
      } else {
	return estate < other.estate;
      }
    }
    friend ostream& operator<<(ostream& os, const LTLState& s);
  };

  ostream& operator<<(ostream& os, const LTLState& s);
  
  // For the LTLTransitionGraph we need a mutable version!
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
  class UChecker {
  public:
    UChecker(EStateSet& ess, TransitionGraph& g);
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
