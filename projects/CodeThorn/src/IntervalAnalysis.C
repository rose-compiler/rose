// Author: Markus Schordan, 2013.

#include "sage3basic.h"
#include "CollectionOperators.h"
#include "IntervalAnalysis.h"
#include "IntervalAstAttribute.h"
#include "IntervalTransferFunctions.h"
#include "IntervalPropertyState.h"
#include "IntervalPropertyStateFactory.h"

using namespace std;

CodeThorn::IntervalAnalysis::IntervalAnalysis() {
  _transferFunctions=new CodeThorn::IntervalTransferFunctions();
  _transferFunctions->setInitialElementFactory(new CodeThorn::IntervalPropertyStateFactory());
}

CodeThorn::IntervalAnalysis::~IntervalAnalysis() {
  delete _transferFunctions;
  delete _transferFunctions->getInitialElementFactory();
}

DFAstAttribute* CodeThorn::IntervalAnalysis::createDFAstAttribute(Lattice* elem) {
  CodeThorn::IntervalPropertyState* ivElem=dynamic_cast<CodeThorn::IntervalPropertyState*>(elem);
  ROSE_ASSERT(ivElem);
  return new IntervalAstAttribute(ivElem);
}

void CodeThorn::IntervalAnalysis::initializeTransferFunctions() {
   DFAnalysisBase::initializeTransferFunctions();
  _numberIntervalLattice=new NumberIntervalLattice();
  ROSE_ASSERT(_numberIntervalLattice);
  CodeThorn::IntervalTransferFunctions* intervalTransferFunctions=dynamic_cast<CodeThorn::IntervalTransferFunctions*>(_transferFunctions);
  ROSE_ASSERT(intervalTransferFunctions->getCppExprEvaluator()==0);
  CodeThorn::CppExprEvaluator* cppExprEvaluator=new CodeThorn::CppExprEvaluator(_numberIntervalLattice,getVariableIdMapping());
  cppExprEvaluator->setShowWarnings(false);
  cppExprEvaluator->setPointerAnalysis(getPointerAnalysis());
  cppExprEvaluator->setSoundness(false);
  intervalTransferFunctions->setCppExprEvaluator(cppExprEvaluator);
 }
