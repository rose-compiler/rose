#ifndef PASOLVER1_H
#define PASOLVER1_H

#include "Labeler.h"
#include "CFAnalyzer.h"
#include "WorkListSeq.h"
#include "Flow.h"
#include "Lattice.h"
#include "PropertyState.h"
#include "PATransferFunctions.h"

#include <vector>

class PASolver1 {
 public:
  PASolver1(WorkListSeq<Edge>& workList,
	   std::vector<Lattice*>& analyzerDataPreInfo,
	   std::vector<Lattice*>& analyzerDataPostInfo,
	   PropertyStateFactory& initialElementFactory,
	   Flow& flow,
	   PATransferFunctions& transferFunctions
	   );
  virtual void runSolver();

  void computePreInfo(Label lab,Lattice& inInfo);
 protected:
  WorkListSeq<Edge>& _workList;
  std::vector<Lattice*>& _analyzerDataPreInfo;
  std::vector<Lattice*>& _analyzerDataPostInfo;
  PropertyStateFactory& _initialElementFactory;
  Flow& _flow;
  PATransferFunctions& _transferFunctions;
};

#endif
