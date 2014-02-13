#pragma once

#include "compose.h"
#include "composed_analysis.h"
#include "partitions.h"
#include <map>
#include <list>
#include <set>
#include <string>

namespace fuse {
  
/************************************
 ***** Factor Transition System *****
 ************************************/
 
/* This is a special transition system used for debugging. Its states are integers and the successor/predecessor
 *    of a given integer is obtained by multiplying/dividing it by one of a set of multipliers (must be mutually prime). 
 *    Different transition systems can be defined by varying the choice of multipliers and by multiplying all the numbers 
 *    within a given transition system by the same base factor that is mutually prime relative to the multipliers. Further,
 *    if two transition systems (i) start from the same number, (ii) use the same multipliers and (ii) the base factor 
 *    of one divides the base factor of the other, then the latter system (the one with the bigger base factor) strictly
 *    refines the former. Specifically, multiple states of the fine system can be uniquely mapped to one state of the 
 *    coarse and this mapping is preserved across state transitions. This fact makes it easy to model multiple transition 
 *    systems that refine each other, simplifying debugging of code the operates on multiple such systems.
 * This file defines the Parts and PartEdges of this transition system as well as an undirected ComposedAalysis that 
 *    returns this graph to the composer and can refine the transition systems by other instances of the analysis. 
 *    All the transition systems in this implementation start at 1 and have an upper bound on the number of 
 *    multiplications they perform before they roll backwards by dividing the current value by a fixed number of factors.
 */

extern int factorTransSystemDebugLevel;
  
class FactorTransState;
typedef CompSharedPtr<FactorTransState> FactorTransStatePtr;
extern FactorTransStatePtr NULLFactorTransState;

class FactorTransEdge;
typedef CompSharedPtr<FactorTransEdge> FactorTransEdgePtr;
extern FactorTransEdgePtr NULLFactorTransEdge;

class PartPtrPartContext;
typedef CompSharedPtr<PartPtrPartContext> PartPtrPartContextPtr;

class FactorTransSystemAnalysis;

/******************************
 ***** PartPtrPartContext *****
 ******************************/

// PartContext that is just a PartPtr
class PartPtrPartContext: public PartContext
{
  PartPtr part;

  public:
  PartPtrPartContext(PartPtr part) : part(part) {}
  
  // Returns a list of PartContextPtr objects that denote more detailed context information about
  // this PartContext's internal contexts. If there aren't any, the function may just return a list containing
  // this PartContext itself.
  std::list<PartContextPtr> getSubPartContexts() const;
    
  bool operator==(const PartContextPtr& that_arg) const;
  bool operator< (const PartContextPtr& that_arg) const;
  
  std::string str(std::string indent="");
}; // PartPtrPartContext

/****************************
 ***** FactorTransState *****
 ****************************/

class FactorTransState : public Part
{
  friend class FactorTransSystemAnalysis;
  friend class FactorTransEdge;
  friend class PartPtrPartContext;
  
  // Factors that we multiply/divide curVal by as part of state transitions.
  // Each FactorTransState maintains a reference to the copy of this set in the host FactorTransSystemAnalysis.
  const std::set<int>& transitionFactors;
  
  // Factors by which all values in this transition system must be multiplied by
  // The product of the common factors of this Part's ancestor states (1 for states in the base transition system)
  int baseCommonFactor;
  // The common factor that is specific to this transition system
  int myCommonFactor;
  
  // The current base value of this Part
  int curVal;
  
  // The number of multiplication steps on the shortest path between this transition system's starting 
  // Part and this Part.
  int numSteps;
  
  // Within the derived system we take a finite number of steps using multipliers from myCommonFactors to create
  // a sub-graph of derived states that corresponds to a single base state. This is the number of multiplication 
  // steps taken within the sub-graph in the derived transition system that corresponds to a single state of the 
  // server transition system, from the start of the sub-graph until the current state.
  int numStepsInDerivedGraph;
  
  // This set contains all the transition factors that can be used to generate states inside each derived
  // sub-graph. All the states reachable via these multipliers correspond to the same server state.
  const std::set<int>& subGraphTransFactors;
  // While generating states within a derived sub-graph we need to keep track of which set of multipliers
  // have been applied to reach the given state. This is the state's current iterator within subGraphTransFactors.
  // Its overall multiplier is the product of the multipliers between the start of subGraphTransFactors and *sgtFactorIt.
  //std::set<int>::iterator sgtFactorIt;
  
  public:
  // Constructor used to create the states of the base transition system (not a refinement of any other system)
  FactorTransState(const std::set<int>& transitionFactors, const std::set<int>& subGraphTransFactors, 
                   int initVal, int numSteps, ComposedAnalysis* analysis);
  
