/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#ifndef DFANALYSISBASE_H
#define DFANALYSISBASE_H

#include <set>
#include <string>

#include "Labeler.h"
#include "CFAnalysis.h"
#include "PAFAnalysisBase.h"
#include "WorkListSeq.h"
#include "CollectionOperators.h"
#include "DFTransferFunctions.h"
#include "DFAbstractSolver.h"
#include "DFAstAttribute.h"
#include "PointerAnalysisInterface.h"
#include "ProgramAbstractionLayer.h"

namespace CodeThorn {

#include "PropertyState.h"

  class DFAnalysisBase : public PAFAnalysisBase {
 public:  
  DFAnalysisBase();
  virtual ~DFAnalysisBase();
  void initializeSolver();

  void initialize(SgProject* root, ProgramAbstractionLayer* programAbstractionLayer) override;

  void initializeAnalyzerDataInfo();
  void run();

  void attachInInfoToAst(string attributeName);
  void attachOutInfoToAst(string attributeName);

  virtual void setPostInfo(Label lab,Lattice*);
  
  void attachInfoToAst(string attributeName,bool inInfo);
  void setSolverTrace(bool trace) { _solver->setTrace(trace); }

  void setSkipUnknownFunctionCalls(bool defer);

  DFTransferFunctions* _transferFunctions=nullptr;

  Lattice* getPreInfo(Label lab);
  Lattice* getPostInfo(Label lab);

  WorkListSeq<Edge>* getWorkList();
  
  protected:
  virtual DFAstAttribute* createDFAstAttribute(Lattice*);
  std::vector<Lattice*> _analyzerDataPreInfo;
  std::vector<Lattice*> _analyzerDataPostInfo;
  WorkListSeq<Edge> _workList;

 private:
  bool _skipSelectedFunctionCalls=false;
  };

} // end of namespace

#endif
