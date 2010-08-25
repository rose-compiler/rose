/****************************************** 
 * Category: DFA
 * Dataflow Analysis declaration
 * created by tps in Feb 2007
 *****************************************/

#ifndef __DFA_HXX_LOADED__
#define __DFA_HXX_LOADED__
#include <string>

class DFAnalysis
{
 public:
  DFAnalysis() {};
  virtual ~DFAnalysis() {};
  
  /** Run the DFA analysis without debugging. */
  virtual int run() = 0;
  virtual int run(bool debug) = 0;
  // request information from the DFA
  
  /** get the Definition multimap of a specific SgNode. */
  virtual std::vector < std::pair <SgInitializedName* , SgNode*> >  getDefMultiMapFor(SgNode* node)=0;

  /** get the Usage multimap of a specific SgNode. */
  virtual std::vector < std::pair <SgInitializedName* , SgNode*> >  getUseMultiMapFor(SgNode* node)=0;
  
  /** get the vector of defining nodes for a specific node and a initializedName. */
  virtual std::vector < SgNode* > getDefFor(SgNode* node, SgInitializedName* initName)=0;
  
  /** get the vector of usage nodes for a specific node and a initializedName. */
  virtual std::vector < SgNode* > getUseFor(SgNode* node, SgInitializedName* initName)=0;

  /** return whether a node is a global node */
  virtual bool isNodeGlobalVariable(SgInitializedName* node)=0;

  // used for testing and visualization
  virtual void printDefMap() = 0;
  virtual void printUseMap() = 0;
  virtual int getDefSize()=0;
  virtual int getUseSize()=0;
  virtual int getIntForSgNode(SgNode* node)=0;
  virtual void dfaToDOT()=0;

  virtual std::map< SgNode* , std::vector < std::pair <SgInitializedName* , SgNode*> > > getDefMap()=0;
  virtual std::map< SgNode* , std::vector < std::pair <SgInitializedName* , SgNode*> > > getUseMap()=0;
  virtual void setMaps(std::map< SgNode* , std::vector < std::pair <SgInitializedName* , SgNode*> > > def,
                       std::map< SgNode* , std::vector < std::pair <SgInitializedName* , SgNode*> > > use)=0;

};

#endif
