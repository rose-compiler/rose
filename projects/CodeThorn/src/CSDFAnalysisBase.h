/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#ifndef CSDFANALYSISBASE_H
#define CSDFANALYSISBASE_H

#include <set>
#include <string>

#include "Labeler.h"
#include "CFAnalysis.h"
#include "DFAnalysisBaseWithoutData.h"
#include "CollectionOperators.h"
#include "DFTransferFunctions.h"
#include "DFAbstractSolver.h"
#include "PointerAnalysisInterface.h"
#include "ProgramAbstractionLayer.h"
#include "CallString.h"

namespace CodeThorn {

  typedef CallString Context;

#include "PropertyState.h"

  class CSDFAnalysisBase : public DFAnalysisBaseWithoutData {
  public:  
    CSDFAnalysisBase();
    virtual ~CSDFAnalysisBase();

    virtual Lattice* getPreInfo(Label lab, Context context);
    virtual Lattice* getPostInfo(Label lab, Context context);
    virtual void setPreInfo(Label lab, Context context, Lattice*);
    virtual void setPostInfo(Label lab, Context context, Lattice*);
  
    void initialize(CodeThornOptions& ctOpt, SgProject* root, ProgramAbstractionLayer* programAbstractionLayer) override;

    void run();

    void setSolverTrace(bool trace) { _solver->setTrace(trace); }
 
    WorkListSeq<Edge>* getWorkList();
    virtual void initializeSolver();

 protected:
    virtual void initializeAnalyzerDataInfo();
    virtual void computeAllPreInfo();
    virtual void computeAllPostInfo();
    virtual void solve();

    bool _preInfoIsValid=false;
    bool _postInfoIsValid=false;

    std::vector<Lattice*> _analyzerDataPreInfo;
    std::vector<Lattice*> _analyzerDataPostInfo;
    WorkListSeq<Edge> _workList;

 private:
  };

} // end of namespace

#endif
