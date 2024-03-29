#include "sage3basic.h"
#include "Solver.h"

using namespace CodeThorn;
using namespace std;

Solver::Solver() {
  _analyzer = nullptr;
}

Solver::Solver(CTAnalysis* analyzer) {
  setAnalyzer(analyzer);
}

Solver::~Solver() {
}

void Solver::setAnalyzer(CTAnalysis* analyzer) {
  _analyzer = analyzer;
}

void Solver::initDiagnostics(Sawyer::Message::Facility& logger, int solverId) {
  stringstream dynamicClassName;
  dynamicClassName << "CodeThorn::Solver" << solverId;
  logger = Sawyer::Message::Facility(dynamicClassName.str(), Rose::Diagnostics::destination);
  Rose::Diagnostics::mfacilities.insertAndAdjust(logger);
}

bool Solver::createsTransitionSystem() {
  return false;
}

bool Solver::checksAssertions() {
  return false;
}
