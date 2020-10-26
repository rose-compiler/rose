/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#ifndef PAFANALYSISBASE_H
#define PAFANALYSISBASE_H

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

#include "PropertyState.h"

namespace CodeThorn {

  class PAFAnalysisBase {
  public:  
    PAFAnalysisBase();
    virtual ~PAFAnalysisBase();

    // computes state for global variable initializations
    virtual Lattice* initializeGlobalVariables(SgProject* root);
    // initializes an element with the combined global initialization state and the extremal value
    virtual void initializeTransferFunctions();
    virtual void initializeSolver()=0;
    virtual void initializeExtremalValue(Lattice* element);
    virtual void initialize(SgProject* root);
    virtual void initialize(SgProject* root, ProgramAbstractionLayer* programAbstractionLayer);

    void setExtremalLabels(LabelSet extremalLabels);

    void setForwardAnalysis();
    void setBackwardAnalysis();
    bool isForwardAnalysis();
    bool isBackwardAnalysis();
    bool getTopologicalSort();
    void setTopologicalSort(bool);

    // deprecated
    bool getNoTopologicalSort();
    // deprecated
    void setNoTopologicalSort(bool);

    void determineExtremalLabels(SgNode* startFunRoot=0,bool onlySingleStartLabel=true);
    virtual void run() =0;

    virtual Labeler* getLabeler() const;
    virtual CFAnalysis* getCFAnalyzer(); 
    virtual VariableIdMappingExtended* getVariableIdMapping();
    virtual FunctionIdMapping* getFunctionIdMapping();
    virtual Flow* getFlow() const;
    virtual Lattice* getPreInfo(Label lab) =0;
    virtual Lattice* getPostInfo(Label lab) =0;

    ProgramAbstractionLayer* getProgramAbstractionLayer() { return _programAbstractionLayer; }

    // optional: allows to set a pointer analysis (if not set, then the default behavior is used (everything is modified through any pointer)).
    void setPointerAnalysis(CodeThorn::PointerAnalysisInterface* pa);
    CodeThorn::PointerAnalysisInterface* getPointerAnalysis();

  protected:
    enum AnalysisType {FORWARD_ANALYSIS, BACKWARD_ANALYSIS};
    virtual void setPostInfo(Label lab,Lattice*) =0;
    virtual void initializeAnalyzerDataInfo()=0;
    virtual void solve();
  
    bool _programAbstractionLayerOwner=true;
    ProgramAbstractionLayer* _programAbstractionLayer=nullptr;
    LabelSet _extremalLabels;
    Flow* _flow=nullptr;
    long _numberOfLabels=0;

    void setInitialElementFactory(PropertyStateFactory*);
    PropertyStateFactory* getInitialElementFactory();

    DFTransferFunctions* _transferFunctions=nullptr;

  protected:
    bool _preInfoIsValid=false;
    bool _postInfoIsValid=false;
    void computeAllPreInfo();
    void computeAllPostInfo();
    DFAbstractSolver* _solver=nullptr;
    Lattice* _globalVariablesState=nullptr;
    AnalysisType _analysisType=PAFAnalysisBase::FORWARD_ANALYSIS;
    bool _no_topological_sort=false;
    PointerAnalysisInterface* _pointerAnalysisInterface=nullptr;
    PointerAnalysisEmptyImplementation* _pointerAnalysisEmptyImplementation=nullptr;

  private:
  };

} // end of namespace

#endif
