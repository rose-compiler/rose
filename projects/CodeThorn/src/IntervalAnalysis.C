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

DFAstAttribute* IntervalAnalysis::createDFAstAttribute(IntervalPropertyState* elem) {
  return new IntervalAstAttribute(elem);
}

void IntervalAnalysis::initializeTransferFunctions() {
  cerr<<"P1"<<endl;
  DFAnalysis2::initializeTransferFunctions();
  ROSE_ASSERT(_numberIntervalLattice);
  ROSE_ASSERT(dynamic_cast<IntervalTransferFunctions*>(_transferFunctions));
  cerr<<"P2"<<endl;
  ROSE_ASSERT(dynamic_cast<IntervalTransferFunctions*>(_transferFunctions)->_cppExprEvaluator==0);
  dynamic_cast<IntervalTransferFunctions*>(_transferFunctions)->_cppExprEvaluator=new CppExprEvaluator(_numberIntervalLattice,0,&_variableIdMapping);
  cerr<<"P3"<<endl;
}
