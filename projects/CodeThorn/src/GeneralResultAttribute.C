
#include "GeneralResultAttribute.h"

GeneralResultAttribute::GeneralResultAttribute(string info) {
  _info="// "+info;
}

string GeneralResultAttribute::toString() {
  return _info; 
}
