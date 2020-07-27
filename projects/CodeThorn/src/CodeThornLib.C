#include "sage3basic.h"
#include "Analyzer.h"
#include "RewriteSystem.h"
#include "Specialization.h"
#include "Normalization.h"
#include "FunctionIdMapping.h"
#include "FunctionCallMapping.h"
#include "Diagnostics.h"
#include "CodeThornLib.h"

using namespace Sawyer::Message;

void CodeThorn::initDiagnostics() {
  Rose::Diagnostics::initialize();
  // general logger for CodeThorn library functions
  Rose::Diagnostics::initAndRegister(&CodeThorn::logger, "CodeThorn");
  // class specific loggers for CodeThorn library functions
  Analyzer::initDiagnostics();
  RewriteSystem::initDiagnostics();
  Specialization::initDiagnostics();
  Normalization::initDiagnostics();
  FunctionIdMapping::initDiagnostics();
  FunctionCallMapping::initDiagnostics();
}

Sawyer::Message::Facility CodeThorn::logger;
