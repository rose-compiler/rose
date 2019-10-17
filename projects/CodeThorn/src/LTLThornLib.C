#include "sage3basic.h"
#include "IOAnalyzer.h"
#include "ReadWriteAnalyzer.h"
#include "CounterexampleGenerator.h"
#include "LTLThornLib.h"

void CodeThorn::initDiagnosticsLTL() {
  IOAnalyzer::initDiagnostics();
  ReadWriteAnalyzer::initDiagnostics();
  CounterexampleGenerator::initDiagnostics();
}