  // Create a State from the given base Part, while adding the extra factor to common factors
  // to make sure that this Part refines the base Part
  FactorTransState(PartPtr base, const std::set<int>& transitionFactors, 
                  int numStepsInDerivedGraph, const std::set<int>& subGraphTransFactors, /*std::set<int>::iterator sgtFactorIt, */
                  int curVal, int numSteps, int myCommonFactor, ComposedAnalysis* analysis);
  
  FactorTransState(const FactorTransState& that);
  FactorTransState(const FactorTransStatePtr& that);
  
  /*void operator=(const FactorTransState& that);
  void operator=(FactorTransStatePtr that);*/
  
  std::list<PartEdgePtr> outEdges();
  // Version of outEdges for transition systems that are not derived from other ones (have a NULL parent)
  std::list<PartEdgePtr> outEdges_base();
  // Version of outEdges for transition systems that are derived from other ones (have a non-NULL parent)
  std::list<PartEdgePtr> outEdges_derived();
  
  std::list<PartEdgePtr> inEdges();
  // Version of inEdges for transition systems that are not derived from other ones (have a NULL parent)
  std::list<PartEdgePtr> inEdges_base();
  std::list<PartEdgePtr> inEdges_derived();
  
  std::set<CFGNode> CFGNodes() const;
  
  // If this Part corresponds to a function call/return, returns the set of Parts that contain
  // its corresponding return/call, respectively.
  std::set<PartPtr> matchingCallParts() const;
  
  // Returns a PartEdgePtr, where the source is a wild-card part (NULLPart) and the target is this Part
  PartEdgePtr inEdgeFromAny();

  // Returns a PartEdgePtr, where the target is a wild-card part (NULLPart) and the source is this Part
  PartEdgePtr outEdgeToAny();
  
  // Returns the specific context of this Part. Can return the NULLPartContextPtr if this
  // Part doesn't implement a non-trivial context.
  PartContextPtr getPartContext() const;
  
  // Returns the specific context of this Part. Can return the NULLPartContextPtr if this
  // Part doesn't implement a non-trivial context.
  //PartContextPtr getPartContext() const;
  
  bool equal(const PartPtr& o) const;
  bool less(const PartPtr& o)  const;
  
  std::string str(std::string indent="");
  
  /*FactorTransStatePtr getParent() const {
    if(commonFactors.size()>1) {
      list<int> parentCF = commonFactors;
      parentCF.pop_back();
      return makePtr<FactorTransState>(transitionFactors, parentCF, curVal);
    } else {
      return NULLFactorTransState;
    }
  }*/
}; // FactorTransState

/***************************
 ***** FactorTransEdge *****
 ***************************/

class FactorTransEdge : public PartEdge
{
  friend class FactorTransState;
  
  // Factors that we multiply/divide curVal by as part of state transitions
  // Each FactorTransEdge maintains a reference to the copy of this set in the host FactorTransSystemAnalysis.
  const std::set<int>& transitionFactors;
  
  // The common factor that is specific to this transition system
  //int myCommonFactor;
  
  // The the base value of this edge's source and target states
  int srcVal;
  int tgtVal;
  
  // The number of multiplication steps on the shortest path between this transition system's starting 
  // state and this edge's source and target states.
  int srcNumSteps;
  int tgtNumSteps;
  
  // Within the derived system we take a finite number of steps using multipliers from myCommonFactors to create
  // a sub-graph of derived states that corresponds to a single base state. This is the number of multiplication 
  // steps taken within the sub-graph in the derived transition system that corresponds to a single state of the 
  // server transition system, from the start of the sub-graph until the current state.
  int srcNumStepsInDerivedGraph;
  int tgtNumStepsInDerivedGraph;
  
  // This set contains all the transition factors that can be used to generate states inside each derived
  // sub-graph. All the states reachable via these multipliers correspond to the same server state.
  const std::set<int>& subGraphTransFactors;
  
  // While generating states within a derived sub-graph we need to keep track of which set of multipliers
  // have been applied to reach the given state. This is the state's current iterator within subGraphTransFactors.
  // Its overall multiplier is the product of the multipliers between the start of subGraphTransFactors and *sgtFactorIt.
  /*std::set<int>::iterator srcSgtFactorIt;
  std::set<int>::iterator tgtSgtFactorIt;*/
  int srcCommonFactor;
  int tgtCommonFactor;
  
  public:
  // Constructor used to create the states of the base transition system (not a refinement of any other system)
  FactorTransEdge(const std::set<int>& transitionFactors, const std::set<int>& subGraphTransFactors, 
                  int srcVal, int tgtVal, 
                  int srcNumSteps, int tgtNumSteps, ComposedAnalysis* analysis);
  
