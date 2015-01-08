#include "sage3basic.h"
#include "DFAstAttribute.h"

using namespace SPRAY;

// default behavior
string DFAstAttribute::toString() { return "undefined";}
void DFAstAttribute::toStream(std::ostream& os,VariableIdMapping*) { os<<"undefined";}

DFAstAttribute::DFAstAttribute() {
}

DFAstAttribute::~DFAstAttribute() {
  // virtual destructor intentionally empty
}
