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
#include "CollectionOperators.h"
#include "DFTransferFunctions.hpp"
#include <set>
#include <string>

namespace CodeThorn {

  using std::set;
  using std::vector;
  using std::string;

template<typename LatticeType>
class DFAnalyzer {
 public:
  DFAnalyzer();
  enum SolverMode { SOLVERMODE_STANDARD, SOLVERMODE_DYNAMICLOOPFIXPOINTS };
  void setExtremalLabels(set<Label> extremalLabels);
  void initialize(SgProject*);
  virtual LatticeType initializeGlobalVariables(SgProject* root);
  virtual void initializeTransferFunctions();
  void determineExtremalLabels(SgNode*);
  void run();

  // results are accessible through begin/end and iterator.
  typedef vector<LatticeType> AnalyzerData;
  typedef vector<LatticeType> ResultAccess;
  ResultAccess& getResultAccess();
  void attachResultsToAst(string);
  Labeler* getLabeler();
  CFAnalyzer* getCFAnalyzer();
  VariableIdMapping* getVariableIdMapping();
  Flow* getFlow() { return &_flow; }
  void setSolverMode(SolverMode);
  LatticeType getPreInfo(Label lab);
  LatticeType getPostInfo(Label lab);
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
  vector<LatticeType> _analyzerDataPreInfo;
  vector<LatticeType> _analyzerData;
  WorkListSeq<Label> _workList;
  LatticeType _initialElement;
 protected:
  bool _preInfoIsValid;
  void computeAllPreInfo();
  DFTransferFunctions<LatticeType>* _transferFunctions;
 private:
  void solveAlgorithm1();
  void solveAlgorithm2();
  void computePreInfo(Label lab,LatticeType& info);
  SolverMode _solverMode;
};

} // end of namespace

// template implementation code
#include "DFAnalyzer.C"

#endif
