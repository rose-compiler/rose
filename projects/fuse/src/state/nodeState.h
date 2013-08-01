#ifndef NODE_STATE_H
#define NODE_STATE_H

#include "DataflowCFG.h"
namespace fuse {
class NodeFact;
class NodeState;
};

#include "lattice.h"
#include "analysis.h"
#include <map>
#include <vector>
#include <string>
#include <set>

namespace fuse {
class ComposedAnalysis;
extern int nodeStateDebugLevel;

/************************************************
 ***         NodeFact       ***
 *** A fact associated with a CFG node by     ***
 *** some analysis thatis not evolved as part ***
 *** of a dataflow analysis (i.e. it should   ***
 *** stay constant throughout the analysis).  ***
 ************************************************/
// A fact associated with a CFG node that is not part of a dataflow analysis. In other words, 
// it is not a lattice and is not meant to evolve during the course of a dataflow analysis.
class NodeFact: public dbglog::printable
{
  public:
  
  // The string that represents this object.
  // Every line of this string must be prefixed by indent.
  // The last character of the returned string must not be '\n', even if it is a multi-line string.
  //virtual string str(string indent="")=0;
  
    // returns a copy of this node fact
  virtual NodeFact* copy() const=0;
  
/*      void* fact;
  
  public:
  NodeFact(void* fact)
  {
    this->fact = fact;
  }
  
  NodeFact(factType* fact)
  {
    this->fact = *fact;
  }
  
  void* getFact()
  {
    return fact;
  }*/
};


/**********************************************
 ***         NodeState    ***
 *** The state of all the Lattice elements  ***
 *** associated by dataflow analyses with a ***
 *** given node. This state will evolve as  ***
 *** a result of the dataflow analysis.     ***
 **********************************************/

class NodeState
{
  typedef std::map<Analysis*, std::map<PartEdgePtr, std::vector<Lattice*> > > LatticeMap;
  typedef std::map<Analysis*, std::vector<NodeFact*> > NodeFactMap;
  typedef std::map<Analysis*, bool > BoolMap;
  
  enum nodeSide {above, below};
  
  // the dataflow information Above the node, for each analysis that 
  // may be interested in the current node
  LatticeMap dfInfoAbove;
  
  // the Analysis information Below the node, for each analysis that 
  // may be interested in the current node
  LatticeMap dfInfoBelow;

  // the facts that are true at this node, for each analysis that 
  // may be interested in the current node
  NodeFactMap facts;
  
  // Contains all the Analyses that have initialized their state at this node. It is a map because
  // TBB doesn't provide a concurrent set.
  BoolMap initializedAnalyses;
  
  // the dataflow node that this NodeState object corresponds to
  //PartPtr parentNode;
  
  public:
  
  NodeState()
  {}
  
  public:
  // Records that this analysis has initializedAnalyses its state at this node
  void initialized(Analysis* analysis);
  
  // Returns true if this analysis has initialized its state at this node and false otherwise
  bool isInitialized(Analysis* analysis);
    
  // adds the given lattice, organizing it under the given analysis and lattice name
  //void addLattice(Analysis* analysis, int latticeName, Lattice* l);
  
  
  // Set this node's lattices for this analysis (possibly above or below only, replacing previous mappings).
  // The lattices will be associated with the NULL edge
  void setLattices    (Analysis* analysis, std::vector<Lattice*>& lattices);
  void setLatticeAbove(Analysis* analysis, std::vector<Lattice*>& lattices);
  void setLatticeBelow(Analysis* analysis, std::vector<Lattice*>& lattices);
  
  // Set this node's lattices for this analysis, along the given departing edge
  void setLatticeAbove(Analysis* analysis, PartEdgePtr departEdge, std::vector<Lattice*>& lattices);
  void setLatticeBelow(Analysis* analysis, PartEdgePtr departEdge, std::vector<Lattice*>& lattices);
  
  // Returns the given lattice above the node from the given analysis along the NULL edge
  Lattice* getLatticeAbove(Analysis* analysis, int latticeName) const;
  // Returns the given lattice below the node from the given analysis along the NULL edge
  Lattice* getLatticeBelow(Analysis* analysis, int latticeName) const;
  
  // Returns the given lattice above the node from the given analysis along the given departing edge
  Lattice* getLatticeAbove(Analysis* analysis, PartEdgePtr departEdge, int latticeName) const;
  // Returns the given lattice below the node from the given analysis along the given departing edge
  Lattice* getLatticeBelow(Analysis* analysis, PartEdgePtr departEdge, int latticeName) const;
  
  // Returns the map containing all the lattices above the node from the given analysis along the all edges
  // (read-only access)
  const std::map<PartEdgePtr, std::vector<Lattice*> >& getLatticeAboveAll(Analysis* analysis) const;
  // Returns the map containing all the lattices below the node from the given analysis along the all edges
  // (read-only access)
  const std::map<PartEdgePtr, std::vector<Lattice*> >& getLatticeBelowAll(Analysis* analysis) const;
  
