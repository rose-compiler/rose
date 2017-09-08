#include "sage3basic.h"
#include "SvcompWitness.h"
#include "Diagnostics.h"

using namespace CodeThorn;
using namespace std;
using namespace Sawyer::Message;

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
SvcompWitness 
SvcompWitness::onlyStatesSatisfying(std::function<bool(const EState*)> predicate) const {
  return onlyStatesSatisfyingTemplate<SvcompWitness>(predicate);
}
