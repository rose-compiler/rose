#include "sage3basic.h"
#include "ParProEState.h"
#include "boost/regex.hpp"

using namespace std;
using namespace CodeThorn;

/*! 
  * \author Marc Jasper
  * \date 2016.
 */
// define order for ParProEState elements (necessary for ParProEStateSet)
bool CodeThorn::operator<(const ParProEState& e1, const ParProEState& e2) {
  return e1.getLabel()<e2.getLabel();
}

bool CodeThorn::operator==(const ParProEState& e1, const ParProEState& e2) {
  return e1.getLabel()==e2.getLabel();
}

bool CodeThorn::operator!=(const ParProEState& e1, const ParProEState& e2) {
  return !(e1==e2);
}

string ParProEState::toString() const {
  ParProLabel label = getLabel();
  bool firstEntry = true;
  stringstream ss;
  ss << "";
  for (ParProLabel::iterator i=label.begin(); i!=label.end(); i++) {
    if (!firstEntry) {
      ss << ",";
    }
    firstEntry = false;
    ss << *i;
  }
  ss << "";
  return ss.str();
}