  // Returns the map containing all the lattices above the node from the given analysis along the NULL edge
  // (read-only access)
  const std::vector<Lattice*>& getLatticeAbove(Analysis* analysis) const;
  // Returns the map containing all the lattices below the node from the given analysis along the NULL edge
  // (read-only access)
  const std::vector<Lattice*>& getLatticeBelow(Analysis* analysis) const;
  
  // Returns the map containing all the lattices above the node from the given analysis along the given departing edge
  // (read-only access)
  const std::vector<Lattice*>& getLatticeAbove(Analysis* analysis, PartEdgePtr departEdge) const;
  // Returns the map containing all the lattices below the node from the given analysis along the given departing edge
  // (read-only access)
  const std::vector<Lattice*>& getLatticeBelow(Analysis* analysis, PartEdgePtr departEdge) const;

  // Returns the map containing all the lattices above the node from the given analysis along the all edges
  // (read/write access)
  std::map<PartEdgePtr, std::vector<Lattice*> >& getLatticeAboveAllMod(Analysis* analysis);
  // Returns the map containing all the lattices below the node from the given analysis along the all edges
  // (read/write access)
  std::map<PartEdgePtr, std::vector<Lattice*> >& getLatticeBelowAllMod(Analysis* analysis);
  
  // Returns the map containing all the lattices above the node from the given analysis along the NULL edge
  // (read/write access)
  std::vector<Lattice*>& getLatticeAboveMod(Analysis* analysis);
  // Returns the map containing all the lattices above the node from the given analysis along the NULL edge
  // (read/write access)
  std::vector<Lattice*>& getLatticeBelowMod(Analysis* analysis);
  
  // Returns the map containing all the lattices above the node from the given analysis along the given departing edge
  // (read/write access)
  std::vector<Lattice*>& getLatticeAboveMod(Analysis* analysis, PartEdgePtr departEdge);
  // Returns the map containing all the lattices above the node from the given analysis along the given departing edge
  // (read/write access)
  std::vector<Lattice*>& getLatticeBelowMod(Analysis* analysis, PartEdgePtr departEdge);
  
   // Deletes all lattices above this node associated with the given analysis
  void deleteLatticeAbove(Analysis* analysis);
  
  // Deletes all lattices below this node associated with the given analysis
  void deleteLatticeBelow(Analysis* analysis);
  
  private:
  
    // General lattice setter function
  void setLattice_ex(LatticeMap& dfMap, Analysis* analysis, PartEdgePtr departEdge, 
                     std::vector<Lattice*>& lattices);
  
  // General lattice getter function
  Lattice* getLattice_ex(const LatticeMap& dfMap, 
                         Analysis* analysis, PartEdgePtr departEdge, int latticeName) const;
  
  
  // General read-only lattice vector getter function
  const std::vector<Lattice*>& getLattice_ex(const LatticeMap& dfMap, Analysis* analysis, 
                                        PartEdgePtr departEdge) const;
  
  // General read-write lattice vector getter function
  std::vector<Lattice*>& getLatticeMod_ex(LatticeMap& dfMap, Analysis* analysis, 
                                     PartEdgePtr departEdge);
  
  // Deletes all lattices above/below this node associated with the given analysis
  void delete_ex(LatticeMap& dfMap, Analysis* analysis);


  public:
  
  // Returns true if the two lattices vectors contain equivalent information and false otherwise
  static bool equivLattices(const std::vector<Lattice*>& latticesA,
                            const std::vector<Lattice*>& latticesB);
  
  // Creates a copy of all the dataflow state (Lattices and Facts) associated with
  // analysis srcA and associates this copied state with analysis tgtA.
  void cloneAnalysisState(Analysis* srcA, Analysis* tgtA);
  
  // Given a set of analyses, one of which is designated as a master, unions together the 
  // lattices associated with each of these analyses. The results are associated on each 
  // CFG node with the master analysis.
  void unionLattices(std::set<Analysis*>& unionSet, Analysis* master);
  
  // Unions the dataflow information in Lattices held by the from map into the to map
  // Returns true if this causes a change in the lattices in to and false otherwise
  static bool unionLatticeMaps(std::map<PartEdgePtr, std::vector<Lattice*> >& to, 
                        const std::map<PartEdgePtr, std::vector<Lattice*> >& from);
  
  // associates the given analysis/fact name with the given NodeFact, 
  // deleting any previous association (the previous NodeFact is freed)
  void addFact(Analysis* analysis, int factName, NodeFact* f);
  
  // associates the given analysis with the given map of fact names to NodeFacts, 
  // deleting any previous association (the previous NodeFacts are freed). This call
  // takes the actual provided facts and does not make a copy of them.
  //void setFacts(Analysis* analysis, const std::map<int, NodeFact*>& newFacts);
  void setFacts(Analysis* analysis, const std::vector<NodeFact*>& newFacts);
  
