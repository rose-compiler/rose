#include "sage3basic.h"
#include "AbstractValue.h"
#include "EState.h"
#include "Analyzer.h"

#include "RERS_Problem10_mod.C"

namespace RERS_Problem {
  void rersGlobalVarsCallInit(CodeThorn::Analyzer* analyzer, CodeThorn::PState& pstate) {
    COPY_PSTATEVAR_TO_GLOBALVAR(a1);
    COPY_PSTATEVAR_TO_GLOBALVAR(a19);
    COPY_PSTATEVAR_TO_GLOBALVAR(a10);
    COPY_PSTATEVAR_TO_GLOBALVAR(a12);
    COPY_PSTATEVAR_TO_GLOBALVAR(a4);
  }
  void rersGlobalVarsCallReturnInit(CodeThorn::Analyzer* analyzer, CodeThorn::PState& pstate) {
    COPY_GLOBALVAR_TO_PSTATEVAR(a1);
    COPY_GLOBALVAR_TO_PSTATEVAR(a19);
    COPY_GLOBALVAR_TO_PSTATEVAR(a10);
    COPY_GLOBALVAR_TO_PSTATEVAR(a12);
    COPY_GLOBALVAR_TO_PSTATEVAR(a4);
  }
}
