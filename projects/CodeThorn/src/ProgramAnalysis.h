/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#ifndef PROGRAMANALYSIS_H
#define PROGRAMANALYSIS_H

#include <set>
#include <string>
#include "Labeler.h"
#include "CFAnalyzer.h"
#include "WorkListSeq.h"
#include "CollectionOperators.h"
#include "PATransferFunctions.h"
#include "PASolver1.h"
#include "DFAstAttribute.h"
#include "PointerAnalysisInterface.h"

namespace CodeThorn {

  using std::set;
  using std::vector;
  using std::string;

#include "PropertyState.h"

class ProgramAnalysis {
 public:
  ProgramAnalysis();
  virtual ~ProgramAnalysis();
  void setExtremalLabels(set<Label> extremalLabels);
  void initialize(SgProject*);
  void setForwardAnalysis();
  void setBackwardAnalysis();
  bool isForwardAnalysis();
  bool isBackwardAnalysis();
  virtual void initializeGlobalVariables(SgProject* root);
  virtual void initializeExtremalValue(Lattice* element);
  virtual void initializeTransferFunctions();
  virtual void initializeSolver();
  void determineExtremalLabels(SgNode*);
  void run();
  PropertyState* createPropertyState();

  // results are accessible through begin/end and iterator.
  typedef vector<Lattice*> AnalyzerData;
  typedef vector<Lattice*> ResultAccess;
  ResultAccess& getResultAccess();
#if 0
  void attachResultsToAst(string);
#endif
  Labeler* getLabeler();
  CFAnalyzer* getCFAnalyzer();
  VariableIdMapping* getVariableIdMapping();
  Flow* getFlow() { return &_flow; }
  Lattice* getPreInfo(Label lab);
  Lattice* getPostInfo(Label lab);
  void attachInInfoToAst(string attributeName);
  void attachOutInfoToAst(string attributeName);

  void attachInfoToAst(string attributeName,bool inInfo);

 protected:

  enum AnalysisType {FORWARD_ANALYSIS, BACKWARD_ANALYSIS};
  virtual void solve();
  VariableIdMapping _variableIdMapping;
  Labeler* _labeler;
  CFAnalyzer* _cfanalyzer;
  set<Label> _extremalLabels;
  Flow _flow;
  // following members are initialized by function initialize()
  long _numberOfLabels; 
  vector<Lattice*> _analyzerDataPreInfo;
  vector<Lattice*> _analyzerDataPostInfo;
  WorkListSeq<Edge> _workList;
  void setInitialElementFactory(PropertyStateFactory*);

  //typedef AnalyzerData::iterator iterator;
  typedef AnalyzerData::iterator iterator;
#if 0
  iterator begin();
  iterator end();
  size_t size();
#endif
  // optional: allows to set a pointer analysis (if not set the default behavior is used (everything is modified through any pointer)).
  void setPointerAnalysis(SPRAY::PointerAnalysisInterface* pa);
 protected:
  virtual DFAstAttribute* createDFAstAttribute(Lattice*);
  void computeAllPreInfo();
  void computeAllPostInfo();
  bool _preInfoIsValid;
  bool _postInfoIsValid;
  PATransferFunctions* _transferFunctions;
  PropertyStateFactory* _initialElementFactory;
  PASolver1* _solver;
  AnalysisType _analysisType;
 private:
  SPRAY::PointerAnalysisInterface* _pointerAnalysisInterface;
  SPRAY::PointerAnalysisEmptyImplementation* _pointerAnalysisEmptyImplementation;
};

} // end of namespace

#endif
