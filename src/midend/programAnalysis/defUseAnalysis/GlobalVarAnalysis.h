/******************************************
 * Category: DFA
 * GlobalVar Analysis Declaration
 * created by tps in Feb 2007
 *****************************************/

#ifndef __GlobalVarAnalysis_HXX_LOADED__
#define __GlobalVarAnalysis_HXX_LOADED__
#include <string>

#include "DefUseAnalysis.h"

class GlobalVarAnalysis {
 private:
  SgProject* project;
  DefUseAnalysis* dfa;
  bool DEBUG_MODE;

  // local functions -------------------
  bool isGlobalVar(SgInitializedName* initName);
  bool isFromLibrary(SgInitializedName* initName);

 public:
  GlobalVarAnalysis(bool debug,SgProject* proj, DefUseAnalysis* analysis) {project = proj;dfa = analysis; DEBUG_MODE=debug;};

  // def-use-public-functions -----------
  std::vector<SgInitializedName*> run();


};

#endif
