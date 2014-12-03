// Author: Markus Schordan, 2013.

#include "sage3basic.h"

#include "RDAnalyzer.h"
#include "RDAstAttribute.h"
#include "CollectionOperators.h"
#include "RDTransferFunctions.hpp"

using namespace std;
using namespace CodeThorn;

RDAnalyzer::RDAnalyzer() {
  _transferFunctions=new RDTransferFunctions();
}

RDAnalyzer::~RDAnalyzer() {
  delete _transferFunctions;
}

DFAstAttribute* RDAnalyzer::createDFAstAttribute(RDLattice* elem) {
  return new RDAstAttribute(elem);
}