  // Create a State from the given base PartEdge, while adding the extra factor to common factors
  // to make sure that this PartEdge refines the base PartEdge
  FactorTransEdge(PartEdgePtr base, const std::set<int>& transitionFactors, 
                  const std::set<int>& subGraphTransFactors, 
                  int srcVal, int srcNumSteps, int srcNumStepsInDerivedGraph, int srcCommonFactor, //std::set<int>::iterator srcSgtFactorIt, 
                  int tgtVal, int tgtNumSteps, int tgtNumStepsInDerivedGraph, int tgtCommonFactor, //std::set<int>::iterator tgtSgtFactorIt, 
                  /*int myCommonFactor, */ComposedAnalysis* analysis);
  
  FactorTransEdge(const FactorTransEdge& that);
  FactorTransEdge(const FactorTransEdgePtr& that);
  
  PartPtr source() const;
  PartPtr target() const;
  
  // If the source Part corresponds to a conditional of some sort (if, switch, while test, etc.)
  // it must evaluate some predicate and depending on its value continue, execution along one of the
  // outgoing edges. The value associated with each outgoing edge is fixed and known statically.
  // getPredicateValue() returns the value associated with this particular edge. Since a single 
  // Part may correspond to multiple CFGNodes getPredicateValue() returns a map from each CFG node
  // within its source part that corresponds to a conditional to the value of its predicate along 
  // this edge. 
  std::map<CFGNode, boost::shared_ptr<SgValueExp> > getPredicateValue();
  
  // Adds a mapping from a CFGNode to the outcome of its predicate
  void mapPred2Val(CFGNode n, boost::shared_ptr<SgValueExp> val);
  
  // Empties out the mapping of CFGNodes to the outcomes of their predicates
  void clearPred2Val();
    
  bool equal(const PartEdgePtr& o) const;
  bool less(const PartEdgePtr& o)  const;
  
  // Pretty print for the object
  std::string str(std::string indent="");
}; // FactorTransEdge

/*************************************
 ***** FactorTransSystemAnalysis *****
 *************************************/

class FactorTransSystemAnalysis : public UndirDataflow
{
  friend class FactorTransState;
  
  private:
  // Flag that indicates whether this is the first FactorTransSystemAnalysis in the composition chain
  bool firstAnalysis;
    
  // Factors that we multiply/divide curVal by as part of state transitions
  std::set<int> transitionFactors;
  
  // The factors by which all values in this transition system must be multiplied by
  // The common factor that is specific to this transition system
  //int myCommonFactor;
  
  // The maximum number of multiplication steps on the shortest path between this transition system's starting 
  // Part and this Part.
  int maxSteps;
  
  // The number of multiplication steps that we unroll whenever we hit the maximum number of steps during
  // the transition system graph traversal.
  int numRollbackSteps;
  
  // Within the derived system we take a finite number of steps using multipliers from myCommonFactors to create
  // a sub-graph of derived states that corresponds to a single base state. This is the maximum number of multiplication 
  // steps taken within the sub-graph in the derived transition system that corresponds to a single state of the 
  // server transition system, from the start of the sub-graph until the current state.
  int maxNumStepsInDerivedGraph;
  
  // This set contains all the transition factors that can be used to generate states inside each derived
  // sub-graph. All the states reachable via these multipliers correspond to the same server state.
  const std::set<int> subGraphTransFactors;
  
  
  public:
  FactorTransSystemAnalysis(
        bool firstAnalysis, 
        const std::set<int>& transitionFactors, 
        /*int myCommonFactor, */int maxSteps, int numRollbackSteps,
        int maxNumStepsInDerivedGraph, const std::set<int>& subGraphTransFactors);
  
  // Returns a shared pointer to a freshly-allocated copy of this ComposedAnalysis object
  ComposedAnalysisPtr copy();
  
  // Return the anchor Parts of the application
  std::set<PartPtr> GetStartAStates_Spec();
  std::set<PartPtr> GetEndAStates_Spec();
  
  // Returns true if this ComposedAnalysis implements the partition graph and false otherwise
  bool implementsPartGraph() { return true; }
  
  // Given a PartEdge pedge implemented by this ComposedAnalysis, returns the part from its predecessor
  // from which pedge was derived. This function caches the results if possible.
  PartEdgePtr convertPEdge_Spec(PartEdgePtr pedge);
  
  // pretty print for the object
  std::string str(std::string indent="")
  //{ std::ostringstream oss; oss << "FactorTransSystemAnalysis"<<(firstAnalysis? "(first)": "")<<", &transitionFactors="<<(&transitionFactors)<<" #transitionFactors="<<transitionFactors.size(); return oss.str(); }
  { return "FactorTransSystemAnalysis"+std::string((firstAnalysis? "-First": "")); }
}; // CallContextSensitivityAnalysis

} // namespace fuse

