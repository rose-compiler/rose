/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include "sage3basic.h"

#include <iostream>
#include <string>
#include <sstream>

#include "PAFAnalysisBase.h"
#include "AstUtility.h"
#include "ExtractFunctionArguments.h"
#include "FunctionNormalization.h"
#include "PASolver1.h" 
#include "AstAnnotator.h"

using namespace std;

namespace CodeThorn 
{

  PAFAnalysisBase::PAFAnalysisBase()
  {
    // all data member initializers are specified in class definition
  }

  PAFAnalysisBase::~PAFAnalysisBase() {
    if(_pointerAnalysisEmptyImplementation)
      delete _pointerAnalysisEmptyImplementation;
    if(_programAbstractionLayer && _programAbstractionLayerOwner)
      delete _programAbstractionLayer;
  }

  Flow* PAFAnalysisBase::getFlow() const {
    return _flow;
  }

  void PAFAnalysisBase::computeAllPreInfo() {
    if(!_preInfoIsValid) {
      _solver->runSolver();
      _preInfoIsValid=true;
      _postInfoIsValid=false;
    }
  }

  void PAFAnalysisBase::computeAllPostInfo() {
    if(!_postInfoIsValid) {
      computeAllPreInfo();
      // compute set of used labels in ICFG.
      for(Labeler::iterator i=getLabeler()->begin();i!=getLabeler()->end();++i) {
        Label lab=*i;
        Lattice* info=getInitialElementFactory()->create();
        _solver->computeCombinedPreInfo(lab,*info);
        // TODO: invoke edge-based transfer function for each edge and
        // (i) combine results or (ii) provide set of results (one
        // result per edge)
        _transferFunctions->transfer(lab,*info);
        setPostInfo(lab.getId(),info);
      }
      _postInfoIsValid=true;
    }
  }

  PropertyStateFactory*
  PAFAnalysisBase::getInitialElementFactory() {
    ROSE_ASSERT(_transferFunctions);
    return _transferFunctions->getInitialElementFactory();
  }

  void PAFAnalysisBase::setInitialElementFactory(PropertyStateFactory* pf) {
    ROSE_ASSERT(_transferFunctions);
    _transferFunctions->setInitialElementFactory(pf);
  }

  void PAFAnalysisBase::setExtremalLabels(LabelSet extremalLabels) {
    _extremalLabels=extremalLabels;
  }

  void PAFAnalysisBase::setForwardAnalysis() {
    _analysisType=PAFAnalysisBase::FORWARD_ANALYSIS;
  }

  void PAFAnalysisBase::setBackwardAnalysis() {
    _analysisType=PAFAnalysisBase::BACKWARD_ANALYSIS;
  }

  bool PAFAnalysisBase::isForwardAnalysis() {
    return _analysisType==PAFAnalysisBase::FORWARD_ANALYSIS;
  }

  bool PAFAnalysisBase::isBackwardAnalysis() {
    return _analysisType==PAFAnalysisBase::BACKWARD_ANALYSIS;
  }

  bool PAFAnalysisBase::getTopologicalSort() {
    return !_no_topological_sort;
  }

  void PAFAnalysisBase::setTopologicalSort(bool topological_sort) {
    _no_topological_sort = !topological_sort;
  }

  // deprecated
  bool PAFAnalysisBase::getNoTopologicalSort() {
    return _no_topological_sort;
  }

  // deprecated
  void PAFAnalysisBase::setNoTopologicalSort(bool no_topological_sort) {
    _no_topological_sort = no_topological_sort;
  }

  void PAFAnalysisBase::initializeExtremalValue(Lattice* element) {
    ROSE_ASSERT(_transferFunctions);
    _transferFunctions->initializeExtremalValue(*element);
  }

  Lattice* PAFAnalysisBase::initializeGlobalVariables(SgProject* root) {
    ROSE_ASSERT(_transferFunctions);
    Lattice* elem=_transferFunctions->initializeGlobalVariables(root);
    _globalVariablesState=elem;
    return elem;
  }

  // runs until worklist is empty
  void
  PAFAnalysisBase::solve() {
    computeAllPreInfo();
    computeAllPostInfo();
  }

  void
  PAFAnalysisBase::initialize(SgProject* root) {
    this->initialize(root,nullptr);
  }

