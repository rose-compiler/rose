#include "sage3basic.h"
#include "Solver.h"

using namespace CodeThorn;
using namespace std;

Solver::Solver() {
  _analyzer = nullptr;
}

Solver::Solver(Analyzer* analyzer) {
  setAnalyzer(analyzer);
}

void Solver::setAnalyzer(Analyzer* analyzer) {
  _analyzer = analyzer;
}

void Solver::initDiagnostics(Sawyer::Message::Facility& logger, int solverId) {
  stringstream dynamicClassName;
  dynamicClassName << "CodeThorn::Solver" << solverId;
  logger = Sawyer::Message::Facility(dynamicClassName.str(), Rose::Diagnostics::destination);
  Rose::Diagnostics::mfacilities.insertAndAdjust(logger);
}
