#include "sage3basic.h"
#include "Solver19.h"
#include "CTAnalysis.h"
#include "CodeThornCommandLineOptions.h"
#include "EStateTransferFunctions.h"
#include <limits>
#include <unordered_set>

using namespace std;
using namespace CodeThorn;
using namespace Sawyer::Message;

#include "CTAnalysis.h"

Sawyer::Message::Facility Solver19::logger;
bool Solver19::_diagnosticsInitialized = false;

Solver19::Solver19() {
  initDiagnostics();
}

void Solver19::initDiagnostics() {
  if (!_diagnosticsInitialized) {
    _diagnosticsInitialized = true;
    Solver::initDiagnostics(logger, 18);
  }
}

int Solver19::getId() {
  return 18;
}

// allows to handle sequences of nodes as basic blocks
bool Solver19::isPassThroughLabel(Label lab) {
  return _analyzer->isPassThroughLabel(lab);
}

void Solver19::initializeAbstractStatesFromWorkList() {
  cerr<<"Error: solver19 not available."<<endl;
}

void Solver19::run() {
  cerr<<"Error: solver19 not available."<<endl;
}
