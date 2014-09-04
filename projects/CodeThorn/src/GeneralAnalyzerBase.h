/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#ifndef GENERALANALYZERBASE_H
#define GENERALANALYZERBASE_H

#include "Labeler.h"
#include "CFAnalyzer.h"
#include "WorkListSeq.h"
#include "CollectionOperators.h"
#include <set>
#include <string>

namespace CodeThorn {

  using std::set;
  using std::vector;
  using std::string;

class PropertyState {
 public:
  virtual void toStream(ostream& os, VariableIdMapping* vim=0) {}
  virtual bool approximatedBy(PropertyState* other) { return false; }
  virtual void combine(PropertyState* other){}
};

class GeneralAnalyzerBase {
 public:
  GeneralAnalyzerBase();
  enum SolverMode { SOLVERMODE_STANDARD, SOLVERMODE_DYNAMICLOOPFIXPOINTS };
  void setExtremalLabels(set<Label> extremalLabels);
  void initialize(SgProject*);
  virtual PropertyState* initializeGlobalVariables(SgProject* root);
  void determineExtremalLabels(SgNode*);
  void run();

  // results are accessible through begin/end and iterator.
  typedef vector<PropertyState*> AnalyzerData;
  typedef vector<PropertyState*> ResultAccess;
  ResultAccess& getResultAccess();
  void attachResultsToAst(string);
  Labeler* getLabeler();
  CFAnalyzer* getCFAnalyzer();
  VariableIdMapping* getVariableIdMapping();
  Flow* getFlow() { return &_flow; }
  void setSolverMode(SolverMode);
  PropertyState* getPreInfo(Label lab);
  PropertyState* getPostInfo(Label lab);
 protected:
  virtual PropertyState* transfer(Label label, PropertyState* element);
  virtual void solve();
  VariableIdMapping _variableIdMapping;
  Labeler* _labeler;
  CFAnalyzer* _cfanalyzer;
  set<Label> _extremalLabels;
  Flow _flow;
  // following members are initialized by function initialize()
  long _numberOfLabels; 
  vector<PropertyState*> _analyzerDataPreInfo;
  vector<PropertyState*> _analyzerData;
  WorkListSeq<Label> _workList;
  PropertyState* _initialElement;
 protected:
  bool _preInfoIsValid;
  void computeAllPreInfo();
 private:
  void solveAlgorithm1();
  void solveAlgorithm2();
  void computePreInfo(Label lab,PropertyState* info);
  SolverMode _solverMode;
};

} // end of namespace

#endif