  // returns the given fact, which owned by the given analysis
  NodeFact* getFact(Analysis* analysis, int factName) const ;
  
  // returns the map of all the facts owned by the given analysis at this NodeState
  // (read-only access)
  //const std::map<int, NodeFact*>& getFacts(Analysis* analysis) const;
  const std::vector<NodeFact*>& getFacts(Analysis* analysis) const;
  
  // returns the map of all the facts owned by the given analysis at this NodeState
  // (read/write access)
  //std::map<int, NodeFact*>& getFactsMod(Analysis* analysis);
  std::vector<NodeFact*>& getFactsMod(Analysis* analysis);
  
  // removes the given fact, owned by the given analysis
  // returns true if the given fact was found and removed and false if it was not found
  //bool removeFact(Analysis* analysis, int factName);
  
  // deletes all facts at this node associated with the given analysis
  void deleteFacts(Analysis* analysis);
  
  // delete all state at this node associated with the given analysis
  void deleteState(Analysis* analysis);
  
  // ====== STATIC ======
  private:
  static std::map<ComposedAnalysis*, std::map<PartPtr, NodeState*> > nodeStateMap;
  
  public:
  // Returns the NodeState object associated with the given Part from the given analysis.
  static NodeState* getNodeState(ComposedAnalysis* analysis, PartPtr p);
  
  public:
  
  // Copies from's above lattices for analysis to to's above lattices for the same analysis, both along the NULL edge.
  static void copyLattices_aEQa(Analysis* analysis, NodeState& to, const NodeState& from);
  // Copies along the given departing edges
  static void copyLattices_aEQa(Analysis* analysis, NodeState& to,   PartEdgePtr toDepartEdge, 
                                              const NodeState& from, PartEdgePtr fromDepartEdge);
  
  // Copies from's above lattices for analysis to to's below lattices for the same analysis, both along the NULL edge.
  static void copyLattices_bEQa(Analysis* analysis, NodeState& to, const NodeState& from);
  // Copies along the given departing edges
  static void copyLattices_bEQa(Analysis* analysis, NodeState& to,   PartEdgePtr toDepartEdge, 
                                              const NodeState& from, PartEdgePtr fromDepartEdge);
  
  // Copies from's below lattices for analysis to to's below lattices for the same analysis, both along the NULL edge.
  static void copyLattices_bEQb(Analysis* analysis, NodeState& to, const NodeState& from);
  // Copies along the given departing edges
  static void copyLattices_bEQb(Analysis* analysis, NodeState& to,   PartEdgePtr toDepartEdge, 
                                              const NodeState& from, PartEdgePtr fromDepartEdge);
  
  // Copies from's below lattices for analysis to to's above lattices for the same analysis, both along the NULL edge.
  static void copyLattices_aEQb(Analysis* analysis, NodeState& to, const NodeState& from);
  // Copies along the given departing edges
  static void copyLattices_aEQb(Analysis* analysis, NodeState& to,   PartEdgePtr toDepartEdge, 
                                              const NodeState& from, PartEdgePtr fromDepartEdge);
  
  
  // Makes dfInfoTo[*] a copy of dfInfoFrom[*], ensuring that they both have the same structure
  static void copyLattices(std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfoTo,
                     const std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfoFrom);
  
  // Makes dfInfoTo[*] a copy of dfInfoFrom[*]. If dfInfoTo is not initially empty, it is cleared and its 
  // Lattices are deallocated.
  static void copyLatticesOW(std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfoTo,
                       const std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfoFrom);

  // Makes dfInfoTo[toDepartEdge] a copy of dfInfoFrom[fromDepartEdge]
  // If adjustPEdge is true, calls Lattice::setPartEdge() on the copied lattices in dfInfoTo to associate them with this edge.
  static void copyLattices(std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfoTo,   PartEdgePtr toDepartEdge, 
                     const std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfoFrom, PartEdgePtr fromDepartEdge,
                     bool adjustPEdge=false);

  // Makes dfInfoTo[toDepartEdge] a copy of dfInfoFrom[fromDepartEdge]. If dfInfoTo[toDepartEdge] is not initially empty, 
  // it is cleared and its Lattices are deallocated.
  // If adjustPEdge is true, calls Lattice::setPartEdge() on the copied lattices in dfInfoTo to associate them with this edge.
  static void copyLatticesOW(std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfoTo,   PartEdgePtr toDepartEdge,
                       const std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfoFrom, PartEdgePtr fromDepartEdge, 
                       bool adjustPEdge=false);
  
  /*public:
  void operator=(NodeState& that);*/
  public:
  std::string str(std::string indent="");
    
  std::string str(Analysis* analysis, std::string indent="");
  
  // Returns the string representation of the Lattices stored in the given map
  static std::string str(const std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo, std::string indent="");
};
}; // namespace fuse
#endif
