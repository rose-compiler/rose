/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#ifndef DFANALYSISBASE_H
#define DFANALYSISBASE_H

#include <set>
#include <string>

#include "Labeler.h"
#include "CFAnalysis.h"
#include "DFAnalysisBaseWithoutData.h"
#include "WorkListSeq.h"
#include "CollectionOperators.h"
#include "DFTransferFunctions.h"
#include "DFAbstractSolver.h"
#include "DFAstAttribute.h"
#include "PointerAnalysisInterface.h"
#include "ProgramAbstractionLayer.h"

namespace CodeThorn {

#include "PropertyState.h"

  class DFAnalysisBase : public DFAnalysisBaseWithoutData {
  public:  
    DFAnalysisBase();
    virtual ~DFAnalysisBase();

    virtual Lattice* getPreInfo(Label lab);
    virtual Lattice* getPostInfo(Label lab);
    virtual void setPostInfo(Label lab,Lattice*);
 
    void initialize(CodeThornOptions& ctOpt, SgProject* root, ProgramAbstractionLayer* programAbstractionLayer) override;

    void run();

    void attachInInfoToAst(string attributeName);
    void attachOutInfoToAst(string attributeName);

    void attachInfoToAst(string attributeName,bool inInfo);
    void setSolverTrace(bool trace) { _solver->setTrace(trace); }

    void setSkipUnknownFunctionCalls(bool defer);

    WorkListSeq<Edge>* getWorkList();
  
    virtual void initializeSolver();
  protected:
    virtual void initializeAnalyzerDataInfo();
    virtual void computeAllPreInfo();
    virtual void computeAllPostInfo();
    virtual void solve();

    bool _preInfoIsValid=false;
    bool _postInfoIsValid=false;

    virtual DFAstAttribute* createDFAstAttribute(Lattice*);
    std::vector<Lattice*> _analyzerDataPreInfo;
    std::vector<Lattice*> _analyzerDataPostInfo;
    WorkListSeq<Edge> _workList;
  private:
    bool _skipSelectedFunctionCalls=false;
  };

} // end of namespace

#endif
