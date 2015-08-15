// Author: Markus Schordan, 2013.

#include "sage3basic.h"
#include "CollectionOperators.h"
#include "IntervalAnalysis.h"
#include "IntervalAstAttribute.h"
#include "IntervalTransferFunctions.h"
#include "IntervalPropertyState.h"
#include "IntervalPropertyStateFactory.h"

using namespace std;

SPRAY::IntervalAnalysis::IntervalAnalysis() {
  _transferFunctions=new SPRAY::IntervalTransferFunctions();
  _initialElementFactory=new SPRAY::IntervalPropertyStateFactory();
}

SPRAY::IntervalAnalysis::~IntervalAnalysis() {
  delete _transferFunctions;
  delete _initialElementFactory;
}

void SPRAY::IntervalAnalysis::initializeExtremalValue(Lattice* element) {
  SPRAY::IntervalPropertyState* pstate=dynamic_cast<SPRAY::IntervalPropertyState*>(element);
  pstate->setEmptyState();
  //iElement->... init to empty state, not being bottom
  cout<<"INFO: initialized extremal value."<<endl;
}

DFAstAttribute* SPRAY::IntervalAnalysis::createDFAstAttribute(Lattice* elem) {
  SPRAY::IntervalPropertyState* ivElem=dynamic_cast<SPRAY::IntervalPropertyState*>(elem);
  ROSE_ASSERT(ivElem);
  return new IntervalAstAttribute(ivElem);
}

void SPRAY::IntervalAnalysis::initializeTransferFunctions() {
   DFAnalysisBase::initializeTransferFunctions();
  _numberIntervalLattice=new NumberIntervalLattice();
  ROSE_ASSERT(_numberIntervalLattice);
  SPRAY::IntervalTransferFunctions* intervalTransferFunctions=dynamic_cast<SPRAY::IntervalTransferFunctions*>(_transferFunctions);
  ROSE_ASSERT(intervalTransferFunctions->getCppExprEvaluator()==0);
  SPRAY::CppExprEvaluator* cppExprEvaluator=new SPRAY::CppExprEvaluator(_numberIntervalLattice,&_variableIdMapping);
  cppExprEvaluator->setShowWarnings(true);
  cppExprEvaluator->setPointerAnalysis(getPointerAnalysis());
  //cppExprEvaluator->setSoundness(false);
  intervalTransferFunctions->setCppExprEvaluator(cppExprEvaluator);
 }