  void
  PAFAnalysisBase::initialize(SgProject* root, ProgramAbstractionLayer* programAbstractionLayer) {
    cout << "INIT: establishing program abstraction layer." << endl;
    if(programAbstractionLayer) {
      ROSE_ASSERT(_programAbstractionLayer==nullptr);
      _programAbstractionLayer=programAbstractionLayer;
      _programAbstractionLayerOwner=false;
    } else {
      _programAbstractionLayer=new ProgramAbstractionLayer();
      _programAbstractionLayerOwner=true;
      _programAbstractionLayer->initialize(root);
    }
    _pointerAnalysisEmptyImplementation=new PointerAnalysisEmptyImplementation(getVariableIdMapping());
    _pointerAnalysisEmptyImplementation->initialize();
    _pointerAnalysisEmptyImplementation->run();
    cout << "INIT: Creating CFAnalysis."<<endl;

    // PP (07/15/19) moved flow generation to ProgramAbstractionLayer
    cout << "INIT: Requesting CFG."<<endl;
    _flow = _programAbstractionLayer->getFlow(isBackwardAnalysis());

    initializeAnalyzerDataInfo();
    cout << "INIT: initialized pre/post property states."<<endl;
    initializeSolver();
    cout << "STATUS: initialized solver."<<endl;
  }

  void PAFAnalysisBase::initializeTransferFunctions() {
    ROSE_ASSERT(_transferFunctions);
    ROSE_ASSERT(getLabeler());
    _transferFunctions->setProgramAbstractionLayer(_programAbstractionLayer);
    if(_pointerAnalysisInterface==0)
      _transferFunctions->setPointerAnalysis(_pointerAnalysisEmptyImplementation);
    else
      _transferFunctions->setPointerAnalysis(_pointerAnalysisInterface);
    _transferFunctions->addParameterPassingVariables();
  }

  void PAFAnalysisBase::setPointerAnalysis(PointerAnalysisInterface* pa) {
    _pointerAnalysisInterface=pa;
  }

  CodeThorn::PointerAnalysisInterface* PAFAnalysisBase::getPointerAnalysis() {
    return _pointerAnalysisInterface;
  }

  void
  PAFAnalysisBase::determineExtremalLabels(SgNode* startFunRoot,bool onlySingleStartLabel) {
    if(startFunRoot) {
      Labeler* labeler = getLabeler();
    
      if(isForwardAnalysis()) {
        Label startLabel=labeler->getLabel(startFunRoot);
        _extremalLabels.insert(startLabel);
      } else if(isBackwardAnalysis()) {
        if(isSgFunctionDefinition(startFunRoot)) {
          Label endLabel=labeler->functionExitLabel(startFunRoot);
          _extremalLabels.insert(endLabel);
        } else {
          cerr<<"Error: backward analysis only supported for start at function exit label."<<endl;
          exit(1);
        }
      }
    } else {
      if(!onlySingleStartLabel) {
        Labeler* labeler=getLabeler();
        long numLabels=labeler->numberOfLabels();
        // naive way of initializing all labels
        for(long i=0;i<numLabels;++i) {
          Label lab=i;
          // only add function entry labels as extremal labels
          if(isForwardAnalysis()) {
            if(labeler->isFunctionEntryLabel(i)) {
              _extremalLabels.insert(lab);
            }
          } else {
            ROSE_ASSERT(isBackwardAnalysis());
            if(labeler->isFunctionExitLabel(i)) {
              _extremalLabels.insert(lab);
            }
          }
        }
      } else {
        // keep _extremalLabels an empty set if no start function is
        // determined and only a single start label is requested.
        // _extremalLabels remains empty. Analysis will not be run.
      }
    }
    cout<<"STATUS: Number of extremal labels: "<<_extremalLabels.size()<<endl;
  }

  CFAnalysis* PAFAnalysisBase::getCFAnalyzer() {
    ROSE_ASSERT(_programAbstractionLayer);

    return _programAbstractionLayer->getCFAnalyzer();
  }

  Labeler* PAFAnalysisBase::getLabeler() const {
    return _programAbstractionLayer->getLabeler();
  }

  VariableIdMappingExtended* PAFAnalysisBase::getVariableIdMapping() {
    return _programAbstractionLayer->getVariableIdMapping();
  }

  FunctionIdMapping* PAFAnalysisBase::getFunctionIdMapping() {
    return _programAbstractionLayer->getFunctionIdMapping();
  }

}
