#ifndef RERS_EMPTY_SPECIALIZATION
#define RERS_EMPTY_SPECIALIZATION

#include "Analyzer.h"
#include "PState.h"

namespace RERS_Problem {
  typedef void (*rersGlobalVarsCallInitFP_Type)(CodeThorn::Analyzer*,CodeThorn::PState&,int);
  typedef void (*rersGlobalVarsCallReturnInitFP_Type)(CodeThorn::Analyzer* analyzer, CodeThorn::PState& pstate, int thread_id);
  typedef void (*rersGlobalVarsArrayInitFP_Type)(int numberOfThreads);
  typedef void (*createGlobalVarAddressMapsFP_Type)(CodeThorn::Analyzer* analyzer);
  typedef void (*calculate_outputFP_Type)(int numberOfThreads);
  extern rersGlobalVarsCallInitFP_Type rersGlobalVarsCallInitFP;
  extern rersGlobalVarsCallReturnInitFP_Type rersGlobalVarsCallReturnInitFP;
  extern rersGlobalVarsArrayInitFP_Type rersGlobalVarsArrayInitFP;
  extern createGlobalVarAddressMapsFP_Type createGlobalVarAddressMapsFP;
  extern calculate_outputFP_Type calculate_outputFP;
}

#endif
