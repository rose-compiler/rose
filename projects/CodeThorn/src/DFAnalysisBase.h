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
#include "DFAbstractSolver.h"
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
  virtual void initialize(SgProject* root, ProgramAbstractionLayer* programAbstractionLayer=nullptr);

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
  virtual void initializeSolver(bool defaultSolver = true);
  void determineExtremalLabels(SgNode* startFunRoot=0,bool onlySingleStartLabel=true);
  void run();

  vector<Lattice*>& getResultAccess();
#if 0
  void attachResultsToAst(string);
#endif
  Labeler* getLabeler();
  CFAnalysis* getCFAnalyzer();
  VariableIdMappingExtended* getVariableIdMapping();
  FunctionIdMapping* getFunctionIdMapping();
  Flow* getFlow() const;
  virtual Lattice* getPreInfo(Label lab);
  virtual Lattice* getPostInfo(Label lab);
  void attachInInfoToAst(string attributeName);
  void attachOutInfoToAst(string attributeName);

  void attachInfoToAst(string attributeName,bool inInfo);
  void setSolverTrace(bool trace) { _solver->setTrace(trace); }

  // optional: allows to set a pointer analysis (if not set, then the default behavior is used (everything is modified through any pointer)).
  void setPointerAnalysis(CodeThorn::PointerAnalysisInterface* pa);
  CodeThorn::PointerAnalysisInterface* getPointerAnalysis();
  void setSkipUnknownFunctionCalls(bool defer);
  ProgramAbstractionLayer* getProgramAbstractionLayer() { return _programAbstractionLayer; }

 protected:
  enum AnalysisType {FORWARD_ANALYSIS, BACKWARD_ANALYSIS};
  virtual void solve();
  ProgramAbstractionLayer* _programAbstractionLayer=nullptr;
  LabelSet _extremalLabels;
  // following members are initialized by function initialize()
  Flow* _flow=nullptr;
  long _numberOfLabels=0;
  vector<Lattice*> _analyzerDataPreInfo;
  vector<Lattice*> _analyzerDataPostInfo;
  WorkListSeq<Edge> _workList;
  void setInitialElementFactory(PropertyStateFactory*);
  PropertyStateFactory* getInitialElementFactory();

#if 0
  typedef vector<Lattice*>::iterator iterator;
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
  DFAbstractSolver* _solver=nullptr;
  AnalysisType _analysisType=DFAnalysisBase::FORWARD_ANALYSIS;
  bool _no_topological_sort=false;
 private:
  PointerAnalysisInterface* _pointerAnalysisInterface=nullptr;
  PointerAnalysisEmptyImplementation* _pointerAnalysisEmptyImplementation=nullptr;
  Lattice* _globalVariablesState=nullptr;
  bool _skipSelectedFunctionCalls=false;
  bool _programAbstractionLayerOwner=true;
};

} // end of namespace

#endif
