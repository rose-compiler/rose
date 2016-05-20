#include "sage3basic.h"
#include <iostream>
#include "AType.h"
#include "StateRepresentations.h"
#include "Analyzer.h"

namespace RERS_Problem {
  //input and output are pointer to arrays in the parallel version 
  int* output;
  int* input; //added because the input variable is now transformed into a global variabal
  void rersGlobalVarsCallInit(CodeThorn::Analyzer* analyzer, CodeThorn::PState& pstate, int thread_id) {
    std::cerr<<"Error: rers-binary mode active but empty default implementation selected."<<endl;
    exit(1);
  }
  void rersGlobalVarsCallReturnInit(CodeThorn::Analyzer* analyzer, CodeThorn::PState& pstate, int thread_id) {
    std::cerr<<"Error: rers-binary mode active but empty default implementation selected."<<endl;
    exit(1);
  }
#if 0
  // RERS 2012 format
  int calculate_output(int) { 
    std::cerr<<"Error: rers-binary mode active but empty default implementation selected."<<endl;
    exit(1);
  }
  /// deprecated, input variable passed in as a parameter instead of being copied like other globals
  // RERS 2013 format
  void calculate_output(int) { 
    std::cerr<<"Error: rers-binary mode active but empty default implementation selected."<<endl;
    exit(1);
  }
#else
  // RERS 2013 format
  void calculate_output(int numberOfThreads) { 
    std::cerr<<"Error: rers-binary mode active but empty default implementation selected."<<endl;
    exit(1);
  }
#endif
  void rersGlobalVarsArrayInit(int numberOfThreads) {
    std::cerr<<"Error: rers-binary mode active but empty default implementation selected."<<endl;
    exit(1);
  }

  void createGlobalVarAddressMaps(CodeThorn::Analyzer* analyzer) {
    std::cerr<<"Error: rers-binary mode active but empty default implementation selected."<<endl;
    exit(1);
  }
}
