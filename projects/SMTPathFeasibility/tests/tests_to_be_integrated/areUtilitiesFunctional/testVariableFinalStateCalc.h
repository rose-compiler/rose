#ifndef HH_TESTVARIABLEFINALSTATECALC_HH
#define HH_TESTVARIABLEFINALSTATECALC_HH

#include "rose.h"

std::string getVariableFinalState(SgVarRefExp*);
void initDefUseAnalysis(SgProject*);
void initializeScopeInformation(SgProject*);

#endif
