#include "sage3basic.h"
#include "SvcompWitness.h"
#include "Rose/Diagnostics.h"

using namespace CodeThorn;
using namespace std;
using namespace Sawyer::Message;

/*!
 * \author Marc Jasper
 * \date 2017.
 */
SvcompWitness
SvcompWitness::onlyStatesSatisfying(std::function<bool(EStatePtr)> predicate) const {
  return onlyStatesSatisfyingTemplate<SvcompWitness>(predicate);
}

void SvcompWitness::writeErrorAutomatonToFile(string /*filename*/) {
    //TODO: implement function that writes
    //      SV-COMP error-witness automaton to file 'filename'
    cerr << "Warning: Option \"--witness-file\" is not supported yet and will be ignored." << endl;
}
