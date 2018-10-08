#include "sage3basic.h"
#include "IOSolver.h"
#include "IOAnalyzer.h"

using namespace CodeThorn;
using namespace std;

IOSolver::IOSolver() {}

IOSolver::IOSolver(Analyzer* analyzer) {
  setAnalyzer(analyzer);
}

void IOSolver::setAnalyzer(Analyzer* analyzer) {
  Solver::setAnalyzer(analyzer);
  if(IOAnalyzer* iOAnalyzer = dynamic_cast<IOAnalyzer*>(analyzer)) {
    _analyzer = iOAnalyzer;
  } else {
    throw CodeThorn::Exception("The analyzer of IOSolver must be an IOAnalyzer.");
  }
}
