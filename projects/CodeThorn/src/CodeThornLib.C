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
  EStateTransferFunctions::initDiagnostics();
}

Sawyer::Message::Facility CodeThorn::logger;

void CodeThorn::turnOffRoseWarnings() {
  string turnOffRoseWarnings=string("Rose(none,>=error),Rose::EditDistance(none,>=error),Rose::FixupAstDeclarationScope(none,>=error),")
    +"Rose::FixupAstSymbolTablesToSupportAliasedSymbols(none,>=error),"
    +"Rose::EditDistance(none,>=error),"
    +"Rose::TestChildPointersInMemoryPool(none,>=error),Rose::UnparseLanguageIndependentConstructs(none,>=error),"
    +"rose_ir_node(none,>=error)";
  // result string must be checked
  string result=Rose::Diagnostics::mfacilities.control(turnOffRoseWarnings); 
  if(result!="") {
    cerr<<result<<endl;
    cerr<<"Error in logger initialization."<<endl;
    exit(1);
  }
  
  // see class Options in src/roseSupport/utility_functions.h
  Rose::global_options.set_frontend_notes(false);
  Rose::global_options.set_frontend_warnings(false);
  Rose::global_options.set_backend_warnings(false);
}
