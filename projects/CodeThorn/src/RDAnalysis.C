// Author: Markus Schordan, 2013.

#include "sage3basic.h"

#include "Lattice.h"
#include "PropertyState.h"

#include "RDAnalysis.h"
#include "RDAstAttribute.h"
#include "CollectionOperators.h"
#include "RDTransferFunctions.h"

#include "Labeler.h"
#include "VariableIdMapping.h"

using namespace std;
using namespace SPRAY;

SPRAY::RDPropertyStateFactory::RDPropertyStateFactory() {
}

SPRAY::PropertyState* SPRAY::RDPropertyStateFactory::create() {
  RDLattice* element=new RDLattice();
  return element;
}

SPRAY::RDPropertyStateFactory::~RDPropertyStateFactory() {
}

SPRAY::RDAnalysis::RDAnalysis() {
  _transferFunctions=new RDTransferFunctions();
  _initialElementFactory=new RDPropertyStateFactory();
}

SPRAY::RDAnalysis::~RDAnalysis() {
  delete _transferFunctions;
  delete _initialElementFactory;
}
void SPRAY::RDAnalysis::initializeExtremalValue(Lattice* element) {
  RDLattice* rdElement=dynamic_cast<RDLattice*>(element);
  rdElement->setEmptySet();
  cout<<"INFO: initialized extremal value."<<endl;
}

SPRAY::DFAstAttribute* SPRAY::RDAnalysis::createDFAstAttribute(Lattice* elem) {
  RDLattice* rdElem=dynamic_cast<RDLattice*>(elem);
  ROSE_ASSERT(rdElem);
  return new RDAstAttribute(rdElem);
}
