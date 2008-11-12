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

 public:
 LivenessAnalysis(bool debug, DefUseAnalysis* dfa_p){
   dfa=dfa_p;
   DEBUG_MODE=debug;
   DEBUG_MODE_EXTRA=false;
   breakPointForWhile=0;
   breakPointForWhileNode=NULL;
   
  };
  virtual ~LivenessAnalysis() {}


  std::map<SgNode*, std::vector<SgInitializedName*> > in;
  std::map<SgNode*, std::vector<SgInitializedName*> > out;


  SgFunctionDefinition* getFunction(SgNode* node);


  int nrOfNodesVisitedPF;
  int breakPointForWhile;
  SgNode* breakPointForWhileNode;

  template <typename T> bool defuse(T cfgNode);
  FilteredCFGNode < IsDFAFilter > run(SgFunctionDefinition* function);
  int getNumberOfNodesVisited();


};

#endif
