/******************************************
 * Category: DFA
 * Liveness Analysis Declaration
 * created by tps in Nov 2008
 *****************************************/

#ifndef __LivenessAnalysis_HXX_LOADED__
#define __LivenessAnalysis_HXX_LOADED__
#include <string>

#include "filteredCFG.h"
#include "DefUseAnalysis.h"
#include "support.h"

#include "DefUseAnalysisAbstract.h"

class LivenessAnalysis : public DefUseAnalysisAbstract {
 private:
  SgProject* project;
  bool DEBUG_MODE;

  SgNode* searchNode;
  void printInAndOut(SgNode* sgNode);

  bool abort;
  int counter;
  std::map<SgNode*, std::vector<SgInitializedName*> > in;
  std::map<SgNode*, std::vector<SgInitializedName*> > out;

  int nrOfNodesVisitedPF;
  //  std::map<SgNode*,int> breakPointForWhile;
  //std::set<SgNode*> breakPointForWhileNode;
  int breakPointForWhile;
  SgNode* breakPointForWhileNode;

  template <typename T> bool defuse(T cfgNode, bool *unhandled);
  std::map<SgNode*,int> visited;
  template <typename T> bool hasANodeAboveCurrentChanged(T source);

  template <class T> void getAllNodesBelow(T source,
                                           std::vector<T>& visited);
  template <class T> void getAllNodesAbove(T source, std::vector<T>& visited);
  bool forwardAlgo;

 public:
 LivenessAnalysis(bool debug, DefUseAnalysis* dfa_p){
   dfa=dfa_p;
   DEBUG_MODE=debug;
   DEBUG_MODE_EXTRA=false;
   breakPointForWhile=0;
   breakPointForWhileNode=NULL;

  };
  virtual ~LivenessAnalysis() {}

  SgFunctionDefinition* getFunction(SgNode* node);
  int getNumberOfNodesVisited();
  // Run liveness analysis on the entire project
  //bool run(bool debug=false);
  // Run liveness analysis for a single function
  FilteredCFGNode < IsDFAFilter > run(SgFunctionDefinition* function, bool& abortme);
  std::vector<SgInitializedName*> getIn(SgNode* sgNode) { return in[sgNode];}
  std::vector<SgInitializedName*> getOut(SgNode* sgNode) { return out[sgNode];}
  int getVisited(SgNode* n) {return visited[n];}

  void setIn(SgNode* sgNode, std::vector<SgInitializedName*> vec) { in[sgNode]= vec;}
  void setOut(SgNode* sgNode, std::vector<SgInitializedName*> vec ) { out[sgNode]=vec;}


  // used by ASTTraversals
  template <class T> T merge_no_dups( T& v1,  T& v2);
  void fixupStatementsINOUT(SgFunctionDefinition* funcDecl);
};

#endif
