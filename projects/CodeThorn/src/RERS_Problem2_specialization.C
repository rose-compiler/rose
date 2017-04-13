#include "sage3basic.h"
#include "AbstractValue.h"
#include "StateRepresentations.h"
#include "Analyzer.h"

#include "RERS_Problem2_mod.C"

namespace RERS_Problem {
  void rersGlobalVarsCallInit(CodeThorn::Analyzer* analyzer, CodeThorn::PState& pstate) {
    COPY_PSTATEVAR_TO_GLOBALVAR(a25);
    COPY_PSTATEVAR_TO_GLOBALVAR(a11);
    COPY_PSTATEVAR_TO_GLOBALVAR(a28);
    COPY_PSTATEVAR_TO_GLOBALVAR(a19);
    COPY_PSTATEVAR_TO_GLOBALVAR(a21);
    COPY_PSTATEVAR_TO_GLOBALVAR(a17);
  }
  void rersGlobalVarsCallReturnInit(CodeThorn::Analyzer* analyzer, CodeThorn::PState& pstate) {
    COPY_GLOBALVAR_TO_PSTATEVAR(a25);
    COPY_GLOBALVAR_TO_PSTATEVAR(a11);
    COPY_GLOBALVAR_TO_PSTATEVAR(a28);
    COPY_GLOBALVAR_TO_PSTATEVAR(a19);
    COPY_GLOBALVAR_TO_PSTATEVAR(a21);
    COPY_GLOBALVAR_TO_PSTATEVAR(a17);
  }
}
