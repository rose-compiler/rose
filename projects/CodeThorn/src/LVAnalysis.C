// Author: Markus Schordan, 2013.

#include "sage3basic.h"

#include "Lattice.h"
#include "PropertyState.h"

#include "LVAnalysis.h"
#include "LVAstAttribute.h"
#include "CollectionOperators.h"

#include "Labeler.h"
#include "VariableIdMapping.h"

using namespace std;
using namespace CodeThorn;

CodeThorn::LVPropertyStateFactory::LVPropertyStateFactory() {
}

CodeThorn::PropertyState* CodeThorn::LVPropertyStateFactory::create() {
  LVLattice* element=new LVLattice();
  return element;
}

CodeThorn::LVPropertyStateFactory::~LVPropertyStateFactory() {
}

CodeThorn::LVAnalysis::LVAnalysis() {
  _transferFunctions=new LVTransferFunctions();
  _initialElementFactory=new LVPropertyStateFactory();
}

CodeThorn::LVAnalysis::~LVAnalysis() {
  delete _transferFunctions;
  delete _initialElementFactory;
}
void CodeThorn::LVAnalysis::initializeExtremalValue(Lattice* element) {
  LVLattice* lvElement=dynamic_cast<LVLattice*>(element);
  ROSE_ASSERT(lvElement);
  lvElement->setEmptySet();
  cout<<"INFO: initialized extremal value."<<endl;
}

DFAstAttribute* CodeThorn::LVAnalysis::createDFAstAttribute(Lattice* elem) {
  LVLattice* lvElem=dynamic_cast<LVLattice*>(elem);
  ROSE_ASSERT(lvElem);
  return new CodeThorn::LVAstAttribute(lvElem);
}
