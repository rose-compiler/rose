#include "sage3basic.h"
#include <iostream>
#include "AbstractValue.h"
#include "EState.h"
#include "CTAnalysis.h"
#include "RERS_empty_specialization.h"

namespace RERS_Problem {
  //input and output are pointers to arrays in the parallel version
  int* output;
  int* input; //added because the input variable is now transformed into a global variable
  void __empty_rersGlobalVarsCallInit(CodeThorn::CTAnalysis*, CodeThorn::PState&, int /*thread_id*/) {
    std::cerr<<"Error: rers-binary mode active but empty default implementation selected."<<std::endl;
    exit(1);
  }
  void __empty_rersGlobalVarsCallReturnInit(CodeThorn::CTAnalysis*, CodeThorn::PState&, int /*thread_id*/) {
    std::cerr<<"Error: rers-binary mode active but empty default implementation selected."<<std::endl;
    exit(1);
  }
  void __empty_rersGlobalVarsArrayInit(int /*numberOfThreads*/) {
    std::cerr<<"Error: rers-binary mode active but empty default implementation selected."<<std::endl;
    exit(1);
  }

  void __empty_createGlobalVarAddressMaps(CodeThorn::CTAnalysis*) {
    std::cerr<<"Error: rers-binary mode active but empty default implementation selected."<<std::endl;
    exit(1);
  }
  void __empty_calculate_output(int /*numberOfThreads*/) {
    std::cerr<<"Error: rers-binary mode active but empty default implementation selected."<<std::endl;
    exit(1);
  }

  // in a linked binary the following function pointers should point to the respective implementation
  rersGlobalVarsCallInitFP_Type rersGlobalVarsCallInitFP=RERS_Problem::__empty_rersGlobalVarsCallInit;
  rersGlobalVarsCallReturnInitFP_Type rersGlobalVarsCallReturnInitFP=RERS_Problem::__empty_rersGlobalVarsCallReturnInit;
  rersGlobalVarsArrayInitFP_Type rersGlobalVarsArrayInitFP=RERS_Problem::__empty_rersGlobalVarsArrayInit;
  createGlobalVarAddressMapsFP_Type createGlobalVarAddressMapsFP=RERS_Problem::__empty_createGlobalVarAddressMaps;
  calculate_outputFP_Type calculate_outputFP=RERS_Problem::__empty_calculate_output;

}
