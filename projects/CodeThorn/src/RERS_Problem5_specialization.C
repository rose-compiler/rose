#include "sage3basic.h"
#include "AbstractValue.h"
#include "StateRepresentations.h"
#include "Analyzer.h"

#include "RERS_Problem5_mod.C"

namespace RERS_Problem {
  void rersGlobalVarsCallInit(CodeThorn::Analyzer* analyzer, CodeThorn::PState& pstate) {
    COPY_PSTATEVAR_TO_GLOBALVAR(a9);
    COPY_PSTATEVAR_TO_GLOBALVAR(a27);
    COPY_PSTATEVAR_TO_GLOBALVAR(a2);
    COPY_PSTATEVAR_TO_GLOBALVAR(a16);
    COPY_PSTATEVAR_TO_GLOBALVAR(a15);
    COPY_PSTATEVAR_TO_GLOBALVAR(a12);
    COPY_PSTATEVAR_TO_GLOBALVAR(a25);
    COPY_PSTATEVAR_TO_GLOBALVAR(a20);
  }

  void rersGlobalVarsCallReturnInit(CodeThorn::Analyzer* analyzer, CodeThorn::PState& pstate) {
    COPY_GLOBALVAR_TO_PSTATEVAR(a9);
    COPY_GLOBALVAR_TO_PSTATEVAR(a27);
    COPY_GLOBALVAR_TO_PSTATEVAR(a2);
    COPY_GLOBALVAR_TO_PSTATEVAR(a16);
    COPY_GLOBALVAR_TO_PSTATEVAR(a15);
    COPY_GLOBALVAR_TO_PSTATEVAR(a12);
    COPY_GLOBALVAR_TO_PSTATEVAR(a25);
    COPY_GLOBALVAR_TO_PSTATEVAR(a20);
  }
}
