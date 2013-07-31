#include "AType.h"
#include "StateRepresentations.h"
#include "Analyzer.h"

namespace RERS_Problem {
  void rersGlobalVarsCallInit(CodeThorn::Analyzer* analyzer, CodeThorn::PState& pstate) {
    cerr<<"Error: rers-binary mode active but empty default implementation selected."<<endl;
    exit(1);
  }
  void rersGlobalVarsCallReturnInit(CodeThorn::Analyzer* analyzer, CodeThorn::PState& pstate) {
    cerr<<"Error: rers-binary mode active but empty default implementation selected."<<endl;
    exit(1);
  }
  int calculate_output(int) { 
    cerr<<"Error: rers-binary mode active but empty default implementation selected."<<endl;
    exit(1);
  }
}
