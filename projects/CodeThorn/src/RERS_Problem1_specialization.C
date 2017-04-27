#include "sage3basic.h"
#include "AbstractValue.h"
#include "EState.h"
#include "Analyzer.h"

#include "RERS_Problem1_mod.C"

namespace RERS_Problem {
  void rersGlobalVarsCallInit(CodeThorn::Analyzer* analyzer, CodeThorn::PState& pstate) {
    COPY_PSTATEVAR_TO_GLOBALVAR(a17);
    COPY_PSTATEVAR_TO_GLOBALVAR(a7);
    COPY_PSTATEVAR_TO_GLOBALVAR(a20);
    COPY_PSTATEVAR_TO_GLOBALVAR(a8);
    COPY_PSTATEVAR_TO_GLOBALVAR(a16);
    COPY_PSTATEVAR_TO_GLOBALVAR(a21);
  }
  void rersGlobalVarsCallReturnInit(CodeThorn::Analyzer* analyzer, CodeThorn::PState& pstate) {
    COPY_GLOBALVAR_TO_PSTATEVAR(a17);
    COPY_GLOBALVAR_TO_PSTATEVAR(a7);
    COPY_GLOBALVAR_TO_PSTATEVAR(a20);
    COPY_GLOBALVAR_TO_PSTATEVAR(a8);
    COPY_GLOBALVAR_TO_PSTATEVAR(a16);
    COPY_GLOBALVAR_TO_PSTATEVAR(a21);
  }
}
