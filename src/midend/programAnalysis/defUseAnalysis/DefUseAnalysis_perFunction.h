/******************************************
 * Category: DFA
 * DefUse Analysis Declaration (per Function)
 * created by tps in Feb 2007
 *****************************************/

#ifndef __DefUseAnalysisPF_HXX_LOADED__
#define __DefUseAnalysisPF_HXX_LOADED__
#include <string>

#include "rose.h"
#include "support.h"
#include <virtualCFG.h>
#include <cfgToDot.h>
#include <list>
#include "filteredCFG.h"
#include "DFAFilter.h"
#include "DefUseAnalysis.h"
#include "dfaToDot.h"

class DefUseAnalysisPF : public Support {
 private:
  // debugging -------------------------
  bool DEBUG_MODE;
  bool DEBUG_MODE_EXTRA;

  // reference to DFA ------------------
  DefUseAnalysis* dfa;

  typedef std::multimap < SgInitializedName* , SgNode* > multitype;
  typedef std::map< SgNode* , multitype > tabletype;
  typedef FilteredCFGEdge < IsDFAFilter > filteredCFGEdgeType;
  typedef FilteredCFGNode < IsDFAFilter > filteredCFGNodeType;

  std::set <SgNode*> doNotVisitMap;
  std::map <SgNode*, bool> nodeChangedMap;

  // printing --------------------------
  template <typename T> void printCFGVector(std::vector<T > worklist);

  bool isDoubleExactEntry(const multitype* multi, 
			  SgInitializedName* name, SgNode* sgNode);
  SgExpression* resolveCast(SgExpression* expr);
  bool makeSureThatTheUseIsInTable(SgInitializedName* initName);
  bool makeSureThatTheDefIsInTable(SgInitializedName* initName);

  template <typename T>
    bool hasANodeAboveCurrentChanged(T cfgNode);

  template <typename T> 
    bool performUseAndDefinition(SgNode* sgNode, SgInitializedName* initName,
			       bool isUsage, bool isDefinition, 
			       SgNode* sgNodeBefore, bool dont_replace,
			       T cfgNode);

  // searching -------------------------
  bool searchMulti(const multitype* multi, SgInitializedName* initName);
  //  bool searchMulti(const multitype* multi, SgInitializedName* initName, SgNode* node);
  bool searchVector(std::vector <filteredCFGNodeType> done, 
		    filteredCFGNodeType cfgNode);
  SgInitializedName* getInitName(SgNode* l_expr);  

  // def-use-sepcific ------------------
  void handleDefCopy(SgNode* sgNode, int nrOfInEdges, SgNode* sgNodeBefore, 
		  filteredCFGNodeType cfgNode);
  void handleUseCopy(SgNode* sgNode, int nrOfInEdges, SgNode* sgNodeBefore, 
		  filteredCFGNodeType cfgNode);
  bool checkElementsForEquality(const multitype* t1, const multitype* t2);
  template <typename T> bool defuse(T cfgNode, bool *unhandled);
  template <typename T> SgNode* getCFGPredNode(T cfgNode);
  template <typename T> SgNode* getOtherInNode(T source, SgNode* oneNode);

  int nrOfNodesVisitedPF;
  int breakPointForWhile;
  SgNode* breakPointForWhileNode;
  
 public:
  DefUseAnalysisPF(bool debug, DefUseAnalysis* dfa_p):DEBUG_MODE(debug),
    DEBUG_MODE_EXTRA(false),dfa(dfa_p){
    breakPointForWhile=0;
    breakPointForWhileNode=NULL;
    doNotVisitMap.clear();
    nodeChangedMap.clear();
  };
  FilteredCFGNode < IsDFAFilter > run(SgFunctionDefinition* function);
  int getNumberOfNodesVisited();
};

#endif

