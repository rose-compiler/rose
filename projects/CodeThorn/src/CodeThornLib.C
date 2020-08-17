#include "sage3basic.h"
#include "Analyzer.h"
#include "RewriteSystem.h"
#include "Specialization.h"
#include "Normalization.h"
#include "FunctionIdMapping.h"
#include "FunctionCallMapping.h"
#include "Diagnostics.h"
#include "CodeThornLib.h"
#include "LTLThornLib.h"

// handler
#include <signal.h>
#include <execinfo.h>

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

// handler for generating backtrace
void handler(int sig) {
  void *array[10];
  size_t size;

  size = backtrace (array, 10);
  printf ("Obtained %zd stack frames.\n", size);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

void CodeThorn::configureRose() {
  CodeThorn::initDiagnostics();

  Rose::Diagnostics::mprefix->showProgramName(false);
  Rose::Diagnostics::mprefix->showThreadId(false);
  Rose::Diagnostics::mprefix->showElapsedTime(false);

  turnOffRoseWarnings();
  signal(SIGSEGV, handler);   // install handler for backtrace
}

