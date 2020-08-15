
#include "sage3basic.h"

#include "Labeler.h"
#include "VariableIdMapping.h"

#include "EState.h"
#include "EStateFactory.h"

using namespace std;
using namespace CodeThorn;

CodeThorn::EStateFactory::EStateFactory() {
}

CodeThorn::EState* CodeThorn::EStateFactory::create() {
  EState* element=new EState();
  return element;
}

CodeThorn::EStateFactory::~EStateFactory() {
}
