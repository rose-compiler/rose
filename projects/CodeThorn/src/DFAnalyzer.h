/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#ifndef MFANALYSIS_H
#define MFANALYSIS_H

#include "Labeler.h"
#include "CFAnalyzer.h"
#include "WorkList.h"

namespace CodeThorn {

template<typename LatticeType>
class DFAnalyzer {
 public:
  void setExtremalLabels(set<Label> extremalLabels);
  DFAnalyzer();
  void initialize(SgProject*);
  void determineExtremalLabels(SgNode*);
  void run();
 protected:
  virtual LatticeType transfer(Label label, LatticeType element);
  virtual void solve();
  VariableIdMapping* getVariableIdMapping();
  VariableIdMapping _variableIdMapping;
  Labeler* _labeler;
  CFAnalyzer* _cfanalyzer;
  set<Label> _extremalLabels;
  Flow _flow;

  // following members are initialized by function initialize()
  long _numberOfLabels; 
  vector<LatticeType> _analyzerData;
  WorkList<Label> _workList;
};

}
#endif
