// Author: Markus Schordan, 2013.

#include "sage3basic.h"

#include "Labeler.h"
#include "VariableIdMappingExtended.h"

#include "Lattice.h"
#include "PropertyState.h"

#include "MemAnalysis.h"
#include "CollectionOperators.h"
#include "MemTransferFunctions.h"


using namespace std;
using namespace CodeThorn;

CodeThorn::MemAnalysis::MemAnalysis() {
  _transferFunctions=new MemTransferFunctions();
  _transferFunctions->setInitialElementFactory(new MemPropertyStateFactory());
}

CodeThorn::MemAnalysis::~MemAnalysis() {
  delete _transferFunctions;
  delete _transferFunctions->getInitialElementFactory();
}

void CodeThorn::MemAnalysis::initialize(SgProject* root) {
  ProgramAbstractionLayer* pal=new ProgramAbstractionLayer();
  pal->initialize(root);
  DFAnalysisBase::initialize(root, pal);
}

void CodeThorn::MemAnalysis::initializeExtremalValue(Lattice* element) {
  _transferFunctions->initializeExtremalValue(*element);
  cout<<"INFO: initialized extremal value."<<endl;
}
