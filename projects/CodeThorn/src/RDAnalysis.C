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
using namespace CodeThorn;

CodeThorn::RDPropertyStateFactory::RDPropertyStateFactory() {
}

CodeThorn::RDLattice* CodeThorn::RDPropertyStateFactory::create() {
  RDLattice* element=new RDLattice();
  return element;
}

CodeThorn::RDPropertyStateFactory::~RDPropertyStateFactory() {
}

CodeThorn::RDAnalysis::RDAnalysis() {
  _transferFunctions=new RDTransferFunctions();
  _transferFunctions->setInitialElementFactory(new RDPropertyStateFactory());
}

CodeThorn::RDAnalysis::~RDAnalysis() {
  delete _transferFunctions;
  delete _transferFunctions->getInitialElementFactory();
}

void CodeThorn::RDAnalysis::initialize(SgProject* root) {
  DFAnalysisBase::initialize(root, nullptr /* no existing abstraction layer */);
}

void CodeThorn::RDAnalysis::initializeExtremalValue(Lattice* element) {
  _transferFunctions->initializeExtremalValue(*element);
  cout<<"INFO: initialized extremal value."<<endl;
}

CodeThorn::DFAstAttribute* CodeThorn::RDAnalysis::createDFAstAttribute(Lattice* elem) {
  RDLattice* rdElem=dynamic_cast<RDLattice*>(elem);
  ROSE_ASSERT(rdElem);
  return new RDAstAttribute(rdElem);
}

CodeThorn::LabelSet CodeThorn::RDAnalysis::getPreRDs(Label lab, VariableId varId) {
  CodeThorn::RDLattice* rdInfo=dynamic_cast<RDLattice*>(getPreInfo(lab));
  ROSE_ASSERT(rdInfo);
  return rdInfo->getRDs(varId);
}

CodeThorn::LabelSet CodeThorn::RDAnalysis::getPostRDs(Label lab, VariableId varId) {
  CodeThorn::RDLattice* rdInfo=dynamic_cast<RDLattice*>(getPostInfo(lab));
  ROSE_ASSERT(rdInfo);
  return rdInfo->getRDs(varId);
}
