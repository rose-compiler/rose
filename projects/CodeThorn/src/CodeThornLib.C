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
using namespace CodeThorn;

void CodeThorn::initDiagnostics() {
  Rose::Diagnostics::initialize();
  Analyzer::initDiagnostics();
  RewriteSystem::initDiagnostics();
  Specialization::initDiagnostics();
  Normalization::initDiagnostics();
  FunctionIdMapping::initDiagnostics();
  FunctionCallMapping::initDiagnostics();

}
