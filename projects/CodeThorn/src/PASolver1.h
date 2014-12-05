#ifndef DFSOLVER1_H
#define DFSOLVER1_H

#include "Labeler.h"
#include "CFAnalyzer.h"
#include "WorkListSeq.h"
#include "Flow.h"
#include "DFTransferFunctions.hpp"

#include <vector>

template<typename LatticeType>
class DFSolver1 {
 public:
  DFSolver1(WorkListSeq<Label>& workList,
	   std::vector<LatticeType>& analyzerDataPreInfo,
	   std::vector<LatticeType>& analyzerDataPostInfo,
	   PropertyStateFactory& initialElement,
	   Flow& flow,
	   DFTransferFunctions<LatticeType>& transferFunctions
	   );
  virtual void runSolver();

 protected:
  void computePreInfo(Label lab,LatticeType& inInfo);
  WorkListSeq<Label>& _workList;
  std::vector<LatticeType>& _analyzerDataPreInfo;
  std::vector<LatticeType>& _analyzerDataPostInfo;
  LatticeType& _initialElement;
  Flow& _flow;
  DFTransferFunctions<LatticeType>& _transferFunctions;
};

#include "DFSolver1.C"

#endif
