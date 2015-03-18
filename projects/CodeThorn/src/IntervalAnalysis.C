// Author: Markus Schordan, 2013.

#include "sage3basic.h"
#include "CollectionOperators.h"
#include "IntervalAnalysis.h"
#include "IntervalAstAttribute.h"
#include "IntervalTransferFunctions.h"
#include "IntervalPropertyState.h"
#include "IntervalPropertyStateFactory.h"

using namespace std;
using namespace CodeThorn;

IntervalAnalysis::IntervalAnalysis() {
  _transferFunctions=new IntervalTransferFunctions();
  _initialElementFactory=new IntervalPropertyStateFactory();
}

IntervalAnalysis::~IntervalAnalysis() {
  delete _transferFunctions;
  delete _initialElementFactory;
}

void IntervalAnalysis::initializeExtremalValue(Lattice* element) {
  IntervalPropertyState* pstate=dynamic_cast<IntervalPropertyState*>(element);
  pstate->setEmptyState();
  //iElement->... init to empty state, not being bottom
  cout<<"INFO: initialized extremal value."<<endl;
}

DFAstAttribute* IntervalAnalysis::createDFAstAttribute(Lattice* elem) {
  IntervalPropertyState* ivElem=dynamic_cast<IntervalPropertyState*>(elem);
  ROSE_ASSERT(ivElem);
  return new IntervalAstAttribute(ivElem);
}

void IntervalAnalysis::initializeTransferFunctions() {
   DFAnalysisBase::initializeTransferFunctions();
  _numberIntervalLattice=new NumberIntervalLattice();
  ROSE_ASSERT(_numberIntervalLattice);
  IntervalTransferFunctions* intervalTransferFunctions=dynamic_cast<IntervalTransferFunctions*>(_transferFunctions);
  ROSE_ASSERT(intervalTransferFunctions->getCppExprEvaluator()==0);
  intervalTransferFunctions->setCppExprEvaluator(new CppExprEvaluator(_numberIntervalLattice,&_variableIdMapping));
 }
