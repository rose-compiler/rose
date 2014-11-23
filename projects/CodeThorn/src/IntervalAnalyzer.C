// Author: Markus Schordan, 2013.

#include "sage3basic.h"
#include "CollectionOperators.h"
#include "IntervalAnalyzer.h"
#include "IntervalAstAttribute.h"
#include "IntervalTransferFunctions.hpp"

using namespace std;
using namespace CodeThorn;

IntervalAnalyzer::IntervalAnalyzer() {
  _transferFunctions=new IntervalTransferFunctions();
}

IntervalAnalyzer::~IntervalAnalyzer() {
  delete _transferFunctions;
}

DFAstAttribute* IntervalAnalyzer::createDFAstAttribute(IntervalLattice* elem) {
  return new IntervalAstAttribute(elem);
}
