
#include "sage3basic.h"

#include "Labeler.h"
#include "VariableIdMapping.h"

#include "MemPropertyState.h"
#include "MemPropertyStateFactory.h"

using namespace std;
using namespace CodeThorn;

CodeThorn::MemPropertyStateFactory::MemPropertyStateFactory() {
}

CodeThorn::PropertyState* CodeThorn::MemPropertyStateFactory::create() {
  MemLattice* element=new MemLattice();
  return element;
}

CodeThorn::MemPropertyStateFactory::~MemPropertyStateFactory() {
}
