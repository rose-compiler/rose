#include "sage3basic.h"
#include "DFAstAttribute.h"

// default behavior
string DFAstAttribute::getPreInfoString() { return "pre-info: undefined";}
string DFAstAttribute::getPostInfoString() { return "post-info: undefined";}

DFAstAttribute::~DFAstAttribute() {
  // virtual destructor intentionally empty
}

