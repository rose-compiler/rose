#include "sage3basic.h"
#include "AbstractValue.h"
#include "EState.h"
#include "Analyzer.h"

#include "RERS_Problem4_mod.C"

namespace RERS_Problem {
  void rersGlobalVarsCallInit(CodeThorn::Analyzer* analyzer, CodeThorn::PState& pstate) {
    COPY_PSTATEVAR_TO_GLOBALVAR(a3);
    COPY_PSTATEVAR_TO_GLOBALVAR(a4);
    COPY_PSTATEVAR_TO_GLOBALVAR(a25);
    COPY_PSTATEVAR_TO_GLOBALVAR(a1);
    COPY_PSTATEVAR_TO_GLOBALVAR(a26);
    COPY_PSTATEVAR_TO_GLOBALVAR(a28);
    COPY_PSTATEVAR_TO_GLOBALVAR(a7);
    COPY_PSTATEVAR_TO_GLOBALVAR(a14);
  }

  void rersGlobalVarsCallReturnInit(CodeThorn::Analyzer* analyzer, CodeThorn::PState& pstate) {
    COPY_GLOBALVAR_TO_PSTATEVAR(a3);
    COPY_GLOBALVAR_TO_PSTATEVAR(a4);
    COPY_GLOBALVAR_TO_PSTATEVAR(a25);
    COPY_GLOBALVAR_TO_PSTATEVAR(a1);
    COPY_GLOBALVAR_TO_PSTATEVAR(a26);
    COPY_GLOBALVAR_TO_PSTATEVAR(a28);
    COPY_GLOBALVAR_TO_PSTATEVAR(a7);
    COPY_GLOBALVAR_TO_PSTATEVAR(a14);
  }
}
