/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#ifndef MFANALYSIS_H
#define MFANALYSIS_H

#include "Labeler.h"
#include "CFAnalyzer.h"

namespace CodeThorn {

template<typename LatticeType>
class MFAnalyzer {
 public:
  void setExtremalLabels(set<Label> extremalLabels);
  MFAnalyzer(CFAnalyzer* cfanalyzer);
  void initialize();
  void run();
  virtual LatticeType transfer(Edge edge, LatticeType element);
 private:,
  Labeler* _labeler;
  CFAnalyzer* _cfanalyzer;
  Label _extremalLabels;

  // following members are initialized by function initialize()
  long _numberOfLabels; 
  vector<LatticeType> _analyzerData;
};

#include "MFAnalyzer.C"

}
#endif
