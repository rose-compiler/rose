#include "sage3basic.h"
#include <iostream>
#include "AType.h"
#include "StateRepresentations.h"
#include "Analyzer.h"

namespace RERS_Problem {
  void rersGlobalVarsCallInit(CodeThorn::Analyzer* analyzer, CodeThorn::PState& pstate) {
    std::cerr<<"Error: rers-binary mode active but empty default implementation selected."<<endl;
    exit(1);
  }
  void rersGlobalVarsCallReturnInit(CodeThorn::Analyzer* analyzer, CodeThorn::PState& pstate) {
    std::cerr<<"Error: rers-binary mode active but empty default implementation selected."<<endl;
    exit(1);
  }
  int calculate_output(int) { 
    std::cerr<<"Error: rers-binary mode active but empty default implementation selected."<<endl;
    exit(1);
  }
}
