/******************************************
 * Category: DFA
 * DefUse Analysis Declaration (per Function)
 * created by tps in Feb 2007
 *****************************************/

#ifndef __DefUseAnalysisAbstractPF_HXX_LOADED__
#define __DefUseAnalysisAbstractPF_HXX_LOADED__

//#include "rose.h"
#include "support.h"
#include <virtualCFG.h>
#include <cfgToDot.h>
#include <list>
#include "filteredCFG.h"
#include "DFAFilter.h"
#include "DefUseAnalysis.h"
#include "dfaToDot.h"
#include <string>

class DefUseAnalysisAbstract : public Support {
 protected:
  // debugging -------------------------
  bool DEBUG_MODE;
  bool DEBUG_MODE_EXTRA;

  // reference to DFA ------------------
  DefUseAnalysis* dfa;

  //  typedef std::multimap < SgInitializedName* , SgNode* > multitype;
  typedef std::vector < std::pair < SgInitializedName* , SgNode* > > multitype;
  typedef std::map< SgNode* , multitype > tabletype;
  typedef FilteredCFGEdge < IsDFAFilter > filteredCFGEdgeType;
  typedef FilteredCFGNode < IsDFAFilter > filteredCFGNodeType;

  std::set <SgNode*> doNotVisitMap;
  std::map <SgNode*, bool> nodeChangedMap;

  // searching -------------------------
  bool searchMulti(const multitype* multi, SgInitializedName* initName);
  //  bool searchMulti(const multitype* multi, SgInitializedName* initName, SgNode* node);
  bool searchVector(std::vector <filteredCFGNodeType> done, 
                    filteredCFGNodeType cfgNode);

  SgInitializedName* getInitName(SgNode* l_expr);  

  SgExpression* resolveCast(SgExpression* expr);
  bool isDoubleExactEntry(const multitype* multi, 
                          SgInitializedName* name, SgNode* sgNode);

  bool checkElementsForEquality(const multitype* t1, const multitype* t2);

  /**********************************************************
   *  For a CFG Node, follow the two incoming edges and get 
   *  the other node (than theNodeBefore)
   *********************************************************/
  template <typename T>
    SgNode* getOtherInNode(T cfgNode, SgNode* oneNode){
    SgNode* otherNode = NULL;  
    std::vector<filteredCFGEdgeType > in_edges = cfgNode.inEdges();
    for (std::vector<filteredCFGEdgeType >::const_iterator i = in_edges.begin(); i != in_edges.end(); ++i) {
      filteredCFGEdgeType filterEdge = *i;
      T filterNode = filterEdge.source();
      if (filterNode.getNode()!=oneNode)
        otherNode = filterNode.getNode();
    }  
    if (DEBUG_MODE_EXTRA)
      std::cout << "getOtherInNode:: other: " << otherNode << "  previous: " << 
        oneNode << "  size of in: " << in_edges.size() << std::endl;
    return otherNode;
  }

  /**********************************************************
   *  get the incoming node of a cfg edge
   *********************************************************/
  template <typename T>
    SgNode* getCFGPredNode(T cfgNode){
    SgNode* node = NULL;
    std::vector<filteredCFGEdgeType > in_edges = cfgNode.inEdges();
    for (std::vector<filteredCFGEdgeType >::const_iterator i = in_edges.begin(); i != in_edges.end(); ++i) {
      filteredCFGEdgeType filterEdge = *i;
      node = filterEdge.source().getNode();
    }  
    return node;
  }

  /**********************************************************
   *  print the values of a vector
   *********************************************************/
  template <typename T >
    void printCFGVector(std::vector< T > worklist) {
    int pos = 0;
    std::cout << "\n!! Worklist : " ;
    for (typename std::vector<T >::iterator i = worklist.begin(); i != worklist.end(); ++i, pos++) {  
      T node = *i;
      std::cout << " - " << node.toStringForDebugging();
    }
    std::cout << std::endl;
  }

  /*
    template <typename T> void printCFGVector(std::vector<T > worklist);
    template <typename T> SgNode* getCFGPredNode(T cfgNode);
    template <typename T> SgNode* getOtherInNode(T source, SgNode* oneNode);
  */
  
 public:
  DefUseAnalysisAbstract(){
    doNotVisitMap.clear();
    nodeChangedMap.clear();
  };
  virtual ~DefUseAnalysisAbstract(){};
  virtual FilteredCFGNode < IsDFAFilter > run(SgFunctionDefinition* function, bool& abortme)=0;

};

#endif

