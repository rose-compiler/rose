/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#ifndef DFANALYSISBASE_H
#define DFANALYSISBASE_H

#include <set>
#include <string>
#include "Labeler.h"
#include "CFAnalysis.h"
#include "WorkListSeq.h"
#include "CollectionOperators.h"
#include "DFTransferFunctions.h"
#include "PASolver1.h"
#include "DFAstAttribute.h"
#include "PointerAnalysisInterface.h"

namespace SPRAY {

  using std::set;
  using std::vector;
  using std::string;

#include "PropertyState.h"

class DFAnalysisBase {
 public:
  /* normalizes a C++ program to a C++ subset. The transformed program is a C++ program, but with additional
     temporary variables and additional statements. E.g. "f(g())" becomes "int t=g(); f(t);"
  */
  static void normalizeProgram(SgProject*);
  DFAnalysisBase();
  virtual ~DFAnalysisBase();
  void setExtremalLabels(set<Label> extremalLabels);
  void initialize(SgProject*);
  void setForwardAnalysis();
  void setBackwardAnalysis();
  bool isForwardAnalysis();
  bool isBackwardAnalysis();
  // computes state for global variable initializations
  virtual Lattice* initializeGlobalVariables(SgProject* root);
  // initializes an element with the combined global initialization state and the extremal value
  virtual void initializeExtremalValue(Lattice* element);
  virtual void initializeTransferFunctions();
  virtual void initializeSolver();
  void determineExtremalLabels(SgNode*);
  void run();
  //virtual PropertyState* createPropertyState();

  // results are accessible through begin/end and iterator.
  typedef vector<Lattice*> AnalyzerData;
  typedef vector<Lattice*> ResultAccess;
  ResultAccess& getResultAccess();
#if 0
  void attachResultsToAst(string);
#endif
  Labeler* getLabeler();
  CFAnalysis* getCFAnalyzer();
  VariableIdMapping* getVariableIdMapping();
  Flow* getFlow() { return &_flow; }
  Lattice* getPreInfo(Label lab);
  Lattice* getPostInfo(Label lab);
  void attachInInfoToAst(string attributeName);
  void attachOutInfoToAst(string attributeName);

  void attachInfoToAst(string attributeName,bool inInfo);
  void setSolverTrace(bool trace) { _solver->setTrace(trace); }

 protected:

  enum AnalysisType {FORWARD_ANALYSIS, BACKWARD_ANALYSIS};
  virtual void solve();
  VariableIdMapping _variableIdMapping;
  Labeler* _labeler;
  CFAnalysis* _cfanalyzer;
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
 public:
  DFTransferFunctions* _transferFunctions;
 protected:
  PropertyStateFactory* _initialElementFactory;
  SPRAY::PASolver1* _solver;
  AnalysisType _analysisType;
 private:
  SPRAY::PointerAnalysisInterface* _pointerAnalysisInterface;
  SPRAY::PointerAnalysisEmptyImplementation* _pointerAnalysisEmptyImplementation;
  Lattice* _globalVariablesState;
};

} // end of namespace

#endif
