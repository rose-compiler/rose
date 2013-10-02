#pragma once 

#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CallGraphTraverse.h"
#include "analysis.h"
#include "latticeFull.h"


extern int divAnalysisDebugLevel;

namespace fuse {
/***********************
 *** SaveDotAnalysis ***
 ***********************/

// A dummy context class used for Abstract Transition System implementations that don't provide context information
class DummyContext: public PartContext
{
  public:
  DummyContext() {}
    
  // Returns a list of PartContextPtr objects that denote more detailed context information about
  // this PartContext's internal contexts. If there aren't any, the function may just return a list containing
  // this PartContext itself.
  std::list<PartContextPtr> getSubPartContexts() const;
  
  bool operator==(const PartContextPtr& that) const;
  bool operator< (const PartContextPtr& that) const;
  
  std::string str(std::string indent="");
};
typedef CompSharedPtr<DummyContext> DummyContextPtr;


class Ctxt2PartsMap_Leaf;
// Class that generates instances of the appropriate sub-type of Ctxt2PartsMap_Leaf
class Ctxt2PartsMap_Leaf_Generator {
  public:
  virtual Ctxt2PartsMap_Leaf* newLeaf() const=0;
};
typedef boost::shared_ptr<Ctxt2PartsMap_Leaf_Generator> Ctxt2PartsMap_Leaf_GeneratorPtr;


// Contains information required to visualize an individual Part
class partDotInfo {
  public:
  int partID;
  partDotInfo(int partID) : partID(partID) {}
  virtual ~partDotInfo() {}
};
typedef boost::shared_ptr<partDotInfo> partDotInfoPtr;

class Ctxt2PartsMap : public dbglog::printable {
  protected:
  std::map<PartContextPtr, Ctxt2PartsMap* > m;
  Ctxt2PartsMap_Leaf* l;
  Ctxt2PartsMap_Leaf_GeneratorPtr lgen;
  
  // True if this map crosses the boundary from the context of one analysis to the context of another,
  // which is indicated by moving onto the next element in the top-level list in the insertion key.
  bool crossAnalysisBoundary;
  public:
  Ctxt2PartsMap(bool crossAnalysisBoundary, Ctxt2PartsMap_Leaf_GeneratorPtr lgen);
  Ctxt2PartsMap(bool crossAnalysisBoundary, const std::list<std::list<PartContextPtr> >& key, PartPtr part, Ctxt2PartsMap_Leaf_GeneratorPtr lgen);
  
  // Given a key, pulls off the PartContextPtr at its very front and returns the resulting key along
  // with a flag that indicates whether the front element of the top-level list was removed in the process
  class SubKey {
    public:
    PartContextPtr front;
    std::list<std::list<PartContextPtr> > back;
    bool crossAnalysisBoundary;
  };
  static SubKey getNextSubKey(const std::list<std::list<PartContextPtr> >& key);
  
  // Returns the total number of PartContextPtr within key, across all the second-level lists
  static int getNumSubKeys(const std::list<std::list<PartContextPtr> >& key);
  
  // Stores the given PartPtr under the given Context key.
  void insert(const std::list<std::list<PartContextPtr> >& key, PartPtr part);
  // Returns the Part mapped under the given Context key
  std::set<PartPtr> get(const std::list<std::list<PartContextPtr> >& key);
  
  // Prints out the hierarchical sub-graphs of Parts denoted by this map to the given output stream
  // partInfo: maps Parts to the information required to display them in the dot graph
  // subgraphName: name of the subgraph that contains the current level in the map
  virtual void map2dot(std::ostream& o, std::map<PartPtr, partDotInfoPtr>& partInfo, std::string subgraphName="cluster", std::string indent="") const;
  
  virtual std::string str(std::string indent="");
};

class Ctxt2PartsMap_Leaf : public dbglog::printable {
  protected:
  std::map<PartContextPtr, std::set<PartPtr> > m;
  public:
  Ctxt2PartsMap_Leaf() {}
  Ctxt2PartsMap_Leaf(const std::list<std::list<PartContextPtr> >& key, PartPtr part);
  
  // Stores the given PartPtr under the given Context key.
  void insert(const std::list<std::list<PartContextPtr> >& key, PartPtr part);
  // Returns the Part mapped under the given Context key
  std::set<PartPtr> get(const std::list<std::list<PartContextPtr> >& key);
  
  // Prints out the hierarchical sub-graphs of Parts denoted by this map to the given output stream
  // partInfo: maps Parts to the information required to display them in the dot graph
  // subgraphName: name of the subgraph that contains the current level in the map
  virtual void map2dot(std::ostream& o, std::map<PartPtr, partDotInfoPtr>& partInfo, std::string subgraphName="cluster", std::string indent="") const;
  
  virtual std::string str(std::string indent="");
};

// Returns an empty Ctxt2PartsMap that can support keys of this length
//Ctxt2PartsMap* createC2PMap(const std::list<std::list<PartContextPtr> >& key);

std::ostream & ats2dot(std::ostream &o, std::string graphName, std::set<PartPtr>& startParts, std::set<PartPtr>& endParts);
std::ostream & ats2dot_bw(std::ostream &o, std::string graphName, std::set<PartPtr>& startParts, std::set<PartPtr>& endParts);
void ats2dot(std::string fName, std::string graphName, std::set<PartPtr>& startParts, std::set<PartPtr>& endParts);
void ats2dot_bw(std::string fName, std::string graphName, std::set<PartPtr>& startParts, std::set<PartPtr>& endParts);
  
}; // namespace fuse
