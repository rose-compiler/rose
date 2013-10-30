#include "sage3basic.h"
#include "DFAstAttribute.h"

// default behavior
string DFAstAttribute::toString() { return "undefined";}

DFAstAttribute::DFAstAttribute():_dfiSelector(DFISEL_DEFAULT) {
}

DFAstAttribute::~DFAstAttribute() {
  // virtual destructor intentionally empty
}

void DFAstAttribute::setDFInfoSelector(DFInfoSelector flowInfoSelector) {
  _dfiSelector=flowInfoSelector;
}

DFInfoSelector DFAstAttribute::getDFInfoSelector() {
  return _dfiSelector;
}

