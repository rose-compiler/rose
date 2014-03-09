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
#include <boost/unordered_map.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "LTL.h"
//using LTL::Formula;
#undef INIT_TO_NEUTRAL_ELEMENT

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

    LTLState() : estate(NULL) { }
    LTLState(const EState* s, size_t stacksize) 
      : estate(s), valstack(stacksize,
#ifdef INIT_TO_NEUTRAL_ELEMENT
			    Bot()
#else
			    Top()
#endif
			    ) {}
    
    inline BoolLattice top()  const { return valstack.back(); }
    inline BoolLattice over() const { return valstack[valstack.size()-2]; }
    BoolLattice pop()  { 
      BoolLattice val = valstack.back(); 
      valstack.pop_back(); 
      return val; 
    }
    void push(BoolLattice val) { valstack.push_back(val); }

    inline bool operator==(const LTLState& other) const { 
      //cerr<<"?  "<<*this<<"\n== "<<other<<"\n=> "
      //       << (estate == other.estate)<<" && "
      //       <<(valstack == other.valstack)<<" && "
      //       <<(val == other.val)<<endl;
      return (estate == other.estate) && (valstack == other.valstack);
    }
    bool operator<(const LTLState& other) const {
      if (estate  < other.estate) return true;
      if (estate == other.estate)
	return (valstack  < other.valstack);
      return false;
    }
    friend ostream& operator<<(ostream& os, const LTLState& s);

    string toHTML() const;
  };

  /// Hash function for LTL States
  inline std::size_t hash_value(CodeThorn::UnifiedLTL::LTLState const& s) {
    // the idea is that there will be rarely more than 4 LTLStates with the same estate
    return (size_t)s.estate+(size_t)s.valstack.back().val();
  }

  ostream& operator<<(ostream& os, const LTLState& s);
  
  // For the LTLTransitionGraph we need a mutable version!
  typedef adjacency_list<hash_setS, listS, bidirectionalS, LTLState> LTLTransitionGraph;
  typedef graph_traits<LTLTransitionGraph> LTLGraphTraits;
  typedef LTLGraphTraits::vertex_descriptor LTLVertex;
  typedef LTLGraphTraits::edge_descriptor LTLEdge;
  typedef boost::unordered_map<LTLState, LTLVertex> LTLStateMap;

  class LTLStateTransitionGraph {
  public:
    LTLStateTransitionGraph() {}
    LTLStateTransitionGraph(LTLStateMap m, LTLTransitionGraph tg) 
      : vertex(m), g(tg) {}
    LTLStateMap vertex;
    LTLTransitionGraph g;
  };

  struct LTLWorklist: public vector<LTLVertex> {
    inline void push(LTLVertex v) { push_back(v); }
    inline LTLVertex pop() { LTLVertex result = back(); pop_back(); return result; }
  };

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

    void collapse_transition_graph(BoostTransitionGraph &g, BoostTransitionGraph &reduced) const;

  protected:
    BoostTransitionGraph g;
    TransitionGraph& transitionGraph;
    EStateSet& eStateSet;
  };

} // end of namespace LTL

} // end of namespace CodeThorn

#endif
