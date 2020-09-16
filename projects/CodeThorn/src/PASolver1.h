#ifndef PASOLVER1_H
#define PASOLVER1_H

#include "Labeler.h"
#include "CFAnalysis.h"
#include "WorkListSeq.h"
#include "Flow.h"
#include "Lattice.h"
#include "PropertyState.h"
#include "DFTransferFunctions.h"
#include "DFAbstractSolver.h"

#include <vector>

namespace CodeThorn {

class PASolver1 : public DFAbstractSolver {
 public:
  PASolver1(WorkListSeq<Edge>& workList,
	   std::vector<Lattice*>& analyzerDataPreInfo,
	   std::vector<Lattice*>& analyzerDataPostInfo,
	   PropertyStateFactory& initialElementFactory,
	   Flow& flow,
	   DFTransferFunctions& transferFunctions
	   );
  
  void runSolver() ROSE_OVERRIDE;
  void computeCombinedPreInfo(Label lab,Lattice& inInfo) ROSE_OVERRIDE;
  
  void setLabeler(Labeler& labeler) { l = &labeler; }
  Labeler getLabeler() 
  { 
    ROSE_ASSERT(l);
    return *l; 
  }
  
  void computePostInfo(Label lab,Lattice& inInfo);

 protected:
  WorkListSeq<Edge>& _workList;
  std::vector<Lattice*>& _analyzerDataPreInfo;
  std::vector<Lattice*>& _analyzerDataPostInfo;
  PropertyStateFactory& _initialElementFactory;
  Flow& _flow;
  DFTransferFunctions& _transferFunctions;
  Labeler* l;
 public:
  void setTrace(bool trace) ROSE_OVERRIDE { _trace=trace; }
 private:
  bool _trace;
};

}

#endif
