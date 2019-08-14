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
#include "ProgramAbstractionLayer.h"

namespace CodeThorn {

  using std::set;
  using std::vector;
  using std::string;

#include "PropertyState.h"

class DFAnalysisBase {
 public:
  DFAnalysisBase();
  virtual ~DFAnalysisBase();
  void setExtremalLabels(LabelSet extremalLabels);
  virtual void initializeExtremalValue(Lattice* element);

  // \todo maybe split into initialize(root,variableIdForEachArrayElement)
  //       and initialize(ProgramAbstractionLayer*).
  virtual void initialize(SgProject* root, ProgramAbstractionLayer* programAbstractionLayer=nullptr, bool variableIdForEachArrayElement = false);

  void setForwardAnalysis();
  void setBackwardAnalysis();
  bool isForwardAnalysis();
  bool isBackwardAnalysis();
  bool getNoTopologicalSort();
  void setNoTopologicalSort(bool);
  // computes state for global variable initializations
  virtual Lattice* initializeGlobalVariables(SgProject* root);
  // initializes an element with the combined global initialization state and the extremal value
  virtual void initializeTransferFunctions();
  virtual void initializeSolver();
  void determineExtremalLabels(SgNode* startFunRoot=0,bool onlySingleStartLabel=true);
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
  FunctionIdMapping* getFunctionIdMapping();
  Flow* getFlow() const { return _flow; }
  Lattice* getPreInfo(Label lab);
  Lattice* getPostInfo(Label lab);
  void attachInInfoToAst(string attributeName);
  void attachOutInfoToAst(string attributeName);

  void attachInfoToAst(string attributeName,bool inInfo);
  void setSolverTrace(bool trace) { _solver->setTrace(trace); }

  // optional: allows to set a pointer analysis (if not set the default behavior is used (everything is modified through any pointer)).
  void setPointerAnalysis(CodeThorn::PointerAnalysisInterface* pa);
  CodeThorn::PointerAnalysisInterface* getPointerAnalysis();
  void setSkipSelectedFunctionCalls(bool defer);
  ProgramAbstractionLayer* getProgramAbstractionLayer() { return _programAbstractionLayer; }

 protected:
  enum AnalysisType {FORWARD_ANALYSIS, BACKWARD_ANALYSIS};
  virtual void solve();
  ProgramAbstractionLayer* _programAbstractionLayer=nullptr;
  LabelSet _extremalLabels;
  Flow* _flow;
  // following members are initialized by function initialize()
  long _numberOfLabels=0;
  vector<Lattice*> _analyzerDataPreInfo;
  vector<Lattice*> _analyzerDataPostInfo;
  WorkListSeq<Edge> _workList;
  void setInitialElementFactory(PropertyStateFactory*);
  PropertyStateFactory* getInitialElementFactory();

  //typedef AnalyzerData::iterator iterator;
  typedef AnalyzerData::iterator iterator;
#if 0
  iterator begin();
  iterator end();
  size_t size();
#endif
 protected:
  virtual DFAstAttribute* createDFAstAttribute(Lattice*);
  void computeAllPreInfo();
  void computeAllPostInfo();
  bool _preInfoIsValid=false;
  bool _postInfoIsValid=false;
 public:
  DFTransferFunctions* _transferFunctions=nullptr;
 protected:
  CodeThorn::PASolver1* _solver=nullptr;
  AnalysisType _analysisType=DFAnalysisBase::FORWARD_ANALYSIS;
  bool _no_topological_sort=false;
 private:
  CodeThorn::PointerAnalysisInterface* _pointerAnalysisInterface=nullptr;
  CodeThorn::PointerAnalysisEmptyImplementation* _pointerAnalysisEmptyImplementation=nullptr;
  Lattice* _globalVariablesState=nullptr;
  bool _skipSelectedFunctionCalls=false;
  bool _programAbstractionLayerOwner=true;
};

} // end of namespace

#endif
