/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#ifndef DFANALYZER_H
#define DFANALYZER_H

#include "Labeler.h"
#include "CFAnalyzer.h"
#include "WorkListSeq.h"
#include <set>
#include <string>

namespace CodeThorn {

  using std::set;
  using std::vector;
  using std::string;

template<typename LatticeType>
class DFAnalyzer {
 public:
  void setExtremalLabels(set<Label> extremalLabels);
  DFAnalyzer();
  void initialize(SgProject*);
  void determineExtremalLabels(SgNode*);
  void run();

  // results are accessible through begin/end and iterator.
  typedef vector<LatticeType> AnalyzerData;
  typedef vector<LatticeType> ResultAccess;
  ResultAccess& getResultAccess();
  void attachResultsToAst(string);
  Labeler* getLabeler();
  VariableIdMapping* getVariableIdMapping();
 protected:
  virtual LatticeType transfer(Label label, LatticeType element);
  virtual void solve();
  VariableIdMapping _variableIdMapping;
  Labeler* _labeler;
  CFAnalyzer* _cfanalyzer;
  set<Label> _extremalLabels;
  Flow _flow;

  // following members are initialized by function initialize()
  long _numberOfLabels; 
  vector<LatticeType> _analyzerData;
  WorkListSeq<Label> _workList;
};

} // end of namespace

// template implementation code
#include "DFAnalyzer.C"

#endif
