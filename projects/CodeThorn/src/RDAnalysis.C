// Author: Markus Schordan, 2013.

#include "sage3basic.h"

#include "RDAnalysis.h"
#include "RDAstAttribute.h"
#include "CollectionOperators.h"
#include "RDTransferFunctions2.h"

using namespace std;
using namespace CodeThorn;

RDAnalysis::RDAnalysis() {
  _transferFunctions=new RDTransferFunctions();
}

RDAnalysis::~RDAnalysis() {
  delete _transferFunctions;
}

void RDAnalysis::initializeSolver() {
  ROSE_ASSERT(&_workList);
  ROSE_ASSERT(&_initialElementFactory);
  ROSE_ASSERT(&_analyzerDataPreInfo);
  ROSE_ASSERT(&_analyzerDataPostInfo);
  ROSE_ASSERT(&_flow);
  ROSE_ASSERT(&_transferFunctions);
  _solver=new PASolver1(_workList,
                      _analyzerDataPreInfo,
                      _analyzerDataPostInfo,
                      *_initialElementFactory,
                      _flow,
                      *_transferFunctions);
}

DFAstAttribute* RDAnalysis::createDFAstAttribute(Lattice* elem) {
  RDLattice* rdElem=dynamic_cast<RDLattice*>(elem);
  ROSE_ASSERT(rdElem);
  return new RDAstAttribute(rdElem);
}
