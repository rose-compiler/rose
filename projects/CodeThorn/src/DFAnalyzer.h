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
#include "DFSolver1.h"
#include "DFAstAttribute.h"

namespace CodeThorn {

  using std::set;
  using std::vector;
  using std::string;

template<typename LatticeType>
class DFAnalyzer {
 public:
  DFAnalyzer();
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
#if 0
  void attachResultsToAst(string);
#endif
  Labeler* getLabeler();
  CFAnalyzer* getCFAnalyzer();
  VariableIdMapping* getVariableIdMapping();
  Flow* getFlow() { return &_flow; }
  LatticeType getPreInfo(Label lab);
  LatticeType getPostInfo(Label lab);
  void attachInInfoToAst(string attributeName);
  void attachOutInfoToAst(string attributeName);

  void attachInfoToAst(string attributeName,bool inInfo);
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


  //typedef AnalyzerData::iterator iterator;
  typedef typename AnalyzerData::iterator iterator;
#if 0
  iterator begin();
  iterator end();
  size_t size();
#endif

 protected:
  virtual DFAstAttribute* createDFAstAttribute(LatticeType*);
  bool _preInfoIsValid;
  void computeAllPreInfo();
  DFTransferFunctions<LatticeType>* _transferFunctions;
 private:
  void computePreInfo(Label lab,LatticeType& info);
};

} // end of namespace

// template implementation code
#include "DFAnalyzer.C"

#endif
