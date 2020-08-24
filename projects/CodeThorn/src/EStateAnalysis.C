// Author: Markus Schordan, 2013.

#include "sage3basic.h"

#include "Labeler.h"
#include "VariableIdMappingExtended.h"

#include "Lattice.h"
#include "PropertyState.h"

#include "EStateAnalysis.h"
#include "CollectionOperators.h"
#include "EStateTransferFunctions.h"


using namespace std;
using namespace CodeThorn;

CodeThorn::EStateAnalysis::EStateAnalysis() {
  _transferFunctions=new EStateTransferFunctions();
  _transferFunctions->setInitialElementFactory(new EStateFactory());
}

CodeThorn::EStateAnalysis::~EStateAnalysis() {
  delete _transferFunctions;
  delete _transferFunctions->getInitialElementFactory();
}

void CodeThorn::EStateAnalysis::initialize(SgProject* root) {
  ProgramAbstractionLayer* pal=new ProgramAbstractionLayer();
  pal->initialize(root);
  DFAnalysisBase::initialize(root, pal);
}

void CodeThorn::EStateAnalysis::initializeExtremalValue(Lattice* element) {
  _transferFunctions->initializeExtremalValue(*element);
  cout<<"INFO: initialized extremal value."<<endl;
}
