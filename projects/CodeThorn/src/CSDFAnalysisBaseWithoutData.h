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

namespace CodeThorn {

#include "PropertyState.h"

  template<typename Context>
  class CSDFAnalysisBaseWithoutData : public DFAnalysisBaseWithoutData {
  public:  
    //CSDFAnalysisBaseWithoutData();
    //virtual ~CSDFAnalysisBaseWithoutData();

    virtual Lattice* getPreInfo(Label lab, Context context)=0;
    virtual Lattice* getPostInfo(Label lab, Context context)=0;
    virtual void setPreInfo(Label lab, Context context, Lattice*)=0;
    virtual void setPostInfo(Label lab, Context context, Lattice*)=0;
  
    //void initialize(CodeThornOptions& ctOpt, SgProject* root, ProgramAbstractionLayer* programAbstractionLayer) override;

    //void run();

    //void setSolverTrace(bool trace) { _solver->setTrace(trace); }
 
    //virtual void initializeSolver();

 protected:
    //virtual void initializeAnalyzerDataInfo();
    //virtual void computeAllPreInfo();
    //virtual void computeAllPostInfo();
    //virtual void solve();

 private:
  };

} // end of namespace

#endif
