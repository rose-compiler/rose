// Author: Markus Schordan, 2013.

#include "sage3basic.h"

#include "RDAnalyzer.h"
#include "RDAstAttribute.h"
#include "CollectionOperators.h"
#include "RDTransferFunctions.hpp"

using namespace std;
using namespace CodeThorn;

RDAnalyzer::RDAnalysis() {
  _transferFunctions=new RDTransferFunctions();
}

RDAnalyzer::~RDAnalysis() {
  delete _transferFunctions;
}

void initializeSolver() {
  ROSE_ASSERT(&_worklist);
  ROSE_ASSERT(&_initialElement);
  ROSE_ASSERT(&_analyzerDataPreInfo);
  ROSE_ASSERT(&_analyzerDataPostInfo);
  ROSE_ASSERT(&_flow);
  ROSE_ASSERT(&_transferFunctions);
  _solver=new PASolver1(_workList,
                      _analyzerDataPreInfo,
                      _analyzerDataPostInfo,
                      _initialElement,
                      _flow,
                      _transferFunctions);
}

DFAstAttribute* RDAnalysis::createDFAstAttribute(Lattice* elem) {
  return new RDAstAttribute(elem);
}
